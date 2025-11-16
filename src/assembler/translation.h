#ifndef TRANSLATION_H
#define TRANSLATION_H 1

#include "common_values.h"

#include <cstdint>
#include <deque>
#include <map>
#include <vector>

// generates single token
int16_t translate_token(std::string token, Atom_Type atom_type,
                        std::map<std::string, int16_t> label_table);

// generates string bytes
std::deque<int16_t> translate_string(std::string token, int16_t &program_idx);

// generates the final program
Debug_Info generate_program(std::vector<int16_t> &result,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table);

#endif
