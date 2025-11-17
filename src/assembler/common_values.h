#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H 1

#include <deque>
#include <string>
#include <map>

// instruction argument types
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
// generate_program. I want to check twice on the same function, but
// can't have the same name used in two different enums, so that's why
// there are almost exactly the same names

// return value for blueprint functions
enum Grammar_Retval {
        EXPECTED_MNEMONIC,
        ACCEPTABLE,
        INVALID_ATOM,
        MISSING_ARGUMENTS,
        MISSING_EXIT,
        MISSING_MAIN,
        UNKNOWN_LABEL,
};

// return value for translation functions
enum Assembler_Retval {
        ACCEPTABLE_2,
        EXPECTED_MNEMONIC_2,
        UNKNOWN_LABEL_2,
        MISSING_ARGUMENTS_2,
        MISSING_MAIN_2,
};

// to pass debug info through syntax checking functions
class Debug_Info {
public:
        int                      relevant_idx;
        std::deque<std::string> relevant_tokens;
        Grammar_Retval           grammar_retval;
        Assembler_Retval         assembler_retval;
};

// instruction argument types
// leaving mnemonic here so that loop in is_valid_arguments works nicer
const std::map<std::string, std::deque<Atom_Type>> BLUEPRINTS = {
        {"NOP",    {MNEMONIC                                } },
        {"MOV",    {MNEMONIC, REGISTER, SOURCE              } },
        {"ADD",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"SUB",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"MUL",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"DIV",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"AND",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"OR",     {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"XOR",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"LSH",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"RSH",    {MNEMONIC, REGISTER, SOURCE, SOURCE      } },
        {"CMP",    {MNEMONIC, SOURCE,   SOURCE              } },
        {"JEQ",    {MNEMONIC, LABEL                         } },
        {"JNE",    {MNEMONIC, LABEL                         } },
        {"JGE",    {MNEMONIC, LABEL                         } },
        {"JGR",    {MNEMONIC, LABEL                         } },
        {"JLE",    {MNEMONIC, LABEL                         } },
        {"JLS",    {MNEMONIC, LABEL                         } },
        {"CALL",   {MNEMONIC, LABEL                         } },
        {"RET",    {MNEMONIC                                } },
        {"PUSH",   {MNEMONIC, SOURCE                        } },
        {"POP",    {MNEMONIC, REGISTER                      } },
        {"WRITE",  {MNEMONIC, SOURCE,   LITERAL_INT         } },
        {"READ",   {MNEMONIC, REGISTER, LITERAL_INT         } },
        {"PRINT",  {MNEMONIC, SOURCE                        } },
        {"SPRINT", {MNEMONIC, LITERAL_STR                   } },
        {"EXIT",   {MNEMONIC                                } },
};

// valid register symbols
const std::map<std::string, int16_t> REGISTER_TABLE = {
        {"RA",  0}, {"RB",  1}, {"RC", 2}, {"RD", 3},
        {"RE",  4}, {"RF",  5}, {"RG", 6}, {"RH", 7},
        {"RSP", 8}, {"RIP", 9}
};

// opcode for each instruction mnemonic
const std::map<std::string, int16_t> OPCODE_TABLE = {
        {"NOP",    0}, {"MOV",     1}, {"ADD",    2}, {"SUB",   3},
        {"MUL",    4}, {"DIV",     5}, {"AND",    6}, {"OR",    7},
        {"XOR",    8}, {"LSH",     9}, {"RSH",   10}, {"CMP",  11},
        {"JEQ",   12}, {"JNE",    13}, {"JGE",   14}, {"JGR",  15},
        {"JLE",   16}, {"JLS",    17}, {"CALL",  18}, {"RET",  19},
        {"PUSH",  20}, {"POP",    21}, {"WRITE", 22}, {"READ", 23},
        {"PRINT", 24}, {"SPRINT", 25}, {"EXIT",  26},
};

#endif
