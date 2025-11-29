#!/usr/bin/env bash

set -o nounset

project_root=..
 testing_dir=.
  executable=../final_project
    test_gen=test_generation.py


# only regenerate files
if [[ $# -eq 1 ]]; then
    if [[ $1 == regenerate ]]; then
        # only try to delete example_programs if they already exist
        example_files=("${testing_dir}"/example_programs/*)
        if [[ "${#example_files[@]}" -gt 0 ]]; then
            for i in "${testing_dir}"/example_programs/*; do
                rm "${i}"
            done
        fi
        python3 "${test_gen}"
    fi
    exit
fi

# make sure final_project executable exists
if [[ ! -f ${executable} ]]; then
    original_dir=$(realpath ./)
    cd "${project_root}" || exit
    make
    cd "${original_dir}" || exit
fi

# make sure example_programs exists
if [[ ! -d ${testing_dir}/example_programs ]]; then
    mkdir "${testing_dir}"/example_programs
fi

# test valid, then test error
for i in "${testing_dir}"/example_programs/gib_valid_*; do
    printf "\x1b[32m%s\x1b[0m:\n" "${i}"
    "${executable}" "${i}"
done
for i in "${testing_dir}"/example_programs/gib_error_*; do
    printf "\x1b[31m%s\x1b[0m:\n" "${i}"
    "${executable}" "${i}"
done
