#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
DIR=$(dirname "$0")
set -eux

nproc && grep Mem /proc/meminfo && df -hT .
apk add build-base dtc dtc-dev gcc-arm-none-eabi git python3 tar

# Workaround problem with faccessat2() on older Docker/libseccomp versions
gcc -shared -o "$DIR"/faccessat-simple.so "$DIR"/faccessat-simple.c
export LD_PRELOAD="$PWD/$DIR"/faccessat-simple.so

wget https://gitlab.com/postmarketOS/pmbootstrap/raw/master/pmb/data/keys/build.postmarketos.org.rsa.pub -P /etc/apk/keys
echo https://mirror.postmarketos.org/postmarketos/master >> /etc/apk/repositories
apk add gcc4-armv7

make -j$(nproc) TOOLCHAIN_PREFIX=gcc4-armv7-alpine-linux-musleabihf- "$@"
