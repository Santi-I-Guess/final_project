# generate gibberish that's syntatically correct and but algorithmically
# worthless: for testing tokenization, definition, and translation
# guarenteed to have these properties:
# - instructions will precede correct number and types of arguments
# - main defined somewhere before EXIT
# - all labels used are defined

from enum import Enum
import math  # ceil
import os  # path.abspathj
import random  # triangular
import sys  # argv


# atom type
class Atom_T(Enum):
    LABEL = 1
    LITERAL_INT = 2
    LITERAL_STR = 3
    MNEMONIC = 4
    REGISTER = 5
    SOURCE = 6
    STACK_OFFSET = 7


# describe template of instruction
BLUEPRINTS_MAP: dict[str, list] = {
    "NOP":    [Atom_T.MNEMONIC],
    "CALL":   [Atom_T.MNEMONIC, Atom_T.LABEL],
    "RET":    [Atom_T.MNEMONIC],
    "PRINT":  [Atom_T.MNEMONIC, Atom_T.SOURCE],
    "SPRINT": [Atom_T.MNEMONIC, Atom_T.LITERAL_STR],
    "EXIT":   [Atom_T.MNEMONIC],
    "READ":   [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.LITERAL_INT],
    "WRITE":  [Atom_T.MNEMONIC, Atom_T.SOURCE,   Atom_T.LITERAL_INT],
    "POP":    [Atom_T.MNEMONIC, Atom_T.REGISTER],
    "PUSH":   [Atom_T.MNEMONIC, Atom_T.SOURCE],
    "MOV":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "ADD":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "SUB":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "MUL":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "DIV":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "AND":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "OR":     [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "XOR":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "LSH":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "RSH":    [Atom_T.MNEMONIC, Atom_T.REGISTER, Atom_T.SOURCE],
    "CMP":    [Atom_T.MNEMONIC, Atom_T.SOURCE,   Atom_T.SOURCE],
    "JEQ":    [Atom_T.MNEMONIC, Atom_T.LABEL],
    "JNE":    [Atom_T.MNEMONIC, Atom_T.LABEL],
    "JGE":    [Atom_T.MNEMONIC, Atom_T.LABEL],
    "JGR":    [Atom_T.MNEMONIC, Atom_T.LABEL],
    "JLE":    [Atom_T.MNEMONIC, Atom_T.LABEL],
    "JLS":    [Atom_T.MNEMONIC, Atom_T.LABEL],
}

# allows for random choosing of
MNEMONICS_LIST: list[str] = [
    "NOP",    "CALL", "RET",  "PRINT",
    "SPRINT", "EXIT", "READ", "WRITE",
    "POP",    "PUSH", "MOV",  "ADD",
    "SUB",    "MUL",  "DIV",  "AND",
    "OR",     "XOR",  "LSH",  "RSH",
    "CMP",    "JEQ",  "JNE",  "JGE",
    "JGR",    "JLE",  "JLS",
]


def generate_random_i16() -> int:
    # for readability, make small positive values more common than large values
    if random.randint(0, 3) == 0:
        return math.ceil(random.triangular(-2**15, 2**15))
    else:
        return math.ceil(random.triangular(-2**5, 2**6))

def generate_random_word() -> str:
    # nonsense, but somewhat easy to pronounce, with some variation
    consonants: str = "bcdfgjklprstvw"
    vowels: str = "aeiou"
    output_word: str = ""
    for i in range(4):
        if i % 2 == 0:
            given_char: str = random.choice(consonants)
            output_word += given_char
        else:
            given_char: str = random.choice(vowels)
            output_word += given_char
            if given_char in 'io' and random.randint(0, 3) == 0:
                output_word += 'a'
            if random.randint(0, 7) == 0:
                output_word += "n"
    return output_word


# to prevent scenario where random_word from SPRINT is the same as a label
# name. not a syntax error, but could be confusing
def generate_random_label() -> str:
    alphabet: str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    output_label: str = "."
    output_label += "".join(random.choice(alphabet) for i in range(4))
    output_label += f"_{random.randint(0, 99)}"
    return output_label


def generate_instruction(mnemonic: str) -> list[str]:
    global BLUEPRINTS_MAP, MNEMONICS_LIST
    output_instruction: list[str] = []
    curr_blueprint: list[Atom_T] = BLUEPRINTS_MAP[mnemonic]
    for idx, curr_type in enumerate(curr_blueprint):
        if curr_type == Atom_T.LABEL:
            output_instruction.append(generate_random_label())
        elif curr_type == Atom_T.LITERAL_INT:
            rand_i16: int = generate_random_i16()
            output_instruction.append(f"${rand_i16}")
        elif curr_type == Atom_T.LITERAL_STR:
            # print literal backslash, let final_project try to interpret that
            output_instruction.append(f"\"{generate_random_word()}\\n\"")
        elif curr_type == Atom_T.MNEMONIC:
            output_instruction.append(mnemonic)
        elif curr_type == Atom_T.REGISTER:
            output_instruction.append("R" + random.choice("ABCDEFGH"))
        elif curr_type == Atom_T.SOURCE:
            # LITERAL_INT, REGISTER, STACK_OFFSET
            the_choice: int = random.randint(0, 2)
            if the_choice == 0:
                # LITERAL_INT
                rand_i16: int = generate_random_i16()
                output_instruction.append(f"${rand_i16}")
            elif the_choice == 1:
                # REGISTER
                output_instruction.append("R" + random.choice("ABCDEFGH"))
            else:
                # STACK_OFFSET
                output_instruction.append(f"%{random.randint(0, 4)}")
        elif curr_type == Atom_T.STACK_OFFSET:
            output_instruction.append(f"%{random.randint(0, 4)}")
    return output_instruction


# ensure correct target directory regardless of where generator is run
# for any bash scripts that come later to automate testing process

def get_num_tests() -> int:
    num_tests: int = 16
    if len(sys.argv) == 2:
        if sys.argv[1].isdigit():
            num_tests = int(sys.argv[1])
        else:
            print(
                f"\x1b[31m{sys.argv[1]} is not a valid positive int: "
                "exitting...\x1b[0m",
                file=sys.stderr
            )
            return 0
    return num_tests


def generate_all_instructions() -> list[list[str]]:
    program_buffer: list[list[str]] = []
    has_exit: bool = False
    for i in range(20):
        # don't generate exit on first 10 instructions
        if i < 10:
            mnemonic: str = random.choice(
                MNEMONICS_LIST[0:5] + MNEMONICS_LIST[6:])
        else:
            mnemonic: str = random.choice(MNEMONICS_LIST)
        if (not has_exit) and (mnemonic == "EXIT"):
            has_exit = True
        program_buffer.append(generate_instruction(mnemonic))
    if not has_exit:
        program_buffer.append(generate_instruction("EXIT"))
    return program_buffer


def insert_label_defs(program_buffer: list[list[str]]) -> list[list[str]]:
    new_program_buffer = program_buffer
    labels: list[str] = ["main"]
    # look through instructions with label arguments
    for i in new_program_buffer:
        if i[0] not in ["CALL", "JEQ", "JNE", "JGE", "JGR", "JLE", "JLS"]:
            continue
        label_name = i[1]
        if label_name not in labels:
            labels.append(label_name)
    label_indexes: list[int] = random.sample(
        range(len(new_program_buffer)), len(labels))
    for idx, name in enumerate(labels):
        new_program_buffer.insert(label_indexes[idx], f"{name}:")
    return new_program_buffer


def write_program(sink) -> None:
    # set left align values
    max_sizes = [0, 0, 0]
    for curr_ins in program_buffer:
        if curr_ins[0] not in MNEMONICS_LIST:
            continue
        for idx, i in enumerate(curr_ins):
            if max_sizes[idx] < len(i):
                max_sizes[idx] = len(i)
    # the actual printing
    print("; -- gibberish program --", file=sink)
    for curr_ins in program_buffer:
        if curr_ins[0] not in MNEMONICS_LIST:
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


if __name__ == "__main__":
    num_tests: int = get_num_tests()
    if num_tests == 0:
        exit()
    FILE_PATH_SPLIT: str = str(__file__).split("/")
    DUMP_DIR: list[str] = os.path.abspath(
        "/".join(FILE_PATH_SPLIT[:-1] + ["example_programs"]))
    for file_idx in range(num_tests):
        file_name: str = f"gibberish_program_{file_idx:0>2}.txt"
        program_buffer: list[list[str]] = generate_all_instructions()

        # find and define labels
        program_buffer = insert_label_defs(program_buffer)
        file_name = f"{DUMP_DIR}/g_example_{str(file_idx).rjust(2, "0")}.txt"
        with open(file_name, "w") as sink:
            write_program(sink)
