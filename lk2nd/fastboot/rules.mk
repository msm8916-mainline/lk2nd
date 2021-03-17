# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/fetch.o \
	$(LOCAL_DIR)/misc.o \

ifneq ($(filter DISPLAY_SPLASH_SCREEN=1,$(DEFINES)),)
OBJS += \
	$(LOCAL_DIR)/screenshot.o \
	$(LOCAL_DIR)/screenshot-neon.o
endif
