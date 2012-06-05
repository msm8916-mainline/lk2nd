LOCAL_PATH := $(GET_LOCAL_DIR)

LIBFDT_INCLUDES = fdt.h libfdt.h
LIBFDT_SRCS = fdt.c fdt_ro.c fdt_wip.c fdt_sw.c fdt_rw.c fdt_strerror.c
LIBFDT_OBJS = $(LIBFDT_SRCS:%.c=%.o)

INCLUDES += -I$(LOCAL_PATH)

OBJS += $(addprefix $(LOCAL_PATH)/, $(LIBFDT_OBJS))
