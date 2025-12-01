![Project Banner](resources/pseudo_assembly_assembler.png)

# CSCI 200 Final Project: Pseudo Assembly Assembler
The Pseudo Assembly Assembler (PAA) is a simple assembler and simulator, meant
to introduce users to the concepts of low level programming and computer
architecture.
PAA is designed for a 16 bit reduced instruction set inspired by x86, with
some changes to make usage simple.

# Quick Reference

## Assembler Flags
- -a, --assemble-only
- -b, --binary-input
- -d, --debug
- -h, --help
- -s, --save-temps

## PAL Debugger Commands
- break \<program address\>
- clear
- continue
- delete \<program address\>?
- help
- interpret
- list
- next
- print \<register|stack offset|ram address\>
- quit

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
| MOV          | dest      | src0      |           |
| INC          | dest      |           |           |
| DEC          | dest      |           |           |
| ADD          | dest      | src0      | src1      |
| SUB          | dest      | src0      | src1      |
| MUL          | dest      | src0      | src1      |
| DIV          | dest      | src0      | src1      |
| AND          | dest      | src0      | src1      |
| OR           | dest      | src0      | src1      |
| NOT          | dest      | src0      |           |
| XOR          | dest      | src0      | src1      |
| LSH          | dest      | src0      | src1      |
| RSH          | dest      | src0      | src1      |
| CMP          | src0      | src1      |           |
| JMP          | label     |           |           |
| JEQ          | label     |           |           |
| JNE          | label     |           |           |
| JGE          | label     |           |           |
| JGR          | label     |           |           |
| JLE          | label     |           |           |
| JLS          | label     |           |           |
| CALL         | label     |           |           |
| RET          |           |           |           |
| PUSH         | src       |           |           |
| POP          | dest      |           |           |
| WRITE        | src       | addr      |           |
| READ         | dest      | addr      |           |
| PRINT        | src       |           |           |
| SPRINT       | string    |           |           |
| CPRINT       | src       |           |           |
| INPUT        |           |           |           |
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
