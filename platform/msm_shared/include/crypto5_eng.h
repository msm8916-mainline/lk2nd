/* Copyright (c) 2012,2015, The Linux Foundation. All rights reserved.

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

#ifndef __CRYPTO5_ENG_H__
#define __CRYPTO5_ENG_H__

#include <bits.h>
#include <bam.h>
#include <crypto_hash.h>

/* CRYPTO registers */
#define CRYPTO_VERSION(x)               (x + 0x0000)
#define CRYPTO_DATA_IN(x)               (x + 0x0010)
#define CRYPTO_DATA_OUT(x)              (x + 0x0020)
#define CRYPTO_STATUS(x)                (x + 0x0100)
#define CRYPTO_STATUS2(x)               (x + 0x0104)
#define CRYPTO_SEG_SIZE(x)              (x + 0x0110)
#define CRYPTO_GOPROC(x)                (x + 0x0120)
#define CRYPTO_ENGINES_AVAIL(x)         (x + 0x0108)
#define CRYPTO_ENCR_SEG_CFG(x)          (x + 0x0200)
#define CRYPTO_AUTH_SEG_CFG(x)          (x + 0x0300)
#define CRYPTO_AUTH_SEG_SIZE(x)         (x + 0x0304)
#define CRYPTO_AUTH_SEG_START(x)        (x + 0x0308)
#define CRYPTO_AUTH_IVn(x, n)           (x + 0x0310 + 4*(n))
#define CRYPTO_AUTH_BYTECNTn(x, n)      (x + 0x0390 + 4*(n))
#define CRYPTO_CONFIG(x)                (x + 0x0400)
#define CRYPTO_DEBUG(x)                 (x + 0x5004)

/* Status register errors. */
#define SW_ERR                          BIT(0)
#define AXI_ERR                         BIT(14)
#define HSD_ERR                         BIT(20)
#define AUTH_BUSY                       BIT(8)

/* Status2 register errors. */
#define AXI_EXTRA                       BIT(1)

/* CRYPTO_CONFIG register bit definitions */
#define MASK_ERR_INTR                   BIT(0)
#define MASK_OP_DONE_INTR               BIT(1)
#define MASK_DIN_INTR                   BIT(2)
#define MASK_DOUT_INTR                  BIT(3)
#define HIGH_SPD_IN_EN_N                (0 << 4)
#define PIPE_SET_SELECT_SHIFT           5
#define LITTLE_ENDIAN_MODE              BIT(9)
#define MAX_QUEUED_REQS                 (0 << 14) /* 1 Max queued request. */
#define REQ_SIZE                        (7 << 17) /* 8 beats */

/* CRYPTO_AUTH_SEG_CFG register bit definitions */
#define SEG_CFG_AUTH_ALG_SHA            (1 << 0)
#define SEG_CFG_AUTH_SIZE_SHA1          (0 << 9)
#define SEG_CFG_AUTH_SIZE_SHA256        (1 << 9)
#define SEG_CFG_LAST                    (1 << 16)

/* The value to be written to the GOPROC register.
 * Enable result dump option along with GO bit.
 */
#define GOPROC_GO                       (1 | 1 << 2)

#define CRYPTO_READ_PIPE_INDEX          0
#define CRYPTO_WRITE_PIPE_INDEX         1

/* Burst length recommended by HPG for maximum throughput. */
#define CRYPTO_BURST_LEN                64

/* This value determines how much descriptor data can be advertised on
 * the pipe sideband interface at a time.
 * The recommendation is to make it the size of your largest descriptor.
 * The max setting is 32KB.
 */

#define CRYPTO_MAX_THRESHOLD            (32 * 1024)

/* Basic CE setting to put the CE in HS mode. */
#define CRYPTO_RESET_CONFIG             0xE000F

struct crypto_bam_pipes
{
	uint8_t read_pipe;
	uint8_t write_pipe;
	uint8_t read_pipe_grp;
	uint8_t write_pipe_grp;
};

struct output_dump
{
	uint32_t auth_iv[16];
	uint32_t auth_bytcnt[4];
	uint32_t encr_cntr_iv[4];
	uint32_t status;
	uint32_t status2;
	uint8_t  burst_align_buf[24];
}__PACKED;

/* Struct to save the CE device info.
 * base              : Base addr of CE.
 * instance          : CE instance used.
 * ce_array          : ptr to the cmd elements array.
 * ce_array_index    : index within ce_array where the next cmd element needs to be added.
 * cd_start          : index within the ce_array to keep track of the num of
 *                         cmd elements that need to be added in the next desc.
 * dump              : ptr to the result dump memory.
 * bam               : bam instance used with this CE.
 * do_bam_init       : Flag to determine if bam should be initalized.
 */
struct crypto_dev
{
	uint32_t            base;
	uint32_t            instance;
	struct cmd_element  *ce_array;
	uint32_t            ce_array_index;
	uint32_t            cd_start;
	struct output_dump  *dump;
	struct bam_instance bam;
	uint8_t             do_bam_init;
};

/* Struct to pass the initial params to CE.
 * crypto_base      : Base addr of the CE.
 * crypto_instance  : CE instance used.
 * bam_base         : Base addr of CE BAM.
 * bam_ee           : EE used for CE BAM.
 * num_ce           : Number of cmd elements to be allocated.
 * read_fifo_size   : Size of the BAM desc read fifo.
 * write_fifo_size  : Size of the BAM desc write fifo.
 * do_bam_init      : Flag to determine if bam should be initalized.
 */
struct crypto_init_params
{
	uint32_t                crypto_base;
	uint32_t                crypto_instance;
	uint32_t                bam_base;
	uint32_t                bam_ee;
	uint32_t                num_ce;
	uint32_t                read_fifo_size;
	uint32_t                write_fifo_size;
	uint8_t                 do_bam_init;
	struct crypto_bam_pipes pipes;
};

void crypto5_init_params(struct crypto_dev *dev, struct crypto_init_params *params);
void crypto5_init(struct crypto_dev *dev);
void crypto5_set_ctx(struct crypto_dev *dev,
					 void *ctx_ptr,
					 crypto_auth_alg_type auth_alg);
uint32_t crypto5_send_data(struct crypto_dev *dev,
						   void *ctx_ptr,
						   uint8_t *data_ptr);
void crypto5_cleanup(struct crypto_dev *dev);
uint32_t crypto5_get_digest(struct crypto_dev *dev,
							uint8_t *digest_ptr,
							crypto_auth_alg_type auth_alg);
void crypto5_get_ctx(struct crypto_dev *dev, void *ctx_ptr);
uint32_t crypto5_get_max_auth_blk_size(struct crypto_dev *dev);
void crypto5_unlock_pipes(struct crypto_dev *dev);

#endif
