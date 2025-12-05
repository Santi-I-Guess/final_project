
# Why am I getting plus/minus 16384?
PAL uses the 14th, 13th, and 12th bit to determine the addressing mode of an
argument, and checks the addressing mode in the order of literal, stack,
string. To avoid strange unrepresentable ranges, all values are clamped
to plus/minus 2**12 - 1, which is 16383. To simulate overflow / underflow,
INC and DEC will clamp to the opposite sign of their value if the register
value goes outside \[-16384, 16384\]

# Why does entering nothing for INPUT push $10 to the stack?
INPUT is specifically for inputting integers, and if a letter is given, it
will be interpreted as the ascii value. To make the implementation consistant,
entering nothing will be interpreted as a newline, which is ascii value 10.

# Why is the result of this large multiplication unpredictable?
While operations of 3 digit numbers have been tested via a bash test script,
numbers that are greater than plus/minus 2**14 may have unexpected behavior.
If the operation would result in a number with more than 16 bits, the top
bits will get thrown away (due to c/c++ specification), and the 15th bit,
being the sign bit, may result in the proper sign value, or may not.

To counteract this, MUL, the main cause of this behavior, will check
the expected sign of the result with the actual sign, and change the
sign if the value needed to be clamped. This makes the value a tiny
bit more predictable, but it is recommended not to multiply by
arguments that cause values greater than an int16_t's range.
