# This make file is included for both lk1st and lk2nd.

set(LK2ND 1 CACHE STRING "" FORCE)
#MODULES += lk2nd

# Use maximum verbosity
set(DEBUG 2 CACHE STRING "" FORCE)
# DEFINES += LK_LOG_BUF_SIZE=16384
target_compile_definitions(lk PRIVATE LK_LOG_BUF_SIZE=16384)

# Disable various stupid stuff that we don't really want or need
# DEFINES += DEFAULT_UNLOCK=1 DISABLE_LOCK=1 DISABLE_DEVINFO=1
target_compile_definitions(lk PRIVATE DEFAULT_UNLOCK=1 DISABLE_LOCK=1 DISABLE_DEVINFO=1)
set(DISABLE_RECOVERY_MESSAGES 1 CACHE STRING "" FORCE)

# Enable fastboot display menu
set(ENABLE_FBCON_DISPLAY_MSG 1 CACHE STRING "" FORCE)

#ifeq ($(DISPLAY_USE_CONTINUOUS_SPLASH)$(LK1ST_PANEL),)
#	$(info NOTE: Display support is disabled without panel/display selection)
#	DEFINES := $(filter-out DISPLAY_SPLASH_SCREEN=1, $(DEFINES))
#	ENABLE_FBCON_DISPLAY_MSG := 0
#endif

# Some old platforms are missing the proper defines
if (${ENABLE_FBCON_DISPLAY_MSG} STREQUAL 1)
	target_compile_definitions(lk PRIVATE FBCON_DISPLAY_MSG=1)
endif()
if (${EMMC_BOOT} STREQUAL 1)
	target_compile_definitions(lk PRIVATE _EMMC_BOOT=1)
endif()

# Stop trying to read battery voltage, probably doesn't work on most devices...
# DEFINES := $(filter-out CHECK_BAT_VOLTAGE=1, $(DEFINES))
target_compile_definitions(lk PRIVATE CHECK_BAT_VOLTAGE=0)  # cmake workaround
