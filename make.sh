#!/usr/bin/env bash
set -eu
TARGET="msm8916-secondary"

#rm -rf "build-$TARGET"

make -j8 TOOLCHAIN_PREFIX=arm-none-eabi- "$TARGET"

cd "build-$TARGET"

sudo cp lk.bin ~/.local/var/pmbootstrap/chroot_native/tmp/mainline/lk.bin

pmbootstrap chroot -- mkbootimg-osm0sis \
--kernel "/tmp/mainline/lk.bin" \
--dt "/tmp/mainline/dt.img" \
--base "0x80000000" \
--second_offset "0x00f00000" \
--kernel_offset "0x00008000" \
--tags_offset "0x01e00000" \
--pagesize "2048" \
--cmdline "lk2nd" \
-o "/tmp/mainline/aboot.img"

