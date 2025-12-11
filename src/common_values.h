#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H 1

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// stack size last 1/4 of the RAM
#define RAM_SIZE          8192
#define STACK_START       6144
#define STACK_SIZE        2048
#define CALL_STACK_SIZE   2048
#define LIT_MIN_VALUE   -16383
#define LIT_MAX_VALUE    16383

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


/**
 * @brief stores all relevant information of an instruction in one place
 * @details helper struct for BLUEPRINTS
 */
struct Instruction_Data {
        int16_t opcode;
        std::string mnem_name;
        std::vector<Atom_Type> blueprint;
        size_t length;
        Instruction_Data() = default;
        Instruction_Data(int16_t given_opcode, std::string given_mnem_name, std::vector<Atom_Type> given_blueprint);
};

/**
 * @brief hashmap that defines template of instructions in assembly language
 */
std::map<std::string, Instruction_Data> BLUEPRINTS;

/**
 * @brief hashmap for valid callable registers in assembly language
 */
const std::map<std::string, int16_t> REGISTER_TABLE = {
        {"RZ",   0}, {"RA",  1}, {"RB",    2}, {"RC",    3},
        {"RD",   4}, {"RE",  5}, {"RF",    6}, {"RG",    7},
        {"RH",   8}, {"RSP", 9}, {"RIP",  10}, {"CMP0", 11},
        {"CMP1", 12}
};


Instruction_Data get_instruction(const int16_t &opcode);

std::string get_mnem_name(const int16_t &opcode);

int16_t get_opcode(const std::string &mnem_name);

#endif
