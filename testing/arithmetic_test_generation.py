import random
import math
import sys

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
#       have 13 bits for number range (since it checks for literals first,
#       we can use bits reserved for addressing in that case).

max_value = 16383
min_value = -16383


def clamp(result) -> int:
    if abs(result) < 1:
        result = 0
    elif result > max_value:
        return max_value
    elif result < min_value:
        return min_value
    return result


operation_map = {
    "ADD": "+",
    "SUB": "-",
    "MUL": "*",
    "DIV": "/",
}


if __name__ == "__main__":
    num_tests = 0
    if len(sys.argv) == 1:
        num_tests = 8
    else:
        num_tests = int(sys.argv[1])
    print("main:")
    for i in range(num_tests):
        operation = random.choice(MNEMONICS_LIST)
        # arg_1 = int(random.triangular(min_value, max_value))
        # arg_2 = int(random.triangular(min_value, max_value))
        arg_1 = int(random.triangular(-200, -100))
        arg_2 = int(random.triangular(-200, -100))
        if operation == "ADD":
            result = arg_1 + arg_2
            result = clamp(result)
        elif operation == "SUB":
            result = arg_1 - arg_2
            result = clamp(result)
        elif operation == "MUL":
            result = arg_1 * arg_2
            # result_bin = bin(result)[2:]
            # result = int(result_bin[len(result_bin)-8:], 2)
            result = clamp(result)
        elif operation == "DIV":
            if arg_2 == 0:
                result = 1
            else:
                result = math.trunc(arg_1 / arg_2)
            if -1 < result and result < 1:
                result = 0

        result = clamp(result)
        print("    ; next operation")
        arg_1 = f"${arg_1}"
        arg_2 = f"${arg_2}"
        print(f"    SPRINT \"{arg_1:<9} {operation_map[operation]}", end="")
        print(f" {arg_2:<9} = ${result:<6}\"")
        if operation == "ADD":
            print("    PUSH $0 ; add")
        if operation == "SUB":
            print("    PUSH $1 ; sub")
        if operation == "MUL":
            print("    PUSH $2 ; mul")
        if operation == "DIV":
            print("    PUSH $3 ; div")
        print(f"    PUSH ${result:<8} ; expected")
        print(f"    PUSH {arg_2:<9} ; arg_2")
        print(f"    PUSH {arg_1:<9} ; arg_1")
        print("    CALL test_oper")

    print("    EXIT")
    print("""\
test_oper:
    ; %0 is arg_1, %1 is arg_2, %2 is expected
    ; %3 is operation: (0|1|2|3)=(add|sub|mul|div)
    MOV RA, %0
    MOV RB, %1
    MOV RC, %2
    MOV RD, %3
    ; check operation to perform
    CMP RD, $0
    JEQ test_oper_add
    CMP RD, $1
    JEQ test_oper_sub
    CMP RD, $2
    JEQ test_oper_mul
    JMP test_oper_div
test_oper_add:
    ADD RA, RB
    JMP check
test_oper_sub:
    SUB RA, RB
    JMP check
test_oper_mul:
    MUL RA, RB
    JMP check
test_oper_div:
    DIV RA, RB
check:
    SPRINT "  -> $"
    PRINT RA
    CPRINT $10
    CMP RA, RC
    JEQ final
    SPRINT "expected "
    PRINT RC
    SPRINT ", actual "
    PRINT RA
    CPRINT $10
    JMP final
final:
    ; dump parameters
    POP RZ
    POP RZ
    POP RZ
    POP RZ
    RET""")
