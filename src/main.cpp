/* CSCI 200: Final Project: Pseudo Assembly Simulator
 *
 * Author: Santiago Sagastegui
 *
 * Pseudo assembly simulator for very simplified ISA. Also offers debug
 * flags, intermediate step files, and line-by-line printing.
 * Also hosted at https://github.com/Santi-I-Guess/final_project
 */

#include <fstream>
#include <deque>
#include <iostream>
#include <random>
#include <string>

#include "assembler/blueprint.h"
#include "assembler/common_values.h"
#include "assembler/tokenizer.h"
// #include "assembler/translation.h"
#include "misc/cmd_line_opts.h"
#include "misc/file_handling.h"

// every subdirectory of src is isolated in dependencies and function
// make tests eventually

int main(int argc, char **argv) {
        Cmd_Options life_opts;
        bool valid_cmd_arg_combo = life_opts.handle_cmd_args(argc, argv);
        if (!valid_cmd_arg_combo)
                return 0;

        // produce intermediate file header, if needed
        // makes doing multiple tests in a row much easier
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution rand_letter(0, 25);
        std::string file_header = "";
        for (int i = 0; i < 10; ++i) {
                // changes of wrongful truncation are 1 : 26^10
                char curr_letter;
                int letter_idx = rand_letter(mt);
                curr_letter = (char)letter_idx + 'a';
                file_header += curr_letter;
        }

        // put assembled program here, so that assembler.h doesn't require
        // cpu_handle
        std::vector<int16_t> final_program = {};

        std::string source_path = argv[life_opts.input_file_idx];
        if (life_opts.is_binary_input) {
                // populate_program_from_binary(cpu_handle, source_path);
        } else {
                std::ifstream source_file(source_path);
                if (source_file.fail()) {
                        std::cerr << "Failed to open input file\n";
                        return 1;
                }
                std::string source_buffer = read_file_to_buffer(source_file);
                std::deque<std::string> tokens = tokenize_buffer(source_buffer);
                tokens.shrink_to_fit();
                if (life_opts.intermediate_files) {
                        bool res_temp = write_tokens_to_sink(tokens, file_header);
                        if (!res_temp) {
                                std::cerr << "Failed to open token sink file\n";
                                return 1;
                        }
                }
                // also removes label definitions from tokens
                std::map<std::string, int16_t> label_table = define_labels(tokens);
                if (life_opts.intermediate_files) {
                        bool res_temp = write_labels_to_sink(label_table, file_header);
                        if (!res_temp) {
                                std::cerr << "Failed to open label sink file\n";
                                return 1;
                        }
                }
                Debug_Info res = is_valid_arguments(tokens);
                switch (res.grammar_retval) {
                case GOOD:
                        break;
                case EXPECTED_MNEMONIC:
                        std::cout << "--- Grammar Error ---\n";
                        std::cout << "Expected mnemonic\n";
                        std::cout << "Instruction #" << res.relevant_idx << "\n";
                        std::cout << "Actual Symbol: " << res.relevant_tokens.at(0) << "\n";
                        return 0;
                case MISSING_ARGUMENTS:
                        std::cout << "--- Grammar Error ---\n";
                        std::cout << "Missing arguments\n";
                        std::cout << "Instruction #" << res.relevant_idx << "\n";
                        std::cout << "Mnemonic: " << res.relevant_tokens.at(0) << "\n";
                        return 0;
                case INVALID_ATOM:
                        std::cout << "--- Grammar Error ---\n";
                        std::cout << "Invalid atom\n";
                        std::cout << "Instruction #" << res.relevant_idx << "\n";
                        std::cout << "Mnemonic: " << res.relevant_tokens.at(0) << "\n";
                        std::cout << "Invalid symbol: " << res.relevant_tokens.at(1) << "\n";
                        return 0;
                }
                /*
                res = generate_program(final_program, tokens, label_table);
                switch (res.assembler_retval) {
                case COMPLETE:
                        break;
                case MISSING_MAIN:
                        std::cout << "--- Assembler Error ---\n";
                        std::cout << "Main label was never defined\n";
                        return 0;
                case UNKNOWN_LABEL:
                        std::cout << "--- Assembler Error ---\n";
                        std::cout << "Label is undefined\n";
                        std::cout << "Token #" << res.relevant_idx << "\n";
                        std::cout << "Invalid instruction: ";
                        std::cout << res.relevant_tokens.at(0) << " ";
                        std::cout << res.relevant_tokens.at(1) << "\n";
                        return 0;
                case UNKNOWN_MNEMONIC:
                        std::cout << "--- Assembler Error ---\n";
                        std::cout << "Mnemonic is undefined\n";
                        std::cout << "Instruction #" << res.relevant_idx << "\n";
                        std::cout << "Invalid mnemonic: " << res.relevant_tokens.at(0) << "\n";
                        return 0;
                }

                if (life_opts.intermediate_files) {
                        bool res_temp = write_program_to_sink(final_program, file_header);
                        if (!res_temp) {
                                std::cerr << "Failed to open program sink file\n";
                                return 1;
                        }
                }
                */
        }


        if (life_opts.compile_only) {
                // write_program_data(cpu_handle, sink_path);
                // return 0;
        }

        // CPU_Handle cpu_handle;

        // run the program
        // run_program(cpu_handle, life_opts.print_step);

        return 0;
}
