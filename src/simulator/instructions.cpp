#include <iostream>

#include "../common_values.h"
#include "cpu_handle.h"
#include "instructions.h"

// note to self: maybe don't hardcode values that are easy to mess up?

void ins_nop(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        prog_ctr += INSTRUCTION_LENS[0];
}

void ins_mov(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_data = cpu_handle.program_data;
        int16_t dest = program_data[prog_ctr + 1];
        int16_t value = cpu_handle.dereference_value(program_data[prog_ctr + 2]);

        update_register(cpu_handle, dest, value);
        prog_ctr += INSTRUCTION_LENS[1];
}

void ins_inc(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = program_data[prog_ctr + 1];

        if (dest < 0 || dest > 7) {
                std::cout << "Error: " << error_messages[0] << "\n";
                std::exit(1);
        }
        switch (dest) {
                case 0: cpu_handle.reg_a++; break;
                case 1: cpu_handle.reg_b++; break;
                case 2: cpu_handle.reg_c++; break;
                case 3: cpu_handle.reg_d++; break;
                case 4: cpu_handle.reg_e++; break;
                case 5: cpu_handle.reg_f++; break;
                case 6: cpu_handle.reg_g++; break;
                case 7: cpu_handle.reg_h++; break;
                default: break; /* impossible */
        }

        prog_ctr += INSTRUCTION_LENS[2];
}

void ins_dec(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = program_data[prog_ctr + 1];

        if (dest < 0 || dest > 7) {
                std::cout << "Error: " << error_messages[0] << "\n";
                std::exit(1);
        }
        switch (dest) {
                case 0: cpu_handle.reg_a--; break;
                case 1: cpu_handle.reg_b--; break;
                case 2: cpu_handle.reg_c--; break;
                case 3: cpu_handle.reg_d--; break;
                case 4: cpu_handle.reg_e--; break;
                case 5: cpu_handle.reg_f--; break;
                case 6: cpu_handle.reg_g--; break;
                case 7: cpu_handle.reg_h--; break;
                default: break; /* impossible */
        }

        prog_ctr += INSTRUCTION_LENS[3];
}

void ins_add(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        int16_t value = src_1 + src_2;

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[4];
}

void ins_sub(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        int16_t value = src_1 - src_2;

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[5];
}

void ins_mul(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        int16_t value = src_1 * src_2;

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[6];
}

void ins_div(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        if (src_2 == 0) {
                std::cout << "Warning: Division by Zero. Result will be 0\n";
        }
        src_1 = (src_2 != 0 ? src_1 : (int16_t)0);
        src_2 = (src_2 != 0 ? src_2 : (int16_t)1);
        int16_t value = src_1 / src_2;
        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[7];
}

void ins_and(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        int16_t value = src_1 & src_2;

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[8];
}

void ins_or(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        int16_t value = src_1 | src_2;
        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[9];
}

void ins_not(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t value = ~src_1;

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[10];
}

void ins_xor(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        int16_t value = src_1 ^ src_2;

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[11];
}

void ins_lsh(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        if (src_2 < 0) {
                std::cout << "Warning: Negative Bitshift. Result will be src 1\n";
        }

        int16_t value = src_1 << (src_2 < 0 ? 0 : src_2);
        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[12];
}

void ins_rsh(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 3]);
        if (src_2 < 0)
                std::cout << "Warning: Negative Bitshift. Result will be src 1\n";
        int16_t value = src_1 >> (src_2 > 0 ? src_2 : 0);
        update_register(cpu_handle, dest, value);
        prog_ctr += INSTRUCTION_LENS[13];
}

void ins_cmp(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;

        int16_t src_1 = cpu_handle.dereference_value(program_data[prog_ctr + 1]);
        int16_t src_2 = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        reg_cmp_a = src_1;
        reg_cmp_b = src_2;

        prog_ctr += INSTRUCTION_LENS[14];
}

void ins_jmp(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t new_address = program_data[prog_ctr + 1];
        prog_ctr = new_address;
        // always jumps, so INSTRUCTION_LENS[15] unneeded
}

void ins_jeq(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = program_data[prog_ctr + 1];
        if (reg_cmp_a == reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += INSTRUCTION_LENS[16];
}

void ins_jne(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = program_data[prog_ctr + 1];
        if (reg_cmp_a != reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += INSTRUCTION_LENS[17];
}

void ins_jge(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = program_data[prog_ctr + 1];
        if (reg_cmp_a >= reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += INSTRUCTION_LENS[18];
}

void ins_jgr(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = program_data[prog_ctr + 1];
        if (reg_cmp_a >  reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += INSTRUCTION_LENS[19];
}

void ins_jle(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = program_data[prog_ctr + 1];
        if (reg_cmp_a <= reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += INSTRUCTION_LENS[20];
}

void ins_jls(CPU_Handle &cpu_handle) {
        int16_t *program_data = cpu_handle.program_data;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = program_data[prog_ctr + 1];
        if (reg_cmp_a < reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += INSTRUCTION_LENS[21];
}

void ins_call(CPU_Handle &cpu_handle) {
        // points to next instruction, not current
        int16_t *call_stack = cpu_handle.call_stack;
        int16_t *program_data = cpu_handle.program_data;
        int16_t &call_stack_ptr = cpu_handle.call_stack_ptr;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t new_address = program_data[prog_ctr + 1];
        call_stack[call_stack_ptr] = prog_ctr + INSTRUCTION_LENS[22];
        call_stack_ptr++;
        prog_ctr = new_address;
}

void ins_ret(CPU_Handle &cpu_handle) {
        int16_t *call_stack = cpu_handle.call_stack;
        int16_t &call_stack_ptr = cpu_handle.call_stack_ptr;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        call_stack_ptr--;
        int16_t new_address = call_stack[call_stack_ptr];
        prog_ctr = new_address;
        // prog_ctr points to next instruction, so INSTRUCTION_LENS[23] unneeded
}

void ins_push(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &stack_ptr = cpu_handle.stack_ptr;
        int16_t *program_data = cpu_handle.program_data;
        int16_t *program_mem = cpu_handle.program_mem;
        if (stack_ptr == 2047) {
                std::cout << "Error: " << error_messages[4] << "\n";
                std::exit(1);
        }
        int16_t argument = cpu_handle.dereference_value(program_data[prog_ctr + 1]);
        program_mem[1536 + stack_ptr] = argument;
        stack_ptr++;

        prog_ctr += INSTRUCTION_LENS[24];
}

void ins_pop(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &stack_ptr = cpu_handle.stack_ptr;
        int16_t *program_data = cpu_handle.program_data;
        int16_t *program_mem = cpu_handle.program_mem;

        if (stack_ptr == 0) {
                std::cout << "Error: " << error_messages[4] << "\n";
                std::exit(1);
        }

        stack_ptr--;
        int16_t value = program_mem[1536 + stack_ptr];
        int16_t dest = program_data[prog_ctr + 1];

        update_register(cpu_handle, dest, value);

        prog_ctr += INSTRUCTION_LENS[25];
}

void ins_write(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_data = cpu_handle.program_data;
        int16_t *program_mem = cpu_handle.program_mem;

        int16_t source = cpu_handle.dereference_value(program_data[prog_ctr + 1]);
        int16_t address = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        if (address < 0 || address > 1535) {
                std::cout << "Error: " << error_messages[5] << "\n";
                std::exit(1);
        }
        program_mem[address] = source;

        prog_ctr += INSTRUCTION_LENS[26];
}

void ins_read(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_data = cpu_handle.program_data;
        int16_t *program_mem = cpu_handle.program_mem;

        int16_t dest = program_data[prog_ctr + 1];
        int16_t address = cpu_handle.dereference_value(program_data[prog_ctr + 2]);
        if (address < 0 || address > 1535) {
                std::cout << "Error: " << error_messages[5] << "\n";
                std::exit(1);
        }

        int16_t value = program_mem[address];
        switch (dest) {
                case 0: cpu_handle.reg_a = value; break;
                case 1: cpu_handle.reg_b = value; break;
                case 2: cpu_handle.reg_c = value; break;
                case 3: cpu_handle.reg_d = value; break;
                case 4: cpu_handle.reg_e = value; break;
                case 5: cpu_handle.reg_f = value; break;
                case 6: cpu_handle.reg_g = value; break;
                case 7: cpu_handle.reg_h = value; break;
                default: break; /* impossible */
        }

        prog_ctr += INSTRUCTION_LENS[27];
}

void ins_print(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_data = cpu_handle.program_data;

        int16_t value = cpu_handle.dereference_value(program_data[prog_ctr + 1]);
        std::cout << value;

        prog_ctr += INSTRUCTION_LENS[28];
}

void ins_sprint(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_data = cpu_handle.program_data;

        int16_t temp_str_idx = cpu_handle.dereference_value(program_data[prog_ctr + 1]);
        while (program_data[temp_str_idx] != (int16_t)0) {
                int16_t curr = program_data[temp_str_idx];
                char lower = (char)(curr & 255);
                char higher = (char)(curr >> 8);
                std::cout << lower << higher;
                temp_str_idx++;
        }

        prog_ctr += INSTRUCTION_LENS[29];
}

void ins_exit(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        prog_ctr += INSTRUCTION_LENS[30];
}

void update_register(
        CPU_Handle &cpu_handle,
        const int16_t dest,
        const int16_t value)
{
        if (dest < 0 || dest > 7) {
                std::cout << "Error: " << error_messages[0] << "\n";
                std::exit(1);
        }
        switch (dest) {
                case 0: cpu_handle.reg_a = value; break;
                case 1: cpu_handle.reg_b = value; break;
                case 2: cpu_handle.reg_c = value; break;
                case 3: cpu_handle.reg_d = value; break;
                case 4: cpu_handle.reg_e = value; break;
                case 5: cpu_handle.reg_f = value; break;
                case 6: cpu_handle.reg_g = value; break;
                case 7: cpu_handle.reg_h = value; break;
                default: break; /* impossible */
        }
}
