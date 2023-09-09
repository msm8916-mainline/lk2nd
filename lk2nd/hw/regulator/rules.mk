# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
include $(if $(BUILD_GPL),$(LOCAL_DIR)/gpl/rules.mk)
