LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/lk2nd-defines.mk
include $(LOCAL_DIR)/msm8226.mk
include $(LOCAL_DIR)/lk2nd-base.mk

APPSBOOTHEADER: $(OUTBOOTIMGADTB)
ANDROID_BOOT_BASE := 0x00000000
