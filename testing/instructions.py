from enum import Enum
import math
import random


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
BLUEPRINTS_MAP = {
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


# allows for random choosing
MNEMONICS_LIST = list(BLUEPRINTS_MAP.keys())


def gen_random_i16() -> int:
    # for readability, make small positive values more common than large values
    if random.randint(0, 3) == 0:
        return math.ceil(random.triangular(-2**15, 2**15))
    else:
        return math.ceil(random.triangular(-2**5, 2**6))


def gen_random_word() -> str:
    # nonsense, but somewhat easy to pronounce, with some variation
    consonants: str = "bcdfgjklprstvw"
    vowels: str = "aeiou"
    output_word: str = ""
    for i in range(4):
        if i % 2 == 0:
            given_char: str = random.choice(consonants)
            output_word += given_char
            if random.randint(0, 7) == 0 and given_char != "w":
                output_word += "w"
        else:
            given_char: str = random.choice(vowels)
            output_word += given_char
            if given_char in "io" and random.randint(0, 3) == 0:
                output_word += "a"
            if random.randint(0, 7) == 0:
                output_word += "n"
    return output_word


# to prevent scenario where random_word from SPRINT is the same as a label
# name. not a syntax error, but could be confusing
def gen_random_label() -> str:
    alphabet: str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    output_label: str = "."
    output_label += "".join(random.choice(alphabet) for i in range(4))
    output_label += f"_{random.randint(0, 99)}"
    return output_label


def gen_instruction() -> list[str]:
    global BLUEPRINTS_MAP
    mnemonic = random.choice(MNEMONICS_LIST)
    output_instruction = []
    curr_blueprint = BLUEPRINTS_MAP[mnemonic]
    for idx, curr_type in enumerate(curr_blueprint):
        if curr_type == Atom_T.LABEL:
            output_instruction.append(gen_random_label())
        elif curr_type == Atom_T.LITERAL_INT:
            rand_i16 = gen_random_i16()
            output_instruction.append(f"${rand_i16}")
        elif curr_type == Atom_T.LITERAL_STR:
            # print literal backslash, let final_project try to interpret that
            output_instruction.append(f"\"{gen_random_word()}\\n\"")
        elif curr_type == Atom_T.MNEMONIC:
            output_instruction.append(mnemonic)
        elif curr_type == Atom_T.REGISTER:
            output_instruction.append("R" + random.choice("ABCDEFGH"))
        elif curr_type == Atom_T.SOURCE:
            # LITERAL_INT, REGISTER, STACK_OFFSET
            the_choice = random.randint(0, 2)
            if the_choice == 0:
                # LITERAL_INT
                rand_i16 = gen_random_i16()
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


def gen_raw_program() -> list[list[str]]:
    program_buffer = []
    for i in range(20):
        program_buffer.append(gen_instruction())
    if "EXIT" not in [i[0] for i in program_buffer]:
        program_buffer.append(["EXIT"])
    while program_buffer[0][0] == "EXIT":
        random.shuffle(program_buffer)
    return program_buffer


def insert_label_defs(program_buffer: list[list[str]]) -> list[list[str]]:
    new_program_buffer = program_buffer
    labels = ["main"]
    # look through instructions with label arguments
    for i in new_program_buffer:
        if i[0] not in ["CALL", "JEQ", "JNE", "JGE", "JGR", "JLE", "JLS"]:
            continue
        label_name = i[1]
        if label_name not in labels:
            labels.append(label_name)
    label_indexes = random.sample(
        range(2, len(new_program_buffer)),
        len(labels)
    )
    random.shuffle(labels)
    for idx, name in enumerate(labels):
        if idx == 0:
            new_program_buffer.insert(0, f"{name}:")
        else:
            new_program_buffer.insert(label_indexes[idx], f"{name}:")

    return new_program_buffer
