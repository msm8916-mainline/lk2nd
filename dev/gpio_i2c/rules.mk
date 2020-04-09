LOCAL_DIR := $(GET_LOCAL_DIR)

ifeq ($(PROJECT), msm8916-secondary)

OBJS += \
	$(LOCAL_DIR)/gpio_i2c.o

DEFINES += \
	GPIO_I2C_BUS_COUNT=$(GPIO_I2C_BUS_COUNT)

endif
