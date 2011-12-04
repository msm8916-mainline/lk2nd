LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH    := arm
ARM_CPU := cortex-a8
CPU     := generic

DEFINES += ARM_CPU_CORE_SCORPION

MMC_SLOT         := 1

DEFINES += WITH_CPU_EARLY_INIT=0 WITH_CPU_WARM_BOOT=0 \
	   MMC_SLOT=$(MMC_SLOT) MDP4=1 \
	   SSD_ENABLE TZ_TAMPER_FUSE


DEFINES += QT_8660_KEYPAD_HW_BUG=1

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include

DEVS += fbcon
MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/acpuclock.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/panel.o \
	$(LOCAL_DIR)/panel_auo_wvga.o \
	$(LOCAL_DIR)/pmic.o \
	$(LOCAL_DIR)/pmic_pwm.o \
	$(LOCAL_DIR)/pmic_batt_alarm.o \
	$(LOCAL_DIR)/scm-io.o \
	$(LOCAL_DIR)/hdmi_core.o

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

