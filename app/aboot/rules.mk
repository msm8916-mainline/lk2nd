LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include

DEFINES += ASSERT_ON_TAMPER=1

OBJS += \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/recovery.o

ifeq ($(ENABLE_MDTP_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/mdtp.o \
	$(LOCAL_DIR)/mdtp_ui.o
endif

