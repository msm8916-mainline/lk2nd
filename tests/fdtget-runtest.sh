#! /bin/sh

. ./tests.sh

LOG=tmp.log.$$
EXPECT=tmp.expect.$$
rm -f $LOG $EXPECT
trap "rm -f $LOG $EXPECT" 0

expect="$1"
echo $expect >$EXPECT
shift

verbose_run_log_check "$LOG" $VALGRIND $DTGET "$@"

diff $EXPECT $LOG
ret="$?"

if [ "$ret" -eq 0 ]; then
	PASS
else
	FAIL
fi
