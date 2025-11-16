# CSCI 200 Final Project: Pseudo Assembly Assembler
The Pseudo Assembly Assembler (PAA) is a simple assembler and simulator, meant
to introduce users to the concepts of low level programming and computer
architecture.
PAA is designed for a 16 bit reduced instruction set inspired by x86, with
some changes to make usage simple.

# Quick Reference

## Assembler Flags
- -c, --compile-only
- -h, --help
- -s, --save-temps
- -b, --binary-input
- -p, --print-step

## Assembly Language
- 16 bit registers
- 8 general purpose registers, 2 comparison registers (nonaccessable)
- given 1024 address RAM with 4 memory access instructions (PUSH, POP, READ,
  WRITE)
- can print integer values with PRINT, and string literals with SPRINT
- can define labels with \<string\>:
- can access stack without pop using stack offset notation (see below)
- can access stack pointer with RSP, and the instruction counter with RIP

## Instructions
| **Mnemonic** | **Arg 1** | **Arg 2** | **Arg 3** |
|--------------|-----------|-----------|-----------|
| NOP          |           |           |           |
| MOV          | dest      | src       |           |
| ADD          | dest      | src       | src       |
| SUB          | dest      | src       | src       |
| MUL          | dest      | src       | src       |
| DIV          | dest      | src       | src       |
| AND          | dest      | src       | src       |
| OR           | dest      | src       | src       |
| XOR          | dest      | src       | src       |
| LSH          | dest      | src       | src       |
| RSH          | dest      | src       | src       |
| CMP          | src       | src       |           |
| JEQ          | label     |           |           |
| JNE          | label     |           |           |
| JGR          | label     |           |           |
| JLE          | label     |           |           |
| JLS          | label     |           |           |
| CALL         | label     |           |           |
| RET          |           |           |           |
| PUSH         | src       |           |           |
| POP          | dest      |           |           |
| READ         | src       | addr      |           |
| WRITE        | dest      | addr      |           |
| PRINT        | src       |           |           |
| SPRINT       | string    |           |           |
| EXIT         |           |           |           |

## Syntax (Backus-Naur Form)
Note: "$" in this case is not the start of a string, but the literal character \
"?" means optional \
"+" means repeated 1 or more times \
"." means any character \
"[...]" means the set of included characters \
\<dest\> ::= \<register\> \
\<src\> ::= \<literal_int\> | \<register\> | \<stack_offset\> \
\<literal_int\> ::= $-?[:digit:]\+ \
\<register\> ::= R[ABCDEFG] \
\<stack_offset\> ::= %[:digit:]\+ \
\<addr\> ::= [:digit:]\+ \
\<label\> ::= .\+ \
\<string\> ::= ".+"

## Other quirks
- every program is required to have a main label and at least one EXIT instruction
- SPRINT supports newlines and escaped double quotes
- there are 2048 addresses allotted to the program simulation
- the stack occupies the last 512 elements of RAM block, where the address
  1536 corresponds to %0 when no elements are on the stack
