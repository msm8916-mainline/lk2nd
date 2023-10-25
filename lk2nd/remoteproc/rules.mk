# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

ifneq ($(BUILD_GPL),)
OBJS += \
	$(LOCAL_DIR)/remoteproc.o
endif

