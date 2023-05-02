# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

ifneq ($(filter msm8916 msm8952, $(PLATFORM)),)
	OBJS += \
		$(LOCAL_DIR)/takeover.o \
		$(LOCAL_DIR)/remapper.o
endif
