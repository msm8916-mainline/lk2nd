LOCAL_DIR := $(GET_LOCAL_DIR)

ifeq ($(PROJECT), msm8916-secondary)
DTBS += \
	$(LOCAL_DIR)/apq8016-samsung.dtb \
	$(LOCAL_DIR)/msm8916-longcheer-l8150.dtb \
	$(LOCAL_DIR)/msm8916-samsung.dtb
endif
