# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/simplefb.o

ifeq ($(LK2ND_DISPLAY), cont-splash)
include $(LOCAL_DIR)/cont-splash/rules.mk
DEFINES += LK2ND_DISPLAY_CONT_SPLASH=1
else ifneq ($(LK2ND_DISPLAY),)
DEFINES += LK2ND_DISPLAY=$(LK2ND_DISPLAY)
include $(LOCAL_DIR)/panel/rules.mk
else
$(error Please specify the display with LK2ND_DISPLAY option)
endif
