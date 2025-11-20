# Application Binary Interface: How the Compiled Program is Layed Out

The first int16_t of the program data is the program address of the
main label, and is followed by the string data of the program.

Next, every string in the program is concatinated into the beginning of the
program. This is done by packing every 2 chars of the string into one int16_t.
Every string is marked with an int16_t at the end equal to 0, to allow for
variable length strings. Because of this, compiled SPRINT instructions are not
followed by their string data, but by a "string index" which points to the
program address of the first int16_t of the string.

The rest of the program data is the compiled tokens of the program: one
int16_t per token.

Each mnemonic and argument declared in the program other than string data
is converted into a single int16_t. Literal integer arguments have
a bitmask of 1 << 30, and stack offset arguments a bitmask of 1 << 29.
String indexes are not given a bitmask, since they only occur in the integer
after SPRINT.
