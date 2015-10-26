LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += $(LOCAL_DIR)/rpmb.o \
        $(LOCAL_DIR)/rpmb_listener.o \
        $(LOCAL_DIR)/rpmb_emmc.o

ifeq ($(ENABLE_UFS_SUPPORT), 1)
    OBJS += $(LOCAL_DIR)/rpmb_ufs.o
endif
