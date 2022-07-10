# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

LK2ND_DEVICE_OBJ := $(LOCAL_DIR)/device.o
OBJS += \
	$(LK2ND_DEVICE_OBJ) \

ifneq ($(LK2ND_COMPATIBLE),)
DEFINES += LK2ND_COMPATIBLE="$(LK2ND_COMPATIBLE)"
endif

include $(LOCAL_DIR)/dts/rules.mk
