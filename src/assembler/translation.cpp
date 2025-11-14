#include "common_values.h"

#include <cstdint>
#include <deque>
#include <map>
#include <vector>

// just to be sure
#define ENTRY_OFFSET 1

Debug_Info generate_program(std::vector<int16_t> &result,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table) {
        Debug_Info context;
        context.assembler_retval = ACCEPTABLE_2;

        int16_t instruction_idx = 0, program_idx = 0;
        if (label_table.find("main") == label_table.end()) {
                context.assembler_retval = MISSING_MAIN_2;
                return context;
        }
        // because of this entry address, all label translations will
        // need +1 added to their index
        result.push_back(label_table.at("main") + ENTRY_OFFSET);
        while (!tokens.empty()) {
                if (BLUEPRINTS.find(tokens.front()) == BLUEPRINTS.end()) {
                        context.relevant_idx = instruction_idx;
                        context.relevant_tokens = {tokens.front()};
                        context.assembler_retval = EXPECTED_MNEMONIC_2;
                        return context;
                }
                // is intentionally duplicate: see main.cpp usage of MISSING_ARGUMENTS_2
                std::vector<Atom_Type> curr_blueprint = BLUEPRINTS.at(tokens.front());
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
                std::deque<int16_t> curr_bytes = {};
                for (size_t i = 0; i < curr_blueprint.size(); ++i) {
                        // actual translation goes here
                }
                for (size_t i = 0; i < curr_blueprint.size(); ++i) {
                        tokens.pop_front();
                        program_idx++;
                }
                instruction_idx++;
        }

        return context;
}
