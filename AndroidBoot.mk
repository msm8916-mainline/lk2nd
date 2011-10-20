#Android makefile to build lk bootloader as a part of Android Build

TARGET_BOOTLOADER := $(PRODUCT_OUT)/appsboot.mbn
BOOTLOADER_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_OBJ

ifeq ($(call is-android-codename,HONEYCOMB),true)
CROSS_TOOL := ../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
else ifeq ($(call is-android-codename,ICECREAM_SANDWICH),true)
CROSS_TOOL := ../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
else
CROSS_TOOL := ../../../prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-
endif

# Remove bootloader binary to trigger recompile when source changes
appsbootldr_clean:
	$(hide) rm -f $(TARGET_BOOTLOADER)
	$(hide) rm -rf $(BOOTLOADER_OUT)

$(BOOTLOADER_OUT):
	mkdir -p $(BOOTLOADER_OUT)


TARGET_EMMC := 0
ifeq ($(TARGET_USERIMAGES_USE_EXT2),true)
  TARGET_EMMC := 1
endif
ifeq ($(TARGET_USERIMAGES_USE_EXT3),true)
  TARGET_EMMC := 1
endif
ifeq ($(TARGET_USERIMAGES_USE_EXT4),true)
  TARGET_EMMC := 1
endif

ifeq ($(TARGET_BOOTIMG_SIGNED),true)
  SIGNED_KERNEL := SIGNED_KERNEL=1
else
  SIGNED_KERNEL := SIGNED_KERNEL=0
endif

ifeq ($(TARGET_EMMC),1)
TARGET_BOOTLOADER_EMMC := $(PRODUCT_OUT)/EMMCBOOT.MBN
BOOTLOADER_EMMC_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_EMMC_OBJ

emmc_appsbootldr_clean:
	$(hide) rm -f $(TARGET_BOOTLOADER_EMMC)
	$(hide) rm -rf $(BOOTLOADER_EMMC_OUT)

$(BOOTLOADER_EMMC_OUT):
	mkdir -p $(BOOTLOADER_EMMC_OUT)

$(TARGET_BOOTLOADER): appsbootldr_clean emmc_appsbootldr_clean | $(BOOTLOADER_OUT) $(BOOTLOADER_EMMC_OUT)
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT) $(SIGNED_KERNEL)
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(BOOTLOADER_EMMC_OUT) $(TARGET_PRODUCT) EMMC_BOOT=1 $(SIGNED_KERNEL)

else

$(TARGET_BOOTLOADER): appsbootldr_clean | $(BOOTLOADER_OUT)
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT) $(SIGNED_KERNEL)

endif

#build nandwrite as a part of Android Build
TARGET_NANDWRITE := $(PRODUCT_OUT)/obj/nandwrite/build-$(TARGET_PRODUCT)_nandwrite/lk
NANDWRITE_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/nandwrite

nandwrite_clean:
	$(hide) rm -f $(TARGET_NANDWRITE)
	$(hide) rm -rf $(NANDWRITE_OUT)

$(NANDWRITE_OUT):
	mkdir -p $(NANDWRITE_OUT)

$(TARGET_NANDWRITE): nandwrite_clean | $(NANDWRITE_OUT)
	@echo $(TARGET_PRODUCT)_nandwrite
	$(MAKE) -C bootable/bootloader/lk TOOLCHAIN_PREFIX=$(CROSS_TOOL) BOOTLOADER_OUT=../../../$(NANDWRITE_OUT) $(TARGET_PRODUCT)_nandwrite BUILD_NANDWRITE=1

