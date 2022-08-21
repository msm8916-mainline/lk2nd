# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += lk2nd/hw/gpio
OBJS += \
	$(LOCAL_DIR)/gpio_i2c.o \
