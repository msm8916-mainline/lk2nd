# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

# Filter out original display implementation
OBJS := $(filter-out target/$(TARGET)/target_display.o, $(OBJS))

DEFINES += \
	$(if $(filter %/mdp3.o, $(OBJS)), MDP3=1) \
	$(if $(filter %/mdp4.o, $(OBJS)), MDP4=1) \
	$(if $(filter %/mdp5.o, $(OBJS)), MDP5=1) \

OBJS += \
	$(if $(filter MDP3=1 MDP4=1, $(DEFINES)), $(LOCAL_DIR)/dma.o) \
	$(if $(filter MDP4=1 MDP5=1, $(DEFINES)), $(LOCAL_DIR)/pipe.o) \
	$(LOCAL_DIR)/refresh.o \
	$(LOCAL_DIR)/target_display.o \
