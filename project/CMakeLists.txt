#include $(LOCAL_DIR)/lk2nd-defines.mk
message(STATUS "Including: ${CMAKE_CURRENT_LIST_DIR}/lk2nd-defines.cmake")
include(${CMAKE_CURRENT_LIST_DIR}/lk2nd-defines.cmake)

#include $(LOCAL_DIR)/msm8974.mk
message(STATUS "Including: ${CMAKE_CURRENT_LIST_DIR}/${LK_PLATFORM}.cmake")
include(${CMAKE_CURRENT_LIST_DIR}/${LK_PLATFORM}.cmake)

#include $(LOCAL_DIR)/lk2nd-base.mk
message(STATUS "Including: ${CMAKE_CURRENT_LIST_DIR}/lk2nd-base.cmake")
include(${CMAKE_CURRENT_LIST_DIR}/lk2nd-base.cmake)

#DEFINES += WITH_DEBUG_LOG_BUF=1

#APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTODINTAR)
set(ANDROID_BOOT_BASE 0x00000000 CACHE STRING "" FORCE)

# Memory usually reserved for RMTFS, should be fine for early SMP bring-up
set(SMP_SPIN_TABLE_BASE 0x0fd80000 CACHE STRING "Base of RMTFS mem, reused for early SMP bring-up" FORCE)
