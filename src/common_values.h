#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H 1

#include <vector>
#include <string>
#include <map>

// stack size last 1/4 of the RAM
#define RAM_SIZE 3000
#define STACK_START 2250
#define STACK_SIZE 750
#define CALL_STACK_SIZE 2014

/**
 * @brief enum for atom (a.k.a. argument) type
 */
enum Atom_Type {
        LABEL,
        LITERAL_INT,
        LITERAL_STR,
        MNEMONIC,
        REGISTER,
        SOURCE,       // LITERAL_INT | REGISTER | STACK_OFFSET
        STACK_OFFSET,
};

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
        int line_num;     ///< line number from original input
        Token_Type type;  ///< type of the token
        std::string data; ///< the data
};

// it would be great if there was a structure, where one instance maps to
//      one instruction, and all information for the instruction was
//      packed into its instance

/**
 * @brief hashmap that defines arguments of instructions in assembly language
 * @details even though every instruction needs a mnemonic, it's left in here
 * so that the inner loop of assemble_program doesn't need to have extra
 * logic for the first argument (i == 0)
 */
const std::map<std::string, std::vector<Atom_Type>> BLUEPRINTS = {
        {"NOP",    {MNEMONIC                          } },
        {"MOV",    {MNEMONIC, REGISTER, SOURCE        } },
        {"INC",    {MNEMONIC, REGISTER                } },
        {"DEC",    {MNEMONIC, REGISTER                } },
        {"ADD",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"SUB",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"MUL",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"DIV",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"MOD",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"AND",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"OR",     {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"NOT",    {MNEMONIC, REGISTER, SOURCE        } },
        {"XOR",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"LSH",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"RSH",    {MNEMONIC, REGISTER, SOURCE, SOURCE} },
        {"CMP",    {MNEMONIC, SOURCE,   SOURCE        } },
        {"JMP",    {MNEMONIC, LABEL                   } },
        {"JEQ",    {MNEMONIC, LABEL                   } },
        {"JNE",    {MNEMONIC, LABEL                   } },
        {"JGE",    {MNEMONIC, LABEL                   } },
        {"JGR",    {MNEMONIC, LABEL                   } },
        {"JLE",    {MNEMONIC, LABEL                   } },
        {"JLS",    {MNEMONIC, LABEL                   } },
        {"CALL",   {MNEMONIC, LABEL                   } },
        {"RET",    {MNEMONIC                          } },
        {"PUSH",   {MNEMONIC, SOURCE                  } },
        {"POP",    {MNEMONIC, REGISTER                } },
        {"WRITE",  {MNEMONIC, SOURCE,   SOURCE        } },
        {"READ",   {MNEMONIC, REGISTER, SOURCE        } },
        {"PRINT",  {MNEMONIC, SOURCE                  } },
        {"SPRINT", {MNEMONIC, LITERAL_STR             } },
        {"CPRINT", {MNEMONIC, SOURCE                  } },
        {"INPUT",  {MNEMONIC,                         } },
        {"EXIT",   {MNEMONIC,                         } },
};


/**
 * @brief hashmap for valid opcodes for each mnemoinc in assembly language
 */
const std::map<std::string, int16_t> OPCODE_TABLE = {
        {"NOP",    0}, {"MOV",    1}, {"INC",     2}, {"DEC",     3},
        {"ADD",    4}, {"SUB",    5}, {"MUL",     6}, {"DIV",     7},
        {"MOD",    8}, {"AND",    9}, {"OR",     10}, {"NOT",    11},
        {"XOR",   12}, {"LSH",   13}, {"RSH",    14}, {"CMP",    15},
        {"JMP",   16}, {"JEQ",   17}, {"JNE",    18}, {"JGE",    19},
        {"JGR",   20}, {"JLE",   21}, {"JLS",    22}, {"CALL",   23},
        {"RET",   24}, {"PUSH",  25}, {"POP",    26}, {"WRITE",  27},
        {"READ",  28}, {"PRINT", 29}, {"SPRINT", 30}, {"CPRINT", 31},
        {"INPUT", 32}, {"EXIT",  33},
};

/**
 * @brief table for lengths, in opcode order, for instruction functions
 */
const int16_t INSTRUCTION_LENS[34] = {
        1, 3, 2, 2,
        4, 4, 4, 4,
        4, 4, 4, 3,
        4, 4, 4, 3,
        2, 2, 2, 2,
        2, 2, 2, 2,
        1, 2, 2, 3,
        3, 2, 2, 2,
        1, 1
};

/**
 * @brief dereference array for CPU_Handle::interpret_program()
 * @details opposite to OPCODE_TABLE
 */
const std::string DEREFERENCE_TABLE[34] = {
        "NOP",   "MOV",   "INC",    "DEC",
        "ADD",   "SUB",   "MUL",    "DIV",
        "MOD",   "AND",   "OR",     "NOT",
        "XOR",   "LSH",   "RSH",    "CMP",
        "JMP",   "JEQ",   "JNE",    "JGE",
        "JGR",   "JLE",   "JLS",    "CALL",
        "RET",   "PUSH",  "POP",    "WRITE",
        "READ",  "PRINT", "SPRINT", "CPRINT",
        "INPUT", "EXIT"
};

/**
 * @brief hashmap for valid callable registers in assembly language
 */
const std::map<std::string, int16_t> REGISTER_TABLE = {
        {"RA",  0}, {"RB",  1}, {"RC",    2}, {"RD",    3},
        {"RE",  4}, {"RF",  5}, {"RG",    6}, {"RH",    7},
        {"RSP", 8}, {"RIP", 9}, {"CMP0", 10}, {"CMP1", 11}
};

#endif
