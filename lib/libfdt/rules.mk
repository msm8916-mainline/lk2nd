LOCAL_PATH := $(GET_LOCAL_DIR)

include $(LOCAL_PATH)/Makefile.libfdt
OBJS += $(addprefix $(LOCAL_PATH)/, $(LIBFDT_OBJS))
INCLUDES += -I$(LOCAL_PATH)
