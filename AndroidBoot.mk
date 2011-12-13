#Android makefile to build lk bootloader as a part of Android Build

ifeq ($(call is-android-codename,HONEYCOMB),true)
CROSS_TOOL := ../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
else ifeq ($(call is-android-codename,ICECREAM_SANDWICH),true)
CROSS_TOOL := ../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
else
CROSS_TOOL := ../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-
endif

# Set flags if we need to include security libs
ifeq ($(TARGET_BOOTIMG_SIGNED),true)
  SIGNED_KERNEL := SIGNED_KERNEL=1
else
  SIGNED_KERNEL := SIGNED_KERNEL=0
endif

# NAND variant output
TARGET_NAND_BOOTLOADER := $(PRODUCT_OUT)/appsboot.mbn
NAND_BOOTLOADER_OUT := $(TARGET_OUT_INTERMEDIATES)/NAND_BOOTLOADER_OBJ

# Remove bootloader binary to trigger recompile when source changes
appsbootldr_clean:
	$(hide) rm -f $(TARGET_NAND_BOOTLOADER)

$(NAND_BOOTLOADER_OUT):
	mkdir -p $(NAND_BOOTLOADER_OUT)

# eMMC variant output
TARGET_EMMC_BOOTLOADER := $(PRODUCT_OUT)/emmc_appsboot.mbn
EMMC_BOOTLOADER_OUT := $(TARGET_OUT_INTERMEDIATES)/EMMC_BOOTLOADER_OBJ

emmc_appsbootldr_clean:
	$(hide) rm -f $(TARGET_EMMC_BOOTLOADER)

$(EMMC_BOOTLOADER_OUT):
	mkdir -p $(EMMC_BOOTLOADER_OUT)

# Top level for NAND variant targets
$(TARGET_NAND_BOOTLOADER): appsbootldr_clean | $(NAND_BOOTLOADER_OUT)
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(NAND_BOOTLOADER_OUT) $(TARGET_PRODUCT) $(SIGNED_KERNEL)

# Top level for eMMC variant targets
$(TARGET_EMMC_BOOTLOADER): emmc_appsbootldr_clean | $(EMMC_BOOTLOADER_OUT)
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(EMMC_BOOTLOADER_OUT) $(TARGET_PRODUCT) EMMC_BOOT=1 $(SIGNED_KERNEL)

# Keep build NAND & eMMC as default for targets still using TARGET_BOOTLOADER
TARGET_BOOTLOADER := $(PRODUCT_OUT)/EMMCBOOT.MBN
$(TARGET_BOOTLOADER): $(NAND_BOOTLOADER_OUT) $(EMMC_BOOTLOADER_OUT) | $(TARGET_NAND_BOOTLOADER) $(TARGET_EMMC_BOOTLOADER)

#
# Build nandwrite as a part of Android Build for NAND configurations
#
TARGET_NANDWRITE := $(PRODUCT_OUT)/obj/nandwrite/build-$(TARGET_PRODUCT)_nandwrite/lk
NANDWRITE_OUT := $(TARGET_OUT_INTERMEDIATES)/nandwrite

nandwrite_clean:
	$(hide) rm -f $(TARGET_NANDWRITE)
	$(hide) rm -rf $(NANDWRITE_OUT)

$(NANDWRITE_OUT):
	mkdir -p $(NANDWRITE_OUT)

$(TARGET_NANDWRITE): nandwrite_clean | $(NANDWRITE_OUT)
	@echo $(TARGET_PRODUCT)_nandwrite
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(NANDWRITE_OUT) $(TARGET_PRODUCT)_nandwrite BUILD_NANDWRITE=1
