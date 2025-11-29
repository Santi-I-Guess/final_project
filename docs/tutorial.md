# The Pseudo Assembly Language

The Pseudo Assembly Language (PAL) is a faux programming language that is
made to mimic an x86 assembly language. It has instructions for arithmetic,
conditional branching, memory manipulation, and printing.

There are three kinds of statements in PAL:
- Instruction: an instruction to the computer, which generally follows the
  structure \<mnemonic\> \<arguments\>. This is how the computer manipulates
  memory and gets things done
- Label definitions: a marking inside a program which the assembler replaces
  with the address of the following instruction. Useful for controlling the
  flow of execution, such as with branching and function calls, without
  hardcoding program addresses to jump to
- Comments: text that is ignored by the assembler, and does not affect the
  program in any way.

It is idiomatic to put only one kind of statement per line, but putting
multiple statements on a single line will not cause an error.

# Registers
Registers are the quickest way of handling data in an assembly language
In PAL, you get 8 general purpose registers to freely read and write to:
RA, RB, RC, ..., RH. Each of which stores a 16 bit signed integer
(Max: 32767, Min: -32768).

You also have access to some read only registers: RSP, the stack pointer;
RIP, the instruction pointer; and CMP0 and CMP1, the comparison registers used
in the branch instructions. These registers can only be changed through the
use of instructions, in order to ensure their operations are valid.

All registers, with the exception of RIP, are guarenteed to be set to 0 at
the start of the program.

# Memory
In this simulator, you are given 2048 integers of memory outside of integers
to work with. This memory is split into the RAM, which takes up the first 3/4
of that space (1536 integers), and the stack, which takes up the last 1/4 of
the space (512 integers). The RAM can be freely written and read from,
provided an address within the RAM space. The stack can only have values
pushed to the top of the stack, but can be read from either the top of the
stack, or from an offset from the top of the stack. The stack has a dedicated
register RSP, the stack pointer, which points to the top of the stack.

# Labels
To define a label, you write a series of characters, followed by a colon (:).
Labels can be defined anywhere in the program. Every program must define
a "main" label, or the program will refuse to assemble.

# Instructions and Arguments
Instructions can generally be divided into a few groups: arithmetic, branching,
memory manipulation, and program manipulation.
- Arithmetic instructions perform basic arithmetic and bitwise operations
  between arguments.
- Branching instructions allow the program to continue somewhere else in the
  program, either unconditionally, conditionally, or via a subprocess.
- Memory manipulation instructions read and write RAM and stack memory to
  and from registers\*.
- Program instructions do others miscellaneous things, such as telling the
  program to exit, or print to stdout.

## Arithmetic Instructions
Arithmetic instructions generally take two sources, perfrom some operation
between the two, and then stores the result in the destination ADDR. INC and
DEC break this pattern, as they increase or decrease the target register by 1.
NOT also breaks the pattern, by simply bitwise notting the source.

## Memory Instructions
To access RAM memory, READ and WRITE are used to either read from or write to
an address. As for the stack memory, PUSH and POP are used for similar
purposes, but they only operate on the top of the stack.

## Branching Instructions
In order to use any of the JMP style instructions other than JMP, CMP must
be used beforehand. CMP takes two sources, and stores their values in CMP0 and
CMP1 respectively. When a JMP style instruction, such as JLE for example,
will only jump to the label if CMP0 <= CMP1, in that example.

Functions can be implemented with CALL, which pushes the current instruction
pointer to the stack and jumps to the specified label. To return, use the RET
instruction, which jumps to the value at the top of the stack and subsequently
pops the stack.

## Program Instructions
PRINT is used to print an integer to stdout, while SPRINT is used specifically
to print string literals. Strings must be surrounded by double quotes, and support
two escaped characters: the double quote, and the newline. Null terminators
are automatically included in string, and as such have no escape character.

Every program is required to have at least one instance of the EXIT
instruction. Failing to do so causes an assembler error.

| **Mnemonic** | **Arg 1** | **Arg 2** | **Arg 3** | **Pseudocode**             |
|--------------|-----------|-----------|-----------|----------------------------|
| NOP          |           |           |           |                            |
| MOV          | dest      | src0      |           | dest =  src0               |
| INC          | dest      |           |           | dest++                     |
| DEC          | dest      |           |           | dest--                     |
| ADD          | dest      | src0      | src1      | dest =  src0 + src1        |
| SUB          | dest      | src0      | src1      | dest =  src0 - src1        |
| MUL          | dest      | src0      | src1      | dest =  src0 * src1        |
| DIV          | dest      | src0      | src1      | dest =  src0 / src1        |
| AND          | dest      | src0      | src1      | dest =  src0 & src1        |
| OR           | dest      | src0      | src1      | dest =  src0 \| src1       |
| NOT          | dest      | src0      |           | dest = ~src0               |
| XOR          | dest      | src0      | src1      | dest =  src0 ^ src1        |
| LSH          | dest      | src0      | src1      | dest =  src0 << src1       |
| RSH          | dest      | src0      | src1      | dest =  src0 >> src1       |
| CMP          | src0      | src1      |           | cmp0 =  src0; cmp1 = src1  |
| JMP          | label     |           |           | goto label                 |
| JEQ          | label     |           |           | goto label if cmp0 == cmp1 |
| JNE          | label     |           |           | goto label if cmp0 != cmp1 |
| JGE          | label     |           |           | goto label if cmp0 >= cmp1 |
| JGR          | label     |           |           | goto label if cmp0 >  cmp1 |
| JLE          | label     |           |           | goto label if cmp0 <= cmp1 |
| JLS          | label     |           |           | goto label if cmp0 <  cmp1 |
| CALL         | label     |           |           | label()                    |
| RET          |           |           |           | return                     |
| PUSH         | src       |           |           | push(src)                  |
| POP          | dest      |           |           | dest = pop()               |
| WRITE        | src       | addr      |           | ram\[addr\] = src          |
| READ         | dest      | addr      |           | dest = ram\[addr\]         |
| PRINT        | src       |           |           | print(src)                 |
| SPRINT       | string    |           |           | print(string)              |
| EXIT         |           |           |           | exit()                     |

# Addressing Modes (Source Arguments)
Registers are the medium where values are used, but without a way to put
specific values into the registers, programming gets very verbose and 
tedious. For ease of programming, two other addressing modes are provided.

## Stack addressing: %N
%N will return the Nth element from the top of the stack, where %0 returns the
top of the stack (the same value as using POP). %N is a read only operation,
meaning it will not pop the value used, nor affect the program.
N must be a positive value within the bounds of an i16, or a runtime error will occur.

## Literal addressing: $N
$N will return the literal value N, which may be any i16. N must be a valid
i16, or an assembler error will occur.

# Program flags
Certain command line flags are provided to the program executable to make
certain common operations repeatable. Descriptions can be provided using
-h or --help as a command line argument.
If no input file is provided, the program will await input from STDIN. Users
can then input instructions and label definitions just like for an input file.
The program will continue to take in input until an empty line is inputted.
This is particularly useful for testing smaller programs without making an
entire file, such as
- <code>printf "main: EXIT\n" | ./final_project</code>
- <code>Write-Output "main: EXIT\n" | .\\final/project<code>
