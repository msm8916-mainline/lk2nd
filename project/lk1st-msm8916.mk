LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/msm8916.mk

ifneq ($(LK1ST_PANEL),)
    DEFINES += LK1ST_PANEL=$(LK1ST_PANEL)

    # Enable fastboot display menu
    ENABLE_FBCON_DISPLAY_MSG := 1
else
    $(info NOTE: Display support is disabled without LK1ST_PANEL)
    DEFINES := $(filter-out DISPLAY_SPLASH_SCREEN=1, $(DEFINES))
endif

include $(LOCAL_DIR)/lk2nd-1st-common.mk
