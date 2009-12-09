#Android makefile to build lk bootloader as a part of Android Build

TARGET_BOOTLOADER := out/target/product/$(TARGET_PRODUCT)/appsboot.mbn
BOOTLOADER_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_OBJ

# Remove bootloader binary to trigger recompile when source changes
appsbootldr_clean:
	$(hide) rm -f $(TARGET_BOOTLOADER)
	$(hide) rm -rf $(BOOTLOADER_OUT)

$(BOOTLOADER_OUT):
	mkdir -p $(BOOTLOADER_OUT)

$(TARGET_BOOTLOADER): $(BOOTLOADER_OUT) appsbootldr_clean
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT)


#build nandwrite as a part of Android Build
TARGET_NANDWRITE := $(PRODUCT_OUT)/obj/nandwrite/build-$(TARGET_PRODUCT)_nandwrite/lk
NANDWRITE_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/nandwrite

$(NANDWRITE_OUT):
	mkdir -p $(BOOTLOADER_OUT)

$(TARGET_NANDWRITE): $(NANDWRITE_OUT)
	@echo $(TARGET_PRODUCT)_nandwrite
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(NANDWRITE_OUT) $(TARGET_PRODUCT)_nandwrite

