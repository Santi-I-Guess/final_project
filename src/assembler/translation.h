#ifndef TRANSLATION_H
#define TRANSLATION_H 1

#include "common_values.h"

#include <cstdint>
#include <deque>
#include <map>
#include <vector>

// valid register symbols
const std::map<std::string, int16_t> REGISTER_TABLE = {
        {"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3},
        {"R4", 4}, {"R5", 5}, {"R6", 6}, {"R7", 7},
};

// opcode for each instruction mnemonic
const std::map<std::string, int16_t> OPCODE_TABLE = {
        {"NOP",    0}, {"CALL",  1}, {"RET",   2}, {"PRINT",  3},
        {"SPRINT", 4}, {"EXIT",  5}, {"READ",  6}, {"WRITE",  7},
        {"POP",    8}, {"PUSH",  9}, {"MOV",  10}, {"ADD",   11},
        {"SUB",   12}, {"MUL",  13}, {"DIV",  14}, {"AND",   15},
        {"OR",    16}, {"XOR",  17}, {"LSH",  18}, {"RSH",   19},
        {"CMP",   20}, {"JEQ",  21}, {"JNE",  22}, {"JGE",   23},
        {"JGR",   24}, {"JLE",  25}, {"JLS",  26},
};

// generates the final program
Debug_Info generate_program(std::vector<int16_t> &result,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table);

#endif
