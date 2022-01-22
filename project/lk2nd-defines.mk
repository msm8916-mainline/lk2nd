# This make file is included to build in "lk2nd" mode (as Android boot image)

# Reserve 512 KiB in boot partition for lk2nd (should be plenty)
DEFINES += LK2ND_SIZE=512*1024

# Use continuous splash from primary bootloader for display
DISPLAY_USE_CONTINUOUS_SPLASH := 1

# Android boot image
OUTBOOTIMG := $(BUILDDIR)/lk2nd.img
OUTODINTAR := $(BUILDDIR)/lk2nd.tar
ANDROID_BOOT_CMDLINE := lk2nd

# Set in platform specific makefile (might need to change ANDROID_BOOT_BASE):
#APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
#ANDROID_BOOT_BASE := 0x80000000
