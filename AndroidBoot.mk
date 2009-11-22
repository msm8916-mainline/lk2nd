#Android makefile to build lk bootloader as a part of Android Build

BOOTLOADER_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_OBJ
TARGET_BOOTLOADER := out/target/product/$(TARGET_PRODUCT)/appsboot.mbn

$(BOOTLOADER_OUT):
	mkdir -p $(BOOTLOADER_OUT)

$(TARGET_BOOTLOADER): $(BOOTLOADER_OUT)
	$(MAKE) -C bootable/bootloader/lk BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT)

