#! /bin/sh

PASS () {
    echo "PASS"
    exit 0
}

FAIL () {
    echo "FAIL" "$@"
    exit 2
}

DTC=../dtc

verbose_run () {
    if [ -z "$QUIET_TEST" ]; then
	"$@"
    else
	"$@" > /dev/null 2> /dev/null
    fi
}

if verbose_run "$DTC" "$@"; then
    PASS
else
    ret="$?"
    FAIL "dtc returned error code $ret"
fi
