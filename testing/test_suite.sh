#!/usr/bin/env bash

set -o nounset

# using SPRINT and PRINT instruction to do my testing

print_check() {
    printf "\x1b[32mPrint Check:\x1b[0m\n"
    printf "\x1b[32mExpect: 1-50, one per line\x1b[0m\n"
    for i in {1..50}; do
        program=$(printf "\
        main:
            MOV RA, \$$i
            PRINT RA
            SPRINT \"\\%s\"
            EXIT" "n")
        printf "%s\n" "$program" | ../final_project
    done
}

read_write_check() {
    # check READ and WRITE
    printf "\x1b[32mREAD-WRITE Check:\x1b[0m\n"
    printf "\x1b[32mExpect: 2-100, one per line, skip 2's\x1b[0m\n"
    for i in {1..50}; do
        program=$(printf "\
            main:
                WRITE \$$(( 2*i )), \$$i
                READ RA, \$$i
                PRINT RA
                SPRINT \"\\%s\"
            ;
            EXIT" "n")
        printf "%s\n" "$program" | ../final_project
    done
}

basic_loop_check() {
    # check PUSH, POP, CMP, JLS, and stack offset addressing
    printf "\x1b[32mBasic Loop Check:\x1b[0m\n"
    printf "\x1b[32mExpect: 12-1, four per line\x1b[0m\n"
    escaped_n="n"
    program="\
        print_top_4:
            PRINT %0
            SPRINT \" \"
            PRINT %1
            SPRINT \" \"
            PRINT %2
            SPRINT \" \"
            PRINT %3
            SPRINT \"\\${escaped_n}\"
            POP RB
            POP RB
            POP RB
            POP RB
            RET
        main:
            MOV RA, \$1
        again_1:
            PUSH RA
            INC RA
            CMP RA, \$12
            JLE again_1
        MOV RA, \$0
        again_2:
            CALL print_top_4
            INC RA
            CMP RA, \$3
            JLS again_2
        EXIT
    "
    printf "%s\n" "$program" | ../final_project
}

ascii_check() {
    # check CPRINT
    printf "\x1b[32mAscii Check:\x1b[0m\n"
    printf "\x1b[32mExpect: lowercase a-z, then uppercase A-Z, on seperate lines\x1b[0m\n"
    escaped_n="n"
    program="\
        print_alphabet:
        ; RA is the value, RB is the bounds
        POP RA
        MOV RB, RA
        ADD RB, RB, \$26
        again_1:
        CPRINT RA
        INC RA
        CMP RA, RB
        JGE next_1
        SPRINT \" \"
        JLS again_1
        next_1:
        SPRINT \"\\${escaped_n}\"
        RET
        ;
        main:
        PUSH \$97
        CALL print_alphabet
        PUSH \$65
        CALL print_alphabet
        EXIT
    "
    printf "%s\n" "${program}" | ../final_project

}

print_check
read_write_check
basic_loop_check
ascii_check
