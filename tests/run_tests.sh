#! /bin/bash

export QUIET_TEST=1

ENV=/usr/bin/env

tot_tests=0
tot_pass=0
tot_fail=0
tot_config=0
tot_strange=0

run_test () {
    tot_tests=$[tot_tests + 1]
    echo -n "$@:	"
    if PATH=".:$PATH" $ENV "$@"; then
	tot_pass=$[tot_pass + 1]
    else
	ret="$?"
	if [ "$ret" == "1" ]; then
	    tot_config=$[tot_config + 1]
	elif [ "$ret" == "2" ]; then
	    tot_fail=$[tot_fail + 1]
	else
	    tot_strange=$[tot_strange + 1]
	fi
    fi
}

tree1_tests () {
    TREE=$1

    # Read-only tests
    run_test root_node $TREE
    run_test find_property $TREE
    run_test subnode_offset $TREE
    run_test path_offset $TREE
    run_test get_name $TREE
    run_test getprop $TREE
    run_test get_path $TREE
    run_test supernode_atdepth_offset $TREE
    run_test parent_offset $TREE
    run_test node_offset_by_prop_value $TREE
    run_test notfound $TREE

    # Write-in-place tests
    run_test setprop_inplace $TREE
    run_test nop_property $TREE
    run_test nop_node $TREE
}

libfdt_tests () {
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

dtc_tests () {
    # Make sure we don't have stale blobs lying around
    rm -f *.test.dtb

    run_test dtc.sh -f -I dts -O dtb -o dtc_tree1.test.dtb test_tree1.dts
    tree1_tests dtc_tree1.test.dtb
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
    TESTSETS="libfdt dtc"
fi

for set in $TESTSETS; do
    case $set in
	"libfdt")
	    libfdt_tests
	    ;;
	"dtc")
	    dtc_tests
	    ;;
    esac
done

echo -e "********** TEST SUMMARY"
echo -e "*     Total testcases:	$tot_tests"
echo -e "*                PASS:	$tot_pass"
echo -e "*                FAIL:	$tot_fail"
echo -e "*   Bad configuration:	$tot_config"
echo -e "* Strange test result:	$tot_strange"
echo -e "**********"

