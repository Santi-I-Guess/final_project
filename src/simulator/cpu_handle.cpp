#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../common_values.h"
#include "cpu_handle.h"
#include "debug_funcs.h"
#include "instructions.h"

CPU_Handle::CPU_Handle() {
        reg_a = 0;
        reg_b = 0;
        reg_c = 0;
        reg_d = 0;
        reg_e = 0;
        reg_f = 0;
        reg_g = 0;
        reg_h = 0;
        reg_cmp_a = 0;
        reg_cmp_b = 0;
        prog_ctr  = 0;
        prog_size = 0;
        stack_ptr = 0;
        call_stack_ptr = 0;
        // i know int16_t should always be 2 bytes by definition, but whatever
        memset(call_stack, 0, sizeof(int16_t) * 1024);
        memset(program_mem, 0, sizeof(int16_t) * 2048);
        program_data = nullptr;
}

CPU_Handle::~CPU_Handle() {
        if (program_data)
                delete[] program_data;
        program_data = nullptr;
        prog_size = 0;
}

int16_t CPU_Handle::dereference_value(int16_t given_value) {
        int16_t intended_value = 0;
        if ((given_value >> 14) & 1) {
                // literal value
                if (given_value >= 0)
                        given_value ^= (int16_t)(1 << 14);
                intended_value = given_value;
        } else if ((given_value >> 13) & 1) {
                // stack offest
                // stack[0] starts at program_mem[1536]
                given_value ^= (int16_t)(1 << 13);
                if (given_value > stack_ptr) {
                        std::cout << "Error: " << error_messages[1];
                        std::cout << "\n";
                        std::exit(1);
                }
                // -1, because stack_ptr points to memory of next element,
                // not top element
                intended_value = program_mem[1536 + stack_ptr - given_value - 1];
        } else if ((given_value >> 12) & 1) {
                // string literal
                given_value ^= (int16_t)(1 << 12);
                intended_value = given_value;
        } else {
                // register idx
                switch (given_value) {
                case  0: intended_value = reg_a;     break;
                case  1: intended_value = reg_b;     break;
                case  2: intended_value = reg_c;     break;
                case  3: intended_value = reg_d;     break;
                case  4: intended_value = reg_e;     break;
                case  5: intended_value = reg_f;     break;
                case  6: intended_value = reg_g;     break;
                case  7: intended_value = reg_h;     break;
                case  8: intended_value = stack_ptr; break;
                case  9: intended_value = prog_ctr;  break;
                case 10: intended_value = reg_cmp_a; break;
                case 11: intended_value = reg_cmp_b; break;
                default:
                        std::cout << "Error: " << error_messages[2];
                        std::cout << "\n";
                        std::exit(1);
                }
        }
        return intended_value;
}

void CPU_Handle::load_program(const std::vector<int16_t> given_program) {
        if (!program_data) {
                program_data = new int16_t[given_program.size()];
                if (!program_data) {
                        std::cerr << "program_data alloc failed\n";
                        std::exit(1);
                }
        }
        int16_t given_size = (int16_t)given_program.size();
        for (int16_t i = 0; i < given_size; ++i) {
                program_data[i] = given_program[i];
        }
        prog_size = given_size;
}

void CPU_Handle::next_instruction(bool &hit_exit) {

        // if program just started
        if (prog_ctr == 0)
                prog_ctr = program_data[4];

        int16_t opcode = program_data[prog_ctr];
        std::string mnem_name = DEREFERENCE_TABLE[opcode];

        // process instruction here
        // not using switch with opcode in case more instructions are added later
        // since mnem name is just a hashmap lookup
        // each instruction handles progressing prog_ctr, since
        // branch instructions, call, and ret don't follow usual rule
        if (mnem_name == "NOP") {
                ins_nop(*this);
        } else if (mnem_name == "MOV") {
                ins_mov(*this);
        } else if (mnem_name == "INC") {
                ins_inc(*this);
        } else if (mnem_name == "DEC") {
                ins_dec(*this);
        } else if (mnem_name == "ADD") {
                ins_add(*this);
        } else if (mnem_name == "SUB") {
                ins_sub(*this);
        } else if (mnem_name == "MUL") {
                ins_mul(*this);
        } else if (mnem_name == "DIV") {
                ins_div(*this);
        } else if (mnem_name == "AND") {
                ins_and(*this);
        } else if (mnem_name == "OR") {
                ins_or(*this);
        } else if (mnem_name == "NOT") {
                ins_not(*this);
        } else if (mnem_name == "XOR") {
                ins_xor(*this);
        } else if (mnem_name == "LSH") {
                ins_lsh(*this);
        } else if (mnem_name == "RSH") {
                ins_rsh(*this);
        } else if (mnem_name == "CMP") {
                ins_cmp(*this);
        } else if (mnem_name == "JMP") {
                ins_jmp(*this);
        } else if (mnem_name == "JEQ") {
                ins_jeq(*this);
        } else if (mnem_name == "JNE") {
                ins_jne(*this);
        } else if (mnem_name == "JGE") {
                ins_jge(*this);
        } else if (mnem_name == "JGR") {
                ins_jgr(*this);
        } else if (mnem_name == "JLE") {
                ins_jle(*this);
        } else if (mnem_name == "JLS") {
                ins_jls(*this);
        } else if (mnem_name == "CALL") {
                ins_call(*this);
        } else if (mnem_name == "RET") {
                ins_ret(*this);
        } else if (mnem_name == "PUSH") {
                ins_push(*this);
        } else if (mnem_name == "POP") {
                ins_pop(*this);
        } else if (mnem_name == "WRITE") {
                ins_write(*this);
        } else if (mnem_name == "READ") {
                ins_read(*this);
        } else if (mnem_name == "PRINT") {
                ins_print(*this);
        } else if (mnem_name == "SPRINT") {
                ins_sprint(*this);
        } else if (mnem_name == "EXIT") {
                ins_exit(*this);
                hit_exit = true;
        }
}

void CPU_Handle::run_program() {
        bool hit_exit = false;
        if (prog_ctr == 0)
                prog_ctr = program_data[4];

        while (!hit_exit) {
                next_instruction(hit_exit);
        }
}

void CPU_Handle::run_program_debug() {
        int16_t num_instructions_left = 0;
        bool hit_exit = false;
        bool continue_cond = false; // to ensure running after continue cmd
        bool jump_breakpoint = false; // for running after hitting breakpoint
        std::vector<int16_t> breakpoints = {};
        std::vector<int16_t> mnemonic_addrs = {}; // to verify breakpoints are valid

        // to ensure all breakpoints are valid addresses
        int16_t temp_idx = 0;
        while ((program_data[temp_idx - 1] != (int16_t)0xffff) && (temp_idx < prog_size))
                temp_idx++;
        while (temp_idx < prog_size) {
                mnemonic_addrs.push_back(temp_idx);
                int16_t curr = program_data[temp_idx];
                std::string mnemonic_str = DEREFERENCE_TABLE[curr];
                std::vector<Atom_Type> curr_blueprint = BLUEPRINTS.at(mnemonic_str);
                temp_idx += (int16_t)curr_blueprint.size();
        }

        std::cout << "PAL Debugger (PalDB)\n";
        std::cout << "For help, type \"help\"\n\n";

        prog_ctr = program_data[4];

        // pause when no more instructions
        while (!hit_exit) {
                std::cout << "(PalDB) > ";
                std::string command;
                std::getline(std::cin, command);

                // tokenize command
                std::vector<std::string> cmd_tokens;
                std::string aux_string = "";
                std::stringstream aux_stream(command);
                while (aux_stream >> aux_string) {
                        cmd_tokens.push_back(aux_string);
                }
                // ignore, if no commands
                if (cmd_tokens.size() == 0)
                        continue;

                // begin parsing
                if (cmd_tokens.front()[0] == 'b') {
                        pdb_handle_break(cmd_tokens, breakpoints, mnemonic_addrs);
                        continue;
                } else if (cmd_tokens.front() == "clear") {
                        system("clear");
                        continue;
                } else if (cmd_tokens.front() == "continue") {
                        continue_cond = true;
                } else if (cmd_tokens.front()[0] == 'd') {
                        // delete
                        pdb_handle_delete(cmd_tokens, breakpoints);
                        continue;
                } else if (cmd_tokens.front()[0] == 'h') {
                        // help
                        pdb_handle_help();
                        continue;
                } else if (cmd_tokens.front()[0] == 'i') {
                        // interpret
                        pdb_handle_interpret(*this);
                        continue;
                } else if (cmd_tokens.front()[0] == 'l') {
                        // next instruction to run
                        itrprt_print_instruction(program_data, prog_ctr);
                        continue;
                } else if (cmd_tokens.front()[0] == 'n') {
                        // next
                        if (cmd_tokens.size() == 2) {
                                // allow 1+ steps
                                int16_t num_steps = (int16_t)std::stoi(cmd_tokens.at(1));
                                num_steps = (num_steps >= 1) ? num_steps : 1;
                                num_instructions_left = num_steps;
                        } else {
                                num_instructions_left = 1;
                        }

                        // next
                } else if (cmd_tokens.front()[0] == 'p') {
                        // print
                        pdb_handle_print(cmd_tokens, *this);
                        continue;
                } else if (cmd_tokens.front()[0] == 'q') {
                        // quit
                        break;
                        continue;
                } else {
                        std::cout << "unrecognized command\n";
                        continue;
                }

                bool previously_ran = false;
                while (num_instructions_left > 0 || continue_cond) {
                        // run instructions

                        // handle breakpoints
                        if (jump_breakpoint) {
                                jump_breakpoint = false;
                        } else {
                                for (int16_t address : breakpoints) {
                                        if (prog_ctr == address) {
                                                continue_cond = false;
                                                num_instructions_left = 0;
                                                jump_breakpoint = true;
                                                break;
                                        }
                                }
                        }

                        // instead of goto
                        if (!continue_cond && num_instructions_left == 0)
                                break;

                        next_instruction(hit_exit);
                        previously_ran = true;
                        // don't track num_instructions_left after continue cmd
                        if (!continue_cond)
                                num_instructions_left--;
                        if (hit_exit)
                                break;
                }
                // prevent extraneous print when starting debugger
                if (!hit_exit && previously_ran) {
                        // print next instruction to run
                        itrprt_print_instruction(program_data, prog_ctr);
                        previously_ran = false;
                }
        }
}

