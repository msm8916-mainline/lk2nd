LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/pm_app_smbchg.o \
	$(LOCAL_DIR)/pm_config_target.o \
	$(LOCAL_DIR)/pm_smbchg_bat_if.o \
	$(LOCAL_DIR)/pm_smbchg_dc_chgpth.o\
	$(LOCAL_DIR)/pm_smbchg_misc.o \
	$(LOCAL_DIR)/pm_comm_spmi_lite.o \
	$(LOCAL_DIR)/pm_fg_adc_usr.o \
	$(LOCAL_DIR)/pm_fg_driver.o \
	$(LOCAL_DIR)/pm_smbchg_chgr.o \
	$(LOCAL_DIR)/pm_smbchg_driver.o \
	$(LOCAL_DIR)/pm_smbchg_usb_chgpth.o
