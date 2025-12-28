# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include
DT_INCLUDES := -I$(LOCAL_DIR) -I$(LOCAL_DIR)/include -I$(BUILDDIR)

-include $(LOCAL_DIR)/$(TARGET)/rules.mk

ALLDTBS := $(DTBS) $(ADTBS) $(QCDTBS)

# addprefix only if the path is relative and not absolute (starts with /)
addrelprefix = $(if $(2:/%=),$(addprefix $(1),$(2)),$(2))

# DTB filter
ifneq ($(flavor LK2ND_DTBS),undefined)
LK2ND_DTBS_FILTER := $(addprefix $(LOCAL_DIR)/,$(LK2ND_DTBS))
ADTBS := $(filter $(LK2ND_DTBS_FILTER),$(ADTBS))
QCDTBS := $(filter $(LK2ND_DTBS_FILTER),$(QCDTBS))
endif
ifneq ($(flavor LK2ND_ADTBS),undefined)
ADTBS := $(call addrelprefix,$(LOCAL_DIR)/,$(LK2ND_ADTBS))
endif
ifneq ($(flavor LK2ND_QCDTBS),undefined)
QCDTBS := $(call addrelprefix,$(LOCAL_DIR)/,$(LK2ND_QCDTBS))
endif

# Bundle DTB
ifneq ($(LK2ND_BUNDLE_DTB),)
LK2ND_BUNDLE_DTB_PATH := $(call addrelprefix,$(BUILDDIR)/$(LOCAL_DIR)/,$(LK2ND_BUNDLE_DTB))
DEFINES += LK2ND_BUNDLE_DTB="$(LK2ND_BUNDLE_DTB_PATH)"
$(BUILDDIR)/$(LK2ND_DEVICE_OBJ): $(LK2ND_BUNDLE_DTB_PATH)
endif

DTBS := $(call addrelprefix,$(BUILDDIR)/,$(DTBS))
ADTBS := $(call addrelprefix,$(BUILDDIR)/,$(ADTBS))
QCDTBS := $(call addrelprefix,$(BUILDDIR)/,$(QCDTBS))
