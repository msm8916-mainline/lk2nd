LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/pm8x41.o \
	$(LOCAL_DIR)/pm8x41_adc.o \
	$(LOCAL_DIR)/pm8x41_wled.o

ifeq ($(ENABLE_PON_VIB_SUPPORT),true)
OBJS += \
	$(LOCAL_DIR)/pm8x41_vib.o
endif

ifeq ($(ENABLE_PWM_SUPPORT),true)
OBJS += \
	$(LOCAL_DIR)/pm_pwm.o
endif
