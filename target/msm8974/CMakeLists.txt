#INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared
#INCLUDES += -I$(LK_TOP_DIR)/dev/gcdb/display -I$(LK_TOP_DIR)/dev/gcdb/display/include

target_include_directories(lk PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_SOURCE_DIR}/platform/msm_shared
	${CMAKE_SOURCE_DIR}/dev/gcdb/display
	${CMAKE_SOURCE_DIR}/dev/gcdb/display/include
)

# SDRAM, 1 MB
set(MEMBASE 0x0F900000 CACHE STRING "" FORCE)
set(MEMSIZE 0x00100000  CACHE STRING "" FORCE)

set(BASE_ADDR 0x00000 CACHE STRING "" FORCE)
set(TAGS_ADDR 0x00000100 CACHE STRING "" FORCE)
set(KERNEL_ADDR 0x00008000 CACHE STRING "" FORCE)
set(RAMDISK_ADDR 0x01000000 CACHE STRING "" FORCE)
set(SCRATCH_ADDR 0x11000000 CACHE STRING "" FORCE)

#MODULES += \
#	dev/keys \
#	dev/pmic/pm8x41 \
#	dev/gcdb/display \
#	lib/ptable \
#	lib/libfdt

target_compile_definitions(lk PRIVATE
	DISPLAY_SPLASH_SCREEN=1
	DISPLAY_TYPE_MIPI=1
	DISPLAY_TYPE_DSI6G=1
	MEMSIZE=${MEMSIZE}
	MEMBASE=${MEMBASE}
	BASE_ADDR=${BASE_ADDR}
	TAGS_ADDR=${TAGS_ADDR}
	KERNEL_ADDR=${KERNEL_ADDR}
	RAMDISK_ADDR=${RAMDISK_ADDR}
	SCRATCH_ADDR=${SCRATCH_ADDR}
)

target_sources(lk PRIVATE
	init.c
	meminfo.c
	target_display.c
	oem_panel.c
)
