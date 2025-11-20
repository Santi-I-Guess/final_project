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

#include "auxiliary.h"

#include "assembler/blueprint.h"
#include "assembler/common_values.h"
#include "assembler/tokenizer.h"
#include "assembler/translation.h"
#include "misc/cmd_line_opts.h"
#include "misc/file_handling.h"

// every subdirectory of src is isolated in dependencies and function
// make tests eventually

int main(int argc, char **argv) {
        Cmd_Options life_opts;
        bool valid_cmd_arg_combo = life_opts.handle_cmd_args(argc, argv);
        if (!valid_cmd_arg_combo)
                return 0;

        // produce intermediate file header
        // makes running multiple tests in a row unlikely to overwrite data
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution rand_letter(0, 25);
        std::string file_header = "";
        for (int i = 0; i < 10; ++i) {
                char curr_letter;
                int letter_idx = rand_letter(mt);
                curr_letter = (char)letter_idx + 'a';
                file_header += curr_letter;
        }

        // put assembled program here, so that assembler.h doesn't require
        // cpu_handle
        std::deque<int16_t> final_program = {};

        std::string source_path = argv[life_opts.input_file_idx];
        if (life_opts.is_binary_input) {
                // populate_program_from_binary(cpu_handle, source_path);
        } else {
                std::ifstream source_file(source_path);
                if (source_file.fail()) {
                        std::cerr << "Failed to open input file\n";
                        return 1;
                }

                // Step 1: tokenize and define labels
                // label_table also removes label definitions from tokens
                std::string source_buffer = read_file_to_buffer(source_file);
                std::deque<std::string> tokens = tokenize_buffer(source_buffer);
                std::map<std::string, int16_t> label_table = define_labels(tokens);
                if (life_opts.intermediate_files)
                        generate_intermediates(file_header, tokens, label_table);

                // Step 2: grammar checking for tokenized program
                Debug_Info res;
                res = grammar_check(tokens, label_table);
                handle_grammar_check_res(res);

                // Step 3: generate program
                // str_idx_offsets should be empty
                Program_Info program_info = {};
                program_info.tokens = tokens;
                program_info.label_table = label_table;
                res = generate_program(final_program, program_info);
                handle_generation_res(res);

                /*
                // Removed while reworking ABI

                // note: program does not fall under intermediate files
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
                bool res_temp;
                res_temp = write_program_to_sink(final_program, file_header);
                if (!res_temp) {
                        std::cerr << "Failed to open token sink file\n";
                        return 1;
                }
        }

        // CPU_Handle cpu_handle;

        // run the program
        // run_program(cpu_handle, life_opts.print_step);

        return 0;
}
