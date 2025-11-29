#ifndef CMD_LINE_OPTS_H
#define CMD_LINE_OPTS_H 1

#include <cstdint>
#include <deque>
#include <string>
#include <map>

/**
 * @brief container for cmd line inputs and flags
 */
struct Cmd_Options {
        bool assemble_only;         ///< -c
        bool executable_help;       ///< -h
        int input_file_idx;         ///< init to -1
        bool intermediate_files;    ///< -s
        bool is_binary_input;       ///< -b
        bool is_debug;         ///< -d

        Cmd_Options();
        bool handle_cmd_args(int argc, char **argv); ///< stores flags
};


/**
 * @brief prints usage info for executable
 */
void print_help();

#endif
