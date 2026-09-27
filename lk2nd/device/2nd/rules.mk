# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
	lib/libfdt \
	lk2nd/hw/gpio \
	lk2nd/hw/i2c \

OBJS += \
	$(LOCAL_DIR)/device.o \
	$(LOCAL_DIR)/match.o \
	$(LOCAL_DIR)/parse-cmdline.o \
	$(LOCAL_DIR)/parse-tags.o \
	$(LOCAL_DIR)/partition.o \
	$(LOCAL_DIR)/samsung-muic-reset.o \
	$(if $(filter %/mdss_spi.o, $(OBJS)), $(LOCAL_DIR)/spi-display.o) \

ifneq ($(LK2ND_PARTITION_SIZE),)
DEFINES += \
	LK2ND_PARTITION_BASE="$(LK2ND_PARTITION_BASE)" \
	LK2ND_PARTITION_NAME="$(LK2ND_PARTITION_NAME)" \
	LK2ND_PARTITION_SIZE=($(LK2ND_PARTITION_SIZE))
endif

include $(if $(BUILD_GPL),$(LOCAL_DIR)/gpl/rules.mk)

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

# Generate a 1-byte RAM disk containing a single zero
RAMDISK := $(BUILDDIR)/ramdisk.img
$(RAMDISK):
	$(NOECHO)printf '\0' > $(RAMDISK)

# Android boot image
MKBOOTIMG_BASE ?= $(BASE_ADDR)

ifeq ($(ENABLE_UFS_SUPPORT), 1)
MKBOOTIMG_PAGESIZE ?= 4096
endif

$(OUTBOOTIMG): $(OUTBINDTB) $(OUTQCDT) $(RAMDISK)
	@echo generating Android boot image: $@
	$(NOECHO)lk2nd/scripts/mkbootimg \
		--kernel=$< \
		--output=$@ \
		--cmdline="$(MKBOOTIMG_CMDLINE)" \
		$(if $(OUTQCDT),--qcdt=$(OUTQCDT)) \
		$(if $(MKBOOTIMG_BASE),--base=$(MKBOOTIMG_BASE)) \
		$(if $(MKBOOTIMG_PAGESIZE),--pagesize=$(MKBOOTIMG_PAGESIZE)) \
		--ramdisk=$(RAMDISK) \
		$(MKBOOTIMG_ARGS)

ifeq ($(SIGN_BOOTIMG), 1)
		$(NOECHO)lk2nd/scripts/bootsignature.py \
			-s \
			-t "/boot" \
			-c "$(BOOTIMG_CERT)" \
			-k "$(BOOTIMG_KEY)" \
			-i $@ \
			-o $@.signed
		$(NOECHO)mv $@.signed $@
endif
	$(NOECHO)echo -n SEANDROIDENFORCE >> $@

APPSBOOTHEADER: $(OUTBOOTIMG)
endif
