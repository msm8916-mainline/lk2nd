LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH := arm
ARM_CPU := arm1136j-s
CPU := generic

MMC_SLOT := 1
DEFINES += MMC_SLOT=$(MMC_SLOT)

INCLUDES += -I$(LOCAL_DIR)/include

MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/interrupts.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/panel.o \
	$(LOCAL_DIR)/acpuclock.o

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

