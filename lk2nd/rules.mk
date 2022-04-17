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

ifeq ($(PROJECT),lk2nd-msm8960)
# Filter out code that does not compile properly anymore because its too old.
# All of this is display setup which is not needed for lk2nd.
# Also compile the device tree code since the atags are ancient...
OBJS := $(filter-out \
	dev/pmic/pm8921/pm8921_pwm.o \
	platform/msm_shared/display.o \
	platform/msm_shared/hdmi.o \
	platform/msm_shared/lvds.o \
	platform/msm_shared/mdp_lcdc.o \
	platform/msm_shared/mdp4.o \
	platform/msm_shared/mipi_dsi_phy.o \
	platform/msm_shared/mipi_dsi.o \
	platform/msm8960/hdmi_core.o \
	target/msm8960/atags.o \
	, $(OBJS)) \
	platform/msm_shared/dev_tree.o \
	target/msm8916/meminfo.o
endif
