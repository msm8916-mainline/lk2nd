LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := msm8996
DISPLAY_USE_CONTINUOUS_SPLASH := 1
include $(LOCAL_DIR)/lk2nd-base.mk

ENABLE_LPAE_SUPPORT := 0
DEFINES := $(filter-out LPAE=1, $(DEFINES))
MODULES := $(filter-out app/mmutest, $(MODULES))
