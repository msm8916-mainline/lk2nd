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

#ifndef __CRYPTO_ENG_H__
#define __CRYPYO_ENG_H__

#define CRYPTO_ENG_REG(offset)		(MSM_CRYPTO_BASE + offset)

#define wr_ce(val,reg)				writel(val,CRYPTO_ENG_REG(reg))
#define rd_ce(reg)					readl(CRYPTO_ENG_REG(reg))

#define IS_ALIGNED(ptr)				(!(((unsigned int)ptr) & 0x03))

#define CRYPTO_ERR_NONE				0x01
#define CRYPTO_ERR_FAIL				0x02

/* CRYPTO3 registers */
#define CRYPTO3_DATA_IN				0x0000
#define CRYPTO3_DATA_OUT			0x0010
#define CRYPTO3_STATUS				0x0020
#define CRYPTO3_CONFIG				0x0024
#define CRYPTO3_DEBUG				0x0028
#define CRYPTO3_REGISTER_LOCK		0x002C
#define CRYPTO3_SEG_CFG				0x0030
#define CRYPTO3_ENCR_SEG_CFG		0x0034
#define CRYPTO3_AUTH_SEG_CFG		0x0038
#define CRYPTO3_SEG_SIZE			0x003C
#define CRYPTO3_GOPROC				0x0040
#define CRYPTO3_ENGINES_AVAIL		0x0044

#define CRYPTO3_DES_KEYn(n)			(0x0050 + 4*(n))
#define CRYPTO3_CNTRn_KEYn(n)		(0x0070 + 4*(n))
#define CRYPTO3_CNTR_MASK			0x0080
#define CRYPTO3_AUTH_BYTECNTn(n)	(0x0090 + 4*(n))
#define CRYPTO3_AUTH_IVn(n)			(0x0100 + 4*(n))
#define CRYPTO3_AES_RNDKEYn(n)		(0x0200 + 4*(n))

#define CRYPTO3_DATA_SHADOW0		0x8000
#define CRYPTO3_DATA_SHADOW8191		0xFFFC

/* Register bit definitions */
#define SW_ERR						0x00000001
#define AUTH_DONE					0x00000002
#define DIN_RDY						0x00000004
#define DIN_SIZE_AVAIL				0x00380000

#define SW_RST						0x00000001
#define AUTO_SHUTDOWN_EN			0x00000004
#define MASK_ERR_INTR				0x00000008
#define MASK_AUTH_DONE_INTR			0x00000010
#define MASK_DIN_INTR				0x00000020
#define MASK_DOUT_INTR				0x00000040
#define HIGH_SPD_IN_EN_N			0x00002000
#define HIGH_SPD_OUT_EN_N			0x00004000
#define HIGH_SPD_HASH_EN_N			0x00008000

#define SEG_CFG_AUTH_ALG_SHA		(1<<9)
#define SEG_CFG_AUTH_SIZE_SHA1		(0<<11)
#define SEG_CFG_AUTH_SIZE_SHA256	(1<<11)
#define SEG_CFG_FIRST				(1<<17)
#define SEG_CFG_LAST				(1<<18)

#define AUTH_SEG_CFG_AUTH_SIZE		16

#define GOPROC_GO					1

#endif
