ifneq ($(SIGNED_KERNEL)$(VERIFIED_BOOT)$(VERIFIED_BOOT_2),)
$(error The lk2nd configurations provide development/debugging helpers and are
	therefore currently not designed to be used together with secure boot)
endif

include project/$(TARGET).mk

DEBUG := 1

ABOOT_STANDALONE ?= 1
CRYPTO_BACKEND ?= none
DTBO_BACKEND ?= none

MODULES += lk2nd

ifeq ($(ENABLE_DISPLAY), 1)
ifneq ($(LK2ND_DISPLAY),)
MODULES += lk2nd/display
ENABLE_FBCON_DISPLAY_MSG := 1
else
ENABLE_DISPLAY := 0
$(info NOTE: Display support is disabled without display/panel selection)
endif
endif

# Allow flashing independent of battery voltage
DEFINES := $(filter-out CHECK_BAT_VOLTAGE=1, $(DEFINES))

-include lk2nd/project/$(TARGET).mk
