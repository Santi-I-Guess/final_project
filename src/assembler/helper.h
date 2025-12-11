#ifndef HELPER_H
#define HELPER_H 1

#include <string>

#include "../instruction_types.h"

/**
 * @brief checks if i matches equivalent regex pattern: [$%|a-zA-Z0-9_-]
 * @details create_tokens helper function
 */
bool is_identifier_char(const char i);

/**
 * @brief checks if a token matches the expected atom type
 * @details helper function of grammar_check
 */
bool is_valid_atom(const Atom_Type atom_type, const std::string &token);

/**
 * @brief checks if string is within bounds of INT16_MAX and INT16_MIN
 * @details helper function of is_valid_atom
 */
bool is_valid_i16(const std::string &token);

#endif
