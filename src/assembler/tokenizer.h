#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../common_values.h"

/**
 * @brief defines possible grammar errors
 */
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

/**
 * @brief holds relevant debug information for erroneous user program
 * @details helper struct for grammar_check
 */
struct Debug_Info {
        int line_num;
        Grammar_Retval grammar_retval;
        Token relevant_token;
};

/**
 * @brief obtains the addresses of user defined labels
 */
std::map<std::string, int16_t> create_label_map(
        const std::vector<Token> &tokens
);

/**
 * @brief tokenizes the user input, and associates types to each token
 */
std::vector<Token> create_tokens(const std::string &source_buffer);

/**
 * @brief does basic grammar checking of program
 */
Debug_Info grammar_check(
        const std::vector<Token> &tokens,
        const std::map<std::string, int16_t> &label_map
);

#endif
