LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH    := arm
ARM_CPU := cortex-a8
CPU     := generic

DEFINES += ARM_CPU_CORE_KRAIT

MMC_SLOT         := 1

DEFINES += WITH_CPU_EARLY_INIT=0 WITH_CPU_WARM_BOOT=0 \
	   MMC_SLOT=$(MMC_SLOT) MDP4=1 SSD_ENABLE

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include

DEVS += fbcon
MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/acpuclock.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/clock.o \
	$(LOCAL_DIR)/hdmi_core.o

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

ifeq ($(ENABLE_DISPLAY), 0)
MODULES := $(filter-out dev/panel/msm, $(MODULES))
OBJS := $(filter-out \
	platform/msm_shared/display.o \
	platform/msm_shared/hdmi.o \
	platform/msm_shared/lvds.o \
	platform/msm_shared/mdp_lcdc.o \
	platform/msm_shared/mdp4.o \
	platform/msm_shared/mipi_dsi_phy.o \
	platform/msm_shared/mipi_dsi.o \
	platform/msm8960/hdmi_core.o \
	target/msm8960/target_display.o \
	, $(OBJS))
else
$(error Display support in msm8960 is currently broken)
endif
