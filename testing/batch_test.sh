#!/usr/bin/env bash

# test tokenizer and grammar checker

python3 generate_gibberish_programs.py 64
for i in example_programs/*; do
    printf "\x1b[32m%s\x1b[0m:\n" ${i}
    ../final_project ${i}
done
