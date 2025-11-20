#ifndef AUXILIARY_H
#define AUXILIARY_H 1

#include "assembler/common_values.h"

/**
 * @brief prints info and exits if assembler failed
 * @details helper function for main.cpp
 */
void handle_generation_res(Debug_Info res);

/**
 * @brief prints info and exits if grammar checker caught an error
 * @details helper function for main.cpp
 */
void handle_grammar_check_res(Debug_Info res);

#endif
