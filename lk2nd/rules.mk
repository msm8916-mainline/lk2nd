LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/lk2nd-device.o \
	$(LOCAL_DIR)/lk2nd-motorola.o

ifneq ($(GPIO_I2C_BUS_COUNT),)
MODULES += dev/gpio_i2c
OBJS += $(LOCAL_DIR)/lk2nd-samsung.o
endif

ifeq ($(TARGET),msm8916)
OBJS += \
	$(LOCAL_DIR)/smb1360.o \
	$(LOCAL_DIR)/smb1360-wt88047.o
endif
