#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H 1

#include "cpu_handle.h"

// series of functions to be used when -d flag is on

/**
 * @brief handle break command for PAL Debugger
 * @details helper function of CPU_Handle::run_program_debug
 */
void pdb_handle_break(
        const std::vector<std::string> cmd_tokens,
        std::vector<int16_t> &breakpoints,
        const std::vector<int16_t> mnemonic_addrs
);

/**
 * @brief handle delete command for PAL Debugger
 * @details helper function of CPU_Handle::run_program_debug
 */
void pdb_handle_delete(
        const std::vector<std::string> cmd_tokens,
        std::vector<int16_t> &breakpoints
);

/**
 * @brief print interpretation of assembled program, along with addresses
 * @details debugging only function: mostly used for branching instruction
 * debugging. Reason this is not a member method is to keep modules seperate
 */
void pdb_handle_interpret(const CPU_Handle &cpu_handle);

/**
 * @brief handle print command for PAL Debugger
 * @details helper function of CPU_Handle::run_program_debug
 */
void pdb_handle_print(
        const std::vector<std::string> cmd_tokens,
        CPU_Handle &cpu_handle
);

/**
 * @brief prints string from string data
 * @detail helper function of pdb_handle_interpret. arguments come from
 * pdb_handle_interpret
 */
void itrprt_print_chars(
        const int16_t curr_int,
        const int16_t int_idx,
        int16_t &curr_str_idx,
        Program_State_Enum &curr_state
);

/**
 * @brief prints PAL debugger help when debug flag is on
 * @details helper function of CPU_Handle:run_program_debug
 */
void pdb_handle_help();

/**
 * @brief prints next instruction, in a similar format to pdb_handle_interpret
 * @details helper function of CPU_Handle::run_program in debug mode
 */
void itrprt_print_instruction(
        const int16_t *program_data,
        const int16_t prog_ct
);

#endif
