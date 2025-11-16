#!/usr/bin/env bash

# eventually write .ps1 script for windows users

required_dirs=( build testing/example_programs )
for i in ${required_dirs[@]}; do
    if [[ ! -d ${i} ]]; then
        echo "+ mkdir ${i}"
        mkdir ${i}
    fi
done
