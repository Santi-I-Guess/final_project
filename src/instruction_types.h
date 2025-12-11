#ifndef INSTRUCTION_TYPES_H
#define INSTRUCTION_TYPES_H 1

#include <cstdint>
#include <string>
#include <map>
#include <vector>

#include "token_types.h"

/**
 * @brief stores all relevant information of an instruction in one place
 * @details helper struct for BLUEPRINTS
 */
struct Instruction_Data {
        int16_t opcode;
        std::string mnem_name;
        std::vector<Atom_Type> blueprint;
        size_t length;
        Instruction_Data() = default;
        Instruction_Data(int16_t given_opcode, std::string given_mnem_name, std::vector<Atom_Type> given_blueprint);
};

Instruction_Data get_instruction(const int16_t &opcode);

std::string get_mnem_name(const int16_t &opcode);

int16_t get_opcode(const std::string &mnem_name);

/**
 * @brief hashmap for valid callable registers in assembly language
 */
const std::map<std::string, int16_t> REGISTER_TABLE = {
        {"RZ",   0}, {"RA",  1}, {"RB",    2}, {"RC",    3},
        {"RD",   4}, {"RE",  5}, {"RF",    6}, {"RG",    7},
        {"RH",   8}, {"RSP", 9}, {"RIP",  10}, {"CMP0", 11},
        {"CMP1", 12}
};

#endif
