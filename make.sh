#!/usr/bin/env bash
set -eu
TARGET="msm8916-secondary"

rm -rf "build-$TARGET"

make -j6 TOOLCHAIN_PREFIX=arm-none-eabi- "$TARGET"

cd "build-$TARGET"
../scripts/mkbootimg \
    --kernel=lk.bin \
    --ramdisk=/dev/null \
    --dt=../../dt.img \
    --base=0x80000000 \
    --output=boot.img \
    --cmdline="lk2nd"
