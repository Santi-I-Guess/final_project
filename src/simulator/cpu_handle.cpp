#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../common_values.h"
#include "cpu_handle.h"
#include "debug_funcs.h"

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

void CPU_Handle::load_program(const std::deque<int16_t> given_program) {
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

        std::vector<int16_t> breakpoints = {};
        std::vector<int16_t> mnemonic_addrs = {};
        int16_t temp_idx = 0;
        // move temp_idx to first mnemonic
        while ((program_data[temp_idx-1] != (int16_t)0xffff) && (temp_idx < prog_size))
                temp_idx++;
        while (temp_idx < prog_size) {
                mnemonic_addrs.push_back(temp_idx);
                int16_t curr = program_data[temp_idx];
                std::string mnemonic_str = DEREFERENCE_TABLE[curr];
                std::deque<Atom_Type> curr_blueprint = BLUEPRINTS.at(mnemonic_str);
                temp_idx += (int16_t)curr_blueprint.size();
        }

        std::cout << "PAL Debugger (PDB)\n";
        std::cout << "For help, type \"help\"\n\n";

        // pause when no more instructions
        while (!hit_exit) {
                std::cout << "(pdb) ";
                std::string command;
                std::getline(std::cin, command);

                // tokenize command
                std::vector<std::string> cmd_tokens;
                std::string aux_string = "";
                std::stringstream aux_stream(command);
                while (aux_stream >> aux_string) {
                        cmd_tokens.push_back(aux_string);
                }
                if (cmd_tokens.size() == 0)
                        continue;

                // begin parsing
                if (cmd_tokens.front()[0] == 'b') {
                        // break
                        if (cmd_tokens.size() != 2) {
                                std::cout << "argument required\n";
                                continue;
                        }
                        // check for 2nd argumenteak funname
                        // check 2nd argument is valid offset
                } else if (cmd_tokens.front() == "clear") {
                        system("clear");
                } else if (cmd_tokens.front() == "continue") {
                        num_instructions_left = -1;
                } else if (cmd_tokens.front()[0] == 'd') {
                        // delete
                        if (cmd_tokens.size() != 2) {
                                // delete all breakpoints
                                breakpoints.clear();
                                std::cout << "all breakpoints deleted\n";
                        } else {
                                // remove address from breakpoints
                                int16_t condemned = (int16_t)std::stoi(cmd_tokens.at(1));
                                for (size_t i = 0; i < breakpoints.size(); ++i) {
                                        if (breakpoints[i] == condemned) {
                                                breakpoints.erase(breakpoints.begin() + i);
                                                break;
                                        }
                                }
                        }
                        // check for 2nd argument
                        // check 2nd argument is valid offset
                } else if (cmd_tokens.front()[0] == 'h') {
                        // help
                        print_pdb_help();
                } else if (cmd_tokens.front()[0] == 'i') {
                        // interpret
                        interpret_program(*this);
                } else if (cmd_tokens.front()[0] == 'l') {
                        // next instruction to run
                        if (prog_ctr == 0)
                                print_instruction_simple(program_data, program_data[4]);
                        else
                                print_instruction_simple(program_data, prog_ctr);
                } else if (cmd_tokens.front()[0] == 'n') {
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
                        // check for 2nd argument
                } else if (cmd_tokens.front()[0] == 'q') {
                        // quit
                        break;
                }

                bool previously_ran = false;
                while (!hit_exit && num_instructions_left > 0) {
                        // run_instructions
                        next_instruction(hit_exit);
                        num_instructions_left--;
                        previously_ran = true;
                }

                if (!hit_exit && previously_ran) {
                        // print next instruction to run
                        if (prog_ctr == 0)
                                print_instruction_simple(program_data, program_data[4]);
                        else
                                print_instruction_simple(program_data, prog_ctr);
                        previously_ran = false;
                }
        }
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
                intended_value = program_mem[1536 + stack_ptr - given_value];
        } else {
                // register idx
                switch (given_value) {
                case 0:
                        intended_value = reg_a;
                        break;
                case 1:
                        intended_value = reg_b;
                        break;
                case 2:
                        intended_value = reg_c;
                        break;
                case 3:
                        intended_value = reg_d;
                        break;
                case 4:
                        intended_value = reg_e;
                        break;
                case 5:
                        intended_value = reg_f;
                        break;
                case 6:
                        intended_value = reg_g;
                        break;
                case 7:
                        intended_value = reg_h;
                        break;
                case 8:
                        intended_value = stack_ptr;
                        break;
                case 9:
                        intended_value = prog_ctr;
                        break;
                case 10:
                        intended_value = reg_cmp_a;
                        break;
                case 11:
                        intended_value = reg_cmp_b;
                        break;
                default:
                        std::cout << "Error: " << error_messages[2];
                        std::cout << "\n";
                        std::exit(1);
                }
        }
        return intended_value;
}
