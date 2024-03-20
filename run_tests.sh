EXEC="./joosc"

TEST_PREFIX="tests/programs"
# Assignment folder
TEST_POSTFIX="marmoset"
# valid/invalid

STDLIB_DIR="tests/stdlib/java"
STDLIB=$(find "$STDLIB_DIR" -type f)

pids=()

function run_exec {
    ASSIGNMENT_NUM=$1; shift
    TEST_FOLDER=$1; shift

    if [[ $TEST_FOLDER == "valid" ]]; then
        EXPECTED_RC=0
    elif [[ $TEST_FOLDER == "invalid" ]]; then
        EXPECTED_RC=42
    elif [[ $TEST_FOLDER == "warning" ]]; then
        EXPECTED_RC=43
    fi

    FULL_PATH="${TEST_PREFIX}/${ASSIGNMENT_NUM}/${TEST_POSTFIX}/${TEST_FOLDER}"

    for TEST in $(ls $FULL_PATH); do
        TEST_PATH="${FULL_PATH}/${TEST}"
        TEST_FILES=$(find "$TEST_PATH" -type f)

        STDOUT=$(mktemp)
        STDERR=$(mktemp)
        $EXEC ${TEST_FILES} ${STDLIB} >$STDOUT 2>$STDERR &
        PID=$!
        pids+=("$PID:$EXPECTED_RC:$STDOUT:$STDERR:$TEST:$ASSIGNMENT_NUM")
    done
}

RED=$(tput setaf 9)
YELLOW=$(tput setaf 11)
PURPLE=$(tput setaf 13)
RESET=$(tput sgr0)
function collect_pids {
    COLLECTED_ASSIGNMENT=$1; shift
    echo "${PURPLE}Results for $COLLECTED_ASSIGNMENT:"
    for pid_info in "${pids[@]}"; do
        IFS=':' read -r PID EXPECTED_RC STDOUT STDERR TEST ASSIGNMENT_NUM <<<"$pid_info"

        if [[ $COLLECTED_ASSIGNMENT == $ASSIGNMENT_NUM ]]; then
            wait $PID
            RC=$?
            if (( $RC != $EXPECTED_RC )); then
                echo "${RED}FAILED test case $TEST - expected exit code $EXPECTED_RC (got $RC):"
                echo -n "${YELLOW}"
                cat $STDOUT | sed -e "s/^/\t/"
                cat $STDERR | sed -e "s/^/\t/"
            fi
            rm "$STDOUT" "$STDERR"
        fi
    done
    echo "${RESET}"
}

run_exec a1 valid
run_exec a1 invalid

run_exec a2 valid
run_exec a2 invalid

run_exec a3 valid
run_exec a3 invalid

run_exec a4 valid
run_exec a4 invalid
run_exec a4 warning

run_exec a5 valid
run_exec a5 invalid

collect_pids a1
collect_pids a2
collect_pids a3
collect_pids a4
collect_pids a5
