#! /bin/sh

. ./tests.sh

for x; do
    shift
    if [ "$x" = "--" ]; then
	break;
    fi
    CHECKS="$CHECKS $x"
done

LOG=tmp.log.$$
rm -f $LOG
trap "rm -f $LOG" 0

verbose_run_log "$LOG" $VALGRIND "$DTC" -o /dev/null "$@"
ret="$?"

FAIL_IF_SIGNAL $ret

for c in $CHECKS; do
    if ! grep -E "^(ERROR)|(Warning) \($c\):" $LOG > /dev/null; then
	FAIL "Failed to trigger check \"$c\""
    fi
done

PASS
