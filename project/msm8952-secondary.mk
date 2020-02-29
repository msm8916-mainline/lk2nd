# top level project rules for the msm8952 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

OUTBOOTIMG := $(BUILDDIR)/lk2nd.img

# Enable fastboot display menu
ENABLE_FBCON_DISPLAY_MSG := 1

include $(LOCAL_DIR)/msm8952.mk

#  maximum verbosity
BOARD_NAME := msm8952-secondary
CFLAGS += -Wno-error
DEBUG := 2
DEFINES += LK_LOG_BUF_SIZE=16384

# Avoid writing device info
DEFINES += SAFE_MODE=1
# Display as unlocked by default
DEFINES += DEFAULT_UNLOCK=1

DEFINES += DISPLAY_SPLASH_SCREEN=1

# Use continuous splash from primary bootloader for display
DISPLAY_USE_CONTINUOUS_SPLASH := 1
TARGET_USES_APPENDED_DTBS := 1

APPSBOOTHEADER: $(OUTBOOTIMG)
ANDROID_BOOT_BASE := 0x80000000
ANDROID_BOOT_CMDLINE := lk2nd
