#! /bin/bash

export QUIET_TEST=1

ENV=/usr/bin/env

run_test () {
    echo -n "$@:	"
    PATH=".:$PATH" $ENV "$@"
}

tree1_tests () {
    TREE=$1

    # Read-only tests
    run_test root_node $TREE
    run_test property_offset $TREE
    run_test subnode_offset $TREE
    run_test path_offset $TREE
    run_test getprop $TREE
    run_test notfound $TREE

    # Write-in-place tests
    run_test setprop_inplace $TREE
    run_test nop_property $TREE
    run_test nop_node $TREE
}

functional_tests () {
    tree1_tests test_tree1.dtb
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
