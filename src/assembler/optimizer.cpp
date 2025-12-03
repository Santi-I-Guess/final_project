#include <cstdint>
#include <iostream>
#include <vector>

#include "../common_values.h"
#include "optimizer.h"

/**
* @brief enum for quick opcode checking, in order
*/
enum Pseudo_Opcode {
         NOP=0, MOV,   INC,    DEC,
         ADD,   SUB,   MUL,    DIV,
         MOD,   AND,   OR,     NOT,
         XOR,   LSH,   RSH,    CMP,
         JMP,   JEQ,   JNE,    JGE,
         JGR,   JLE,   JLS,    CALL,
         RET,   PUSH,  POP,    WRITE,
         READ,  PRINT, SPRINT, CPRINT,
         INPUT, EXIT, 
};

void peephole_optimize_program(std::vector<int16_t> &program) {
        // grammar checker ensures program will be valid beforehand
        std::vector<int16_t> fixed_program = {};
        // move prog_ctr to first instruction
        int16_t prog_ctr = 0;
        fixed_program.push_back(program.at(prog_ctr));
        prog_ctr++;
        while (program.at(prog_ctr - 1) != (int16_t)0xffff) {
                fixed_program.push_back(program.at(prog_ctr));
                prog_ctr++;
        }
        while (prog_ctr < (int16_t)program.size()) {
                int16_t opcode = program.at(prog_ctr);
                std::vector<int16_t> instruction;
                for (int16_t i = 0; i < INSTRUCTION_LENS[opcode]; ++i) {
                        instruction.push_back(program.at(prog_ctr + i));
                }

                if (is_null_sequence(instruction)) {
                        std::cout << "\x1b[32mApplied null sequence deletion\x1b[0m\n";
                        prog_ctr += INSTRUCTION_LENS[opcode];
                        continue;
                } else if (can_constant_fold(instruction)) {
                        int16_t dest = instruction.at(1);
                        int16_t src_1 = instruction.at(2);
                        int16_t src_2 = instruction.at(3);
                        int16_t target_value = get_folded_value(opcode, src_1, src_2);
                        fixed_program.push_back((int16_t)MOV);
                        fixed_program.push_back(dest);
                        fixed_program.push_back(target_value);
                        std::cout << "\x1b[32mApplied constant folding\x1b[0m\n";
                        prog_ctr += INSTRUCTION_LENS[opcode];
                        continue;
                } else if (can_reduce_strength(instruction)) {
                        int16_t dest = instruction.at(1);
                        int16_t src_1 = instruction.at(2);
                        int16_t src_2 = instruction.at(3);
                        if (opcode == (int16_t)ADD) {
                                fixed_program.push_back((int16_t)LSH);
                                fixed_program.push_back(dest);
                                fixed_program.push_back(src_1);
                                fixed_program.push_back((int16_t)((1 << 14) + 1));
                        } else if (opcode == (int16_t)MUL) {
                                int16_t value = src_2 ^ (int16_t)(1 << 14);
                                int16_t shift = 0;
                                for (int16_t i = 1; i < 13; ++i) {
                                        if ((value >> i) & 1)
                                                shift = i;
                                }
                                fixed_program.push_back((int16_t)LSH);
                                fixed_program.push_back(src_1);
                                fixed_program.push_back(dest);
                                fixed_program.push_back((int16_t)((1 << 14) + shift));
                        } else if (opcode == (int16_t)DIV) {
                                int16_t value = src_2 ^ (int16_t)(1 << 14);
                                int16_t shift = 0;
                                for (int16_t i = 1; i < 13; ++i) {
                                        if ((value >> i) & 1)
                                                shift = i;
                                }
                                fixed_program.push_back((int16_t)RSH);
                                fixed_program.push_back(src_1);
                                fixed_program.push_back(dest);
                                fixed_program.push_back((int16_t)((1 << 14) + shift));
                        }
                        std::cout << "\x1b[32mApplied strength reduction\x1b[0m\n";
                        prog_ctr += INSTRUCTION_LENS[opcode];
                        continue;
                }
                for (int16_t i = 0; i < INSTRUCTION_LENS[opcode]; i++) {
                        int16_t curr = program.at(prog_ctr);
                        fixed_program.push_back(curr);
                        prog_ctr++;
                }
        }
        program.swap(fixed_program);
}

bool is_null_sequence(const std::vector<int16_t> instruction) {
        int opcode = instruction.at(0);
        int16_t dest, src_1, src_2; // auxiliary
        switch ((Pseudo_Opcode)opcode) {
        case NOP:
                return true;
        case MOV:
                dest = instruction.at(1);
                src_1 = instruction.at(2);
                return dest == src_1;
        case INC:
        case DEC:
                return false;
        case ADD:
        case SUB:
                // add or sub by literal value zero
                dest = instruction.at(1);
                src_1 = instruction.at(2);
                src_2 = instruction.at(3);
                if (src_1 == dest && src_2 == (int16_t)(1 << 14))
                        return true;
                if (src_1 == (int16_t)(1 << 14) && src_2 == dest)
                        return true;
                return false;
        case MUL:
        case DIV:
                // mul or div by literal value 1
                dest = instruction.at(1);
                src_1 = instruction.at(2);
                src_2 = instruction.at(3);
                if (src_1 == dest && src_2 == (int16_t)((1 << 14) + 1))
                        return true;
                if (src_1 == (int16_t)((1 << 14) + 1) && src_2 == dest)
                        return true;
                return false;
        case MOD:
                return false;
        case AND:
        case OR:
                // and/or by itself
                dest = instruction.at(1);
                src_1 = instruction.at(2);
                src_2 = instruction.at(3);
                if (src_1 == src_2 && src_1 == dest)
                        return true;
                return false;

                break; 
        case NOT:
        case XOR:
        case LSH:
        case RSH:
        case CMP:
        case JMP:
        case JEQ:
        case JNE:
        case JGE:
        case JGR:
        case JLE:
        case JLS:
        case CALL:
        case RET:
        case PUSH:
        case POP:
        case WRITE:
        case READ:
        case PRINT:
        case SPRINT:
        case CPRINT:
        case INPUT:
        case EXIT:
                return false;
        }
        return false; /* impossible */
}

bool can_constant_fold(const std::vector<int16_t> instruction) {
        int opcode = instruction.at(0);
        int16_t src_1, src_2;
        switch ((Pseudo_Opcode)opcode) {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case MOD:
        case AND:
        case OR:
        case XOR:
        case LSH:
        case RSH:
                src_1 = instruction.at(2);
                src_2 = instruction.at(3);
                return (((src_1 >> 14) & 1) && ((src_2 >> 14) & 1));
        case NOP:
        case INC:
        case DEC:
        case MOV:
        case NOT:
        case CMP:
        case JMP:
        case JEQ:
        case JNE:
        case JGE:
        case JGR:
        case JLE:
        case JLS:
        case CALL:
        case RET:
        case PUSH:
        case POP:
        case WRITE:
        case READ:
        case PRINT:
        case SPRINT:
        case CPRINT:
        case INPUT:
        case EXIT:
                return false;
        }
        return false; /* impossible */
}


// Y = X + X -> Y = X << 1
// Y = A * 2^N -> Y = A << N
// Y = A / 2^N -> Y = A >> N
bool can_reduce_strength(const std::vector<int16_t> instruction) {
        int opcode = instruction.at(0);
        int16_t src_1, src_2;
        switch ((Pseudo_Opcode)opcode) {
        case ADD:
                // if both sources are the same register
                src_1 = instruction.at(2);
                src_2 = instruction.at(3);
                if (src_1 != (src_1 & 15))
                        return false;
                return src_1 == src_2;
        case MUL:
        case DIV:
                // if src_2 is a power of 2
                src_2 = instruction.at(3);
                if ((src_2 >> 14) & 1) {
                        src_2 ^= (int16_t)(1 << 14);
                        return (src_2 & (src_2 - 1)) == 0;
                } else {
                        return false;
                }
        case SUB:
        case MOD:
        case AND:
        case OR:
        case XOR:
        case LSH:
        case RSH:
        case NOP:
        case INC:
        case DEC:
        case MOV:
        case NOT:
        case CMP:
        case JMP:
        case JEQ:
        case JNE:
        case JGE:
        case JGR:
        case JLE:
        case JLS:
        case CALL:
        case RET:
        case PUSH:
        case POP:
        case WRITE:
        case READ:
        case PRINT:
        case SPRINT:
        case CPRINT:
        case INPUT:
        case EXIT:
                return false;
        }
        return false; /* impossible */

}

int16_t get_folded_value(
        const int16_t opcode,
        const int16_t arg_1,
        const int16_t arg_2
) {
        int16_t target_value = 0;
        int16_t value_1 = arg_1 ^ (int16_t)(1 << 14);
        int16_t value_2 = arg_2 ^ (int16_t)(1 << 14);
        if (opcode == (int16_t)ADD) {
                target_value = value_1 + value_2;
        } else if (opcode == (int16_t)SUB) {
                target_value = value_1 - value_2;
        } else if (opcode == (int16_t)MUL) {
                target_value = value_1 * value_2;
        } else if (opcode == (int16_t)DIV) {
                target_value = value_1 / value_2;
        } else if (opcode == (int16_t)MOD) {
                target_value = value_1 % value_2;
        } else if (opcode == (int16_t)AND) {
                target_value = value_1 & value_2;
        } else if (opcode == (int16_t)OR) {
                target_value = value_1 | value_2;
        } else if (opcode == (int16_t)XOR) {
                target_value = value_1 ^ value_2;
        } else if (opcode == (int16_t)LSH) {
                target_value = value_1 << value_2;
        } else if (opcode == (int16_t)RSH) {
                target_value = value_1 >> value_2;
        }
        if (target_value < -4096)
                target_value = -4096;
        if (target_value > 4096)
                target_value = 4096;
        target_value |= (int16_t)(1 << 14); // apply bitmask
        return target_value;
}
