# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

OBJS += \
	$(LOCAL_DIR)/gpio.o \
	$(LOCAL_DIR)/tlmm.o \
	$(if $(filter dev/pmic/pm8x41, $(ALLMODULES)), $(LOCAL_DIR)/pm8x41.o) \
