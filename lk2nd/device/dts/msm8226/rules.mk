# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

# NOTE: lenok wants to be first to be able to boot.
ADTBS += \
	$(LOCAL_DIR)/apq8026-lg-lenok.dtb \

QCDTBS += \
	$(LOCAL_DIR)/apq8026-asus-sparrow.dtb \
	$(LOCAL_DIR)/apq8026-huawei-sturgeon.dtb \


DTBS += \
	$(LOCAL_DIR)/lumia.dtb \
