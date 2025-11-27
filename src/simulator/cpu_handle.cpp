#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iostream>

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
