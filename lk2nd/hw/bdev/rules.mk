# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
	   lib/bio \
	   lib/partition \

OBJS += \
	$(LOCAL_DIR)/bdev.o \
	$(LOCAL_DIR)/util.o \
	$(LOCAL_DIR)/wrapper.o \
