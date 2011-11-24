#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTHEADER_DIR  := $(BUILDDIR)
else
APPSBOOTHEADER_DIR  := $(BOOTLOADER_OUT)/../..
endif

SRC_DIR  := target/$(TARGET)/tools
COMPILER := gcc

ifeq ($(EMMC_BOOT), 1)
  APPSBOOTHDR_FILES := EMMCBOOT.MBN
else
  ifeq ($(BUILD_NANDWRITE), 1)
    APPSBOOTHDR_FILES :=
  else
    APPSBOOTHDR_FILES := appsboot.mbn
  endif
endif

APPSBOOTHEADER: $(APPSBOOTHDR_FILES)


appsboot.mbn: appsboothd.mbn $(OUTBIN)
	cp $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboot.raw
	cat $(APPSBOOTHEADER_DIR)/appsboothd.mbn $(OUTBIN) > $(APPSBOOTHEADER_DIR)/appsboot.mbn
	rm -f $(APPSBOOTHEADER_DIR)/appsboothd.mbn

appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboothd.mbn

EMMCBOOT.MBN: emmc_appsboothd.mbn $(OUTBIN)
	cp $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboot.raw
	cat $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn $(OUTBIN) > $(APPSBOOTHEADER_DIR)/EMMCBOOT.MBN
	cat $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn $(OUTBIN) > $(APPSBOOTHEADER_DIR)/emmc_appsboot.mbn
	rm -f $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn

emmc_appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn

mkheader: $(SRC_DIR)/mkheader.c
	${COMPILER} -DMEMBASE=$(MEMBASE) $(SRC_DIR)/mkheader.c -o $(SRC_DIR)/mkheader
