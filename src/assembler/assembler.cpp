#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "../common_values.h"
#include "assembler.h"
#include "tokenizer.h"

std::vector<int16_t> assemble_program(
        const std::vector<Token> &tokens,
        const std::map<std::string, int16_t> &label_map
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
                Instruction_Data curr_instruction;
                curr_instruction = BLUEPRINTS.at(first_token.data);
                for (size_t ins_idx = 0; ins_idx < curr_instruction.length; ins_idx++) {
                        Token curr_token = tokens.at(token_idx + ins_idx);
                        int16_t translated = 0;
                        std::stringstream aux_stream;
                        std::string stripped_token = "";
                        switch (curr_token.type) {
                        case T_INTEGER_LIT:
                                stripped_token = curr_token.data.substr(1, curr_token.data.length() - 1);
                                aux_stream << stripped_token;
                                aux_stream >> translated;
                                translated |= (int16_t)(4 << 12); // addressing mode bitmask
                                break;
                        case T_LABEL_DEF:
                                break;
                        case T_LABEL_REF:
                                translated = label_map.at(curr_token.data) + main_addr_offset;
                                break;
                        case T_MNEMONIC:
                                translated = curr_instruction.opcode;
                                break;
                        case T_RAM_ADDR:
                                stripped_token = curr_token.data.substr(2, curr_token.data.length() - 3);
                                aux_stream << stripped_token;
                                aux_stream >> translated;
                                translated |= (int16_t)(2 << 12); // addressing mode bitmask
                                break;
                        case T_REGISTER:
                                translated = REGISTER_TABLE.at(curr_token.data);
                                break;
                        case T_STACK_OFF:
                                stripped_token = curr_token.data.substr(1, curr_token.data.length() - 1);
                                aux_stream << stripped_token;
                                aux_stream >> translated;
                                translated |= (int16_t)(1 << 12); // addressing mode bitmask
                                break;
                        case T_STRING_LIT:
                                translated = string_addrs.at(num_seen_strs);
                                translated |= (int16_t)(3 << 12); // addressing mode bitmask
                                break;
                        default: /* impossible */
                                break;
                        }
                        program.push_back(translated);
                        if (curr_instruction.blueprint.at(ins_idx) == LITERAL_STR)
                                num_seen_strs++;
                }
                token_idx += curr_instruction.length;
        }

        return program;
}

std::vector<int16_t> translate_string(const std::string &stripped_quote) {
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
