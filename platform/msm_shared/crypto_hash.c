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
#include "crypto_eng.h"

crypto_result_type crypto_sha256(unsigned char *buff_ptr,
                                 unsigned int buff_size,
                                 unsigned char *digest_ptr);

static unsigned int sha256_init_vector[] = { 0x6A09E667, 0xBB67AE85,
                                             0x3C6EF372, 0xA54FF53A,
                                             0x510E527F, 0x9B05688C,
                                             0x1F83D9AB, 0x5BE0CD19 };

crypto_SHA256_ctx g_sha256_ctx;

void hash_find(unsigned char *addr, unsigned int size, unsigned char *digest)
{
    crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;

    ret_val = crypto_sha256(addr,size,digest);

    if(ret_val != CRYPTO_SHA_ERR_NONE)
    {
        dprintf(CRITICAL, "crypto_sha256 returns error %d\n",ret_val);
    }
}

crypto_result_type crypto_sha256_init(crypto_SHA256_ctx *ctx_ptr)
{
    unsigned int i;

    if(ctx_ptr == NULL)
    {
        return CRYPTO_SHA_ERR_INVALID_PARAM;
    }

    ctx_ptr->auth_bytecnt[0] = 0;
    ctx_ptr->auth_bytecnt[1] = 0;

    memset(ctx_ptr->saved_buff, 0, CRYPTO_SHA_BLOCK_SIZE);

    for(i=0;i<SHA256_INIT_VECTOR_SIZE;i++)
    {
        ctx_ptr->auth_iv[i] = sha256_init_vector[i];
    }

    ctx_ptr->saved_buff_indx = 0;

    return CRYPTO_SHA_ERR_NONE;
}

crypto_result_type crypto_sha256(unsigned char *buff_ptr,
                                 unsigned int buff_size,
                                 unsigned char *digest_ptr)
{
    crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;

    if((!buff_size) || (buff_ptr == NULL) || (digest_ptr == NULL))
    {
        return CRYPTO_SHA_ERR_INVALID_PARAM;
    }

    crypto_init();

    ret_val = do_sha(buff_ptr, buff_size, digest_ptr);

    if(ret_val != CRYPTO_SHA_ERR_NONE)
    {
        dprintf(CRITICAL, "crypto_sha256 returns error %d\n",ret_val);
    }

    return ret_val;
}

static crypto_result_type do_sha(unsigned char *buff_ptr,
                                 unsigned int buff_size,
                                 unsigned char *digest_ptr)
{
    crypto_SHA256_ctx *ctx_ptr=NULL;
    crypto_result_type ret_val = CRYPTO_SHA_ERR_NONE;

    crypto_sha256_init(&g_sha256_ctx);
    ctx_ptr = &g_sha256_ctx;

    ret_val = do_sha_update(ctx_ptr, buff_ptr, buff_size, TRUE, TRUE);

    if(ret_val != CRYPTO_SHA_ERR_NONE)
    {
        dprintf(CRITICAL, "do_sha_update returns error %d\n",ret_val);
        return ret_val;
    }

    memcpy(digest_ptr, (unsigned char *)(ctx_ptr->auth_iv), 32);

    return CRYPTO_SHA_ERR_NONE;
}

static crypto_result_type do_sha_update(crypto_SHA256_ctx *ctx_ptr,
                                        unsigned char *buff_ptr,
                                        unsigned int buff_size,
                                        bool first, bool last)
{
    unsigned int ret_val = CRYPTO_ERR_NONE;
    unsigned int bytes_to_write = 0;
    unsigned int bytes_remaining = 0;
    unsigned int tmp_bytes = 0;
    unsigned int bytes_written = 0;
    unsigned int tmp_buff_size = 0;
    unsigned char *tmp_buff_ptr = NULL;
    unsigned char tmp_saved_buff_indx = NULL;
    bool tmp_first;
    bool tmp_last;

    bytes_to_write = calc_num_bytes_to_send(ctx_ptr, buff_size, last);
    bytes_remaining = buff_size + ctx_ptr->saved_buff_indx - bytes_to_write;

    tmp_first = first;
    tmp_saved_buff_indx = ctx_ptr->saved_buff_indx;

    do
    {
        if((bytes_to_write - bytes_written) > CRYPTO_MAX_AUTH_BLOCK_SIZE)
        {
            tmp_bytes = CRYPTO_MAX_AUTH_BLOCK_SIZE;
            tmp_last = FALSE;

            if(ctx_ptr->saved_buff_indx != 0)
            {
                tmp_buff_ptr = buff_ptr;
                tmp_buff_size = tmp_bytes - ctx_ptr->saved_buff_indx;
            }
            else
            {
                tmp_buff_ptr = buff_ptr + bytes_written - tmp_saved_buff_indx;
                tmp_buff_size = tmp_bytes;
            }
        }
        else
        {
            if(ctx_ptr->saved_buff_indx != 0)
            {
                tmp_buff_ptr = buff_ptr;
                tmp_buff_size = bytes_to_write - bytes_written - ctx_ptr->saved_buff_indx;
            }
            else
            {
                tmp_buff_ptr = buff_ptr + bytes_written - tmp_saved_buff_indx;
                tmp_buff_size = bytes_to_write - bytes_written - tmp_saved_buff_indx;
            }

            tmp_bytes = (bytes_to_write - bytes_written);
            tmp_last = last;
        }

        crypto_set_sha_ctx(ctx_ptr, tmp_bytes, tmp_first, tmp_last);

        crypto_send_data(ctx_ptr, tmp_buff_ptr, tmp_buff_size, tmp_bytes, &ret_val);

        if(ret_val != CRYPTO_ERR_NONE)
        {
            return CRYPTO_SHA_ERR_FAIL;
        }

        crypto_get_digest((unsigned char *)(ctx_ptr->auth_iv), &ret_val, tmp_last);

        if(ret_val != CRYPTO_ERR_NONE)
        {
            return CRYPTO_SHA_ERR_FAIL;
        }

        if(!tmp_last)
        {
            crypto_get_ctx(ctx_ptr);
        }

        bytes_written += tmp_bytes;
        ctx_ptr->saved_buff_indx = 0;

        if(bytes_written != bytes_to_write)
        {
            tmp_first = FALSE;
        }

    }while ((bytes_to_write - bytes_written) != 0);

    if(bytes_remaining)
    {
        memcpy(ctx_ptr->saved_buff, (buff_ptr + buff_size - bytes_remaining),
               bytes_remaining);
        ctx_ptr->saved_buff_indx = bytes_remaining;
    }
    else
    {
        ctx_ptr->saved_buff_indx = 0;
    }

    return CRYPTO_SHA_ERR_NONE;
}

static unsigned int calc_num_bytes_to_send(crypto_SHA256_ctx *ctx_ptr,
                                           unsigned int buff_size, bool last)
{
    unsigned int bytes_to_write=0;

    if(last)
    {
        bytes_to_write = buff_size + ctx_ptr->saved_buff_indx;
    }
    else
    {
        bytes_to_write = ((buff_size + ctx_ptr->saved_buff_indx)/
                           CRYPTO_SHA_BLOCK_SIZE) * CRYPTO_SHA_BLOCK_SIZE;
    }
    return bytes_to_write;
}
