#! /bin/sh

. ./tests.sh

LOG=tmp.log.$$
EXPECT=tmp.expect.$$
rm -f $LOG $EXPECT
trap "rm -f $LOG $EXPECT" 0

expect="$1"
echo "$expect" >$EXPECT
shift

verbose_run_log "$LOG" $VALGRIND "$DTGET" "$@"
ret="$?"

if [ "$ret" -ne 0 -a "$expect" = "ERR" ]; then
	PASS
fi

FAIL_IF_SIGNAL $ret

diff $EXPECT $LOG
ret="$?"

if [ "$ret" -eq 0 ]; then
	PASS
else
	FAIL
fi
