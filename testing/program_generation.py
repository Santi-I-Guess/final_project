# generates a single valid program to stdout

# generate gibberish that's syntatically correct and but algorithmically
# worthless: for testing tokenization, definition, and translation
# guarenteed to have these properties:
# - instructions will precede correct number and types of arguments
# - all labels called by branching instructions are defined
# - at least one EXIT instruction after main label definition

import instructions


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
def print_ins(curr_ins, max_sizes):
    print("    ", end="")
    print_str: str = ""
    for idx, token in enumerate(curr_ins):
        if token[-1] == ':':
            print(token)
            return
        else:
            token = token.ljust(max_sizes[idx] + 2, " ")
            print_str += token
    print_str = print_str.ljust(40, " ")
    print(print_str, end="")


if __name__ == "__main__":
    program_buffer = instructions.gen_raw_program()
    program_buffer = instructions.insert_label_defs(program_buffer)
    max_sizes = get_alignments(program_buffer)
    print("; --- gibberish valid program ---")
    ins_num = 0
    for curr_ins in program_buffer:
        if is_label(curr_ins[0]):
            print(curr_ins[0])
        else:
            print_ins(curr_ins, max_sizes)
            print(f"; ins #{ins_num}")
            ins_num += 1
