/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <string.h>
#include <debug.h>
#include <sys/types.h>
#include "crypto_hash.h"

static crypto_SHA256_ctx g_sha256_ctx;
static crypto_SHA1_ctx g_sha1_ctx;
static unsigned char crypto_init_done = FALSE;

extern void ce_clock_init(void);

__WEAK void crypto_eng_cleanup()
{
}

/*
 * Top level function which calculates SHAx digest with given data and size.
 * Digest varies based on the authentication algorithm.
 * It works on contiguous data and does single pass calculation.
 */

void
hash_find(unsigned char *addr, unsigned int size, unsigned char *digest,
	  unsigned char auth_alg)
{
	crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;
	crypto_engine_type platform_ce_type = board_ce_type();

	if (auth_alg == 1) {
		if(platform_ce_type == CRYPTO_ENGINE_TYPE_SW)
			/* Hardware CE is not present , use software hashing */
			digest = SHA1(addr, size, digest);
		else if (platform_ce_type == CRYPTO_ENGINE_TYPE_HW)
			ret_val = crypto_sha1(addr, size, digest);
		else
			ret_val = CRYPTO_SHA_ERR_FAIL;
	} else if (auth_alg == 2) {
		if(platform_ce_type == CRYPTO_ENGINE_TYPE_SW)
			/* Hardware CE is not present , use software hashing */
			digest = SHA256(addr, size, digest);
		else if (platform_ce_type == CRYPTO_ENGINE_TYPE_HW)
			ret_val = crypto_sha256(addr, size, digest);
		else
		ret_val = CRYPTO_SHA_ERR_FAIL;
	}

	if (ret_val != CRYPTO_SHA_ERR_NONE) {
		dprintf(CRITICAL, "crypto_sha256 returns error %d\n", ret_val);
	}

	crypto_eng_cleanup();

}

/*
 * Function to reset and init crypto engine. It resets the engine for the
 * first time. Used for multiple SHA operations.
 */

static void crypto_init(void)
{
	if (crypto_init_done != TRUE) {
		ce_clock_init();
		crypto_eng_reset();
		crypto_init_done = TRUE;
	}
	crypto_eng_init();
}

/*
 * Function to initialize SHA256 context
 */

static crypto_result_type crypto_sha256_init(crypto_SHA256_ctx * ctx_ptr)
{
	unsigned int i;
	/* Standard initialization vector for SHA256 */
	unsigned int sha256_init_vector[] = { 0x6A09E667, 0xBB67AE85,
		0x3C6EF372, 0xA54FF53A,
		0x510E527F, 0x9B05688C,
		0x1F83D9AB, 0x5BE0CD19
	};

	if (ctx_ptr == NULL) {
		return CRYPTO_SHA_ERR_INVALID_PARAM;
	}

	ctx_ptr->auth_bytecnt[0] = 0;
	ctx_ptr->auth_bytecnt[1] = 0;

	memset(ctx_ptr->saved_buff, 0, CRYPTO_SHA_BLOCK_SIZE);

	for (i = 0; i < SHA256_INIT_VECTOR_SIZE; i++) {
		ctx_ptr->auth_iv[i] = sha256_init_vector[i];
	}

	ctx_ptr->saved_buff_indx = 0;

	return CRYPTO_SHA_ERR_NONE;
}

/*
 * Function to initialize SHA1 context
 */

static crypto_result_type crypto_sha1_init(crypto_SHA1_ctx * ctx_ptr)
{
	unsigned int i;
	/* Standard initialization vector for SHA1 */
	unsigned int sha1_init_vector[] = { 0x67452301, 0xEFCDAB89,
		0x98BADCFE, 0x10325476,
		0xC3D2E1F0
	};

	if (ctx_ptr == NULL) {
		return CRYPTO_SHA_ERR_INVALID_PARAM;
	}

	ctx_ptr->auth_bytecnt[0] = 0;
	ctx_ptr->auth_bytecnt[1] = 0;

	memset(ctx_ptr->saved_buff, 0, CRYPTO_SHA_BLOCK_SIZE);

	for (i = 0; i < SHA1_INIT_VECTOR_SIZE; i++) {
		ctx_ptr->auth_iv[i] = sha1_init_vector[i];
	}

	ctx_ptr->saved_buff_indx = 0;

	return CRYPTO_SHA_ERR_NONE;
}

/*
 * Function to calculate SHA256 digest of given data buffer.
 * It works on contiguous data and gives digest in single pass.
 */

static crypto_result_type
crypto_sha256(unsigned char *buff_ptr,
	      unsigned int buff_size, unsigned char *digest_ptr)
{
	crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;

	if ((!buff_size) || (buff_ptr == NULL) || (digest_ptr == NULL)) {
		return CRYPTO_SHA_ERR_INVALID_PARAM;
	}

	/* Initialize crypto engine hardware for a new SHA256 operation */
	crypto_init();

	/* Now do SHA256 hashing */
	ret_val =
	    do_sha(buff_ptr, buff_size, digest_ptr, CRYPTO_AUTH_ALG_SHA256);

	if (ret_val != CRYPTO_SHA_ERR_NONE) {
		dprintf(CRITICAL, "crypto_sha256 returns error %d\n", ret_val);
	}

	return ret_val;
}

/*
 * Function to calculate SHA1 digest of given data buffer.
 * It works on contiguous data and gives digest in single pass.
 */

static crypto_result_type
crypto_sha1(unsigned char *buff_ptr,
	    unsigned int buff_size, unsigned char *digest_ptr)
{
	crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;

	if ((!buff_size) || (buff_ptr == NULL) || (digest_ptr == NULL)) {
		return CRYPTO_SHA_ERR_INVALID_PARAM;
	}

	/* Initialize crypto engine hardware for a new SHA1 operation */
	crypto_init();

	/* Now do SHA1 hashing */
	ret_val = do_sha(buff_ptr, buff_size, digest_ptr, CRYPTO_AUTH_ALG_SHA1);

	if (ret_val != CRYPTO_SHA_ERR_NONE) {
		dprintf(CRITICAL, "crypto_sha256 returns error %d\n", ret_val);
	}

	return ret_val;
}

/*
 * Common function to calculate SHA1 and SHA256 digest based on auth algorithm.
 */

static crypto_result_type
do_sha(unsigned char *buff_ptr,
       unsigned int buff_size,
       unsigned char *digest_ptr, crypto_auth_alg_type auth_alg)
{
	void *ctx_ptr = NULL;
	crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;

	/* Initialize SHA context based on algorithm */
	if (auth_alg == CRYPTO_AUTH_ALG_SHA1) {
		crypto_sha1_init(&g_sha1_ctx);
		ctx_ptr = (void *)&g_sha1_ctx;
	} else if (auth_alg == CRYPTO_AUTH_ALG_SHA256) {
		crypto_sha256_init(&g_sha256_ctx);
		ctx_ptr = (void *)&g_sha256_ctx;
	}

	ret_val =
	    do_sha_update(ctx_ptr, buff_ptr, buff_size, auth_alg, TRUE, TRUE);

	if (ret_val != CRYPTO_SHA_ERR_NONE) {
		dprintf(CRITICAL, "do_sha_update returns error %d\n", ret_val);
		return ret_val;
	}

	/* Copy the digest value from context pointer to digest pointer */
	if (auth_alg == CRYPTO_AUTH_ALG_SHA1) {
		memcpy(digest_ptr,
		       (unsigned char *)(((crypto_SHA1_ctx *) ctx_ptr)->
					 auth_iv), 20);
	} else if (auth_alg == CRYPTO_AUTH_ALG_SHA256) {
		memcpy(digest_ptr,
		       (unsigned char *)(((crypto_SHA256_ctx *) ctx_ptr)->
					 auth_iv), 32);
	}

	return CRYPTO_SHA_ERR_NONE;
}

/*
 * Common function to calculate SHA1 and SHA256 digest based on auth algorithm.
 * Calls crypto engine APIs to setup SHAx registers, send the data and gets
 * the digest.
 */

static crypto_result_type
do_sha_update(void *ctx_ptr,
	      unsigned char *buff_ptr,
	      unsigned int buff_size,
	      crypto_auth_alg_type auth_alg, bool first, bool last)
{
	unsigned int ret_val = CRYPTO_ERR_NONE;
	unsigned int bytes_to_write = 0;
	unsigned int bytes_remaining = 0;
	unsigned int tmp_bytes = 0;
	unsigned int bytes_written = 0;
	unsigned int tmp_buff_size = 0;
	unsigned char *tmp_buff_ptr = NULL;
	unsigned char tmp_saved_buff_indx = 0;
	bool tmp_first;
	bool tmp_last;

	/* Type casting to SHA1 context as offset is similar for SHA256 context */
	crypto_SHA1_ctx *sha1_ctx = (crypto_SHA1_ctx *) ctx_ptr;

	bytes_to_write = calc_num_bytes_to_send(ctx_ptr, buff_size, last);
	bytes_remaining =
	    buff_size + sha1_ctx->saved_buff_indx - bytes_to_write;

	tmp_first = first;
	tmp_saved_buff_indx = sha1_ctx->saved_buff_indx;

	do {
		if ((bytes_to_write - bytes_written) >
		    CRYPTO_MAX_AUTH_BLOCK_SIZE) {
			/* Write CRYPTO_MAX_AUTH_BLOCK_SIZE bytes at a time to the CE */
			tmp_bytes = CRYPTO_MAX_AUTH_BLOCK_SIZE;
			tmp_last = FALSE;

			if (sha1_ctx->saved_buff_indx != 0) {
				tmp_buff_ptr = buff_ptr;
				tmp_buff_size =
				    tmp_bytes - sha1_ctx->saved_buff_indx;
			} else {
				tmp_buff_ptr =
				    buff_ptr + bytes_written -
				    tmp_saved_buff_indx;
				tmp_buff_size = tmp_bytes;
			}
		} else {
			/* Since bytes_to_write are less than CRYPTO_MAX_AUTH_BLOCK_SIZE
			   write all remaining bytes now */
			if (sha1_ctx->saved_buff_indx != 0) {
				tmp_buff_ptr = buff_ptr;
				tmp_buff_size =
				    bytes_to_write - bytes_written -
				    sha1_ctx->saved_buff_indx;
			} else {
				tmp_buff_ptr =
				    buff_ptr + bytes_written -
				    tmp_saved_buff_indx;
				tmp_buff_size =
				    bytes_to_write - bytes_written -
				    tmp_saved_buff_indx;
			}

			tmp_bytes = (bytes_to_write - bytes_written);
			tmp_last = last;
		}

		/* Set SHAx context in the crypto engine */
		crypto_set_sha_ctx(ctx_ptr, tmp_bytes, auth_alg, tmp_first,
				   tmp_last);

		/* Send data to the crypto engine */
		crypto_send_data(ctx_ptr, tmp_buff_ptr, tmp_buff_size,
				 tmp_bytes, &ret_val);

		if (ret_val != CRYPTO_ERR_NONE) {
			dprintf(CRITICAL,
				"do_sha_update returns error from crypto_send_data\n");
			return CRYPTO_SHA_ERR_FAIL;
		}

		/* Get the SHAx digest from the crypto engine */
		crypto_get_digest((unsigned char *)(sha1_ctx->auth_iv),
				  &ret_val, auth_alg, tmp_last);

		if (ret_val != CRYPTO_ERR_NONE) {
			dprintf(CRITICAL,
				"do_sha_update returns error from crypto_get_digest\n");
			return CRYPTO_SHA_ERR_FAIL;
		}

		if (!tmp_last) {
			crypto_get_ctx(ctx_ptr);
		}

		bytes_written += tmp_bytes;
		sha1_ctx->saved_buff_indx = 0;

		if (bytes_written != bytes_to_write) {
			tmp_first = FALSE;
		}

	}
	while ((bytes_to_write - bytes_written) != 0);

	/* If there are bytes remaining, copy it to saved_buff */

	if (bytes_remaining) {
		memcpy(sha1_ctx->saved_buff,
		       (buff_ptr + buff_size - bytes_remaining),
		       bytes_remaining);
		sha1_ctx->saved_buff_indx = bytes_remaining;
	} else {
		sha1_ctx->saved_buff_indx = 0;
	}

	return CRYPTO_SHA_ERR_NONE;
}

/*
 * Function to calculate the number of bytes to be sent to crypto engine.
 */

static unsigned int
calc_num_bytes_to_send(void *ctx_ptr, unsigned int buff_size, bool last)
{
	unsigned int bytes_to_write = 0;
	crypto_SHA1_ctx *sha1_ctx = (crypto_SHA1_ctx *) ctx_ptr;

	if (last) {
		bytes_to_write = buff_size + sha1_ctx->saved_buff_indx;
	} else {
		bytes_to_write = ((buff_size + sha1_ctx->saved_buff_indx) /
				  CRYPTO_SHA_BLOCK_SIZE) *
		    CRYPTO_SHA_BLOCK_SIZE;
	}
	return bytes_to_write;
}
