#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTOUT_DIR  := $(BUILDDIR)
else
APPSBOOTOUT_DIR  := $(BOOTLOADER_OUT)/../..
endif

ifeq ($(EMMC_BOOT), 1)
ABOOTMBN := emmc_appsboot.mbn
else
ABOOTMBN := appsboot.mbn
endif

APPSBOOTHEADER: $(ABOOTMBN)

$(ABOOTMBN): $(OUTELF_STRIP)
	$(hide) cp -f $(OUTELF_STRIP) $(APPSBOOTOUT_DIR)/$(ABOOTMBN)
