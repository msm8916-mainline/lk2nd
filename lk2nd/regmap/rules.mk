LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += dev/gpio_i2c
OBJS += \
	$(LOCAL_DIR)/regmap.o \
	$(LOCAL_DIR)/regmap-gpio-i2c.o
