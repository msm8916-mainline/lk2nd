LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/pm8x41.o \
	$(LOCAL_DIR)/pm8x41_adc.o \
	$(LOCAL_DIR)/pm8x41_wled.o

ifeq ($(ENABLE_WEAK_BATT_CHRG_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/qpnp-smb2.o \
	$(LOCAL_DIR)/qpnp-fg-gen3.o
endif

ifeq ($(ENABLE_PON_VIB_SUPPORT),true)
OBJS += \
	$(LOCAL_DIR)/pm8x41_vib.o
endif

ifeq ($(ENABLE_PWM_SUPPORT),true)
OBJS += \
	$(LOCAL_DIR)/pm_pwm.o
endif

ifeq ($(ENABLE_VADC_HC_SUPPORT),true)
OBJS += \
	$(LOCAL_DIR)/pm_vadc_hc.o
endif
