#! /bin/bash

export QUIET_TEST=1

export VALGRIND=
VGCODE=126

tot_tests=0
tot_pass=0
tot_fail=0
tot_config=0
tot_vg=0
tot_strange=0

run_test () {
    tot_tests=$[tot_tests + 1]
    echo -n "$@:	"
    VGLOCAL="$VALGRIND"
    if [ -n "$VALGRIND" ]; then
	if [ -f $1.supp ]; then
	    VGLOCAL="$VGLOCAL --suppressions=$1.supp"
	fi
    fi
    if $VGLOCAL "./$@"; then
	tot_pass=$[tot_pass + 1]
    else
	ret="$?"
	if [ "$ret" == "1" ]; then
	    tot_config=$[tot_config + 1]
	elif [ "$ret" == "2" ]; then
	    tot_fail=$[tot_fail + 1]
	elif [ "$ret" == "$VGCODE" ]; then
	    tot_vg=$[tot_vg + 1]
	else
	    tot_strange=$[tot_strange + 1]
	fi
    fi
}

tree1_tests () {
    TREE=$1

    # Read-only tests
    run_test get_mem_rsv $TREE
    run_test root_node $TREE
    run_test find_property $TREE
    run_test subnode_offset $TREE
    run_test path_offset $TREE
    run_test get_name $TREE
    run_test getprop $TREE
    run_test get_phandle $TREE
    run_test get_path $TREE
    run_test supernode_atdepth_offset $TREE
    run_test parent_offset $TREE
    run_test node_offset_by_prop_value $TREE
    run_test node_offset_by_phandle $TREE
    run_test node_check_compatible $TREE
    run_test node_offset_by_compatible $TREE
    run_test notfound $TREE

    # Write-in-place tests
    run_test setprop_inplace $TREE
    run_test nop_property $TREE
    run_test nop_node $TREE
}

tree1_tests_rw () {
    TREE=$1

    # Read-write tests
    run_test setprop $TREE
    run_test del_property $TREE
    run_test del_node $TREE
}

ALL_LAYOUTS="mts mst tms tsm smt stm"

libfdt_tests () {
    tree1_tests test_tree1.dtb

    # Sequential write tests
    run_test sw_tree1
    tree1_tests sw_tree1.test.dtb
    tree1_tests unfinished_tree1.test.dtb
    run_test dtbs_equal_ordered test_tree1.dtb sw_tree1.test.dtb

    # fdt_move tests
    for tree in test_tree1.dtb sw_tree1.test.dtb unfinished_tree1.test.dtb; do
	rm -f moved.$tree shunted.$tree deshunted.$tree
	run_test move_and_save $tree
	run_test dtbs_equal_ordered $tree moved.$tree
	run_test dtbs_equal_ordered $tree shunted.$tree
	run_test dtbs_equal_ordered $tree deshunted.$tree
    done

    # v16 and alternate layout tests
    for tree in test_tree1.dtb; do
	for version in 17 16; do
	    for layout in $ALL_LAYOUTS; do
		run_test mangle-layout $tree $version $layout
		tree1_tests v$version.$layout.$tree
		run_test dtbs_equal_ordered $tree v$version.$layout.$tree
	    done
	done
    done

    # Read-write tests
    for basetree in test_tree1.dtb; do
	for version in 17 16; do
	    for layout in $ALL_LAYOUTS; do
		tree=v$version.$layout.$basetree
		rm -f opened.$tree repacked.$tree
		run_test open_pack $tree
		tree1_tests opened.$tree
		tree1_tests repacked.$tree

		tree1_tests_rw $tree
		tree1_tests_rw opened.$tree
		tree1_tests_rw repacked.$tree
	    done
	done
    done
    run_test rw_tree1
    tree1_tests rw_tree1.test.dtb
    tree1_tests_rw rw_tree1.test.dtb

    # Tests for behaviour on various sorts of corrupted trees
    run_test truncated_property
}

dtc_tests () {
    run_test dtc.sh -I dts -O dtb -o dtc_tree1.test.dtb test_tree1.dts
    tree1_tests dtc_tree1.test.dtb
    tree1_tests_rw dtc_tree1.test.dtb
    run_test dtbs_equal_ordered dtc_tree1.test.dtb test_tree1.dtb

    run_test dtc.sh -I dts -O dtb -o dtc_tree1_dts0.test.dtb test_tree1_dts0.dts
    tree1_tests dtc_tree1_dts0.test.dtb
    tree1_tests_rw dtc_tree1_dts0.test.dtb

    run_test dtc.sh -I dts -O dtb -o dtc_escapes.test.dtb escapes.dts
    run_test string_escapes dtc_escapes.test.dtb

    run_test dtc.sh -I dts -O dtb -o dtc_references.test.dtb references.dts
    run_test references dtc_references.test.dtb

    # Check -Odts mode preserve all dtb information
    for tree in test_tree1.dtb dtc_tree1.test.dtb dtc_escapes.test.dtb ; do
	run_test dtc.sh -I dtb -O dts -o odts_$tree.test.dts $tree
	run_test dtc.sh -I dts -O dtb -o odts_$tree.test.dtb odts_$tree.test.dts
	run_test dtbs_equal_ordered $tree odts_$tree.test.dtb
    done

    # Check some checks
    run_test dtc-checkfails.sh -I dts -O dtb dup-nodename.dts
    run_test dtc-checkfails.sh -I dts -O dtb dup-propname.dts
    run_test dtc-checkfails.sh -I dts -O dtb dup-phandle.dts
    run_test dtc-checkfails.sh -I dts -O dtb zero-phandle.dts
    run_test dtc-checkfails.sh -I dts -O dtb minusone-phandle.dts
    run_test dtc-checkfails.sh -I dts -O dtb nonexist-node-ref.dts
    run_test dtc-checkfails.sh -I dts -O dtb nonexist-label-ref.dts
}

while getopts "vt:m" ARG ; do
    case $ARG in
	"v")
	    unset QUIET_TEST
	    ;;
	"t")
	    TESTSETS=$OPTARG
	    ;;
	"m")
	    VALGRIND="valgrind --tool=memcheck -q --error-exitcode=$VGCODE"
	    ;;
    esac
done

if [ -z "$TESTSETS" ]; then
    TESTSETS="libfdt dtc"
fi

# Make sure we don't have stale blobs lying around
rm -f *.test.dtb *.test.dts

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
if [ -n "$VALGRIND" ]; then
    echo -e "*    valgrind errors:	$tot_vg"
fi
echo -e "* Strange test result:	$tot_strange"
echo -e "**********"

