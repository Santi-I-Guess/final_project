#include <cstdint>
#include <map>
#include <string>
#include <vector>

// handle preprocessing of tokens before translation

#include "../common_values.h"
#include "tokenizer.h"

enum Grammar_Retval_2 {
        EXPECTED_MNEMONIC_E,
        ACCEPTABLE_E,
        INVALID_ATOM_E,
        MISSING_ARGUMENTS_E,
        MISSING_EXIT_E,
        MISSING_MAIN_E,
        UNKNOWN_LABEL_E,
        UNKNOWN_MNEMONIC_E,
};

struct Debug_Info_2 {
        int line_num;
        Grammar_Retval_2 grammar_retval;
        Token relevant_token;
};

/**
 * @brief obtains the addresses of user defined labels
 */
std::map<std::string, int16_t> create_label_map(
        const std::vector<Token> tokens
);

/**
 * @brief does basic grammar checking of program
 * @details checks for unknown mnemonics, invalid atoms, missing arguments,
 * missing EXIT program, missing main label, or calling an undefined label
 */
Debug_Info_2 grammar_check_2(
        const std::vector<Token> tokens,
        const std::map<std::string, int16_t> label_map
);

//---------------------------------------------------------------------------

/**
 * @brief checks if a token matches the expected atom type
 * @details helper function of grammar_check
 */
bool is_valid_atom(const Atom_Type atom_type, const std::string token);

/**
 * @brief checks if string is within bounds of INT16_MAX and INT16_MIN
 * @details helper function of is_valid_atom
 */
bool is_valid_i16(const std::string token);

/**
 * @brief does basic grammar checking of program
 * @details checks for unknown mnemonics, invalid atoms, missing arguments,
 * missing EXIT program, missing main label, or calling an undefined label
 */
Debug_Info grammar_check(
        const std::vector<std::string> tokens,
        const std::map<std::string, int16_t> label_table
);

