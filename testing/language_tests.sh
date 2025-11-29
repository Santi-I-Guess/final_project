#!/usr/bin/env bash

# tests to ensure assembler follows ISA

mnemonics=(
    "NOP"   "MOV"    "INC"   "DEC"
    "ADD"   "SUB"    "MUL"   "DIV"
    "AND"   "OR"     "NOT"   "XOR"
    "LSH"   "RSH"    "CMP"   "JMP"
    "JEQ"   "JNE"    "JGE"   "JGR"
    "JLE"   "JLS"    "CALL"  "RET"
    "PUSH"  "POP"    "WRITE" "READ"
    "PRINT" "SPRINT" "EXIT"
)

# quick instructions that have valid argument number and types
quick_instructions=(
    "NOP"
    "MOV RA, \$0"
    "INC RA"
    "DEC RA"
    "ADD RA, RA, \$0"
    "SUB RA, RA, \$0"
    "MUL RA, RA, \$0"
    "DIV RA, RA, \$0"
    "AND RA, RA, \$0"
    "OR  RA, RA, \$0"
    "NOT RA, \$0"
    "XOR RA, RA, \$0"
    "LSH RA, RA, \$0"
    "RSH RA, RA, \$0"
    "CMP RA, RB"
    "JMP main"
    "JEQ main"
    "JNE main"
    "JGE main"
    "JGR main"
    "JLE main"
    "JLS main"
    "CALL main"
    "RET"
    "PUSH  \$0"
    "POP   RA"
    "WRITE \$0 \$0"
    "READ  RA \$0"
    "PRINT \$0"
    "SPRINT \"tene\""
    "EXIT"
)

for i in {0..30}; do
    printf "\x1b[32m%s:\x1b[0m\n" "${mnemonics[i]}"
    curr_program=$(printf "main:\n  %s\n  EXIT" "${quick_instructions[i]}")
    printf "%s\n" "${curr_program}"
    printf "%s\n" "${curr_program}" | ../final_project -d
done
