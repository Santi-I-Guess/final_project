#include <iostream>
#include <string>

#include "cmd_line_opts.h"

Cmd_Options::Cmd_Options() {
        assemble_only         = false;
        executable_help       = false;
        input_file_idx        = -1;
        intermediate_files    = false;
        is_binary_input       = false;
        is_debug              = false;
        is_stdin              = false;
        test_only             = false;
}

/* auxiliary function to handle command line arguments
 misc: doesn't rust's cargo have a package for cmd parsing? */
void Cmd_Options::store_cmd_args(const int argc, char ** const argv) {
        if (argc < 2)
                return;
        for (int i = 1; i < argc; ++i) {
                std::string curr_arg = argv[i];
                if (curr_arg == "-a" || curr_arg == "--assemble-only") 
                        assemble_only = true;
                else if (curr_arg == "-b" || curr_arg == "--binary-input") 
                        is_binary_input = true;
                else if (curr_arg == "-h" || curr_arg == "--help") 
                        executable_help = true;
                else if (curr_arg == "-s" || curr_arg == "--save-temps") 
                        intermediate_files = true;
                else if (curr_arg == "-S" || curr_arg == "--use-stdin") 
                        is_stdin = true;
                else if (curr_arg == "-d" || curr_arg == "--debug") 
                        is_debug = true;
                else if (curr_arg == "-t" || curr_arg == "--test-only") 
                        test_only = true;
                else if (curr_arg[0] == '-') 
                        std::cout << "Unrecognized option: " << curr_arg << "\n";
                else 
                        input_file_idx = i;
        }
}

bool Cmd_Options::is_valid_args() {
        // combination of flags that are either incompatible or
        // cause an early return
        if (executable_help) {
                print_help();
                return false;
        } else if (assemble_only && is_binary_input) {
                std::cout << "Flag Error: Binary input is redundant, and will";
                std::cout << "not be regenerated with --assemble-only\n";
                return false;
        } else if (intermediate_files && is_binary_input) {
                std::cout << "Flag Error: No intermediate files are generated";
                std::cout << " with pre-assembled input\n";
                return false;
        } else if (is_binary_input && test_only) {
                std::cout << "Flag Error: Binary input is redundant, and will";
                std::cout << "not be ran with --test-only\n";
                return false;
        } else if (is_binary_input && is_stdin) {
                std::cout << "Flag Error: Cannot accept binary file input and";
                std::cout << "stdin input in the same command call\n";
                return false;
        } else if (!is_stdin && input_file_idx == -1) {
                std::cout << "Flag Warning: Did not provide an input file, ";
                std::cout << "and --use-stdin is not flagged.\nIf you are a first ";
                std::cout << "time user, run with -h or --help for usage\n";
                return false;
        }
        return true;
}

// misc: man page style: see cbonsai tool
void print_help() {
        std::string help_buffer = 
        "Usage: final_project \x1b[4minput_file\x1b[0m [OPTION]\n\n"
        "final_project is a pseudo assembly assembler, simulator, and debugger\n"
        "for educational purposes. It is designed for use with the pseudo \n"
        "assembly language (PAL), and comes with a dedicated debugger (PAL debugger).\n"
        "By default, final_project will assemble and simulate the program in one step\n\n"
        "Options:\n"
        "  -a, --assemble-only\n"
        "      assemble ascii source file (or stdin when used with -S) into a binary file, and quit.\n\n"
        "  -b, --binary-input\n"
        "      use a preassembled binary file instead of a ascii source file\n\n"
        "  -d, --debug\n"
        "      enable PAL debugger (pdb) when running user program\n\n"
        "  -h, --help\n"
        "      show this help screen\n\n"
        "  -s, --save-temps\n"
        "      create intermediate ascii files for tokenizer and label table.\n\n"
        "  -S, --use-stdin\n"
        "      enter the program via the terminal instead of with an ascii or binary file.\n"
        "      stdin will continue to be read until an empty line is found\n\n"
        "  -t, --test-only\n"
        "      go through the assembler process, but do not simulate the"
              "program.\n    most useful with -s, or to experiment with errors\n\n"
        "For a tutorial, read docs/tutorial.md\n"
        "For examples, see examples folder or testing/testing_suite.sh\n\n";
        std::cout << help_buffer;
}
