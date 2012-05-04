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
#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include "crypto_eng.h"
#include "crypto_hash.h"

/*
 * Function to reset the crypto engine.
 */

void crypto_eng_reset(void)
{
#if ASYNC_RESET_CE
	ce_async_reset();
#else
	wr_ce(SW_RST, CRYPTO3_CONFIG);
	dmb();
#endif
	return;
}

/*
 * Function to initialize the crypto engine for a new session. It enables the
 * auto shutdown feature of CRYPTO3 and mask various interrupts since we use
 * polling. We are not using DMOV now.
 */

void crypto_eng_init(void)
{
	unsigned int val;
	val = (AUTO_SHUTDOWN_EN | MASK_ERR_INTR | MASK_AUTH_DONE_INTR |
	       MASK_DIN_INTR | MASK_DOUT_INTR | HIGH_SPD_IN_EN_N |
	       HIGH_SPD_OUT_EN_N | HIGH_SPD_HASH_EN_N);

	wr_ce(val, CRYPTO3_CONFIG);
}

/*
 * Function to set various SHAx registers in CRYPTO3 based on algorithm type.
 */

void
crypto_set_sha_ctx(void *ctx_ptr, unsigned int bytes_to_write,
		   crypto_auth_alg_type auth_alg, bool first, bool last)
{
	crypto_SHA1_ctx *sha1_ctx = (crypto_SHA1_ctx *) ctx_ptr;
	crypto_SHA256_ctx *sha256_ctx = (crypto_SHA256_ctx *) ctx_ptr;
	unsigned int i = 0;
	unsigned int iv_len = 0;
	unsigned int *auth_iv;
	unsigned int seg_cfg_val;

	seg_cfg_val = SEG_CFG_AUTH_ALG_SHA;

	if (auth_alg == CRYPTO_AUTH_ALG_SHA1) {
		seg_cfg_val |= SEG_CFG_AUTH_SIZE_SHA1;

		if ((first) || ((sha1_ctx->saved_buff_indx != 0) &&
				(sha1_ctx->auth_bytecnt[0] != 0
				 || sha1_ctx->auth_bytecnt[1] != 0))) {
			seg_cfg_val |= SEG_CFG_FIRST;
		}
		if (last) {
			seg_cfg_val |= SEG_CFG_LAST;
		}

		iv_len = SHA1_INIT_VECTOR_SIZE;
		auth_iv = sha1_ctx->auth_iv;
	} else if (auth_alg == CRYPTO_AUTH_ALG_SHA256) {
		seg_cfg_val |= SEG_CFG_AUTH_SIZE_SHA256;

		if ((first) || ((sha256_ctx->saved_buff_indx != 0) &&
				(sha256_ctx->auth_bytecnt[0] != 0
				 || sha256_ctx->auth_bytecnt[1] != 0))) {
			seg_cfg_val |= SEG_CFG_FIRST;
		}
		if (last) {
			seg_cfg_val |= SEG_CFG_LAST;
		}

		iv_len = SHA256_INIT_VECTOR_SIZE;
		auth_iv = sha256_ctx->auth_iv;
	} else {
		dprintf(CRITICAL,
			"crypto_set_sha_ctx invalid auth algorithm\n");
		return;
	}

	for (i = 0; i < iv_len; i++) {
		wr_ce(*(auth_iv + i), CRYPTO3_AUTH_IVn(i));
	}

	wr_ce(seg_cfg_val, CRYPTO3_SEG_CFG);

	/* Typecast with crypto_SHA1_ctx because offset of auth_bytecnt in both
	   crypto_SHA1_ctx and crypto_SHA256_ctx are same */

	wr_ce(((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[0],
	      CRYPTO3_AUTH_BYTECNTn(0));
	wr_ce(((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[1],
	      CRYPTO3_AUTH_BYTECNTn(1));

	wr_ce((bytes_to_write << AUTH_SEG_CFG_AUTH_SIZE), CRYPTO3_AUTH_SEG_CFG);
	wr_ce(bytes_to_write, CRYPTO3_SEG_SIZE);
	wr_ce(GOPROC_GO, CRYPTO3_GOPROC);

	return;
}

/*
 * Function to send data to CRYPTO3. This is non-DMOV implementation and uses
 * polling to send the requested amount of data.
 */

void
crypto_send_data(void *ctx_ptr, unsigned char *data_ptr,
		 unsigned int buff_size, unsigned int bytes_to_write,
		 unsigned int *ret_status)
{
	crypto_SHA1_ctx *sha1_ctx = (crypto_SHA1_ctx *) ctx_ptr;
	unsigned int bytes_left = 0;
	unsigned int i = 0;
	unsigned int ce_status = 0;
	unsigned int ce_err_bmsk = 0;
	unsigned int is_not_aligned = FALSE;
	unsigned char data[4];
	unsigned char *buff_ptr = data_ptr;

	/* Check if the buff_ptr is aligned */
	if (!(IS_ALIGNED(buff_ptr))) {
		is_not_aligned = TRUE;
	}

	/* Fill the saved_buff with data from buff_ptr. First we have to write
	   all the data from the saved_buff and then we will write data from
	   buff_ptr. We will update bytes_left and buff_ptr in the while loop
	   once are done writing all the data from saved_buff. */

	if (sha1_ctx->saved_buff_indx != 0) {
		memcpy(sha1_ctx->saved_buff + sha1_ctx->saved_buff_indx,
		       buff_ptr,
		       (((buff_size + sha1_ctx->saved_buff_indx) <=
			 CRYPTO_SHA_BLOCK_SIZE)
			? buff_size : (CRYPTO_SHA_BLOCK_SIZE -
				       sha1_ctx->saved_buff_indx)));

		if (bytes_to_write >= CRYPTO_SHA_BLOCK_SIZE) {
			bytes_left = CRYPTO_SHA_BLOCK_SIZE;
		} else {
			bytes_left = bytes_to_write;
		}
	} else {
		bytes_left = bytes_to_write;
	}

	/* Error bitmask to check crypto engine status */
	ce_err_bmsk = (SW_ERR | DIN_RDY | DIN_SIZE_AVAIL);

	while (bytes_left >= 4) {
		ce_status = rd_ce(CRYPTO3_STATUS);
		ce_status &= ce_err_bmsk;

		if (ce_status & SW_ERR) {
			/* If there is SW_ERR, reset the engine */
			crypto_eng_reset();
			*ret_status = CRYPTO_ERR_FAIL;
			dprintf(CRITICAL, "crypto_send_data sw error\n");
			return;
		}

		/* We can write data now - 4 bytes at a time in network byte order */
		if ((ce_status & DIN_RDY)
		    && ((ce_status & DIN_SIZE_AVAIL) >= 4)) {
			if (sha1_ctx->saved_buff_indx != 0) {
				/* Write from saved_buff */
				wr_ce(htonl
				      (*
				       ((unsigned int *)(sha1_ctx->saved_buff) +
					i)), CRYPTO3_DATA_IN);
			} else {
				if (!is_not_aligned) {
					/* Write from buff_ptr aligned */
					wr_ce(htonl
					      (*((unsigned int *)buff_ptr + i)),
					      CRYPTO3_DATA_IN);
				} else {
					/* If buff_ptr is not aligned write byte by byte */
					data[0] = *(buff_ptr + i);
					data[1] = *(buff_ptr + i + 1);
					data[2] = *(buff_ptr + i + 2);
					data[3] = *(buff_ptr + i + 3);
					/* i will incremented by 1 in outside block */
					i += 3;
					wr_ce(htonl(*(unsigned int *)data),
					      CRYPTO3_DATA_IN);
					memset(data, 0, 4);
				}
			}
			i++;
			bytes_left -= 4;

			/* Check if we have written from saved_buff. Adjust buff_ptr and
			   bytes_left accordingly */
			if ((sha1_ctx->saved_buff_indx != 0)
			    && (bytes_left == 0)
			    && (bytes_to_write > CRYPTO_SHA_BLOCK_SIZE)) {
				bytes_left =
				    (bytes_to_write - CRYPTO_SHA_BLOCK_SIZE);
				buff_ptr =
				    (unsigned char *)((unsigned char *)data_ptr
						      + CRYPTO_SHA_BLOCK_SIZE -
						      sha1_ctx->
						      saved_buff_indx);
				i = 0;
				sha1_ctx->saved_buff_indx = 0;
				if (!(IS_ALIGNED(buff_ptr))) {
					is_not_aligned = TRUE;
				}
			}
		}
	}

	/* We might have bytes_left < 4. Write them now if available */
	if (bytes_left) {
		memset(data, 0, sizeof(unsigned int));

		if (sha1_ctx->saved_buff_indx)
			buff_ptr = (sha1_ctx->saved_buff + bytes_to_write - 1);
		else
			buff_ptr =
			    (((unsigned char *)data_ptr) + buff_size - 1);

		for (i = 0; i < bytes_left; i++) {
			data[3 - i] = *(buff_ptr - bytes_left + i + 1);
		}

		ce_status = rd_ce(CRYPTO3_STATUS);
		ce_status &= ce_err_bmsk;

		if (ce_status & SW_ERR) {
			crypto_eng_reset();
			*ret_status = CRYPTO_ERR_FAIL;
			dprintf(CRITICAL, "crypto_send_data sw error 2\n");
			return;
		}
		if ((ce_status & DIN_RDY)
		    && ((ce_status & DIN_SIZE_AVAIL) >= 4)) {
			wr_ce(*(unsigned int *)data, CRYPTO3_DATA_IN);
		}
	}
	*ret_status = CRYPTO_ERR_NONE;
	return;
}

/*
 * Function to get digest from CRYPTO3. We poll for AUTH_DONE from CRYPTO3.
 */

void
crypto_get_digest(unsigned char *digest_ptr, unsigned int *ret_status,
		  crypto_auth_alg_type auth_alg, bool last)
{
	unsigned int ce_status = 0;
	unsigned int ce_err_bmsk = (AUTH_DONE | SW_ERR);
	unsigned int i = 0;
	unsigned int digest_len = 0;

	do {
		ce_status = rd_ce(CRYPTO3_STATUS);
		ce_status &= ce_err_bmsk;
	}
	while (ce_status == 0);

	if (ce_status & SW_ERR) {
		crypto_eng_reset();
		*ret_status = CRYPTO_ERR_FAIL;
		dprintf(CRITICAL, "crypto_get_digest sw error\n");
		return;
	}

	/* Digest length depends on auth_alg */

	if (auth_alg == CRYPTO_AUTH_ALG_SHA1) {
		digest_len = SHA1_INIT_VECTOR_SIZE;
	} else if (auth_alg == CRYPTO_AUTH_ALG_SHA256) {
		digest_len = SHA256_INIT_VECTOR_SIZE;
	}

	/* Retrieve digest from CRYPTO3 */

	for (i = 0; i < digest_len; i++) {
		unsigned int auth_iv = rd_ce(CRYPTO3_AUTH_IVn(i));

		if (last) {
			*((unsigned int *)digest_ptr + i) = htonl(auth_iv);
		} else {
			*((unsigned int *)digest_ptr + i) = auth_iv;
		}
	}
	*ret_status = CRYPTO_ERR_NONE;
	return;
}

/* Function to restore auth_bytecnt registers for ctx_ptr */

void crypto_get_ctx(void *ctx_ptr)
{
	((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[0] =
	    rd_ce(CRYPTO3_AUTH_BYTECNTn(0));
	((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[1] =
	    rd_ce(CRYPTO3_AUTH_BYTECNTn(1));
	return;
}
