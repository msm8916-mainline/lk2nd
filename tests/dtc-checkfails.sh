#! /bin/sh

. tests.sh

TMPFILE="tmp.out.$$"

rm -f $TMPFILE

verbose_run "$DTC" -o $TMPFILE "$@"
ret="$?"

if [ -f $TMPFILE ]; then
    FAIL "output file was created despite bad input"
fi

if [ "$ret" = "2" ]; then
    PASS
else
    FAIL "dtc returned error code $ret instead of 2 (check failed)"
fi

rm -f $TMPFILE
