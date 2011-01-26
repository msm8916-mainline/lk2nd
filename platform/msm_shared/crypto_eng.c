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
#include <endian.h>
#include <reg.h>
#include <platform/iomap.h>
#include "crypto_eng.h"
#include "crypto_hash.h"

unsigned char crypto_init_done;

void crypto_eng_reset(void)
{
    wr_ce(SW_RST,CRYPTO3_CONFIG);
}

void crypto_eng_init(void)
{
    unsigned int val;
    val = (AUTO_SHUTDOWN_EN | MASK_ERR_INTR | MASK_AUTH_DONE_INTR | MASK_DIN_INTR
          | MASK_DOUT_INTR | HIGH_SPD_IN_EN_N | HIGH_SPD_OUT_EN_N | HIGH_SPD_HASH_EN_N);

    wr_ce(val,CRYPTO3_CONFIG);
}

void crypto_init(void)
{
    if(crypto_init_done != TRUE)
    {
         crypto_eng_reset();
         crypto_init_done = TRUE;
    }
    crypto_eng_init();
}

void crypto_set_sha_ctx(crypto_SHA256_ctx *ctx_ptr, unsigned int bytes_to_write,
                        bool first, bool last)
{
    unsigned int i=0;
    unsigned int iv_len=0;
    unsigned int *auth_iv;
    unsigned int seg_cfg_val;

    seg_cfg_val = SEG_CFG_AUTH_ALG_SHA | SEG_CFG_AUTH_SIZE_SHA256;

    if((first) || ((ctx_ptr->saved_buff_indx != 0) &&
       (ctx_ptr->auth_bytecnt[0] != 0 || ctx_ptr->auth_bytecnt[1] != 0)))
    {
        seg_cfg_val |= SEG_CFG_FIRST;
    }
    if(last)
    {
        seg_cfg_val |= SEG_CFG_LAST;
    }

    iv_len = SHA256_INIT_VECTOR_SIZE;
    auth_iv = ctx_ptr->auth_iv;

    for(i=0; i<iv_len; i++)
    {
        wr_ce(*(auth_iv+i),CRYPTO3_AUTH_IVn(i));
    }

    wr_ce(seg_cfg_val,CRYPTO3_SEG_CFG);
    wr_ce(ctx_ptr->auth_bytecnt[0],CRYPTO3_AUTH_BYTECNTn(0));
    wr_ce(ctx_ptr->auth_bytecnt[1],CRYPTO3_AUTH_BYTECNTn(1));
    wr_ce((bytes_to_write << AUTH_SEG_CFG_AUTH_SIZE),CRYPTO3_AUTH_SEG_CFG);
    wr_ce(bytes_to_write,CRYPTO3_SEG_SIZE);
    wr_ce(GOPROC_GO,CRYPTO3_GOPROC);

    return;
}

void crypto_send_data(crypto_SHA256_ctx *ctx_ptr, unsigned char *data_ptr,
                      unsigned int buff_size, unsigned int bytes_to_write,
                      unsigned int *ret_status)
{
    unsigned int bytes_left=0;
    unsigned int i=0;
    unsigned int ce_status=0;
    unsigned int ce_err_bmsk=0;
    unsigned int is_not_aligned=FALSE;
    unsigned char data[4];
    unsigned char *buff_ptr=data_ptr;

    if(!(IS_ALIGNED(buff_ptr)))
    {
        is_not_aligned = TRUE;
    }

    if(ctx_ptr->saved_buff_indx != 0)
    {
        memcpy(ctx_ptr->saved_buff + ctx_ptr->saved_buff_indx, buff_ptr,
               (((buff_size + ctx_ptr->saved_buff_indx) <= CRYPTO_SHA_BLOCK_SIZE)
               ? buff_size : (CRYPTO_SHA_BLOCK_SIZE - ctx_ptr->saved_buff_indx)));
        if(bytes_to_write >= CRYPTO_SHA_BLOCK_SIZE)
        {
            bytes_left = CRYPTO_SHA_BLOCK_SIZE;
        }
        else
        {
            bytes_left = bytes_to_write;
        }
    }
    else
    {
        bytes_left = bytes_to_write;
    }

    ce_err_bmsk = (SW_ERR | DIN_RDY | DIN_SIZE_AVAIL);

    while(bytes_left >= 4)
    {
        ce_status = rd_ce(CRYPTO3_STATUS);
        ce_status &= ce_err_bmsk;

        if(ce_status & SW_ERR)
        {
            crypto_eng_reset();
            *ret_status = CRYPTO_ERR_FAIL;
            return;
        }
        if((ce_status & DIN_RDY) && ((ce_status & DIN_SIZE_AVAIL) >= 4))
        {
            if(ctx_ptr->saved_buff_indx != 0)
            {
                wr_ce(htonl(*((unsigned int *)(ctx_ptr->saved_buff)+i)),CRYPTO3_DATA_IN);
            }
            else
            {
                if(!is_not_aligned)
                {
                    wr_ce(htonl(*((unsigned int *)buff_ptr+i)),CRYPTO3_DATA_IN);
                }
                else
                {
                    data[0] = *(buff_ptr+i);
                    data[1] = *(buff_ptr+i+1);
                    data[2] = *(buff_ptr+i+2);
                    data[3] = *(buff_ptr+i+3);
                    i+=3;
                    wr_ce(htonl(*(unsigned int *)data),CRYPTO3_DATA_IN);
                    memset(data,0,4);
                }
            }
            i++;
            bytes_left -=4;

            if((ctx_ptr->saved_buff_indx != 0) && (bytes_left == 0) &&
               (bytes_to_write > CRYPTO_SHA_BLOCK_SIZE))
            {
                bytes_left = (bytes_to_write - CRYPTO_SHA_BLOCK_SIZE);
                buff_ptr = (unsigned char *)((unsigned char *)data_ptr +
                           CRYPTO_SHA_BLOCK_SIZE - ctx_ptr->saved_buff_indx);
                i = 0;
                ctx_ptr->saved_buff_indx = 0;
                if(!(IS_ALIGNED(buff_ptr)))
                {
                    is_not_aligned = TRUE;
                }
            }
        }
    }

    if(bytes_left)
    {
        memset(data,0,4);

        if(ctx_ptr->saved_buff_indx)
        {
            data[4-bytes_left] = *(ctx_ptr->saved_buff + bytes_to_write -
                                   bytes_left +(bytes_left - 1));
        }
        else
        {
            data[4-bytes_left] = *(((unsigned char *)data_ptr) + buff_size -
                                   bytes_left +(bytes_left - 1));
        }

        ce_status = rd_ce(CRYPTO3_STATUS);
        ce_status &= ce_err_bmsk;

        if(ce_status & SW_ERR)
        {
            crypto_eng_reset();
            *ret_status = CRYPTO_ERR_FAIL;
            return;
        }
        if((ce_status & DIN_RDY) && ((ce_status & DIN_SIZE_AVAIL) >= 4))
        {
            wr_ce(*(unsigned int *)data,CRYPTO3_DATA_IN);
        }
    }
    *ret_status = CRYPTO_ERR_NONE;
    return;
}

void crypto_get_digest(unsigned char *digest_ptr, unsigned int *ret_status,
                       bool last)
{
    unsigned int ce_status=0;
    unsigned int ce_err_bmsk = (AUTH_DONE | SW_ERR);
    unsigned int i=0;
    unsigned int digest_len = SHA256_INIT_VECTOR_SIZE;

    do
    {
        ce_status = rd_ce(CRYPTO3_STATUS);
        ce_status &= ce_err_bmsk;
    }while (ce_status == 0);

    if(ce_status & SW_ERR)
    {
        crypto_eng_reset();
        *ret_status = CRYPTO_ERR_FAIL;
        return;
    }

    for(i=0; i < digest_len;i++)
    {
        unsigned int auth_iv = rd_ce(CRYPTO3_AUTH_IVn(i));

        if(last)
        {
            *((unsigned int *)digest_ptr + i) = htonl(auth_iv);
        }
        else
        {
            *((unsigned int *)digest_ptr + i) = auth_iv;
        }
    }
    *ret_status = CRYPTO_ERR_NONE;
    return;
}

void crypto_get_ctx(crypto_SHA256_ctx *ctx_ptr)
{
    ctx_ptr->auth_bytecnt[0] = rd_ce(CRYPTO3_AUTH_BYTECNTn(0));
    ctx_ptr->auth_bytecnt[1] = rd_ce(CRYPTO3_AUTH_BYTECNTn(1));
    return;
}
