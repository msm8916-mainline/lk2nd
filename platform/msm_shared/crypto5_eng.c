/* Copyright (c) 2012-2015, The Linux Foundation. All rights reserved.
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

#include <reg.h>
#include <debug.h>
#include <endian.h>
#include <stdlib.h>
#include <arch/ops.h>
#include <platform.h>
#include <platform/iomap.h>
#include <clock.h>
#include <platform/clock.h>
#include <crypto5_eng.h>

#define CLEAR_STATUS(dev)                                crypto_write_reg(&dev->bam, CRYPTO_STATUS(dev->base), 0, BAM_DESC_UNLOCK_FLAG)
#define CONFIG_WRITE(dev, val)                           crypto_write_reg(&dev->bam, CRYPTO_CONFIG(dev->base), val, BAM_DESC_LOCK_FLAG)
#define REG_WRITE(dev, addr, val)                        crypto_write_reg(&dev->bam, addr, val, 0)

#ifndef CRYPTO_REG_ACCESS
#define CE_INIT(dev)                                     dev->ce_array_index = 0; dev->cd_start = 0
#define ADD_WRITE_CE(dev, addr, val)                     crypto_add_cmd_element(dev, addr, val)
#define ADD_CMD_DESC(dev, flags)                         crypto_add_cmd_desc(dev, flags)
#define CMD_EXEC(bam, num_desc, pipe)                    crypto_wait_for_cmd_exec(bam, num_desc, pipe)

#define REG_WRITE_QUEUE_INIT(dev)                        CE_INIT(dev)
#define REG_WRITE_QUEUE(dev, addr, val)                  ADD_WRITE_CE(dev, addr, val)
#define REG_WRITE_QUEUE_DONE(dev, flags)                 ADD_CMD_DESC(dev, flags)
#define REG_WRITE_EXEC(bam, num_desc, pipe)              CMD_EXEC(bam, num_desc, pipe)
#else
#define REG_WRITE_QUEUE_INIT(dev)                        /* nop */
#define REG_WRITE_QUEUE(dev, addr, val)                  writel(val, addr)
#define REG_WRITE_QUEUE_DONE(dev, flags)                 /* nop */
#define REG_WRITE_EXEC(bam, num_desc, pipe)              /* nop */
#endif

#define ADD_READ_DESC(bam, buf_addr, buf_size, flags)    bam_add_desc(bam, CRYPTO_READ_PIPE_INDEX, buf_addr, buf_size, flags)
#define ADD_WRITE_DESC(bam, buf_addr, buf_size, flags)   bam_add_desc(bam, CRYPTO_WRITE_PIPE_INDEX, buf_addr, buf_size, flags)


static struct bam_desc *crypto_allocate_fifo(uint32_t size)
{
	struct bam_desc *ptr;

	ptr = (struct bam_desc *) memalign(lcm(CACHE_LINE, BAM_DESC_SIZE),
					   ROUNDUP(size * BAM_DESC_SIZE, CACHE_LINE));

	if (ptr == NULL)
		dprintf(CRITICAL, "Could not allocate fifo buffer\n");

	return ptr;
}

static struct output_dump *crypto_allocate_dump_buffer(void)
{
	struct output_dump *ptr;

	ptr = (struct output_dump *) memalign(lcm(CACHE_LINE, CRYPTO_BURST_LEN),
					      ROUNDUP(sizeof(struct output_dump), CACHE_LINE));

	if (ptr == NULL)
		dprintf(CRITICAL, "Could not allocate output dump buffer\n");

	return ptr;
}

static struct cmd_element *crypto_allocate_ce_array(uint32_t size)
{
	struct cmd_element *ptr = NULL;

#ifndef CRYPTO_REG_ACCESS
	ptr = (struct cmd_element*) memalign(CACHE_LINE,
					     ROUNDUP(size * sizeof(struct cmd_element), CACHE_LINE));

	if (ptr == NULL)
		dprintf(CRITICAL, "Could not allocate ce array buffer\n");
#endif

	return ptr;
}

static void crypto_wait_for_cmd_exec(struct bam_instance *bam_core,
									 uint32_t num_desc,
									 uint8_t pipe)
{
	/* Create a read/write event to notify the periperal of the added desc. */
	bam_sys_gen_event(bam_core, pipe, num_desc);

	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(bam_core, pipe, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(bam_core, pipe);
}

static void crypto_wait_for_data(struct bam_instance *bam, uint32_t pipe_num)
{
	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(bam, pipe_num, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(bam, pipe_num);
}

static uint32_t crypto_write_reg(struct bam_instance *bam_core,
								 uint32_t reg_addr,
								 uint32_t val,
								 uint8_t flags)
{
	uint32_t ret = 0;
	struct cmd_element cmd_list_ptr;

#ifdef CRYPTO_REG_ACCESS
	writel(val, reg_addr);
#else
	ret = (uint32_t)bam_add_cmd_element(&cmd_list_ptr, reg_addr, val, CE_WRITE_TYPE);

	arch_clean_invalidate_cache_range((addr_t)&cmd_list_ptr, sizeof(struct cmd_element));

	/* Enqueue the desc for the above command */
	ret = bam_add_one_desc(bam_core,
						   CRYPTO_WRITE_PIPE_INDEX,
						   (unsigned char*)PA((addr_t)&cmd_list_ptr),
						   BAM_CE_SIZE,
						   BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG | flags);

	if (ret)
	{
		dprintf(CRITICAL,
				"CRYPTO_WRITE_REG: Reg write failed. reg addr = %x\n",
				reg_addr);
		goto crypto_read_reg_err;
	}

	crypto_wait_for_cmd_exec(bam_core, 1, CRYPTO_WRITE_PIPE_INDEX);
#endif

crypto_read_reg_err:
	return ret;
}

static void crypto_add_cmd_element(struct crypto_dev *dev,
								   uint32_t addr,
								   uint32_t val)
{
	struct cmd_element *ptr = dev->ce_array;

	bam_add_cmd_element(&(ptr[dev->ce_array_index]), addr, val, CE_WRITE_TYPE);

	arch_clean_invalidate_cache_range((addr_t) &(ptr[dev->ce_array_index]), sizeof(struct cmd_element));

	dev->ce_array_index++;
}

static void crypto_add_cmd_desc(struct crypto_dev *dev, uint8_t flags)
{
	uint32_t ce_size;
	uint32_t start = (uint32_t)&(dev->ce_array[dev->cd_start]);
	uint32_t ret;

	ce_size = (uint32_t)&(dev->ce_array[dev->ce_array_index]) - start;

	ret = bam_add_one_desc(&dev->bam,
						   CRYPTO_WRITE_PIPE_INDEX,
						   (unsigned char*)start,
						   ce_size,
						   BAM_DESC_CMD_FLAG | flags);

	if (ret)
	{
		dprintf(CRITICAL, "CRYPTO_ADD_DESC: Adding desc failed\n");
	}

	/* Update the CD ptr. */
	dev->cd_start = dev->ce_array_index;
}

static int crypto_bam_init(struct crypto_dev *dev)
{
	uint32_t bam_ret;

	/* Do BAM Init only if required. */
	if (dev->do_bam_init)
		bam_init(&dev->bam);

	/* Initialize BAM CRYPTO read pipe */
	bam_sys_pipe_init(&dev->bam, CRYPTO_READ_PIPE_INDEX);

	/* Init read fifo */
	bam_ret = bam_pipe_fifo_init(&dev->bam, CRYPTO_READ_PIPE_INDEX);

	if (bam_ret)
	{
		dprintf(CRITICAL, "CRYPTO: BAM Read FIFO init error\n");
		bam_ret = CRYPTO_ERR_FAIL;
		goto crypto_bam_init_err;
	}

	/* Initialize BAM CRYPTO write pipe */
	bam_sys_pipe_init(&dev->bam, CRYPTO_WRITE_PIPE_INDEX);

	/* Init write fifo. Use the same fifo as read fifo. */
	bam_ret = bam_pipe_fifo_init(&dev->bam, CRYPTO_WRITE_PIPE_INDEX);

	if (bam_ret)
	{
		dprintf(CRITICAL, "CRYPTO: BAM Write FIFO init error\n");
		bam_ret = CRYPTO_ERR_FAIL;
		goto crypto_bam_init_err;
	}

	bam_ret = CRYPTO_ERR_NONE;

crypto_bam_init_err:
	return bam_ret;
}

static void crypto_reset(struct crypto_dev *dev)
{
	clock_config_ce(dev->instance);
}

void crypto5_init_params(struct crypto_dev *dev, struct crypto_init_params *params)
{
	dev->base     = params->crypto_base;
	dev->instance = params->crypto_instance;

	dev->bam.base    = params->bam_base;
	dev->do_bam_init = params->do_bam_init;

	/* Set Read pipe params. */
	dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].pipe_num   = params->pipes.read_pipe;
	/* System consumer */
	dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].trans_type = BAM2SYS;
	dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].fifo.size  = params->read_fifo_size;
	dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].fifo.head  = crypto_allocate_fifo(params->read_fifo_size);
	dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].lock_grp   = params->pipes.read_pipe_grp;

	/* Set Write pipe params. */
	dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].pipe_num   = params->pipes.write_pipe;
	/* System producer */
	dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].trans_type = SYS2BAM;
	dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].fifo.size  = params->write_fifo_size;
	dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].fifo.head  = crypto_allocate_fifo(params->write_fifo_size);
	dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].lock_grp   = params->pipes.write_pipe_grp;

	dev->bam.threshold = CRYPTO_MAX_THRESHOLD;

	dev->bam.ee        = params->bam_ee;

	/* A H/W bug on Crypto 5.0.0 enforces a rule that the desc lengths must be burst aligned. */
	dev->bam.max_desc_len = ROUNDDOWN(BAM_NDP_MAX_DESC_DATA_LEN, CRYPTO_BURST_LEN);

	dev->dump           = crypto_allocate_dump_buffer();
	dev->ce_array       = crypto_allocate_ce_array(params->num_ce);
	dev->ce_array_index = 0;
	dev->cd_start       = 0;
}

void crypto5_init(struct crypto_dev *dev)
{
	uint32_t config = CRYPTO_RESET_CONFIG
			| (dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].pipe_num >> 1) << PIPE_SET_SELECT_SHIFT;

	/* Configure CE clocks. */
	clock_config_ce(dev->instance);

	/* Setup BAM */
	if (crypto_bam_init(dev) != CRYPTO_ERR_NONE)
	{
		dprintf(CRITICAL, "CRYPTO: BAM init error\n");
		goto crypto_init_err;
	}

	/* Write basic config to CE.
	 *  Note: This setting will be changed to be set from TZ.
	 */
	writel(config, CRYPTO_CONFIG(dev->base));

	config = 0;

	/* Setup config reg. */
	/* Mask all irqs. */
	config |= MASK_ERR_INTR | MASK_OP_DONE_INTR |
	          MASK_DIN_INTR | MASK_DOUT_INTR;
	/* Program BAM specific crypto settings. */
	config |= HIGH_SPD_IN_EN_N
			| ((dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].pipe_num >> 1) << PIPE_SET_SELECT_SHIFT)
			| MAX_QUEUED_REQS
			| REQ_SIZE;
	/* Use a few registers in little endian mode. */
	config |= LITTLE_ENDIAN_MODE;

	CONFIG_WRITE(dev, config);

crypto_init_err:
	return;
}

static uint32_t crypto5_get_sha_cfg(void *ctx_ptr, crypto_auth_alg_type auth_alg)
{
	crypto_SHA256_ctx *sha256_ctx = (crypto_SHA256_ctx *) ctx_ptr;
	crypto_SHA1_ctx *sha1_ctx = (crypto_SHA1_ctx *) ctx_ptr;
	uint32_t seg_cfg_val;

	seg_cfg_val = SEG_CFG_AUTH_ALG_SHA;

	if (auth_alg == CRYPTO_AUTH_ALG_SHA1)
	{
		seg_cfg_val |= SEG_CFG_AUTH_SIZE_SHA1;

		if (sha1_ctx->flags & CRYPTO_LAST_CHUNK)
		{
			seg_cfg_val |= SEG_CFG_LAST;
		}
	}
	else if (auth_alg == CRYPTO_AUTH_ALG_SHA256)
	{
		seg_cfg_val |= SEG_CFG_AUTH_SIZE_SHA256;

		if (sha256_ctx->flags & CRYPTO_LAST_CHUNK)
		{
			seg_cfg_val |= SEG_CFG_LAST;
		}
	}
	else
	{
		dprintf(CRITICAL, "crypto_set_sha_ctx invalid auth algorithm\n");
		return 0;
	}

	return seg_cfg_val;
}

void crypto5_set_ctx(struct crypto_dev *dev,
					 void *ctx_ptr,
					 crypto_auth_alg_type auth_alg)
{
	crypto_SHA256_ctx *sha256_ctx = (crypto_SHA256_ctx *) ctx_ptr;
	crypto_SHA1_ctx *sha1_ctx = (crypto_SHA1_ctx *) ctx_ptr;
	uint32_t i = 0;
	uint32_t iv_len = 0;
	uint32_t *auth_iv = sha1_ctx->auth_iv;
	uint32_t seg_cfg_val;

	if(auth_alg == CRYPTO_AUTH_ALG_SHA1)
	{
		iv_len = SHA1_INIT_VECTOR_SIZE;
	}
	else if(auth_alg == CRYPTO_AUTH_ALG_SHA256)
	{
		iv_len = SHA256_INIT_VECTOR_SIZE;
	}

	seg_cfg_val = crypto5_get_sha_cfg(ctx_ptr, auth_alg);

	if (!seg_cfg_val)
	{
		dprintf(CRITICAL, "Authentication alg config failed.\n");
		return;
	}

	/* Initialize CE pointers. */
	REG_WRITE_QUEUE_INIT(dev);

	/* For authentication operation set the encryption cfg reg to 0 as per HPG */
	REG_WRITE_QUEUE(dev, CRYPTO_ENCR_SEG_CFG(dev->base), 0);
	REG_WRITE_QUEUE(dev, CRYPTO_AUTH_SEG_CFG(dev->base), seg_cfg_val);

	for (i = 0; i < iv_len; i++)
	{
		if (sha256_ctx->flags & CRYPTO_FIRST_CHUNK)
			REG_WRITE_QUEUE(dev, CRYPTO_AUTH_IVn(dev->base, i), BE32(*(auth_iv + i)));
		else
			REG_WRITE_QUEUE(dev, CRYPTO_AUTH_IVn(dev->base, i), (*(auth_iv + i)));
	}

	/* Typecast with crypto_SHA1_ctx because offset of auth_bytecnt
	 * in both crypto_SHA1_ctx and crypto_SHA256_ctx are same.
	 */
	REG_WRITE_QUEUE(dev, CRYPTO_AUTH_BYTECNTn(dev->base, 0), ((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[0]);
	REG_WRITE_QUEUE(dev, CRYPTO_AUTH_BYTECNTn(dev->base, 1), ((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[1]);
}

/* Function: crypto5_set_auth_cfg
 * Arg     : dev, ptr to data buffer, buffer_size, burst_mask for alignment
 * Return  : aligned buffer incase of unaligned data_ptr and total no. of bytes
 *           passed to crypto HW(includes header and trailer size).
 * Flow    : If data buffer is aligned, we just configure the crypto auth
 *           registers for start, size of data etc. If buffer is unaligned
 *           we align it to burst(64-byte) boundary and also make the no. of
 *           bytes a multiple of 64 for bam and then configure the registers
 *           for header/trailer settings.
 */

static void crypto5_set_auth_cfg(struct crypto_dev *dev, uint8_t **buffer,
							uint8_t *data_ptr,
							uint32_t burst_mask,
							uint32_t bytes_to_write,
							uint32_t *total_bytes_to_write)
{
	uint32_t minor_ver = 0;
	uint32_t auth_seg_start = 0;

	/* Bits 23:16 - minor version */
        minor_ver = (readl(CRYPTO_VERSION(dev->base)) & 0x00FF0000) >> 16;

	/* A H/W bug on Crypto 5.0.0 enforces a rule that the desc lengths must
	 * be burst aligned. Here we use the header/trailer crypto register settings.
         * buffer                : The previous 64 byte aligned address for data_ptr.
         * CRYPTO_AUTH_SEG_START : Number of bytes to skip to reach the address data_ptr.
         * CRYPTO_AUTH_SEG_SIZE  : Number of  bytes to be sent to crypto HW.
         * CRYPTO_SEG_SIZE       : CRYPTO_AUTH_SEG_START + CRYPTO_AUTH_SEG_SIZE.
         * Function: We pick a previous 64 byte aligned address buffer, and tell crypto to
         * skip (data_ptr - buffer) number of bytes.
         * This bug is fixed in 5.1.0 onwards.*/

	if(minor_ver == 0)
	{
		if ((uint32_t) data_ptr & (CRYPTO_BURST_LEN - 1))
		{
			dprintf(CRITICAL, "Data start not aligned at burst length.\n");

			*buffer = (uint8_t *)ROUNDDOWN((uint32_t)data_ptr, CRYPTO_BURST_LEN);

			/* Header & Trailer */
			*total_bytes_to_write = ((bytes_to_write +(data_ptr - *buffer) + burst_mask) & (~burst_mask));

			auth_seg_start = (data_ptr - *buffer);
		}
		else
		{
			/* No header */
			/* Add trailer to make it a burst multiple as 5.0.x HW mandates data to be a multiple of 64. */
			*total_bytes_to_write = (bytes_to_write + burst_mask) & (~burst_mask);
		}
	}
	else
	{
		/* No header. 5.1 crypto HW doesnt require alignment as partial reads and writes are possible*/
		*total_bytes_to_write = bytes_to_write;
	}

	REG_WRITE_QUEUE(dev, CRYPTO_AUTH_SEG_START(dev->base), auth_seg_start);
	REG_WRITE_QUEUE(dev, CRYPTO_AUTH_SEG_SIZE(dev->base), bytes_to_write);
	REG_WRITE_QUEUE(dev, CRYPTO_SEG_SIZE(dev->base), *total_bytes_to_write);
	REG_WRITE_QUEUE(dev, CRYPTO_GOPROC(dev->base), GOPROC_GO);
	REG_WRITE_QUEUE_DONE(dev, BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG);
	REG_WRITE_EXEC(&dev->bam, 1, CRYPTO_WRITE_PIPE_INDEX);
}

uint32_t crypto5_send_data(struct crypto_dev *dev,
						   void *ctx_ptr,
						   uint8_t *data_ptr)
{
	uint32_t bam_status;
	crypto_SHA256_ctx *sha256_ctx = (crypto_SHA256_ctx *) ctx_ptr;
	uint32_t wr_flags = BAM_DESC_NWD_FLAG | BAM_DESC_INT_FLAG | BAM_DESC_EOT_FLAG;
	uint32_t ret_status;
	uint8_t *buffer = NULL;
	uint32_t total_bytes_to_write = 0;

	crypto5_set_auth_cfg(dev, &buffer, data_ptr, CRYPTO_BURST_LEN - 1, sha256_ctx->bytes_to_write,
											&total_bytes_to_write);

	if(buffer)
	{
		arch_clean_invalidate_cache_range((addr_t) buffer, total_bytes_to_write);

		bam_status = ADD_WRITE_DESC(&dev->bam, buffer, total_bytes_to_write, wr_flags);
	}
	else
	{
		arch_clean_invalidate_cache_range((addr_t) data_ptr, total_bytes_to_write);
		bam_status = ADD_WRITE_DESC(&dev->bam, data_ptr, total_bytes_to_write, wr_flags);
	}

	if (bam_status)
	{
		dprintf(CRITICAL, "Crypto send data failed\n");
		ret_status = CRYPTO_ERR_FAIL;
		goto CRYPTO_SEND_DATA_ERR;
	}

	arch_clean_invalidate_cache_range((addr_t) (dev->dump), sizeof(struct output_dump));

	bam_status = ADD_READ_DESC(&dev->bam,
							   (unsigned char *)PA((addr_t)(dev->dump)),
							   sizeof(struct output_dump),
							   BAM_DESC_INT_FLAG);

	if (bam_status)
	{
		dprintf(CRITICAL, "Crypto send data failed\n");
		ret_status = CRYPTO_ERR_FAIL;
		goto CRYPTO_SEND_DATA_ERR;
	}

	crypto_wait_for_data(&dev->bam, CRYPTO_WRITE_PIPE_INDEX);

	crypto_wait_for_data(&dev->bam, CRYPTO_READ_PIPE_INDEX);

	arch_clean_invalidate_cache_range((addr_t) (dev->dump), sizeof(struct output_dump));

	ret_status = CRYPTO_ERR_NONE;

CRYPTO_SEND_DATA_ERR:

	crypto5_unlock_pipes(dev);

	return ret_status;
}

void crypto5_unlock_pipes(struct crypto_dev *dev)
{
	CLEAR_STATUS(dev);
}

void crypto5_cleanup(struct crypto_dev *dev)
{
	CLEAR_STATUS(dev);

	/* reset the pipes. */
	bam_pipe_reset(&(dev->bam), CRYPTO_READ_PIPE_INDEX);
	bam_pipe_reset(&(dev->bam), CRYPTO_WRITE_PIPE_INDEX);

	/* Free all related memory. */
	free(dev->dump);
	free(dev->ce_array);
	free(dev->bam.pipe[CRYPTO_READ_PIPE_INDEX].fifo.head);
	free(dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].fifo.head);
}

uint32_t crypto5_get_digest(struct crypto_dev *dev,
							uint8_t *digest_ptr,
							crypto_auth_alg_type auth_alg)
{
    uint32_t ce_status = 0;
	uint32_t ce_status2 = 0;
    uint32_t ce_err_bmsk = 0;
    uint32_t i = 0;
    uint32_t digest_len = 0;
    uint32_t auth_iv;

	/* Check status register for errors. */
    ce_err_bmsk = (AXI_ERR | SW_ERR | HSD_ERR);
    ce_status = BE32(dev->dump->status);

	/* Check status register for errors. */
	ce_status2 = BE32(dev->dump->status2);

    if ((ce_status & ce_err_bmsk) || (ce_status2 & AXI_EXTRA))
	{
        crypto_reset(dev);
        dprintf(CRITICAL, "crypto_get_digest status error");
		dprintf(CRITICAL, "status = %x status2 = %x\n", ce_status, ce_status2);
        return CRYPTO_ERR_FAIL;
    }

    /* Digest length depends on auth_alg */
    if (auth_alg == CRYPTO_AUTH_ALG_SHA1)
    {
        digest_len = SHA1_INIT_VECTOR_SIZE;
    }
    else if (auth_alg == CRYPTO_AUTH_ALG_SHA256)
    {
        digest_len = SHA256_INIT_VECTOR_SIZE;
    }

    /* Retrieve digest from CRYPTO */
    for (i = 0; i < digest_len; i++)
    {
        auth_iv = (dev->dump->auth_iv[i]);

       *((unsigned int *)digest_ptr + i) = auth_iv;
    }

    return CRYPTO_ERR_NONE;
}

void crypto5_get_ctx(struct crypto_dev *dev, void *ctx_ptr)
{
	((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[0] = BE32(dev->dump->auth_bytcnt[0]);
	((crypto_SHA1_ctx *) ctx_ptr)->auth_bytecnt[1] = BE32(dev->dump->auth_bytcnt[1]);
}

uint32_t crypto5_get_max_auth_blk_size(struct crypto_dev *dev)
{
	return (dev->bam.max_desc_len * (dev->bam.pipe[CRYPTO_WRITE_PIPE_INDEX].fifo.size - 2));
}
