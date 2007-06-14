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
    run_test find_property $TREE
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
    # Make sure we don't have stale blobs lying around
    rm -f *.test.dtb

    tree1_tests test_tree1.dtb

    # Sequential write tests
    run_test sw_tree1
    tree1_tests sw_tree1.test.dtb
    tree1_tests unfinished_tree1.test.dtb

    # fdt_move tests
    for tree in test_tree1.dtb sw_tree1.test.dtb unfinished_tree1.test.dtb; do
	rm -f moved.$tree shunted.$tree deshunted.$tree
	run_test move_and_save $tree
	tree1_tests moved.$tree
	tree1_tests shunted.$tree
	tree1_tests deshunted.$tree
    done

    # Read-write tests
    for tree in test_tree1.dtb sw_tree1.test.dtb; do
	rm -f opened.$tree repacked.$tree
	run_test open_pack $tree
	tree1_tests opened.$tree
	tree1_tests repacked.$tree
    done
    run_test setprop test_tree1.dtb
    run_test del_property test_tree1.dtb
    run_test del_node test_tree1.dtb
    run_test rw_tree1
    tree1_tests rw_tree1.test.dtb

    # Tests for behaviour on various sorts of corrupted trees
    run_test truncated_property
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
