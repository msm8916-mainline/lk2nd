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

#ifndef __CRYPTO_HASH_H__
#define __CRYPYO_HASH_H__

#ifndef NULL
#define NULL		0
#endif

#define TRUE		1
#define FALSE		0

#define SHA256_INIT_VECTOR_SIZE		8
#define CRYPTO_SHA_BLOCK_SIZE		64
#define CRYPTO_MAX_AUTH_BLOCK_SIZE	0xFA00

typedef enum {
    CRYPTO_SHA_ERR_NONE,
    CRYPTO_SHA_ERR_BUSY,
    CRYPTO_SHA_ERR_FAIL,
    CRYPTO_SHA_ERR_INVALID_PARAM,
}crypto_result_type;

typedef struct {
    unsigned int auth_bytecnt[2];
    unsigned char saved_buff[64];
    unsigned char saved_buff_indx;
    unsigned int auth_iv[8];
    unsigned char flags;
}crypto_SHA256_ctx;

extern void crypto_init(void);

extern void crypto_set_sha_ctx(crypto_SHA256_ctx *ctx_ptr,
                               unsigned int bytes_to_write,bool first,
                               bool last);

extern void crypto_send_data(crypto_SHA256_ctx *ctx_ptr,
                             unsigned char *data_ptr, unsigned int buff_size,
                             unsigned int bytes_to_write,
                             unsigned int *ret_status);

extern void crypto_get_digest(unsigned char *digest_ptr,
                              unsigned int *ret_status, bool last);

extern void crypto_get_ctx(crypto_SHA256_ctx *ctx_ptr);

static crypto_result_type do_sha(unsigned char *buff_ptr,
                                 unsigned int buff_size,
                                 unsigned char *digest_ptr);

static crypto_result_type do_sha_update(crypto_SHA256_ctx *ctx_ptr,
                                        unsigned char *buff_ptr,
                                        unsigned int buff_size,
                                        bool first, bool last);

static unsigned int calc_num_bytes_to_send(crypto_SHA256_ctx *ctx_ptr,
                                           unsigned int buff_size, bool last);
#endif
