LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH    := arm
# Cann't use cortex-a5 as its not supported by gcc 4.4.0
ARM_CPU := cortex-a8
CPU     := generic

DEFINES += ARM_CPU_CORE_A5

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/acpuclock.o \

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk

