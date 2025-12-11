#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#ifdef _WIN32
#include <cstdio>
#include <io.h>
#else
#include <unistd.h>
#endif

#include "../common_values.h"
#include "../instruction_types.h"
#include "cpu_handle.h"
#include "pal_debugger.h"
#include "instructions.h"

extern std::map<std::string, Instruction_Data> BLUEPRINTS;

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
        memset(call_stack, 0, sizeof(int16_t) * CALL_STACK_SIZE);
        memset(program_mem, 0, sizeof(int16_t) * STACK_SIZE);
        program_data = nullptr;
}

CPU_Handle::~CPU_Handle() {
        if (program_data)
                delete[] program_data;
        program_data = nullptr;
        prog_size = 0;
}

int16_t CPU_Handle::dereference_value(const int16_t given_value) {
        int16_t intended_address = 0;
        int16_t intended_value   = 0;
        int16_t addr_bits = (given_value >> 12) & 7;
        if ((given_value >> 14) == 1 || given_value < 0) {
                // literal value
                // set addr bits off for non-negative numbers
                intended_value = given_value;
                if (intended_value >= 0)
                        intended_value ^= (int16_t)(4 << 12);
        } else if (addr_bits == 1) {
                // stack offset
                intended_address = given_value;
                intended_address ^= (int16_t)(1 << 12);
                if (intended_address > stack_ptr || stack_ptr <= 0) {
                        handle_runtime_error(INVALID_STACK_OFFSET);
                }
                intended_value = program_mem[STACK_START + stack_ptr - intended_address - 1];
        } else if (addr_bits == 2) {
                // ram address
                intended_address = given_value;
                intended_address ^= (int16_t)(2 << 12);
                if (intended_address < 0 || intended_address >= STACK_START) {
                        std::cout << "ram hotfix\n";
                        handle_runtime_error(INVALID_STACK_OFFSET);
                }
                intended_value = program_mem[intended_address];
        } else if (addr_bits == 3) {
                // string literal
                intended_value = given_value;
                intended_value ^= (int16_t)(3 << 12);
        } else {
                // register idx
                switch (given_value) {
                case  0: intended_value = 0;         break; // zero reg
                case  1: intended_value = reg_a;     break;
                case  2: intended_value = reg_b;     break;
                case  3: intended_value = reg_c;     break;
                case  4: intended_value = reg_d;     break;
                case  5: intended_value = reg_e;     break;
                case  6: intended_value = reg_f;     break;
                case  7: intended_value = reg_g;     break;
                case  8: intended_value = reg_h;     break;
                case  9: intended_value = stack_ptr; break;
                case 10: intended_value = prog_ctr;  break;
                case 11: intended_value = reg_cmp_a; break;
                case 12: intended_value = reg_cmp_b; break;
                default:
                        handle_runtime_error(UNKNOWN_REGISTER);
                }
        }
        return intended_value;
}


int16_t CPU_Handle::get_program_data(const int16_t idx) const {
        if (idx < 0 || idx > prog_size) {
                handle_runtime_error(UNKNOWN_OPCODE);
        }
        return program_data[idx];
}


int16_t CPU_Handle::get_prog_size() const {
        return prog_size;
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

void CPU_Handle::next_instruction(bool &hit_exit, bool continue_cond) {
        // if program just started
        if (prog_ctr == 0)
                prog_ctr = get_program_data(4);

        int16_t opcode = get_program_data(prog_ctr);
        if (opcode < 0 || opcode >= (int16_t)BLUEPRINTS.size()) {
                handle_runtime_error(UNKNOWN_OPCODE);
        }
        std::string mnem_name = get_mnem_name(opcode);

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
        } else if (mnem_name == "MOD") {
                ins_mod(*this);
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
                if (!continue_cond)
                        std::cout << "\n";
        } else if (mnem_name == "SPRINT") {
                ins_sprint(*this);
                if (!continue_cond)
                        std::cout << "\n";
        } else if (mnem_name == "CPRINT") {
                ins_cprint(*this);
                if (!continue_cond)
                        std::cout << "\n";
        } else if (mnem_name == "INPUT") {
                ins_input(*this);
        } else if (mnem_name == "SINPUT") {
                ins_sinput(*this);
        } else if (mnem_name == "RAND") {
                ins_rand(*this);
        } else if (mnem_name == "EXIT") {
                ins_exit(*this);
                hit_exit = true;
        }
}

void CPU_Handle::run_program() {
        bool hit_exit = false;
        prog_ctr = get_program_data(4);

        while (!hit_exit) {
                next_instruction(hit_exit, true);
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
        int16_t temp_idx = 5; // SA, NT, IA, GO, main
        while ((get_program_data(temp_idx - 1) != (int16_t)0xffff) && (temp_idx < prog_size))
                temp_idx++;
        while (temp_idx < prog_size) {
                mnemonic_addrs.push_back(temp_idx);
                int16_t opcode = get_program_data(temp_idx);
                temp_idx += (int16_t)BLUEPRINTS.at(get_mnem_name(opcode)).length;
        }

        std::cout << "PAL Debugger (PalDB)\n";
        std::cout << "For help, type \"help\"\n\n";

        prog_ctr = get_program_data(4);

#ifdef _WIN32
        bool is_piped_input = !_isatty(_fileno(stdin));
        std::ifstream terminal("CONIN$");
#else
        bool is_piped_input = !isatty(STDIN_FILENO);
        std::ifstream terminal("/dev/tty");
#endif
        if (terminal.fail()) {
                std::cerr << "Failed to open terminal device\n";
                std::exit(1);
        }


        // pause when no more instructions
        while (!hit_exit) {
                std::cout << "(PalDB) > ";
                std::cout.flush();
                std::string command = "";
                if (is_piped_input) {
                        std::getline(terminal, command);
                } else {
                        std::getline(std::cin, command);
                }

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
                } else if (cmd_tokens.front() == "delete") {
                        // delete
                        pdb_handle_delete(cmd_tokens, breakpoints);
                        continue;
                } else if (cmd_tokens.front()[0] == 'h') {
                        // help
                        pdb_handle_help();
                        continue;
                } else if (cmd_tokens.front() == "disassemble") {
                        // interpret
                        pdb_handle_disassemble(*this);
                        continue;
                } else if (cmd_tokens.front()[0] == 'l') {
                        // next instruction to run
                        int16_t opcode = get_program_data(prog_ctr);
                        int16_t ins_len = (int16_t)BLUEPRINTS.at(get_mnem_name(opcode)).length;
                        std::vector<int16_t> instruction = {};
                        for (int16_t i = 0; i < ins_len; ++i) {
                                int16_t curr_element = get_program_data(prog_ctr + i);
                                instruction.push_back(curr_element);
                        }
                        disassemble_print_instruction(instruction, prog_ctr);
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
                        return;
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

                        next_instruction(hit_exit, continue_cond);
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
                        int16_t opcode = get_program_data(prog_ctr);
                        int16_t ins_len = (int16_t)BLUEPRINTS.at(get_mnem_name(opcode)).length;
                        std::vector<int16_t> instruction = {};
                        for (int16_t i = 0; i < ins_len; ++i) {
                                int16_t curr_element = get_program_data(prog_ctr + i);
                                instruction.push_back(curr_element);
                        }
                        disassemble_print_instruction(instruction, prog_ctr);
                        previously_ran = false;
                }
        }
        terminal.close();
}


void handle_runtime_error(Runtime_Error_Enum error_code) {
        std::cerr << "\x1b[34mRuntime Error:\x1b[0m ";
        std::cerr << RUNTIME_ERROR_MESSAGES[error_code] << "\n";
        std::exit(1);
}
