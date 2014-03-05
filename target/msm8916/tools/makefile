#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTOUT_DIR  := $(BUILDDIR)
else
APPSBOOTOUT_DIR  := $(BOOTLOADER_OUT)/../..
endif

APPSBOOTHEADER: emmc_appsboot.mbn

emmc_appsboot.mbn: $(OUTELF_STRIP)
	$(hide) cp -f $(OUTELF_STRIP) $(APPSBOOTOUT_DIR)/emmc_appsboot.mbn
