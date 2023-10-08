# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
	lib/bio \
	lib/fs \
	lk2nd/hw/bdev \

OBJS += \
	$(LOCAL_DIR)/boot.o \
	$(LOCAL_DIR)/extlinux.o \
	$(LOCAL_DIR)/util.o \
