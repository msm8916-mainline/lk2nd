#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eux

nproc && grep Mem /proc/meminfo && df -hT .
apk add build-base dtc dtc-dev git python3 tar
git clean -dxf

# Workaround problem with faccessat2() on Drone CI
wget https://gist.githubusercontent.com/TravMurav/36c83efbc188115aa9b0fc7f4afba63e/raw/faccessat.c -P /opt
gcc -O2 -shared -o /opt/faccessat.so /opt/faccessat.c
export LD_PRELOAD=/opt/faccessat.so

case "$DRONE_STEP_NAME" in
*gcc4*)
	wget https://gitlab.com/postmarketOS/pmbootstrap/raw/master/pmb/data/keys/build.postmarketos.org.rsa.pub -P /etc/apk/keys
	echo https://mirror.postmarketos.org/postmarketos/v21.12 >> /etc/apk/repositories
	apk add gcc4-armv7
	TOOLCHAIN_PREFIX=gcc4-armv7-alpine-linux-musleabihf-
	;;
*)
	apk add gcc-arm-none-eabi
	TOOLCHAIN_PREFIX=arm-none-eabi-
esac

exec make -j$(nproc) TOOLCHAIN_PREFIX=$TOOLCHAIN_PREFIX "$@"
