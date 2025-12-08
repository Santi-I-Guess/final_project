#ifndef ASSEMBLER_H
#define ASSEMBLER_H 1

#include "../common_values.h"

#include <cstdint>
#include <map>
#include <vector>

/**
 * @brief assembles the final program into a vector of int16_t's
 */
std::vector<int16_t> assemble_program(
        const std::vector<Token> &tokens,
        const std::map<std::string, int16_t> &label_map
);

/**
 * @brief translates a single string into series of int16_t's with a null int16_t
 * @details helper function of assemble_program
 */
std::vector<int16_t> translate_string(const std::string &stripped_token);

#endif
