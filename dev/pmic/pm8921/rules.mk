LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/pm8921.o \
	$(LOCAL_DIR)/pm8921_pwm.o \

