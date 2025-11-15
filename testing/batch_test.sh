#!/usr/bin/env bash

# test tokenizer and grammar checker

rm example_programs/*
python3 test_generation.py 8
for i in example_programs/*; do
    printf "\x1b[32m%s\x1b[0m:\n" ${i}
    ../final_project ${i}
done
