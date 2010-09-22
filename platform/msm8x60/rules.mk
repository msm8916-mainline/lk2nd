LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH := arm
ARM_CPU := cortex-a8
#arm1136j-s
CPU := generic

MMC_SLOT         := 1

DEFINES += WITH_CPU_EARLY_INIT=0 WITH_CPU_WARM_BOOT=0 \
	   MMC_SLOT=$(MMC_SLOT)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include

DEVS += fbcon
MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/interrupts.o \
	$(LOCAL_DIR)/acpuclock.o \
	$(LOCAL_DIR)/mmc_init.o \
	$(LOCAL_DIR)/gpio.o

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

