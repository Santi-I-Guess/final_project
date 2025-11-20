# generate gibberish that"s syntatically correct and but algorithmically
# worthless: for testing tokenization, definition, and translation
# guarenteed to have these properties:
# - instructions will precede correct number and types of arguments
# - main defined somewhere before EXIT
# - all labels used are defined

import os      # path.abspathj
import random
import sys     # argv


import instructions


# ensure correct target directory regardless of where generator is run
# for any bash scripts that come later to automate testing process
def get_num_tests() -> int:
    if len(sys.argv) != 2:
        return 16
    if sys.argv[1].isdigit():
        num_tests = int(sys.argv[1])
        return num_tests
    message = "{sys.argv[1]} is not a valid positive int: exitting..."
    print(f"\x1b[31m{message}\x1b[0m", file=sys.stderr)
    return 0


# helper function for write_program
def is_label(x) -> bool:
    return x not in instructions.MNEMONICS_LIST


# let max lengths of each idx
def get_alignments(program_buffer) -> list[int]:
    max_sizes = [0, 0, 0, 0]
    for curr_ins in program_buffer:
        if is_label(curr_ins[0]):
            continue
        for idx, atom in enumerate(curr_ins):
            max_sizes[idx] = max(max_sizes[idx], len(atom))
    return max_sizes


# helper function for write_program
def print_ins(curr_ins, max_sizes, sink):
    print("    ", end="", file=sink)
    print_str: str = ""
    for idx, token in enumerate(curr_ins):
        if token[-1] == ':':
            print(token, file=sink)
            return
        else:
            token = token.ljust(max_sizes[idx] + 2, " ")
            print_str += token
    print_str = print_str.ljust(60, " ")
    print(print_str, end="", file=sink)


def write_program(sink, program_buffer, message) -> None:
    max_sizes = get_alignments(program_buffer)
    print(message, file=sink)
    ins_num = 0
    for curr_ins in program_buffer:
        if is_label(curr_ins[0]):
            print(curr_ins[0], file=sink)
        else:
            print_ins(curr_ins, max_sizes, sink)
            print(f"; ins #{ins_num}", file=sink)
            ins_num += 1

if __name__ == "__main__":
    num_tests = get_num_tests()
    if num_tests == 0:
        exit()
    FILE_PATH_SPLIT = str(__file__).split("/")
    DUMP_DIR = os.path.abspath(
        "/".join(FILE_PATH_SPLIT[:-1] + ["example_programs"]))
    # gibberish tests with valid syntax and labels
    for file_idx in range(num_tests):
        program_buffer = instructions.gen_raw_program()
        program_buffer = instructions.insert_label_defs(program_buffer)
        instructions.quick_check(program_buffer)
        file_name = f"{DUMP_DIR}/gib_valid_example_{file_idx:0>2}.txt"
        with open(file_name, "w") as sink:
            message = "; -- gibberish valid program --"
            write_program(sink, program_buffer, message)
    # gibberish tests with some kind of syntax error
    for file_idx in range(num_tests):
        # guarentees at least one error is included
        error_str: str = bin(random.randint(1, 63))[2:].rjust(6, "0")
        error_bits: list[bool] = [i == "1" for i in error_str]
        program_buffer: list[list[str]] = instructions.gen_raw_program()
        program_buffer = instructions.insert_label_defs(program_buffer)
        instructions.quick_check(program_buffer)
        program_buffer = instructions.insert_errors(program_buffer, error_bits)
        file_name = f"{DUMP_DIR}/gib_error_example_{file_idx:0>2}.txt"
        message: str = "; -- gibberish erroneous program --"
        if error_bits[0]:  # EXPECTED_MNEMONIC
            message += "\n; EXPECTED_MNEMONIC"
        if error_bits[1]:  # INVALID_ATOM
            message += "\n; INVALID_ATOM"
        if error_bits[2]:  # MISSING_ARGUMENTS
            message += "\n; MISSING_ARGUMENTS"
        if error_bits[3]:  # MISSING_EXIT
            message += "\n; MISSING_EXIT"
        if error_bits[4]:  # MISSING_MAIN
            message += "\n; MISSING_MAIN"
        if error_bits[5]:  # UNKNOWN_LABEL
            message += "\n; UNKNOWN_LABEL"
        with open(file_name, "w") as sink:
            write_program(sink, program_buffer, message)
