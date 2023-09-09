# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/cpuid.o \
	$(LOCAL_DIR)/register.o \

# Currently the regulator fastboot debug code is only used for SPMI regulators
ifneq ($(filter dev/pmic/pm8x41, $(ALLMODULES)),)
ifneq ($(BUILD_GPL),)
MODULES += lk2nd/hw/regulator
OBJS += $(LOCAL_DIR)/regulator.o
endif
endif
