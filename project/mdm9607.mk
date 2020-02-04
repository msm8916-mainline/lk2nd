# top level project rules for the mdm9607 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := mdm9607

MODULES += app/aboot

ifeq ($(TARGET_BUILD_VARIANT),user)
DEBUG := 0
else
DEBUG := 1
endif

ifeq ($(TARGET_BOOTIMG_SIGNED),true)
CFLAGS += -D_SIGNED_KERNEL=1
endif

DEFINES += WITH_DEBUG_UART=1
DEFINES += WITH_DEBUG_LOG_BUF=1
DEFINES += DEVICE_TREE=1
DEFINES += CONTIGUOUS_MEMORY=1

DEFINES += SPMI_CORE_V2=1
DEFINES += BAM_V170=1
#Disable thumb mode
ENABLE_THUMB := false

#Override linker for mdm targets
LD := $(TOOLCHAIN_PREFIX)ld.bfd

ENABLE_SMD_SUPPORT := 1
ifeq ($(ENABLE_SMD_SUPPORT),1)
DEFINES += SMD_SUPPORT=1
endif
