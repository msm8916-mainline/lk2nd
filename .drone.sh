#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-only
set -eux

nproc && grep Mem /proc/meminfo && df -hT .
apt-get update -qq && apt-get install -yqq gcc gcc-arm-none-eabi device-tree-compiler libfdt-dev make python > /dev/null
make -j$(nproc) TOOLCHAIN_PREFIX=arm-none-eabi- "$1-secondary"
