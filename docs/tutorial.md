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
RA, RB, RC, ..., RH. Each of which stores a 16 bit signed integer, but
due to bits 14, 13, and 12 being reserved for addressing modes, values
will be clamped to \[-4096, 4096\].

You also have access to some read only registers: RSP, the stack pointer;
RIP, the instruction pointer; and CMP0 and CMP1, the comparison registers used
in the branch instructions. These registers can only be changed through the
use of instructions, in order to ensure their operations are valid.

RSP, however, is allowed to be mutated with arithmetic instructions,
as all strings inputted by the user at runtime are pushed to the stack.
Allowing RSP to be written to makes string operations possible without
moving the string to RAM. Attempting to set RSP to a negative value or
above the MAX_STACK_SIZE will cause a STACK_WRITE_ERROR.

There is also a zero register, RZ, that is provided. Writing to it has
no effect, and reading from RZ will always return 0; This is particularly
useful for popping parameters of a function call from a stack, without
worrying about changing any registers.

All registers, with the exception of RIP, are guarenteed to be set to 0 at
the start of the program.

# Memory
In this simulator, you are given 3000 integers of memory outside of integers
to work with. This memory is split into the RAM, which takes up the first 3/4
of that space (2250 integers), and the stack, which takes up the last 1/4 of
the space (750 integers). The RAM can be freely written and read from,
provided an address within the RAM space. The stack can only have values
pushed to the top of the stack, but can be read from either the top of the
stack, or from an offset from the top of the stack. The stack has a dedicated
register RSP, the stack pointer, which points to the top of the stack.

Every element of the RAM, including the stack, is guarenteed to be set to 0
at the start of the program.

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

## Program / MISC Instructions
PRINT is used to print an integer to stdout, while SPRINT is used specifically
to print string literals. CPRINT is used to print an integer, casted to
a char. This is especially useful for printing spaces and newlines, since
a space is ascii code 32, and a newline is ascii code 10. The main reason
this instruction exists is to not clutter the string literal data section
of a program with single letters, for debugging purposes.

For SPRINT, Strings must be surrounded by double quotes, and support
two escaped characters: the double quote, and the newline. Null terminators
are automatically included in string, and as such have no escape character.

INPUT takes in a single integer from stdin, and pushes the resulting
integer onto the stack. If the user inputs a single ascii character, such
as a letter or number, then the ascii value of will be pushed to the stack.
This makes elementary char operations possible, since lowercase letters
have ascii values exactly 32 greater than their uppercase alternates.

SINPUT is similar to input, except it will take each character of the input,
convert it into the ascii value, and push each value to the stack. If no
characters are given, it will push a newline char to the stack, to match
INPUT behavior. SINPUT will push a null terminator.

Every program is required to have at least one instance of the EXIT
instruction, or the program will refuse to assemble.

| **Mnemonic** | **Arg 1** | **Arg 2**  | **Pseudocode**               |
|--------------|-----------|------------|------------------------------|
| NOP          |           |            |                              |
| MOV          | dest      | src        | dest =  src                  |
| INC          | dest      |            | dest++                       |
| DEC          | dest      |            | dest--                       |
| ADD          | dest      | src        | dest += src                  |
| SUB          | dest      | src        | dest -= src                  |
| MUL          | dest      | src        | dest *= src                  |
| DIV          | dest      | src        | dest /= src                  |
| MOD          | dest      | src        | dest %= src                  |
| AND          | dest      | src        | dest &= src                  |
| OR           | dest      | src        | dest \|= src                 |
| NOT          | dest      | src        | dest = ~src                  |
| XOR          | dest      | src        | dest ^= src_0                |
| LSH          | dest      | src        | dest <<= src                 |
| RSH          | dest      | src        | dest >>= src                 |
| CMP          | src0      | src1       | cmp0 = src0; cmp1 = src1     |
| JMP          | label     |            | goto label                   |
| JEQ          | label     |            | goto label if cmp0 == cmp1   |
| JNE          | label     |            | goto label if cmp0 != cmp1   |
| JGE          | label     |            | goto label if cmp0 >= cmp1   |
| JGR          | label     |            | goto label if cmp0 >  cmp1   |
| JLE          | label     |            | goto label if cmp0 <= cmp1   |
| JLS          | label     |            | goto label if cmp0 <  cmp1   |
| CALL         | label     |            | label()                      |
| RET          |           |            | return                       |
| PUSH         | src       |            | push(src)                    |
| POP          | dest      |            | dest = pop()                 |
| WRITE        | src0      | src1       | ram\[src1\] = src0           |
| READ         | dest      | src1       | dest = ram\[src1\]           |
| PRINT        | src       |            | print(src)                   |
| SPRINT       | string    |            | print(string)                |
| CPRINT       | src       |            | print((ascii)src)            |
| INPUT        |           |            | push((int16_t)input())       |
| SINPUT       |           |            | push((int16_t)input())       |
| RAND         |           |            | push((int16_t)rand(-100,100))|
| EXIT         |           |            | exit()                       |

# Addressing Modes (Source Arguments)
Registers are the medium where values are used, but without a way to put
specific values into the registers, programming gets very verbose and 
tedious. For ease of programming, two other addressing modes are provided.

## Stack addressing: %N
%N will return the Nth element from the top of the stack, where %0 returns the
top of the stack (the same value as using POP). N must be a positive value
within the value of 0 and the current stack_ptr, or a runtime error will occur.

## Ram addressing: \[$N\]
\[$N\] will return the value of address $N in the ram block, with an address
space of $0 to $2249. Going outside this range will cause a runtime error.

## Literal addressing: $N
$N will return the literal value N, which may be any i16. N must be a valid
i16 in the bounds of \[-16383, 16383\], or an assembler error will occur.

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
- <code>Write-Output "main: EXIT\n" | .\\final\\project</code>

# Pal Debugger
When running the program with the -d flag, the program enters the Pal Debugger
before running the program. Intended to be similar to the GNU Debugger (gdb),
this offers a variety of commands to help debug programs and memory issues.

## break
When given an address in the program (without any prefixes), the debugger
will halt execution right before the specified address. If the second argument
is "list", then all current breakpoints will be listed (not necesarily in
sorted order).

## clear
Unlike gdb, the clear command clears the terminal. The equivalent command of
clear in gdb would be delete in PalDB

## continue
This will continue the program's execution until a breakpoint is hit, or until
the program hits an EXIT instruction. Unlike gdb, the program doesn not need
to already be running, as PalDB does not provide a run command. Also unlike
gdb, when the program hits the EXIT instruction, PalDB will also exit.

## delete
If given a address that was set to a breakpoint, said address will have it's
breakpoint removed at that address. If no address is given, all breakpoints
will be deleted.

## help
The help command shows a summary of commands that PalDB can take in.

## disassemble
This will print out the disassembled user program, showing program addresses
of every instruction, the addresses to the string literal data, and the
address of the main label.

## list
The list command simply lists the next instruction to be performed, in a similar
format to the disassemble command. Unlike gdb, you can not list the instructions
outside the next instruction

## next
Execute the next instruction in the user program

## print
Print the specified value. This can be a register value, stack offset, or
address in program memory / ram. Examples:
- p RA
- p RIP
- p %0
- p %1
- p \[$0\]
- p \[$100\]

## quit
Quit the debugger, and exit the program
