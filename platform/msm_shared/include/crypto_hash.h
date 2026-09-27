/* Copyright (c) 2010-2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CRYPTO_HASH_H__
#define __CRYPTO_HASH_H__

#include <sys/types.h>

#ifndef NULL
#define NULL		0
#endif

#define SHA256_INIT_VECTOR_SIZE		8
#define SHA1_INIT_VECTOR_SIZE		5

#define CRYPTO_SHA_BLOCK_SIZE		64U

#define CRYPTO_ERR_NONE				0x01
#define CRYPTO_ERR_FAIL				0x02

typedef enum {
	CRYPTO_FIRST_CHUNK = 1,
	CRYPTO_LAST_CHUNK  = 2,
} crypto_flag_type;

typedef enum {
	CRYPTO_SHA_ERR_NONE,
	CRYPTO_SHA_ERR_BUSY,
	CRYPTO_SHA_ERR_FAIL,
	CRYPTO_SHA_ERR_INVALID_PARAM,
} crypto_result_type;

typedef enum {
	CRYPTO_ENGINE_TYPE_NONE,
	CRYPTO_ENGINE_TYPE_SW,
	CRYPTO_ENGINE_TYPE_HW,
}crypto_engine_type;

typedef enum {
	CRYPTO_AUTH_ALG_SHA1 = 1,
	CRYPTO_AUTH_ALG_SHA256
} crypto_auth_alg_type;

typedef struct {
	unsigned int auth_bytecnt[2];
	unsigned char saved_buff[64];
	unsigned char saved_buff_indx;
	uint32_t bytes_to_write;
	uint32_t flags;
	unsigned int auth_iv[5];
} crypto_SHA1_ctx;

typedef struct {
	unsigned int auth_bytecnt[2];
	unsigned char saved_buff[64];
	unsigned char saved_buff_indx;
	uint32_t bytes_to_write;
	uint32_t flags;
	unsigned int auth_iv[8];
} crypto_SHA256_ctx;

extern void crypto_eng_reset(void);

extern void crypto_eng_init(void);

extern void crypto_set_sha_ctx(void *ctx_ptr,
			       unsigned int bytes_to_write,
			       crypto_auth_alg_type auth_alg,
			       bool first, bool last);

extern void crypto_send_data(void *ctx_ptr,
			     unsigned char *data_ptr, unsigned int buff_size,
			     unsigned int bytes_to_write,
			     unsigned int *ret_status);

extern void crypto_get_digest(unsigned char *digest_ptr,
			      unsigned int *ret_status,
			      crypto_auth_alg_type auth_alg, bool last);

extern void crypto_get_ctx(void *ctx_ptr);

extern uint32_t crypto_get_max_auth_blk_size(void);

static void crypto_init(void);

static crypto_result_type do_sha(unsigned char *buff_ptr,
				 unsigned int buff_size,
				 unsigned char *digest_ptr,
				 crypto_auth_alg_type auth_alg);

static crypto_result_type do_sha_update(void *ctx_ptr,
					unsigned char *buff_ptr,
					unsigned int buff_size,
					crypto_auth_alg_type auth_alg,
					bool first, bool last);

static unsigned int calc_num_bytes_to_send(void *ctx_ptr,
					   unsigned int buff_size, bool last);

static crypto_result_type crypto_sha256(unsigned char *buff_ptr,
					unsigned int buff_size,
					unsigned char *digest_ptr);

static crypto_result_type crypto_sha1(unsigned char *buff_ptr,
				      unsigned int buff_size,
				      unsigned char *digest_ptr);

bool crypto_initialized(void);
crypto_result_type
hash_find(unsigned char *addr, unsigned int size, unsigned char *digest,
          unsigned char auth_alg);

crypto_engine_type board_ce_type(void);
#endif
