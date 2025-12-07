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
- -t, --test-only

## PAL Debugger Commands
- break \<program address\>
- clear
- continue
- delete \<program address\>?
- help
- disassemble
- list
- next
- print \<register|stack offset|ram address\>
- quit

## Assembly Language
- 16 bit registers
- 8 general purpose registers, 2 comparison registers (nonaccessable)
- given 3000 address RAM with 4 memory access instructions (PUSH, POP, READ,
  WRITE)
- can print integer values with PRINT, ascii characters with CPRINT, and
  string literals with SPRINT
- can define labels with \<string\>:
- stack addressing with "%n"
- ram addressing with "\[$n\]"
- can access stack pointer with RSP, and the instruction counter with RIP

## Instructions
| **Mnemonic** | **Arg 1** | **Arg 2**  |
|--------------|-----------|------------|
| NOP          |           |            |
| MOV          | dest      | src        |
| INC          | dest      |            |
| DEC          | dest      |            |
| ADD          | dest      | src        |
| SUB          | dest      | src        |
| MUL          | dest      | src        |
| DIV          | dest      | src        |
| MOD          | dest      | src        |
| AND          | dest      | src        |
| OR           | dest      | src        |
| NOT          | dest      | src        |
| XOR          | dest      | src        |
| LSH          | dest      | src        |
| RSH          | dest      | src        |
| CMP          | src0      | src1       |
| JMP          | label     |            |
| JEQ          | label     |            |
| JNE          | label     |            |
| JGE          | label     |            |
| JGR          | label     |            |
| JLE          | label     |            |
| JLS          | label     |            |
| CALL         | label     |            |
| RET          |           |            |
| PUSH         | src       |            |
| POP          | dest      |            |
| WRITE        | src0      | src1       |
| READ         | dest      | src1       |
| PRINT        | src       |            |
| SPRINT       | string    |            |
| CPRINT       | src       |            |
| INPUT        |           |            |
| SINPUT       |           |            |
| RAND         |           |            |
| EXIT         |           |            |

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
- INPUT supports single ascii characters and integers
- values are clamped to \[-16383,16383\]
- there are 8192 addresses allotted to the program's runtime memory
- the stack occupies the last 2048 elements of RAM block, where the address
  6144 corresponds to %0 when no elements are on the stack
