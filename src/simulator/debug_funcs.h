#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H 1

#include "cpu_handle.h"

// series of functions to be used when -d flag is on

void pdb_handle_delete(
        const std::vector<std::string> cmd_tokens,
        std::vector<int16_t> &breakpoints
);

void pdb_handle_print(
        const std::vector<std::string> cmd_tokens,
        CPU_Handle &cpu_handle
);

/**
 * @brief print interpretation of assembled program, along with addresses
 * @details debugging only function: mostly used for branching instruction
 * debugging. Reason this is not a member method is to keep modules seperate
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

/**
 * @brief prints PAL debugger help when debug flag is on
 */
void print_pdb_help();

/**
 * @brief prints next instruction, in a similar format to interpret_program
 * @details helper function of CPU_Handle::run_program in debug mode
 */
void print_instruction_simple(int16_t *program_data, int16_t prog_ctr);

#endif
