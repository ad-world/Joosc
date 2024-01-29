#!/bin/bash

INPUT_DIR_NAME=tests/input
OUTPUT_DIR_NAME=tests/output
BINARY=lex_tester

for file in $(ls tests/input/*.java); do
    TEST_NAME=$(basename -s .java $file)
    IN_FILE="${INPUT_DIR_NAME}/${TEST_NAME}.java"
    OUT_FILE="${OUTPUT_DIR_NAME}/${TEST_NAME}.out"

    diff <(./${BINARY} < ${IN_FILE}) <(cat ${OUT_FILE})
    DIFF_CODE=$?
    if [ ${DIFF_CODE} -eq 0 ]; then
        echo "${TEST_NAME} passed"
    else
        echo "${TEST_NAME} failed"
        exit ${DIFF_CODE}
    fi
done
