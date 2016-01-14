LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared
INCLUDES += -I$(LK_TOP_DIR)/dev/gcdb/display -I$(LK_TOP_DIR)/dev/gcdb/display/include

PLATFORM := msmtitanium

MEMBASE := 0x8F600000 # SDRAM
MEMSIZE := 0x00400000 # 1MB

BASE_ADDR        := 0x80000000
SCRATCH_ADDR     := 0xA0000000

DEFINES += PMI_CONFIGURED=1
DEFINES += DISPLAY_TYPE_MIPI=1
DEFINES += DISPLAY_TYPE_DSI6G=1

MODULES += \
	dev/keys \
	dev/vib \
	lib/ptable \
	dev/pmic/pm8x41 \
	dev/qpnp_haptic \
	lib/libfdt

DEFINES += \
	MEMSIZE=$(MEMSIZE) \
	MEMBASE=$(MEMBASE) \
	BASE_ADDR=$(BASE_ADDR) \
	SCRATCH_ADDR=$(SCRATCH_ADDR)


OBJS += \
	$(LOCAL_DIR)/init.o \
	$(LOCAL_DIR)/meminfo.o
