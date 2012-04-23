#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTHEADER_DIR  := $(BUILDDIR)
else
APPSBOOTHEADER_DIR  := $(BOOTLOADER_OUT)/../..
endif

SRC_DIR  := target/$(TARGET)/tools
COMPILER := gcc

ifeq ($(EMMC_BOOT), 1)
  APPSBOOTHDR_FILES := EMMCBOOT.MBN emmc_appsboothd.mbn
else
  ifeq ($(BUILD_NANDWRITE), 1)
    APPSBOOTHDR_FILES :=
  else
    APPSBOOTHDR_FILES := appsboot.mbn appsboothd.mbn
  endif
endif

APPSBOOTHEADER: $(APPSBOOTHDR_FILES)

appsboot.mbn: $(OUTBIN)
	cp $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboot.raw
	cp -f $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboot.mbn

appsboothd.mbn: mkheader $(OUTBIN)
	$(BUILDDIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboothd.mbn

EMMCBOOT.MBN: $(OUTBIN)
	cp $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboot.raw
	cp -f $(OUTBIN) $(APPSBOOTHEADER_DIR)/EMMCBOOT.MBN
	cp -f $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboot.mbn

emmc_appsboothd.mbn: mkheader $(OUTBIN)
	$(BUILDDIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn unified-boot

mkheader: $(SRC_DIR)/mkheader.c
	@mkdir -p $(BUILDDIR)
	${COMPILER} -DMEMBASE=$(MEMBASE) $(SRC_DIR)/mkheader.c -o $(BUILDDIR)/mkheader
