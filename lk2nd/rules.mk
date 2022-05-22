# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include

GPL ?= 0
DEFINES += GPL=$(GPL)
ifeq ($(GPL), 1)
BUILD_GPL := 1
ifeq ($(CRYPTO_BACKEND), openssl)
$(warning WARNING: OpenSSL License is not compatible with \
	  GPL-2.0-only used for some parts of the lk2nd module. \
	  Set GPL=0 to exclude GPL-licensed code.)
endif
ifeq ($(DTBO_BACKEND), libufdt)
$(warning WARNING: Apache-2.0 license of libufdt is not compatible with \
	  GPL-2.0-only used for some parts of the lk2nd module. \
	  Set GPL=0 to exclude GPL-licensed code.)
endif
endif
