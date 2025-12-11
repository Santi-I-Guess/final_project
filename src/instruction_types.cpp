#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "instruction_types.h"

/**
 * @brief hashmap that defines template of instructions in assembly language
 */
std::map<std::string, Instruction_Data> BLUEPRINTS;

Instruction_Data::Instruction_Data(
        int16_t given_opcode,
        std::string given_mnem_name,
        std::vector<Atom_Type> given_blueprint
) {
        length = given_blueprint.size();
        opcode = given_opcode;
        mnem_name = given_mnem_name;
        blueprint = given_blueprint;
}

Instruction_Data get_instruction(const int16_t &opcode) {
        // ordered_maps are organized by key in lexographical order,
        // not by initalization order
        std::map<std::string, Instruction_Data>::const_iterator it;
        for (it = BLUEPRINTS.begin(); it != BLUEPRINTS.end(); ++it) {
                if (it->second.opcode == opcode)
                        return it->second;
        }
        return Instruction_Data();
}

std::string get_mnem_name(const int16_t &opcode) {
        // ordered_maps are organized by key in lexographical order,
        // not by initalization order
        std::map<std::string, Instruction_Data>::const_iterator it;
        for (it = BLUEPRINTS.begin(); it != BLUEPRINTS.end(); ++it) {
                if (it->second.opcode == opcode)
                        return it->first;
        }
        return "";
}

int16_t get_opcode(const std::string &mnem_name) {
        return BLUEPRINTS.at(mnem_name).opcode;
}
