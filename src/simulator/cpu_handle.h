#ifndef CPU_HANDLE_H
#define CPU_HANDLE_H 1

#include <cstdint>
#include <map>
#include <string>
#include <vector>

const std::string error_messages[7] = {
        "attemped to mutate immutable destination",
        "attemped to access stack by offset before stack base",
        "attemped to access unknown register",
        "attemped to push past stack bounds",
        "attemped to push before stack bounds",
        "attemped to access out of bounds memory",
        "attempted to print invalid ascii character"
};

/**
 * @brief describes current state of program
 * @details is also used in program interpretation
 */
enum Program_State_Enum {
        NONE,
        READING_ENTRY_LABEL,
        READING_STR,
        READING_MNEMONIC,
};

/**
 * @brief Container class for memory during program simulation
 * @details last 512 int's are reserved for the program's stack,
 * which therefore occupies program_mem's indices 1536-2047
 */
class CPU_Handle {
        // general purpose registers
        int16_t reg_a; /** general purpose register */
        int16_t reg_b; /** general purpose register */
        int16_t reg_c; /** general purpose register */
        int16_t reg_d; /** general purpose register */
        int16_t reg_e; /** general purpose register */
        int16_t reg_f; /** general purpose register */
        int16_t reg_g; /** general purpose register */
        int16_t reg_h; /** general purpose register */
        int16_t reg_cmp_a; /** comparison register, inaccessable */
        int16_t reg_cmp_b; /** comparison register, inaccessable */
        int16_t prog_ctr; /** program counter / instruction pointer */
        int16_t stack_ptr; /** stack counter / pointer */
        int16_t call_stack_ptr;
        int16_t call_stack[1024]; /** holds returns for call stack */
        int16_t program_mem[2048]; /** holds ram and stack memory */
public:
        int16_t *program_data; /** assembled program */
        int16_t prog_size; /** size of program data */
        CPU_Handle();
        ~CPU_Handle();
        int16_t dereference_value(const int16_t given_value);
        void load_program(const std::vector<int16_t> given_program);
        void next_instruction(bool &hit_exit);
        void run_program();
        void run_program_debug();

        // needs access to private members, but won't be member method for reasons
        friend void ins_nop(CPU_Handle &cpu_handle);
        friend void ins_mov(CPU_Handle &cpu_handle);
        friend void ins_inc(CPU_Handle &cpu_handle);
        friend void ins_dec(CPU_Handle &cpu_handle);
        friend void ins_add(CPU_Handle &cpu_handle);
        friend void ins_sub(CPU_Handle &cpu_handle);
        friend void ins_mul(CPU_Handle &cpu_handle);
        friend void ins_div(CPU_Handle &cpu_handle);
        friend void ins_and(CPU_Handle &cpu_handle);
        friend void ins_or(CPU_Handle &cpu_handle);
        friend void ins_not(CPU_Handle &cpu_handle);
        friend void ins_xor(CPU_Handle &cpu_handle);
        friend void ins_lsh(CPU_Handle &cpu_handle);
        friend void ins_rsh(CPU_Handle &cpu_handle);
        friend void ins_cmp(CPU_Handle &cpu_handle);
        friend void ins_jmp(CPU_Handle &cpu_handle);
        friend void ins_jeq(CPU_Handle &cpu_handle);
        friend void ins_jne(CPU_Handle &cpu_handle);
        friend void ins_jge(CPU_Handle &cpu_handle);
        friend void ins_jgr(CPU_Handle &cpu_handle);
        friend void ins_jle(CPU_Handle &cpu_handle);
        friend void ins_jls(CPU_Handle &cpu_handle);
        friend void ins_call(CPU_Handle &cpu_handle);
        friend void ins_ret(CPU_Handle &cpu_handle);
        friend void ins_push(CPU_Handle &cpu_handle);
        friend void ins_pop(CPU_Handle &cpu_handle);
        friend void ins_write(CPU_Handle &cpu_handle);
        friend void ins_read(CPU_Handle &cpu_handle);
        friend void ins_print(CPU_Handle &cpu_handle);
        friend void ins_sprint(CPU_Handle &cpu_handle);
        friend void ins_cprint(CPU_Handle &cpu_handle);
        friend void ins_exit(CPU_Handle &cpu_handle);
        friend void pdb_handle_break(
                const std::vector<std::string> cmd_tokens,
                std::vector<int16_t> &breakpoints,
                const std::vector<int16_t> mnemonic_addrs
        );
        friend void pdb_handle_delete(
                const std::vector<std::string> cmd_tokens,
                std::vector<int16_t> &breakpoints
        );
        friend void pdb_handle_print(
                const std::vector<std::string> cmd_tokens,
                CPU_Handle &cpu_handle
        );
        friend void update_register(
                CPU_Handle &cpu_handle,
                const int16_t dest,
                const int16_t value
        );
};

/**
 * @fn void CPU_Handle::load_program(const std::vector<int16_t> given_program)
 * @brief loads elements of given_program to program_data
 * @details also allocates program_data member to have enough space to load
 * given_data, since string data may be large
 */

/**
 * @fn void CPU_Handle::run_program(const bool is_debug)
 * @brief runs the assembled program, with a debugger if enabled
 */

/**
 * @fn void CPU_Handle::next_instruction(bool &hit_exit)
 * @brief simulates the next instruction to run
 * @details if an EXIT instruction is run, set hit_exit to true to end
 * execution. Runs the same in debug and normal mode
 */

/**
 * @fn int16_t CPU_Handle::dereference_value(int16_t given_value);
 * @brief gets the intended source value, whether it's a register, offset, or literal
 * @details helper function of CPU_Handle::next_instruction
 */

#endif
