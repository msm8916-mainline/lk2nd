# SPDX-License-Identifier: BSD-3-Clause
ifneq ($(SIGNED_KERNEL)$(VERIFIED_BOOT)$(VERIFIED_BOOT_2),)
$(error The lk2nd configurations provide development/debugging helpers and are
	therefore currently not designed to be used together with secure boot)
endif

override ENABLE_LPAE_SUPPORT := 0
include project/$(TARGET).mk

DEBUG := 1

ABOOT_STANDALONE ?= 1
CRYPTO_BACKEND ?= none
DTBO_BACKEND ?= none
FASTBOOT_HELP ?= 1
GPL ?= 1

MODULES += \
	lk2nd \
	lk2nd/fastboot \
	lk2nd/mac \

ifeq ($(ENABLE_DISPLAY), 1)
ifneq ($(LK2ND_DISPLAY),)
MODULES += lk2nd/display
ENABLE_FBCON_DISPLAY_MSG := 1
else
ENABLE_DISPLAY := 0
$(info NOTE: Display support is disabled without display/panel selection)
endif
endif

# Reserve 32 KiB for the log buffer so it can be accessed via fastboot
DEFINES += WITH_DEBUG_LOG_BUF=1 LK_LOG_BUF_SIZE=32768

# Allow entering fastboot after forced reset
DEFINES := $(filter-out USER_FORCE_RESET_SUPPORT=1, $(DEFINES))

# Allow flashing independent of battery voltage
DEFINES := $(filter-out CHECK_BAT_VOLTAGE=1, $(DEFINES))

# Keep the kernel command line clean when booting other operating systems
DEFINES += GENERATE_CMDLINE_ONLY_FOR_ANDROID=1

-include lk2nd/project/$(TARGET).mk
