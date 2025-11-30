#include <cctype>
#include <iomanip>
#include <iostream>

#include "debug_funcs.h"
#include "cpu_handle.h"
#include "../common_values.h"

#define BOLD "\x1b[1m"
#define CLEAR "\x1b[0m"

void pdb_handle_break(
        const std::vector<std::string> cmd_tokens,
        std::vector<int16_t> &breakpoints,
        const std::vector<int16_t> mnemonic_addrs
) {
        // break
        if (cmd_tokens.size() != 2) {
                std::cout << "argument required\n";
                return;
        }

        if (cmd_tokens.at(1) == "list") {
                std::cout << "breakpoints:\n";
                for (int16_t address : breakpoints) {
                        std::cout << "#" <<  address << "\n";
                }
                return;
        }

        int16_t awaiting = (int16_t)std::stoi(cmd_tokens.at(1));
        bool is_valid = false;
        // check if breakpoint is at an opcode
        for (int16_t address : mnemonic_addrs) {
                if (awaiting == address) {
                        is_valid = true;
                        break;
                }
        }
        // check if breakpoint already exists
        for (int16_t address : breakpoints) {
                if (awaiting == address) {
                        std::cout << "breakpoint already exists\n";
                        break;
                }
        }

        if (is_valid) {
                breakpoints.push_back(awaiting);
                std::cout << "added " << awaiting << "\n";
        } else {
                std::cout << awaiting << " is not a valid breakpoint\n";
        }
}

void pdb_handle_delete(
        const std::vector<std::string> cmd_tokens,
        std::vector<int16_t> &breakpoints
) {
        if (cmd_tokens.size() != 2) {
                // delete all breakpoints
                breakpoints.clear();
                std::cout << "all breakpoints deleted\n";
                return;
        }
        // remove address from breakpoints
        int16_t condemned = (int16_t)std::stoi(cmd_tokens.at(1));
        for (size_t i = 0; i < breakpoints.size(); ++i) {
                if (breakpoints[i] == condemned) {
                        breakpoints.erase(breakpoints.begin() + i);
                        std::cout << "deleted #" << condemned << "\n";
                        break;
                }
        }
}

void pdb_handle_help() {
        // if you're wondering why I don't just use std::endl, it's because
        // I'm trying to prevent stdout flushing every single line
        std::cout << BOLD "break" CLEAR " <program address>\n";
        std::cout << "    set a breakpoint at a specified address, which halts execution\n";
        std::cout << BOLD "clear" CLEAR "\n";
        std::cout << "    clear the console\n";
        std::cout << BOLD "continue" CLEAR "\n";
        std::cout << "    continue program execution until EXIT or next breakpoint\n";
        std::cout << BOLD "delete" CLEAR " <program address>\n";
        std::cout << "    delete breakpoint at a specified address\n";
        std::cout << BOLD "help" CLEAR "\n";
        std::cout << "    show this help screen\n";
        std::cout << BOLD "interpret" CLEAR "\n";
        std::cout << "    show disassembled program.\n";
        std::cout << BOLD "list" CLEAR "\n";
        std::cout << "    show the next instruction to be performed\n";
        std::cout << "    useful for debugging branching instructions and SPRINT\n";
        std::cout << BOLD "next" CLEAR "\n";
        std::cout << "    continue program until next instruction\n";
        std::cout << BOLD "print" CLEAR " <register|stack offset|ram address>\n";
        std::cout << "    print value in program's memory\n";
        std::cout << BOLD "quit" CLEAR "\n";
        std::cout << "    quit debugger and program execution\n\n";
}

void pdb_handle_interpret(const CPU_Handle &cpu_handle) {
        Program_State_Enum curr_state = READING_ENTRY_LABEL;
        int16_t header[4] = {
                cpu_handle.program_data[0],
                cpu_handle.program_data[1],
                cpu_handle.program_data[2],
                cpu_handle.program_data[3]
        };
        int16_t magic_nums[4] = {
                0x4153,
                0x544e,
                0x4149,
                0x4f47
        };
        if ((header[0] != magic_nums[0])
                || (header[1] != magic_nums[1])
                || (header[2] != magic_nums[2])
                || (header[3] != magic_nums[3])) {
                std::cout << "--- *Warning*: Magic Number Mismatch ---\n";
        }

        // I mean, after it hits the first mnemonic, it's expected that
        // it's only going to see mnemonics afterwards

        // start at 4 to skip magic numbers
        int16_t curr_str_idx = 0;
        int16_t int_idx = 4;
        while (int_idx < cpu_handle.prog_size) {
                int16_t curr_int = cpu_handle.program_data[int_idx];
                switch (curr_state) {
                case NONE:
                        break;
                case READING_ENTRY_LABEL:
                        std::cout << "--- Main @ " << curr_int << " ---\n";
                        std::cout << "--- String Data ---\n";
                        curr_state = READING_STR;
                        int_idx++;
                        break;
                case READING_MNEMONIC:
                        itrprt_print_instruction(cpu_handle.program_data, int_idx);
                        int_idx += INSTRUCTION_LENS[cpu_handle.program_data[int_idx]];
                        break;
                case READING_STR:
                        // keep int_idx here: not all control paths return at end
                        itrprt_print_chars(curr_int, int_idx, curr_str_idx, curr_state); // &
                        int_idx++;
                        break;
                default: /* impossible */
                        break;
                }
        }
}

void pdb_handle_print(
        const std::vector<std::string> cmd_tokens,
        CPU_Handle &cpu_handle
) {
        if (cmd_tokens.size() != 2) {
                std::cout << "no arguments provided\n";
                return;
        }

        if (cmd_tokens.at(1).at(0) == '%') {
                // stack offset: expect %num
                std::string temp = "";
                for (size_t i = 1; i < cmd_tokens.at(1).length(); i++) {
                        if (!isdigit(cmd_tokens.at(1).at(i)))
                                break;
                        temp += cmd_tokens.at(1).at(i);
                }
                if (temp.empty()) {
                        std::cout << "No valid value provided to offset\n";
                        return;
                }
                int16_t value = (int16_t)std::stoi(temp);
                if (value < 0 || value >= cpu_handle.stack_ptr) {
                        std::cout << "Cannot access stack with offset outside [0,stack_ptr - 1]\n";
                } else {
                        std::cout << cmd_tokens.at(1) << " = ";
                        std::cout << cpu_handle.program_mem[1536 + cpu_handle.stack_ptr - value] << "\n";
                }
        } else if (cmd_tokens.at(1).at(0) == 'M') {
                // mem address: expect MEM[num]
                std::string temp = "";
                for (size_t i = 4; i < cmd_tokens.at(1).length(); i++) {
                        if (!isdigit(cmd_tokens.at(1).at(i)))
                                break;
                        temp += cmd_tokens.at(1).at(i);
                }
                if (temp.empty()) {
                        std::cout << "No valid value provided to address\n";
                        return;
                }
                int16_t value = (int16_t)std::stoi(temp);
                if (value < 0 || value > 1535) {
                        std::cout << "Cannot access mem value outside [0,1535]\n";;
                } else {
                        std::cout << cmd_tokens.at(1) << " = ";
                        std::cout << cpu_handle.program_mem[value] << "\n";
                }
        } else if (cmd_tokens.at(1) == "RA") {
                std::cout << "RA = " << cpu_handle.reg_a << "\n";
        } else if (cmd_tokens.at(1) == "RB") {
                std::cout << "RB = " << cpu_handle.reg_b << "\n";
        } else if (cmd_tokens.at(1) == "RC") {
                std::cout << "RC = " << cpu_handle.reg_c << "\n";
        } else if (cmd_tokens.at(1) == "RD") {
                std::cout << "RD = " << cpu_handle.reg_d << "\n";
        } else if (cmd_tokens.at(1) == "RE") {
                std::cout << "RE = " << cpu_handle.reg_e << "\n";
        } else if (cmd_tokens.at(1) == "RF") {
                std::cout << "RF = " << cpu_handle.reg_f << "\n";
        } else if (cmd_tokens.at(1) == "RG") {
                std::cout << "RG = " << cpu_handle.reg_g << "\n";
        } else if (cmd_tokens.at(1) == "RH") {
                std::cout << "RH = " << cpu_handle.reg_h << "\n";
        } else if (cmd_tokens.at(1) == "CMP1") {
                std::cout << "RH = " << cpu_handle.reg_cmp_a << "\n";
        } else if (cmd_tokens.at(1) == "CMP2") {
                std::cout << "RH = " << cpu_handle.reg_cmp_b << "\n";
        } else if (cmd_tokens.at(1) == "RSP") {
                std::cout << "RSP = " << cpu_handle.stack_ptr << "\n";
        } else if (cmd_tokens.at(1) == "RIP") {
                std::cout << "RIP = " << cpu_handle.prog_ctr << "\n";
        } else {
                std::cout << "unrecognized value\n";
        }
}

void itrprt_print_chars(
        const int16_t curr_int,
        const int16_t int_idx,
        int16_t &curr_str_idx,
        Program_State_Enum &curr_state
) {
        // auxiliary variables
        char lower  = (char)(curr_int  & 0b11111111);
        char higher = (char)(curr_int >> 8);
        bool is_str_beg  = (curr_int >  0) && (curr_str_idx == 0);
        bool is_str_end  = (curr_int == 0) && (curr_str_idx >  0);
        bool is_data_end = (curr_int == (int16_t)0xffff);

        if (is_str_beg) {
                std::cout << "#";
                std::cout << std::right << std::setw(4);
                std::cout << int_idx << ": " << "\"";
        } else if (is_str_end) {
                std::cout << "\"\n";
                curr_str_idx = 0;
                return;
        } else if (is_data_end) {
                std::cout << "--- Program Data ---\n";
                curr_state = READING_MNEMONIC;
                return;
        }
        curr_str_idx += 2;
        if (curr_int == 0)
                return;
        if (lower == '\n')
                std::cout << "\\n";
        else if (lower == '\"')
                std::cout << "\\\"";
        else
                std::cout << lower;
        if (higher == '\n')
                std::cout << "\\n";
        else if (higher == '\"')
                std::cout << "\\\"";
        else
                std::cout << higher;
}

void itrprt_print_instruction(
        const int16_t *program_data,
        const int16_t prog_ctr
) {
        std::string out_string = "";
        std::stringstream out_stream;

        // first, the mnemoinc
        int16_t opcode = program_data[prog_ctr];
        std::string mnem_name = DEREFERENCE_TABLE[opcode];
        out_stream << "#" << std::right << std::setw(4) << prog_ctr << ": ";
        out_stream << std::left << std::setw(7) << mnem_name;

        // second, the arguments
        int16_t ins_size = INSTRUCTION_LENS[opcode];
        for (int arg_idx = 1; arg_idx < ins_size; ++arg_idx) {
                std::string arg_string = "";
                std::stringstream arg_stream;
                int16_t curr_arg = program_data[prog_ctr + arg_idx];
                Atom_Type arg_type = BLUEPRINTS.at(mnem_name)[arg_idx];
                if ((curr_arg >> 14) & 1) {
                        // literal bitmask
                        if (curr_arg >= 0)
                                curr_arg ^= (int16_t)(1 << 14);
                        arg_stream << "$";
                        arg_stream << curr_arg;
                } else if ((curr_arg >> 13) & 1) {
                        // stack offset bitmask
                        curr_arg ^= (int16_t)(1 << 13);
                        arg_stream << "%";
                        arg_stream << curr_arg;
                } else if ((curr_arg >> 12) & 1) {
                        // string literal
                        curr_arg ^= (int16_t)(1 << 12);
                        arg_stream << "#";
                        arg_stream << curr_arg;
                } else if ((arg_type == REGISTER) || (arg_type == SOURCE)) {
                        // register idx
                        arg_stream << "R";
                        arg_stream << (char)('A' + curr_arg);
                } else {
                        // label
                        arg_stream << "#";
                        arg_stream << curr_arg;
                }
                arg_string = arg_stream.str();
                out_stream << std::right << std::setw(8) << arg_string;
        }

        // print instruction buffer
        out_string = out_stream.str();
        std::cout << out_string << "\n";
}
