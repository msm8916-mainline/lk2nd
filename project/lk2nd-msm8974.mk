LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/lk2nd-defines.mk
include $(LOCAL_DIR)/msm8974.mk
include $(LOCAL_DIR)/lk2nd-base.mk

DEFINES += WITH_DEBUG_LOG_BUF=1

APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
ANDROID_BOOT_BASE := 0x00000000

# Memory usually reserved for RMTFS, should be fine for early SMP bring-up
SMP_SPIN_TABLE_BASE := 0x0fd80000
