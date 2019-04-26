LOCAL_DIR := $(GET_LOCAL_DIR)

# Filter out original panel implementation
OBJS := $(filter-out target/$(TARGET)/oem_panel.o, $(OBJS))

ifeq ($(LK2ND_DISPLAY), cont-splash)
# Filter out original display implementation as well
OBJS := $(filter-out target/$(TARGET)/target_display.o, $(OBJS))
DEFINES += \
	$(if $(filter %/mdp3.o,$(OBJS)), MDP3=1) \
	$(if $(filter %/mdp4.o,$(OBJS)), MDP4=1) \
	$(if $(filter %/mdp5.o,$(OBJS)), MDP5=1)
OBJS += $(LOCAL_DIR)/target_display_cont_splash.o
else
$(error Display '$(LK2ND_DISPLAY)' is not supported yet)
endif
