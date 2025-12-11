#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H 1

#include <string>

/**
 * @brief enum for atom (a.k.a. argument) type
 */
enum Atom_Type {
        LABEL,
        LITERAL_INT,
        LITERAL_STR,
        MNEMONIC,
        RAM_ADDR,
        REGISTER,
        SOURCE,       // LITERAL_INT | REGISTER | STACK_OFFSET | RAM_ADDR
        STACK_OFFSET,
};

/**
 * @brief defines possible token types
 */
enum Token_Type {
        T_INTEGER_LIT,
        T_LABEL_DEF,
        T_LABEL_REF,
        T_MNEMONIC,
        T_RAM_ADDR,
        T_REGISTER,
        T_STACK_OFF,
        T_STRING_LIT,
};

/**
 * @brief struct that associates original line number and token type
 * to a string token
 */
struct Token {
        int line_num;     ///< line number from original input
        Token_Type type;  ///< type of the token
        std::string data; ///< the data
};

#endif
