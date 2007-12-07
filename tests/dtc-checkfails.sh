#! /bin/sh

. tests.sh

for x; do
    shift
    if [ "$x" = "--" ]; then
	break;
    fi
    CHECKS="$CHECKS $x"
done

LOG="tmp.log.$$"

rm -f $TMPFILE $LOG

verbose_run_log "$LOG" "$DTC" -o /dev/null "$@"
ret="$?"

if [ "$ret" -gt 127 ]; then
    FAIL "dtc killed by signal (ret=$ret)"
fi

for c in $CHECKS; do
    if ! grep -E "^(ERROR)|(Warning) \($c\):" $LOG > /dev/null; then
	FAIL "Failed to trigger check \"%c\""
    fi
done

rm -f $LOG

PASS
