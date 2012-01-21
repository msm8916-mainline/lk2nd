#! /bin/sh

. ./tests.sh

LOG="tmp.log.$$"
EXPECT="tmp.expect.$$"

rm -f $TMPFILE $LOG

expect="$1"
echo "$expect" >$EXPECT
shift

verbose_run_log "$LOG" $VALGRIND "$DTGET" "$@"
ret="$?"

if [ "$ret" -ne 0 -a "$expect" = "ERR" ]; then
	PASS
fi

if [ "$ret" -gt 127 ]; then
    signame=$(kill -l $[ret - 128])
    FAIL "Killed by SIG$signame"
fi

diff $EXPECT $LOG
ret="$?"

rm -f $LOG $EXPECT

if [ "$ret" -eq 0 ]; then
	PASS
else
	FAIL
fi
