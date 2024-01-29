#!/bin/bash

cd "$(dirname "$0")"

INPUT_DIR_NAME=tests/input
OUTPUT_DIR_NAME=tests/output
BINARY=../../build/lex_tester

NCOLORS=$(tput colors)

if [ $NCOLORS -ge 8 ]; then
    normal=$(tput sgr0)
    red=$(tput setaf 1)
    green=$(tput setaf 2)
fi

echo "Lexer Tests Starting"

count=1

for file in $(ls tests/input/*.java); do
    TEST_NAME=$(basename -s .java $file)
    IN_FILE="${INPUT_DIR_NAME}/${TEST_NAME}.java"
    OUT_FILE="${OUTPUT_DIR_NAME}/${TEST_NAME}.out"

    diff --strip-trailing-cr <(./${BINARY} < ${IN_FILE}) <(cat ${OUT_FILE})
    DIFF_CODE=$?
    if [ ${DIFF_CODE} -eq 0 ]; then
        echo "${green}Test $count: ${TEST_NAME} passed${normal}"
    else
        echo "${red}Test $count: ${TEST_NAME} failed${normal}"
        exit $DIFF_CODE
    fi

    count=$((count+1))
done

echo "Lexer Tests Finished"
