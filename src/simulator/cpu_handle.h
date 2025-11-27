#ifndef CPU_HANDLE_H
#define CPU_HANDLE_H 1

#include <cstdint>
#include <deque>
#include <map>
#include <string>

/**
 * @brief describes current state of program
 * @details is also used in program interpretation
 */
enum Program_State_Enum {
        NONE,
        READING_ENTRY_LABEL,
        READING_STR,
        READING_MNEMONIC,
};

/**
 * @brief dereference hashmap for CPU_Handle::interpret_program()
 * @details opposite hashmap to OPCODE_TABLE in common_values.h
 */
const std::map<int16_t, std::string> DEREFERENCE_TABLE = {
        {0,    "NOP"}, {1,     "MOV"}, {2,   "INC"},
        {3,    "DEC"}, {4,     "ADD"}, {5,   "SUB"},
        {6,    "MUL"}, {7,     "DIV"}, {8,   "AND"},
        {9,     "OR"}, {10,    "NOT"}, {11,  "XOR"},
        {12,   "LSH"}, {13,    "RSH"}, {14,  "CMP"},
        {15,   "JEQ"}, {16,    "JNE"}, {17,  "JGE"},
        {18,   "JGR"}, {19,    "JLE"}, {20,  "JLS"},
        {21,  "CALL"}, {22,    "RET"}, {23, "PUSH"},
        {24,   "POP"}, {25,  "WRITE"}, {26, "READ"},
        {27, "PRINT"}, {28, "SPRINT"}, {29, "EXIT"},
};

/**
 * @brief Container class for memory during program simulation
 */
class CPU_Handle {
        // general purpose registers
        int16_t reg_a; /** general purpose register */
        int16_t reg_b; /** general purpose register */
        int16_t reg_c; /** general purpose register */
        int16_t reg_d; /** general purpose register */
        int16_t reg_e; /** general purpose register */
        int16_t reg_f; /** general purpose register */
        int16_t reg_g; /** general purpose register */
        int16_t reg_h; /** general purpose register */
        int16_t reg_cmp_a; /** comparison register, inaccessable */
        int16_t reg_cmp_b; /** comparison register, inaccessable */
        int16_t prog_ctr; /** program counter / instruction pointer */
        int16_t stack_ptr; /** stack counter / pointer */
        int16_t program_mem[2048]; /** holds ram and stack memory */
public:
        int16_t *program_data; /** assembled program */
        int16_t prog_size; /** size of program data */
        CPU_Handle();
        ~CPU_Handle();
        void load_program(const std::deque<int16_t> given_program);
};

/**
 * @fn void CPU_Handle::load_program(const std::deque<int16_t> given_program)
 * @brief loads elements of given_program to program_data
 * @details also allocates program_data member to have enough space to load
 * given_data, since string data may be large
 */

#endif
