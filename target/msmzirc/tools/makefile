#Makefile to generate appsboot.mbn

ifeq ($(BOOTLOADER_OUT),.)
APPSBOOTOUT_DIR  := $(BUILDDIR)
else
APPSBOOTOUT_DIR := $(BOOTLOADER_OUT)/../..
endif

ABOOTMBN := appsboot.mbn
APPSBOOTHEADER: $(ABOOTMBN)

$(ABOOTMBN): $(OUTELF_STRIP)
	$(hide) cp -f $(OUTELF_STRIP) $(APPSBOOTOUT_DIR)/$(ABOOTMBN)
