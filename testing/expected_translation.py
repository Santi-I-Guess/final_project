import sys
import instructions

if sys.argv == 1:
    print("usage: python3 assembler_expectations.py <token>")
    exit()

# because I want to catch errors like [$-1], errors won't quit the translation

token: str = sys.argv[1]
print(token)
if token[0] == '[':
    # ram address
    token = token[2:-1]
    value = int(token)
    if value < 0 or value > 2249:
        print("warning: ram address outside bounds")
    value |= (1 << 13)
    hex_value: str = hex(value)[2:].rjust(4, "0")
    little_endian_hex: str = hex_value[2:] + hex_value[0:2]
    print(f"little endian hex: {little_endian_hex}")
    big_endian_bin: str = bin(value)[2:].rjust(16, "0")
    print(f"big endian binary: {big_endian_bin[:8]},{big_endian_bin[8:]}")
elif token[0] == '%':
    # stack offset
    token = token[1:]
    value = int(token)
    if value < 0:
        print("warning: stack offset can't be less than 0")
    value |= (2 << 13)
    hex_value: str = hex(value)[2:].rjust(4, "0")
    little_endian_hex: str = hex_value[2:] + hex_value[0:2]
    print(f"little endian hex: {little_endian_hex}")
    big_endian_bin: str = bin(value)[2:].rjust(16, "0")
    print(f"big endian binary: {big_endian_bin[:8]},{big_endian_bin[8:]}")
elif token[0] == '$':
    # literal value
    token = token[1:]
    value = int(token)
    if value < (-(1 << 14) + 1) or value > ((1 << 14) - 1):
        print("warning: value is outside bounds of [-16383,16383]")
    if value < 0:
        value += (2 << 15)  # == 1 << 16
    value |= (3 << 13)
    hex_value: str = hex(value)[2:].rjust(4, "0")
    little_endian_hex: str = hex_value[2:] + hex_value[0:2]
    print(f"little endian hex: {little_endian_hex}")
    big_endian_bin: str = bin(value)[2:].rjust(16, "0")
    print(f"big endian binary: {big_endian_bin[:8]},{big_endian_bin[8:]}")
elif token in instructions.MNEMONICS_LIST:
    value = instructions.MNEMONICS_LIST.index(token)
    hex_value: str = hex(value)[2:].rjust(4, "0")
    little_endian_hex: str = hex_value[2:] + hex_value[0:2]
    print(f"little endian hex: {little_endian_hex}")
    big_endian_bin: str = bin(value)[2:].rjust(16, "0")
    print(f"big endian binary: {big_endian_bin[:8]},{big_endian_bin[8:]}")
