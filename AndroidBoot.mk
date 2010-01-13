#Android makefile to build lk bootloader as a part of Android Build

TARGET_BOOTLOADER := $(PRODUCT_OUT)/appsboot.mbn
BOOTLOADER_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_OBJ

# Remove bootloader binary to trigger recompile when source changes
appsbootldr_clean:
	$(hide) rm -f $(TARGET_BOOTLOADER)
	$(hide) rm -rf $(BOOTLOADER_OUT)

$(BOOTLOADER_OUT):
	mkdir -p $(BOOTLOADER_OUT)


ifeq ($(TARGET_USERIMAGES_USE_EXT2),true)
TARGET_BOOTLOADER_EMMC := $(PRODUCT_OUT)/emmc_appsboot.mbn
BOOTLOADER_EMMC_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_EMMC_OBJ

emmc_appsbootldr_clean:
	$(hide) rm -f $(TARGET_BOOTLOADER_EMMC)
	$(hide) rm -rf $(BOOTLOADER_EMMC_OUT)

$(BOOTLOADER_EMMC_OUT):
	mkdir -p $(BOOTLOADER_EMMC_OUT)

$(TARGET_BOOTLOADER): appsbootldr_clean emmc_appsbootldr_clean $(BOOTLOADER_OUT) $(BOOTLOADER_EMMC_OUT)
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT)
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(BOOTLOADER_EMMC_OUT) $(TARGET_PRODUCT) EMMC_BOOT=1

else

$(TARGET_BOOTLOADER): appsbootldr_clean $(BOOTLOADER_OUT)
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT)

endif

#build nandwrite as a part of Android Build
TARGET_NANDWRITE := $(PRODUCT_OUT)/obj/nandwrite/build-$(TARGET_PRODUCT)_nandwrite/lk
NANDWRITE_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/nandwrite

nandwrite_clean:
	$(hide) rm -f $(TARGET_NANDWRITE)
	$(hide) rm -rf $(NANDWRITE_OUT)

$(NANDWRITE_OUT):
	mkdir -p $(NANDWRITE_OUT)

$(TARGET_NANDWRITE): nandwrite_clean $(NANDWRITE_OUT)
	@echo $(TARGET_PRODUCT)_nandwrite
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(NANDWRITE_OUT) $(TARGET_PRODUCT)_nandwrite BUILD_NANDWRITE=1

