#include <iostream>
#include <string>

#include "cmd_line_opts.h"

Cmd_Options::Cmd_Options() {
        assemble_only         = false;
        executable_help       = false;
        input_file_idx        = -1;
        is_binary_input       = false;
        intermediate_files    = false;
        debug_options         = false;
}

/* auxiliary function to handle command line arguments
 misc: doesn't rust's cargo have a package for cmd parsing? */
bool Cmd_Options::handle_cmd_args(int argc, char **argv) {
        if (argc < 2)
                return true;
        for (int i = 1; i < argc; ++i) {
                std::string curr_arg = argv[i];
                if (curr_arg == "-a" || curr_arg == "--assemble-only") {
                        assemble_only = true;
                } else if (curr_arg == "-b" || curr_arg == "--binary-input") {
                        is_binary_input = true;
                } else if (curr_arg == "-h" || curr_arg == "--help") {
                        executable_help = true;
                } else if (curr_arg == "-s" || curr_arg == "--save-temps") {
                        intermediate_files = true;
                } else if (curr_arg == "-d" || curr_arg == "--debug") {
                        debug_options = true;
                } else if (curr_arg[0] == '-') {
                        std::cout << "Unrecognized option: " << curr_arg << "\n";
                        return false;
                } else {
                        input_file_idx = i;
                }
        }
        // combination of flags that are either incompatible or
        // cause an early return
        if (executable_help) {
                print_help();
                return false;
        } else if (is_binary_input && (assemble_only)) {
                std::cout << "Binary input is already assembled\n";
                return false;
        }
        return true;
}

// misc: man page style: see cbonsai tool
void print_help() {
        std::string help_buffer = 
        "Usage: final_project \x1b[4minput_file\x1b[0m [OPTION]\n\n"
        "final_project is a pseudo assembly simulator for education purposes\n\n"
        "Options:\n"
        "  -a, --assemble-only\n"
        "      assemble ascii source file into a binary file, and quit.\n\n"
        "  -b, --binary-input\n"
        "      use a preassembled binary file instead of a ascii source file\n\n"
        "  -d, --debug\n"
        "      enable debug symbols for interpreting and running the program\n\n"
        "  -h, --help\n"
        "      show this help screen\n\n"
        "  -s, --save-temps\n"
        "      create and run ascii source file as normal, but also create \n"
        "      intermediate ascii files for tokenizer and label table.\n\n";
        std::cout << help_buffer;
}
