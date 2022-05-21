LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/lib/zlib_inflate

DEFINES += ASSERT_ON_TAMPER=1

MODULES += lib/zlib_inflate

ABOOT_STANDALONE ?= 0
DEFINES += ABOOT_STANDALONE=$(ABOOT_STANDALONE)

OBJS += \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/recovery.o

ABOOT_STANDALONE ?= 0
DEFINES += ABOOT_STANDALONE=$(ABOOT_STANDALONE)
ifeq ($(ABOOT_STANDALONE), 1)
OBJS := $(filter-out $(LOCAL_DIR)/recovery.o, $(OBJS))
DEFINES := $(filter-out SSD_ENABLE TZ_SAVE_KERNEL_HASH TZ_TAMPER_FUSE, $(DEFINES))
endif

ifeq ($(ENABLE_UNITTEST_FW), 1)
OBJS += \
	$(LOCAL_DIR)/fastboot_test.o
	ifeq ($(ENABLE_MDTP_SUPPORT),1)
		OBJS += \
			$(LOCAL_DIR)/mdtp_lk_ut.o
	endif
endif

ifeq ($(ENABLE_MDTP_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/mdtp.o \
	$(LOCAL_DIR)/mdtp_ui.o \
	$(LOCAL_DIR)/mdtp_fuse.o \
	$(LOCAL_DIR)/mdtp_defs.o \
	$(LOCAL_DIR)/mdtp_fs.o
endif
