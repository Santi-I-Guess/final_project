#ifndef COMMON_VALUES_H
#define COMMON_VALUES_H 1

#include <vector>
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
        std::vector<std::string> relevant_tokens;
        Grammar_Retval           grammar_retval;
        Assembler_Retval         assembler_retval;
};

// instruction argument types
// leaving mnemonic here so that loop in is_valid_arguments works nicer
const std::map<std::string, std::vector<Atom_Type>> BLUEPRINTS = {
        {"NOP",    {MNEMONIC                        } },
        {"CALL",   {MNEMONIC, LABEL                 } },
        {"RET",    {MNEMONIC                        } },
        {"PRINT",  {MNEMONIC, SOURCE                } },
        {"SPRINT", {MNEMONIC, LITERAL_STR           } },
        {"EXIT",   {MNEMONIC                        } },
        {"READ",   {MNEMONIC, REGISTER, LITERAL_INT } },
        {"WRITE",  {MNEMONIC, SOURCE,   LITERAL_INT } },
        {"POP",    {MNEMONIC, REGISTER              } },
        {"PUSH",   {MNEMONIC, SOURCE                } },
        {"MOV",    {MNEMONIC, REGISTER, SOURCE      } },
        {"ADD",    {MNEMONIC, REGISTER, SOURCE      } },
        {"SUB",    {MNEMONIC, REGISTER, SOURCE      } },
        {"MUL",    {MNEMONIC, REGISTER, SOURCE      } },
        {"DIV",    {MNEMONIC, REGISTER, SOURCE      } },
        {"AND",    {MNEMONIC, REGISTER, SOURCE      } },
        {"OR",     {MNEMONIC, REGISTER, SOURCE      } },
        {"XOR",    {MNEMONIC, REGISTER, SOURCE      } },
        {"LSH",    {MNEMONIC, REGISTER, SOURCE      } },
        {"RSH",    {MNEMONIC, REGISTER, SOURCE      } },
        {"CMP",    {MNEMONIC, SOURCE,   SOURCE      } },
        {"JEQ",    {MNEMONIC, LABEL                 } },
        {"JNE",    {MNEMONIC, LABEL                 } },
        {"JGE",    {MNEMONIC, LABEL                 } },
        {"JGR",    {MNEMONIC, LABEL                 } },
        {"JLE",    {MNEMONIC, LABEL                 } },
        {"JLS",    {MNEMONIC, LABEL                 } },
};

// valid register symbols
const std::map<std::string, int16_t> REGISTER_TABLE = {
        {"RA", 0}, {"RB", 1}, {"RC", 2}, {"RD", 3},
        {"RE", 4}, {"RF", 5}, {"RG", 6}, {"RH", 7},
        {"RSP", 8}, {"RIP", 9}
};

// opcode for each instruction mnemonic
const std::map<std::string, int16_t> OPCODE_TABLE = {
        {"NOP",    0}, {"CALL",  1}, {"RET",   2}, {"PRINT",  3},
        {"SPRINT", 4}, {"EXIT",  5}, {"READ",  6}, {"WRITE",  7},
        {"POP",    8}, {"PUSH",  9}, {"MOV",  10}, {"ADD",   11},
        {"SUB",   12}, {"MUL",  13}, {"DIV",  14}, {"AND",   15},
        {"OR",    16}, {"XOR",  17}, {"LSH",  18}, {"RSH",   19},
        {"CMP",   20}, {"JEQ",  21}, {"JNE",  22}, {"JGE",   23},
        {"JGR",   24}, {"JLE",  25}, {"JLS",  26},
};

#endif
