#include <cctype>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../token_types.h"
#include "helper.h"
#include "tokenizer.h"

extern std::map<std::string, Instruction_Data> BLUEPRINTS;

std::map<std::string, int16_t> create_label_map(
        const std::vector<Token> &tokens
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
                label_name.pop_back();
                label_map.insert({label_name, prog_addr - num_seen_declarations});
                num_seen_declarations++;
        }
        return label_map;
}

std::vector<Token> create_tokens(const std::string &source_buffer) {
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
                        else if (curr_token.data.front() == '[')
                                curr_token.type = T_RAM_ADDR;
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
        const std::vector<Token> &tokens,
        const std::map<std::string, int16_t> &label_map
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

                Instruction_Data curr_instruction = BLUEPRINTS.at(first_token.data);
                std::vector<Atom_Type> curr_blueprint = curr_instruction.blueprint;
                // check if there are enough tokens
                if (token_idx + curr_instruction.length > tokens.size()) {
                        // expected arguments
                        context.grammar_retval = MISSING_ARGUMENTS_E;
                        context.line_num = first_token.line_num;
                        context.relevant_token = first_token;
                        return context;
                }

                for (int arg_idx = 1; arg_idx < (int)curr_instruction.length; arg_idx++) {
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
                        case T_RAM_ADDR:
                                atom_check_res = is_valid_atom(RAM_ADDR, curr_token.data);
                                type_check_res = curr_blueprint.at(arg_idx) == RAM_ADDR;
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
                                        || (curr_token.type == T_RAM_ADDR)
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
                token_idx += curr_instruction.length;
        }

        // sure exit instruction exists
        if (!seen_exit) {
                context.grammar_retval = MISSING_EXIT_E;
                context.line_num = -1;
        }
        return context;
}
