#include <cstdint>
#include <deque>
#include <map>
#include <sstream>
#include <string>

#include "common_values.h"
#include "translation.h"

// just to be sure
#define ENTRY_OFFSET (int16_t)1

int16_t translate_token(std::string token, Atom_Type atom_type,
                        std::map<std::string, int16_t> label_table) {
        // can't instantiate variables in switch case
        // literal flag: 1 << 14
        // stack flag: 1 << 13
        int16_t value;
        std::string aux;
        std::stringstream sstream;
        switch (atom_type) {
        case LABEL:
                return label_table.at(token);
        case LITERAL_INT:
                aux = token.substr(1, token.length() - 1);
                sstream << aux;
                sstream >> value;
                value |= (1 << 14);
                return value;
        case LITERAL_STR:
                // LITERAL_STR handeled by a different function
                return 0;
        case MNEMONIC:
                return OPCODE_TABLE.at(token);
        case REGISTER:
                return REGISTER_TABLE.at(token);
        case SOURCE:
                if (token.at(0) == '$')
                        return translate_token(token, LITERAL_INT, label_table);
                if (token.at(0) == 'R')
                        return translate_token(token, REGISTER, label_table);
                if (token.at(0) == '%')
                        return translate_token(token, STACK_OFFSET, label_table);
                return 0; // impossible if grammar checker did its thing
        case STACK_OFFSET:
                aux = token.substr(1, token.length() - 1);
                sstream << aux;
                sstream >> value;
                value |= (1 << 13);
                return value;
        }
        return 0; // just to satisfy compiler
}


std::deque<int16_t> translate_string(std::string token) {
        // each char takes up a single byte, but assembler uses int16_t
        // to remedy this, we'll store two chars per int16_t
        // strings are marked as ended when the lowest 8 bits are null
        std::deque<char> raw_chars = {};
        std::deque<int16_t> curr_bytes = {};
        size_t i = 0, token_len = token.length();
        while (i < token_len) {
                // don't really care about starting quotes
                if (i == 0 || i == token_len - 1) {
                        i++;
                        continue;
                }
                // support interpreting newlines and double quotes
                if (token.at(i) == '\\' && i != token_len - 1) {
                        if (token.at(i+1) == 'n') {
                                raw_chars.push_back('\n');
                                i += 2;
                                continue;
                        } else if (token.at(i+1) == '\"') {
                                raw_chars.push_back('\"');
                                i += 2;
                                continue;
                        }
                        // fall through if condition is unmet
                }
                raw_chars.push_back(token.at(i));
                i++;
        }
        // ensure the last byte to be added is null
        if (raw_chars.size() % 2 == 0) {
                raw_chars.push_back(0);
                raw_chars.push_back(0);
        } else {
                raw_chars.push_back(0);
        }
        size_t chars_len = raw_chars.size();
        for (size_t j = 0; j < chars_len; j += 2) {
                // pack every 2 chars into one int16_t
                // note to self: my system is little endian
                int16_t higher = raw_chars.at(j + 1) << 8;
                int16_t lower = raw_chars.at(j);
                curr_bytes.push_back(higher | lower);
        }
        return curr_bytes;
}

Debug_Info generate_program(std::deque<int16_t> &result,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table) {
        Debug_Info context;
        context.assembler_retval = ACCEPTABLE_2;

        int instruction_idx = 0, program_idx = 0;
        if (label_table.find("main") == label_table.end()) {
                context.assembler_retval = MISSING_MAIN_2;
                return context;
        }
        // because of this entry address, all label translations will
        // need +1 added to their index
        // because strings have variable length, labels will also
        // need to account for extra string length
        int16_t string_offset = 0;
        result.push_back(label_table.at("main") + ENTRY_OFFSET);
        while (!tokens.empty()) {
                if (BLUEPRINTS.find(tokens.front()) == BLUEPRINTS.end()) {
                        context.relevant_idx = instruction_idx;
                        context.relevant_tokens = {tokens.front()};
                        context.assembler_retval = EXPECTED_MNEMONIC_2;
                        return context;
                }
                // is intentionally duplicate: see main.cpp usage of MISSING_ARGUMENTS_2
                std::deque<Atom_Type> curr_blueprint;
                curr_blueprint = BLUEPRINTS.at(tokens.front());
                curr_blueprint.shrink_to_fit();
                if (curr_blueprint.size() > tokens.size()) {
                        context.relevant_idx = instruction_idx;
                        context.relevant_tokens = {tokens.front()};
                        context.assembler_retval = MISSING_ARGUMENTS_2;
                        return context;
                }
                std::deque<std::string> curr_ins = std::deque<std::string>(
                        tokens.begin(),
                        tokens.begin() + curr_blueprint.size()
                );
                for (size_t i = 0; i < curr_blueprint.size(); ++i) {
                        // actual translation goes here
                        // can't declare variables in switch
                        int16_t value;
                        std::deque<int16_t> str_bytes;
                        switch (curr_blueprint.at(i)) {
                        case LITERAL_STR:
                                // increase program_idx by string length
                                str_bytes = translate_string(curr_ins.at(i));
                                result.insert(
                                        result.end(),
                                        str_bytes.begin(),
                                        str_bytes.end()
                                );
                                string_offset += (int16_t)str_bytes.size();
                                break;
                        case LABEL:
                                value = translate_token(
                                        curr_ins.at(i),
                                        curr_blueprint.at(i),
                                        label_table
                                );
                                value += ENTRY_OFFSET;
                                value += string_offset;
                                result.push_back(value);
                                break;
                        default:
                                value = translate_token(
                                        curr_ins.at(i),
                                        curr_blueprint.at(i),
                                        label_table
                                );
                                result.push_back(value);
                                break;
                        }
                }
                for (size_t i = 0; i < curr_blueprint.size(); ++i) {
                        tokens.pop_front();
                        program_idx++;
                }
                instruction_idx++;
        }

        return context;
}
