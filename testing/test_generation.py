# generate gibberish that"s syntatically correct and but algorithmically
# worthless: for testing tokenization, definition, and translation
# guarenteed to have these properties:
# - instructions will precede correct number and types of arguments
# - main defined somewhere before EXIT
# - all labels used are defined

from enum import Enum
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


# note to self: python will resolve program_buffer as a global variable if
# not explicitly defined
def write_program(sink, program_buffer, message) -> None:
    # set left align values
    max_sizes = [0, 0, 0]
    for curr_ins in program_buffer:
        if curr_ins[0] not in instructions.MNEMONICS_LIST:
            # label
            continue
        for idx, atom in enumerate(curr_ins):
            max_sizes[idx] = max(max_sizes[idx], len(atom))
    # the actual printing
    print(message, file=sink)
    for curr_ins in program_buffer:
        if curr_ins[0] not in instructions.MNEMONICS_LIST:
            # label definition
            print(curr_ins, file=sink)
            continue
        # actual instruction
        print("    ", end="", file=sink)
        for idx, token in enumerate(curr_ins):
            if idx == len(curr_ins) - 1:
                print(token, end="", file=sink)
            else:
                print(
                    token.ljust(max_sizes[idx] + 2, " "),
                    end="", file=sink)
        print("", file=sink)


# for eventually generating malformed programs
class Error_T(Enum):
    EXPECTED_MNEMONIC = 1,
    INVALID_ATOM = 1 << 1,
    MISSING_ARGUMENTS = 1 << 2,
    MISSING_EXIT = 1 << 3,
    MISSING_MAIN = 1 << 4,
    UNKNOWN_LABEL = 1 << 5,


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
        file_name = f"{DUMP_DIR}/g_example_{str(file_idx).rjust(2, "0")}.txt"
        with open(file_name, "w") as sink:
            message = "; -- gibberish program --"
            write_program(sink, program_buffer, message)
    # gibberish tests with some kind of syntax error
    for file_idx in range(num_tests):
        program_buffer = instructions.gen_raw_program()
        program_buffer = instructions.insert_label_defs(program_buffer)
        # guarentees at least one error is included
        error_str = bin(random.randint(1, 127))[2:].rjust(5, "0")
        error_bits = [i == "1" for i in error_str]
        print(error_bits)
