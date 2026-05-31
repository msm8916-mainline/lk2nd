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

# Default boot memory size limit (50 MiB)
LK2ND_BOOT_MEM_SIZE ?= 0x03200000

DEFINES += LK2ND_BOOT_MEM_SIZE=$(LK2ND_BOOT_MEM_SIZE)
