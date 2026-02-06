#!/bin/bash
# Flash a boot.img to the boot partition via adb while device is in TWRP.
# Usage: ./flash-boot-via-adb.sh /path/to/boot.img [--verify]
# Device must be in TWRP, connected with adb. No fastboot required.
set -e
BOOT_BLOCK="${BOOT_BLOCK:-/dev/block/mmcblk0p21}"
ADB="${ADB:-adb}"
REMOTE_PATH="/tmp/boot_restore.img"
REMOTE_VERIFY="/tmp/boot_verify.bin"
VERIFY=0
[[ "${1:-}" = "--verify" ]] && { VERIFY=1; shift; }
BOOT_IMG="${1:?Usage: $0 [--verify] /path/to/boot.img}"

[[ ! -f "$BOOT_IMG" ]] && { echo "Error: not a file: $BOOT_IMG"; exit 1; }

echo "Pushing $BOOT_IMG to device $REMOTE_PATH..."
"$ADB" push "$BOOT_IMG" "$REMOTE_PATH"

echo "Flashing to $BOOT_BLOCK..."
"$ADB" shell "dd if=$REMOTE_PATH of=$BOOT_BLOCK bs=4096 && sync"

echo "Removing temp file..."
"$ADB" shell "rm -f $REMOTE_PATH"

if [[ -n "$VERIFY" ]] && [[ "$VERIFY" -eq 1 ]]; then
	IMG_SIZE=$(stat -c%s "$BOOT_IMG" 2>/dev/null || stat -f%z "$BOOT_IMG")
	COUNT=$(( (IMG_SIZE + 4095) / 4096 ))
	echo "Verify: pulling first $IMG_SIZE bytes from $BOOT_BLOCK..."
	"$ADB" shell "dd if=$BOOT_BLOCK of=$REMOTE_VERIFY bs=4096 count=$COUNT 2>/dev/null"
	PULLED=$(mktemp)
	trap "rm -f $PULLED" EXIT
	"$ADB" pull "$REMOTE_VERIFY" "$PULLED" >/dev/null 2>&1
	"$ADB" shell "rm -f $REMOTE_VERIFY"
	head -c "$IMG_SIZE" "$PULLED" | cmp -n "$IMG_SIZE" - "$BOOT_IMG" && echo "Verify OK: partition matches $BOOT_IMG" || { echo "Verify FAIL: partition differs from $BOOT_IMG"; exit 1; }
fi

echo "Done. Reboot to use the boot image."
