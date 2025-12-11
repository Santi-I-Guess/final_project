#ifndef PAL_DEBUGGER_H
#define PAL_DEBUGGER_H 1

#include <cstdint>
#include <string>
#include <vector>

#include "cpu_handle.h"

// series of functions for the Pal Debugger

/**
 * @brief handle break command for PAL Debugger
 * @details helper function of CPU_Handle::run_program_debug
 */
void pdb_handle_break(
        const std::vector<std::string> &cmd_tokens,
        std::vector<int16_t> &breakpoints,
        const std::vector<int16_t> &mnemonic_addrs
);

/**
 * @brief handle delete command for PAL Debugger
 * @details helper function of CPU_Handle::run_program_debug
 */
void pdb_handle_delete(
        const std::vector<std::string> &cmd_tokens,
        std::vector<int16_t> &breakpoints
);

/**
 * @brief prints PAL debugger help when debug flag is on
 * @details helper function of CPU_Handle:run_program_debug
 */
void pdb_handle_help();

/**
 * @brief print disassembled program, along with addresses
 * @details debugging only function: mostly used for branching instruction
 * debugging. Reason this is not a member method is to keep modules seperate
 */
void pdb_handle_disassemble(const CPU_Handle &cpu_handle);

/**
 * @brief handle print command for PAL Debugger
 * @details helper function of CPU_Handle::run_program_debug
 */
void pdb_handle_print(
        const std::vector<std::string> &cmd_tokens,
        CPU_Handle &cpu_handle
);

/**
 * @brief prints string from string data
 * @detail helper function of pdb_handle_disassemble. arguments come from
 * pdb_handle_disassemble
 */
void disassemble_print_chars(
        const int16_t &curr_int,
        const int16_t &int_idx,
        int16_t &curr_str_idx,
        Program_State_Enum &curr_state
);

/**
 * @brief prints next instruction, in a similar format to pdb_handle_disassemble
 * @details helper function of CPU_Handle::run_program in debug mode
 */
void disassemble_print_instruction(
        const std::vector<int16_t> &instruction,
        const int16_t &prog_ctr
);

#endif
