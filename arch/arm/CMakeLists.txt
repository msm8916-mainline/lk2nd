option(ENABLE_THUMB "Enable Thumb instruction set" ON)

# target_compile_definitions(lk PRIVATE ARM_CPU_${ARM_CPU}=1)

message(STATUS "Arch: arm, cpu: ${ARM_CPU}, thumb enabled: ${ENABLE_THUMB}")

# do set some options based on the cpu core
if (${ARM_CPU} STREQUAL "cortex-a8")
	target_compile_definitions(lk PRIVATE
		ARM_CPU_CORTEX_A8
		ARM_WITH_CP15=1
		ARM_WITH_MMU=1
		ARM_ISA_ARMV7=1
		ARM_WITH_VFP=1
		ARM_WITH_NEON=1
		ARM_WITH_THUMB=1
		ARM_WITH_THUMB2=1
		ARM_WITH_CACHE=1
		ARM_WITH_L2=1
	)
	set(CMAKE_CFLAGS "${CMAKE_CFLAGS} -mcpu=${ARM_CPU}")
	#CFLAGS += -mfpu=vfp -mfloat-abi=softfp
else()
	message(WARNING "${CMAKE_CURRENT_LIST_DIR} doesnt have logic for arm core ${ARM_CPU}; this is likely to be broken")
endif()

if (ENABLE_THUMB)
	#THUMBCFLAGS := -mthumb -D__thumb__
	#THUMBINTERWORK := -mthumb-interwork
	set(CMAKE_CFLAGS "${CMAKE_CFLAGS} -mthumb -mthumb-interwork")
	target_compile_definitions(lk PRIVATE "__thumb__")
endif()

target_compile_definitions(lk PRIVATE ARCH_ARM)

target_include_directories(lk PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/include
)

target_sources(lk PRIVATE
	arch.c
	asm.S
	cache-ops.S
	cache.c
	crt0.S
	dcc.S
	exceptions.S
	faults.c
	mmu.c
	ops.S
	thread.c
)
