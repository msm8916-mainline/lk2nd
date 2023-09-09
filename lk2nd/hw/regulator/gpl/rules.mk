# SPDX-License-Identifier: GPL-2.0-only
LOCAL_DIR := $(GET_LOCAL_DIR)

ifneq ($(filter dev/pmic/pm8x41, $(ALLMODULES)),)
OBJS += $(LOCAL_DIR)/qcom_spmi-regulator.o
$(BUILDDIR)/$(LOCAL_DIR)/qcom_spmi-regulator.o: \
	CFLAGS := $(CFLAGS) -Wno-missing-field-initializers
endif
