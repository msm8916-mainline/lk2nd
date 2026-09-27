LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared

PLATFORM := fsm9010

MEMBASE := 0x18a00000 # SDRAM
MEMSIZE := 0x00100000 # 1MB

SCRATCH_ADDR     := 0x32200000

MODULES += \
	dev/keys \
	lib/ptable \
	lib/libfdt \
	dev/pmic/pm8x41

DEFINES += \
	MEMSIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	SCRATCH_ADDR=$(SCRATCH_ADDR)


OBJS += \
    $(LOCAL_DIR)/init.o \
    $(LOCAL_DIR)/meminfo.o
