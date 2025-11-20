#include <cstdlib>
#include <iostream>

#include "auxiliary.h"
#include "assembler/common_values.h"

// refactoring functions for main.cpp, therefore i'll allow includes from
// other modules here

void handle_generation_res(Debug_Info res) {
        switch (res.assembler_retval) {
        case ACCEPTABLE_2:
                break;
                std::exit(0);
        case EXPECTED_MNEMONIC_2:
                std::cout << "--- Assembler Error ---\n";
                std::cout << "Mnemonic is undefined\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Invalid mnemonic: " << res.relevant_tokens.at(0) << "\n";
                std::exit(0);
        case INVALID_ATOM_2:
                std::cout << "--- Assembler Error ---\n";
                std::cout << "Invalid Atom\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Invalid token: " << res.relevant_tokens.at(0) << "\n";
                std::exit(0);
        case MISSING_ARGUMENTS_2:
                // is intentionally duplicate, will remove the previous
                // MISSING_ARGUMENTS later
                std::cout << "--- Assembler Error ---\n";
                std::cout << "Missing arguments\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Mnemonic: " << res.relevant_tokens.at(0) << "\n";
                std::exit(0);
        case MISSING_MAIN_2:
                std::cout << "--- Assembler Error ---\n";
                std::cout << "Main label was never defined\n";
                std::exit(0);
        case UNKNOWN_LABEL_2:
                std::cout << "--- Assembler Error ---\n";
                std::cout << "Label is undefined\n";
                std::cout << "Token #" << res.relevant_idx << "\n";
                std::cout << "Invalid instruction: ";
                std::cout << res.relevant_tokens.at(0) << " ";
                std::cout << res.relevant_tokens.at(1) << "\n";
        }
}

void handle_grammar_check_res(Debug_Info res) {
        switch (res.grammar_retval) {
        case ACCEPTABLE:
                break;
        case EXPECTED_MNEMONIC:
                std::cout << "--- Grammar Error ---\n";
                std::cout << "Expected mnemonic\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Actual Symbol: " << res.relevant_tokens.at(0) << "\n";
                std::exit(0);
        case INVALID_ATOM:
                std::cout << "--- Grammar Error ---\n";
                std::cout << "Invalid atom\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Mnemonic: " << res.relevant_tokens.at(0) << "\n";
                std::cout << "Invalid symbol: " << res.relevant_tokens.at(1) << "\n";
                std::exit(0);
        case MISSING_ARGUMENTS:
                std::cout << "--- Grammar Error ---\n";
                std::cout << "Missing arguments\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Mnemonic: " << res.relevant_tokens.at(0) << "\n";
                std::exit(0);
        case MISSING_EXIT:
                std::cout << "--- Grammar Error ---\n";
                std::cout << "Missing EXIT instruction\n";
                std::exit(0);
        case MISSING_MAIN:
                std::cout << "--- Grammar Error ---\n";
                std::cout << "Missing MAIN label\n";
                std::exit(0);
        case UNKNOWN_LABEL:
                std::cout << "--- Grammar Error ---\n";
                std::cout << "Attempted to call unknown label\n";
                std::cout << "Instruction #" << res.relevant_idx << "\n";
                std::cout << "Mnemonic: " << res.relevant_tokens.at(0) << "\n";
                std::cout << "Invalid symbol: " << res.relevant_tokens.at(1) << "\n";
                std::exit(0);
        }
}
