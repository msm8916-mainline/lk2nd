LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/lk2nd-device.o \
	$(LOCAL_DIR)/lk2nd-motorola.o \
	$(LOCAL_DIR)/lk2nd-rproc.o \
	$(LOCAL_DIR)/lk2nd-smd-rpm.o

ifneq ($(GPIO_I2C_BUS_COUNT),)
MODULES += lk2nd/regmap
OBJS += $(LOCAL_DIR)/lk2nd-samsung.o
endif

ifeq ($(TARGET),msm8916)
MODULES += lk2nd/smb1360
endif

ifneq ($(SMP_SPIN_TABLE_BASE),)
MODULES += lk2nd/smp
DEFINES += SMP_SPIN_TABLE_BASE=$(SMP_SPIN_TABLE_BASE)
endif
