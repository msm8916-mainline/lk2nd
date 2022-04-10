LOCAL_DIR := $(GET_LOCAL_DIR)

ifneq ($(LK1ST_PANEL),)
# Filter out original panel implementation
OBJS := $(filter-out target/$(TARGET)/oem_panel.o, $(OBJS))
MODULES += lk2nd/panel
CFLAGS += -DLK1ST_PANEL=$(LK1ST_PANEL)
else
# Filter out original display implementation
OBJS := $(filter-out target/$(TARGET)/target_display.o target/$(TARGET)/oem_panel.o, $(OBJS))
ifeq ($(DISPLAY_USE_CONTINUOUS_SPLASH),1)
DEFINES += \
	$(if $(filter %/mdp3.o,$(OBJS)), MDP3=1) \
	$(if $(filter %/mdp4.o,$(OBJS)), MDP4=1) \
	$(if $(filter %/mdp5.o,$(OBJS)), MDP5=1)
OBJS += $(LOCAL_DIR)/target_display_cont_splash.o
endif
endif
