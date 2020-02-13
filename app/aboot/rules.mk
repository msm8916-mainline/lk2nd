LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/lib/zlib_inflate

DEFINES += ASSERT_ON_TAMPER=1

MODULES += lib/zlib_inflate

OBJS += \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/fastboot-lk2nd.o \
	$(LOCAL_DIR)/lk2nd-device.o \
	$(LOCAL_DIR)/lk2nd-motorola.o \
	$(LOCAL_DIR)/lk2nd-samsung.o \
	$(LOCAL_DIR)/recovery.o

ifeq ($(ENABLE_UNITTEST_FW), 1)
OBJS += \
	$(LOCAL_DIR)/fastboot_test.o
	ifeq ($(ENABLE_MDTP_SUPPORT),1)
		OBJS += \
			$(LOCAL_DIR)/mdtp_lk_ut.o
	endif
endif

ifeq ($(ENABLE_MDTP_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/mdtp.o \
	$(LOCAL_DIR)/mdtp_ui.o \
	$(LOCAL_DIR)/mdtp_fuse.o \
	$(LOCAL_DIR)/mdtp_defs.o \
	$(LOCAL_DIR)/mdtp_fs.o
endif

# lk2nd-samsung.c
GPIO_I2C_BUS_COUNT := 1
MODULES += dev/gpio_i2c
