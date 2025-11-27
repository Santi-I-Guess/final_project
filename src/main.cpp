/* CSCI 200: Final Project: Pseudo Assembly Simulator
 *
 * Author: Santiago Sagastegui
 *
 * Pseudo assembly simulator for very simplified ISA. Also offers debug
 * flags, intermediate step files, and line-by-line printing.
 * Also hosted at https://github.com/Santi-I-Guess/final_project
 */

#include <deque>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "assembler/blueprint.h"
#include "assembler/tokenizer.h"
#include "assembler/translation.h"
#include "auxiliary.h"
#include "common_values.h"
#include "misc/cmd_line_opts.h"
#include "misc/file_handling.h"
#include "simulator/cpu_handle.h"
#include "simulator/debug_funcs.h"

// every subdirectory of src is isolated in dependencies and function
// make tests eventually

// TODO: allow piping in program if no file path is provided

int main(int argc, char **argv) {
        Cmd_Options life_opts;
        bool valid_cmd_arg_combo = life_opts.handle_cmd_args(argc, argv);
        if (!valid_cmd_arg_combo)
                return 0;

        // produce random file header for intermediate files
        // makes running multiple tests in a row unlikely to overwrite data
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> rand_letter(0, 25);
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

        if (life_opts.is_binary_input) {
                std::string bin_source_path = argv[life_opts.input_file_idx];
                populate_program_from_binary(final_program, bin_source_path);
        } else {
                std::string source_buffer = "";
                if (life_opts.input_file_idx != -1) {
                        std::string source_path = argv[life_opts.input_file_idx];
                        std::ifstream source_file(source_path);
                        if (source_file.fail()) {
                                std::cerr << "Failed to open input file\n";
                                return 1;
                        }
                        source_buffer = read_file_to_buffer(source_file);
                        source_file.close();
                } else {
                        source_buffer = "";
                        while (1) {
                                std::string aux_string = "";
                                std::getline(std::cin, aux_string);
                                if (aux_string == "; EOF")
                                        break;
                                source_buffer += aux_string + "\n";
                        }
                }

                // Step 1: tokenize and define labels
                // label_table also removes label definitions from tokens
                std::deque<std::string> tokens = create_tokens(source_buffer);
                std::map<std::string, int16_t> label_table = create_label_map(tokens);
                if (life_opts.intermediate_files)
                        generate_intermediates(file_header, tokens, label_table);

                // Step 2: grammar checking for tokenized program
                Debug_Info res;
                res = grammar_check(tokens, label_table);
                handle_grammar_check_res(res);

                // Step 3: generate program
                Program_Info program_info = {};
                program_info.tokens = tokens;
                program_info.label_table = label_table;
                res = assemble_program(final_program, program_info);
                handle_assemble_res(res);
        }

        if (life_opts.assemble_only) {
                bool res_temp;
                res_temp = write_program_to_sink(final_program, file_header);
                if (!res_temp) {
                        std::cerr << "Failed to open token sink file\n";
                        return 1;
                }
        }

        // note to self: can i create something like gdb?? PALdb? pdb?

        CPU_Handle cpu_handle;
        cpu_handle.load_program(final_program);

        if (life_opts.debug_options)
                interpret_program(cpu_handle);
//      cpu_handle.run_program();

        return 0;
}
