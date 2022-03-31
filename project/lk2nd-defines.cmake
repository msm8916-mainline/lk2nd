# This make file is included to build in "lk2nd" mode (as Android boot image)

# Reserve 512 KiB in boot partition for lk2nd (should be plenty)
# DEFINES += LK2ND_SIZE=512*1024
target_compile_definitions(lk PRIVATE LK2ND_SIZE=524288)

# Use continuous splash from primary bootloader for display
set(DISPLAY_USE_CONTINUOUS_SPLASH 1 CACHE STRING "" FORCE)

# Android boot image
#OUTBOOTIMG $(BUILDDIR)/lk2nd.img
#OUTODINTAR := $(BUILDDIR)/lk2nd.tar
set(ANDROID_BOOT_CMDLINE lk2nd CACHE STRING "" FORCE)
