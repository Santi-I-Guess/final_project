#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

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
        prog_ctr  = 0;
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
        Program_State_Enum &curr_state,
        size_t &num_args_left
) {
        // for little endian systems
        std::stringstream aux_stream;
        std::string aux_string;
        std::cout << std::setbase(6) << std::right;
        std::cout << std::setw(7);
        if ((curr >> 14) & 1) {
                // literal bitmask
                // works because two's compliment negatives requires 14th
                // bit as 1 to properly interpret, while positive numbers
                // are using 14th bit to figure out how to interpret in the
                // user program
                if (curr >= 0)
                        curr ^= (int16_t)(1 << 14);
                aux_stream << "$";
        } else if ((curr >> 13) & 1) {
                // stack offset bitmask
                curr ^= (int16_t)(1 << 13);
                aux_stream << "%";
        }
        aux_stream << curr;
        std::getline(aux_stream, aux_string);
        std::cout << aux_string;
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
        Program_State_Enum &curr_state,
        size_t &curr_str_idx)
{
        // auxiliary variables
        char lower  = (char)(curr  & 0b11111111);
        char higher = (char)(curr >> 8);
        bool is_str_beg  = (curr >  0)  && (curr_str_idx == 0);
        bool is_str_end  = (curr == 0) && (curr_str_idx >  0);
        bool is_data_end = (curr == (int16_t)0xffff);

        if (is_str_end) {
                std::cout << "\"\n";
                curr_str_idx = 0;
                return;
        } else if (is_data_end) {
                std::cout << "--- Program Data ---\n";
                curr_state = READING_MNEMONIC;
                return;
        }
        if (is_str_beg)
                std::cout << "\"";
        curr_str_idx += 2;
        if (curr == 0)
                return;
        if (lower == '\n')
                std::cout << "\\n";
        else
                std::cout << lower;
        if (higher == '\n')
                std::cout << "\\n";
        else
                std::cout << higher;
}

void print_entry_label(int16_t curr, Program_State_Enum &curr_state) {
        std::cout << std::setbase(10);
        std::cout << "--- Entry @ " << curr << " ---\n";
        std::cout << "--- String Data ---\n";
        curr_state = READING_STR;
}

void print_mnemonic(
        int16_t curr,
        Program_State_Enum &curr_state,
        size_t &num_args_left,
        int mnemonic_idx,
        int int_idx
) {
        std::stringstream aux_stream;
        std::string aux_string;
        // INS.INT: (OPCODE) MNEMONIC ARG1 ARG2 ARG3
        aux_stream << std::right << std::setw(3) << mnemonic_idx << ".";
        aux_stream << std::right << std::setw(3) << int_idx << ": ";
        aux_stream << "(" << std::right << std::setw(2) <<  curr << ") ";
        aux_stream << std::left << std::setw(6) << DEREFERENCE_TABLE.at(curr);
        std::getline(aux_stream, aux_string);
        std::cout << std::left << std::setw(14) << aux_string;
        num_args_left = BLUEPRINTS.at(DEREFERENCE_TABLE.at(curr)).size() - 1;
        if (num_args_left > 0)
                curr_state = READING_ARGS;
        else
                std::cout << "\n";
}

void CPU_Handle::interpret_program() const {
        Program_State_Enum curr_state = READING_ENTRY_LABEL;
        size_t curr_str_idx = 0;
        size_t num_args_left = 0;
        int16_t header[4] = {
                program_data[0],
                program_data[1],
                program_data[2],
                program_data[3]
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

        // start at 4 to skip magic numbers
        int mnemonic_idx = 0;
        int int_idx = 0;
        for (size_t i = 4; i < prog_size; ++i) {
                int16_t curr = program_data[i];
                switch (curr_state) {
                case NONE:
                        break;
                case READING_ENTRY_LABEL:
                        print_entry_label(curr, curr_state);
                        break;
                case READING_MNEMONIC:
                        print_mnemonic(
                                curr,
                                curr_state,
                                num_args_left,
                                mnemonic_idx,
                                int_idx
                        );
                        mnemonic_idx++;
                        int_idx++;
                        break;
                case READING_ARGS:
                        print_arg(curr, curr_state, num_args_left);
                        int_idx++;
                        break;
                case READING_STR:
                        print_chars(curr, curr_state, curr_str_idx);
                        break;
                default: /* impossible */
                        break;
                }
        }
}
