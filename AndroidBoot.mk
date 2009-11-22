#Android makefile to build lk bootloader as a part of Android Build

BOOTLOADER_OUT := $(TOP)/$(TARGET_OUT_INTERMEDIATES)/BOOTLOADER_OBJ
TARGET_BOOTLOADER := $(BOOTLOADER_OUT)/lk.elf

$(BOOTLOADER_OUT):
	mkdir -p $(BOOTLOADER_OUT)

$(TARGET_BOOTLOADER): $(BOOTLOADER_OUT)
	$(MAKE) -C bootable/bootloader/lk TARGET_OUT=../../../$(TARGET_OUT) BOOTLOADER_OUT=../../../$(BOOTLOADER_OUT) $(TARGET_PRODUCT)

