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
- -S, --use-stdin
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
- 8 general purpose registers, 2 comparison registers
- given 8192 address RAM with 4 memory access instructions (PUSH, POP, READ,
  WRITE)
- can print integer values with PRINT, ascii characters with CPRINT, and
  string literals with SPRINT
- can take in user integer input with INPUT, and user string input with SINPUT
- can define labels with \<string\>:
- stack addressing with "%n" (see addressing modes)
- ram addressing with "\[$n\]" (see addressing modesk)
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

- src can refer to any register, a literal value, a stack offset,
  or an address in RAM
- dest refers to any general purpose register RA-RH
- for write and read, src1 is the value of the address, not necesarily using
  RAM addressing. For example,
`
MOV RB, $1
WRITE $10, RB
`
  is a valid use of WRITE, which writes the value 10 to the address of 1.
- INPUT, SINPUT, and RAND push their values onto the stack, instead of
  storing inside a register

## Other quirks
- every program is required to have a main label and at least one EXIT instruction
- SPRINT supports newlines and escaped double quotes, very specifically "\n" and "\""
- INPUT supports single ascii characters and integers
- register values are clamped to \[-16383,16383\]
- there are 8192 addresses allotted to the program's runtime memory
- the stack occupies the last 2048 elements of RAM block, where the address
  6144 corresponds to %0 when no elements are on the stack
