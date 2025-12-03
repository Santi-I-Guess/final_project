#include <cstdint>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#include "../common_values.h"
#include "tokenizer.h"
#include "blueprint.h"

std::map<std::string, int16_t> create_label_map(
        const std::vector<Token> tokens
) {
        std::map<std::string, int16_t> label_map;
        for (int16_t prog_addr = 0; prog_addr < (int16_t)tokens.size(); ++prog_addr) {
                Token curr_token = tokens.at(prog_addr);
                if (curr_token.type != T_LABEL_DEF)
                        continue;
                std::string label_name = curr_token.data;
                // remove colon
                label_name = label_name.substr(0, label_name.length()-1);
                label_map.insert({label_name, prog_addr});
        }
        return label_map;
}


Debug_Info_2 grammar_check_2(
        const std::vector<Token> tokens,
        const std::map<std::string, int16_t> label_map
) {
        Debug_Info_2 context;
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
                        if (!(atom_check_res && type_check_res)) {
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

bool is_valid_atom(const Atom_Type atom_type, const std::string token) {
        bool first, second;
        std::string stripped_token;
        switch (atom_type) {
        case LABEL: /* checked earlier to avoid adding label_map as parameter */
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
