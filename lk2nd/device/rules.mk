# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

LK2ND_DEVICE_OBJ := $(LOCAL_DIR)/device.o
OBJS += \
	$(LK2ND_DEVICE_OBJ) \
	$(LOCAL_DIR)/panel.o \

ifneq ($(LK2ND_COMPATIBLE),)
DEFINES += LK2ND_COMPATIBLE="$(LK2ND_COMPATIBLE)"
endif

include \
	$(if $(filter msm8916, $(TARGET)), $(LOCAL_DIR)/smb1360/rules.mk) \
	$(LOCAL_DIR)/dts/rules.mk \
