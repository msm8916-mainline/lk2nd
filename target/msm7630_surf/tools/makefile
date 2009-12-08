#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTHEADER_DIR  := $(BUILDDIR)
else
APPSBOOTHEADER_DIR  := $(BOOTLOADER_OUT)/../../
endif

SRC_DIR  := target/$(TARGET)/tools
COMPILER := gcc

APPSBOOTHEADER: appsboot.mbn emmc_appsboot.mbn


appsboot.mbn: appsboothd.mbn $(OUTBIN)
	cat $(APPSBOOTHEADER_DIR)/appsboothd.mbn $(OUTBIN) > $(APPSBOOTHEADER_DIR)/appsboot.mbn
	rm -rf $(APPSBOOTHEADER_DIR)/appsboothd.mbn

appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboothd.mbn

emmc_appsboot.mbn: emmc_appsboothd.mbn $(OUTBIN)
	cat $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn $(OUTBIN) > $(APPSBOOTHEADER_DIR)/emmc_appsboot.mbn
	rm -rf $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn

emmc_appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn unified-boot

mkheader: $(SRC_DIR)/mkheader.c
	${COMPILER} $(SRC_DIR)/mkheader.c -o $(SRC_DIR)/mkheader

