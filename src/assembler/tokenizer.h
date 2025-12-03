#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstddef>
#include <vector>
#include <string>

enum Token_Type {
        T_INTEGER_LIT,
        T_LABEL_DEF,
        T_LABEL_REF,
        T_MNEMONIC,
        T_REGISTER,
        T_STACK_OFF,
        T_STRING_LIT,
};

struct Token {
        std::string data; ///< the data
        int line_num;  ///< line number from original input
        Token_Type type;  ///< type of the token
};

/**
 * @brief tokenizes the user input, and associates types to each token
 */
std::vector<Token> create_tokens(const std::string source_buffer);

/**
 * @brief checks if i matches equivalent regex pattern: [$%|a-zA-Z0-9_-]
 * @details create_tokens helper function
 */
bool is_identifier_char(const char i);

#endif
