LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include  -I$(LK_TOP_DIR)/platform/msm_shared/include

OBJS += \
	$(LOCAL_DIR)/mmu_test.o
