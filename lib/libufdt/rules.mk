LOCAL_PATH := $(GET_LOCAL_DIR)

MODULES += lib/libfdt

LIBFDT_SRCS =  ufdt_overlay.c ufdt_convert.c ufdt_node.c ufdt_node_pool.c ufdt_prop_dict.c sysdeps/libufdt_sysdeps_vendor.c
LIBFDT_OBJS = $(LIBFDT_SRCS:%.c=%.o)

INCLUDES += -I$(LOCAL_PATH) -I$(LOCAL_PATH)/include -I$(LOCAL_PATH)/sysdeps/include

OBJS += $(addprefix $(LOCAL_PATH)/, $(LIBFDT_OBJS))
