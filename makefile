ifeq ($(MAKECMDGOALS),spotless)
spotless:
	rm -rf build-*
else

-include local.mk
include make/macros.mk

# If one of our goals (from the commandline) happens to have a
# matching project/goal.mk, then we should re-invoke make with
# that project name specified...

project-name := $(firstword $(MAKECMDGOALS))

ifneq ($(project-name),)
ifneq ($(wildcard project/$(project-name).mk),)
do-nothing := 1
$(MAKECMDGOALS) _all: make-make
make-make:
	@PROJECT=$(project-name) $(MAKE) $(filter-out $(project-name), $(MAKECMDGOALS))
endif
endif

ifeq ($(do-nothing),)

ifeq ($(PROJECT),)
$(error No project specified.  Use "make projectname" or put "PROJECT := projectname" in local.mk)
endif

DEBUG ?= 0

ifndef $(BOOTLOADER_OUT)
BOOTLOADER_OUT := .
endif

LK_TOP_DIR:= .
BUILDDIR := $(BOOTLOADER_OUT)/build-$(PROJECT)
OUTBIN := $(BUILDDIR)/lk.bin
OUTELF := $(BUILDDIR)/lk
OUTELF_STRIP := $(BUILDDIR)/lk_s.elf

CONFIGHEADER := $(BUILDDIR)/config.h

#Initialize the command-line flag ENABLE_TRUSTZONE. Value for flag passed in at command-line will take precedence
ENABLE_TRUSTZONE := 0
ENABLE_DISPLAY   ?= 1

ifeq ($(ENABLE_TRUSTZONE),1)
	INPUT_TZ_BIN := tzbsp/tzbsp.bin
	OUTPUT_TZ_BIN := $(BUILDDIR)/tzbsp_bin.o
endif

INCLUDES := -I$(BUILDDIR) -Iinclude
CFLAGS := -O2 -g -finline -W -Wall -Wno-multichar -Wno-unused-parameter -Wno-unused-function $(LKLE_CFLAGS) -include $(CONFIGHEADER)
# -fcommon is needed to build using GCC 10
CFLAGS += -fcommon
# causes several warnings in the fastboot code where truncation is expected
CFLAGS += -Wno-format-truncation
#CFLAGS += -Werror

ifeq ($(SIGNED_KERNEL),1)
  CFLAGS += -D_SIGNED_KERNEL=1
endif

ifeq ($(TARGET_BUILD_VARIANT),user)
  CFLAGS += -DDISABLE_FASTBOOT_CMDS=1
endif

ifeq ($(APPEND_CMDLINE),1)
  CFLAGS += -D_APPEND_CMDLINE=1
endif
ifeq ($(ENABLE_HARD_FPU),1)
  CFLAGS += -mfloat-abi=hard -mfpu=neon
endif

ifeq ($(ENABLE_EARLY_ETHERNET),1)
  CFLAGS += -DENABLE_EARLY_ETHERNET=1
endif

# setup toolchain prefix
TOOLCHAIN_PREFIX ?= arm-eabi-
CFLAGS += -fstack-protector-all
CFLAGS += -fno-strict-overflow
CPPFLAGS := -fno-exceptions -fno-rtti -fno-threadsafe-statics
#CPPFLAGS += -Weffc++
ASMFLAGS := -DASSEMBLY
LDFLAGS := 

CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -gc-sections

# top level rule
all:: $(OUTBIN) $(OUTELF).lst $(OUTELF).debug.lst $(OUTELF).sym $(OUTELF).size $(OUTELF_STRIP) APPSBOOTHEADER

# the following three object lists are identical except for the ordering
# which is bootobjs, kobjs, objs
BOOTOBJS :=	
OBJS :=

# a linker script needs to be declared in one of the project/target/platform files
LINKER_SCRIPT := 			

# anything you add here will be deleted in make clean
GENERATED := $(CONFIGHEADER)

# anything added to DEFINES will be put into $(BUILDDIR)/config.h
DEFINES := LK=1				

# Anything added to SRCDEPS will become a dependency of every source file in the system.
# Useful for header files that may be included by one or more source files.
SRCDEPS := $(CONFIGHEADER)

ifeq ($(VERIFIED_BOOT),1)
  DEFINES += VERIFIED_BOOT=1
  DEFINES += _SIGNED_KERNEL=1
  ifeq ($(DEFAULT_UNLOCK),true)
    DEFINES += DEFAULT_UNLOCK=1
  endif
endif

ifeq ($(VERIFIED_BOOT_LE),1)
  DEFINES += VERIFIED_BOOT_LE=1
  ifeq ($(DEFAULT_UNLOCK),true)
    DEFINES += DEFAULT_UNLOCK=1
  endif
endif
ifeq ($(VERIFIED_BOOT_2),1)
  DEFINES += VERIFIED_BOOT_2=1
  DEFINES += _SIGNED_KERNEL=1
endif

ifeq ($(VB1_KEY_USED),1)
  DEFINES += VB1_KEY_USED=1
endif

ifeq ($(DYNAMIC_PARTITION_SUPPORT),1)
  DEFINES += DYNAMIC_PARTITION_SUPPORT=1
endif

ifeq ($(TARGET_DTBO_NOT_SUPPORTED),1)
  DEFINES += TARGET_DTBO_NOT_SUPPORTED=1
endif

ifeq ($(OSVERSION_IN_BOOTIMAGE),1)
 DEFINES += OSVERSION_IN_BOOTIMAGE=1
endif

ifeq ($(HIBERNATION_SUPPORT),1)
DEFINES += HIBERNATION_SUPPORT=1
endif

ifeq ($(ENABLE_VB_ATTEST),1)
 DEFINES += ENABLE_VB_ATTEST=1
endif

ifeq ($(USER_BUILD_VARIANT),true)
  DEFINES += USER_BUILD_VARIANT=1
endif

ifeq ($(USE_LE_SYSTEMD),true)
  DEFINES += USE_LE_SYSTEMD=1
else
  DEFINES += USE_LE_SYSTEMD=0
endif

ifeq ($(MOUNT_EMMC_LE),true)
  DEFINES += MOUNT_EMMC_LE=1
else
  DEFINES += MOUNT_EMMC_LE=0
endif

#Enable kaslr seed support
ifeq ($(ENABLE_KASLRSEED),1)
  DEFINES += ENABLE_KASLRSEED_SUPPORT=1
else
  DEFINES += ENABLE_KASLRSEED_SUPPORT=0
endif

ifeq ($(TARGET_USE_SYSTEM_AS_ROOT_IMAGE),1)
  DEFINES += TARGET_USE_SYSTEM_AS_ROOT_IMAGE=1
else
  DEFINES += TARGET_USE_SYSTEM_AS_ROOT_IMAGE=0
endif

# these need to be filled out by the project/target/platform rules.mk files
TARGET :=
PLATFORM :=
ARCH :=
ALLMODULES :=
MODULES :=

# any rules you put here will also be built by the system before considered being complete
EXTRA_BUILDDEPS := 

# any rules you put here will be depended on in clean builds
EXTRA_CLEANDEPS := 

include project/$(PROJECT).mk
include target/$(TARGET)/rules.mk
include target/$(TARGET)/tools/makefile
include platform/$(PLATFORM)/rules.mk
include arch/$(ARCH)/rules.mk
include platform/rules.mk
include target/rules.mk
include kernel/rules.mk
include dev/rules.mk
include app/rules.mk

# recursively include any modules in the MODULE variable, leaving a trail of included
# modules in the ALLMODULES list
include make/module.mk

# any extra top level build dependencies that someone declared
all:: $(EXTRA_BUILDDEPS)

ALLOBJS := \
	$(BOOTOBJS) \
	$(OBJS)

# add some automatic configuration defines
DEFINES += \
	PROJECT_$(PROJECT)=1 \
	TARGET_$(TARGET)=1 \
	PLATFORM_$(PLATFORM)=1 \
	ARCH_$(ARCH)=1 \
	$(addsuffix =1,$(addprefix WITH_,$(ALLMODULES)))

# Add MEMRWOFF as . for targets this is not declared.
# . will be replaced as string in linker file.
ifeq ($(MEMRWOFF),)
MEMRWOFF:= .
DEFINES += MEMRWOFF=$(MEMRWOFF)
endif

# debug build?
ifneq ($(DEBUG),)
DEFINES += \
	DEBUG=$(DEBUG)
endif

ifeq ($(EMMC_BOOT),1)
DEFINES += _EMMC_BOOT=1
endif

ALLOBJS := $(addprefix $(BUILDDIR)/,$(ALLOBJS))
ALLDTBS := $(addprefix $(BUILDDIR)/,$(ALLDTBS))

DEPS := $(ALLOBJS:%o=%d) $(addsuffix .d,$(ALLDTBS))

# default to no ccache
CCACHE ?= 
CC := $(CCACHE) $(TOOLCHAIN_PREFIX)gcc
ifeq ($(LD),ld)
LD := $(TOOLCHAIN_PREFIX)ld
endif
STRIP := $(TOOLCHAIN_PREFIX)strip
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
CPPFILT := $(TOOLCHAIN_PREFIX)c++filt
SIZE := $(TOOLCHAIN_PREFIX)size
NM := $(TOOLCHAIN_PREFIX)nm

# comment out or override if you want to see the full output of each command
NOECHO ?= @

# the logic to compile and link stuff is in here
include make/build.mk

clean: $(EXTRA_CLEANDEPS)
	rm -f $(ALLOBJS) $(ALLDTBS) $(DEPS) $(GENERATED) $(OUTBIN) $(OUTELF) $(OUTELF).lst $(OUTELF_STRIP)

install: all
	scp $(OUTBIN) 192.168.0.4:/tftproot

# generate a config.h file with all of the DEFINES laid out in #define format
configheader:

$(CONFIGHEADER): configheader
	@$(MKDIR)
	@echo generating $@
	@rm -f $(CONFIGHEADER).tmp; \
	echo \#ifndef __CONFIG_H > $(CONFIGHEADER).tmp; \
	echo \#define __CONFIG_H >> $(CONFIGHEADER).tmp; \
	echo \#define BOARD $(BOARD_NAME) >> $(CONFIGHEADER).tmp; \
	echo -n '$(DEFINES)' | \
		awk -F= -vRS=' ' 'NF { gsub(/-|\//, "_", $$1); $$1 = toupper($$1); print "#define", $$0 }' \
		>> $(CONFIGHEADER).tmp; \
	echo \#endif >> $(CONFIGHEADER).tmp; \
	if [ -f "$(CONFIGHEADER)" ]; then \
		if cmp "$(CONFIGHEADER).tmp" "$(CONFIGHEADER)"; then \
			rm -f $(CONFIGHEADER).tmp; \
		else \
			mv $(CONFIGHEADER).tmp $(CONFIGHEADER); \
		fi \
	else \
		mv $(CONFIGHEADER).tmp $(CONFIGHEADER); \
	fi

# Empty rule for the .d files. The above rules will build .d files as a side
# effect. Only works on gcc 3.x and above, however.
%.d:

ifeq ($(filter $(MAKECMDGOALS), clean), )
-include $(DEPS)
endif

.PHONY: configheader
endif

endif # make spotless
