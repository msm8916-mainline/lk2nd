#! /bin/bash

export QUIET_TEST=1

ENV=/usr/bin/env

run_test () {
    echo -n "$@:	"
    PATH=".:$PATH" $ENV "$@"
}

functional_tests () {
    # Read-only tests
    run_test root_node
    run_test property_offset
    run_test subnode_offset
    run_test path_offset
    run_test getprop
    run_test notfound

    # Write-in-place tests
    run_test setprop_inplace
    run_test nop_property
    run_test nop_node
}

stress_tests () {
    ITERATIONS=10           # Number of iterations for looping tests
}

while getopts "vdt:" ARG ; do
    case $ARG in
	"v")
	    unset QUIET_TEST
	    ;;
	"t")
	    TESTSETS=$OPTARG
	    ;;
    esac
done

if [ -z "$TESTSETS" ]; then
    TESTSETS="func stress"
fi

for set in $TESTSETS; do
    case $set in
	"func")
	    functional_tests
	    ;;
	"stress")
	    stress_tests
	    ;;
    esac
done
