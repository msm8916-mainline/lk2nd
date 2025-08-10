/* Copyright (c) 2012, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
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

#ifndef __CRYPTO4_ENG_H__
#define __CRYPTO4_ENG_H__

#define CRYPTO_ENG_REG(base, offset)    ((base) + offset)

#define IS_ALIGNED(ptr)				(!(((unsigned int)ptr) & 0x03))

/* CRYPTO4 registers */
#define CRYPTO_DATA_IN				0x0008
#define CRYPTO_DATA_OUT				0x0010
#define CRYPTO_STATUS				0x0100
#define CRYPTO_CONFIG				0x0500
#define CRYPTO_DEBUG				0x0508
#define CRYPTO_ENCR_SEG_CFG			0x0300
#define CRYPTO_SEG_SIZE				0x0200
#define CRYPTO_GOPROC				0x0204
#define CRYPTO_ENGINES_AVAIL		0x0104
#define CRYPTO_AUTH_SEG_CFG			0x0400
#define CRYPTO_AUTH_SEG_SIZE		0x0404
#define CRYPTO_AUTH_SEG_START		0x0408

#define CRYPTO_AUTH_BYTECNTn(n)		(0x04A0 + 4*(n))
#define CRYPTO_AUTH_IVn(n)			(0x0450 + 4*(n))

/* Register bit definitions */
#define SW_ERR						BIT(0)
#define OPERATION_DONE				BIT(1)
#define DIN_RDY						BIT(2)
#define DIN_SIZE_AVAIL				0x00380000

/* CRYPTO_CONFIG register bit definitions */
#define AUTO_SHUTDOWN_EN			BIT(2)
#define MASK_ERR_INTR				BIT(3)
#define MASK_OP_DONE_INTR			BIT(4)
#define MASK_DIN_INTR				BIT(5)
#define MASK_DOUT_INTR				BIT(6)
#define HIGH_SPD_IN_EN_N			BIT(13)
#define HIGH_SPD_OUT_EN_N			BIT(14)

/* CRYPTO_AUTH_SEG_CFG register bit definitions */
#define SEG_CFG_AUTH_ALG_SHA		(1<<0)
#define SEG_CFG_AUTH_SIZE_SHA1		(0<<9)
#define SEG_CFG_AUTH_SIZE_SHA256	(1<<9)
#define SEG_CFG_LAST				(1<<14)

#define GOPROC_GO					1

void crypto_eng_cleanup(void);
#endif
