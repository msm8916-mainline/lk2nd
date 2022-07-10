# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

OBJS += \
	$(LOCAL_DIR)/device.o \
	$(LOCAL_DIR)/match.o \
	$(LOCAL_DIR)/parse-cmdline.o \
	$(LOCAL_DIR)/parse-tags.o \

ifneq ($(OUTBOOTIMG),)
# Appended DTBs
OUTBINDTB := $(OUTBIN)
ifneq ($(ADTBS),)
OUTBINDTB := $(OUTBIN)-dtb
$(OUTBINDTB): $(OUTBIN) $(ADTBS)
	@echo generating image with $(words $(ADTBS)) appended DTBs: $@
	$(NOECHO)cat $^ > $@
endif

# QCDT image
ifneq ($(LK2ND_QCDT),)
OUTQCDT := $(LK2ND_QCDT)
$(warning WARNING: Using pre-built QCDT image from $(LK2ND_QCDT))
else ifneq ($(QCDTBS),)
OUTQCDT := $(BUILDDIR)/qcdt.img
$(OUTQCDT): $(QCDTBS)
	@echo generating QCDT image with $(words $(QCDTBS)) DTBs: $@
	$(NOECHO)lk2nd/scripts/dtbTool -o $@ $(QCDTBS)
endif

# Android boot image
MKBOOTIMG_BASE ?= $(BASE_ADDR)

ifeq ($(ENABLE_UFS_SUPPORT), 1)
MKBOOTIMG_PAGESIZE ?= 4096
endif

$(OUTBOOTIMG): $(OUTBINDTB) $(OUTQCDT)
	@echo generating Android boot image: $@
	$(NOECHO)lk2nd/scripts/mkbootimg \
		--kernel=$< \
		--output=$@ \
		--cmdline="$(MKBOOTIMG_CMDLINE)" \
		$(if $(OUTQCDT),--qcdt=$(OUTQCDT)) \
		$(if $(MKBOOTIMG_BASE),--base=$(MKBOOTIMG_BASE)) \
		$(if $(MKBOOTIMG_PAGESIZE),--pagesize=$(MKBOOTIMG_PAGESIZE)) \
		$(MKBOOTIMG_ARGS)
	$(NOECHO)echo -n SEANDROIDENFORCE >> $@

APPSBOOTHEADER: $(OUTBOOTIMG)
endif
