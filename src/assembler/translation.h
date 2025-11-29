#ifndef TRANSLATION_H
#define TRANSLATION_H 1

#include "../common_values.h"

#include <cstdint>
#include <deque>
#include <map>

/**
 * @brief assembles the final program into a deque of int16_t's
 */
Debug_Info assemble_program(
        std::deque<int16_t> &program,
        Program_Info program_info
);

/**
 * @brief check if token expected to be mapped is valid
 * @details helper function of assemble_program
 */
bool is_valid_key(
        std::string token,
        Atom_Type atom_type,
        Program_Info program_info
);

/**
 * @brief translates a single string into series of int16_t's with a null int16_t
 * @details helper function of assemble_program
 */
std::deque<int16_t> translate_string(
        std::string stripped_token
);

/**
 * @brief translates a single token into an int16_t
 * @details helper function of assemble_program
 */
int16_t translate_token(
        std::string token,
        Atom_Type atom_type,
        Program_Info program_info,
        int16_t num_seen_strs,
        int16_t entry_offset
);

#endif
