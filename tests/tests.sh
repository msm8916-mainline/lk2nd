# Common functions for shell testcases

PASS () {
    echo "PASS"
    exit 0
}

FAIL () {
    echo "FAIL" "$@"
    exit 2
}

FAIL_IF_SIGNAL () {
    ret="$1"
    if [ "$ret" -gt 127 ]; then
	signame=$(kill -l $((ret - 128)))
	FAIL "Killed by SIG$signame"
    fi
}

DTC=../dtc
DTGET=../fdtget
DTPUT=../fdtput

verbose_run () {
    if [ -z "$QUIET_TEST" ]; then
	"$@"
    else
	"$@" > /dev/null 2> /dev/null
    fi
}

verbose_run_log () {
    LOG="$1"
    shift
    "$@" > "$LOG" 2>&1
    ret=$?
    if [ -z "$QUIET_TEST" ]; then
	cat "$LOG" >&2
    fi
    return $ret
}
