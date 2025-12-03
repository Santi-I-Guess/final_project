#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "../common_values.h"
#include "helper.h"
#include "synthesis.h"

std::vector<int16_t> assemble_program(
        const std::vector<Token> tokens,
        const std::map<std::string, int16_t> label_map
) {
        // Step 1: calculate main address offset, store string indexes and addresses
        std::vector<int16_t> program = {};
        std::vector<int16_t> string_elements = {};
        std::map<int16_t, int16_t> string_addrs = {}; /* {index, address} */
        int16_t main_addr_offset = 5; // 4 magic numbers + main addr itself
        int16_t num_strings = 0;
        for (Token i : tokens) {
                if (i.type != T_STRING_LIT)
                        continue;
                i.data = i.data.substr(1, i.data.length() - 2); // strip quotes
                std::vector<int16_t> translated_string = translate_string(i.data);
                std::vector<int16_t>::iterator first, second;
                first = translated_string.begin();
                second = translated_string.end();
                string_elements.insert(string_elements.end(), first, second);
                string_addrs.insert({num_strings, main_addr_offset});
                main_addr_offset += (int16_t)translated_string.size();
                num_strings++;
        }

        // Step 2: add entry offset and magic number to beginning of program
        program.push_back((int16_t)(0x4153)); // SA
        program.push_back((int16_t)(0x544e)); // NT
        program.push_back((int16_t)(0x4149)); // IA
        program.push_back((int16_t)(0x4f47)); // GO
        // ensure 0 element buffer between entry addr and first opcode
        if (num_strings == 0)
                main_addr_offset++;
        // increase offset for 0xffff element after strings get parsed
        main_addr_offset++;
        program.push_back(label_map.at("main") + main_addr_offset);

        // Step 3: add string literal data to program
        if (num_strings > 0) {
                std::vector<int16_t>::iterator first, second;
                first = string_elements.begin();
                second = string_elements.end();
                program.insert(program.end(), first, second);
        } else {
                // add zero buffer if not included
                program.push_back((int16_t)0x0000);
        }
        program.push_back((int16_t)0xffff);

        // Step 4: translate normal instructions and arguments
        int16_t num_seen_strs = 0;
        size_t token_idx = 0;
        while (token_idx < tokens.size()) {
                Token first_token = tokens.at(token_idx);
                std::vector<Atom_Type> curr_blueprint;
                curr_blueprint = BLUEPRINTS.at(first_token.data);
                for (size_t ins_idx = 0; ins_idx < curr_blueprint.size(); ins_idx++) {
                        Token curr_token = tokens.at(token_idx + ins_idx);
                        int16_t translated = 0;
                        std::stringstream aux_stream;
                        std::string stripped_token = "";
                        if (curr_token.data.length() >= 2)
                                stripped_token = curr_token.data.substr(1, curr_token.data.length() - 1);
                        switch (curr_token.type) {
                        case T_INTEGER_LIT:
                                aux_stream << stripped_token;
                                aux_stream >> translated;
                                translated |= (int16_t)(1 << 14); // addressing mode bitmask
                                break;
                        case T_LABEL_DEF:
                                break;
                        case T_LABEL_REF:
                                translated = label_map.at(curr_token.data) + main_addr_offset;
                                break;
                        case T_MNEMONIC:
                                translated = OPCODE_TABLE.at(curr_token.data);
                                break;
                        case T_REGISTER:
                                translated = REGISTER_TABLE.at(curr_token.data);
                                break;
                        case T_STACK_OFF:
                                aux_stream << stripped_token;
                                aux_stream >> translated;
                                translated |= (int16_t)(1 << 13); // addressing mode bitmask
                                break;
                        case T_STRING_LIT:
                                translated = string_addrs.at(num_seen_strs);
                                translated |= (int16_t)(1 << 12); // addressing mode bitmask
                                break;
                        default: /* impossible */
                                break;
                        }
                        program.push_back(translated);
                        if (curr_blueprint.at(ins_idx) == LITERAL_STR)
                                num_seen_strs++;
                }
                token_idx += curr_blueprint.size();
        }

        return program;
}

std::map<std::string, int16_t> create_label_map(
        const std::vector<Token> tokens
) {
        std::map<std::string, int16_t> label_map;
        // declarations themselves aren't translated into the binary program,
        //      so they need to be accounted for when calculating addresses
        int16_t num_seen_declarations= 0;
        for (int16_t prog_addr = 0; prog_addr < (int16_t)tokens.size(); ++prog_addr) {
                Token curr_token = tokens.at(prog_addr);
                if (curr_token.type != T_LABEL_DEF)
                        continue;
                std::string label_name = curr_token.data;
                // remove colon
                label_name = label_name.substr(0, label_name.length()-1);
                label_map.insert({label_name, prog_addr - num_seen_declarations});
                num_seen_declarations++;
        }
        return label_map;
}

std::vector<Token> create_tokens(const std::string source_buffer) {
        std::vector<Token> tokens = {};
        size_t buff_idx = 0;
        size_t buff_len = source_buffer.size();
        int line_num = 1;
        // line_num should increase only if it's in the same step that
        //      mutates buff_idx or token_idx
        next_lexeme:
        while (buff_idx < buff_len) {
                char curr = source_buffer[buff_idx];
                // increase buff_idx until not on whitespace
                while (buff_idx < buff_len) {
                        curr = source_buffer[buff_idx];
                        if (isspace(curr)) {
                                if (curr == '\n')
                                        line_num++;
                                buff_idx++;
                        } else {
                                break;
                        }
                }
                if (buff_idx == buff_len)
                        return tokens;
                // increase buff_idx until not on a comment
                if (curr == ';') {
                        while (buff_idx < buff_len) {
                                curr = source_buffer[buff_idx];
                                buff_idx++;
                                if (curr == '\n') {
                                        line_num++;
                                        break;
                                }
                        }
                        if (buff_idx == buff_len)
                                return tokens;
                        goto next_lexeme;
                }
                size_t token_len = 0;
                if (curr == '\"') {
                        // if token is a string
                        token_len++;
                        bool skip_quote = false;
                        while (((buff_idx + token_len) < buff_len)) {
                                curr = source_buffer[buff_idx + token_len];
                                if (curr == '\n')
                                        break;
                                token_len++;
                                if (curr == '\"' && !skip_quote)
                                        break;
                                skip_quote = false;
                                // to allow for escaped quotes
                                if (curr == '\\')
                                        skip_quote = true;
                        }
                        if (buff_idx == buff_len)
                                break;
                        Token curr_token;
                        curr_token.data = source_buffer.substr(buff_idx, token_len);
                        curr_token.type = T_STRING_LIT;
                        curr_token.line_num = line_num;
                        tokens.push_back(curr_token);
                        buff_idx += token_len;
                } else if (is_identifier_char(source_buffer[buff_idx])) {
                        // if token is normal
                        token_len++;
                        while (((buff_idx + token_len) < buff_len) && (curr != '\n')) {
                                curr = source_buffer[buff_idx + token_len];
                                if (is_identifier_char(curr))
                                        token_len++;
                                else
                                        break;
                        }
                        if (buff_idx == buff_len)
                                break;
                        Token curr_token;
                        curr_token.data = source_buffer.substr(buff_idx, token_len);
                        curr_token.type = T_MNEMONIC; // default type
                        curr_token.line_num = line_num;
                        for (size_t i = 0; i < curr_token.data.length(); ++i) {
                                if (!isupper(curr_token.data.at(i)))
                                        curr_token.type = T_LABEL_REF;
                        }
                        if (curr_token.data.front() == '$')
                                curr_token.type = T_INTEGER_LIT;
                        else if (curr_token.data.front() == '%')
                                curr_token.type = T_STACK_OFF;
                        else if (curr_token.data.back() == ':')
                                curr_token.type = T_LABEL_DEF;
                        else if (REGISTER_TABLE.find(curr_token.data) != REGISTER_TABLE.end())
                                curr_token.type = T_REGISTER;
                        else if (curr_token.type != T_MNEMONIC)
                                curr_token.type = T_LABEL_REF;
                        tokens.push_back(curr_token);
                        buff_idx += token_len;
                } else {
                        // skip undesired character, like commas
                        buff_idx++;
                }
        }
        return tokens;
}

Debug_Info grammar_check(
        const std::vector<Token> tokens,
        const std::map<std::string, int16_t> label_map
) {
        Debug_Info context;
        context.grammar_retval = ACCEPTABLE_E;
        // ensure main definition exists
        if (label_map.find("main") == label_map.end()) {
                context.grammar_retval = MISSING_MAIN_E;
                context.line_num = -1;
                return context;
        }

        bool seen_exit = false;
        size_t token_idx = 0;
        while (token_idx < tokens.size()) {
                Token first_token = tokens.at(token_idx);
                if (first_token.type != T_MNEMONIC) {
                        // expected mnemonic
                        context.grammar_retval = EXPECTED_MNEMONIC_E;
                        context.line_num = first_token.line_num;
                        context.relevant_token = first_token;
                        return context;
                }
                if (BLUEPRINTS.find(first_token.data) == BLUEPRINTS.end()) {
                        // mnemonic not recognized
                        context.grammar_retval = UNKNOWN_MNEMONIC_E;
                        context.line_num = first_token.line_num;
                        context.relevant_token = first_token;
                        return context;
                }
                if (first_token.data == "EXIT")
                        seen_exit = true;

                std::vector<Atom_Type> curr_blueprint = BLUEPRINTS.at(first_token.data);
                // check if there are enough tokens
                if (token_idx + curr_blueprint.size() > tokens.size()) {
                        // expected arguments
                        context.grammar_retval = MISSING_ARGUMENTS_E;
                        context.line_num = first_token.line_num;
                        context.relevant_token = first_token;
                        return context;
                }

                for (int arg_idx = 1; arg_idx < (int)curr_blueprint.size(); arg_idx++) {
                        // check each atom for correctness
                        Token curr_token = tokens.at(token_idx + arg_idx);
                        // to prevent mnemonic consumption causing missing exit
                        if (curr_token.type == T_MNEMONIC) {
                                context.grammar_retval = MISSING_ARGUMENTS_E;
                                context.line_num = first_token.line_num;
                                context.relevant_token = first_token;
                                return context;
                        }
                        bool atom_check_res = false;
                        bool type_check_res = false;
                        switch (curr_token.type) {
                        case T_INTEGER_LIT:
                                atom_check_res = is_valid_atom(LITERAL_INT, curr_token.data);
                                type_check_res = curr_blueprint.at(arg_idx) == LITERAL_INT;
                                break;
                        case T_LABEL_DEF: /* filtered out*/
                                break;
                        case T_LABEL_REF:
                                atom_check_res = label_map.find(curr_token.data) != label_map.end();
                                type_check_res = curr_blueprint.at(arg_idx) == LABEL;
                                break;
                        case T_MNEMONIC: /* filtered out */
                                break;
                        case T_REGISTER:
                                // checks specifically for generic registers
                                atom_check_res = is_valid_atom(REGISTER, curr_token.data);
                                type_check_res = curr_blueprint.at(arg_idx) == REGISTER;
                                break;
                        case T_STACK_OFF:
                                atom_check_res = is_valid_atom(STACK_OFFSET, curr_token.data);
                                type_check_res = curr_blueprint.at(arg_idx) == STACK_OFFSET;
                                break;
                        case T_STRING_LIT:
                                atom_check_res = is_valid_atom(LITERAL_STR, curr_token.data);
                                type_check_res = curr_blueprint.at(arg_idx) == LITERAL_STR;
                                break;
                        default: /* impossible */
                                break;
                        }
                        // ensure SOURCE types work properly
                        if (curr_blueprint.at(arg_idx) == SOURCE) {
                                atom_check_res = is_valid_atom(SOURCE, curr_token.data);
                                type_check_res =
                                        (curr_token.type == T_INTEGER_LIT)
                                        || (curr_token.type == T_REGISTER)
                                        || (curr_token.type == T_STACK_OFF);
                        }
                        bool is_valid_arg = atom_check_res && type_check_res;
                        if (!is_valid_arg && curr_blueprint.at(arg_idx) == LABEL) {
                                context.grammar_retval = UNKNOWN_LABEL_E;
                                context.line_num = first_token.line_num;
                                context.relevant_token = curr_token;
                                return context;
                        } else if (!is_valid_arg) {
                                context.grammar_retval = INVALID_ATOM_E;
                                context.line_num = first_token.line_num;
                                context.relevant_token = curr_token;
                                return context;
                        }
                }
                token_idx += curr_blueprint.size();
        }

        // sure exit instruction exists
        if (!seen_exit) {
                context.grammar_retval = MISSING_EXIT_E;
                context.line_num = -1;
        }
        return context;
}

std::vector<int16_t> translate_string(const std::string stripped_quote) {
        std::vector<int16_t> result = {};
        std::vector<char> intermediate = {};
        size_t str_idx = 0;
        while (str_idx < stripped_quote.length()) {
                char curr = stripped_quote.at(str_idx);
                // add normally if not a possible escape character
                if (curr != '\\' || str_idx == stripped_quote.length() - 1) {
                        intermediate.push_back(curr);
                        str_idx++;
                        continue;
                }
                // handle escaped char
                char next = stripped_quote.at(str_idx + 1);
                if (next == 'n') {
                        intermediate.push_back('\n');
                        str_idx += 2;
                } else if (next == '\"') {
                        intermediate.push_back('\"');
                        str_idx += 2;
                } else {
                        intermediate.push_back(curr);
                        str_idx++;
                }
        }
        if (intermediate.size() % 2 == 1)
                intermediate.push_back('\0');
        for (size_t idx = 0; idx < intermediate.size(); idx += 2) {
                int16_t lower = (int16_t)(intermediate.at(idx));
                int16_t upper = (int16_t)(intermediate.at(idx + 1));
                int16_t final = (upper << 8) | lower;
                result.push_back(final);
        }
        result.push_back(0);
        result.shrink_to_fit();
        return result;
}
