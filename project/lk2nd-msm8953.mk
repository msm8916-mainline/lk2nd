LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := msm8953
DISPLAY_USE_CONTINUOUS_SPLASH := 1
include $(LOCAL_DIR)/lk2nd-base.mk

# Since lk2nd is typically used through lk.bin, having separate code/data
# segments with a fixed 1 MiB offset increases the binary size significantly,
# since a lot of padding has to be added inbetween. Let's just disable this for now.
DEFINES := $(filter-out SECURE_CODE_MEM=1, $(DEFINES))
