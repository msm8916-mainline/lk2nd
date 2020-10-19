LOCAL_DIR := $(GET_LOCAL_DIR)

ifeq ($(VERIFIED_BOOT),1)
MODULES += \
	lib/openssl
endif

OBJS += \
	$(LOCAL_DIR)/app.o

