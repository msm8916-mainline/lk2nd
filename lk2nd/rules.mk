LOCAL_DIR := $(GET_LOCAL_DIR)

ifneq ($(SIGNED_KERNEL)$(VERIFIED_BOOT)$(VERIFIED_BOOT_2),)
$(error The lk2nd module provides development/debugging helpers and is therefore \
        currently not designed to be used together with secure boot. Let us know \
	if you are interested in this use case)
endif
# Fitler out more TZ-specific defines that might not work with some firmware
DEFINES := $(filter-out SSD_ENABLE TZ_SAVE_KERNEL_HASH TZ_TAMPER_FUSE, $(DEFINES))

ifneq ($(LK1ST_PANEL),)
# Filter out original panel implementation
OBJS := $(filter-out target/$(TARGET)/oem_panel.o, $(OBJS))
MODULES += lk2nd/panel
CFLAGS += -DLK1ST_PANEL=$(LK1ST_PANEL)
else
# Filter out original display implementation
OBJS := $(filter-out target/$(TARGET)/target_display.o target/$(TARGET)/oem_panel.o, $(OBJS))
ifeq ($(DISPLAY_USE_CONTINUOUS_SPLASH),1)
DEFINES += \
	$(if $(filter %/mdp3.o,$(OBJS)), MDP3=1) \
	$(if $(filter %/mdp4.o,$(OBJS)), MDP4=1) \
	$(if $(filter %/mdp5.o,$(OBJS)), MDP5=1)
OBJS += $(LOCAL_DIR)/target_display_cont_splash.o
endif
endif
