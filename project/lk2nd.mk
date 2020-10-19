# Use maximum verbosity
DEBUG := 2
DEFINES += LK_LOG_BUF_SIZE=16384

# Avoid writing device info
DEFINES += SAFE_MODE=1
# Display as unlocked by default
DEFINES += DEFAULT_UNLOCK=1

# Reserve 512 KiB in boot partition for lk2nd (should be plenty)
DEFINES += LK2ND_SIZE=512*1024

# Use continuous splash from primary bootloader for display
DISPLAY_USE_CONTINUOUS_SPLASH := 1

# Enable fastboot display menu
ENABLE_FBCON_DISPLAY_MSG := 1

# Android boot image
OUTBOOTIMG := $(BUILDDIR)/lk2nd.img
OUTODINTAR := $(BUILDDIR)/lk2nd.tar
ANDROID_BOOT_CMDLINE := lk2nd

# Set in platform specific makefile (might need to change ANDROID_BOOT_BASE):
#APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
#ANDROID_BOOT_BASE := 0x80000000

# Some old platforms are missing the proper defines
ifeq ($(ENABLE_FBCON_DISPLAY_MSG),1)
DEFINES += FBCON_DISPLAY_MSG=1
endif
ifeq ($(EMMC_BOOT),1)
DEFINES += _EMMC_BOOT=1
endif
