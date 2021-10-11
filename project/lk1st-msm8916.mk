LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/msm8916.mk

# FIXME: Disable splash screen / display for now
DEFINES := $(filter-out DISPLAY_SPLASH_SCREEN=1, $(DEFINES))

include $(LOCAL_DIR)/lk2nd-1st-common.mk
