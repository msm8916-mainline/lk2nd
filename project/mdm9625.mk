# top level project rules for the mdm9615 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := mdm9625

MODULES += app/aboot

ifeq ($(TARGET_BUILD_VARIANT),user)
DEBUG := 0
else
DEBUG := 1
endif

#DEFINES += WITH_DEBUG_DCC=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
DEFINES += DEVICE_TREE=1

#disable Thumb mode for the codesourcery/arm-2011.03 toolchain
ENABLE_THUMB := false

#Override linker
LD := $(TOOLCHAIN_PREFIX)ld.bfd
