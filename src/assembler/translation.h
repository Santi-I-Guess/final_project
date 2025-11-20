#ifndef TRANSLATION_H
#define TRANSLATION_H 1

#include "common_values.h"

#include <cstdint>
#include <deque>
#include <map>

/**
 * @brief hashmap for valid opcodes for each mnemoinc in assembly language
 */
// new: not, inc, dec
const std::map<std::string, int16_t> OPCODE_TABLE = {
        {"NOP",    0}, {"MOV",     1}, {"INC",   2},
        {"DEC",    3}, {"ADD",     4}, {"SUB",   5},
        {"MUL",    6}, {"DIV",     7}, {"AND",   8},
        {"OR",     9}, {"NOT",    10}, {"XOR",  11},
        {"LSH",   12}, {"RSH",    13}, {"CMP",  14},
        {"JEQ",   15}, {"JNE",    16}, {"JGE",  17},
        {"JGR",   18}, {"JLE",    19}, {"JLS",  20},
        {"CALL",  21}, {"RET",    22}, {"PUSH", 23},
        {"POP",   24}, {"WRITE",  25}, {"READ", 26},
        {"PRINT", 27}, {"SPRINT", 28}, {"EXIT", 29},
};

/**
 * @brief assembles the final program into a deque of int16_t's
 */
Debug_Info generate_program(std::deque<int16_t> &program,
                            Program_Info program_info);

/**
 * @brief check if token expected to be mapped is valid
 * @details helper function of generate_program
 */
bool is_valid_key(std::string token, Atom_Type atom_type, Program_Info program_info);

/**
 * @brief translates a single string into series of int16_t's with a null int16_t
 * @details helper function of generate_program
 */
std::deque<int16_t> translate_string(std::string stripped_token);

/**
 * @brief translates a single token into an int16_t
 * @details helper function of generate_program
 */
int16_t translate_token(std::string token, Atom_Type atom_type,
                        Program_Info program_info, int16_t num_seen_strs);

#endif
