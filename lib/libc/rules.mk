LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/atoi.o \
	$(LOCAL_DIR)/bsearch.o \
	$(LOCAL_DIR)/ctype.o \
	$(LOCAL_DIR)/errno.o \
	$(LOCAL_DIR)/gcd_lcm.o \
	$(LOCAL_DIR)/itoa.o \
	$(LOCAL_DIR)/printf.o \
	$(LOCAL_DIR)/malloc.o \
	$(LOCAL_DIR)/rand.o \
	$(LOCAL_DIR)/stdio.o \
	$(LOCAL_DIR)/eabi.o

# Without -fno-builtin the compiler might attempt to optimize the libc code by
# replacing loops with calls to libc code, potentially causing infinite loops.
$(BUILDDIR)/$(LOCAL_DIR)/%.o: CFLAGS := $(CFLAGS) -fno-builtin

include $(LOCAL_DIR)/string/rules.mk

ifeq ($(WITH_CPP_SUPPORT),true)
OBJS += \
	$(LOCAL_DIR)/new.o \
	$(LOCAL_DIR)/atexit.o \
	$(LOCAL_DIR)/pure_virtual.o
endif
