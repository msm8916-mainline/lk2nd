#!/bin/bash
# Build lk2nd, then flash lk2nd.img to boot partition via adb (TWRP) and verify.
# Device must be in TWRP, connected with adb. No fastboot required.
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
LK2ND_DIR="$REPO_ROOT/lk2nd"
LK2ND_IMG="${LK2ND_IMG:-$LK2ND_DIR/build-lk2nd-msm8916/lk2nd.img}"
TOOLCHAIN_PREFIX="${TOOLCHAIN_PREFIX:-arm-none-eabi-}"
LK2ND_COMPATIBLE="${LK2ND_COMPATIBLE:-zte,blade-s6}"
LK2ND_DISPLAY="${LK2ND_DISPLAY:-td4291_jdi_720p_video}"

echo "Building lk2nd-msm8916..."
(cd "$LK2ND_DIR" && make \
	TOOLCHAIN_PREFIX="$TOOLCHAIN_PREFIX" \
	LK2ND_COMPATIBLE="$LK2ND_COMPATIBLE" \
	LK2ND_DISPLAY="$LK2ND_DISPLAY" \
	lk2nd-msm8916)

[[ ! -f "$LK2ND_IMG" ]] && { echo "Error: lk2nd image not found: $LK2ND_IMG"; exit 1; }

echo "Flashing and verifying..."
exec "$SCRIPT_DIR/flash-boot-via-adb.sh" --verify "$LK2ND_IMG"
