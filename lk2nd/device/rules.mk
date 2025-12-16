# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
	   lib/libfdt \
	   lk2nd/hw/gpio \

LK2ND_DEVICE_OBJ := $(LOCAL_DIR)/device.o

# NOTE: The init functions are performed in the order of linking
# thus the order might be important here. Notably:
#
# - keys.o must be after device.o as device.o will (if needed) probe
#   the keys driver and the keys.o will then rely on the created data
#   to report the keys.

OBJS += \
	$(LK2ND_DEVICE_OBJ) \
	$(LOCAL_DIR)/panel.o \
	$(LOCAL_DIR)/keys.o \
	$(LOCAL_DIR)/leds.o \
	$(LOCAL_DIR)/regulator-fixed.o \

ifneq ($(LK2ND_COMPATIBLE),)
DEFINES += LK2ND_COMPATIBLE="$(LK2ND_COMPATIBLE)"
endif

include \
	$(if $(filter msm8916, $(TARGET)), $(LOCAL_DIR)/smb1360/rules.mk) \
	$(LOCAL_DIR)/dts/rules.mk \
