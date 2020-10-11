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
	$(LOCAL_DIR)/recovery.o

ifneq ($(GPIO_I2C_BUS_COUNT),)
MODULES += dev/gpio_i2c
OBJS += $(LOCAL_DIR)/lk2nd-samsung.o
endif

ifeq ($(TARGET), msm8916)
OBJS += $(LOCAL_DIR)/fastboot-lk2nd-msm8916.o
endif
