LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared
INCLUDES += -I$(LK_TOP_DIR)/dev/gcdb/display -I$(LK_TOP_DIR)/dev/gcdb/display/include

PLATFORM := msm8996

MEMBASE := 0x90B00000 # SDRAM
MEMSIZE := 0x00400000 # 4MB

BASE_ADDR    := 0x0000000

SCRATCH_ADDR := 0x91100000
SCRATCH_SIZE := 750
KERNEL_ADDR  := 0x80000000
KERNEL_SIZE  := 88
# LPAE supports only 32 virtual address, L1 pt size is 4
L1_PT_SZ     := 4
L2_PT_SZ     := 2


DEFINES += DISPLAY_SPLASH_SCREEN=1
DEFINES += DISPLAY_TYPE_MIPI=1
DEFINES += DISPLAY_TYPE_DSI6G=1

MODULES += \
	dev/keys \
	dev/pmic/pm8x41 \
	dev/qpnp_wled \
	dev/qpnp_led \
	dev/gcdb/display \
	lib/ptable \
	lib/libfdt

DEFINES += \
	MEMSIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	TAGS_ADDR=$(TAGS_ADDR) \
	KERNEL_ADDR=$(KERNEL_ADDR) \
	KERNEL_SIZE=$(KERNEL_SIZE) \
	RAMDISK_ADDR=$(RAMDISK_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR) \
	SCRATCH_SIZE=$(SCRATCH_SIZE) \
	L1_PT_SZ=$(L1_PT_SZ) \
	L2_PT_SZ=$(L2_PT_SZ)


OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/meminfo.o \
	$(LOCAL_DIR)/target_display.o \
	$(LOCAL_DIR)/oem_panel.o \

ifeq ($(ENABLE_GLINK_SUPPORT),1)
OBJS += \
    $(LOCAL_DIR)/regulator.o
endif
