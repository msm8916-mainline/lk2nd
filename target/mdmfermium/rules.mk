LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared

PLATFORM := mdmfermium

MEMBASE := 0x82900000 # SDRAM
MEMSIZE := 0x00100000 # 1MB

SCRATCH_ADDR                        := 0x88000000
SCRATCH_REGION1                     := 0x88000000
SCRATCH_REGION1_SIZE                := 0x08000000 # 128MB
SCRATCH_REGION2                     := 0x0
SCRATCH_REGION2_SIZE                := 0x0

KERNEL_REGION                       := 0x80000000
KERNEL_REGION_SIZE                  := 0x2000000 # 20MB

BASE_ADDR                           := 0x80000000


MODULES += \
	dev/keys \
	dev/vib \
	lib/ptable \
	dev/pmic/pm8x41 \
	lib/libfdt

DEFINES += \
	MEMSIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR) \
	SCRATCH_REGION1=$(SCRATCH_REGION1) \
	SCRATCH_REGION1_SIZE=$(SCRATCH_REGION1_SIZE) \
	SCRATCH_REGION2_SIZE=$(SCRATCH_REGION2_SIZE) \
	KERNEL_REGION=$(KERNEL_REGION) \
	KERNEL_REGION_SIZE=$(KERNEL_REGION_SIZE)

OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/meminfo.o \
	$(LOCAL_DIR)/keypad.o
