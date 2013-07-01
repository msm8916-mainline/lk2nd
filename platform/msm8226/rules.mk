LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH    := arm
#Compiling this as cortex-a8 until the compiler supports cortex-a7
ARM_CPU := cortex-a8
CPU     := generic

DEFINES += ARM_CPU_CORE_A7

MMC_SLOT         := 1

DEFINES += PERIPH_BLK_BLSP=1
DEFINES += WITH_CPU_EARLY_INIT=0 WITH_CPU_WARM_BOOT=0 \
	   MMC_SLOT=$(MMC_SLOT) SSD_ENABLE

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include

DEVS += fbcon
MODULES += dev/fbcon

OBJS += \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/acpuclock.o \
	$(LOCAL_DIR)/msm8226-clock.o \
	$(LOCAL_DIR)/gpio.o

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

include platform/msm_shared/rules.mk
