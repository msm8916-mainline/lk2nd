LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

OBJS += \
	$(LOCAL_DIR)/lvds_chimei_wxga.o \
	$(LOCAL_DIR)/mipi_toshiba_video_wsvga.o

