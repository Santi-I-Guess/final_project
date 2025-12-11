#include <cctype>
#include <iomanip>
#include <iostream>

#include "../instruction_types.h"
#include "../token_types.h"
#include "pal_debugger.h"
#include "cpu_handle.h"

#define BOLD "\x1b[1m"
#define CLEAR "\x1b[0m"

extern std::map<std::string, Instruction_Data> BLUEPRINTS;

void pdb_handle_break(
        const std::vector<std::string> &cmd_tokens,
        std::vector<int16_t> &breakpoints,
        const std::vector<int16_t> &mnemonic_addrs
) {
        if (cmd_tokens.size() != 2) {
                std::cout << "argument required\n";
                return;
        }

        // if subcommand is "list", list breakpoints and quit
        if (cmd_tokens.at(1) == "list") {
                std::cout << "breakpoints:\n";
                for (int16_t address : breakpoints) {
                        std::cout << "#" <<  address << "\n";
                }
                return;
        }

        // check if breakpoint points to an address with an opcode
        int16_t awaiting = (int16_t)std::stoi(cmd_tokens.at(1));
        bool is_valid = false;
        for (int16_t address : mnemonic_addrs) {
                if (awaiting == address) {
                        is_valid = true;
                        break;
                }
        }
        if (!is_valid) {
                std::cout << awaiting << " is not a valid breakpoint\n";
                return;
        }

        // check if breakpoint already exists
        for (int16_t address : breakpoints) {
                if (awaiting == address) {
                        std::cout << "breakpoint already exists\n";
                        break;
                }
        }

        breakpoints.push_back(awaiting);
        std::cout << "added " << awaiting << "\n";
}

void pdb_handle_delete(
        const std::vector<std::string> &cmd_tokens,
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
        std::cout << BOLD "disassemble" CLEAR "\n";
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

void pdb_handle_disassemble(const CPU_Handle &cpu_handle) {
        Program_State_Enum curr_state = READING_ENTRY_LABEL;
        const int16_t header[4] = {
                cpu_handle.get_program_data(0),
                cpu_handle.get_program_data(1),
                cpu_handle.get_program_data(2),
                cpu_handle.get_program_data(3),
        };

        const int16_t magic_nums[4] = {
                0x4153,
                0x544e,
                0x4149,
                0x4f47
        };

        // check if program starts with hex values of "SANTIAGO",
        // and warn user if false
        if ((header[0] != magic_nums[0])
                || (header[1] != magic_nums[1])
                || (header[2] != magic_nums[2])
                || (header[3] != magic_nums[3])) {
                std::cout << "--- *Warning*: Magic Number Mismatch ---\n";
        }

        // I mean, after it hits the first mnemonic, it's expected that
        // it's only going to see mnemonics afterwards


        // auxiliary variables
        int16_t opcode;
        int16_t ins_len;

        // start at 4 to skip magic numbers
        int16_t curr_str_idx = 0;
        int16_t int_idx = 4;
        std::vector<int16_t> instruction = {};
        while (int_idx < cpu_handle.get_prog_size()) {
                int16_t curr_int = cpu_handle.get_program_data(int_idx);
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
                        opcode = cpu_handle.get_program_data(int_idx);
                        ins_len = (int16_t)BLUEPRINTS.at(get_mnem_name(opcode)).length;
                        for (int16_t i = 0; i < ins_len; ++i) {
                                int16_t curr_element = cpu_handle.get_program_data(int_idx + i);
                                instruction.push_back(curr_element);
                        }
                        disassemble_print_instruction(instruction, int_idx);
                        int_idx += ins_len;
                        break;
                case READING_STR:
                        // keep int_idx here: not all control paths return at end
                        disassemble_print_chars(curr_int, int_idx, curr_str_idx, curr_state); // &
                        int_idx++;
                        break;
                default: /* impossible */
                        break;
                }
                instruction.clear();
        }
}

void pdb_handle_print(
        const std::vector<std::string> &cmd_tokens,
        CPU_Handle &cpu_handle
) {
        if (cmd_tokens.size() != 2) {
                std::cout << "no arguments provided\n";
                return;
        }

        std::map<std::string, int16_t*> reg_addr_map = {
                {"RA", &cpu_handle.reg_a},      {"RB", &cpu_handle.reg_b},
                {"RC", &cpu_handle.reg_c},      {"RD", &cpu_handle.reg_d},
                {"RE", &cpu_handle.reg_e},      {"RF", &cpu_handle.reg_f},
                {"RG", &cpu_handle.reg_g},      {"RH", &cpu_handle.reg_h},
                {"RH", &cpu_handle.reg_cmp_a},  {"RH", &cpu_handle.reg_cmp_b},
                {"RSP", &cpu_handle.stack_ptr}, {"RIP", &cpu_handle.prog_ctr}
        };

        std::string requested = cmd_tokens.at(1);
        if (requested.at(0) == '%') {
                // stack offset: expect %num
                std::string temp = "";
                for (size_t i = 1; i < requested.length(); i++) {
                        if (!isdigit(requested.at(i)))
                                break;
                        temp += requested.at(i);
                }
                if (temp.empty()) {
                        std::cout << "No valid value provided to offset\n";
                        return;
                }
                int16_t value = (int16_t)std::stoi(temp);
                if (value < 0 || value >= cpu_handle.stack_ptr) {
                        std::cout << "Cannot access stack with offset outside [0,stack_ptr - 1]\n";
                } else {
                        std::cout << requested << " = ";
                        // CPU_Handle see dereference_value
                        std::cout << cpu_handle.program_mem[STACK_START + cpu_handle.stack_ptr - value - 1] << "\n";
                }
        } else if (requested.at(0) == '[') {
                // mem address: expect [%num]
                std::string temp = "";
                for (size_t i = 2; i < requested.length(); i++) {
                        if (!isdigit(requested.at(i)))
                                break;
                        temp += requested.at(i);
                }
                if (temp.empty()) {
                        std::cout << "No valid value provided to address\n";
                        return;
                }
                int16_t value = (int16_t)std::stoi(temp);
                if (value < 0 || value >= STACK_START) {
                        std::cout << "Cannot access mem value outside [0," << STACK_SIZE << "]\n";
                } else {
                        std::cout << requested << " = ";
                        std::cout << cpu_handle.program_mem[value] << "\n";
                }
        } else if (reg_addr_map.find(requested) != reg_addr_map.end()) {
                // print register value
                int16_t value = *reg_addr_map.at(requested);
                std::cout << requested << " = " << value << "\n";
        } else {
                std::cout << "unrecognized value\n";
        }
}

void disassemble_print_chars(
        const int16_t &curr_int,
        const int16_t &int_idx,
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

void disassemble_print_instruction(
        const std::vector<int16_t> &instruction,
        const int16_t &prog_ctr
) {
        std::string out_string;
        std::stringstream out_stream;

        // first, the mnemoinc
        int16_t opcode = instruction.at(0);
        std::string mnem_name = get_mnem_name(opcode);
        out_stream << "#" << std::right << std::setw(4) << prog_ctr << ": ";
        out_stream << std::left << std::setw(7) << mnem_name;

        const std::string REG_DEREFERENCE[13] = {
                "RZ", "RA", "RB", "RC", "RD", "RE", "RF", "RG",
                "RSP", "RIP", "CMP0", "CMP1"
        };

        // second, the arguments
        Instruction_Data curr_instruction = BLUEPRINTS.at(mnem_name);
        size_t ins_size = BLUEPRINTS.at(mnem_name).length;
        for (size_t arg_idx = 1; arg_idx < ins_size; ++arg_idx) {
                std::string arg_string;
                std::stringstream arg_stream;
                int16_t curr_arg = instruction.at(arg_idx);
                Atom_Type arg_type = curr_instruction.blueprint.at(arg_idx);

                int16_t addr_bits = (curr_arg >> 12) & 7;
                if ((curr_arg >> 14) == 1 || curr_arg < 0) {
                        // literal bitmask
                        if (curr_arg >= 0)
                                curr_arg ^= (int16_t)(4 << 12);
                        arg_stream << "$";
                        arg_stream << curr_arg;
                } else if (addr_bits == 1) {
                        // stack offset bitmask
                        curr_arg ^= (int16_t)(1 << 12);
                        arg_stream << "%";
                        arg_stream << curr_arg;
                } else if (addr_bits == 2) {
                        // ram address
                        curr_arg ^= (int16_t)(2 << 12);
                        arg_stream << "[$";
                        arg_stream << curr_arg;
                        arg_stream << "]";
                } else if (addr_bits == 3) {
                        // string literal
                        curr_arg ^= (int16_t)(3 << 12);
                        arg_stream << "#";
                        arg_stream << curr_arg;
                } else if (arg_type == REGISTER && 0 <= curr_arg && curr_arg < 14) {
                        // register idx
                        arg_stream << REG_DEREFERENCE[curr_arg];
                } else {
                        // label
                        arg_stream << "#";
                        arg_stream << curr_arg;
                }
                std::getline(arg_stream, arg_string);
                out_stream << std::right << std::setw(8) << arg_string;
        }

        // print instruction buffer
        out_string = out_stream.str();
        std::cout << out_string << "\n";
}
