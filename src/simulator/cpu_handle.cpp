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

void print_instruction(
        int16_t curr,
        int16_t &mnemonic_idx,
        int16_t &int_idx,
        int16_t *program_data
) {
        std::stringstream out_stream;
        std::string out_string;
        // INS.INT: (OPCODE) MNEMONIC ARG1 ARG2 ARG3

        // note to self: std::getline sets EOF bit of stream

        // set instruction part to be left aligned with 14 chars
        std::string mnen_name = DEREFERENCE_TABLE.at(curr);
        out_stream << std::right << std::setw(3) << mnemonic_idx << ".";
        out_stream << std::right << std::setw(3) << int_idx << ": (";
        out_stream << std::right << std::setw(2) << curr << ") ";
        out_stream << std::left  << std::setw(6) << mnen_name;
        out_string = out_stream.str();
        int_idx++;

        // set each argument to be right aligned with 7 chars
        int16_t num_args = (int16_t)(BLUEPRINTS.at(mnen_name).size() - 1);
        for (int i = 0; i < num_args; ++i) {
                std::stringstream arg_stream;
                std::string arg_string;
                int16_t curr_arg = program_data[int_idx + i];
                if ((curr_arg >> 14) & 1) {
                        // literal bitmask
                        if (curr_arg >= 0)
                                curr_arg ^= (int16_t)(1 << 14);
                        arg_stream << "$";
                } else if ((curr_arg >> 13) & 1) {
                        // stack offset bitmask
                        curr_arg ^= (int16_t)(1 << 13);
                        arg_stream << "%";
                }
                arg_stream << curr_arg;
                arg_string = arg_stream.str();
                out_stream << std::right << std::setw(7) << arg_string;
                int_idx++;
        }

        // print instruction buffer
        out_string = out_stream.str();
        std::cout << out_string << "\n";
        mnemonic_idx++;
}

void CPU_Handle::interpret_program() const {
        Program_State_Enum curr_state = READING_ENTRY_LABEL;
        size_t curr_str_idx = 0;
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

        // I mean, after it hits the first mnemonic, it's expected that
        // it's only going to see mnemonics afterwards

        // start at 4 to skip magic numbers
        int16_t mnemonic_idx = 0;
        int16_t int_idx = 4;
        while (int_idx < (int16_t)prog_size) {
                int16_t curr = program_data[int_idx];
                switch (curr_state) {
                case NONE:
                        break;
                case READING_ENTRY_LABEL:
                        std::cout << "--- Entry @ " << curr << " ---\n";
                        std::cout << "--- String Data ---\n";
                        curr_state = READING_STR;
                        int_idx++;
                        break;
                case READING_MNEMONIC:
                        // print_instruction uses reference for int_idx
                        print_instruction(
                                curr,
                                mnemonic_idx,
                                int_idx,
                                program_data
                        );
                        break;
                case READING_STR:
                        print_chars(curr, curr_state, curr_str_idx);
                        int_idx++;
                        break;
                default: /* impossible */
                        break;
                }
        }
        std::cout << "\n";
}
