LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
	lib/openssl

ifeq ($(ENABLE_UNITTEST_FW), 1)
MODULES += \
	app/tests
endif

OBJS += \
	$(LOCAL_DIR)/app.o

