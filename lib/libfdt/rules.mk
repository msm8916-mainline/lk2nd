LOCAL_PATH := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_PATH)
include $(LOCAL_PATH)/Makefile.libfdt
OBJS += $(addprefix $(LOCAL_PATH)/, $(LIBFDT_OBJS))
