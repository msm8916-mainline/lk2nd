# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)
MODULES += lib/libfdt

ifneq ($(filter msm8226 msm8610 msm8909 msm8916, $(PLATFORM)),)
CPU_BOOT_OBJ := $(LOCAL_DIR)/cortex-a.o
DEFINES += CPU_BOOT_CORTEX_A=1
else ifneq ($(filter apq8084 msm8974, $(PLATFORM)),)
CPU_BOOT_OBJ := $(if $(BUILD_GPL), $(LOCAL_DIR)/gpl/krait.o)
DEFINES += CPU_BOOT_KPSSV2=1
else ifneq ($(filter msm8960, $(PLATFORM)),)
CPU_BOOT_OBJ := $(if $(BUILD_GPL), $(LOCAL_DIR)/gpl/krait.o)
DEFINES += CPU_BOOT_KPSSV1=1
endif

OBJS += $(if $(CPU_BOOT_OBJ), $(LOCAL_DIR)/cpu-boot.o $(CPU_BOOT_OBJ))
