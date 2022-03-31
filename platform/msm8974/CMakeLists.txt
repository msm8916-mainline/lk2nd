set(ARM_CPU cortex-a8  CACHE STRING "Arm cpu type" FORCE)
set(MMC_SLOT 1  CACHE STRING "MMC slot" FORCE)
set(LINKER_SCRIPT "system-onesegment.ld"  CACHE STRING "Linker script" FORCE)

target_compile_definitions(lk PRIVATE
	ARM_CPU_CORE_KRAIT
	PERIPH_BLK_BLSP=1
	WITH_CPU_EARLY_INIT=0
	WITH_CPU_WARM_BOOT=0
	MMC_SLOT=${MMC_SLOT}
	SSD_ENABLE
	TZ_SAVE_KERNEL_HASH
)

# INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/platform/msm_shared/include
target_include_directories(lk PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
	${CMAKE_SOURCE_DIR}/platform/msm_shared/include
	${CMAKE_SOURCE_DIR}/dev/pmic/pm8x41/include
)

#DEVS += fbcon
#MODULES += dev/fbcon

target_sources(lk PRIVATE
	acpuclock.c
	gpio.c
	msm8974-clock.c
	platform.c
)
