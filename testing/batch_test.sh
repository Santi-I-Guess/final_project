#!/usr/bin/env bash

set -o nounset

# assuming project directory is named "final_project"

# get relative paths to important files
project_root=$(realpath --relative-to=./ $(pwd | sed "s/\(.*final_project\).*/\1/"))
 testing_dir=$(realpath --relative-to=./ ${project_root}/testing)
  executable=$(realpath --relative-to=./ ${project_root}/final_project)
    test_gen=$(realpath --relative-to=./ ${testing_dir}/test_generation.py)

# make sure final_project executable exists
if [[ ! -f ${executable} ]]; then
    original_dir=$(realpath ./)
    cd ${project_root}
    make
    cd ${original_dir}
fi

# make sure example_programs exists
if [[ ! -d ${testing_dir}/example_programs ]]; then
    mkdir ${testing_dir}/example_programs
fi

# test valid, then test error
for i in ${testing_dir}/example_programs/gib_valid_*; do
    printf "\x1b[32m${i}\x1b[0m:\n"
    ${executable} ${i}
done
for i in ${testing_dir}/example_programs/gib_error_*; do
    printf "\x1b[31m${i}\x1b[0m:\n"
    ${executable} ${i}
done
