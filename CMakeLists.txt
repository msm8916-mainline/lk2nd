cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW) # allow to add sources for target declared in another directory

project(lk2nd
	LANGUAGES C
	VERSION 0.12.0
	DESCRIPTION "Secondary bootloader for Qualcomm devices"
	HOMEPAGE_URL "https://github.com/msm8916-mainline/lk2nd")

#  target platform selection
set(LK_PLATFORM "msm8974" CACHE STRING "Select target platform. Possible values: msm8916, msm8974, ...")
set(LK_ARCH "arm" CACHE STRING "Select LK target arch. Lk only supports arm and x86")

# Fixme
set(ALLOBJS "" CACHE STRING "")
set(dtbs "" CACHE STRING "")
set(LINKER_SCRIPT "" CACHE STRING "Linker script") # is set inside platform/
set(LDFLAGS "")

# our main target
add_executable(lk)

target_include_directories(lk PRIVATE
	"include"
)

# order of including subdirs from top lk makefile:
#    project -> platform -> target -> arch
add_subdirectory(project)
add_subdirectory(platform)
add_subdirectory(target)
add_subdirectory(arch/${LK_ARCH})

#add_subdirectory(app)
#add_subdirectory(dev)
#add_subdirectory(dts)
add_subdirectory(kernel) # has main()
#add_subdirectory(lib)
#add_subdirectory(lk2nd)

# This will create a cmake target to build the final lk.bin
add_custom_command(
	OUTPUT lk.bin
	COMMENT "generating image: "
	MAIN_DEPENDENCY lk.elf
	COMMAND ${CMAKE_OBJCOPY} -O binary lk.elf lk.bin
	VERBATIM
)
add_custom_command(
	OUTPUT lk.bin-dtb
	COMMENT "generating image with appended dtb: "
	MAIN_DEPENDENCY lk.bin
	DEPENDS ${dtbs}
	COMMAND cat lk.bin ${dtbs} > lk.bin-dtb
	VERBATIM
)

# OMG, liker script is also generated file!
# hopefuly target platform will set the MEMBASE and  MEMSIZE variables
add_custom_command(
	OUTPUT ${LINKER_SCRIPT}
	DEPENDS arch/${LK_ARCH}/${LINKER_SCRIPT}
	COMMENT "Running sed on linker script ${LINKER_SCRIPT}"
	COMMAND sed "s/%MEMBASE%/${MEMBASE}/;s/%MEMSIZE%/${MEMSIZE}/" < arch/${LK_ARCH}/${LINKER_SCRIPT} > ${LINKER_SCRIPT}
	VERBATIM
)

# This will create a cmake target to build lk.elf
add_custom_command(
	OUTPUT lk.elf
	DEPENDS ${ALLOBJS} ${LINKER_SCRIPT}
	COMMENT "linking lk.elf"
	COMMAND ${CMAKE_LINKER} ${LDFLAGS} -T ${LINKER_SCRIPT} ${ALLOBJS} ${LIBGCC} -o lk.elf
	VERBATIM
)
