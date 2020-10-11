LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/msm8916.mk
include $(LOCAL_DIR)/lk2nd.mk

# NOTE! Typically you don't need this when porting lk2nd to other platforms.
# Right now this is only used in workarounds for some MSM8916 Samsung devices.
# Remove this to get rid of some compile errors regarding gpio_i2c.
GPIO_I2C_BUS_COUNT := 1

APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
ANDROID_BOOT_BASE := 0x80000000
