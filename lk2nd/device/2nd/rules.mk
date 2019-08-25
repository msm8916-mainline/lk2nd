# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

OBJS += \
	$(LOCAL_DIR)/device.o \
	$(LOCAL_DIR)/match.o \
	$(LOCAL_DIR)/parse-cmdline.o \
	$(LOCAL_DIR)/parse-tags.o \
