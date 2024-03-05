#!/bin/bash

if [ $0 == $BASH_SOURCE ]; then
    echo "Usage: source $BASH_SOURCE"
    exit 1
fi

# Export STDLIB
export STDLIB="$(find tests/stdlib/java/ -name '*.java')"

# Find input files from test name
#   Not for use
function getinput {
    FILE=$(find ./tests/ -name $1 | tail -1)
    if [[ -n $FILE ]]; then
        if [ -f $FILE ]; then
            INPUT=$FILE
        else
            INPUT=$(find $FILE -type f -name "*.java")
        fi
        echo ${INPUT}
    fi
}

# Find test and run with gdb
#   (supports folders & files)
# Example: rundebug Je_6_StaticThis_AfterStaticInvoke.java
function rundebug {
    INPUT=$(getinput $1)
    if [[ -n $INPUT ]]; then
        gdb --args ./joosc $INPUT ${STDLIB}
    fi
}

# Find test and run
#   (supports folders & files)
# Example: runtest Je_6_StaticThis_AfterStaticInvoke.java
function runtest {
    INPUT=$(getinput $1)
    if [[ -n $INPUT ]]; then
        ./joosc ${INPUT} ${STDLIB}
    fi
}

# Run gdb on stdlib
#   (useful for debugging critical errors on minimal input)
function runstdlib {
    gdb --args ./joosc ${STDLIB}
}

# Open test files in editor
function viewtest {
    # FILE=$(find ./tests/ -name $1 | tail -1)
    INPUT=$(getinput $1)
    ${EDITOR} ${INPUT}
}

# Get failing tests
# -> Do a grep on each test
# -> Output tests with matching grep
# Example: search_failing "DISAMBIGUATION"
function search_failing {
    INPUT_CMD='$(getinput {})'
    GREP_STR=$1
    make integration-test 2>/dev/null | grep "FAIL:" | cut -d' ' -f5 |                  # get failing tests
        xargs -i bash -c "source make_env.sh && grep \"$GREP_STR\" $INPUT_CMD -l" |     # grep each failing test for str
        cut -d'/' -f7 | sort | uniq                                                     # get test names only
}
