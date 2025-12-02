#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H 1

#include <vector>
#include <string>
#include <map>

/**
 * @brief enum for atom (a.k.a. argument) type
 */
enum Atom_Type {
        LABEL        = 1,
        LITERAL_INT  = 1 << 1,
        LITERAL_STR  = 1 << 2,
        MNEMONIC     = 1 << 3,
        REGISTER     = 1 << 4,
        SOURCE       = 1 << 5, // LITERAL_INT | REGISTER | STACK_OFFSET
        STACK_OFFSET = 1 << 6,
};

// many values between Grammar_Retval and Assembler_Retval are only
// slightly different, because I made the blueprint functions before
// assemble_program. I want to check twice on the same function, but
// can't have the same name used in two different enums, so that's why
// there are almost exactly the same names

/**
 * @brief enum for errors in blueprint functions
 */
enum Grammar_Retval {
        EXPECTED_MNEMONIC,
        ACCEPTABLE,
        INVALID_ATOM,
        MISSING_ARGUMENTS,
        MISSING_EXIT,
        MISSING_MAIN,
        UNKNOWN_LABEL,
};

/**
 * @brief enum for errors in translation functions
 * @details this is deprecated, and basically just duplicates of
 * Grammar_Retval, but im keeping in here for now
 */
enum Assembler_Retval {
        ACCEPTABLE_2,
        EXPECTED_MNEMONIC_2,
        INVALID_ATOM_2,
        UNKNOWN_LABEL_2,
        MISSING_ARGUMENTS_2,
        MISSING_MAIN_2,
};

/**
 * @brief container struct for debugging / error catching in blueprint
 * and translation functions
 */
struct Debug_Info {
        int                     relevant_idx;     ///< any relevant idx
        std::vector<std::string> relevant_tokens;  ///< any relevant tokens
        Grammar_Retval          grammar_retval;   ///< blueprint error
        Assembler_Retval        assembler_retval; ///< translation error
};

/**
 * @brief container struct for pass info through translation tokens
 * @details as of Tue 18.Nov.2025, this has no applications yet
 */
struct Program_Info {
        std::vector<std::string> tokens;             ///< tokens from user program
        std::map<std::string, int16_t> label_table; ///< labels from user program
        std::map<int16_t, int16_t> str_idx_offsets; ///< refer to abi.md
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
