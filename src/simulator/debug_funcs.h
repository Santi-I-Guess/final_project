#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H 1

#include "cpu_handle.h"

// series of functions to be used when -d flag is on

/**
 * @brief print interpretation of assembled program, along with addresses
 * @details debugging only function: mostly used for branching instruction
 * debugging. Note to self: if this is not a reference, then it frees the
 * program data twice.
 */
void interpret_program(const CPU_Handle &cpu_handle);

/**
 * @brief prints string from string data
 * @detail helper function of interpret_program. arguments come from
 * interpret_program
 */
void print_chars(
        int16_t curr,
        int16_t &curr_str_idx,
        int16_t &int_idx,
        Program_State_Enum &curr_state
);

/**
 * @brief prints mnemonic and arguments
 * @detail helper function of interpret_program. arguments come from
 */
void print_instruction(
        int16_t curr,
        int16_t &mnemonic_idx,
        int16_t &int_idx,
        int16_t *program_data
);

#endif
