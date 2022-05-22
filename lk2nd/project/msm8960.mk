# Assume EMMC_BOOT by default for lk2nd configurations
EMMC_BOOT ?= 1

# Initializing the display is currently broken on msm8960, but continuous splash
# works if an earlier bootloader has already initialized the display.
ENABLE_DISPLAY := 0
