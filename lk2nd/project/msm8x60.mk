# SPDX-License-Identifier: BSD-3-Clause

# Assume EMMC_BOOT by default for lk2nd configurations
EMMC_BOOT ?= 1

# Strictly speaking msm8660 uses custom PMIC code inside platform/msm8x60,
# but pm8921 seems "compatible enough" for purposes in lk2nd.
MODULES += dev/pmic/pm8921

ifeq ($(ENABLE_DISPLAY),1)
# Initializing the display is currently broken on msm8960, but continuous splash
# works if an earlier bootloader has already initialized the display.
ENABLE_DISPLAY := 0

# HACK: Use 42 to ensure DISPLAY_SPLASH_SCREEN=1 is not filtered-out
DEFINES += DISPLAY_SPLASH_SCREEN=42
endif
