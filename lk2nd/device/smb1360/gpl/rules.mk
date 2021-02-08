# SPDX-License-Identifier: GPL-2.0-only
LOCAL_DIR := $(GET_LOCAL_DIR)

# For battery-wt88047
MODULES += lk2nd/hw/gpio

OBJS += \
	$(LOCAL_DIR)/battery-wt88047.o \
