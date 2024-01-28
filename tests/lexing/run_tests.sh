#!/bin/bash

DIR_NAME=tests
BINARY=lex_tester

for file in $(ls tests/*.in); do
    TEST_NAME=$(basename -s .in $file)
    IN_FILE="${DIR_NAME}/${TEST_NAME}.in"
    OUT_FILE="${DIR_NAME}/${TEST_NAME}.out"

    diff <(./${BINARY} < ${IN_FILE}) <(cat ${OUT_FILE})
    DIFF_CODE=$?
    if [ ${DIFF_CODE} -eq 0 ]; then
        echo "${TEST_NAME} passed"
    else
        echo "${TEST_NAME} failed"
        exit ${DIFF_CODE}
    fi
done
