#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTHEADER_DIR  := $(BUILDDIR)
else
APPSBOOTHEADER_DIR  := $(BOOTLOADER_OUT)/../../
endif

SRC_DIR  := target/$(TARGET)/tools
COMPILER := gcc

APPSBOOTHEADER: appsboot.mbn appsboothd.mbn

appsboot.mbn: $(OUTBIN)
	cp -r $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboot.mbn

appsboothd.mbn: mkheader $(OUTBIN)
	$(SRC_DIR)/mkheader $(OUTBIN) $(APPSBOOTHEADER_DIR)/appsboothd.mbn

mkheader: $(SRC_DIR)/mkheader.c
	${COMPILER} $(SRC_DIR)/mkheader.c -o $(SRC_DIR)/mkheader

