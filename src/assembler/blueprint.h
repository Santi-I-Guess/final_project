#include <deque>
#include <map>
#include <string>

// handle preprocessing of tokens before int16_t preprocessing
// also, uses way less references, given I don't want to worry about
// writting implementations that keep error causing token in buffer

#include "common_values.h"

// define addresses of program labels
// filters out label definition tokens
std::map<std::string, int16_t> define_labels(std::deque<std::string> &tokens);

// helper function of is_valid_atom
// check if in bounds of INT16_MAX and INT16_MIN
bool is_valid_i16(std::string token);

// helper function of is_valid_arguments
bool is_valid_atom(Atom_Type atom_type, std::string token);

// first program syntax check function
Debug_Info is_valid_arguments(std::deque<std::string> tokens,
                              std::map<std::string, int16_t> label_table);
