# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

# NOTE: lenok wants to be first to be able to boot.
ADTBS += \
	$(LOCAL_DIR)/apq8026-lg-lenok.dtb \
	$(LOCAL_DIR)/msm8226-motorola-falcon.dtb \
	$(LOCAL_DIR)/msm8926-motorola-peregrine.dtb \
	$(LOCAL_DIR)/msm8926-motorola-thea.dtb \

QCDTBS += \
	$(LOCAL_DIR)/apq8026-asus-sparrow.dtb \
	$(LOCAL_DIR)/apq8026-huawei-sturgeon.dtb \
	$(LOCAL_DIR)/apq8026-samsung.dtb \
	$(LOCAL_DIR)/msm8226-motorola-titan.dtb \
	$(LOCAL_DIR)/msm8226-samsung.dtb \
	$(LOCAL_DIR)/msm8926-htc-memul.dtb \
	$(LOCAL_DIR)/msm8926-huawei-g6-l11-vb.dtb \
	$(LOCAL_DIR)/msm8926-lg-madai.dtb \
	$(LOCAL_DIR)/msm8926-lg-vfp.dtb \
	$(LOCAL_DIR)/msm8926-samsung.dtb \
	$(LOCAL_DIR)/msm8926-v2-720p-mtp.dtb \

DTBS += \
	$(LOCAL_DIR)/lumia.dtb \
