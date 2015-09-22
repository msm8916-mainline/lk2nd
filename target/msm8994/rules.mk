LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared
INCLUDES += -I$(LK_TOP_DIR)/dev/gcdb/display -I$(LK_TOP_DIR)/dev/gcdb/display/include
ifeq ($(ENABLE_MDTP_SUPPORT),1)
INCLUDES += -I$(LK_TOP_DIR)/app/aboot
endif

PLATFORM := msm8994

MEMBASE := 0x0F900000 # SDRAM
MEMSIZE := 0x00200000 # 2MB

BASE_ADDR    := 0x0000000

SCRATCH_ADDR := 0x10100000

SCRATCH_SIZE := 0x20000000

DEFINES += DISPLAY_SPLASH_SCREEN=1
DEFINES += DISPLAY_TYPE_MIPI=1
DEFINES += DISPLAY_TYPE_DSI6G=1

DEFINES += DISPLAY_EN_20NM_PLL_90_PHASE

MODULES += \
	dev/keys \
	dev/pmic/pm8x41 \
	dev/qpnp_wled \
	dev/qpnp_led \
    lib/ptable \
	dev/gcdb/display \
	dev/pmic/pmi8994 \
    lib/libfdt

DEFINES += \
	MEMSIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	TAGS_ADDR=$(TAGS_ADDR) \
	KERNEL_ADDR=$(KERNEL_ADDR) \
	RAMDISK_ADDR=$(RAMDISK_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR) \
	SCRATCH_SIZE=$(SCRATCH_SIZE)


OBJS += \
    $(LOCAL_DIR)/init.o \
    $(LOCAL_DIR)/meminfo.o \
    $(LOCAL_DIR)/target_display.o \
    $(LOCAL_DIR)/oem_panel.o

ifeq ($(ENABLE_SMD_SUPPORT),1)
OBJS += \
    $(LOCAL_DIR)/regulator.o
endif

ifeq ($(ENABLE_MDTP_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/mdtp_defs.o
endif
