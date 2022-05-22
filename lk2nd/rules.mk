LOCAL_DIR := $(GET_LOCAL_DIR)

ifeq ($(CRYPTO_BACKEND), openssl)
$(warning WARNING: OpenSSL License is not compatible with \
	  GPL-2.0-only of the lk2nd module)
endif
ifeq ($(DTBO_BACKEND), libufdt)
$(warning WARNING: Apache-2.0 license of libufdt is not compatible with \
	  GPL-2.0-only of the lk2nd module)
endif
