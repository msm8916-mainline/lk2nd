#! /bin/sh

. tests.sh

if verbose_run "$DTC" "$@"; then
    PASS
else
    ret="$?"
    FAIL "dtc returned error code $ret"
fi
