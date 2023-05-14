LOCAL_DIR := $(GET_LOCAL_DIR)

# apq8026-lg-lenok needs to be first, otherwise lenok doesn't boot.
DTBS += \
	$(LOCAL_DIR)/apq8026-lg-lenok.dtb \
	$(LOCAL_DIR)/apq8026-asus-sparrow.dtb \
	$(LOCAL_DIR)/apq8026-huawei-sturgeon.dtb \
	$(LOCAL_DIR)/apq8026-samsung-r03.dtb \
	$(LOCAL_DIR)/msm8226-motorola-falcon.dtb \
	$(LOCAL_DIR)/msm8226-samsung-ms013g.dtb \
	$(LOCAL_DIR)/msm8926-huawei-g6-l11-vb.dtb \
	$(LOCAL_DIR)/msm8926-samsung-r02.dtb
