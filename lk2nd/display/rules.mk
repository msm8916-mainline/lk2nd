# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

# Filter out original panel implementation
OBJS := $(filter-out target/$(TARGET)/oem_panel.o, $(OBJS))

ifeq ($(LK2ND_DISPLAY), cont-splash)
include $(LOCAL_DIR)/cont-splash/rules.mk
else
$(error Display '$(LK2ND_DISPLAY)' is not supported yet)
endif
