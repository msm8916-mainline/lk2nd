# SPDX-License-Identifier: BSD-3-Clause
LK2ND_PROJECT := lk2nd
LK2ND_DISPLAY ?= cont-splash
include lk2nd/project/base.mk

MODULES += \
	lk2nd/device \
	lk2nd/device/2nd \

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
