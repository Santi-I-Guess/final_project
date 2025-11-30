#include <vector>
#include <map>
#include <string>

// handle preprocessing of tokens before translation
// also, uses way less references, given I don't want to worry about
// writting implementations that keep error causing token in buffer

#include "../common_values.h"

/**
 * @brief obtains the addresses of user defined labels, and filters labels
 * out of the tokens paramenter
 */
std::map<std::string, int16_t> create_label_map(std::vector<std::string> &tokens);


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

