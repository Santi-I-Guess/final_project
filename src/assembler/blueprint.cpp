#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include <sstream>

#include "blueprint.h"
#include "common_values.h"

std::map<std::string, int16_t> define_labels(std::deque<std::string> &tokens) {
        std::map<std::string, int16_t> symbols = {};
        std::deque<std::string> filtered_tokens = {}; // remove label definitions
        size_t program_addr = 0;
        for (std::string curr_token: tokens) {
                if (curr_token.back() != ':') {
                        filtered_tokens.push_back(curr_token);
                        program_addr++;
                        continue;
                }
                curr_token.pop_back();
                if (symbols.find(curr_token) == symbols.end())
                        symbols.insert({curr_token, (int16_t)program_addr});
                else
                        symbols[curr_token] = (int16_t)program_addr;
        }
        tokens.swap(filtered_tokens);
        return symbols;
}

bool is_valid_i16(std::string token) {
        std::stringstream the_stream(token);
        int32_t value = 0;
        the_stream >> token;
        if (the_stream.fail())
                return false;
        if (value > (int32_t)INT16_MAX)
                return false;
        if (value < (int32_t)INT16_MIN)
                return false;
        return true;
}

bool is_valid_atom(Atom_Type atom_type, std::string token) {
        bool first, second;
        switch (atom_type) {
        case LABEL:
                // easier to check in later function call
                return true;
        case LITERAL_INT:
                if (token.front() != '$')
                        return false;
                token = token.substr(1, token.length()-1);
                return is_valid_i16(token);
        case LITERAL_STR:
                first = token.front() == token.back();
                second = token.front() == '\"';
                return first && second;
        case MNEMONIC:
                return BLUEPRINTS.find(token) != BLUEPRINTS.end();
        case REGISTER:
                return REGISTER_TABLE.find(token) != REGISTER_TABLE.end();
        case SOURCE:
                if (is_valid_atom(LITERAL_INT, token))
                        return true;
                if (is_valid_atom(REGISTER, token))
                        return true;
                return is_valid_atom(STACK_OFFSET, token);
        case STACK_OFFSET:
                if (token.front() != '%')
                        return false;
                token = token.substr(1, token.length()-1);
                return is_valid_i16(token);
        }
        return true;
}


Debug_Info is_valid_arguments(std::deque<std::string> tokens,
                              std::map<std::string, int16_t> label_table) {
        // std::deque::pop_front makes std::deque::erase irrelevant
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
        int instruction_idx = 0;
        while (!tokens.empty()) {
                if (BLUEPRINTS.find(tokens.front()) == BLUEPRINTS.end()) {
                        context.relevant_idx = instruction_idx;
                        context.relevant_tokens = {tokens.front()};
                        context.grammar_retval = EXPECTED_MNEMONIC;
                        return context;
                }
                std::deque<Atom_Type> curr_blueprint;
                curr_blueprint = BLUEPRINTS.at(tokens.front());
                curr_blueprint.shrink_to_fit(); /* prevent werid uninit data */
                if (tokens.size() < curr_blueprint.size()) {
                        context.relevant_idx = instruction_idx;
                        context.relevant_tokens = {tokens.front()};
                        context.grammar_retval = MISSING_ARGUMENTS;
                        return context;
                }
                for (size_t i = 0; i < curr_blueprint.size(); ++i) {
                        if (!is_valid_atom(curr_blueprint.at(i), tokens.at(i))) {
                                context.relevant_idx = instruction_idx;
                                context.relevant_tokens = {tokens.at(0), tokens.at(i)};
                                context.grammar_retval = INVALID_ATOM;
                                return context;
                        }
                        bool lbl_chk_1 = curr_blueprint.at(i) == LABEL;
                        bool lbl_chk_2 = label_table.find(tokens.at(i))
                                == label_table.end();
                        if (lbl_chk_1 && lbl_chk_2) {
                                context.relevant_idx = instruction_idx;
                                context.relevant_tokens = {tokens.at(0), tokens.at(i)};
                                context.grammar_retval = UNKNOWN_LABEL;
                        }
                }
                for (size_t i = 0; i < curr_blueprint.size(); ++i)
                        tokens.pop_front();
                instruction_idx++;
        }
        return context;
}
