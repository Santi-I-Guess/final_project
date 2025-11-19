#ifndef TRANSLATION_H
#define TRANSLATION_H 1

#include "common_values.h"

#include <cstdint>
#include <deque>
#include <map>

/**
 * @brief generates a single token into one int16_t
 * @details helper function of generate_program
 */
int16_t translate_token(std::string token, Atom_Type atom_type,
                        std::map<std::string, int16_t> label_table);

/**
 * @brief generates a string token into a variable number of int16_t's
 * @details helper function of generate_program
 */
std::deque<int16_t> translate_string(std::string token, int16_t &program_idx);

/**
 * @brief assembles the final program into a deque of int16_t's
 */
Debug_Info generate_program(std::deque<int16_t> &result,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table);

#endif
