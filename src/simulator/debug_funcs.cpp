#include <iomanip>
#include <iostream>

#include "debug_funcs.h"
#include "../common_values.h"

#define BOLD "\x1b[1m"
#define CLEAR "\x1b[0m"

void print_chars(
        int16_t curr,
        int16_t &curr_str_idx,
        int16_t &int_idx,
        Program_State_Enum &curr_state
) {
        // auxiliary variables
        char lower  = (char)(curr  & 0b11111111);
        char higher = (char)(curr >> 8);
        bool is_str_beg  = (curr >  0) && (curr_str_idx == 0);
        bool is_str_end  = (curr == 0) && (curr_str_idx >  0);
        bool is_data_end = (curr == (int16_t)0xffff);

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
        if (curr == 0)
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

void print_instruction(
        int16_t curr,
        int16_t &mnemonic_idx,
        int16_t &int_idx,
        int16_t *program_data
) {
        std::stringstream out_stream;
        std::string out_string;
        // INT.INS: (OPCODE) MNEMONIC ARG1 ARG2 ARG3

        // note to self: std::getline sets EOF bit of stream

        // set instruction part to be right aligned with 14 chars
        std::string mnem_name = DEREFERENCE_TABLE[curr];
        out_stream << "#";
        out_stream << std::right << std::setw(4) << int_idx << ":";
        out_stream << std::right << std::setw(3) << mnemonic_idx << " ";
        out_stream << std::right << std::setw(7) << mnem_name;
        out_string = out_stream.str();

        // set each argument to be right aligned with 7 chars
        int16_t ins_size = (int16_t)(BLUEPRINTS.at(mnem_name).size());
        for (int arg_idx = 1; arg_idx < ins_size; ++arg_idx) {
                std::stringstream arg_stream;
                std::string arg_string;
                int16_t curr_arg = program_data[int_idx + arg_idx];
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
                } else if ((arg_type == REGISTER) || (arg_type == SOURCE)) {
                        arg_stream << "R";
                        arg_stream << (char)('A' + curr_arg);
                } else {
                        arg_stream << "#";
                        arg_stream << curr_arg;
                }
                arg_string = arg_stream.str();
                out_stream << std::right << std::setw(8) << arg_string;
        }

        // print instruction buffer
        out_string = out_stream.str();
        std::cout << out_string << "\n";
        mnemonic_idx++;
        int_idx += ins_size;
}

void interpret_program(const CPU_Handle &cpu_handle) {
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
        int16_t mnemonic_idx = 0;
        while (int_idx < cpu_handle.prog_size) {
                int16_t curr = cpu_handle.program_data[int_idx];
                switch (curr_state) {
                case NONE:
                        break;
                case READING_ENTRY_LABEL:
                        std::cout << "--- Main @ " << curr << " ---\n";
                        std::cout << "--- String Data ---\n";
                        curr_state = READING_STR;
                        int_idx++;
                        break;
                case READING_MNEMONIC:
                        print_instruction(
                                curr,
                                mnemonic_idx,
                                int_idx, // &
                                cpu_handle.program_data
                        );
                        break;
                case READING_STR:
                        // keep int_idx here: not all control paths return at end
                        print_chars(curr, curr_str_idx, int_idx, curr_state); // &
                        int_idx++;
                        break;
                default: /* impossible */
                        break;
                }
        }
}

void print_pdb_help() {
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
        std::cout << BOLD "step" CLEAR "\n";
        std::cout << "    continue program until next instruction\n";
        std::cout << BOLD "print" CLEAR " <register|stack offset|ram address>\n";
        std::cout << "    print value in program's memory\n";
        std::cout << BOLD "quit" CLEAR "\n";
        std::cout << "    quit debugger and program execution\n\n";
}


void print_instruction_simple(int16_t *program_data, int16_t prog_ctr) {
        // most of this is just copy and paste from print_instruction
        std::string out_string = "";
        std::stringstream out_stream;

        // first, the mnemoinc
        int16_t curr = program_data[prog_ctr];
        std::string mnem_name = DEREFERENCE_TABLE[curr];
        out_stream << std::right << std::setw(7) << mnem_name;

        // second, the arguments
        int16_t ins_size = (int16_t)(BLUEPRINTS.at(mnem_name).size());
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
                } else if ((arg_type == REGISTER) || (arg_type == SOURCE)) {
                        arg_stream << "R";
                        arg_stream << (char)('A' + curr_arg);
                } else {
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
