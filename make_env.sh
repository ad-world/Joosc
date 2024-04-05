#!/bin/bash

if [ $0 == $BASH_SOURCE ]; then
    echo "Usage: source $BASH_SOURCE"
    exit 1
fi

# Export STDLIB
export STDLIB="$(find tests/stdlib/java/ -name '*.java')"

# Get all tests for autocompletion
function _test_completions {
    local FILES=$(ls tests/programs/*/marmoset/*/* -d | cut -d'/' -f 6 | sort --unique)
    local cur=$"${COMP_WORDS[COMP_CWORD]}"
    COMPREPLY=($(compgen -W "${FILES}" -- "$cur"))
}

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
complete -F _test_completions getinput

# Find test and run with gdb
#   (supports folders & files)
# Example: rundebug Je_6_StaticThis_AfterStaticInvoke.java
function rundebug {
    INPUT=$(getinput $1); shift
    if [[ -n $INPUT ]]; then
        gdb --args ./joosc $@ $INPUT ${STDLIB}
    fi
}
complete -F _test_completions rundebug

# Find test and run
#   (supports folders & files)
# Example: runtest Je_6_StaticThis_AfterStaticInvoke.java
function runtest {
    INPUT=$(getinput $1); shift
    if [[ -n $INPUT ]]; then
        ./joosc $@ ${INPUT} ${STDLIB}
    fi
}
complete -F _test_completions runtest

function runvalgrind {
    INPUT=$(getinput $1); shift
    if [[ -n $INPUT ]]; then
        valgrind $@ ./joosc ${INPUT} ${STDLIB}
    fi
}
complete -F _test_completions runvalgrind

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
complete -F _test_completions viewtest

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

# Make and run the graph generator
function rungraph {
    INPUT=$(getinput $1)
    if [[ -n $INPUT ]]; then
        make graph && (
            make clean -C graphs/
            ./joosc ${INPUT} ${STDLIB}
            make -C graphs/
        )
    fi
}
complete -F _test_completions rungraph

function run_java_ir {
    javac -d java-ir-bin $(find java-ir -name "*.java")
    java -cp java-ir-bin joosc.ir.interpret.Main
}
