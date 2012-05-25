LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

ifeq ($(PLATFORM),msm8960)
OBJS += \
	$(LOCAL_DIR)/lvds_chimei_wxga.o \
	$(LOCAL_DIR)/mipi_toshiba_video_wsvga.o
endif

ifeq ($(PLATFORM),msm7x27a)
OBJS += \
	$(LOCAL_DIR)/mipi_renesas.o \
	$(LOCAL_DIR)/mipi_renesas_video_fwvga.o \
	$(LOCAL_DIR)/mipi_renesas_cmd_fwvga.o \
	$(LOCAL_DIR)/mipi_nt35510.o \
	$(LOCAL_DIR)/mipi_nt35510_video_wvga.o \
	$(LOCAL_DIR)/mipi_nt35510_cmd_wvga.o
endif
