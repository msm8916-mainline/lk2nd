# top level project rules for the msm8610 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := msm8610

MODULES += app/aboot

DEBUG := 1
EMMC_BOOT := 1
ENABLE_SDHCI_SUPPORT := 1

#DEFINES += WITH_DEBUG_DCC=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
DEFINES += DEVICE_TREE=1
#DEFINES += MMC_BOOT_BAM=1
#DEFINES += CRYPTO_BAM=1

#Disable thumb mode
#TODO: The gold linker has issues generating correct
#thumb interworking code for LK. Confirm that the issue
#is with the linker and file a bug report.
ENABLE_THUMB := false

ifeq ($(ENABLE_SDHCI_SUPPORT),1)
DEFINES += MMC_SDHCI_SUPPORT=1
endif
