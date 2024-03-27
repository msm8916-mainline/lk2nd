# SPDX-License-Identifier: BSD-3-Clause
ifneq ($(SIGNED_KERNEL)$(VERIFIED_BOOT)$(VERIFIED_BOOT_2),)
$(error The lk2nd configurations provide development/debugging helpers and are
	therefore currently not designed to be used together with secure boot)
endif

override ENABLE_LPAE_SUPPORT := 0

# lk2nd provides its own mainline-friendly partial-goods implementation
ifneq ($(filter msm8909, $(TARGET)),)
override ENABLE_PARTIAL_GOODS_SUPPORT := 0
MODULES += lk2nd/partial-goods
endif

include project/$(TARGET).mk

DEBUG := 1

ABOOT_STANDALONE ?= 1
CRYPTO_SW_BACKEND ?= none
DTBO_BACKEND ?= none
GPL ?= 1

MODULES += \
	lk2nd \
	lk2nd/fastboot \
	lk2nd/fastboot/debug \
	lk2nd/hw/gpio \
	lk2nd/hw/i2c \
	lk2nd/ramoops \
	lk2nd/serialno \
	lk2nd/smp \
	lk2nd/smp/spin-table \
	lk2nd/version \

# Disable SMP spin table if unsupported (without throwing errors)
LK2ND_SMP_OPTIONAL := 1

ifeq ($(ENABLE_DISPLAY), 1)
ifneq ($(LK2ND_DISPLAY),)
MODULES += lk2nd/display
else
ENABLE_DISPLAY := 0
$(info NOTE: Display support is disabled without display/panel selection)
endif
endif

# Reserve 32 KiB for the log buffer so it can be accessed via fastboot
DEFINES += WITH_DEBUG_LOG_BUF=1 LK_LOG_BUF_SIZE=32768

ifeq ($(DEBUG_FBCON), 1)
	DEFINES += WITH_DEBUG_FBCON=1
endif

# Keep the kernel command line clean when booting other operating systems
DEFINES += GENERATE_CMDLINE_ONLY_FOR_ANDROID=1

# Disable reading splash partition to avoid crashes on some devices
DEFINES += DISABLE_SPLASH_PARTITION=1

# Enable "fastboot oem help" for a list of supported fastboot commands
DEFINES += FASTBOOT_HELP=1

# Allow entering fastboot after forced reset
DEFINES := $(filter-out USER_FORCE_RESET_SUPPORT=1, $(DEFINES))

# Allow flashing independent of battery voltage
DEFINES := $(filter-out CHECK_BAT_VOLTAGE=1, $(DEFINES))

include $(if $(filter msm8660 msm8960, $(TARGET)), lk2nd/project/msm8x60.mk)

# Enable extlinux boot module for all targets with eMMC/UFS
ifeq ($(EMMC_BOOT), 1)
MODULES += lk2nd/boot
endif
