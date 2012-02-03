#! /bin/sh

# Run script for fdtput tests
# We run fdtput to update the device tree, thn fdtget to check it

# Usage
#    fdtput-runtest.sh name expected_output dtb_file node property flags value

. ./tests.sh

LOG=tmp.log.$$
EXPECT=tmp.expect.$$
rm -f $LOG $EXPECT
trap "rm -f $LOG $EXPECT" 0

expect="$1"
echo $expect >$EXPECT
dtb="$2"
node="$3"
property="$4"
flags="$5"
shift 5
value="$@"

# First run fdtput
verbose_run_check $VALGRIND "$DTPUT" "$dtb" "$node" "$property" $value $flags

# Now fdtget to read the value
verbose_run_log_check "$LOG" $VALGRIND "$DTGET" "$dtb" "$node" "$property" $flags

diff $EXPECT $LOG
ret="$?"

if [ "$ret" -eq 0 ]; then
	PASS
else
	FAIL
fi
