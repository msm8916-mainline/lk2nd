LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include

DEFINES += ASSERT_ON_TAMPER=1

OBJS += \
	$(LOCAL_DIR)/ufs_rpmb.o \
	$(LOCAL_DIR)/qseecom_lk_test.o
