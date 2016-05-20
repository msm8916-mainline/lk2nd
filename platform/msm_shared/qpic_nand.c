/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2014, The Linux Foundation. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <qpic_nand.h>
#include <bam.h>
#include <dev/flash.h>
#include <lib/ptable.h>
#include <debug.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <platform.h>
#include <platform/clock.h>
#include <platform/iomap.h>

static uint32_t nand_base;
static struct ptable *flash_ptable;
static struct flash_info flash;
static unsigned char *flash_spare_bytes;
static uint32_t cfg0;
static uint32_t cfg1;
static uint32_t cfg0_raw;
static uint32_t cfg1_raw;
static uint32_t ecc_bch_cfg;

struct cmd_element ce_array[100];

#define QPIC_BAM_DATA_FIFO_SIZE          64
#define QPIC_BAM_CMD_FIFO_SIZE           64

static struct bam_desc cmd_desc_fifo[QPIC_BAM_CMD_FIFO_SIZE] __attribute__ ((aligned(BAM_DESC_SIZE)));
static struct bam_desc data_desc_fifo[QPIC_BAM_DATA_FIFO_SIZE] __attribute__ ((aligned(BAM_DESC_SIZE)));

static struct bam_instance bam;
static uint8_t *bbtbl;

static uint8_t* rdwr_buf;

static struct flash_id supported_flash[] = {
	/* Flash ID    ID Mask      Density(MB)    Wid Pgsz    Blksz              oobsz   8-bit ECCf */
	{0x1590AC2C,   0xFFFFFFFF,  0x20000000,    0,  2048,   0x00020000,        0x40,   0},
	{0x1590AA2C,   0xFFFFFFFF,  0x10000000,    0,  2048,   0x00020000,        0xE0,   1},
	{0x2690AC2C,   0xFFFFFFFF,  0x20000000,    0,  4096,   0x00040000,        0xE0,   1},
	{0x1590ACAD,   0xFFFFFFFF,  0x20000000,    0,  2048,   0x00020000,        0x80,   0},
	{0x9590DC2C,   0xFFFFFFFF,  0x10000000,    0,  2048,   0x00020000,        0x40,   0},
	{0x1590aa98,   0xFFFFFFFF,  0x10000000,    0,  2048,   0x00020000,        0x80,   1},
	/* Note: Width flag is 0 for 8 bit Flash and 1 for 16 bit flash   */
};

static int qpic_nand_mark_badblock(uint32_t page);

static void
qpic_nand_wait_for_cmd_exec(uint32_t num_desc)
{
	/* Create a read/write event to notify the periperal of the added desc. */
	bam_sys_gen_event(&bam, CMD_PIPE_INDEX, num_desc);

	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(&bam, CMD_PIPE_INDEX, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(&bam, CMD_PIPE_INDEX);
}

static void
qpic_nand_wait_for_data(uint32_t pipe_num)
{
	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(&bam, pipe_num, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(&bam, pipe_num);
}

static uint32_t
qpic_nand_read_reg(uint32_t reg_addr,
				   uint8_t flags,
				   struct cmd_element *cmd_list_ptr)
{
	uint32_t val;

	bam_add_cmd_element(cmd_list_ptr, reg_addr, (uint32_t)PA((addr_t)&val), CE_READ_TYPE);

	/* Enqueue the desc for the above command */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)PA((addr_t)cmd_list_ptr),
					 BAM_CE_SIZE,
					 BAM_DESC_CMD_FLAG| BAM_DESC_INT_FLAG | flags);

	qpic_nand_wait_for_cmd_exec(1);

	return val;
}

/* Assume the BAM is in a locked state. */
void
qpic_nand_erased_status_reset(struct cmd_element *cmd_list_ptr, uint8_t flags)
{
	uint32_t val = 0;

	/* Reset the Erased Codeword/Page detection controller. */
	val = NAND_ERASED_CW_DETECT_CFG_RESET_CTRL;

	bam_add_cmd_element(cmd_list_ptr, NAND_ERASED_CW_DETECT_CFG, val, CE_WRITE_TYPE);

	/* Enqueue the desc for the above command */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr,
					 BAM_CE_SIZE,
					 BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG | flags);

	qpic_nand_wait_for_cmd_exec(1);

	/* Enable the Erased Codeword/Page detection
	 * controller to check the data as it arrives.
	 * Also disable ECC reporting for an erased CW.
	 */
	val = NAND_ERASED_CW_DETECT_CFG_ACTIVATE_CTRL | NAND_ERASED_CW_DETECT_ERASED_CW_ECC_MASK;

	bam_add_cmd_element(cmd_list_ptr, NAND_ERASED_CW_DETECT_CFG, val, CE_WRITE_TYPE);

	/* Enqueue the desc for the above command */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr,
					 BAM_CE_SIZE,
					 BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG);

	qpic_nand_wait_for_cmd_exec(1);
}

static nand_result_t
qpic_nand_check_status(uint32_t status)
{
	uint32_t erase_sts;

	/* Check for errors */
	if (status & NAND_FLASH_ERR)
	{
		/* Check if this is an ECC error on an erased page. */
		if (status & NAND_FLASH_OP_ERR)
		{
			erase_sts = qpic_nand_read_reg(NAND_ERASED_CW_DETECT_STATUS, 0, ce_array);
			if ((erase_sts & (1 << NAND_ERASED_CW_DETECT_STATUS_PAGE_ALL_ERASED)))
			{
				/* Mask the OP ERROR. */
				status &= ~NAND_FLASH_OP_ERR;
				qpic_nand_erased_status_reset(ce_array, 0);
			}
		}

		/* ECC error flagged on an erased page read.
		 * Ignore and return success.
		 */
		if (!(status & NAND_FLASH_ERR))
			return NANDC_RESULT_SUCCESS;

		dprintf(CRITICAL, "Nand Flash error. Status = %d\n", status);

		if (status & NAND_FLASH_TIMEOUT_ERR)
			return NANDC_RESULT_TIMEOUT;
		else
			return NANDC_RESULT_FAILURE;
	}

	return NANDC_RESULT_SUCCESS;
}

static uint32_t
qpic_nand_fetch_id(struct flash_info *flash)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	int num_desc = 0;
	uint32_t status;
	uint32_t id;
	uint32_t flash_cmd = NAND_CMD_FETCH_ID;
	uint32_t exec_cmd = 1;
	int nand_ret = NANDC_RESULT_SUCCESS;

	/* Issue the Fetch id command to the NANDc */
	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)flash_cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Execute the cmd */
	bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)exec_cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Prepare the cmd desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG |
					 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG);

	/* Keep track of the number of desc added. */
	num_desc++;
	qpic_nand_wait_for_cmd_exec(num_desc);

	cmd_list_ptr_start = ce_array;
	cmd_list_ptr = ce_array;

	/* Read the status register */
	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0, cmd_list_ptr);

	/* Check for errors */
	nand_ret = qpic_nand_check_status(status);
	if (nand_ret)
	{
		dprintf( CRITICAL, "Read ID cmd status failed\n");
		goto qpic_nand_fetch_id_err;
	}

	/* Read the id */
	id = qpic_nand_read_reg(NAND_READ_ID, BAM_DESC_UNLOCK_FLAG, cmd_list_ptr);

	flash->id = id;
	flash->vendor = id & 0xff;
	flash->device = (id >> 8) & 0xff;
	flash->dev_cfg = (id >> 24) & 0xFF;
	flash->widebus = 0;
	flash->widebus &= (id >> 24) & 0xFF;
	flash->widebus = flash->widebus? 1: 0;

qpic_nand_fetch_id_err:
	return nand_ret;
}

static int
qpic_bam_init(struct qpic_nand_init_config *config)
{
	uint32_t bam_ret = NANDC_RESULT_SUCCESS;

	bam.base = config->bam_base;
	/* Set Read pipe params. */
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].pipe_num = config->pipes.read_pipe;
	/* System consumer */
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].trans_type = BAM2SYS;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].fifo.size = QPIC_BAM_DATA_FIFO_SIZE;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].fifo.head = data_desc_fifo;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].lock_grp = config->pipes.read_pipe_grp;

	/* Set Write pipe params. */
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].pipe_num = config->pipes.write_pipe;
	/* System producer */
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].trans_type = SYS2BAM;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].fifo.size = QPIC_BAM_DATA_FIFO_SIZE;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].fifo.head = data_desc_fifo;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].lock_grp = config->pipes.write_pipe_grp;

	/* Set Cmd pipe params. */
	bam.pipe[CMD_PIPE_INDEX].pipe_num = config->pipes.cmd_pipe;
	/* System consumer */
	bam.pipe[CMD_PIPE_INDEX].trans_type = SYS2BAM;
	bam.pipe[CMD_PIPE_INDEX].fifo.size = QPIC_BAM_CMD_FIFO_SIZE;
	bam.pipe[CMD_PIPE_INDEX].fifo.head = cmd_desc_fifo;
	bam.pipe[CMD_PIPE_INDEX].lock_grp = config->pipes.cmd_pipe_grp;

	/* Programs the threshold for BAM transfer
	 * When this threshold is reached, BAM signals the peripheral via the pipe_bytes_available
	 * interface.
	 * The peripheral is signalled with this notification in the following cases:
	 * a.  It has accumulated all the descriptors.
	 * b.  It has accumulated more than threshold bytes.
	 * c.  It has reached EOT (End Of Transfer).
	 * Note: this value needs to be set by the h/w folks and is specific for each peripheral.
	*/
	bam.threshold = 32;

	/* Set the EE.  */
	bam.ee = config->ee;

	/* Set the max desc length for this BAM. */
	bam.max_desc_len = config->max_desc_len;

	/* BAM Init. */
	bam_init(&bam);

	/* Initialize BAM QPIC read pipe */
	bam_sys_pipe_init(&bam, DATA_PRODUCER_PIPE_INDEX);

	/* Init read fifo */
	bam_ret = bam_pipe_fifo_init(&bam, bam.pipe[DATA_PRODUCER_PIPE_INDEX].pipe_num);

	if (bam_ret)
	{
		dprintf(CRITICAL, "QPIC:NANDc BAM Read FIFO init error\n");
		bam_ret = NANDC_RESULT_FAILURE;
		goto qpic_nand_bam_init_error;
	}

	/* Initialize BAM QPIC write pipe */
	bam_sys_pipe_init(&bam, DATA_CONSUMER_PIPE_INDEX);

	/* Init write fifo. Use the same fifo as read fifo. */
	bam_ret = bam_pipe_fifo_init(&bam, bam.pipe[DATA_CONSUMER_PIPE_INDEX].pipe_num);

	if (bam_ret)
	{
		dprintf(CRITICAL, "QPIC: NANDc: BAM Write FIFO init error\n");
		bam_ret = NANDC_RESULT_FAILURE;
		goto qpic_nand_bam_init_error;
	}

	/* Initialize BAM QPIC cmd pipe */
	bam_sys_pipe_init(&bam, CMD_PIPE_INDEX);

	/* Init cmd fifo */
	bam_ret = bam_pipe_fifo_init(&bam, bam.pipe[CMD_PIPE_INDEX].pipe_num);

	if (bam_ret)
	{
		dprintf(CRITICAL, "QPIC:NANDc BAM CMD FIFO init error\n");
		bam_ret = NANDC_RESULT_FAILURE;
		goto qpic_nand_bam_init_error;
	}

qpic_nand_bam_init_error:
return bam_ret;
}

/* Adds command elements for addr and cfg register writes.
 * cfg: Defines the configuration for the flash cmd.
 * start: Address where the command elements are added.
 *
 * Returns the address where the next cmd element can be added.
 */
static struct cmd_element*
qpic_nand_add_addr_n_cfg_ce(struct cfg_params *cfg,
							struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr = start;

	bam_add_cmd_element(cmd_list_ptr, NAND_ADDR0, (uint32_t)cfg->addr0, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_ADDR1, (uint32_t)cfg->addr1, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_CFG0, (uint32_t)cfg->cfg0, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_CFG1, (uint32_t)cfg->cfg1, CE_WRITE_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}


static struct cmd_element*
qpic_nand_add_onfi_probe_ce(struct onfi_probe_params *params,
							struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr = start;

	cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(&params->cfg, cmd_list_ptr);

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD1, (uint32_t)params->dev_cmd1, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD_VLD, (uint32_t)params->vld, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_READ_LOCATION_n(0), (uint32_t)params->cfg.addr_loc_0, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)params->cfg.cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)params->cfg.exec, CE_WRITE_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}

static int
onfi_probe_cmd_exec(struct onfi_probe_params *params,
					unsigned char* data_ptr,
					int data_len)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	int num_desc = 0;
	uint32_t status = 0;
	int nand_ret = NANDC_RESULT_SUCCESS;
	uint8_t desc_flags = BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG
						| BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG;

	params->cfg.addr_loc_0 = 0;
	params->cfg.addr_loc_0 |= NAND_RD_LOC_LAST_BIT(1);
	params->cfg.addr_loc_0 |= NAND_RD_LOC_OFFSET(0);
	params->cfg.addr_loc_0 |= NAND_RD_LOC_SIZE(data_len);

	cmd_list_ptr = qpic_nand_add_onfi_probe_ce(params, cmd_list_ptr);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((addr_t)(uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 desc_flags);

	cmd_list_ptr_start = cmd_list_ptr;
	num_desc++;

	/* Add Data desc */
	bam_add_desc(&bam,
				 DATA_PRODUCER_PIPE_INDEX,
				 (unsigned char *)PA((addr_t)data_ptr),
				 data_len,
				 BAM_DESC_INT_FLAG);

	/* Wait for the commands to be executed */
	qpic_nand_wait_for_cmd_exec(num_desc);

	/* Read buffer status and check for errors. */
	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0, cmd_list_ptr++);

	if (qpic_nand_check_status(status))
	{
		nand_ret = NANDC_RESULT_FAILURE;
		goto onfi_probe_exec_err;
	}

	/* Wait for data to be available */
	qpic_nand_wait_for_data(DATA_PRODUCER_PIPE_INDEX);

	/* Check for errors */
	nand_ret = qpic_nand_check_status(status);

onfi_probe_exec_err:
	return nand_ret;
}

/* TODO: check why both vld and cmd need to be written. */
void
qpic_nand_onfi_probe_cleanup(uint32_t vld, uint32_t dev_cmd1)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD1, dev_cmd1, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD_VLD,	vld, CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 BAM_DESC_UNLOCK_FLAG | BAM_DESC_CMD_FLAG| BAM_DESC_INT_FLAG);

	qpic_nand_wait_for_cmd_exec(1);
}

static int
qpic_nand_onfi_save_params(struct onfi_param_page *param_page, struct flash_info *flash)
{
	int onfi_ret = NANDC_RESULT_SUCCESS;
	uint32_t ecc_bits;

	onfi_ret = qpic_nand_fetch_id(flash);

	if (onfi_ret)
	{
		dprintf(CRITICAL, "Fetch ID cmd failed\n");
		goto onfi_save_params_err;
	}

	flash->page_size = param_page->data_per_pg;
	flash->block_size = param_page->pgs_per_blk * flash->page_size;
	flash->num_blocks = param_page->blks_per_LUN;
	flash->widebus = param_page->feature_supported & 0x1;
	flash->density = param_page->blks_per_LUN * flash->blksize;
	flash->spare_size = param_page->spare_per_pg;
	ecc_bits = param_page->num_bits_ecc_correctability;
	flash->num_pages_per_blk = param_page->pgs_per_blk;
	flash->num_pages_per_blk_mask = param_page->pgs_per_blk - 1;

	if (ecc_bits >= 8)
		flash->ecc_width = NAND_WITH_8_BIT_ECC;
	else
		flash->ecc_width = NAND_WITH_4_BIT_ECC;

	onfi_save_params_err:
		return onfi_ret;
}

static void
qpic_nand_save_config(struct flash_info *flash)
{

	/* Save Configurations */
	flash->cws_per_page = flash->page_size >> NAND_CW_DIV_RIGHT_SHIFT;

	/* Verify that we have enough buffer to handle all the cws in a page. */
	ASSERT(flash->cws_per_page <= QPIC_NAND_MAX_CWS_IN_PAGE);

	/* Codeword Size = UD_SIZE_BYTES + ECC_PARITY_SIZE_BYTES
	 *                          + SPARE_SIZE_BYTES + Bad Block size
	 */
	if (flash->ecc_width & NAND_WITH_8_BIT_ECC)
	{
		flash->cw_size = NAND_CW_SIZE_8_BIT_ECC;
		ecc_bch_cfg |= (1 << NAND_DEV0_ECC_MODE_SHIFT); /* Use 8-bit ecc */

		if (flash->widebus)
		{
			cfg0 |= (0 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT); /* spare size bytes in each CW */
			ecc_bch_cfg |= (14 << NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT); /* parity bytes in each CW */
		}
		else
		{
			cfg0 |= (2 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT); /* spare size bytes in each CW */
			ecc_bch_cfg |= (13 << NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT); /* parity bytes in each CW */
		}
	}
	else
	{
		flash->cw_size = NAND_CW_SIZE_4_BIT_ECC;

		if (flash->widebus)
		{
			cfg0 |= (2 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT); /* spare size bytes in each CW */
			ecc_bch_cfg |= (8 << NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT); /* parity bytes in each CW */
		}
		else
		{
			cfg0 |= (4 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT); /* spare size bytes in each CW */
			ecc_bch_cfg |= (7 << NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT); /* parity bytes in each CW */
		}
	}

	/* BAD_BLOCK_BYTE_NUM = Page Size -
	 *                                        (CW_PER_PAGE * Codeword Size) + 1
	 * Note: Set CW_PER_PAGE to 1 less than the actual number.
	 */
	flash->bad_blk_loc = flash->page_size - flash->cw_size * (flash->cws_per_page - 1) + 1;

	cfg0 |= ((flash->cws_per_page - 1) << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT) /* 4/8 cw/pg for 2/4k */
			|(DATA_BYTES_IN_IMG_PER_CW << NAND_DEV0_CFG0_UD_SIZE_BYTES_SHIFT) /* 516 user data bytes */
			|(5 << NAND_DEV0_CFG0_ADDR_CYCLE_SHIFT) /* 5 address cycles */
			|(0 << NAND_DEV0_CFG0_DIS_STS_AFTER_WR_SHIFT);/* Send read status cmd after each write. */

	cfg1 |= (7 << NAND_DEV0_CFG1_RECOVERY_CYCLES_SHIFT) /* 8 recovery cycles */
			|(0 << NAND_DEV0_CFG1_CS_ACTIVE_BSY_SHIFT)		/* Allow CS deassertion */
			|(flash->bad_blk_loc << NAND_DEV0_CFG1_BAD_BLK_BYTE_NUM_SHIFT)/* Bad block marker location */
			|(0 << NAND_DEV0_CFG1_BAD_BLK_IN_SPARE_SHIFT)		/* Bad block in user data area */
			|(2 << NAND_DEV0_CFG1_WR_RD_BSY_GAP_SHIFT)		/* 8 cycle tWB/tRB */
			|(flash->widebus << NAND_DEV0_CFG1_WIDE_BUS_SHIFT);	/* preserve wide flash flag */

	cfg0_raw = ((flash->cws_per_page- 1) << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT)
				|(5 << NAND_DEV0_CFG0_ADDR_CYCLE_SHIFT)
				|(516 << NAND_DEV0_CFG0_UD_SIZE_BYTES_SHIFT) //figure out the size of cw
				| (1 << NAND_DEV0_CFG0_DIS_STS_AFTER_WR_SHIFT);

    cfg1_raw = (7 <<  NAND_DEV0_CFG1_RECOVERY_CYCLES_SHIFT)
				| (0 <<  NAND_DEV0_CFG1_CS_ACTIVE_BSY_SHIFT)
				| (17 <<  NAND_DEV0_CFG1_BAD_BLK_BYTE_NUM_SHIFT)
				| (1 << NAND_DEV0_CFG1_BAD_BLK_IN_SPARE_SHIFT)
				| (2 << NAND_DEV0_CFG1_WR_RD_BSY_GAP_SHIFT)
				| (flash->widebus << NAND_DEV0_CFG1_WIDE_BUS_SHIFT)
				|1 ; /* to disable reed solomon ecc..this feild is now read only. */

	ecc_bch_cfg |= (0 << NAND_DEV0_ECC_DISABLE_SHIFT) /* Enable ECC */
					| (0 << NAND_DEV0_ECC_SW_RESET_SHIFT) /* Put ECC core in op mode */
					| (DATA_BYTES_IN_IMG_PER_CW << NAND_DEV0_ECC_NUM_DATA_BYTES)
					| (1 << NAND_DEV0_ECC_FORCE_CLK_OPEN_SHIFT); /* Enable all clocks */
}

/* Onfi probe should issue the following commands to the flash device:
 * 1. Read ID - with addr ONFI_READ_ID_ADDR.
 *              This returns the ONFI ASCII string indicating support for ONFI.
 * 2. Read Prameter Page - with addr ONFI_READ_PARAM_PAGE_ADDR.
 *                         This returns the params for the device.
 * Each command inturn issues commands- ADDR0, ADDR1, chip_select,
 * cfg0, cfg1, cmd_vld, dev_cmd1, read_loc0, flash, exec.
 */
static int
qpic_nand_onfi_probe(struct flash_info *flash)
{
	struct onfi_probe_params params;
	uint32_t vld;
	uint32_t dev_cmd1;
	unsigned char *buffer;
	unsigned char onfi_str[4];
	uint32_t *id;
	struct onfi_param_page *param_page;
	int onfi_ret = NANDC_RESULT_SUCCESS;

	/* Allocate memory required to read the onfi param page */
	buffer = (unsigned char*) malloc(ONFI_READ_PARAM_PAGE_BUFFER_SIZE);
	ASSERT(buffer != NULL);

	/* Read the vld and dev_cmd1 registers before modifying */
	vld = qpic_nand_read_reg(NAND_DEV_CMD_VLD, 0, ce_array);
	dev_cmd1 = qpic_nand_read_reg(NAND_DEV_CMD1, 0, ce_array);

	/* Initialize flash cmd */
	params.cfg.cmd = NAND_CMD_PAGE_READ;
	params.cfg.exec = 1;

	/* Execute Read ID cmd */

	/* Initialize the config */
	params.cfg.cfg0 = NAND_CFG0_RAW_ONFI_ID;
	params.cfg.cfg1 = NAND_CFG1_RAW_ONFI_ID;

	/* Initialize the cmd and vld */
	params.dev_cmd1 = (dev_cmd1 & 0xFFFFFF00) | ONFI_READ_ID_CMD;
	params.vld = vld & 0xFFFFFFFE;

	/* Initialize the address
	 * addr1 is not used bcos of the cfg.
	 */
	params.cfg.addr0 = ONFI_READ_ID_ADDR;
	params.cfg.addr1 = 0;

	/* Lock the pipe and execute the cmd. */
	onfi_ret = onfi_probe_cmd_exec(&params, onfi_str, ONFI_READ_ID_BUFFER_SIZE);
	if (onfi_ret)
	{
		dprintf(CRITICAL, "ONFI Read id cmd failed\n");
		goto qpic_nand_onfi_probe_err;
	}

	/* Write back vld and cmd and unlock the pipe. */
	qpic_nand_onfi_probe_cleanup(vld, dev_cmd1);

	/* Check for onfi string */
	id = (uint32_t*)onfi_str;
	if (*id != ONFI_SIGNATURE)
	{
		dprintf(CRITICAL, "Not an ONFI device\n");
		/* Not an onfi device. Return error. */
		onfi_ret = NANDC_RESULT_DEV_NOT_SUPPORTED;
		goto qpic_nand_onfi_probe_err;
	}

	dprintf(INFO, "ONFI device found\n");
	/* Now read the param page */
	/* Initialize the config */
	params.cfg.cfg0 = NAND_CFG0_RAW_ONFI_PARAM_PAGE;
	params.cfg.cfg1 = NAND_CFG1_RAW_ONFI_PARAM_PAGE;

	/* Initialize the cmd and vld */
	params.dev_cmd1 = (dev_cmd1 & 0xFFFFFF00) | ONFI_READ_PARAM_PAGE_CMD;
	params.vld = vld & 0xFFFFFFFE;

	/* Initialize the address
	 * addr1 is not used bcos of the cfg.
	 */
	params.cfg.addr0 = ONFI_READ_PARAM_PAGE_ADDR;
	params.cfg.addr1 = 0;

	/* Lock the pipe and execute the cmd. */
	onfi_ret = onfi_probe_cmd_exec(&params, buffer, ONFI_READ_PARAM_PAGE_BUFFER_SIZE);
	if (onfi_ret)
	{
		dprintf(CRITICAL, "ONFI Read param page failed\n");
		goto qpic_nand_onfi_probe_err;
	}

	/* Write back vld and cmd and unlock the pipe. */
	qpic_nand_onfi_probe_cleanup(vld, dev_cmd1);

	/* Verify the integrity of the returned page */
	param_page = (struct onfi_param_page*)buffer;

	/* TODO: Add CRC check to validate the param page. */

	/* Save the parameter values */
	onfi_ret = qpic_nand_onfi_save_params(param_page, flash);

qpic_nand_onfi_probe_err:
	if (onfi_ret)
		dprintf(CRITICAL, "ONFI probe failed\n");

	free(buffer);

	return onfi_ret;
}

/* Enquues a desc for a flash cmd with NWD flag set:
 * cfg: Defines the configuration for the flash cmd.
 * start: Address where the command elements are added.
 *
 * Returns the address where the next cmd element can be added.
 */
struct cmd_element*
qpic_nand_add_cmd_ce(struct cfg_params *cfg,
                                 struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr;

	cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(cfg, start);

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)cfg->cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;

	bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)cfg->exec, CE_WRITE_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}

/* Reads nand_flash_status and resets nand_flash_status and nand_read_status */
struct cmd_element*
qpic_nand_add_read_n_reset_status_ce(struct cmd_element *start,
									 uint32_t *flash_status_read,
									 uint32_t read_status)
{
	struct cmd_element *cmd_list_ptr = start;
	uint32_t flash_status_reset;
	uint32_t read_status_reset;

	/* Read and reset the status registers. */
	flash_status_reset = NAND_FLASH_STATUS_RESET;
	read_status_reset = NAND_READ_STATUS_RESET;

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_STATUS, (uint32_t)PA((addr_t)flash_status_read), CE_READ_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_STATUS, (uint32_t)flash_status_reset, CE_WRITE_TYPE);
	cmd_list_ptr++;

	if (read_status)
	{
		bam_add_cmd_element(cmd_list_ptr, NAND_READ_STATUS, (uint32_t)read_status_reset, CE_WRITE_TYPE);
		cmd_list_ptr++;
	}

	return cmd_list_ptr;
}

struct cmd_element*
qpic_nand_add_isbad_cmd_ce(struct cfg_params *cfg,
                                 struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr = start;

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_ECC_CFG, (uint32_t)cfg->ecc_cfg, CE_WRITE_TYPE);
	cmd_list_ptr++;

	bam_add_cmd_element(cmd_list_ptr, NAND_READ_LOCATION_n(0), (uint32_t)cfg->addr_loc_0, CE_WRITE_TYPE);
	cmd_list_ptr++;

	cmd_list_ptr = qpic_nand_add_cmd_ce(cfg, cmd_list_ptr);

	return cmd_list_ptr;
}

static int
qpic_nand_block_isbad_exec(struct cfg_params *params,
                           uint8_t *bad_block)
{

	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	uint8_t desc_flags = BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG
						| BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG;
	int num_desc = 0;
	uint32_t status = 0;
	int nand_ret = NANDC_RESULT_SUCCESS;

	cmd_list_ptr = qpic_nand_add_isbad_cmd_ce(params, cmd_list_ptr);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 desc_flags);

	num_desc++;

	/* Add Data desc */
	bam_add_desc(&bam,
				 DATA_PRODUCER_PIPE_INDEX,
				 (unsigned char *)PA((addr_t)bad_block),
				 4,
				 BAM_DESC_INT_FLAG);

	qpic_nand_wait_for_cmd_exec(num_desc);

	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0, cmd_list_ptr);

	nand_ret = qpic_nand_check_status(status);

	/* Dummy read to unlock pipe. */
	status = qpic_nand_read_reg(NAND_FLASH_STATUS, BAM_DESC_UNLOCK_FLAG, cmd_list_ptr);

	if (nand_ret)
		return NANDC_RESULT_FAILURE;

	qpic_nand_wait_for_data(DATA_PRODUCER_PIPE_INDEX);

	return nand_ret;
}

/**
 * qpic_nand_block_isbad() - Checks is given block is bad
 * @page - number of page the block starts at
 *
 * Returns nand_result_t
 */
nand_result_t qpic_nand_block_isbad(unsigned page)
{
	unsigned cwperpage;
	struct cfg_params params;
	uint8_t bad_block[4];
	unsigned nand_ret = NANDC_RESULT_SUCCESS;
	uint32_t blk = page / flash.num_pages_per_blk;

	if (bbtbl[blk] == NAND_BAD_BLK_VALUE_IS_GOOD)
		return NANDC_RESULT_SUCCESS;
	else if (bbtbl[blk] == NAND_BAD_BLK_VALUE_IS_BAD)
		return NANDC_RESULT_BAD_BLOCK;
	else
	{
		/* Read the bad block value from the flash.
		 * Bad block value is stored in the first page of the block.
		 */
		/* Read the first page in the block. */
		cwperpage = flash.cws_per_page;

		/* Read page cmd */
		params.cmd =  NAND_CMD_PAGE_READ_ECC;
		/* Clear the CW per page bits */
		params.cfg0 = cfg0_raw & ~(7U << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT);
		params.cfg1 = cfg1_raw;
		/* addr0 - Write column addr + few bits in row addr upto 32 bits. */
		params.addr0 = (page << 16) | (USER_DATA_BYTES_PER_CW * cwperpage);

		/* addr1 - Write rest of row addr.
		 * This will be all 0s.
		 */
		params.addr1 = (page >> 16) & 0xff;
		params.addr_loc_0 = NAND_RD_LOC_OFFSET(0);
		params.addr_loc_0 |= NAND_RD_LOC_LAST_BIT(1);
		params.addr_loc_0 |= NAND_RD_LOC_SIZE(4); /* Read 4 bytes */
		params.ecc_cfg = ecc_bch_cfg | 0x1; /* Disable ECC */
		params.exec = 1;

		if (qpic_nand_block_isbad_exec(&params, bad_block))
		{
			dprintf(CRITICAL,
					"Could not read bad block value\n");
			return NANDC_RESULT_FAILURE;
		}

		if (flash.widebus)
		{
			if (bad_block[0] != 0xFF && bad_block[1] != 0xFF)
			{
				bbtbl[blk] = NAND_BAD_BLK_VALUE_IS_BAD;
				nand_ret = NANDC_RESULT_BAD_BLOCK;
			}
		}
		else if (bad_block[0] != 0xFF)
		{
			bbtbl[blk] = NAND_BAD_BLK_VALUE_IS_BAD;
			nand_ret = NANDC_RESULT_BAD_BLOCK;
		}
		else
			bbtbl[blk] = NAND_BAD_BLK_VALUE_IS_GOOD;

		return nand_ret;
	}
}

/* Function to erase a block on the nand.
 * page: Starting page address for the block.
 */
nand_result_t qpic_nand_blk_erase(uint32_t page)
{
	struct cfg_params cfg;
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	uint32_t status;
	int num_desc = 0;
	uint32_t blk_addr = page / flash.num_pages_per_blk;
	int nand_ret;

	/* Erase only if the block is not bad */
	if (qpic_nand_block_isbad(page))
	{
		dprintf(CRITICAL,
				"NAND Erase error: Block address belongs to bad block: %d\n",
				blk_addr);
		return NANDC_RESULT_FAILURE;
	}

	/* Fill in params for the erase flash cmd */
	cfg.addr0 = page;
	cfg.addr1 = 0;
	/* Clear CW_PER_PAGE in cfg0 */
	cfg.cfg0 = cfg0 & ~(7U << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT);
	cfg.cfg1 = cfg1;
	cfg.cmd = NAND_CMD_BLOCK_ERASE;
	cfg.exec = 1;

	cmd_list_ptr = qpic_nand_add_cmd_ce(&cfg, cmd_list_ptr);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG | BAM_DESC_LOCK_FLAG);

	cmd_list_ptr_start = cmd_list_ptr;
	num_desc++;

	qpic_nand_wait_for_cmd_exec(num_desc);

	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0, cmd_list_ptr);

	cmd_list_ptr++;
	cmd_list_ptr_start = cmd_list_ptr;

	/* QPIC controller automatically sends
	 * GET_STATUS cmd to the nand card because
	 * of the configuration programmed.
	 * Read the result of GET_STATUS cmd.
	 */
	cmd_list_ptr = qpic_nand_add_read_n_reset_status_ce(cmd_list_ptr, &status, 1);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 BAM_DESC_INT_FLAG | BAM_DESC_CMD_FLAG) ;

	num_desc = 1;
	qpic_nand_wait_for_cmd_exec(num_desc);

	status = qpic_nand_check_status(status);

	/* Dummy read to unlock pipe. */
	nand_ret = qpic_nand_read_reg(NAND_FLASH_STATUS, BAM_DESC_UNLOCK_FLAG, cmd_list_ptr);

	/* Check for status errors*/
	if (status)
	{
		dprintf(CRITICAL,
				"NAND Erase error: Block address belongs to bad block: %d\n",
				blk_addr);
		qpic_nand_mark_badblock(page);
		return NANDC_RESULT_FAILURE;
	}

	/* Check for PROG_ERASE_OP_RESULT bit for the result of erase operation. */
	if (!(status & PROG_ERASE_OP_RESULT))
		return NANDC_RESULT_SUCCESS;

	qpic_nand_mark_badblock(page);
	return NANDC_RESULT_FAILURE;
}

/* Return num of desc added. */
static void
qpic_nand_add_wr_page_cws_cmd_desc(struct cfg_params *cfg,
								   uint32_t status[],
								   enum nand_cfg_value cfg_mode)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	uint32_t ecc;
	int num_desc = 0;
	int int_flag = 0;

	if (cfg_mode == NAND_CFG)
		ecc = ecc_bch_cfg;
	else
		ecc = ecc_bch_cfg | 0x1; /* Disable ECC */

	/* Add ECC configuration */
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_ECC_CFG,
						(uint32_t)ecc, CE_WRITE_TYPE);
	cmd_list_ptr++;
	cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(cfg, cmd_list_ptr);

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD,
						(uint32_t)cfg->cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 BAM_DESC_CMD_FLAG | BAM_DESC_LOCK_FLAG);

	num_desc++;

	/* Add CE for all the CWs */
	for (unsigned i = 0; i < flash.cws_per_page; i++)
	{
		cmd_list_ptr_start = cmd_list_ptr;
		int_flag = BAM_DESC_INT_FLAG;

		bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)cfg->exec, CE_WRITE_TYPE);
		cmd_list_ptr++;

		/* Enqueue the desc for the above commands */
		bam_add_one_desc(&bam,
						 CMD_PIPE_INDEX,
						 (unsigned char*)cmd_list_ptr_start,
						 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
						 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG);

		num_desc++;
		cmd_list_ptr_start = cmd_list_ptr;

		/* Set interrupt bit only for the last CW */
		if (i == flash.cws_per_page - 1)
		{
			cmd_list_ptr = qpic_nand_add_read_n_reset_status_ce(cmd_list_ptr,
																&status[i],
																1);
		}
		else
			cmd_list_ptr = qpic_nand_add_read_n_reset_status_ce(cmd_list_ptr,
																&status[i],
																0);

		/* Enqueue the desc for the above commands */
		bam_add_one_desc(&bam,
						 CMD_PIPE_INDEX,
						 (unsigned char*)cmd_list_ptr_start,
						 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
						 int_flag | BAM_DESC_CMD_FLAG);
		num_desc++;

		qpic_nand_wait_for_cmd_exec(num_desc);

		status[i] = qpic_nand_check_status(status[i]);

		num_desc = 0;
	}
	return;
}

void
qpic_add_wr_page_cws_data_desc(const void *buffer,
							   enum nand_cfg_value cfg_mode,
							   const void *spareaddr)
{
	int len;
	int flags;
	uint32_t start;
	unsigned num_desc = 0;

	for( unsigned i = 0; i < flash.cws_per_page; i++)
	{
		flags = 0;

		/* Set the interrupt flag on the last CW write for the page. */
		if( i == flash.cws_per_page - 1)
			flags |= BAM_DESC_INT_FLAG;

		if (cfg_mode != NAND_CFG_RAW)
		{
				start = (uint32_t)buffer + i * DATA_BYTES_IN_IMG_PER_CW;

				if (i < (flash.cws_per_page - 1))
				{
					len = DATA_BYTES_IN_IMG_PER_CW;
					flags |= BAM_DESC_EOT_FLAG;
				}
				else
				{
					/* Allow space for spare bytes in the last page */
					len = USER_DATA_BYTES_PER_CW - ((flash.cws_per_page -  1) << 2);
					flags = 0;
				}
			}
		else
		{
			start = (uint32_t)buffer;
			len = flash.cw_size;
			flags |= BAM_DESC_EOT_FLAG;
		}
		bam_add_one_desc(&bam, DATA_CONSUMER_PIPE_INDEX, (unsigned char*)PA(start), len, flags);
		num_desc++;

		if ((i == (flash.cws_per_page - 1)) && (cfg_mode == NAND_CFG))
		{
			/* write extra data */
			start = (uint32_t)spareaddr;
			len = (flash.cws_per_page << 2);
			flags = BAM_DESC_EOT_FLAG | BAM_DESC_INT_FLAG;
			bam_add_one_desc(&bam, DATA_CONSUMER_PIPE_INDEX, (unsigned char*)PA(start), len, flags);
			num_desc++;
		}
	}

	bam_sys_gen_event(&bam, DATA_CONSUMER_PIPE_INDEX, num_desc);
}

static nand_result_t
qpic_nand_write_page(uint32_t pg_addr,
                     enum nand_cfg_value cfg_mode,
                     const void* buffer,
                     const void* spareaddr)
{
	struct cfg_params cfg;
	uint32_t status[QPIC_NAND_MAX_CWS_IN_PAGE];
	int nand_ret = NANDC_RESULT_SUCCESS;

	if (cfg_mode == NAND_CFG_RAW)
	{
		cfg.cfg0 = cfg0_raw;
		cfg.cfg1 = cfg1_raw;
	}
	else
	{
		cfg.cfg0 = cfg0;
		cfg.cfg1 = cfg1;
	}

	cfg.cmd = NAND_CMD_PRG_PAGE;
	cfg.exec = 1;

	cfg.addr0 = pg_addr << 16;
	cfg.addr1 = (pg_addr >> 16) & 0xff;

	qpic_add_wr_page_cws_data_desc(buffer, cfg_mode, spareaddr);

	qpic_nand_add_wr_page_cws_cmd_desc(&cfg, status, cfg_mode);

	/* Check for errors */
	for(unsigned i = 0; i < flash.cws_per_page; i++)
	{
		nand_ret = qpic_nand_check_status(status[i]);
		if (nand_ret)
		{
			dprintf(CRITICAL,
					"Failed to write CW %d for page: %d\n",
					i, pg_addr);
			break;
		}
	}

	/* Wait for data to be available */
	qpic_nand_wait_for_data(DATA_CONSUMER_PIPE_INDEX);

	return nand_ret;
}

static int
qpic_nand_mark_badblock(uint32_t page)
{
	char empty_buf[NAND_CW_SIZE_8_BIT_ECC];

	memset(empty_buf, 0, NAND_CW_SIZE_8_BIT_ECC);

	/* Going to first page of the block */
	if (page & flash.num_pages_per_blk_mask)
		page = page - (page & flash.num_pages_per_blk_mask);

	return qpic_nand_write_page(page, NAND_CFG_RAW, empty_buf, 0);
}

static void
qpic_nand_non_onfi_probe(struct flash_info *flash)
{
	int dev_found = 0;
	unsigned index;
	uint32_t ecc_bits;

	/* Read the nand id. */
	qpic_nand_fetch_id(flash);

	/* Check if we support the device */
	for (index = 0; index < (ARRAY_SIZE(supported_flash)); index++)
	{
		if ((flash->id & supported_flash[index].mask) ==
		    (supported_flash[index].flash_id & (supported_flash[index].mask)))
		{
			dev_found = 1;
			break;
		}
	}

	if (dev_found)
	{
		flash->page_size = supported_flash[index].pagesize;
		flash->block_size = supported_flash[index].blksize;
		flash->spare_size = supported_flash[index].oobsize;
		ecc_bits = supported_flash[index].ecc_8_bits;

		/* Make sure that the block size and page size are defined. */
		ASSERT(flash->block_size);
		ASSERT(flash->page_size);

		flash->num_blocks = supported_flash[index].density;
		flash->num_blocks /= (flash->block_size);
		flash->num_pages_per_blk = flash->block_size / flash->page_size;
		flash->num_pages_per_blk_mask = flash->num_pages_per_blk - 1;

		/* Look for 8bit BCH ECC Nand, TODO: ECC Correctability >= 8 */
		if (ecc_bits)
			flash->ecc_width = NAND_WITH_8_BIT_ECC;
		else
			flash->ecc_width = NAND_WITH_4_BIT_ECC;

		flash->density = supported_flash[index].density;
		flash->widebus = supported_flash[index].widebus;

		return;
	}

	/* Flash device is not supported, print flash device info and halt */
	if (dev_found == 0)
	{
		dprintf(CRITICAL, "NAND device is not supported: nandid: 0x%x"
						  "maker=0x%02x device=0x%02x\n",
				flash->id,
				flash->vendor,
				flash->device);
		ASSERT(0);
	}

	dprintf(INFO, "nandid: 0x%x maker=0x%02x device=0x%02x page_size=%d\n",
			flash->id,
			flash->vendor,
			flash->device,
			flash->page_size);

	dprintf(INFO, "spare_size=%d block_size=%d num_blocks=%d\n",
			flash->spare_size,
			flash->block_size,
			flash->num_blocks);
}

void
qpic_nand_init(struct qpic_nand_init_config *config)
{
	uint32_t i;
	int nand_ret;

	nand_base = config->nand_base;

	qpic_bam_init(config);

	qpic_nand_non_onfi_probe(&flash);

	/* Save the RAW and read/write configs */
	qpic_nand_save_config(&flash);

	flash_spare_bytes = (unsigned char *)malloc(flash.spare_size);

	if (flash_spare_bytes == NULL)
	{
		dprintf(CRITICAL, "Failed to allocate memory for spare bytes\n");
		return;
	}

	/* Create a bad block table */
	bbtbl = (uint8_t *) malloc(sizeof(uint8_t) * flash.num_blocks);

	if (bbtbl == NULL)
	{
		dprintf(CRITICAL, "Failed to allocate memory for bad block table\n");
		return;
	}

	for (i = 0; i < flash.num_blocks; i++)
		bbtbl[i] = NAND_BAD_BLK_VALUE_NOT_READ;

	/* Set aside contiguous memory for reads/writes.
	 * This is needed as the BAM transfers only work with
	 * physically contiguous buffers.
	 * We will copy any data to be written/ to be read from
	 * nand to this buffer and this buffer will be submitted to BAM.
	 */
	rdwr_buf = (uint8_t*) malloc(flash.page_size + flash.spare_size);

	if (rdwr_buf == NULL)
	{
		dprintf(CRITICAL, "Failed to allocate memory for page reads or writes\n");
		return;
	}

}

unsigned
flash_page_size(void)
{
	return flash.page_size;
}

unsigned
flash_block_size(void)
{
    return flash.block_size;
}

unsigned
flash_num_blocks(void)
{
    return flash.num_blocks;
}

unsigned
flash_num_pages_per_blk(void)
{
	return flash.num_pages_per_blk;
}

unsigned
flash_spare_size(void)
{
    return flash.spare_size;
}

struct ptable *
flash_get_ptable(void)
{
	return flash_ptable;
}

void
qpic_nand_uninit()
{
	bam_pipe_reset(&bam, DATA_PRODUCER_PIPE_INDEX);
	bam_pipe_reset(&bam, DATA_CONSUMER_PIPE_INDEX);
	bam_pipe_reset(&bam, CMD_PIPE_INDEX);

}
void
flash_set_ptable(struct ptable *new_ptable)
{
	ASSERT(flash_ptable == NULL && new_ptable != NULL);
	flash_ptable = new_ptable;
}

/* Note: No support for raw reads. */
static int
qpic_nand_read_page(uint32_t page, unsigned char* buffer, unsigned char* spareaddr)
{
	struct cfg_params params;
	uint32_t ecc;
	uint32_t flash_sts[QPIC_NAND_MAX_CWS_IN_PAGE];
	uint32_t buffer_sts[QPIC_NAND_MAX_CWS_IN_PAGE];
	uint32_t addr_loc_0;
	uint32_t addr_loc_1;
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	uint32_t num_cmd_desc = 0;
	uint32_t num_data_desc = 0;
	uint32_t status;
	uint32_t i;
	int nand_ret = NANDC_RESULT_SUCCESS;
	uint8_t flags = 0;
	uint32_t *cmd_list_temp = NULL;

	uint32_t temp_status = 0;
	/* UD bytes in last CW is 512 - cws_per_page *4.
	 * Since each of the CW read earlier reads 4 spare bytes.
	 */
	uint16_t ud_bytes_in_last_cw = USER_DATA_BYTES_PER_CW - ((flash.cws_per_page - 1) << 2);
	uint16_t oob_bytes = DATA_BYTES_IN_IMG_PER_CW - ud_bytes_in_last_cw;

	params.addr0 = page << 16;
	params.addr1 = (page >> 16) & 0xff;
	params.cfg0 = cfg0;
	params.cfg1 = cfg1;
	params.cmd = NAND_CMD_PAGE_READ_ALL;
	params.exec = 1;
	ecc = ecc_bch_cfg;

	/* Read all the Data bytes in the first 3 CWs. */
	addr_loc_0 = NAND_RD_LOC_OFFSET(0);
	addr_loc_0 |= NAND_RD_LOC_SIZE(DATA_BYTES_IN_IMG_PER_CW);
	addr_loc_0 |= NAND_RD_LOC_LAST_BIT(1);


	addr_loc_1 = NAND_RD_LOC_OFFSET(ud_bytes_in_last_cw);
	addr_loc_1 |= NAND_RD_LOC_SIZE(oob_bytes);
	addr_loc_1 |= NAND_RD_LOC_LAST_BIT(1);

	status = qpic_nand_block_isbad(page);

	if (status)
		return status;

	/* Reset and Configure erased CW/page detection controller */
	qpic_nand_erased_status_reset(ce_array, BAM_DESC_LOCK_FLAG);

	/* Queue up the command and data descriptors for all the codewords in a page
	 * and do a single bam transfer at the end.*/
	for (i = 0; i < flash.cws_per_page; i++)
	{
		num_cmd_desc = 0;
		num_data_desc = 0;

		if (i == 0)
		{
			cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(&params, cmd_list_ptr);

			bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_ECC_CFG,(uint32_t)ecc, CE_WRITE_TYPE);
			cmd_list_ptr++;
		}
		else
			cmd_list_ptr_start = cmd_list_ptr;

		bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)params.cmd, CE_WRITE_TYPE);
		cmd_list_ptr++;

		if (i == flash.cws_per_page - 1)
		{
			addr_loc_0 = NAND_RD_LOC_OFFSET(0);
			addr_loc_0 |= NAND_RD_LOC_SIZE(ud_bytes_in_last_cw);
			addr_loc_0 |= NAND_RD_LOC_LAST_BIT(0);

			/* Write addr loc 1 only for the last CW. */
			bam_add_cmd_element(cmd_list_ptr, NAND_READ_LOCATION_n(1), (uint32_t)addr_loc_1, CE_WRITE_TYPE);
			cmd_list_ptr++;

			/* Add Data desc */
			bam_add_one_desc(&bam,
							 DATA_PRODUCER_PIPE_INDEX,
							 (unsigned char *)PA((addr_t)buffer),
							 ud_bytes_in_last_cw,
							 0);
			num_data_desc++;

			bam_add_one_desc(&bam,
							 DATA_PRODUCER_PIPE_INDEX,
							 (unsigned char *)PA((addr_t)spareaddr),
							 oob_bytes,
							 BAM_DESC_INT_FLAG);
			num_data_desc++;

			bam_sys_gen_event(&bam, DATA_PRODUCER_PIPE_INDEX, num_data_desc);
		}
		else
		{
			/* Add Data desc */
			bam_add_one_desc(&bam,
							 DATA_PRODUCER_PIPE_INDEX,
							 (unsigned char *)PA((addr_t)buffer),
							 DATA_BYTES_IN_IMG_PER_CW,
							 0);
			num_data_desc++;
			bam_sys_gen_event(&bam, DATA_PRODUCER_PIPE_INDEX, num_data_desc);
		}

		/* Write addr loc 0. */
		bam_add_cmd_element(cmd_list_ptr,
							NAND_READ_LOCATION_n(0),
							(uint32_t)addr_loc_0,
							CE_WRITE_TYPE);

		cmd_list_ptr++;
		bam_add_cmd_element(cmd_list_ptr,
							NAND_EXEC_CMD,
							(uint32_t)params.exec,
							CE_WRITE_TYPE);
		cmd_list_ptr++;

		/* Enqueue the desc for the above commands */
		bam_add_one_desc(&bam,
					 CMD_PIPE_INDEX,
					 (unsigned char*)cmd_list_ptr_start,
					 PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
					 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG);
		num_cmd_desc++;

		bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_STATUS, (uint32_t)PA((addr_t)&(flash_sts[i])), CE_READ_TYPE);

		cmd_list_temp = cmd_list_ptr;

		cmd_list_ptr++;

		bam_add_cmd_element(cmd_list_ptr, NAND_BUFFER_STATUS, (uint32_t)PA((addr_t)&(buffer_sts[i])), CE_READ_TYPE);
		cmd_list_ptr++;

		if (i == flash.cws_per_page - 1)
		{
			flags = BAM_DESC_CMD_FLAG | BAM_DESC_UNLOCK_FLAG;
		}
		else
			flags = BAM_DESC_CMD_FLAG;

		/* Enqueue the desc for the above command */
		bam_add_one_desc(&bam,
					CMD_PIPE_INDEX,
					(unsigned char*)PA((addr_t)cmd_list_temp),
					PA((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_temp),
					flags);
		num_cmd_desc++;

		buffer += DATA_BYTES_IN_IMG_PER_CW;

		/* Notify BAM HW about the newly added descriptors */
		bam_sys_gen_event(&bam, CMD_PIPE_INDEX, num_cmd_desc);
	}

	qpic_nand_wait_for_data(DATA_PRODUCER_PIPE_INDEX);

	/* Check status */
	for (i = 0; i < flash.cws_per_page ; i ++)
	{
		flash_sts[i] = qpic_nand_check_status(flash_sts[i]);
		if (flash_sts[i])
		{
			nand_ret = NANDC_RESULT_BAD_PAGE;
			dprintf(CRITICAL, "NAND page read failed. page: %x status %x\n", page, flash_sts[i]);
			goto qpic_nand_read_page_error;
		}
	}
qpic_nand_read_page_error:
return nand_ret;
}

/**
 * qpic_nand_read() - read data
 * @start_page: number of page to begin reading from
 * @num_pages: number of pages to read
 * @buffer: buffer where to store the read data
 * @spareaddr: buffer where to store spare data.
 * 		If null, spare data wont be read
 *
 * This function reads @num_pages starting from @start_page and stores the
 * read data in buffer. Note that it's in the caller responsibility to make
 * sure the read pages are all from same partition.
 *
 * Returns nand_result_t
 */
nand_result_t qpic_nand_read(uint32_t start_page, uint32_t num_pages,
		unsigned char* buffer, unsigned char* spareaddr)
{
	unsigned i = 0, ret = 0;

	if (!buffer) {
		dprintf(CRITICAL, "qpic_nand_read: buffer = null\n");
		return NANDC_RESULT_PARAM_INVALID;
	}
	while (i < num_pages) {
		ret = qpic_nand_read_page(start_page + i, buffer + flash.page_size * i,
				spareaddr);
		i++;
		if (ret == NANDC_RESULT_BAD_PAGE)
			qpic_nand_mark_badblock(start_page + i);
		if (ret) {
			dprintf(CRITICAL,
					"qpic_nand_read: reading page %d failed with %d err\n",
					start_page + i, ret);
			return ret;
		}
	}
	return NANDC_RESULT_SUCCESS;
}

/**
 * qpic_nand_write() - read data
 * @start_page: number of page to begin writing to
 * @num_pages: number of pages to write
 * @buffer: buffer to be written
 * @write_extra_bytes: true if spare data (ox 0xff) to be written
 *
 * This function writes @num_pages starting from @start_page. Note that it's
 * in the caller responsibility to make sure the written pages are all from
 * same partition.
 *
 * Returns nand_result_t
 */
nand_result_t qpic_nand_write(uint32_t start_page, uint32_t num_pages,
		unsigned char* buffer, unsigned  write_extra_bytes)
{
	int i = 0, ret = NANDC_RESULT_SUCCESS;
	uint32_t *spare = (unsigned *)flash_spare_bytes;
	uint32_t wsize;
	uint32_t spare_byte_count = 0;

	if (!buffer) {
		dprintf(CRITICAL, "qpic_nand_write: buffer = null\n");
		return NANDC_RESULT_PARAM_INVALID;
	}
	spare_byte_count = ((flash.cw_size * flash.cws_per_page)- flash.page_size);

	if (write_extra_bytes)
		wsize = flash.page_size + spare_byte_count;
	else
		wsize = flash.page_size;

	memset(spare, 0xff, (spare_byte_count / flash.cws_per_page));

	for (i = 0; i < (int)num_pages; i++) {
		memcpy(rdwr_buf, buffer, flash.page_size);
		if (write_extra_bytes) {
			memcpy(rdwr_buf + flash.page_size,
					buffer + flash.page_size, spare_byte_count);
			ret = qpic_nand_write_page(start_page + i,
					NAND_CFG, rdwr_buf, rdwr_buf + flash.page_size);
		} else {
			ret = qpic_nand_write_page(start_page + i,
					NAND_CFG, rdwr_buf, spare);
		}
		if (ret) {
			dprintf(CRITICAL,
					"flash_write: write failure @ page %d, block %d\n",
					start_page + i,
					(start_page + i) / flash.num_pages_per_blk);
			if (ret == NANDC_RESULT_BAD_PAGE)
				qpic_nand_mark_badblock(start_page + i);
			goto out;
		}
		buffer += wsize;
	}
out:
	return ret;
}

/* Function to read a flash partition.
 * ptn : Partition to read.
 * extra_per_page : Spare data to be read.
 * offset : Num of bytes offset into the partition.
 * data : Buffer to read the data into.
 * bytes : Num of bytes to be read.
 */
 /* TODO: call this func read_partition. */
int
flash_read_ext(struct ptentry *ptn,
			   unsigned extra_per_page,
			   unsigned offset,
			   void *data,
			   unsigned bytes)
{
	uint32_t page =
		(ptn->start * flash.num_pages_per_blk) + (offset / flash.page_size);
	uint32_t lastpage = (ptn->start + ptn->length) * flash.num_pages_per_blk;
	uint32_t count =
		(bytes + flash.page_size - 1 + extra_per_page) / (flash.page_size +
									 extra_per_page);
	uint32_t *spare = (unsigned *)flash_spare_bytes;
	uint32_t errors = 0;
	unsigned char *image = data;
	int result = 0;
	uint32_t current_block =
	    (page - (page & flash.num_pages_per_blk_mask)) / flash.num_pages_per_blk;
	uint32_t start_block = ptn->start;
	uint32_t start_block_count = 0;
	uint32_t isbad = 0;
	uint32_t current_page;

	/* Verify first byte is at page boundary. */
	if (offset & (flash.page_size - 1))
	{
		dprintf(CRITICAL, "Read request start not at page boundary: %d\n",
				offset);
		return NANDC_RESULT_PARAM_INVALID;
	}

	current_page = start_block * flash.num_pages_per_blk;
	/* Adjust page offset based on number of bad blocks from start to current page */
	if (start_block < current_block)
	{
		start_block_count = (current_block - start_block);
		while (start_block_count
			   && (start_block < (ptn->start + ptn->length)))
		{
			isbad = qpic_nand_block_isbad(current_page);
			if (isbad)
				page += flash.num_pages_per_blk;
			else
				start_block_count--;
			start_block++;
			current_page += flash.num_pages_per_blk;
		}
	}

	while ((page < lastpage) && !start_block_count)
	{
		if (count == 0)
		{
			dprintf(SPEW, "flash_read_image: success (%d errors)\n",
					errors);
			return NANDC_RESULT_SUCCESS;
		}

#if CONTIGUOUS_MEMORY
		result = qpic_nand_read_page(page, image, (unsigned char *) spare);
#else
		result = qpic_nand_read_page(page, rdwr_buf, (unsigned char *) spare);
#endif
		if (result == NANDC_RESULT_BAD_PAGE)
		{
			/* bad page, go to next page. */
			page++;
			errors++;
			continue;
		}
		else if (result == NANDC_RESULT_BAD_BLOCK)
		{
			/* bad block, go to next block same offset. */
			page += flash.num_pages_per_blk;
			errors++;
			continue;
		}

#ifndef CONTIGUOUS_MEMORY
		/* Copy the read page into correct location. */
		memcpy(image, rdwr_buf, flash.page_size);
#endif
		page++;
		image += flash.page_size;
		/* Copy spare bytes to image */
		if(extra_per_page)
		{
			memcpy(image, spare, extra_per_page);
			image += extra_per_page;
		}

		count -= 1;
	}

	/* could not find enough valid pages before we hit the end */
	dprintf(CRITICAL, "flash_read_image: failed (%d errors)\n", errors);
	return NANDC_RESULT_FAILURE;
}

int
flash_erase(struct ptentry *ptn)
{
	int ret = 0;

	ret = qpic_nand_blk_erase(ptn->start * flash.num_pages_per_blk);

	if (ret)
		dprintf(CRITICAL, "Erase operation failed \n");

	return ret;
}

int
flash_ecc_bch_enabled()
{
	return (flash.ecc_width == NAND_WITH_4_BIT_ECC)? 0 : 1;
}

int
flash_write(struct ptentry *ptn,
			unsigned write_extra_bytes,
			const void *data,
			unsigned bytes)
{
	uint32_t page = ptn->start * flash.num_pages_per_blk;
	uint32_t lastpage = (ptn->start + ptn->length) * flash.num_pages_per_blk;
	uint32_t *spare = (unsigned *)flash_spare_bytes;
	const unsigned char *image = data;
	uint32_t wsize;
	uint32_t spare_byte_count = 0;
	int r;

	spare_byte_count = ((flash.cw_size * flash.cws_per_page)- flash.page_size);

	if(write_extra_bytes)
		wsize = flash.page_size + spare_byte_count;
	else
		wsize = flash.page_size;

	memset(spare, 0xff, (spare_byte_count / flash.cws_per_page));

	while (bytes > 0)
	{
		if (bytes < wsize)
		{
			dprintf(CRITICAL,
					"flash_write_image: image undersized (%d < %d)\n",
					bytes,
					wsize);
			return -1;
		}

		if (page >= lastpage)
		{
			dprintf(CRITICAL, "flash_write_image: out of space\n");
			return -1;
		}

		if ((page & flash.num_pages_per_blk_mask) == 0)
		{
			if (qpic_nand_blk_erase(page))
			{
				dprintf(INFO,
					"flash_write_image: bad block @ %d\n",
					page / flash.num_pages_per_blk);

				page += flash.num_pages_per_blk;
				continue;
			}
		}

		memcpy(rdwr_buf, image, flash.page_size);

		if (write_extra_bytes)
		{
			memcpy(rdwr_buf + flash.page_size, image + flash.page_size, spare_byte_count);
			r = qpic_nand_write_page(page,
									 NAND_CFG,
									 rdwr_buf,
									 rdwr_buf + flash.page_size);
		}
		else
		{
			r = qpic_nand_write_page(page, NAND_CFG, rdwr_buf, spare);
		}

		if (r)
		{
			dprintf(INFO,
					"flash_write_image: write failure @ page %d (src %d)\n",
					page,
					image - (const unsigned char *)data);

			image -= (page & flash.num_pages_per_blk_mask) * wsize;
			bytes += (page & flash.num_pages_per_blk_mask) * wsize;
			page &= ~flash.num_pages_per_blk_mask;
			if (qpic_nand_blk_erase(page))
			{
				dprintf(INFO,
						"flash_write_image: erase failure @ page %d\n",
						page);
			}

			qpic_nand_mark_badblock(page);

			dprintf(INFO,
					"flash_write_image: restart write @ page %d (src %d)\n",
					page, image - (const unsigned char *)data);

			page += flash.num_pages_per_blk;
			continue;
		}
		page++;
		image += wsize;
		bytes -= wsize;
	}

	/* erase any remaining pages in the partition */
	page = (page + flash.num_pages_per_blk_mask) & (~flash.num_pages_per_blk_mask);

	while (page < lastpage)
	{
		if (qpic_nand_blk_erase(page))
		{
			dprintf(INFO, "flash_write_image: bad block @ %d\n",
					page / flash.num_pages_per_blk);
		}
		page += flash.num_pages_per_blk;
	}

	dprintf(INFO, "flash_write_image: success\n");
	return 0;
}

uint32_t nand_device_base()
{
	return nand_base;
}
