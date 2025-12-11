/* CSCI 200: Final Project: Pseudo Assembly Simulator
 *
 * Author: Santiago Sagastegui
 *
 * Pseudo assembly assembler for very simplified x86 ISA. Also offers
 * simulator, stdin input, and a dedicated debugger
 * Also hosted at https://github.com/Santi-I-Guess/PAL-Assembler
 */

#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <string>
#include <vector>

#include "assembler/assembler.h"
#include "assembler/tokenizer.h"
#include "common_values.h"
#include "misc/cmd_line_opts.h"
#include "misc/file_handling.h"
#include "simulator/cpu_handle.h"

/**
 * @brief stores error message for grammar errors in user programs
 * @details element 0 is the same idx as ACCEPTABLE, so no message is needed.
 * helper variable for handle_grammar_error
 */
const std::string GRAMMAR_ERROR_MESSAGES[10] = {
        "",
        "Expected Mnemonic",
        "Invalid Atom",
        "Missing Arguments",
        "Missing Exit",
        "Missing Main",
        "Unknown Label",
        "Unknown Mnemonic",
};

/**
 * @brief handle for user program grammar errors
 * @details capable of exiting. helper function for main
 */
void handle_grammar_error(
        const Grammar_Retval error_code,
        const Debug_Info context,
        const std::string erroneous_line
) {
        // get original error-causing line
        std::cerr << "\x1b[34mTraceback line " << context.line_num;
        std::cerr << ":\x1b[0m ";
        std::cerr << erroneous_line << "\n";

        std::cerr << "\x1b[34mGrammar Error:\x1b[0m ";
        std::cerr << GRAMMAR_ERROR_MESSAGES[error_code];
        // intentional lack of newline for first 5 elements
        switch (error_code) {
        case ACCEPTABLE_E: /* filtered out */
                break;
        case EXPECTED_MNEMONIC_E:
        case INVALID_ATOM_E:
        case MISSING_ARGUMENTS_E:
        case UNKNOWN_LABEL_E:
        case UNKNOWN_MNEMONIC_E:
                std::cerr << " \"" << context.relevant_token.data << "\"";
                [[fallthrough]]; // c++17
        case MISSING_EXIT_E:
        case MISSING_MAIN_E:
                std::cerr << "\n";
                break;
        }
        std::exit(1);
}

/**
 * @brief handle for generating program from user ascii input
 * @details capable of exiting, helper function for main
 */
std::vector<int16_t> generate_program(
        char** const argv,
        const Cmd_Options &life_opts
) {
        // produce random file header for intermediate files, which makes
        //      running multiple tests in a row unlikely to overwrite data
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> uid(1, 10000);
        std::string file_header = std::to_string(uid(mt));
        while (file_header.length() < 5) {
                file_header = "0" + file_header;
        }

        // choose input source, and put into string
        std::string source_buffer;
        if (life_opts.input_file_idx == -1)
                source_buffer = get_source_buffer("", true);
        else
                source_buffer = get_source_buffer(argv[life_opts.input_file_idx], false);

        // tokenize source_buffer, create label_map
        std::vector<Token> tokens = create_tokens(source_buffer); 

        // store program addresses of user defined label
        std::map<std::string, int16_t> label_map = create_label_map(tokens);

        // get rid of label declaration tokens, as they are no longer needed
        std::vector<Token> filtered_tokens;
        for (Token curr_token : tokens) {
                if (curr_token.type != T_LABEL_DEF)
                        filtered_tokens.push_back(curr_token);
        }

        // generate intermediate file with tokens and labels
        if (life_opts.intermediate_files)
                generate_intermediate_file(file_header, filtered_tokens, label_map);

        // exit program if there is a grammar error
        Debug_Info context = grammar_check(filtered_tokens, label_map);
        if (context.grammar_retval != ACCEPTABLE_E) {
                std::stringstream aux_stream;
                std::string aux_string;
                aux_stream << source_buffer;
                for (int i = 0; i < context.line_num; ++i)
                        std::getline(aux_stream, aux_string);
                handle_grammar_error(context.grammar_retval, context, aux_string);
        }

        // create the assembled program
        std::vector<int16_t> final_program = assemble_program(filtered_tokens, label_map);

        // if assemble_only flag is on, write binary to file and quit
        if (life_opts.assemble_only) {
                bool res_temp;
                res_temp = write_program_to_sink(final_program, file_header);
                if (!res_temp) {
                        std::cerr << "Failed to open assembly binary file\n";
                        std::exit(1);
                }
                std::exit(0);
        };
        return final_program;
}

int main(int argc, char **argv) {
        // batch create instructions
        Instruction_Data B_NOP(0,     "NOP",     {MNEMONIC});
        Instruction_Data B_MOV(1,     "MOV",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_INC(2,     "INC",     {MNEMONIC, REGISTER});
        Instruction_Data B_DEC(3,     "DEC",     {MNEMONIC, REGISTER});
        Instruction_Data B_ADD(4,     "ADD",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_SUB(5,     "SUB",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_MUL(6,     "MUL",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_DIV(7,     "DIV",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_MOD(8,     "MOD",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_AND(9,     "AND",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_OR(10,     "OR",      {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_NOT(11,    "NOT",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_XOR(12,    "XOR",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_LSH(13,    "LSH",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_RSH(14,    "RSH",     {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_CMP(15,    "CMP",     {MNEMONIC, SOURCE,   SOURCE});
        Instruction_Data B_JMP(16,    "JMP",     {MNEMONIC, LABEL});
        Instruction_Data B_JEQ(17,    "JEQ",     {MNEMONIC, LABEL});
        Instruction_Data B_JNE(18,    "JNE",     {MNEMONIC, LABEL});
        Instruction_Data B_JGE(19,    "JGE",     {MNEMONIC, LABEL});
        Instruction_Data B_JGR(20,    "JGR",     {MNEMONIC, LABEL});
        Instruction_Data B_JLE(21,    "JLE",     {MNEMONIC, LABEL});
        Instruction_Data B_JLS(22,    "JLS",     {MNEMONIC, LABEL});
        Instruction_Data B_CALL(23,   "CALL",    {MNEMONIC, LABEL});
        Instruction_Data B_RET(24,    "RET",     {MNEMONIC});
        Instruction_Data B_PUSH(25,   "PUSH",    {MNEMONIC, SOURCE});
        Instruction_Data B_POP(26,    "POP",     {MNEMONIC, REGISTER});
        Instruction_Data B_WRITE(27,  "WRITE",   {MNEMONIC, SOURCE,   SOURCE});
        Instruction_Data B_READ(28,   "READ",    {MNEMONIC, REGISTER, SOURCE});
        Instruction_Data B_PRINT(29,  "PRINT",   {MNEMONIC, SOURCE});
        Instruction_Data B_SPRINT(30, "SPRINT",  {MNEMONIC, LITERAL_STR});
        Instruction_Data B_CPRINT(31, "CPRINT",  {MNEMONIC, SOURCE});
        Instruction_Data B_INPUT(32,  "INPUT",   {MNEMONIC});
        Instruction_Data B_SINPUT(33, "SINPUT",  {MNEMONIC});
        Instruction_Data B_RAND(34,   "RAND",    {MNEMONIC});
        Instruction_Data B_EXIT(35,   "EXIT",    {MNEMONIC});

        // load instructions at runtime into BLUEPRINTS global argument
        BLUEPRINTS.insert({"NOP",    B_NOP});
        BLUEPRINTS.insert({"MOV",    B_MOV});
        BLUEPRINTS.insert({"INC",    B_INC});
        BLUEPRINTS.insert({"DEC",    B_DEC});
        BLUEPRINTS.insert({"ADD",    B_ADD});
        BLUEPRINTS.insert({"SUB",    B_SUB});
        BLUEPRINTS.insert({"MUL",    B_MUL});
        BLUEPRINTS.insert({"DIV",    B_DIV});
        BLUEPRINTS.insert({"MOD",    B_MOD});
        BLUEPRINTS.insert({"AND",    B_AND});
        BLUEPRINTS.insert({"OR",     B_OR});
        BLUEPRINTS.insert({"NOT",    B_NOT});
        BLUEPRINTS.insert({"XOR",    B_XOR});
        BLUEPRINTS.insert({"LSH",    B_LSH});
        BLUEPRINTS.insert({"RSH",    B_RSH});
        BLUEPRINTS.insert({"CMP",    B_CMP});
        BLUEPRINTS.insert({"JMP",    B_JMP});
        BLUEPRINTS.insert({"JEQ",    B_JEQ});
        BLUEPRINTS.insert({"JNE",    B_JNE});
        BLUEPRINTS.insert({"JGE",    B_JGE});
        BLUEPRINTS.insert({"JGR",    B_JGR});
        BLUEPRINTS.insert({"JLE",    B_JLE});
        BLUEPRINTS.insert({"JLS",    B_JLS});
        BLUEPRINTS.insert({"CALL",   B_CALL});
        BLUEPRINTS.insert({"RET",    B_RET});
        BLUEPRINTS.insert({"PUSH",   B_PUSH});
        BLUEPRINTS.insert({"POP",    B_POP});
        BLUEPRINTS.insert({"WRITE",  B_WRITE});
        BLUEPRINTS.insert({"READ",   B_READ});
        BLUEPRINTS.insert({"PRINT",  B_PRINT});
        BLUEPRINTS.insert({"SPRINT", B_SPRINT});
        BLUEPRINTS.insert({"CPRINT", B_CPRINT});
        BLUEPRINTS.insert({"INPUT",  B_INPUT});
        BLUEPRINTS.insert({"SINPUT", B_SINPUT});
        BLUEPRINTS.insert({"RAND",   B_RAND});
        BLUEPRINTS.insert({"EXIT",   B_EXIT});

        Cmd_Options life_opts;
        life_opts.store_cmd_args(argc, argv);
        bool valid_cmd_arg_combo = life_opts.is_valid_args();
        if (!valid_cmd_arg_combo)
                return 0;

        // put assembled program here, so assembler module
        //      doesn't require cpu_handle
        std::vector<int16_t> final_program = {};
        if (life_opts.is_binary_input) {
                std::string file_path = argv[life_opts.input_file_idx];
                populate_program_from_binary(final_program, file_path);
        } else {
                final_program = generate_program(argv, life_opts);
        }

        // if test only flag is on, don't simulate program
        if (!life_opts.test_only) {
                CPU_Handle cpu_handle;
                cpu_handle.load_program(final_program);
                if (life_opts.is_debug)
                        cpu_handle.run_program_debug();
                else
                        cpu_handle.run_program();
        }
        return 0;
}
