#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
DIR=$(dirname "$0")
set -eux

nproc && grep Mem /proc/meminfo && df -hT .
apk add build-base dtc dtc-dev gcc-arm-none-eabi git python3 tar

# Workaround problem with faccessat2() on Drone CI
wget https://gist.githubusercontent.com/TravMurav/36c83efbc188115aa9b0fc7f4afba63e/raw/faccessat.c
gcc -shared -o faccessat.so faccessat.c
export LD_PRELOAD="$PWD"/faccessat.so

wget https://gitlab.com/postmarketOS/pmbootstrap/raw/master/pmb/data/keys/build.postmarketos.org.rsa.pub -P /etc/apk/keys
echo https://mirror.postmarketos.org/postmarketos/v21.12 >> /etc/apk/repositories
apk add gcc4-armv7

make -j$(nproc) TOOLCHAIN_PREFIX=gcc4-armv7-alpine-linux-musleabihf- "$@"
