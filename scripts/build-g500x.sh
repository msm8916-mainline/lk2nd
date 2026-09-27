#!/bin/bash
# G500X (Readboy) lk1st 正式版构建脚本
# 产物:  build-lk1st-msm8952/emmc_appsboot-test-signed.mbn
set -e
cd "$(dirname "$0")/.."

OUT=/root/rkl/lk1st-g500x-xz-official.mbn
RBL=/storage/emulated/0/Stryker/rbl/lk1st-g500x-xz-official.mbn

echo "== 1/4 编译捆绑 DTB =="
dtc -I dts -O dtb -o lk1st-g500x.dtb lk1st-g500x.dts

echo "== 2/4 构建 lk1st =="
make TOOLCHAIN_PREFIX=arm-none-eabi- \
     LK2ND_BUNDLE_DTB="$PWD/lk1st-g500x.dtb" \
     LK2ND_COMPATIBLE="readboy,g500x" \
     ENABLE_DISPLAY=1 \
     LK2ND_DISPLAY=xz_nt51021_video \
     lk1st-msm8952

echo "== 3/4 签名 =="
python3 /root/rkl/qtestsign/qtestsign.py aboot build-lk1st-msm8952/emmc_appsboot.mbn

echo "== 4/4 交付 =="
cp build-lk1st-msm8952/emmc_appsboot-test-signed.mbn "$OUT"
cp "$OUT" "$RBL"
sha1sum "$OUT"
echo "OK: $OUT"