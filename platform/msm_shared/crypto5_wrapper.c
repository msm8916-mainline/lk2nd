/* Copyright (c) 2012,2015, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <crypto5_wrapper.h>

/* This file is a wrapper to the crypto5_eng.c.
 * This is required so that we maintian the backward compatibility
 * with the authentication logic in image_verify.c
 */
static struct crypto_dev dev;

void crypto_init_params(struct crypto_init_params * params)
{
	crypto5_init_params(&dev, params);
}

void crypto_eng_cleanup(void)
{
	crypto5_cleanup(&dev);
}

void crypto_unlock(void)
{
	crypto5_unlock_pipes(&dev);
}

void ce_clock_init(void)
{
	/* Clock init is done during crypto_init. */
}

void crypto_eng_reset(void)
{
	/* Reset tied in with the clock init. */
}

void crypto_eng_init(void)
{
	crypto5_init(&dev);
}

void crypto_set_sha_ctx(void *ctx_ptr,
						unsigned int bytes_to_write,
						crypto_auth_alg_type auth_alg,
						bool first,
						bool last)
{
	crypto_SHA256_ctx *sha256_ctx = (crypto_SHA256_ctx *) ctx_ptr;

	sha256_ctx->flags = 0;

	if (first)
		sha256_ctx->flags |= CRYPTO_FIRST_CHUNK;

	if (last)
		sha256_ctx->flags |= CRYPTO_LAST_CHUNK;

	sha256_ctx->bytes_to_write = bytes_to_write;

	crypto5_set_ctx(&dev, ctx_ptr, auth_alg);

	/* Clear the flags. */
	sha256_ctx->flags = 0;
}

void crypto_send_data(void *ctx_ptr,
					  unsigned char *data_ptr,
					  unsigned int buff_size,
					  unsigned int bytes_to_write,
					  unsigned int *ret_status)
{
	*ret_status = crypto5_send_data(&dev, ctx_ptr, data_ptr);
}

void crypto_get_digest(unsigned char *digest_ptr,
					   unsigned int *ret_status,
					   crypto_auth_alg_type auth_alg,
					   bool last)
{
	*ret_status = crypto5_get_digest(&dev, digest_ptr, auth_alg);
}

void crypto_get_ctx(void *ctx_ptr)
{
	crypto5_get_ctx(&dev, ctx_ptr);
}

uint32_t crypto_get_max_auth_blk_size()
{
	return crypto5_get_max_auth_blk_size(&dev);
}
