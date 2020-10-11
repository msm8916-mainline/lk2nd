LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/msm8226.mk
include $(LOCAL_DIR)/lk2nd.mk

# TODO: Configure Android boot image after adding some device trees
#APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
#ANDROID_BOOT_BASE := 0x80000000
