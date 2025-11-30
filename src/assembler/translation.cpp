#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "../common_values.h"
#include "translation.h"

// TODO: reduce max indentation to 3 levels: refactor step 3 of assemble_program
// TODO: refactor instances of 80+ chars

Debug_Info assemble_program(
        std::vector<int16_t> &program,
        Program_Info program_info
) {
        Debug_Info context;
        context.assembler_retval = ACCEPTABLE_2;
        std::vector<std::string> &tokens = program_info.tokens;
        std::map<std::string, int16_t> &label_table = program_info.label_table;
        std::map<int16_t, int16_t> &str_idx_offsets = program_info.str_idx_offsets;

        // Step 1: calculate entry_offset, store string idx offsets
        int16_t entry_offset = 5; // 'SA','NT','IA','GO', entry_addr
        int16_t num_strings = 0;
        for (std::string i : tokens) {
                // skip if not a string
                if (i.back() != '\"')
                        continue;
                i = i.substr(1, i.length() - 2); // strip quotes
                std::vector<int16_t> string_output = translate_string(i);
                str_idx_offsets.insert({num_strings, entry_offset});
                entry_offset += (int16_t)(string_output.size());
                num_strings++;
        }
        // Step 2: add entry offset and magic number to beginning of program
        program.push_back((int16_t)(0x4153)); // SA
        program.push_back((int16_t)(0x544e)); // NT
        program.push_back((int16_t)(0x4149)); // IA
        program.push_back((int16_t)(0x4f47)); // GO
        // ensure 0 element buffer between entry addr and first opcode
        if (num_strings == 0)
                entry_offset++;
        // increase offset for 0xffff after strings get parsed
        entry_offset++;
        program.push_back(label_table.at("main") + entry_offset);

        // Step 2.5: add string literal data
        for (std::string i : tokens) {
                // skip if not a string
                if (i.back() != '\"')
                        continue;
                i = i.substr(1, i.length() - 2); // strip quotes
                std::vector<int16_t> string_output = translate_string(i);
                for (int16_t j : string_output) {
                        program.push_back(j);
                }
        }
        // marks end of strings
        if (num_strings == 0)
                program.push_back((int16_t)(0x0));
        program.push_back((int16_t)(0xffff));

        // Step 3: translate normal instructions and arguments
        int16_t num_seen_strs = 0;
        int16_t debug_instruction_idx = 0;
        size_t token_idx = 0;
        while (token_idx < tokens.size()) {
                std::vector<Atom_Type> curr_blueprint;
                curr_blueprint = BLUEPRINTS.at(tokens.at(token_idx));
                for (size_t idx = 0; idx < curr_blueprint.size(); ++idx) {
                        bool res = is_valid_key(
                                tokens.at(token_idx + idx),
                                curr_blueprint.at(idx),
                                program_info
                        );
                        if (!res) {
                                context.assembler_retval = INVALID_ATOM_2;
                                context.relevant_idx = debug_instruction_idx;
                                context.relevant_tokens = {tokens.at(token_idx + idx)};
                                return context;
                        }
                        int16_t translated = translate_token(
                                tokens.at(token_idx + idx),
                                curr_blueprint.at(idx),
                                program_info,
                                num_seen_strs,
                                entry_offset
                        );
                        program.push_back(translated);
                        if (curr_blueprint.at(idx) == LITERAL_STR)
                                num_seen_strs++;
                }
                token_idx += curr_blueprint.size();
                debug_instruction_idx++;
        }

        return context;
}

bool is_valid_key(std::string token, Atom_Type atom_type, Program_Info program_info) {
        std::map<std::string, int16_t> &label_table = program_info.label_table;

        switch (atom_type) {
        case LABEL:
                return label_table.find(token) != label_table.end();
        case MNEMONIC:
                return OPCODE_TABLE.find(token) != OPCODE_TABLE.end();
        case REGISTER:
                return REGISTER_TABLE.find(token) != REGISTER_TABLE.end();
        case SOURCE:
                if (token.front() == 'R')
                        return REGISTER_TABLE.find(token) != REGISTER_TABLE.end();
                else
                        return true;
        default:
                return true;
        }
}

std::vector<int16_t> translate_string(std::string stripped_quote) {
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

int16_t translate_token(std::string token, Atom_Type atom_type,
                        Program_Info program_info, int16_t num_seen_strs,
                        int16_t entry_offset) {
        std::map<int16_t, int16_t> str_idx_offsets = program_info.str_idx_offsets;
        std::map<std::string, int16_t> label_table = program_info.label_table;
        std::stringstream aux_stream;
        int16_t aux_int;
        switch (atom_type) {
        case LABEL:
                return label_table.at(token) + entry_offset;
        case LITERAL_INT:
                token = token.substr(1, token.length() - 1);
                aux_stream << token;
                aux_stream >> aux_int;
                aux_int |= (int16_t)(1 << 14); // bitmask
                return aux_int;
        case LITERAL_STR:
                // the idx, not the string itself
                return str_idx_offsets.at(num_seen_strs) | (1 << 12);
        case MNEMONIC:
                return OPCODE_TABLE.at(token);
        case REGISTER:
                return REGISTER_TABLE.at(token);
        case SOURCE:
                if (token.front() == '$')
                        return translate_token(token, LITERAL_INT, program_info, num_seen_strs, entry_offset);
                else if (token.front() == 'R')
                        return translate_token(token, REGISTER, program_info, num_seen_strs, entry_offset);
                else
                        return translate_token(token, STACK_OFFSET, program_info, num_seen_strs, entry_offset);
        case STACK_OFFSET:
                token = token.substr(1, token.length() - 1);
                aux_stream << token;
                aux_stream >> aux_int;
                aux_int |= (int16_t)(1 << 13); // bitmask
                return aux_int;
        }
        return 0; // impossible
}
