LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH     := arm
ARM_CPU  := cortex-a8
CPU      := generic

DEFINES  += ARM_CPU_CORE_KRAIT

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include

DEVS += fbcon
MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/clock.o \
	$(LOCAL_DIR)/gpio.o \

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

