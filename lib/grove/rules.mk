LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += lib/libfdt

INCLUDES += -I$(LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/grove.o

