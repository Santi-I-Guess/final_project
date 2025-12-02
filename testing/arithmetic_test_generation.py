import random
import math

# generate many tests, then format into PAL language

# atom type
LABEL = 1
LITERAL_INT = 2
LITERAL_STR = 3
MNEMONIC = 4
REGISTER = 5
SOURCE = 6
STACK_OFFSET = 7


# describe template of instruction
# only the deterministic arithmetic instructions
BLUEPRINTS_MAP: dict[str, list] = {
    "ADD":    [MNEMONIC, REGISTER, SOURCE, SOURCE],
    "SUB":    [MNEMONIC, REGISTER, SOURCE, SOURCE],
    "MUL":    [MNEMONIC, REGISTER, SOURCE, SOURCE],
    "DIV":    [MNEMONIC, REGISTER, SOURCE, SOURCE]}
#    "AND":    [MNEMONIC, REGISTER, SOURCE, SOURCE],
#    "OR":     [MNEMONIC, REGISTER, SOURCE, SOURCE],
#    "NOT":    [MNEMONIC, REGISTER, SOURCE],
#    "XOR":    [MNEMONIC, REGISTER, SOURCE, SOURCE],
#    "LSH":    [MNEMONIC, REGISTER, SOURCE, SOURCE],
#    "RSH":    [MNEMONIC, REGISTER, SOURCE, SOURCE],

MNEMONICS_LIST = list(BLUEPRINTS_MAP.keys())

# only the general purpose registers
registers = ["RA", "RB", "RC", "RD",
             "RE", "RF", "RG", "RH"]


# pick random operation
# pick random, easy to visually verify functions
# simulate arithmetic logic
# print operation and RA

# note: since 1 << 15 is the sign bit, 1 << 14 is for literals, 1 << 13
#       is for string offsets, and 1 << 12 is for string, that means I only
#       have 12 bits for number range.


def clamp(result) -> int:
    if -1 < result and result < 1:
        return 0
    elif result > 4096:
        return 4096
    elif result < -4096:
        return -4096
    return result


if __name__ == "__main__":
    print("main:")
    for i in range(32):
        operation = random.choice(MNEMONICS_LIST)
        arg_1 = int(random.triangular(-4096, 4095))
        arg_2 = int(random.triangular(-4096, 4095))
        if operation == "ADD":
            result = arg_1 + arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}+{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "SUB":
            result = arg_1 - arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}-{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "MUL":
            result = arg_1 * arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}*{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "DIV":
            if arg_2 == 0:
                result = 1
            else:
                result = math.floor(arg_1 / arg_2)
            if -1 < result and result < 1:
                result = 0
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}/{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "AND":
            result = arg_1 & arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}&{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "OR":
            result = arg_1 | arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}|{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "NOT":
            result = 65536 - arg_1
            result = clamp(result)
            print(f"    SPRINT \"~{arg_1} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}")
        elif operation == "XOR":
            result = arg_1 ^ arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}^{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "LSH":
            if arg_2 < 0:
                arg_2 = 0
            result = arg_1 << arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}|{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        elif operation == "RSH":
            if arg_2 < 0:
                arg_2 = 0
            result = arg_1 >> arg_2
            result = clamp(result)
            print(f"    SPRINT \"{arg_1}|{arg_2} = {result}\"")
            print("    SPRINT \"        -> \"")
            print(f"    {operation} RA, ${arg_1}, ${arg_2}")
        print("    PRINT RA")
        print("    CPRINT $10")
    print("    EXIT")
