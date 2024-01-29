#!/bin/bash

cd "$(dirname "$0")"

INPUT_DIR_NAME=tests/input
OUTPUT_DIR_NAME=tests/output
BINARY=lex_tester

echo "Lexer Tests Starting"

count=1

for file in $(ls tests/input/*.java); do
    TEST_NAME=$(basename -s .java $file)
    IN_FILE="${INPUT_DIR_NAME}/${TEST_NAME}.java"
    OUT_FILE="${OUTPUT_DIR_NAME}/${TEST_NAME}.out"

    diff <(./${BINARY} < ${IN_FILE}) <(cat ${OUT_FILE})
    DIFF_CODE=$?
    if [ ${DIFF_CODE} -eq 0 ]; then
        echo "Test $count: ${TEST_NAME} passed"
    else
        echo "Test $count: ${TEST_NAME} failed"
        exit $DIFF_CODE
    fi

    count=$((count+1))
done

echo "Lexer Tests Finished"
