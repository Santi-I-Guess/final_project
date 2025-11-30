#include <cstdint>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "../common_values.h"
#include "blueprint.h"

std::map<std::string, int16_t> create_label_map(std::vector<std::string> &tokens) {
        std::map<std::string, int16_t> symbols = {};
        std::vector<std::string> filtered_tokens = {}; // remove label definitions
        size_t program_addr = 0;
        for (std::string curr_token: tokens) {
                if (curr_token.back() != ':') {
                        filtered_tokens.push_back(curr_token);
                        program_addr++;
                        continue;
                }
                curr_token.pop_back();
                // if symbol has been defined, redefine but put out a warning
                if (symbols.find(curr_token) == symbols.end()) {
                        symbols.insert({curr_token, (int16_t)program_addr});
                } else {
                        std::cout << "Warning: " << curr_token;
                        std::cout << " is defined more than once, and will ";
                        std::cout << "take the newest definition\n";
                        symbols[curr_token] = (int16_t)program_addr;
                }
        }
        tokens.swap(filtered_tokens);
        return symbols;
}

bool is_valid_atom(const Atom_Type atom_type, const std::string token) {
        bool first, second;
        std::string stripped_token;
        switch (atom_type) {
        case LABEL:
                // easier to check in later function call
                return true;
        case LITERAL_INT:
                if (token.front() != '$')
                        return false;
                stripped_token = token.substr(1, token.length() - 1);
                return is_valid_i16(stripped_token);
        case LITERAL_STR:
                first = token.front() == token.back();
                second = token.front() == '\"';
                return first && second;
        case MNEMONIC:
                return BLUEPRINTS.find(token) != BLUEPRINTS.end();
        case REGISTER:
                // general purpose registers only
                if (REGISTER_TABLE.find(token) == REGISTER_TABLE.end())
                        return false;
                return REGISTER_TABLE.at(token) < 8;
        case SOURCE:
                if (is_valid_atom(LITERAL_INT, token))
                        return true;
                if (REGISTER_TABLE.find(token) != REGISTER_TABLE.end())
                        return true;
                return is_valid_atom(STACK_OFFSET, token);
        case STACK_OFFSET:
                if (token.front() != '%')
                        return false;
                stripped_token = token.substr(1, token.length() - 1);
                return is_valid_i16(stripped_token);
        }
        return true;
}

bool is_valid_i16(const std::string token) {
        std::stringstream the_stream(token);
        int32_t value = 0;
        the_stream >> value;
        if (the_stream.fail())
                return false;
        if (value > (int32_t)INT16_MAX)
                return false;
        if (value < (int32_t)INT16_MIN)
                return false;
        return true;
}



Debug_Info grammar_check(
        const std::vector<std::string> tokens,
        const std::map<std::string, int16_t> label_table
) {
        // std::vector::pop_front makes std::vector::erase irrelevant
        // (saves on copies)

        Debug_Info context;
        context.grammar_retval = ACCEPTABLE;
        if (label_table.find("main") == label_table.end()) {
                context.grammar_retval = MISSING_MAIN;
                return context;
        }
        // make sure there's at least one EXIT instruction
        bool seen_exit = false;
        for (std::string i : tokens) {
                if (i == "EXIT") {
                        seen_exit = true;
                        break;
                }
        }
        if (!seen_exit) {
                context.grammar_retval = MISSING_EXIT;
                return context;
        }

        // map has std::map::find, which makes checking easy
        int debug_instruction_idx = 0;
        size_t token_idx = 0;
        while (token_idx < tokens.size()) {
                // check front token is a known mnemonic
                if (BLUEPRINTS.find(tokens.at(token_idx)) == BLUEPRINTS.end()) {
                        context.relevant_idx = debug_instruction_idx;
                        context.relevant_tokens = {tokens.at(token_idx)};
                        context.grammar_retval = EXPECTED_MNEMONIC;
                        return context;
                }
                std::vector<Atom_Type> curr_blueprint;
                curr_blueprint = BLUEPRINTS.at(tokens.at(token_idx));
                curr_blueprint.shrink_to_fit();
                // check there are enough tokens to warrent argument check
                if (tokens.size() < curr_blueprint.size()) {
                        context.relevant_idx = debug_instruction_idx;
                        context.relevant_tokens = {tokens.at(token_idx)};
                        context.grammar_retval = MISSING_ARGUMENTS;
                        return context;
                }
                // check each argument type
                for (size_t i = 0; i < curr_blueprint.size(); ++i) {
                        if (!is_valid_atom(curr_blueprint.at(i), tokens.at(token_idx + i))) {
                                context.relevant_idx = debug_instruction_idx;
                                context.relevant_tokens = {tokens.at(token_idx), tokens.at(token_idx + i)};
                                context.grammar_retval = INVALID_ATOM;
                                return context;
                        }
                        bool lbl_chk_1 = curr_blueprint.at(i) == LABEL;
                        bool lbl_chk_2 = label_table.find(tokens.at(token_idx + i)) == label_table.end();
                        if (lbl_chk_1 && lbl_chk_2) {
                                context.relevant_idx = debug_instruction_idx;
                                context.relevant_tokens = {tokens.at(token_idx), tokens.at(token_idx + i)};
                                context.grammar_retval = UNKNOWN_LABEL;
                        }
                }
                token_idx += curr_blueprint.size();
                debug_instruction_idx++;
        }
        return context;
}
