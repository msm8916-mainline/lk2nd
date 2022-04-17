LOCAL_DIR := $(GET_LOCAL_DIR)

ifeq ($(ENABLE_UNITTEST_FW), 1)
MODULES += \
	app/tests
endif

OBJS += \
	$(LOCAL_DIR)/app.o

