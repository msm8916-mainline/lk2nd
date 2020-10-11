# top level project rules for the msm8916 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

OUTBOOTIMG := $(BUILDDIR)/lk2nd.img
OUTODINTAR := $(BUILDDIR)/lk2nd.tar

# Enable fastboot display menu
ENABLE_FBCON_DISPLAY_MSG := 1

include $(LOCAL_DIR)/msm8916.mk

# Use maximum verbosity
DEBUG := 2
DEFINES += LK_LOG_BUF_SIZE=16384

# Avoid writing device info
DEFINES += SAFE_MODE=1
# Display as unlocked by default
DEFINES += DEFAULT_UNLOCK=1

# Use continuous splash from primary bootloader for display
DISPLAY_USE_CONTINUOUS_SPLASH := 1

# NOTE! Typically you don't need this when porting lk2nd to other platforms.
# Right now this is only used in workarounds for some MSM8916 Samsung devices.
# Remove this to get rid of some compile errors regarding gpio_i2c.
GPIO_I2C_BUS_COUNT := 1

APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
ANDROID_BOOT_BASE := 0x80000000
ANDROID_BOOT_CMDLINE := lk2nd
