LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/tests.o \
	$(LOCAL_DIR)/thread_tests.o \
	$(LOCAL_DIR)/printf_tests.o \
	$(LOCAL_DIR)/i2c_tests.o \
	$(LOCAL_DIR)/adc_tests.o \
	$(LOCAL_DIR)/kauth_test.o
