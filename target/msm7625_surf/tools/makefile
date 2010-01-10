#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTHEADER_DIR  := $(BUILDDIR)
else
APPSBOOTHEADER_DIR  := $(BOOTLOADER_OUT)/../../
endif

SRC_DIR  := target/$(TARGET)/tools
COMPILER := gcc

ifeq ($(EMMC_BOOT), 1)
  APPSBOOTHDR_FILES := emmc_appsboot.mbn emmc_appsboothd.mbn
else
  ifeq ($(BUILD_NANDWRITE), 1)
    APPSBOOTHDR_FILES :=
  else
    APPSBOOTHDR_FILES := appsboot.mbn appsboothd.mbn
  endif
endif

APPSBOOTHEADER: $(APPSBOOTHDR_FILES)

appsboot.mbn: $(OUTBIN)
	cp -f $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboot.mbn

appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboothd.mbn

emmc_appsboot.mbn: $(OUTBIN)
	cp -f $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboot.mbn

emmc_appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/emmc_appsboothd.mbn unified-boot

mkheader: $(SRC_DIR)/mkheader.c
	${COMPILER} $(SRC_DIR)/mkheader.c -o $(SRC_DIR)/mkheader
