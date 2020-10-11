LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/target_display.o

ifeq ($(DISPLAY_USE_CONTINUOUS_SPLASH),1)
# Filter out original display implementation
OBJS := $(filter-out target/$(TARGET)/target_display.o target/$(TARGET)/oem_panel.o, $(OBJS))
ifneq ($(filter $(DEFINES),DISPLAY_TYPE_MDSS=1),)
    OBJS += $(LOCAL_DIR)/target_display_cont_splash_mdp5.o
else
    $(error Continuous splash display is not supported for the current target)
endif
endif
