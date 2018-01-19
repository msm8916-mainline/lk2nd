LOCAL_PATH := $(GET_LOCAL_DIR)

LIBFDT_INCLUDES = ufdt_util.h fdt_internal.h ufdt_types.h ufdt_overlay.h libufdt.h
LIBFDT_SRCS =  ufdt_overlay.c ufdt_node_dict.c ufdt_node.c ufdt_convert.c sysdeps/libufdt_sysdeps_vendor.c
LIBFDT_OBJS = $(LIBFDT_SRCS:%.c=%.o)

INCLUDES += -I$(LOCAL_PATH) -I$(LOCAL_PATH)/include -I$(LOCAL_PATH)/sysdeps/include

OBJS += $(addprefix $(LOCAL_PATH)/, $(LIBFDT_OBJS))
