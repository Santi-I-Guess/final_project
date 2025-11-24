#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iomanip>
#include <iostream>

#include "../common_values.h"
#include "cpu_handle.h"

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
        prog_ctr = 0;
        prog_size = 0;
        stack_ptr = 0;
        // i know int16_t should always be 2 bytes, but whatever
        memset(program_mem, 0, sizeof(int16_t) * 2048);
        program_data = nullptr;
}

CPU_Handle::~CPU_Handle() {
        if (program_data)
                delete[] program_data;
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
        size_t given_size = given_program.size();
        for (size_t i = 0; i < given_size; ++i) {
                program_data[i] = given_program[i];
        }
        prog_size = given_size;
}

// TODO: flagged integers seem to print some negatives wrong, such as
//       $-16394 instead of $-10, or $-20794 instead of $-4410. But gets other
//       negatives right, such as $-19044 and $-21846. So, it seems to make numbers
//       bigger: maybe a flag is being set wrong?
// TODO: python is kinda shit at unpacking numbers of specific bit lengths,
//       and with interpreting them as signed.

void print_arg(
        int16_t curr,
        Program_State &curr_state,
        size_t &num_args_left
) {
        std::cout << std::setbase(10) << std::right;
        std::cout << std::setw(7);
        if ((curr >> 14) & 1) {
                // literal
                curr ^= (int16_t)(1 << 14);
                std::cout << "$";
        } else if ((curr >> 13) & 1) {
                // stack offset
                curr ^= (int16_t)(1 << 13);
                std::cout << "%";
        }
        std::cout << curr;
        num_args_left--;
        if (num_args_left == 0) {
                curr_state = READING_MNEMONIC;
                std::cout << "\n";
        } else {
                std::cout << " ";
        }
}

void print_chars(
        int16_t curr,
        Program_State &curr_state,
        size_t &curr_str_idx)
{
        int16_t higher = curr >> 8;
        int16_t lower = curr & 0xff;
        if (curr == 0 && curr_str_idx > 0) {
                // end of string
                // why is this never being run?
                std::cout << "\"\n";
                curr_str_idx = 0;
        } else if (curr == (int16_t)(0xffff)) {
                // end of string data
                std::cout << "--- Program Data ---\n";
                curr_state = READING_MNEMONIC;
        } else {
                // ensure programs with no string don't
                // add extraneous programs
                if (curr_str_idx == 0 && curr) {
                        // start of string
                        std::cout << "\"";
                }
                curr_str_idx += 2;
                if (curr) {
                        if (lower == '\n')
                                std::cout << "\\n";
                        else
                                std::cout << (char)lower;
                        if (higher == '\n')
                                std::cout << "\n";
                        else
                                std::cout << (char)higher;
                }
        }
}

void print_entry_label(int16_t curr, Program_State &curr_state) {
        std::cout << std::setbase(16) << "0x" << curr;
        std::cout << ": main label addr\n";
        std::cout << "--- String Data ---\n";
        curr_state = READING_STR;
}

void print_mnemonic(
        int16_t curr,
        Program_State &curr_state,
        size_t &num_args_left
) {
        std::cout << std::left << std::setw(9);
        std::cout << DEREFERENCE_TABLE.at(curr);
        num_args_left = BLUEPRINTS.at(DEREFERENCE_TABLE.at(curr)).size() - 1;
        if (num_args_left > 0)
                curr_state = READING_ARGS;
        else
                std::cout << "\n";
}

void CPU_Handle::interpret_program() const {
        Program_State curr_state = READING_ENTRY_LABEL;
        size_t curr_str_idx = 0;
        size_t num_args_left = 0;
        // start at 4 to skip SANTIAGO
        for (size_t i = 4; i < prog_size; ++i) {
                int16_t curr = program_data[i];
                switch (curr_state) {
                case NONE:
                        break;
                case READING_ENTRY_LABEL:
                        print_entry_label(curr, curr_state);
                        break;
                case READING_MNEMONIC:
                        print_mnemonic(curr, curr_state, num_args_left);
                        break;
                case READING_ARGS:
                        print_arg(curr, curr_state, num_args_left);
                        break;
                case READING_STR:
                        print_chars(curr, curr_state, curr_str_idx);
                        break;
                default: /* impossible */
                        break;
                }
        }
};
