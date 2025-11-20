#include <deque>
#include <map>
#include <string>
#include <sstream>

#include "common_values.h"
#include "translation.h"

Debug_Info generate_program(std::deque<int16_t> &program,
                            Program_Info program_info) {
        Debug_Info context;
        context.assembler_retval = ACCEPTABLE_2;
        std::deque<std::string> &tokens = program_info.tokens;
        std::map<std::string, int16_t> &label_table = program_info.label_table;
        std::map<int16_t, int16_t> &str_idx_offsets = program_info.str_idx_offsets;

        // Step 1: calculate string data offset, store string idx offsets
        int16_t entry_offset = 1; // main label
        int16_t num_strings = 0;
        for (std::string i : tokens) {
                if (i.back() != '\"')
                        continue;
                i = i.substr(1, i.length() - 2); // strip quotes
                std::deque<int16_t> string_output = translate_string(i);
                for (int16_t j : string_output) {
                        program.push_back(j);
                }
                str_idx_offsets.insert({num_strings, entry_offset});
                entry_offset += (int16_t)(string_output.size());
                num_strings++;
        }

        // Step 2: add entry offset to beginning of program
        program.push_front(label_table.at("main") + entry_offset);

        // Step 3: translate normal instructions and arguments
        int16_t num_seen_strs = 0;
        int16_t instruction_idx = 0;
        while (!tokens.empty()) {
                std::deque<Atom_Type> curr_blueprint;
                curr_blueprint = BLUEPRINTS.at(tokens.front());
                for (size_t idx = 0; idx < curr_blueprint.size(); ++idx) {
                        bool res = is_valid_key(
                                tokens.at(idx),
                                curr_blueprint.at(idx),
                                program_info
                        );
                        if (!res) {
                                context.assembler_retval = INVALID_ATOM_2;
                                context.relevant_idx = instruction_idx;
                                context.relevant_tokens = {tokens.at(idx)};
                                return context;
                        }
                        int16_t translated = translate_token(
                                tokens.at(idx),
                                curr_blueprint.at(idx),
                                program_info,
                                num_seen_strs
                        );
                        program.push_back(translated);
                        if (curr_blueprint.at(idx) == LITERAL_STR)
                                num_seen_strs++;
                }
                for (size_t idx = 0; idx < curr_blueprint.size(); ++idx)
                        tokens.pop_front();
                instruction_idx++;
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

std::deque<int16_t> translate_string(std::string stripped_quote) {
        std::deque<int16_t> result = {};
        std::deque<char> intermediate = {};
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
                int16_t upper = (int16_t)(intermediate.at(idx + 1));
                int16_t lower = (int16_t)(intermediate.at(idx));
                int16_t final = (upper << 8) | lower;
                result.push_back(final);
        }
        result.push_back(0);
        result.shrink_to_fit();
        return result;
}

int16_t translate_token(std::string token, Atom_Type atom_type,
                        Program_Info program_info, int16_t num_seen_strs) {
        std::map<int16_t, int16_t> str_idx_offsets = program_info.str_idx_offsets;
        std::map<std::string, int16_t> label_table = program_info.label_table;
        std::stringstream aux_stream;
        int16_t aux_int;
        switch (atom_type) {
        case LABEL:
                return label_table.at(token);
        case LITERAL_INT:
                token = token.substr(1, token.length() - 1);
                aux_stream << token;
                aux_stream >> aux_int;
                aux_int |= (int16_t)(1 << 30); // bitmask
                return aux_int;
        case LITERAL_STR: // the idx, not the string itself
                return str_idx_offsets.at(num_seen_strs);
        case MNEMONIC:
                return OPCODE_TABLE.at(token);
        case REGISTER:
                return REGISTER_TABLE.at(token);
        case SOURCE:
                if (token.front() == '$')
                        return translate_token(token, LITERAL_INT, program_info, num_seen_strs);
                else if (token.front() == 'R')
                        return translate_token(token, REGISTER, program_info, num_seen_strs);
                else
                        return translate_token(token, STACK_OFFSET, program_info, num_seen_strs);
        case STACK_OFFSET:
                token = token.substr(1, token.length() - 1);
                aux_stream << token;
                aux_stream >> aux_int;
                aux_int |= (int16_t)(1 << 29); // bitmask
                return aux_int;
        }
        return 0; // impossible
}
