LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/lib/zlib_inflate

DEFINES += ASSERT_ON_TAMPER=1

MODULES += lib/zlib_inflate

OBJS += \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/recovery.o

ifneq ($(filter lk2nd,$(ALLMODULES)),)
OBJS += $(LOCAL_DIR)/fastboot-lk2nd.o
endif

ifeq ($(TARGET), msm8916)
OBJS += $(LOCAL_DIR)/fastboot-lk2nd-msm8916.o
endif
