#ifndef SYNTHESIS_H
#define SYNTHESIS_H 1

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../common_values.h"

enum Grammar_Retval {
        ACCEPTABLE_E,
        EXPECTED_MNEMONIC_E,
        INVALID_ATOM_E,
        MISSING_ARGUMENTS_E,
        MISSING_EXIT_E,
        MISSING_MAIN_E,
        UNKNOWN_LABEL_E,
        UNKNOWN_MNEMONIC_E,
};

struct Debug_Info {
        int line_num;
        Grammar_Retval grammar_retval;
        Token relevant_token;
};

/**
 * @brief assembles the final program into a vector of int16_t's
 */
std::vector<int16_t> assemble_program(
        const std::vector<Token> tokens,
        const std::map<std::string, int16_t> label_map
);

/**
 * @brief obtains the addresses of user defined labels
 */
std::map<std::string, int16_t> create_label_map(
        const std::vector<Token> tokens
);

/**
 * @brief tokenizes the user input, and associates types to each token
 */
std::vector<Token> create_tokens(const std::string source_buffer);

/**
 * @brief does basic grammar checking of program
 */
Debug_Info grammar_check(
        const std::vector<Token> tokens,
        const std::map<std::string, int16_t> label_map
);

/**
 * @brief translates a single string into series of int16_t's with a null int16_t
 * @details helper function of assemble_program
 */
std::vector<int16_t> translate_string(const std::string stripped_token);

#endif
