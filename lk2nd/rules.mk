LOCAL_DIR := $(GET_LOCAL_DIR)

OBJS += \
	$(LOCAL_DIR)/lk2nd-device.o \
	$(LOCAL_DIR)/lk2nd-fdt.o \
	$(LOCAL_DIR)/lk2nd-motorola.o \
	$(LOCAL_DIR)/lk2nd-rproc.o \
	$(LOCAL_DIR)/lk2nd-smd-rpm.o \
	$(LOCAL_DIR)/target_keys_lk2nd.o

ifneq ($(GPIO_I2C_BUS_COUNT),)
MODULES += lk2nd/regmap
OBJS += $(LOCAL_DIR)/lk2nd-samsung.o
endif

ifeq ($(TARGET),msm8916)
MODULES += lk2nd/smb1360
endif

ifneq ($(SMP_SPIN_TABLE_BASE),)
MODULES += lk2nd/smp
DEFINES += SMP_SPIN_TABLE_BASE=$(SMP_SPIN_TABLE_BASE)
endif

ifneq ($(LK1ST_PANEL),)
# Filter out original panel implementation
OBJS := $(filter-out target/$(TARGET)/oem_panel.o, $(OBJS))
MODULES += lk2nd/panel
CFLAGS += -DLK1ST_PANEL=$(LK1ST_PANEL)
else ifeq ($(DISPLAY_USE_CONTINUOUS_SPLASH),1)
# Filter out original display implementation
OBJS := $(filter-out target/$(TARGET)/target_display.o target/$(TARGET)/oem_panel.o, $(OBJS))
ifneq ($(filter $(DEFINES),DISPLAY_TYPE_MDSS=1),)
    OBJS += $(LOCAL_DIR)/target_display_cont_splash_mdp5.o
else
    $(error Continuous splash display is not supported for the current target)
endif
endif
