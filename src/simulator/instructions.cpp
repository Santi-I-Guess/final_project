#include <iostream>
#include <random>
#include <string>
#include <sstream>

#include "../common_values.h"
#include "cpu_handle.h"
#include "instructions.h"

// note to self: maybe don't hardcode values that are easy to mess up?

int16_t clamp(const int16_t value) {
        if (value > LIT_MAX_VALUE)
                return LIT_MAX_VALUE;
        else if (value < LIT_MIN_VALUE)
                return LIT_MIN_VALUE;
        return value;
}

void ins_nop(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        prog_ctr += 1;
}

void ins_mov(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t value = cpu_handle.dereference_value(raw_2);
        update_register(cpu_handle, dest, value);
        prog_ctr += 3;
}

void ins_inc(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        if (dest < 0 || dest > 7) {
                handle_runtime_error(IMMUTABLE_MUTATION);
        }
        int16_t value = cpu_handle.dereference_value(dest) + 1;
        // simulate wrap around
        if (value == LIT_MAX_VALUE + 1) {
                value = LIT_MIN_VALUE;
        }
        update_register(cpu_handle, dest, value);

        prog_ctr += 2;
}

void ins_dec(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        if (dest < 0 || dest > 7) {
                handle_runtime_error(IMMUTABLE_MUTATION);
        }
        int16_t value = cpu_handle.dereference_value(dest) - 1;
        // simulate wrap around
        if (value == LIT_MIN_VALUE - 1) {
                value = LIT_MAX_VALUE;
        }
        update_register(cpu_handle, dest, value);

        prog_ctr += 2;
}

void ins_add(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        int16_t value = src_1 + src_2;
        update_register(cpu_handle, dest, value);

        prog_ctr += 4;
}

void ins_sub(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        int16_t value = src_1 - src_2;
        update_register(cpu_handle, dest, value);
        prog_ctr += 4;
}

void ins_mul(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        // ensure 16 bit overflow keeps the right sign before clamp
        bool expected_positive = (src_1 >= 0) == (src_2 >= 0);
        int32_t raw_value = src_1 * src_2;
        if (raw_value < 0 && expected_positive)
                raw_value *= -1;
        else if (raw_value > 0 && !expected_positive)
                raw_value *= -1;
        // ensure 16 bit overflow clamps to expected value
        if (raw_value > LIT_MAX_VALUE)
                raw_value = LIT_MAX_VALUE;
        else if (raw_value < LIT_MIN_VALUE)
                raw_value = LIT_MIN_VALUE;
        int16_t value = (int16_t)raw_value;
        update_register(cpu_handle, dest, value);
        prog_ctr += 4;
}

void ins_div(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        if (src_2 == 0) {
                std::cout << "Warning: Division by Zero. Result will be 0\n";
        }
        src_1 = (src_2 != 0 ? src_1 : (int16_t)0);
        src_2 = (src_2 != 0 ? src_2 : (int16_t)1);
        int16_t value = src_1 / src_2;
        update_register(cpu_handle, dest, value);

        prog_ctr += 4;
}

void ins_mod(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        if (src_2 == 0) {
                std::cout << "Warning: Mod by Zero. Result will be 0\n";
        }
        src_1 = (src_2 != 0 ? src_1 : (int16_t)0);
        src_2 = (src_2 != 0 ? src_2 : (int16_t)1);
        int16_t value = src_1 % src_2;
        update_register(cpu_handle, dest, value);

        prog_ctr += 4;
}

void ins_and(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        int16_t value = src_1 & src_2;
        update_register(cpu_handle, dest, value);

        prog_ctr += 4;
}

void ins_or(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        int16_t value = src_1 | src_2;
        update_register(cpu_handle, dest, value);
        prog_ctr += 4;
}

void ins_not(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.dereference_value(prog_ctr + 2);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t value = ~src_1;
        update_register(cpu_handle, dest, value);
        prog_ctr += 3;
}

void ins_xor(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        int16_t value = src_1 ^ src_2;
        update_register(cpu_handle, dest, value);
        prog_ctr += 4;
}

void ins_lsh(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        if (src_2 < 0) {
                std::cout << "Warning: Negative Bitshift. Result will be src 1\n";
        }

        int16_t value = src_1 << (src_2 < 0 ? 0 : src_2);
        update_register(cpu_handle, dest, value);

        prog_ctr += 4;
}

void ins_rsh(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;

        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t raw_3 = cpu_handle.get_program_data(prog_ctr + 3);
        int16_t src_1 = cpu_handle.dereference_value(raw_2);
        int16_t src_2 = cpu_handle.dereference_value(raw_3);
        if (src_2 < 0)
                std::cout << "Warning: Negative Bitshift. Result will be src 1\n";
        int16_t value = src_1 >> (src_2 > 0 ? src_2 : 0);
        update_register(cpu_handle, dest, value);
        prog_ctr += 4;
}

void ins_cmp(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;

        int16_t raw = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t src_1 = cpu_handle.dereference_value(raw);
        int16_t src_2 = cpu_handle.dereference_value(raw_2);
        src_1 = clamp(src_1);
        src_2 = clamp(src_2);
        reg_cmp_a = src_1;
        reg_cmp_b = src_2;

        prog_ctr += 3;
}

void ins_jmp(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        prog_ctr = new_address;
        // always jumps, so INSTRUCTION_LENS[16] unneeded
}

void ins_jeq(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (reg_cmp_a == reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += 2;
}

void ins_jne(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (reg_cmp_a != reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += 2;
}

void ins_jge(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (reg_cmp_a >= reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += 2;
}

void ins_jgr(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (reg_cmp_a >  reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += 2;
}

void ins_jle(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (reg_cmp_a <= reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += 2;
}

void ins_jls(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &reg_cmp_a = cpu_handle.reg_cmp_a;
        int16_t &reg_cmp_b = cpu_handle.reg_cmp_b;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (reg_cmp_a < reg_cmp_b)
                prog_ctr = new_address;
        else
                prog_ctr += 2;
}

void ins_call(CPU_Handle &cpu_handle) {
        // points to next instruction, not current
        int16_t *call_stack = cpu_handle.call_stack;
        int16_t &call_stack_ptr = cpu_handle.call_stack_ptr;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t new_address = cpu_handle.get_program_data(prog_ctr + 1);
        if (call_stack_ptr < CALL_STACK_SIZE) {
                call_stack[call_stack_ptr] = prog_ctr + 2;
                call_stack_ptr++;
                prog_ctr = new_address;
        } else {
                handle_runtime_error(CALL_STACK_UNDERFLOW);
        }
}

void ins_ret(CPU_Handle &cpu_handle) {
        int16_t *call_stack = cpu_handle.call_stack;
        int16_t &call_stack_ptr = cpu_handle.call_stack_ptr;
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        if (call_stack_ptr > 0) {
                call_stack_ptr--;
                int16_t new_address = call_stack[call_stack_ptr];
                prog_ctr = new_address;
                // prog_ctr points to next instruction, so INSTRUCTION_LENS[24] unneeded
        } else {
                handle_runtime_error(CALL_STACK_UNDERFLOW);
        }
}

void ins_push(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &stack_ptr = cpu_handle.stack_ptr;
        int16_t *program_mem = cpu_handle.program_mem;
        if (stack_ptr == STACK_SIZE) {
                handle_runtime_error(STACK_OVERFLOW);
        }
        int16_t raw = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t value = cpu_handle.dereference_value(raw);
        value = clamp(value);
        program_mem[STACK_START + stack_ptr] = value;
        stack_ptr++;

        prog_ctr += 2;
}

void ins_pop(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &stack_ptr = cpu_handle.stack_ptr;
        int16_t *program_mem = cpu_handle.program_mem;

        if (stack_ptr <= 0) {
                handle_runtime_error(STACK_UNDERFLOW);
        }

        stack_ptr--;
        int16_t value = program_mem[STACK_START + stack_ptr];
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        update_register(cpu_handle, dest, value);

        prog_ctr += 2;
}

void ins_write(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_mem = cpu_handle.program_mem;
        int16_t raw = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t value = cpu_handle.dereference_value(raw);
        value = clamp(value);
        int16_t raw_2 = cpu_handle.get_program_data(prog_ctr + 2);
        int16_t address = cpu_handle.dereference_value(raw_2);
        if (address < 0 || address >= STACK_START) {
                handle_runtime_error(OOB_ADDRESS);
        }
        program_mem[address] = value;
        prog_ctr += 3;
}

void ins_read(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t *program_mem = cpu_handle.program_mem;
        int16_t dest = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t address = cpu_handle.dereference_value(cpu_handle.get_program_data(prog_ctr + 2));
        if (address < 0 || address >= STACK_START) {
                handle_runtime_error(OOB_ADDRESS);
        }
        int16_t value = program_mem[address];
        update_register(cpu_handle, dest, value);
        prog_ctr += 3;
}

void ins_print(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t raw = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t value = cpu_handle.dereference_value(raw);
        std::cout << value;
        prog_ctr += 2;
}

void ins_sprint(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t raw = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t temp_str_idx = cpu_handle.dereference_value(raw);
        while (cpu_handle.get_program_data(temp_str_idx) != (int16_t)0) {
                int16_t curr = cpu_handle.get_program_data(temp_str_idx);
                char lower = (char)(curr & 255);
                char higher = (char)(curr >> 8);
                std::cout << lower << higher;
                temp_str_idx++;
        }

        prog_ctr += 2;
}

void ins_cprint(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t raw = cpu_handle.get_program_data(prog_ctr + 1);
        int16_t value = cpu_handle.dereference_value(raw);
        if (value < 0 || value > 127) {
                handle_runtime_error(ASCII_ERROR);
        }
        std::cout << (char)value;

        prog_ctr += 2;
}

void ins_input(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &stack_ptr = cpu_handle.stack_ptr;
        int16_t *program_mem = cpu_handle.program_mem;
        if (stack_ptr == STACK_SIZE) {
                handle_runtime_error(STACK_OVERFLOW);
        }

        int16_t value;
        std::string user_input;
        std::getline(std::cin, user_input);
        if (user_input.length() == 0) {
                // empty input interpreted as newline (ascii value 10)
                user_input = "\n";
        }

        bool is_ascii_input = (user_input.length() == 1) && isascii(user_input.at(0));
        if (!isdigit(user_input.at(0)) && is_ascii_input) {
                // if user inputs an asciiable, interpret it's ascii value
                value = (int16_t)((char)user_input.at(0));
        } else {
                // else, interpret as an integer
                std::stringstream aux_stream;
                aux_stream << user_input;
                aux_stream >> value;
                if (aux_stream.fail())
                        handle_runtime_error(INPUT_ERROR);
        }
        value = clamp(value);
        program_mem[STACK_START + stack_ptr] = value;
        stack_ptr++;
        prog_ctr += 1;
}


void ins_rand(CPU_Handle   &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        int16_t &stack_ptr = cpu_handle.stack_ptr;
        int16_t *program_mem = cpu_handle.program_mem;
        if (stack_ptr == STACK_SIZE) {
                handle_runtime_error(STACK_OVERFLOW);
        }

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int16_t> generator((int16_t)-100, (int16_t)100);
        program_mem[STACK_START + stack_ptr] = generator(mt);
        stack_ptr++;
        prog_ctr += 1;
}

void ins_exit(CPU_Handle &cpu_handle) {
        int16_t &prog_ctr = cpu_handle.prog_ctr;
        prog_ctr += 1;
}

void update_register(
        CPU_Handle &cpu_handle,
        const int16_t dest,
        const int16_t value)
{
        if (dest < 0 || dest > 7) {
                handle_runtime_error(IMMUTABLE_MUTATION);
        }
        int16_t clamped_value = clamp(value);
        switch (dest) {
                case 0: cpu_handle.reg_a = clamped_value; break;
                case 1: cpu_handle.reg_b = clamped_value; break;
                case 2: cpu_handle.reg_c = clamped_value; break;
                case 3: cpu_handle.reg_d = clamped_value; break;
                case 4: cpu_handle.reg_e = clamped_value; break;
                case 5: cpu_handle.reg_f = clamped_value; break;
                case 6: cpu_handle.reg_g = clamped_value; break;
                case 7: cpu_handle.reg_h = clamped_value; break;
                default: break; /* impossible */
        }
}
