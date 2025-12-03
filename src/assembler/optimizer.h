#ifndef OPTIMIZER_H
#define OPTIMIZER_H 1

#include <cstdint>
#include <vector>

/**
 * @brief applies peephole optimizations in place
 * @details will apply strength reduction, null sequence elimination,
 * and deadcode elimination
 */
void peephole_optimize_program(std::vector<int16_t> &program);

/**
 * @brief generate constant folded value for MOV 
 * @details helper function of peephole_optimize_program
 */
int16_t get_folded_value(
        const int16_t opcode,
        const int16_t arg_1,
        const int16_t arg_2
);


/**
 * @brief check if instruction can be replaced with MOV
 * @details helper function of peephole_optimize_program
 */
bool can_constant_fold(const std::vector<int16_t> instruction);


/**
 * @brief check if instruction can be replaced with MOV
 * @details helper function of peephole_optimize_program
 */
bool can_reduce_strength(const std::vector<int16_t> instruction);

/**
 * @brief check if instruction can be deleted
 * @details helper function of peephole_optimize_program
 */
bool is_null_sequence(const std::vector<int16_t> instruction);

#endif
