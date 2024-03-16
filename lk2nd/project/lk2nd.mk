# SPDX-License-Identifier: BSD-3-Clause
LK2ND_PROJECT := lk2nd
LK2ND_DISPLAY ?= cont-splash
include lk2nd/project/base.mk

MODULES += \
	lk2nd/device \
	lk2nd/device/2nd \
	lk2nd/hyp \

ifneq ($(ENABLE_FBCON_DISPLAY_MSG),1)
MODULES += $(if $(filter $(MODULES), lk2nd/display), lk2nd/device/menu)
endif

# Use part of the "boot" partition for the lk2nd boot image. The real Android
# boot image can be placed in the partition with 512 KiB offset.
LK2ND_PARTITION_BASE ?= boot
LK2ND_PARTITION_NAME ?= lk2nd
LK2ND_PARTITION_SIZE ?= 512*1024

# 4 KiB qhypstub partition will be placed at the end of lk2nd virtual partition.
QHYPSTUB_PARTITION_BASE ?= lk2nd
QHYPSTUB_PARTITION_NAME ?= qhypstub
QHYPSTUB_PARTITION_SIZE ?= 4*1024

# The primary bootloader will implement LONG_PRESS_POWER_ON if needed.
# If we do it again in lk2nd we might accidentally shutdown the device because
# the user needs to keep the power key pressed for *really* long.
DEFINES := $(filter-out LONG_PRESS_POWER_ON=1, $(DEFINES))

# Since lk2nd is typically used through lk.bin, having separate code/data
# segments with a fixed 1 MiB offset increases the binary size significantly,
# since a lot of padding has to be added inbetween. Disable it for now....
DEFINES := $(filter-out SECURE_CODE_MEM=1, $(DEFINES))

# Weak battery charging is handled by the primary bootloader
DEFINES := $(filter-out ENABLE_WBC=1, $(DEFINES))

# Should be already done by primary bootloader if wanted
DEFINES := $(filter-out ENABLE_XPU_VIOLATION=1, $(DEFINES))

# Build Android boot image
OUTBOOTIMG := $(BUILDDIR)/lk2nd.img
MKBOOTIMG_CMDLINE := lk2nd
