#ifndef CMD_LINE_OPTS_H
#define CMD_LINE_OPTS_H 1

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <deque>

// handles cmd line inputs and flags
struct Cmd_Options {
        bool compile_only;          // -c
        bool executable_help;       // -h
        int input_file_idx;         // init to -1
        bool intermediate_files;    // -s
        bool is_binary_input;       // -b
        bool line_by_line_printing; // -p

        Cmd_Options();
        // stores flags, makes sure incompatible flags cause early return
        bool handle_cmd_args(int argc, char **argv);
};

// prints usage info for executable
void print_help();

#endif
