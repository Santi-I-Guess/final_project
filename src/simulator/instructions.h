#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H 1

#include "cpu_handle.h"

// functions to simulate instructions

void ins_nop(CPU_Handle    &cpu_handle);
void ins_mov(CPU_Handle    &cpu_handle);
void ins_inc(CPU_Handle    &cpu_handle);
void ins_dec(CPU_Handle    &cpu_handle);
void ins_add(CPU_Handle    &cpu_handle);
void ins_sub(CPU_Handle    &cpu_handle);
void ins_mul(CPU_Handle    &cpu_handle);
void ins_div(CPU_Handle    &cpu_handle);
void ins_and(CPU_Handle    &cpu_handle);
void ins_or(CPU_Handle     &cpu_handle);
void ins_not(CPU_Handle    &cpu_handle);
void ins_xor(CPU_Handle    &cpu_handle);
void ins_lsh(CPU_Handle    &cpu_handle);
void ins_rsh(CPU_Handle    &cpu_handle);
void ins_cmp(CPU_Handle    &cpu_handle);
void ins_jmp(CPU_Handle    &cpu_handle);
void ins_jeq(CPU_Handle    &cpu_handle);
void ins_jne(CPU_Handle    &cpu_handle);
void ins_jge(CPU_Handle    &cpu_handle);
void ins_jgr(CPU_Handle    &cpu_handle);
void ins_jle(CPU_Handle    &cpu_handle);
void ins_jls(CPU_Handle    &cpu_handle);
void ins_call(CPU_Handle   &cpu_handle);
void ins_ret(CPU_Handle    &cpu_handle);
void ins_push(CPU_Handle   &cpu_handle);
void ins_pop(CPU_Handle    &cpu_handle);
void ins_write(CPU_Handle  &cpu_handle);
void ins_read(CPU_Handle   &cpu_handle);
void ins_print(CPU_Handle  &cpu_handle);
void ins_sprint(CPU_Handle &cpu_handle);
void ins_exit(CPU_Handle   &cpu_handle);
void update_register(
        CPU_Handle &cpu_handle,
        const int16_t dest,
        const int16_t value
);

#endif
