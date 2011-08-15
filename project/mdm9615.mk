# top level project rules for the mdm9615 project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := mdm9615

MODULES += app/aboot

#DEFINES += WITH_DEBUG_DCC=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
