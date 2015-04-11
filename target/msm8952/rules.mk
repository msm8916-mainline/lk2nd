LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared

PLATFORM := msm8952

MEMBASE := 0x8F600000 # SDRAM
MEMSIZE := 0x00100000 # 1MB

BASE_ADDR        := 0x80000000
SCRATCH_ADDR     := 0x90000000

MODULES += \
	dev/keys \
	lib/ptable \
	dev/pmic/pm8x41 \
	lib/libfdt \
	dev/qpnp_wled

DEFINES += \
	MEMSIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR)


OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/meminfo.o
ifeq ($(ENABLE_SMD_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/regulator.o
endif
