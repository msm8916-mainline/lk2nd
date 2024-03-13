LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/msm8909.mk
include $(LOCAL_DIR)/lk2nd-base.mk

# Memory usually reserved for RMTFS, should be fine for early SMP bring-up
SMP_SPIN_TABLE_BASE := 0x87c00000
