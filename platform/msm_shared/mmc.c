/* Copyright (c) 2010-2014, The Linux Foundation. All rights reserved.
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

#include <string.h>
#include <stdlib.h>
#include <debug.h>
#include <reg.h>
#include "mmc.h"
#include <partition_parser.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <bits.h>

#if MMC_BOOT_ADM
#include "adm.h"
#endif

#if MMC_BOOT_BAM
#include "bam.h"
#include "mmc_dml.h"
#endif

#ifndef NULL
#define NULL        0
#endif

#define USEC_PER_SEC           (1000000L)

#define MMC_BOOT_DATA_READ     0
#define MMC_BOOT_DATA_WRITE    1

static unsigned int mmc_boot_data_transfer(unsigned int *data_ptr,
						unsigned int data_len,
						unsigned char direction);

static unsigned int mmc_boot_fifo_read(unsigned int *data_ptr,
				       unsigned int data_len);

static unsigned int mmc_boot_fifo_write(unsigned int *data_ptr,
					unsigned int data_len);

static unsigned int mmc_boot_status_error(unsigned mmc_status);

#if MMC_BOOT_BAM

void mmc_boot_dml_init();

static void mmc_boot_dml_producer_trans_init(unsigned trans_end,
										     unsigned size);

static void mmc_boot_dml_consumer_trans_init();

static uint32_t mmc_boot_dml_chk_producer_idle();

static void mmc_boot_dml_wait_producer_idle();
static void mmc_boot_dml_wait_consumer_idle();
static void mmc_boot_dml_reset();
static int mmc_bam_init(uint32_t bam_base);
static int mmc_bam_transfer_data();
static unsigned int
mmc_boot_bam_setup_desc(unsigned int *data_ptr,
			    unsigned int data_len, unsigned char direction);
uint32_t mmc_page_size();

#endif


#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

/* data access time unit in ns */
static const unsigned int taac_unit[] =
    { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000 };
/* data access time value x 10 */
static const unsigned int taac_value[] =
    { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

/* data transfer rate in kbit/s */
static const unsigned int xfer_rate_unit[] =
    { 100, 1000, 10000, 100000, 0, 0, 0, 0 };
/* data transfer rate value x 10*/
static const unsigned int xfer_rate_value[] =
    { 0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80 };

unsigned char mmc_slot = 0;
unsigned int mmc_boot_mci_base = 0;

static unsigned char ext_csd_buf[512];
static unsigned char wp_status_buf[8];

#if MMC_BOOT_BAM

static uint32_t mmc_sdc_bam_base[] =
	{ MSM_SDC1_BAM_BASE, MSM_SDC2_BAM_BASE, MSM_SDC3_BAM_BASE, MSM_SDC4_BAM_BASE };

static uint32_t mmc_sdc_dml_base[] =
	{ MSM_SDC1_DML_BASE, MSM_SDC2_DML_BASE, MSM_SDC3_DML_BASE, MSM_SDC4_DML_BASE };

uint32_t dml_base;
static struct bam_instance bam;

#define MMC_BOOT_BAM_FIFO_SIZE           100

#define MMC_BOOT_BAM_READ_PIPE_INDEX     0
#define MMC_BOOT_BAM_WRITE_PIPE_INDEX    1

#define MMC_BOOT_BAM_READ_PIPE           0
#define MMC_BOOT_BAM_WRITE_PIPE          1

/* Align at BAM_DESC_SIZE boundary */
static struct bam_desc desc_fifo[MMC_BOOT_BAM_FIFO_SIZE] __attribute__ ((aligned(BAM_DESC_SIZE)));

#endif

int mmc_clock_enable_disable(unsigned id, unsigned enable);
int mmc_clock_get_rate(unsigned id);
int mmc_clock_set_rate(unsigned id, unsigned rate);

struct mmc_host mmc_host;
struct mmc_card mmc_card;

static unsigned int mmc_wp(unsigned int addr, unsigned int size,
			   unsigned char set_clear_wp);
static unsigned int mmc_boot_send_ext_cmd(struct mmc_card *card,
					  unsigned char *buf);
static unsigned int mmc_boot_read_reg(struct mmc_card *card,
				      unsigned int data_len,
				      unsigned int command,
				      unsigned int addr, unsigned int *out);

unsigned int SWAP_ENDIAN(unsigned int val)
{
	return ((val & 0xFF) << 24) |
	    (((val >> 8) & 0xFF) << 16) | (((val >> 16) & 0xFF) << 8) | (val >>
									 24);
}

uint32_t mmc_page_size()
{
	return BOARD_KERNEL_PAGESIZE;
}

void mmc_mclk_reg_wr_delay()
{
	if (mmc_host.mmc_cont_version)
	{
		/* Wait for the MMC_BOOT_MCI register write to go through. */
		while(readl(MMC_BOOT_MCI_STATUS2) & MMC_BOOT_MCI_MCLK_REG_WR_ACTIVE);
	}
	else
		udelay((1 + ((3 * USEC_PER_SEC) / (mmc_host.mclk_rate? mmc_host.mclk_rate : MMC_CLK_400KHZ))));
}

/* Sets a timeout for read operation.
 */
static unsigned int
mmc_boot_set_read_timeout(struct mmc_host *host,
			  struct mmc_card *card)
{
	unsigned int timeout_ns = 0;

	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	if ((card->type == MMC_BOOT_TYPE_MMCHC)
	    || (card->type == MMC_BOOT_TYPE_SDHC)) {
		card->rd_timeout_ns = 100000000;
	} else if ((card->type == MMC_BOOT_TYPE_STD_SD)
		   || (card->type == MMC_BOOT_TYPE_STD_MMC)) {
		timeout_ns = 10 * ((card->csd.taac_ns) +
				   (card->csd.nsac_clk_cycle /
				    (host->mclk_rate / 1000000000)));
		card->rd_timeout_ns = timeout_ns;
	} else {
		return MMC_BOOT_E_NOT_SUPPORTED;
	}

	dprintf(SPEW, " Read timeout set: %d ns\n", card->rd_timeout_ns);

	return MMC_BOOT_E_SUCCESS;
}

/* Sets a timeout for write operation.
 */
static unsigned int
mmc_boot_set_write_timeout(struct mmc_host *host,
			   struct mmc_card *card)
{
	unsigned int timeout_ns = 0;

	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	if ((card->type == MMC_BOOT_TYPE_MMCHC)
	    || (card->type == MMC_BOOT_TYPE_SDHC)) {
		card->wr_timeout_ns = 100000000;
	} else if (card->type == MMC_BOOT_TYPE_STD_SD
		   || (card->type == MMC_BOOT_TYPE_STD_MMC)) {
		timeout_ns = 10 * ((card->csd.taac_ns) +
				   (card->csd.nsac_clk_cycle /
				    (host->mclk_rate / 1000000000)));
		timeout_ns = timeout_ns << card->csd.r2w_factor;
		card->wr_timeout_ns = timeout_ns;
	} else {
		return MMC_BOOT_E_NOT_SUPPORTED;
	}

	dprintf(SPEW, " Write timeout set: %d ns\n", card->wr_timeout_ns);

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Decodes CSD response received from the card. Note that we have defined only
 * few of the CSD elements in csd structure. We'll only decode those values.
 */
static unsigned int
mmc_boot_decode_and_save_csd(struct mmc_card *card, unsigned int *raw_csd)
{
	unsigned int mmc_sizeof = 0;
	unsigned int mmc_unit = 0;
	unsigned int mmc_value = 0;
	unsigned int mmc_temp = 0;

	struct mmc_csd mmc_csd;

	if ((card == NULL) || (raw_csd == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	mmc_sizeof = sizeof(unsigned int) * 8;

	mmc_csd.cmmc_structure = UNPACK_BITS(raw_csd, 126, 2, mmc_sizeof);

	if ((card->type == MMC_BOOT_TYPE_SDHC)
	    || (card->type == MMC_BOOT_TYPE_STD_SD)) {
		/* Parse CSD according to SD card spec. */

		/* CSD register is little bit differnet for CSD version 2.0 High Capacity
		 * and CSD version 1.0/2.0 Standard memory cards. In Version 2.0 some of
		 * the fields have fixed values and it's not necessary for host to refer
		 * these fields in CSD sent by card */

		if (mmc_csd.cmmc_structure == 1) {
			/* CSD Version 2.0 */
			mmc_csd.card_cmd_class =
			    UNPACK_BITS(raw_csd, 84, 12, mmc_sizeof);
			mmc_csd.write_blk_len = 512;	/* Fixed value is 9 = 2^9 = 512 */
			mmc_csd.read_blk_len = 512;	/* Fixed value is 9 = 512 */
			mmc_csd.r2w_factor = 0x2;	/* Fixed value: 010b */
			mmc_csd.c_size_mult = 0;	/* not there in version 2.0 */
			mmc_csd.c_size =
			    UNPACK_BITS(raw_csd, 48, 22, mmc_sizeof);
			mmc_csd.nsac_clk_cycle =
			    UNPACK_BITS(raw_csd, 104, 8, mmc_sizeof) * 100;

//TODO: Investigate the nsac and taac. Spec suggests not using this for timeouts.

			mmc_unit = UNPACK_BITS(raw_csd, 112, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 115, 4, mmc_sizeof);
			mmc_csd.taac_ns =
			    (taac_value[mmc_value] * taac_unit[mmc_unit]) / 10;

			mmc_csd.erase_blk_len = 1;
			mmc_csd.read_blk_misalign = 0;
			mmc_csd.write_blk_misalign = 0;
			mmc_csd.read_blk_partial = 0;
			mmc_csd.write_blk_partial = 0;

			mmc_unit = UNPACK_BITS(raw_csd, 96, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 99, 4, mmc_sizeof);
			mmc_csd.tran_speed =
			    (xfer_rate_value[mmc_value] *
			     xfer_rate_unit[mmc_unit]) / 10;

			mmc_csd.wp_grp_size = 0x0;
			mmc_csd.wp_grp_enable = 0x0;
			mmc_csd.perm_wp =
			    UNPACK_BITS(raw_csd, 13, 1, mmc_sizeof);
			mmc_csd.temp_wp =
			    UNPACK_BITS(raw_csd, 12, 1, mmc_sizeof);

			/* Calculate the card capcity */
			card->capacity = (1 + mmc_csd.c_size) * 512 * 1024;
		} else {
			/* CSD Version 1.0 */
			mmc_csd.card_cmd_class =
			    UNPACK_BITS(raw_csd, 84, 12, mmc_sizeof);

			mmc_temp = UNPACK_BITS(raw_csd, 22, 4, mmc_sizeof);
			mmc_csd.write_blk_len = (mmc_temp > 8
						 && mmc_temp <
						 12) ? (1 << mmc_temp) : 512;

			mmc_temp = UNPACK_BITS(raw_csd, 80, 4, mmc_sizeof);
			mmc_csd.read_blk_len = (mmc_temp > 8
						&& mmc_temp <
						12) ? (1 << mmc_temp) : 512;

			mmc_unit = UNPACK_BITS(raw_csd, 112, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 115, 4, mmc_sizeof);
			mmc_csd.taac_ns =
			    (taac_value[mmc_value] * taac_unit[mmc_unit]) / 10;

			mmc_unit = UNPACK_BITS(raw_csd, 96, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 99, 4, mmc_sizeof);
			mmc_csd.tran_speed =
			    (xfer_rate_value[mmc_value] *
			     xfer_rate_unit[mmc_unit]) / 10;

			mmc_csd.nsac_clk_cycle =
			    UNPACK_BITS(raw_csd, 104, 8, mmc_sizeof) * 100;

			mmc_csd.r2w_factor =
			    UNPACK_BITS(raw_csd, 26, 3, mmc_sizeof);
			mmc_csd.sector_size =
			    UNPACK_BITS(raw_csd, 39, 7, mmc_sizeof) + 1;

			mmc_csd.erase_blk_len =
			    UNPACK_BITS(raw_csd, 46, 1, mmc_sizeof);
			mmc_csd.read_blk_misalign =
			    UNPACK_BITS(raw_csd, 77, 1, mmc_sizeof);
			mmc_csd.write_blk_misalign =
			    UNPACK_BITS(raw_csd, 78, 1, mmc_sizeof);
			mmc_csd.read_blk_partial =
			    UNPACK_BITS(raw_csd, 79, 1, mmc_sizeof);
			mmc_csd.write_blk_partial =
			    UNPACK_BITS(raw_csd, 21, 1, mmc_sizeof);

			mmc_csd.c_size_mult =
			    UNPACK_BITS(raw_csd, 47, 3, mmc_sizeof);
			mmc_csd.c_size =
			    UNPACK_BITS(raw_csd, 62, 12, mmc_sizeof);
			mmc_csd.wp_grp_size =
			    UNPACK_BITS(raw_csd, 32, 7, mmc_sizeof);
			mmc_csd.wp_grp_enable =
			    UNPACK_BITS(raw_csd, 31, 1, mmc_sizeof);
			mmc_csd.perm_wp =
			    UNPACK_BITS(raw_csd, 13, 1, mmc_sizeof);
			mmc_csd.temp_wp =
			    UNPACK_BITS(raw_csd, 12, 1, mmc_sizeof);

			/* Calculate the card capacity */
			mmc_temp =
			    (1 << (mmc_csd.c_size_mult + 2)) * (mmc_csd.c_size +
								1);
			card->capacity = mmc_temp * mmc_csd.read_blk_len;
		}
	} else {
		/* Parse CSD according to MMC card spec. */
		mmc_csd.spec_vers = UNPACK_BITS(raw_csd, 122, 4, mmc_sizeof);
		mmc_csd.card_cmd_class =
		    UNPACK_BITS(raw_csd, 84, 12, mmc_sizeof);
		mmc_csd.write_blk_len =
		    1 << UNPACK_BITS(raw_csd, 22, 4, mmc_sizeof);
		mmc_csd.read_blk_len =
		    1 << UNPACK_BITS(raw_csd, 80, 4, mmc_sizeof);
		mmc_csd.r2w_factor = UNPACK_BITS(raw_csd, 26, 3, mmc_sizeof);
		mmc_csd.c_size_mult = UNPACK_BITS(raw_csd, 47, 3, mmc_sizeof);
		mmc_csd.c_size = UNPACK_BITS(raw_csd, 62, 12, mmc_sizeof);
		mmc_csd.nsac_clk_cycle =
		    UNPACK_BITS(raw_csd, 104, 8, mmc_sizeof) * 100;

		mmc_unit = UNPACK_BITS(raw_csd, 112, 3, mmc_sizeof);
		mmc_value = UNPACK_BITS(raw_csd, 115, 4, mmc_sizeof);
		mmc_csd.taac_ns =
		    (taac_value[mmc_value] * taac_unit[mmc_unit]) / 10;

		mmc_csd.read_blk_misalign =
		    UNPACK_BITS(raw_csd, 77, 1, mmc_sizeof);
		mmc_csd.write_blk_misalign =
		    UNPACK_BITS(raw_csd, 78, 1, mmc_sizeof);
		mmc_csd.read_blk_partial =
		    UNPACK_BITS(raw_csd, 79, 1, mmc_sizeof);
		mmc_csd.write_blk_partial =
		    UNPACK_BITS(raw_csd, 21, 1, mmc_sizeof);
		mmc_csd.tran_speed = 0x00;	/* Ignore -- no use of this value. */

		mmc_csd.erase_grp_size =
		    UNPACK_BITS(raw_csd, 42, 5, mmc_sizeof);
		mmc_csd.erase_grp_mult =
		    UNPACK_BITS(raw_csd, 37, 5, mmc_sizeof);
		mmc_csd.wp_grp_size = UNPACK_BITS(raw_csd, 32, 5, mmc_sizeof);
		mmc_csd.wp_grp_enable = UNPACK_BITS(raw_csd, 31, 1, mmc_sizeof);
		mmc_csd.perm_wp = UNPACK_BITS(raw_csd, 13, 1, mmc_sizeof);
		mmc_csd.temp_wp = UNPACK_BITS(raw_csd, 12, 1, mmc_sizeof);

		/* Calculate the card capcity */
		if (mmc_csd.c_size != 0xFFF) {
			/* For cards less than or equal to 2GB */
			mmc_temp =
			    (1 << (mmc_csd.c_size_mult + 2)) * (mmc_csd.c_size +
								1);
			card->capacity = mmc_temp * mmc_csd.read_blk_len;
		} else {
			/* For cards greater than 2GB, Ext CSD register's SEC_COUNT
			 * is used to calculate the size.
			 */
			unsigned long long sec_count;

			sec_count = (ext_csd_buf[215] << 24) |
			    (ext_csd_buf[214] << 16) |
			    (ext_csd_buf[213] << 8) | ext_csd_buf[212];

			card->capacity = sec_count * 512;
		}
	}

	/* save the information in card structure */
	memcpy((struct mmc_csd *)&card->csd,
	       (struct mmc_csd *)&mmc_csd, sizeof(struct mmc_csd));

	dprintf(SPEW, "Decoded CSD fields:\n");
	dprintf(SPEW, "cmmc_structure: %d\n", mmc_csd.cmmc_structure);
	dprintf(SPEW, "card_cmd_class: %x\n", mmc_csd.card_cmd_class);
	dprintf(SPEW, "write_blk_len: %d\n", mmc_csd.write_blk_len);
	dprintf(SPEW, "read_blk_len: %d\n", mmc_csd.read_blk_len);
	dprintf(SPEW, "r2w_factor: %d\n", mmc_csd.r2w_factor);
	dprintf(SPEW, "sector_size: %d\n", mmc_csd.sector_size);
	dprintf(SPEW, "c_size_mult:%d\n", mmc_csd.c_size_mult);
	dprintf(SPEW, "c_size: %d\n", mmc_csd.c_size);
	dprintf(SPEW, "nsac_clk_cycle: %d\n", mmc_csd.nsac_clk_cycle);
	dprintf(SPEW, "taac_ns: %d\n", mmc_csd.taac_ns);
	dprintf(SPEW, "tran_speed: %d kbps\n", mmc_csd.tran_speed);
	dprintf(SPEW, "erase_blk_len: %d\n", mmc_csd.erase_blk_len);
	dprintf(SPEW, "read_blk_misalign: %d\n", mmc_csd.read_blk_misalign);
	dprintf(SPEW, "write_blk_misalign: %d\n", mmc_csd.write_blk_misalign);
	dprintf(SPEW, "read_blk_partial: %d\n", mmc_csd.read_blk_partial);
	dprintf(SPEW, "write_blk_partial: %d\n", mmc_csd.write_blk_partial);
	dprintf(SPEW, "Card Capacity: %llu Bytes\n", card->capacity);

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Decode CID sent by the card.
 */
static unsigned int
mmc_boot_decode_and_save_cid(struct mmc_card *card, unsigned int *raw_cid)
{
	struct mmc_cid mmc_cid;
	unsigned int mmc_sizeof = 0;
	int i = 0;

	if ((card == NULL) || (raw_cid == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	mmc_sizeof = sizeof(unsigned int) * 8;

	if ((card->type == MMC_BOOT_TYPE_SDHC)
	    || (card->type == MMC_BOOT_TYPE_STD_SD)) {
		mmc_cid.mid = UNPACK_BITS(raw_cid, 120, 8, mmc_sizeof);
		mmc_cid.oid = UNPACK_BITS(raw_cid, 104, 16, mmc_sizeof);

		for (i = 0; i < 5; i++) {
			mmc_cid.pnm[i] = (unsigned char)UNPACK_BITS(raw_cid,
								    (104 -
								     8 * (i +
									  1)),
								    8,
								    mmc_sizeof);
		}
		mmc_cid.pnm[5] = 0;
		mmc_cid.pnm[6] = 0;

		mmc_cid.prv = UNPACK_BITS(raw_cid, 56, 8, mmc_sizeof);
		mmc_cid.psn = UNPACK_BITS(raw_cid, 24, 32, mmc_sizeof);
		mmc_cid.month = UNPACK_BITS(raw_cid, 8, 4, mmc_sizeof);
		mmc_cid.year = UNPACK_BITS(raw_cid, 12, 8, mmc_sizeof);
		mmc_cid.year += 2000;
	} else {
		mmc_cid.mid = UNPACK_BITS(raw_cid, 120, 8, mmc_sizeof);
		mmc_cid.oid = UNPACK_BITS(raw_cid, 104, 16, mmc_sizeof);

		for (i = 0; i < 6; i++) {
			mmc_cid.pnm[i] = (unsigned char)UNPACK_BITS(raw_cid,
								    (104 -
								     8 * (i +
									  1)),
								    8,
								    mmc_sizeof);
		}
		mmc_cid.pnm[6] = 0;

		mmc_cid.prv = UNPACK_BITS(raw_cid, 48, 8, mmc_sizeof);
		mmc_cid.psn = UNPACK_BITS(raw_cid, 16, 32, mmc_sizeof);
		mmc_cid.month = UNPACK_BITS(raw_cid, 8, 4, mmc_sizeof);
		mmc_cid.year = UNPACK_BITS(raw_cid, 12, 4, mmc_sizeof);
		mmc_cid.year += 1997;
	}

	/* save it in card database */
	memcpy((struct mmc_cid *)&card->cid,
	       (struct mmc_cid *)&mmc_cid, sizeof(struct mmc_cid));

	dprintf(SPEW, "Decoded CID fields:\n");
	dprintf(SPEW, "Manufacturer ID: %x\n", mmc_cid.mid);
	dprintf(SPEW, "OEM ID: 0x%x\n", mmc_cid.oid);
	dprintf(SPEW, "Product Name: %s\n", mmc_cid.pnm);
	dprintf(SPEW, "Product revision: %d.%d\n", (mmc_cid.prv >> 4),
		(mmc_cid.prv & 0xF));
	dprintf(SPEW, "Product serial number: %X\n", mmc_cid.psn);
	dprintf(SPEW, "Manufacturing date: %d %d\n", mmc_cid.month,
		mmc_cid.year);

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Sends specified command to a card and waits for a response.
 */
static unsigned int mmc_boot_send_command(struct mmc_boot_command *cmd)
{
	unsigned int mmc_cmd = 0;
	unsigned int mmc_status = 0;
	unsigned int mmc_resp = 0;
	unsigned int mmc_return = MMC_BOOT_E_SUCCESS;
	unsigned int cmd_index = 0;
	int i = 0;

	/* basic check */
	if (cmd == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	/* 1. Write command argument to MMC_BOOT_MCI_ARGUMENT register */
	writel(cmd->argument, MMC_BOOT_MCI_ARGUMENT);

	/* 2. Set appropriate fields and write MMC_BOOT_MCI_CMD */
	/* 2a. Write command index in CMD_INDEX field */
	cmd_index = cmd->cmd_index;
	mmc_cmd |= cmd->cmd_index;
	/* 2b. Set RESPONSE bit to 1 for all cmds except CMD0
	 * And dont set RESPONSE bit for commands with no response
	 */
	if (cmd_index != CMD0_GO_IDLE_STATE && cmd->resp_type != MMC_BOOT_RESP_NONE) {
		mmc_cmd |= MMC_BOOT_MCI_CMD_RESPONSE;
	}

	/* 2c. Set LONGRESP bit to 1 for CMD2, CMD9 and CMD10 */
	if (IS_RESP_136_BITS(cmd->resp_type)) {
		mmc_cmd |= MMC_BOOT_MCI_CMD_LONGRSP;
	}

	/* 2d. Set INTERRUPT bit to 1 to disable command timeout */

	/* 2e. Set PENDING bit to 1 for CMD12 in the beginning of stream
	   mode data transfer */
	if (cmd->xfer_mode == MMC_BOOT_XFER_MODE_STREAM) {
		mmc_cmd |= MMC_BOOT_MCI_CMD_PENDING;
	}

	/* 2f. Set ENABLE bit to 1 */
	mmc_cmd |= MMC_BOOT_MCI_CMD_ENABLE;

	/* 2g. Set PROG_ENA bit */
	if (cmd->prg_enabled) {
		mmc_cmd |= MMC_BOOT_MCI_CMD_PROG_ENA;
	}

	/* 2h. Set MCIABORT bit to 1 for CMD12 when working with SDIO card */
	/* 2i. Set CCS_ENABLE bit to 1 for CMD61 when Command Completion Signal
	   of CE-ATA device is enabled */

	/* 2j. clear all static status bits */
	writel(MMC_BOOT_MCI_STATIC_STATUS, MMC_BOOT_MCI_CLEAR);

	/* 2k. Write to MMC_BOOT_MCI_CMD register */
	writel(mmc_cmd, MMC_BOOT_MCI_CMD);

	/* Wait for the MMC_BOOT_MCI_CMD write to go through. */
	mmc_mclk_reg_wr_delay();

	dprintf(SPEW, "Command sent: CMD%d MCI_CMD_REG:%x MCI_ARG:%x\n",
		cmd_index, mmc_cmd, cmd->argument);

	/* 3. Wait for interrupt or poll on the following bits of MCI_STATUS
	   register */
	do {
		/* 3a. Read MCI_STATUS register */
		while (readl(MMC_BOOT_MCI_STATUS) &
		       MMC_BOOT_MCI_STAT_CMD_ACTIVE) ;

		mmc_status = readl(MMC_BOOT_MCI_STATUS);

		/* 3b. CMD_SENT bit supposed to be set to 1 only after CMD0 is sent -
		   no response required. */
		if ((cmd->resp_type == MMC_BOOT_RESP_NONE) &&
		    (mmc_status & MMC_BOOT_MCI_STAT_CMD_SENT)) {
			break;
		}

		/* 3c. If CMD_TIMEOUT bit is set then no response was received */
		else if (mmc_status & MMC_BOOT_MCI_STAT_CMD_TIMEOUT) {
			mmc_return = MMC_BOOT_E_TIMEOUT;
			break;
		}
		/* 3d. If CMD_RESPONSE_END bit is set to 1 then command's response was
		   received and CRC check passed
		   Spcial case for ACMD41: it seems to always fail CRC even if
		   the response is valid
		 */
		else if ((mmc_status & MMC_BOOT_MCI_STAT_CMD_RESP_END)
			 || (cmd_index == CMD1_SEND_OP_COND)
			 || (cmd_index == CMD8_SEND_IF_COND)) {
			/* 3i. Read MCI_RESP_CMD register to verify that response index is
			   equal to command index */
			mmc_resp = readl(MMC_BOOT_MCI_RESP_CMD) & 0x3F;

			/* However, long response does not contain the command index field.
			 * In that case, response index field must be set to 111111b (0x3F) */
			if ((mmc_resp == cmd_index) ||
			    (cmd->resp_type == MMC_BOOT_RESP_R2 ||
			     cmd->resp_type == MMC_BOOT_RESP_R3 ||
			     cmd->resp_type == MMC_BOOT_RESP_R6 ||
			     cmd->resp_type == MMC_BOOT_RESP_R7)) {
				/* 3j. If resp index is equal to cmd index, read command resp
				   from MCI_RESPn registers
				   - MCI_RESP0/1/2/3 for CMD2/9/10
				   - MCI_RESP0 for all other registers */
				if (IS_RESP_136_BITS(cmd->resp_type)) {
					for (i = 0; i < 4; i++) {
						cmd->resp[3 - i] =
						    readl(MMC_BOOT_MCI_RESP_0 +
							  (i * 4));

					}
				} else {
					cmd->resp[0] =
					    readl(MMC_BOOT_MCI_RESP_0);
				}
			} else {
				/* command index mis-match */
				mmc_return = MMC_BOOT_E_CMD_INDX_MISMATCH;
			}

			dprintf(SPEW, "Command response received: %X\n",
				cmd->resp[0]);
			break;
		}

		/* 3e. If CMD_CRC_FAIL bit is set to 1 then cmd's response was recvd,
		   but CRC check failed. */
		else if ((mmc_status & MMC_BOOT_MCI_STAT_CMD_CRC_FAIL)) {
			if (cmd_index == ACMD41_SEND_OP_COND) {
				cmd->resp[0] = readl(MMC_BOOT_MCI_RESP_0);
			} else
				mmc_return = MMC_BOOT_E_CRC_FAIL;
			break;
		}

	}
	while (1);


	/* 2k. Write to MMC_BOOT_MCI_CMD register */
	writel(0, MMC_BOOT_MCI_CMD);

	/* Wait for the MMC_BOOT_MCI_CMD write to go through. */
	mmc_mclk_reg_wr_delay();

	return mmc_return;
}

/*
 * Reset all the cards to idle condition (CMD 0)
 */
static unsigned int mmc_boot_reset_cards(void)
{
	struct mmc_boot_command cmd;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	cmd.cmd_index = CMD0_GO_IDLE_STATE;
	cmd.argument = 0;	// stuff bits - ignored
	cmd.cmd_type = MMC_BOOT_CMD_BCAST;
	cmd.resp_type = MMC_BOOT_RESP_NONE;

	/* send command */
	return mmc_boot_send_command(&cmd);
}

/*
 * Send CMD1 to know whether the card supports host VDD profile or not.
 */
static unsigned int
mmc_boot_send_op_cond(struct mmc_host *host, struct mmc_card *card)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_resp = 0;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD1 format:
	 * [31] Busy bit
	 * [30:29] Access mode
	 * [28:24] reserved
	 * [23:15] 2.7-3.6
	 * [14:8]  2.0-2.6
	 * [7]     1.7-1.95
	 * [6:0]   reserved
	 */

	cmd.cmd_index = CMD1_SEND_OP_COND;
	cmd.argument = host->ocr;
	cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
	cmd.resp_type = MMC_BOOT_RESP_R3;

	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Now it's time to examine response */
	mmc_resp = cmd.resp[0];

	/* Response contains card's ocr. Update card's information */
	card->ocr = mmc_resp;

	/* Check the response for busy status */
	if (!(mmc_resp & MMC_BOOT_OCR_BUSY)) {
		return MMC_BOOT_E_CARD_BUSY;
	}

	if (mmc_resp & MMC_BOOT_OCR_SEC_MODE) {
		card->type = MMC_BOOT_TYPE_MMCHC;
	} else {
		card->type = MMC_BOOT_TYPE_STD_MMC;
	}
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Request any card to send its uniquie card identification (CID) number (CMD2).
 */
static unsigned int mmc_boot_all_send_cid(struct mmc_card *card)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD2 Format:
	 * [31:0] stuff bits
	 */
	cmd.cmd_index = CMD2_ALL_SEND_CID;
	cmd.argument = 0;
	cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
	cmd.resp_type = MMC_BOOT_RESP_R2;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Response contains card's 128 bits CID register */
	mmc_ret = mmc_boot_decode_and_save_cid(card, cmd.resp);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Ask any card to send it's relative card address (RCA).This RCA number is
 * shorter than CID and is used by the host to address the card in future (CMD3)
 */
static unsigned int mmc_boot_send_relative_address(struct mmc_card *card)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD3 Format:
	 * [31:0] stuff bits
	 */
	if (card->type == MMC_BOOT_TYPE_SDHC
	    || card->type == MMC_BOOT_TYPE_STD_SD) {
		cmd.cmd_index = CMD3_SEND_RELATIVE_ADDR;
		cmd.argument = 0;
		cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
		cmd.resp_type = MMC_BOOT_RESP_R6;

		/* send command */
		mmc_ret = mmc_boot_send_command(&cmd);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			return mmc_ret;
		}
		/* For sD, card will send RCA. Store it */
		card->rca = (cmd.resp[0] >> 16);
	} else {
		cmd.cmd_index = CMD3_SEND_RELATIVE_ADDR;
		cmd.argument = (MMC_RCA << 16);
		card->rca = (cmd.argument >> 16);
		cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
		cmd.resp_type = MMC_BOOT_RESP_R1;

		/* send command */
		mmc_ret = mmc_boot_send_command(&cmd);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			return mmc_ret;
		}
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Requests card to send it's CSD register's contents. (CMD9)
 */
static unsigned int
mmc_boot_send_csd(struct mmc_card *card, unsigned int *raw_csd)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_arg = 0;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD9 Format:
	 * [31:16] RCA
	 * [15:0] stuff bits
	 */
	mmc_arg |= card->rca << 16;

	cmd.cmd_index = CMD9_SEND_CSD;
	cmd.argument = mmc_arg;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R2;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* response contains the card csd */
	memcpy(raw_csd, cmd.resp, sizeof(cmd.resp));

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Selects a card by sending CMD7 to the card with its RCA.
 * If RCA field is set as 0 ( or any other address ),
 * the card will be de-selected. (CMD7)
 */
static unsigned int
mmc_boot_select_card(struct mmc_card *card, unsigned int rca)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_arg = 0;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD7 Format:
	 * [31:16] RCA
	 * [15:0] stuff bits
	 */
	mmc_arg |= rca << 16;

	cmd.cmd_index = CMD7_SELECT_DESELECT_CARD;
	cmd.argument = mmc_arg;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	/* If we are deselecting card, we do not get response */
	if (rca == card->rca && rca) {
		if (card->type == MMC_BOOT_TYPE_SDHC
		    || card->type == MMC_BOOT_TYPE_STD_SD)
			cmd.resp_type = MMC_BOOT_RESP_R1B;
		else
			cmd.resp_type = MMC_BOOT_RESP_R1;
	} else {
		cmd.resp_type = MMC_BOOT_RESP_NONE;
	}

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* As of now no need to look into a response. If it's required
	 * we'll explore later on */

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Send command to set block length.
 */
static unsigned int
mmc_boot_set_block_len(struct mmc_card *card, unsigned int block_len)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD16 Format:
	 * [31:0] block length
	 */

	cmd.cmd_index = CMD16_SET_BLOCKLEN;
	cmd.argument = block_len;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* If blocklength is larger than 512 bytes,
	 * the card sets BLOCK_LEN_ERROR bit. */
	if (cmd.resp[0] & MMC_BOOT_R1_BLOCK_LEN_ERR) {
		return MMC_BOOT_E_BLOCKLEN_ERR;
	}
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Requests the card to stop transmission of data.
 */
static unsigned int
mmc_boot_send_stop_transmission(struct mmc_card *card,
				unsigned int prg_enabled)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD12 Format:
	 * [31:0] stuff bits
	 */

	cmd.cmd_index = CMD12_STOP_TRANSMISSION;
	cmd.argument = 0;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1B;
	cmd.xfer_mode = MMC_BOOT_XFER_MODE_BLOCK;
	cmd.prg_enabled = prg_enabled;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Get the card's current status
 */
static unsigned int
mmc_boot_get_card_status(struct mmc_card *card,
			 unsigned int prg_enabled, unsigned int *status)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD13 Format:
	 * [31:16] RCA
	 * [15:0] stuff bits
	 */
	cmd.cmd_index = CMD13_SEND_STATUS;
	cmd.argument = card->rca << 16;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;
	cmd.prg_enabled = prg_enabled;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Checking ADDR_OUT_OF_RANGE error in CMD13 response */
	if (IS_ADDR_OUT_OF_RANGE(cmd.resp[0])) {
		return MMC_BOOT_E_FAILURE;
	}

	*status = cmd.resp[0];
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Decode type of error caused during read and write
 */
static unsigned int mmc_boot_status_error(unsigned mmc_status)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* If DATA_CRC_FAIL bit is set to 1 then CRC error was detected by
	   card/device during the data transfer */
	if (mmc_status & MMC_BOOT_MCI_STAT_DATA_CRC_FAIL) {
		mmc_ret = MMC_BOOT_E_DATA_CRC_FAIL;
	}
	/* If DATA_TIMEOUT bit is set to 1 then the data transfer time exceeded
	   the data timeout period without completing the transfer */
	else if (mmc_status & MMC_BOOT_MCI_STAT_DATA_TIMEOUT) {
		mmc_ret = MMC_BOOT_E_DATA_TIMEOUT;
	}
	/* If RX_OVERRUN bit is set to 1 then SDCC2 tried to receive data from
	   the card before empty storage for new received data was available.
	   Verify that bit FLOW_ENA in MCI_CLK is set to 1 during the data xfer. */
	else if (mmc_status & MMC_BOOT_MCI_STAT_RX_OVRRUN) {
		/* Note: We've set FLOW_ENA bit in MCI_CLK to 1. so no need to verify
		   for now */
		mmc_ret = MMC_BOOT_E_RX_OVRRUN;
	}
	/* If TX_UNDERRUN bit is set to 1 then SDCC2 tried to send data to
	   the card before new data for sending was available. Verify that bit
	   FLOW_ENA in MCI_CLK is set to 1 during the data xfer. */
	else if (mmc_status & MMC_BOOT_MCI_STAT_TX_UNDRUN) {
		/* Note: We've set FLOW_ENA bit in MCI_CLK to 1.so skipping it now */
		mmc_ret = MMC_BOOT_E_RX_OVRRUN;
	}
	return mmc_ret;
}

/*
 * Send ext csd command.
 */
static unsigned int
mmc_boot_send_ext_cmd(struct mmc_card *card, unsigned char *buf)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_reg = 0;
	unsigned int *mmc_ptr = (unsigned int *)buf;

	memset(buf, 0, 512);

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	/* set block len */
	if ((card->type != MMC_BOOT_TYPE_MMCHC)
	    && (card->type != MMC_BOOT_TYPE_SDHC)) {
		mmc_ret = mmc_boot_set_block_len(card, 512);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL,
				"Error No.%d: Failure setting block length for Card (RCA:%s)\n",
				mmc_ret, (char *)(card->rca));
			return mmc_ret;
		}
	}

	/* Set the FLOW_ENA bit of MCI_CLK register to 1 */
	mmc_reg = readl(MMC_BOOT_MCI_CLK);
	mmc_reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	writel(mmc_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Write data timeout period to MCI_DATA_TIMER register. */
	/* Data timeout period should be in card bus clock periods */
	mmc_reg = 0xFFFFFFFF;
	writel(mmc_reg, MMC_BOOT_MCI_DATA_TIMER);
	writel(512, MMC_BOOT_MCI_DATA_LENGTH);

	/* Set appropriate fields and write the MCI_DATA_CTL register. */
	/* Set ENABLE bit to 1 to enable the data transfer. */
	mmc_reg =
	    MMC_BOOT_MCI_DATA_ENABLE | MMC_BOOT_MCI_DATA_DIR | (512 <<
								MMC_BOOT_MCI_BLKSIZE_POS);

#if MMC_BOOT_ADM || MMC_BOOT_BAM
	mmc_reg |= MMC_BOOT_MCI_DATA_DM_ENABLE;
#endif

	writel(mmc_reg, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

#if MMC_BOOT_BAM
	/*  Setup SDCC BAM descriptors for Read operation. */
	mmc_ret = mmc_boot_bam_setup_desc(mmc_ptr, 512, MMC_BOOT_DATA_READ);
#endif

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));
	/* CMD8 */
	cmd.cmd_index = CMD8_SEND_EXT_CSD;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;
	cmd.xfer_mode = MMC_BOOT_XFER_MODE_BLOCK;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Read the transfer data from SDCC FIFO. */
	mmc_ret = mmc_boot_data_transfer(mmc_ptr, 512, MMC_BOOT_DATA_READ);

	/* Reset DPSM */
	writel(0, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

	return mmc_ret;
}

/*
 * Switch command
 */
static unsigned int
mmc_boot_switch_cmd(struct mmc_card *card,
		    unsigned access, unsigned index, unsigned value)
{

	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	uint32_t mmc_status;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD6 Format:
	 * [31:26] set to 0
	 * [25:24] access
	 * [23:16] index
	 * [15:8] value
	 * [7:3] set to 0
	 * [2:0] cmd set
	 */
	cmd.cmd_index = CMD6_SWITCH_FUNC;
	cmd.argument |= (access << 24);
	cmd.argument |= (index << 16);
	cmd.argument |= (value << 8);
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1B;
	cmd.prg_enabled = 1;

	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,"Send cmd6 failed\n");
		return mmc_ret;
	}

	/* Wait for interrupt or poll on PROG_DONE bit of MCI_STATUS register.
	 * If PROG_DONE bit is set to 1 it means that the card finished it programming
	 * and stopped driving DAT0 line to 0.
	 */
	do {
		mmc_status = readl(MMC_BOOT_MCI_STATUS);
		if (mmc_status & MMC_BOOT_MCI_STAT_PROG_DONE) {
			break;
		}
	}
	while (1);

	/* Check if the card completed the switch command processing */
	mmc_ret = mmc_boot_get_card_status(card, 0, &mmc_status);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Get card status failed\n");
		return mmc_ret;
	}

	if (MMC_BOOT_CARD_STATUS(mmc_status) != MMC_BOOT_TRAN_STATE)
	{
		dprintf(CRITICAL, "Switch cmd failed. Card not in tran state\n");
		mmc_ret = MMC_BOOT_E_FAILURE;
	}

	if (mmc_status & MMC_BOOT_SWITCH_FUNC_ERR_FLAG)
	{
		dprintf(CRITICAL, "Switch cmd failed. Switch Error.\n");
		mmc_ret = MMC_BOOT_E_FAILURE;
	}

	return mmc_ret;
}

/*
 * A command to set the data bus width for card. Set width to either
 */
static unsigned int
mmc_boot_set_bus_width(struct mmc_card *card, unsigned int width)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_reg = 0;
	unsigned int mmc_width = 0;

	if (width != MMC_BOOT_BUS_WIDTH_1_BIT) {
		mmc_width = width - 1;
	}

	mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_ACCESS_WRITE,
				      MMC_BOOT_EXT_CMMC_BUS_WIDTH, mmc_width);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Switch cmd failed\n");
		return mmc_ret;
	}

	/* set MCI_CLK accordingly */
	mmc_reg = readl(MMC_BOOT_MCI_CLK);
	mmc_reg &= ~MMC_BOOT_MCI_CLK_WIDEBUS_MODE;
	if (width == MMC_BOOT_BUS_WIDTH_1_BIT) {
		mmc_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_1_BIT;
	} else if (width == MMC_BOOT_BUS_WIDTH_4_BIT) {
		mmc_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_4_BIT;
	} else if (width == MMC_BOOT_BUS_WIDTH_8_BIT) {
		mmc_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_8_BIT;
	}

	writel(mmc_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Function to enable HS200 mode
 * 1. Set the clock frequency to 100 MHZ
 * 2. Set the bus width to 4/8 bit SDR as supported byt the target & host
 * 3. Set the HS_TIMING on ext_csd 185 for the card
 */
static uint32_t mmc_set_hs200_mode(struct mmc_host *host,
								   struct mmc_card *card)
{
	uint32_t mmc_ret = MMC_BOOT_E_SUCCESS;

	/* Set Clock @ 100 MHZ */
	clock_config_mmc(mmc_slot, host->caps.hs_clk_rate);
	host->mclk_rate = host->caps.hs_clk_rate;

	/* Set 4/8 bit SDR bus width */
	mmc_ret = mmc_boot_set_bus_width(card, host->caps.bus_width);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure to set wide bus for Card(RCA:%x)\n",
			mmc_ret, card->rca);
			return mmc_ret;
	}

	/* Setting HS200 in HS_TIMING using EXT_CSD (CMD6) */
	mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_ACCESS_WRITE,
				      MMC_BOOT_EXT_CMMC_HS_TIMING, 2);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Switch cmd returned failure %d\n", __LINE__);
		return mmc_ret;
	}

	return mmc_ret;
}

/*
 * Function to enable DDR mode
 * 1. Set the bus width to 8 bit DDR
 * 1. Set the clock frequency to 100 MHZ
 * 3. Set DDR mode in mci clk register
 * 4. Set Widebus enable in mci clock register
 */
static uint32_t mmc_set_ddr_mode(struct mmc_host *host,
								 struct mmc_card *card)
{
	uint8_t mmc_ret = MMC_BOOT_E_SUCCESS;
	uint32_t mmc_reg = 0;
	uint32_t width = 0;

	switch (host->caps.bus_width) {
		case MMC_BOOT_BUS_WIDTH_4_BIT:
			width = MMC_DDR_BUS_WIDTH_4_BIT;
			break;
		case MMC_BOOT_BUS_WIDTH_8_BIT:
			width = MMC_DDR_BUS_WIDTH_8_BIT;
			break;
		default:
			dprintf(CRITICAL, "Invalid bus width, DDR mode is not enabled\n");
			mmc_ret = MMC_BOOT_E_FAILURE;
			goto end;
	};

	/* Set width for 4/8 bit DDR bus width */
	mmc_ret = mmc_boot_set_bus_width(card, width);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure to set wide bus for Card(RCA:%x)\n",
			mmc_ret, card->rca);
		return mmc_ret;
	}

	/* Bump up the clock frequency */
	clock_config_mmc(mmc_slot, host->caps.hs_clk_rate);
	host->mclk_rate = host->caps.hs_clk_rate;

	/* Select DDR mode in mci register */
	mmc_reg = readl(MMC_BOOT_MCI_CLK);
	mmc_reg |= (MMC_MCI_DDR_MODE_EN << MMC_MCI_MODE_SELECT);

	writel(mmc_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Enable wide bus for DDR */
	mmc_reg = readl(MMC_BOOT_MCI_CLK);
	mmc_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_MODE;
	writel(mmc_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

end:
	return MMC_BOOT_E_SUCCESS;
}

/*
 * A command to start data read from card. Either a single block or
 * multiple blocks can be read. Multiple blocks read will continuously
 * transfer data from card to host unless requested to stop by issuing
 * CMD12 - STOP_TRANSMISSION.
 */
static unsigned int
mmc_boot_send_read_command(struct mmc_card *card,
			   unsigned int xfer_type, unsigned int data_addr)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD17/18 Format:
	 * [31:0] Data Address
	 */
	if (xfer_type == MMC_BOOT_XFER_MULTI_BLOCK) {
		cmd.cmd_index = CMD18_READ_MULTIPLE_BLOCK;
	} else {
		cmd.cmd_index = CMD17_READ_SINGLE_BLOCK;
	}

	cmd.argument = data_addr;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Response contains 32 bit Card status. Here we'll check
	   BLOCK_LEN_ERROR and ADDRESS_ERROR */
	if (cmd.resp[0] & MMC_BOOT_R1_BLOCK_LEN_ERR) {
		return MMC_BOOT_E_BLOCKLEN_ERR;
	}
	/* Misaligned address not matching block length */
	if (cmd.resp[0] & MMC_BOOT_R1_ADDR_ERR) {
		return MMC_BOOT_E_ADDRESS_ERR;
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * A command to start data write to card. Either a single block or
 * multiple blocks can be written. Multiple block write will continuously
 * transfer data from host to card unless requested to stop by issuing
 * CMD12 - STOP_TRANSMISSION.
 */
static unsigned int
mmc_boot_send_write_command(struct mmc_card *card,
			    unsigned int xfer_type, unsigned int data_addr)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD24/25 Format:
	 * [31:0] Data Address
	 */
	if (xfer_type == MMC_BOOT_XFER_MULTI_BLOCK) {
		cmd.cmd_index = CMD25_WRITE_MULTIPLE_BLOCK;
	} else {
		cmd.cmd_index = CMD24_WRITE_SINGLE_BLOCK;
	}

	cmd.argument = data_addr;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Response contains 32 bit Card status. Here we'll check
	   BLOCK_LEN_ERROR and ADDRESS_ERROR */
	if (cmd.resp[0] & MMC_BOOT_R1_BLOCK_LEN_ERR) {
		return MMC_BOOT_E_BLOCKLEN_ERR;
	}
	/* Misaligned address not matching block length */
	if (cmd.resp[0] & MMC_BOOT_R1_ADDR_ERR) {
		return MMC_BOOT_E_ADDRESS_ERR;
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Write data_len data to address specified by data_addr. data_len is
 * multiple of blocks for block data transfer.
 */
static unsigned int
mmc_boot_write_to_card(struct mmc_host *host,
		       struct mmc_card *card,
		       unsigned long long data_addr,
		       unsigned int data_len, unsigned int *in)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_status = 0;
	unsigned int mmc_reg = 0;
	unsigned int addr;
	unsigned int xfer_type;
	unsigned int status;

	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	/* Set block length. High Capacity MMC/SD card uses fixed 512 bytes block
	   length. So no need to send CMD16. */
	if ((card->type != MMC_BOOT_TYPE_MMCHC)
	    && (card->type != MMC_BOOT_TYPE_SDHC)) {
		mmc_ret = mmc_boot_set_block_len(card, card->wr_block_len);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL, "Error No.%d: Failure setting block length for Card\
                    (RCA:%s)\n", mmc_ret,
				(char *)(card->rca));
			return mmc_ret;
		}
	}

	/* use multi-block mode to transfer for data larger than a block */
	xfer_type =
	    (data_len >
	     card->
	     rd_block_len) ? MMC_BOOT_XFER_MULTI_BLOCK :
	    MMC_BOOT_XFER_SINGLE_BLOCK;

	/* For MMCHC/SDHC data address is specified in unit of 512B */
	addr = ((card->type != MMC_BOOT_TYPE_MMCHC)
		&& (card->type !=
		    MMC_BOOT_TYPE_SDHC)) ? (unsigned int)data_addr : (unsigned
								      int)
	    (data_addr / 512);

	/* Set the FLOW_ENA bit of MCI_CLK register to 1 */
	mmc_reg = readl(MMC_BOOT_MCI_CLK);
	mmc_reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	writel(mmc_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Write data timeout period to MCI_DATA_TIMER register */
	/* Data timeout period should be in card bus clock periods */
	/*TODO: Fix timeout value */
	mmc_reg = 0xFFFFFFFF;
	writel(mmc_reg, MMC_BOOT_MCI_DATA_TIMER);

	/* Write the total size of the transfer data to MCI_DATA_LENGTH register */
	writel(data_len, MMC_BOOT_MCI_DATA_LENGTH);

#if MMC_BOOT_BAM
		mmc_boot_bam_setup_desc(in, data_len, MMC_BOOT_DATA_WRITE);
#endif

	/* Send command to the card/device in order to start the write data xfer.
	   The possible commands are CMD24/25/53/60/61 */
	mmc_ret = mmc_boot_send_write_command(card, xfer_type, addr);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error No.%d: Failure sending write command to the\
                Card(RCA:%x)\n", mmc_ret,
			card->rca);
		return mmc_ret;
	}

	/* Set appropriate fields and write the MCI_DATA_CTL register */
	/* Set ENABLE bit to 1 to enable the data transfer. */
	mmc_reg = 0;
	mmc_reg |= MMC_BOOT_MCI_DATA_ENABLE;
	/* Clear DIRECTION bit to 0 to enable transfer from host to card */
	/* Clear MODE bit to 0 to enable block oriented data transfer. For
	   MMC cards only, if stream data transfer mode is desired, set
	   MODE bit to 1. */

	/* Set DM_ENABLE bit to 1 in order to enable DMA, otherwise set 0 */

#if MMC_BOOT_ADM || MMC_BOOT_BAM
	mmc_reg |= MMC_BOOT_MCI_DATA_DM_ENABLE;
#endif

	/* Write size of block to be used during the data transfer to
	   BLOCKSIZE field */
	mmc_reg |= card->wr_block_len << MMC_BOOT_MCI_BLKSIZE_POS;
	writel(mmc_reg, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

	/* write data to FIFO */
	mmc_ret =
	    mmc_boot_data_transfer(in, data_len, MMC_BOOT_DATA_WRITE);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error No.%d: Failure on data transfer from the \
                Card(RCA:%x)\n", mmc_ret,
			card->rca);
		/* In case of any failure happening for multi block transfer */
		if (xfer_type == MMC_BOOT_XFER_MULTI_BLOCK)
			mmc_boot_send_stop_transmission(card, 1);
		return mmc_ret;
	}

	/* Send command to the card/device in order to poll the de-assertion of
	   card/device BUSY condition. It is important to set PROG_ENA bit in
	   MCI_CLK register before sending the command. Possible commands are
	   CMD12/13. */
	if (xfer_type == MMC_BOOT_XFER_MULTI_BLOCK) {
		mmc_ret = mmc_boot_send_stop_transmission(card, 1);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL, "Error No.%d: Failure sending Stop Transmission \
                    command to the Card(RCA:%x)\n", mmc_ret,
				card->rca);
			return mmc_ret;
		}
	} else {
		mmc_ret = mmc_boot_get_card_status(card, 1, &status);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL,
				"Error No.%d: Failure getting card status of Card(RCA:%x)\n",
				mmc_ret, card->rca);
			return mmc_ret;
		}
	}

	/* Wait for interrupt or poll on PROG_DONE bit of MCI_STATUS register. If
	   PROG_DONE bit is set to 1 it means that the card finished it programming
	   and stopped driving DAT0 line to 0 */
	do {
		mmc_status = readl(MMC_BOOT_MCI_STATUS);
		if (mmc_status & MMC_BOOT_MCI_STAT_PROG_DONE) {
			break;
		}
	}
	while (1);

#if MMC_BOOT_BAM
	/* Wait for DML trasaction to end */
	mmc_boot_dml_wait_consumer_idle();
#endif

	/* Reset DPSM */
	writel(0, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Adjust the interface speed to optimal speed
 */
static unsigned int
mmc_boot_adjust_interface_speed(struct mmc_host *host,
				struct mmc_card *card)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* Setting HS_TIMING in EXT_CSD (CMD6) */
	mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_ACCESS_WRITE,
				      MMC_BOOT_EXT_CMMC_HS_TIMING, 1);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Switch cmd returned failure %d\n", __LINE__);
		return mmc_ret;
	}

	clock_config_mmc(mmc_slot, MMC_CLK_50MHZ);

	host->mclk_rate = MMC_CLK_50MHZ;

	return MMC_BOOT_E_SUCCESS;
}

static unsigned int
mmc_boot_set_block_count(struct mmc_card *card, unsigned int block_count)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if (card == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* CMD23 Format:
	 * [15:0] number of blocks
	 */

	cmd.cmd_index = CMD23_SET_BLOCK_COUNT;
	cmd.argument = block_count;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	if (cmd.resp[0] & MMC_BOOT_R1_OUT_OF_RANGE) {
		return MMC_BOOT_E_BLOCKLEN_ERR;
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Reads a data of data_len from the address specified. data_len
 * should be multiple of block size for block data transfer.
 */
static unsigned int
mmc_boot_read_from_card(struct mmc_host *host,
			struct mmc_card *card,
			unsigned long long data_addr,
			unsigned int data_len, unsigned int *out)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_reg = 0;
	unsigned int xfer_type;
	unsigned int addr = 0;
	unsigned char open_ended_read = 1;

	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	/* Set block length. High Capacity MMC/SD card uses fixed 512 bytes block
	   length. So no need to send CMD16. */
	if ((card->type != MMC_BOOT_TYPE_MMCHC)
	    && (card->type != MMC_BOOT_TYPE_SDHC)) {
		mmc_ret = mmc_boot_set_block_len(card, card->rd_block_len);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL,
				"Error No.%d: Failure setting block length for Card (RCA:%s)\n",
				mmc_ret, (char *)(card->rca));
			return mmc_ret;
		}
	}

	/* use multi-block mode to transfer for data larger than a block */
	xfer_type =
	    (data_len >
	     card->
	     rd_block_len) ? MMC_BOOT_XFER_MULTI_BLOCK :
	    MMC_BOOT_XFER_SINGLE_BLOCK;

	if (xfer_type == MMC_BOOT_XFER_MULTI_BLOCK) {
		if ((card->type == MMC_BOOT_TYPE_MMCHC)
		    || (card->type == MMC_BOOT_TYPE_STD_MMC)) {
			/* Virtio model does not support open-ended multi-block reads.
			 * So, block count must be set before sending read command.
			 * All SD cards do not support this command. Restrict this to MMC.
			 */
			mmc_ret =
			    mmc_boot_set_block_count(card,
						     data_len /
						     (card->rd_block_len));
			if (mmc_ret != MMC_BOOT_E_SUCCESS) {
				dprintf(CRITICAL,
					"Error No.%d: Failure setting read block count for Card (RCA:%s)\n",
					mmc_ret, (char *)(card->rca));
				return mmc_ret;
			}

			open_ended_read = 0;
		}
	}

	/* Set the FLOW_ENA bit of MCI_CLK register to 1 */
	/* Note: It's already enabled */

	/* If Data Mover is used for data transfer then prepare Command
	   List Entry and enable the Data mover to work with SDCC2 */

	/* Write data timeout period to MCI_DATA_TIMER register. */
	/* Data timeout period should be in card bus clock periods */
	mmc_reg = (unsigned long)(card->rd_timeout_ns / 1000000) *
	    (host->mclk_rate / 1000);
	mmc_reg += 1000;	// add some extra clock cycles to be safe
	mmc_reg = mmc_reg / 2;
	writel(mmc_reg, MMC_BOOT_MCI_DATA_TIMER);

	/* Write the total size of the transfer data to MCI_DATA_LENGTH
	   register. For block xfer it must be multiple of the block
	   size. */
	writel(data_len, MMC_BOOT_MCI_DATA_LENGTH);

	/* For MMCHC/SDHC data address is specified in unit of 512B */
	addr = ((card->type != MMC_BOOT_TYPE_MMCHC)
		&& (card->type !=
		    MMC_BOOT_TYPE_SDHC)) ? (unsigned int)data_addr : (unsigned
								      int)
	    (data_addr / 512);

	/* Set appropriate fields and write the MCI_DATA_CTL register. */
	/* Set ENABLE bit to 1 to enable the data transfer. */
	mmc_reg = 0;
	mmc_reg |= MMC_BOOT_MCI_DATA_ENABLE;
	/* Clear DIRECTION bit to 1 to enable transfer from card to host */
	mmc_reg |= MMC_BOOT_MCI_DATA_DIR;
	/* Clear MODE bit to 0 to enable block oriented data transfer. For
	   MMC cards only, if stream data transfer mode is desired, set
	   MODE bit to 1. */

	/* If DMA is to be used, Set DM_ENABLE bit to 1 */

#if MMC_BOOT_ADM || MMC_BOOT_BAM
	mmc_reg |= MMC_BOOT_MCI_DATA_DM_ENABLE;
#endif

	/* Write size of block to be used during the data transfer to
	   BLOCKSIZE field */
	mmc_reg |= (card->rd_block_len << MMC_BOOT_MCI_BLKSIZE_POS);
	writel(mmc_reg, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

#if MMC_BOOT_BAM
	/* Setup SDCC FIFO descriptors for Read operation. */
	mmc_ret = mmc_boot_bam_setup_desc(out, data_len, MMC_BOOT_DATA_READ);
#endif
	/* Send command to the card/device in order to start the read data
	   transfer. Possible commands: CMD17/18/53/60/61. */
	mmc_ret = mmc_boot_send_read_command(card, xfer_type, addr);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure sending read command to the Card(RCA:%x)\n",
			mmc_ret, card->rca);
		return mmc_ret;
	}

	/* Read the transfer data from SDCC FIFO. */
	mmc_ret = mmc_boot_data_transfer(out, data_len, MMC_BOOT_DATA_READ);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error No.%d: Failure on data transfer from the \
                Card(RCA:%x)\n", mmc_ret,
			card->rca);
		return mmc_ret;
	}

	/* In case a multiple block transfer was performed, send CMD12 to the
	   card/device in order to indicate the end of read data transfer */
	if ((xfer_type == MMC_BOOT_XFER_MULTI_BLOCK) && open_ended_read) {
		mmc_ret = mmc_boot_send_stop_transmission(card, 0);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL, "Error No.%d: Failure sending Stop Transmission \
                    command to the Card(RCA:%x)\n", mmc_ret,
				card->rca);
			return mmc_ret;
		}
	}

	/* Reset DPSM */
	writel(0, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Initialize host structure, set and enable clock-rate and power mode.
 */
unsigned int mmc_boot_init(struct mmc_host *host)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_pwr = 0;

	host->ocr = MMC_BOOT_OCR_27_36 | MMC_BOOT_OCR_SEC_MODE;
	host->cmd_retry = MMC_BOOT_MAX_COMMAND_RETRY;

	/* Disable sdhc mode */
	RMWREG32(MMC_BOOT_MCI_HC_MODE, SDHCI_HC_START_BIT, SDHCI_HC_WIDTH, 0);

	/* Wait for the MMC_BOOT_MCI_POWER write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Reset controller */
	RMWREG32(MMC_BOOT_MCI_POWER, CORE_SW_RST_START, CORE_SW_RST_WIDTH, 1);

	/* Wait for the MMC_BOOT_MCI_POWER write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Initialize any clocks needed for SDC controller */
	clock_init_mmc(mmc_slot);

	/* Save the verison on the mmc controller. */
	host->mmc_cont_version = readl(MMC_BOOT_MCI_VERSION);

	/* Setup initial freq to 400KHz */
	clock_config_mmc(mmc_slot, MMC_CLK_400KHZ);

	host->mclk_rate = MMC_CLK_400KHZ;

	/* set power mode */
	/* give some time to reach minimum voltate */

	mmc_pwr &= ~MMC_BOOT_MCI_PWR_UP;
	mmc_pwr |= MMC_BOOT_MCI_PWR_ON;
	mmc_pwr |= MMC_BOOT_MCI_PWR_UP;
	writel(mmc_pwr, MMC_BOOT_MCI_POWER);

	/* Wait for the MMC_BOOT_MCI_POWER write to go through. */
	mmc_mclk_reg_wr_delay();

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Performs card identification process:
 * - get card's unique identification number (CID)
 * - get(for sd)/set (for mmc) relative card address (RCA)
 * - get CSD
 * - select the card, thus transitioning it to Transfer State
 * - get Extended CSD (for mmc)
 */
static unsigned int
mmc_boot_identify_card(struct mmc_host *host, struct mmc_card *card)
{
	unsigned int mmc_return = MMC_BOOT_E_SUCCESS;
	unsigned int raw_csd[4];

	/* basic check */
	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	/* Ask card to send its unique card identification (CID) number (CMD2) */
	mmc_return = mmc_boot_all_send_cid(card);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No. %d: Failure getting card's CID number!\n",
			mmc_return);
		return mmc_return;
	}

	/* Ask card to send a relative card address (RCA) (CMD3) */
	mmc_return = mmc_boot_send_relative_address(card);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error No. %d: Failure getting card's RCA!\n",
			mmc_return);
		return mmc_return;
	}

	/* Get card's CSD register (CMD9) */
	mmc_return = mmc_boot_send_csd(card, raw_csd);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure getting card's CSD information!\n",
			mmc_return);
		return mmc_return;
	}

	/* Select the card (CMD7) */
	mmc_return = mmc_boot_select_card(card, card->rca);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure selecting the Card with RCA: %x\n",
			mmc_return, card->rca);
		return mmc_return;
	}

	/* Set the card status as active */
	card->status = MMC_BOOT_STATUS_ACTIVE;

	if ((card->type == MMC_BOOT_TYPE_STD_MMC)
	    || (card->type == MMC_BOOT_TYPE_MMCHC)) {
		/* For MMC cards, also get the extended csd */
		mmc_return = mmc_boot_send_ext_cmd(card, ext_csd_buf);

		if (mmc_return != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL,
				"Error No.%d: Failure getting card's ExtCSD information!\n",
				mmc_return);

			return mmc_return;
		}

	}

	/* Decode and save the CSD register */
	mmc_return = mmc_boot_decode_and_save_csd(card, raw_csd);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure decoding card's CSD information!\n",
			mmc_return);
		return mmc_return;
	}

	/* Once CSD is received, set read and write timeout value now itself */
	mmc_return = mmc_boot_set_read_timeout(host, card);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure setting Read Timeout value!\n",
			mmc_return);
		return mmc_return;
	}

	mmc_return = mmc_boot_set_write_timeout(host, card);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.%d: Failure setting Write Timeout value!\n",
			mmc_return);
		return mmc_return;
	}

	return MMC_BOOT_E_SUCCESS;
}

static unsigned int mmc_boot_send_app_cmd(unsigned int rca)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	cmd.cmd_index = CMD55_APP_CMD;
	cmd.argument = (rca << 16);
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	mmc_ret = mmc_boot_send_command(&cmd);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	return MMC_BOOT_E_SUCCESS;
}

static unsigned int mmc_boot_sd_init_card(struct mmc_card *card)
{
	unsigned int i, mmc_ret;
	unsigned int ocr_cmd_arg;
	struct mmc_boot_command cmd;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* Send CMD8 to set interface condition */
	for (i = 0; i < 3; i++) {
		cmd.cmd_index = CMD8_SEND_IF_COND;
		cmd.argument = MMC_BOOT_SD_HC_VOLT_SUPPLIED;
		cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
		cmd.resp_type = MMC_BOOT_RESP_R7;

		mmc_ret = mmc_boot_send_command(&cmd);
		if (mmc_ret == MMC_BOOT_E_SUCCESS) {
			if (cmd.resp[0] != MMC_BOOT_SD_HC_VOLT_SUPPLIED)
				return MMC_BOOT_E_FAILURE;
			/* Set argument for ACMD41 */
			ocr_cmd_arg = MMC_BOOT_SD_NEG_OCR | MMC_BOOT_SD_HC_HCS;
			break;
		}
		mdelay(1);
	}

	/* Send ACMD41 to set operating condition */
	/* Try for a max of 1 sec as per spec */
	for (i = 0; i < 20; i++) {
		mmc_ret = mmc_boot_send_app_cmd(0);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			return mmc_ret;
		}

		cmd.cmd_index = ACMD41_SEND_OP_COND;
		cmd.argument = ocr_cmd_arg;
		cmd.cmd_type = MMC_BOOT_CMD_BCAST_W_RESP;
		cmd.resp_type = MMC_BOOT_RESP_R3;

		mmc_ret = mmc_boot_send_command(&cmd);
		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			return mmc_ret;
		} else if (cmd.resp[0] & MMC_BOOT_SD_DEV_READY) {
			/* Check for HC */
			if (cmd.resp[0] & (1 << 30)) {
				card->type = MMC_BOOT_TYPE_SDHC;
			} else {
				card->type = MMC_BOOT_TYPE_STD_SD;
			}
			break;
		}
		mdelay(50);
	}
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Routine to initialize MMC card. It resets a card to idle state, verify operating
 * voltage and set the card inready state.
 */
static unsigned int
mmc_boot_init_card(struct mmc_host *host, struct mmc_card *card)
{
	unsigned int mmc_retry = 0;
	unsigned int mmc_return = MMC_BOOT_E_SUCCESS;

	/* basic check */
	if ((host == NULL) || (card == NULL)) {
		return MMC_BOOT_E_INVAL;
	}

	/* 1. Card Reset - CMD0 */
	mmc_return = mmc_boot_reset_cards();
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error No.:%d: Failure resetting MMC cards!\n",
			mmc_return);
		return mmc_return;
	}

	/* 2. Card Initialization process */

	/* Send CMD1 to identify and reject cards that do not match host's VDD range
	   profile. Cards sends its OCR register in response.
	 */
	mmc_retry = 0;
	do {
		mmc_return = mmc_boot_send_op_cond(host, card);
		/* Card returns busy status. We'll retry again! */
		if (mmc_return == MMC_BOOT_E_CARD_BUSY) {
			mmc_retry++;
			mdelay(1);
			continue;
		} else if (mmc_return == MMC_BOOT_E_SUCCESS) {
			break;
		} else {
			dprintf(CRITICAL,
				"Error No. %d: Failure Initializing MMC Card!\n",
				mmc_return);

			/* Check for sD card */
			mmc_return = mmc_boot_sd_init_card(card);
			return mmc_return;
		}
	}
	while (mmc_retry < host->cmd_retry);

	/* If card still returned busy status we are out of luck.
	 * Card cannot be initialized */
	if (mmc_return == MMC_BOOT_E_CARD_BUSY) {
		dprintf(CRITICAL, "Error No. %d: Card has busy status set. \
                Initialization not completed\n", mmc_return);
		return MMC_BOOT_E_CARD_BUSY;
	}

	card->block_size = BLOCK_SIZE;

	return MMC_BOOT_E_SUCCESS;
}

static unsigned int
mmc_boot_set_sd_bus_width(struct mmc_card *card, unsigned int width)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int sd_reg;

	mmc_ret = mmc_boot_send_app_cmd(card->rca);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* Send ACMD6 to set bus width */
	cmd.cmd_index = ACMD6_SET_BUS_WIDTH;
	/* 10 => 4 bit wide */
	if (width == MMC_BOOT_BUS_WIDTH_1_BIT) {
		cmd.argument = 0;
	} else if (width == MMC_BOOT_BUS_WIDTH_4_BIT) {
		cmd.argument = (1 << 1);
	}
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	mmc_ret = mmc_boot_send_command(&cmd);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* set MCI_CLK accordingly */
	sd_reg = readl(MMC_BOOT_MCI_CLK);
	sd_reg &= ~MMC_BOOT_MCI_CLK_WIDEBUS_MODE;
	if (width == MMC_BOOT_BUS_WIDTH_1_BIT) {
		sd_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_1_BIT;
	} else if (width == MMC_BOOT_BUS_WIDTH_4_BIT) {
		sd_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_4_BIT;
	} else if (width == MMC_BOOT_BUS_WIDTH_8_BIT) {
		sd_reg |= MMC_BOOT_MCI_CLK_WIDEBUS_8_BIT;
	}
	writel(sd_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	return MMC_BOOT_E_SUCCESS;
}

static unsigned int
mmc_boot_set_sd_hs(struct mmc_host *host, struct mmc_card *card)
{
	unsigned char sw_buf[64];
	unsigned int mmc_ret;

	/* CMD6 is a data transfer command. sD card returns 512 bits of data */
	/* Refer 4.3.10 of sD card specification 3.0 */
	mmc_ret =
	    mmc_boot_read_reg(card, 64, CMD6_SWITCH_FUNC, MMC_BOOT_SD_SWITCH_HS,
			      (unsigned int *)&sw_buf);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	clock_config_mmc(mmc_slot, MMC_CLK_50MHZ);

	host->mclk_rate = MMC_CLK_50MHZ;

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Performs initialization and identification of all the MMC cards connected
 * to the host.
 */

static unsigned int
mmc_boot_init_and_identify_cards(struct mmc_host *host,
				 struct mmc_card *card)
{
	unsigned int mmc_return = MMC_BOOT_E_SUCCESS;
	unsigned int status;
	uint8_t mmc_bus_width = 0;

	/* Basic check */
	if (host == NULL) {
		return MMC_BOOT_E_INVAL;
	}

	/* Initialize MMC card structure */
	card->status = MMC_BOOT_STATUS_INACTIVE;
	card->rd_block_len = MMC_BOOT_RD_BLOCK_LEN;
	card->wr_block_len = MMC_BOOT_WR_BLOCK_LEN;

	/* Start initialization process (CMD0 & CMD1) */
	mmc_return = mmc_boot_init_card(host, card);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		return mmc_return;
	}

	/* Identify (CMD2, CMD3 & CMD9) and select the card (CMD7) */
	mmc_return = mmc_boot_identify_card(host, card);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		return mmc_return;
	}

	if (card->type == MMC_BOOT_TYPE_SDHC
	    || card->type == MMC_BOOT_TYPE_STD_SD) {
		/* Setting sD card to high speed without checking card's capability.
		   Cards that do not support high speed may fail to boot */
		mmc_return = mmc_boot_set_sd_hs(host, card);
		if (mmc_return != MMC_BOOT_E_SUCCESS) {
			return mmc_return;
		}

		mmc_return =
		    mmc_boot_set_sd_bus_width(card, MMC_BOOT_BUS_WIDTH_4_BIT);
		if (mmc_return != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL,
				"Couldn't set 4bit mode for sD card\n");
			mmc_return =
			    mmc_boot_set_sd_bus_width(card,
						      MMC_BOOT_BUS_WIDTH_1_BIT);
			if (mmc_return != MMC_BOOT_E_SUCCESS) {
				dprintf(CRITICAL,
					"Error No.%d: Failed in setting bus width!\n",
					mmc_return);
				return mmc_return;
			}
		}
	} else {
		/* set interface speed */
		mmc_return = mmc_boot_adjust_interface_speed(host, card);
		if (mmc_return != MMC_BOOT_E_SUCCESS) {
			dprintf(CRITICAL,
				"Error No.%d: Error adjusting interface speed!\n",
				mmc_return);
			return mmc_return;
		}

		/* Enable HS200 mode by default if supported,
		 * else if DDR mode is supported enable it.
		 * else use default 4/8 bit mode
		 */
		if (card_supports_hs200_mode() && host->caps.hs200_mode) {
			mmc_return = mmc_set_hs200_mode(host, card);
			if (mmc_return != MMC_BOOT_E_SUCCESS) {
				dprintf(CRITICAL,
					"Error No.%d: Failure to set HS200 mode for Card(RCA:%x)\n",
					mmc_return, card->rca);
				return mmc_return;
			}
		} else if (card_supports_ddr_mode() && host->caps.ddr_mode) {
			mmc_return = mmc_set_ddr_mode(host, card);
			if (mmc_return != MMC_BOOT_E_SUCCESS) {
				dprintf(CRITICAL,
					"Error No.%d: Failure to set DDR mode for Card(RCA:%x)\n",
					mmc_return, card->rca);
				return mmc_return;
			}
		} else {
			mmc_return =
				mmc_boot_set_bus_width(card, host->caps.bus_width);
			if (mmc_return != MMC_BOOT_E_SUCCESS) {
				dprintf(CRITICAL,
					"Error No.%d: Failure to set wide bus for Card(RCA:%x)\n",
					mmc_return, card->rca);
				return mmc_return;
			}
		}
	}

	/* Just checking whether we're in TRAN state after changing speed and bus width */
	mmc_return = mmc_boot_get_card_status(card, 0, &status);
	if (mmc_return != MMC_BOOT_E_SUCCESS) {
		return mmc_return;
	}

	if (MMC_BOOT_CARD_STATUS(status) != MMC_BOOT_TRAN_STATE)
		return MMC_BOOT_E_FAILURE;

	return MMC_BOOT_E_SUCCESS;
}

void mmc_display_ext_csd(void)
{
	dprintf(SPEW, "part_config: %x\n", ext_csd_buf[179]);
	dprintf(SPEW, "erase_group_def: %x\n", ext_csd_buf[175]);
	dprintf(SPEW, "user_wp: %x\n", ext_csd_buf[171]);
}

void mmc_display_csd(void)
{
	dprintf(SPEW, "erase_grpsize: %d\n", mmc_card.csd.erase_grp_size);
	dprintf(SPEW, "erase_grpmult: %d\n", mmc_card.csd.erase_grp_mult);
	dprintf(SPEW, "wp_grpsize: %d\n", mmc_card.csd.wp_grp_size);
	dprintf(SPEW, "wp_grpen: %d\n", mmc_card.csd.wp_grp_enable);
	dprintf(SPEW, "perm_wp: %d\n", mmc_card.csd.perm_wp);
	dprintf(SPEW, "temp_wp: %d\n", mmc_card.csd.temp_wp);
}

/*
 * Entry point to MMC boot process
 */
unsigned int mmc_boot_main(unsigned char slot, unsigned int base)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	memset((struct mmc_host *)&mmc_host, 0,
	       sizeof(struct mmc_host));
	memset((struct mmc_card *)&mmc_card, 0,
	       sizeof(struct mmc_card));

	mmc_slot = slot;
	mmc_boot_mci_base = base;

	/* Get the capabilities for the host/target */
	target_mmc_caps(&mmc_host);

	/* Initialize necessary data structure and enable/set clock and power */
	dprintf(SPEW, " Initializing MMC host data structure and clock!\n");
	mmc_ret = mmc_boot_init(&mmc_host);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "MMC Boot: Error Initializing MMC Card!!!\n");
		return MMC_BOOT_E_FAILURE;
	}

#if MMC_BOOT_BAM

	mmc_ret = mmc_bam_init(mmc_sdc_bam_base[slot - 1]);
	dml_base = mmc_sdc_dml_base[slot - 1];
#endif

	/* Initialize and identify cards connected to host */
	mmc_ret = mmc_boot_init_and_identify_cards(&mmc_host, &mmc_card);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"MMC Boot: Failed detecting MMC/SDC @ slot%d\n", slot);
		return MMC_BOOT_E_FAILURE;
	}

	mmc_display_csd();
	mmc_display_ext_csd();

	mmc_ret = partition_read_table();
	return mmc_ret;
}

/*
 * MMC write function
 */
unsigned int
mmc_write(unsigned long long data_addr, unsigned int data_len, unsigned int *in)
{
	int val = 0;
	unsigned int write_size = ((unsigned)(0xFFFFFF / 512)) * 512;
	unsigned offset = 0;
	unsigned int *sptr = in;

	if (data_len % 512)
		data_len = ROUND_TO_PAGE(data_len, 511);

	while (data_len > write_size) {
		val = mmc_boot_write_to_card(&mmc_host, &mmc_card,
					     data_addr + offset, write_size,
					     sptr);
		if (val) {
			return val;
		}

		sptr += (write_size / sizeof(unsigned));
		offset += write_size;
		data_len -= write_size;
	}
	if (data_len) {
		val = mmc_boot_write_to_card(&mmc_host, &mmc_card,
					     data_addr + offset, data_len,
					     sptr);
	}
	return val;
}

/*
 * MMC read function
 */

unsigned int
mmc_read(unsigned long long data_addr, unsigned int *out, unsigned int data_len)
{
	int val = 0;
	unsigned int data_limit = mmc_card.rd_block_len * 0xffff;
	unsigned int this_len;

	do {
		this_len = (data_len > data_limit) ? data_limit : data_len;

		val =
		    mmc_boot_read_from_card(&mmc_host, &mmc_card, data_addr,
				    this_len, out);

		if (val != MMC_BOOT_E_SUCCESS)
			return val;

		data_len -= this_len;
		data_addr += this_len;
		out += (this_len / sizeof(*out));
	} while (data_len > 0);

	return val;
}

/*
 * Function to read registers from MMC or SD card
 */
static unsigned int
mmc_boot_read_reg(struct mmc_card *card,
		  unsigned int data_len,
		  unsigned int command, unsigned int addr, unsigned int *out)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_reg = 0;

	/* Set the FLOW_ENA bit of MCI_CLK register to 1 */
	mmc_reg = readl(MMC_BOOT_MCI_CLK);
	mmc_reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	writel(mmc_reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Write data timeout period to MCI_DATA_TIMER register. */
	/* Data timeout period should be in card bus clock periods */
	mmc_reg = 0xFFFFFFFF;
	writel(mmc_reg, MMC_BOOT_MCI_DATA_TIMER);
	writel(data_len, MMC_BOOT_MCI_DATA_LENGTH);

	/* Set appropriate fields and write the MCI_DATA_CTL register. */
	/* Set ENABLE bit to 1 to enable the data transfer. */
	mmc_reg =
	    MMC_BOOT_MCI_DATA_ENABLE | MMC_BOOT_MCI_DATA_DIR | (data_len <<
								MMC_BOOT_MCI_BLKSIZE_POS);

#if MMC_BOOT_ADM || MMC_BOOT_BAM
	mmc_reg |= MMC_BOOT_MCI_DATA_DM_ENABLE;
#endif

	writel(mmc_reg, MMC_BOOT_MCI_DATA_CTL);

	/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
	mmc_mclk_reg_wr_delay();

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	cmd.cmd_index = command;
	cmd.argument = addr;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Read the transfer data from SDCC FIFO. */
	mmc_ret =
	    mmc_boot_data_transfer(out, data_len, MMC_BOOT_DATA_READ);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error No.%d: Failure on data transfer from the \
                Card(RCA:%x)\n", mmc_ret,
			card->rca);
		return mmc_ret;
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Function to set/clear power-on write protection for the user area partitions
 */
static unsigned int
mmc_boot_set_clr_power_on_wp_user(struct mmc_card *card,
				  unsigned int addr,
				  unsigned int size, unsigned char set_clear_wp)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int wp_group_size, loop_count;
	unsigned int status;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	/* Disabling PERM_WP for USER AREA (CMD6) */
	mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_SET_BIT,
				      MMC_BOOT_EXT_USER_WP,
				      MMC_BOOT_US_PERM_WP_DIS);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	mmc_ret = mmc_boot_send_ext_cmd(card, ext_csd_buf);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Make sure power-on write protection for user area is not disabled
	   and permanent write protection for user area is not enabled */

	if ((IS_BIT_SET_EXT_CSD(MMC_BOOT_EXT_USER_WP, MMC_BOOT_US_PERM_WP_EN))
	    ||
	    (IS_BIT_SET_EXT_CSD(MMC_BOOT_EXT_USER_WP, MMC_BOOT_US_PWR_WP_DIS)))
	{
		return MMC_BOOT_E_FAILURE;
	}

	if (ext_csd_buf[MMC_BOOT_EXT_ERASE_GROUP_DEF]) {
		/* wp_group_size = 512KB * HC_WP_GRP_SIZE * HC_ERASE_GRP_SIZE.
		   Getting write protect group size in sectors here. */

		wp_group_size =
		    (512 * 1024) * ext_csd_buf[MMC_BOOT_EXT_HC_WP_GRP_SIZE] *
		    ext_csd_buf[MMC_BOOT_EXT_HC_ERASE_GRP_SIZE] /
		    MMC_BOOT_WR_BLOCK_LEN;
	} else {
		/* wp_group_size = (WP_GRP_SIZE + 1) * (ERASE_GRP_SIZE + 1)
		 * (ERASE_GRP_MULT + 1).
		 This is defined as the number of write blocks directly */

		wp_group_size = (card->csd.erase_grp_size + 1) *
		    (card->csd.erase_grp_mult + 1) * (card->csd.wp_grp_size +
						      1);
	}

	if (wp_group_size == 0) {
		return MMC_BOOT_E_FAILURE;
	}

	/* Setting POWER_ON_WP for USER AREA (CMD6) */
	mmc_ret = mmc_boot_switch_cmd(card, MMC_BOOT_SET_BIT,
				      MMC_BOOT_EXT_USER_WP,
				      MMC_BOOT_US_PWR_WP_EN);

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Calculating the loop count for sending SET_WRITE_PROTECT (CMD28)
	   or CLEAR_WRITE_PROTECT (CMD29).
	   We are write protecting the partitions in blocks of write protect
	   group sizes only */

	if (size % wp_group_size) {
		loop_count = (size / wp_group_size) + 1;
	} else {
		loop_count = (size / wp_group_size);
	}

	if (set_clear_wp)
		cmd.cmd_index = CMD28_SET_WRITE_PROTECT;
	else
		cmd.cmd_index = CMD29_CLEAR_WRITE_PROTECT;

	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1B;

	for (unsigned int i = 0; i < loop_count; i++) {
		/* Sending CMD28 for each WP group size
		   address is in sectors already */
		cmd.argument = (addr + (i * wp_group_size));

		mmc_ret = mmc_boot_send_command(&cmd);

		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			return mmc_ret;
		}

		/* Checking ADDR_OUT_OF_RANGE error in CMD28 response */
		if (IS_ADDR_OUT_OF_RANGE(cmd.resp[0])) {
			return MMC_BOOT_E_FAILURE;
		}

		/* Sending CMD13 to check card status */
		do {
			mmc_ret = mmc_boot_get_card_status(card, 0, &status);
			if (MMC_BOOT_CARD_STATUS(status) == MMC_BOOT_TRAN_STATE)
				break;
		}
		while ((mmc_ret == MMC_BOOT_E_SUCCESS) &&
		       (MMC_BOOT_CARD_STATUS(status) == MMC_BOOT_PROG_STATE));

		if (mmc_ret != MMC_BOOT_E_SUCCESS) {
			return mmc_ret;
		}
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Function to get Write Protect status of the given sector
 */
static unsigned int
mmc_boot_get_wp_status(struct mmc_card *card, unsigned int sector)
{
	unsigned int rc = MMC_BOOT_E_SUCCESS;
	memset(wp_status_buf, 0, 8);

	rc = mmc_boot_read_reg(card, 8, CMD31_SEND_WRITE_PROT_TYPE, sector,
			       (unsigned int *)wp_status_buf);
	return rc;
}

/*
 * Test Function for setting Write protect for given sector
 */
static unsigned int
mmc_wp(unsigned int sector, unsigned int size, unsigned char set_clear_wp)
{
	unsigned int rc = MMC_BOOT_E_SUCCESS;

	/* Checking whether group write protection feature is available */
	if (mmc_card.csd.wp_grp_enable)
	{
		rc = mmc_boot_get_wp_status(&mmc_card, sector);
		if (rc != MMC_BOOT_E_SUCCESS)
		{
			dprintf(CRITICAL, "Failure in getting wp_status (%u):%s:%u\n", rc, __FILE__, __LINE__);
			return rc;
		}
		rc = mmc_boot_set_clr_power_on_wp_user(&mmc_card, sector, size,
						       set_clear_wp);
		if (rc != MMC_BOOT_E_SUCCESS)
		{
			dprintf(CRITICAL, "Failure in setting power on wp user (%u):%s:%u\n", rc, __FILE__, __LINE__);
			return rc;
		}
		rc = mmc_boot_get_wp_status(&mmc_card, sector);
		if (rc != MMC_BOOT_E_SUCCESS)
		{
			dprintf(CRITICAL, "Failure in getting wp_status (%u)%s:%u\n", rc, __FILE__, __LINE__);
			return rc;
		}
	}
	else
		return MMC_BOOT_E_FAILURE;
}

void mmc_wp_test(void)
{
	unsigned int mmc_ret = 0;
	mmc_ret = mmc_wp(0xE06000, 0x5000, 1);
}

unsigned mmc_get_psn(void)
{
	return mmc_card.cid.psn;
}

/*
 * Read/write data from/to SDC FIFO.
 */
static unsigned int
mmc_boot_data_transfer(unsigned int *data_ptr,
			    unsigned int data_len, unsigned char direction)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

#if MMC_BOOT_ADM
	adm_result_t ret;
	adm_dir_t adm_dir;

	if (direction == MMC_BOOT_DATA_READ) {
		adm_dir = ADM_MMC_READ;
	} else {
		adm_dir = ADM_MMC_WRITE;
	}

	ret = adm_transfer_mmc_data(mmc_slot,
				    (unsigned char *)data_ptr, data_len,
				    adm_dir);

	if (ret != ADM_RESULT_SUCCESS) {
		dprintf(CRITICAL, "MMC ADM transfer error: %d\n", ret);
		mmc_ret = MMC_BOOT_E_FAILURE;
	}

#elif MMC_BOOT_BAM
	mmc_ret = mmc_bam_transfer_data(data_ptr, data_len, direction);
#else

	if (direction == MMC_BOOT_DATA_READ) {
		mmc_ret = mmc_boot_fifo_read(data_ptr, data_len);
	} else {
		mmc_ret = mmc_boot_fifo_write(data_ptr, data_len);
	}
#endif

	return mmc_ret;
}

/*
 * Read data to SDC FIFO.
 */
static unsigned int
mmc_boot_fifo_read(unsigned int *mmc_ptr, unsigned int data_len)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_status = 0;
	unsigned int mmc_count = 0;
	unsigned int read_error = MMC_BOOT_MCI_STAT_DATA_CRC_FAIL |
	    MMC_BOOT_MCI_STAT_DATA_TIMEOUT | MMC_BOOT_MCI_STAT_RX_OVRRUN;

	/* Read the data from the MCI_FIFO register as long as RXDATA_AVLBL
	   bit of MCI_STATUS register is set to 1 and bits DATA_CRC_FAIL,
	   DATA_TIMEOUT, RX_OVERRUN of MCI_STATUS register are cleared to 0.
	   Continue the reads until the whole transfer data is received */

	do {
		mmc_ret = MMC_BOOT_E_SUCCESS;
		mmc_status = readl(MMC_BOOT_MCI_STATUS);

		if (mmc_status & read_error) {
			mmc_ret = mmc_boot_status_error(mmc_status);
			break;
		}

		if (mmc_status & MMC_BOOT_MCI_STAT_RX_DATA_AVLBL) {
			unsigned read_count = 1;
			if (mmc_status & MMC_BOOT_MCI_STAT_RX_FIFO_HFULL) {
				read_count = MMC_BOOT_MCI_HFIFO_COUNT;
			}

			for (unsigned int i = 0; i < read_count; i++) {
				/* FIFO contains 16 32-bit data buffer on 16 sequential addresses */
				*mmc_ptr = readl(MMC_BOOT_MCI_FIFO +
						 (mmc_count %
						  MMC_BOOT_MCI_FIFO_SIZE));
				mmc_ptr++;
				/* increase mmc_count by word size */
				mmc_count += sizeof(unsigned int);
			}
			/* quit if we have read enough of data */
			if (mmc_count == data_len)
				break;
		} else if (mmc_status & MMC_BOOT_MCI_STAT_DATA_END) {
			break;
		}
	}
	while (1);

	return mmc_ret;
}

/*
 * Write data to SDC FIFO.
 */
static unsigned int
mmc_boot_fifo_write(unsigned int *mmc_ptr, unsigned int data_len)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int mmc_status = 0;
	unsigned int mmc_count = 0;
	unsigned int write_error = MMC_BOOT_MCI_STAT_DATA_CRC_FAIL |
	    MMC_BOOT_MCI_STAT_DATA_TIMEOUT | MMC_BOOT_MCI_STAT_TX_UNDRUN;
	unsigned int count = 0;
	unsigned int sz = 0;

	/* Write the transfer data to SDCC3 FIFO */
	do {
		mmc_status = readl(MMC_BOOT_MCI_STATUS);

		/* Bytes left to write */
		count = data_len - mmc_count;

		/* Break if whole data is transferred */
		if (!count)
			break;

		/* Write half FIFO or less (remaining) words in MCI_FIFO as long as either
		   TX_FIFO_EMPTY or TX_FIFO_HFULL bits of MCI_STATUS register are set. */
		if ((mmc_status & MMC_BOOT_MCI_STAT_TX_FIFO_EMPTY) ||
			(mmc_status & MMC_BOOT_MCI_STAT_TX_FIFO_HFULL)) {

			/* Write minimum of half FIFO and remaining words */
			sz = ((count >> 2) >  MMC_BOOT_MCI_HFIFO_COUNT) \
				 ? MMC_BOOT_MCI_HFIFO_COUNT : (count >> 2);

			for (int i = 0; i < sz; i++) {
				writel(*mmc_ptr, MMC_BOOT_MCI_FIFO);
				mmc_ptr++;
				/* increase mmc_count by word size */
				mmc_count += sizeof(unsigned int);
			}
		}
	}
	while (1);

	do
	{
		mmc_status = readl(MMC_BOOT_MCI_STATUS);
		if (mmc_status & write_error) {
			mmc_ret = mmc_boot_status_error(mmc_status);
			break;
		}
	}
	while (!(mmc_status & MMC_BOOT_MCI_STAT_DATA_END));

	return mmc_ret;
}

/*
 * CMD35_ERASE_GROUP_START
 */

static unsigned int
mmc_boot_send_erase_group_start(struct mmc_card *card,
				unsigned long long data_addr)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	if (card == NULL)
		return MMC_BOOT_E_INVAL;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	cmd.cmd_index = CMD35_ERASE_GROUP_START;
	cmd.argument = data_addr;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Checking for address error */
	if (IS_ADDR_OUT_OF_RANGE(cmd.resp[0])) {
		return MMC_BOOT_E_BLOCKLEN_ERR;
	}

	return MMC_BOOT_E_SUCCESS;

}

/*
 * CMD36 ERASE GROUP END
 */
static unsigned int
mmc_boot_send_erase_group_end(struct mmc_card *card,
			      unsigned long long data_addr)
{
	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	if (card == NULL)
		return MMC_BOOT_E_INVAL;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	cmd.cmd_index = CMD36_ERASE_GROUP_END;
	cmd.argument = data_addr;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1;

	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Checking for address error */
	if (IS_ADDR_OUT_OF_RANGE(cmd.resp[0])) {
		return MMC_BOOT_E_BLOCKLEN_ERR;
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * CMD38 ERASE
 */
static unsigned int mmc_boot_send_erase(struct mmc_card *card)
{

	struct mmc_boot_command cmd;
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned int status;

	if (card == NULL)
		return MMC_BOOT_E_INVAL;

	memset((struct mmc_boot_command *)&cmd, 0,
	       sizeof(struct mmc_boot_command));

	cmd.cmd_index = CMD38_ERASE;
	cmd.argument = 0x00000000;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1B;

	/* Checking if the card is in the transfer state */
	do {
		mmc_ret = mmc_boot_get_card_status(card, 0, &status);
		if (MMC_BOOT_CARD_STATUS(status) == MMC_BOOT_TRAN_STATE)
			break;
	}
	while ((mmc_ret == MMC_BOOT_E_SUCCESS) &&
	       (MMC_BOOT_CARD_STATUS(status) == MMC_BOOT_PROG_STATE));

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	/* Checking for write protect */
	if (cmd.resp[0] & MMC_BOOT_R1_WP_ERASE_SKIP) {
		dprintf(CRITICAL, "Write protect enabled for sector \n");
		return;
	}

	/* Checking if the erase operation for the card is compelete */
	do {
		mmc_ret = mmc_boot_get_card_status(card, 0, &status);
		if (MMC_BOOT_CARD_STATUS(status) == MMC_BOOT_TRAN_STATE)
			break;
	}
	while ((mmc_ret == MMC_BOOT_E_SUCCESS) &&
	       (MMC_BOOT_CARD_STATUS(status) == MMC_BOOT_PROG_STATE));

	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		return mmc_ret;
	}

	return MMC_BOOT_E_SUCCESS;
}

/*
 * Function to erase data on the eMMC card
 */
unsigned int
mmc_erase_card(unsigned long long data_addr, unsigned long long size)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;
	unsigned long long erase_grp_size;
	unsigned long long data_end = 0x00000000;
	unsigned long long loop_count;
	unsigned int out[512] = { 0 };

	/* Converting size to sectors */
	size = size / 512;

	if (ext_csd_buf[MMC_BOOT_EXT_ERASE_GROUP_DEF]) {
		erase_grp_size =
		    (512 * ext_csd_buf[MMC_BOOT_EXT_HC_ERASE_GRP_SIZE] * 1024);
		erase_grp_size = erase_grp_size / 512;
	} else {
		erase_grp_size = (mmc_card.csd.erase_grp_size + 1) *
		    (mmc_card.csd.erase_grp_mult + 1);
	}

	if (erase_grp_size == 0) {
		return MMC_BOOT_E_FAILURE;
	}

	if (size % erase_grp_size) {
		dprintf(CRITICAL, "Overflow beyond ERASE_GROUP_SIZE:%llu\n",
			(size % erase_grp_size));

	}
	loop_count = (size / erase_grp_size);
	/*
	 *In case the partition size is less than the erase_grp_size
	 0 is written to the first block of the partition.
	 */
	if (loop_count < 1) {
		mmc_ret = mmc_write(data_addr, 512, (unsigned int *)out);
		if (mmc_ret != MMC_BOOT_E_SUCCESS)
			return mmc_ret;
		else
			return MMC_BOOT_E_SUCCESS;
	} else {
		data_addr = ((mmc_card.type != MMC_BOOT_TYPE_MMCHC) &&
			     (mmc_card.type != MMC_BOOT_TYPE_SDHC))
		    ? (unsigned int)data_addr : (unsigned int)(data_addr / 512);
		data_end = data_addr + erase_grp_size * (loop_count - 1);
	}

	/* Sending CMD35 */
	mmc_ret = mmc_boot_send_erase_group_start(&mmc_card, data_addr);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error %d: Failure sending erase group start "
			"command to the card (RCA:%x)\n", mmc_ret,
			mmc_card.rca);
		return mmc_ret;
	}

	/* Sending CMD36 */
	mmc_ret = mmc_boot_send_erase_group_end(&mmc_card, data_end);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL, "Error %d: Failure sending erase group end "
			"command to the card (RCA:%x)\n", mmc_ret,
			mmc_card.rca);
		return mmc_ret;
	}

	/* Sending CMD38 */
	mmc_ret = mmc_boot_send_erase(&mmc_card);
	if (mmc_ret != MMC_BOOT_E_SUCCESS) {
		dprintf(CRITICAL,
			"Error %d: Failure sending erase command "
			"to the card (RCA:%x)\n", mmc_ret, mmc_card.rca);
		return mmc_ret;

	}

	dprintf(CRITICAL, "ERASE SUCCESSFULLY COMPLETED\n");
	return MMC_BOOT_E_SUCCESS;
}

/*
 * Disable MCI clk
 */
void mmc_boot_mci_clk_disable()
{
	uint32_t reg = 0;

	reg |= MMC_BOOT_MCI_CLK_DISABLE;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();
}

/*
 * Enable MCI CLK
 */
void mmc_boot_mci_clk_enable()
{
	uint32_t reg = 0;

	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Enable power save */
	reg |= MMC_BOOT_MCI_CLK_PWRSAVE;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();
}

#if MMC_BOOT_BAM

void mmc_boot_dml_init()
{
	uint32_t val = 0;

	/* Initialize s/w reset for DML core */
	mmc_boot_dml_reset();

	/* Program DML config:
	 * 1. Disable producer and consumer CRCI.
	 * 2. Set Bypass mode for the DML for Direct access.
	 */
	val = 0;
	val |= 1 >> SDCC_BYPASS_SHIFT;
	writel(val, SDCC_DML_CONFIG(dml_base));

	/* Program consumer logic size:
	 * This is for handshaking between the BAM and the DML blocks.
	 */
	writel(4096, SDCC_DML_CONSUMER_PIPE_LOGICAL_SIZE(dml_base));

	/* Program producer logic size
	 * This is for handshaking between the BAM and the DML blocks.
	 */
	writel(4096, SDCC_DML_PRODUCER_PIPE_LOGICAL_SIZE(dml_base));


	/* Write the pipe id numbers. */
	val = 0;
	val |= bam.pipe[MMC_BOOT_BAM_READ_PIPE_INDEX].pipe_num << SDCC_PRODUCER_PIPE_ID_SHIFT;
	val |= bam.pipe[MMC_BOOT_BAM_WRITE_PIPE_INDEX].pipe_num << SDCC_CONSUMER_PIPE_ID_SHIFT;

	writel(val, SDCC_DML_PIPE_ID(dml_base));

}

/* Function to set up SDCC dml for System producer transaction. */
static void mmc_boot_dml_consumer_trans_init()
{
	uint32_t val = 0;

	val = 0 << SDCC_PRODUCER_CRCI_SEL_SHIFT;
	val |= 1 << SDCC_CONSUMER_CRCI_SEL_SHIFT;
	writel(val, SDCC_DML_CONFIG(dml_base));


	/* Start the consumer transaction */
	writel(1, SDCC_DML_CONSUMER_START(dml_base));

}

/* Function to set up SDCC dml for System consumer transaction.
 * trans_end: 1: Assert DML trasaction signal
 *                 at the end of transaction.
 *                 0: Do not assert DML transaction signal.
 * size: Transaction size
 */
static void mmc_boot_dml_producer_trans_init(unsigned trans_end,
                                             unsigned size)
{
	uint32_t val = 0;

	val = 1 << SDCC_PRODUCER_CRCI_SEL_SHIFT;
	val |= 0 << SDCC_CONSUMER_CRCI_SEL_SHIFT;
	val |=  trans_end << SDCC_PRODUCER_TRANS_END_EN_SHIFT;
	writel(val, SDCC_DML_CONFIG(dml_base));

	/* Set block size */
    writel(BLOCK_SIZE, SDCC_DML_PRODUCER_BAM_BLOCK_SIZE(dml_base));

	/* Write transaction size */
	writel(size, SDCC_DML_PRODUCER_BAM_TRANS_SIZE(dml_base));

	/* Start the producer transaction */
	writel(1, SDCC_DML_PRODUCER_START(dml_base));
}

/* Function to check producer idle status of the DML.
 * return value: 1: Producer is idle
 *                    0: Producer is busy
 */
static uint32_t mmc_boot_dml_chk_producer_idle()
{
	uint32_t val = 0;

	val = readl(SDCC_DML_STATUS(dml_base));

	/* Read only the producer idle status */
	val &= (1 << SDCC_DML_PRODUCER_IDLE_SHIFT);

	return val;
}

/* Function to clear transaction complete flag */
static void mmc_boot_dml_clr_trans_complete()
{
	uint32_t val;

	val = readl(SDCC_DML_CONFIG(dml_base));

	val &=  ~(1 << SDCC_PRODUCER_TRANS_END_EN_SHIFT);
	writel(val, SDCC_DML_CONFIG(dml_base));
}

/* Blocking function to wait until DML is idle. */
static void mmc_boot_dml_wait_producer_idle()
{
	while(!(readl(SDCC_DML_STATUS(dml_base)) & 1));
}

/* Blocking function to wait until DML is idle. */
static void mmc_boot_dml_wait_consumer_idle()
{
	while(!(readl(SDCC_DML_STATUS(dml_base)) & (1 << SDCC_DML_CONSUMER_IDLE_SHIFT)));
}

/* Initialize S/W reset */
static void mmc_boot_dml_reset()
{
	/* Initialize s/w reset for DML core */
	writel(1, SDCC_DML_SW_RESET(dml_base));

}

static int mmc_bam_init(uint32_t bam_base)
{

	uint32_t mmc_ret = MMC_BOOT_E_SUCCESS;

	bam.base = bam_base;
	/* Read pipe parameter initializations. */
	bam.pipe[MMC_BOOT_BAM_READ_PIPE_INDEX].pipe_num = MMC_BOOT_BAM_READ_PIPE;
	/* System consumer */
	bam.pipe[MMC_BOOT_BAM_READ_PIPE_INDEX].trans_type = BAM2SYS;
	/* Set the descriptor FIFO start ptr */
	bam.pipe[MMC_BOOT_BAM_READ_PIPE_INDEX].fifo.head = desc_fifo;
	/* Set the descriptor FIFO lengths */
	bam.pipe[MMC_BOOT_BAM_READ_PIPE_INDEX].fifo.size = MMC_BOOT_BAM_FIFO_SIZE;

	/* Write pipe parameter initializations.*/
	bam.pipe[MMC_BOOT_BAM_WRITE_PIPE_INDEX].pipe_num = MMC_BOOT_BAM_WRITE_PIPE;
	/* System producer */
	bam.pipe[MMC_BOOT_BAM_WRITE_PIPE_INDEX].trans_type = SYS2BAM;
	/* Write fifo uses the same fifo as read */
	bam.pipe[MMC_BOOT_BAM_WRITE_PIPE_INDEX].fifo.head = desc_fifo;
	/* Set the descriptor FIFO lengths */
	bam.pipe[MMC_BOOT_BAM_WRITE_PIPE_INDEX].fifo.size = MMC_BOOT_BAM_FIFO_SIZE;

	/* Programs the minimum threshold for BAM transfer*/
	bam.threshold = BLOCK_SIZE;

	/* Initialize MMC BAM */
	bam_init(&bam);

	/* Initialize BAM MMC read pipe */
	bam_sys_pipe_init(&bam, MMC_BOOT_BAM_READ_PIPE_INDEX);

	mmc_ret = bam_pipe_fifo_init(&bam, bam.pipe[MMC_BOOT_BAM_READ_PIPE_INDEX].pipe_num);

	if (mmc_ret)
	{
		dprintf(CRITICAL, "MMC: BAM Read pipe fifo init error\n");
		goto mmc_bam_init_error;
	}

	/* Initialize BAM MMC write pipe */
	bam_sys_pipe_init(&bam, MMC_BOOT_BAM_WRITE_PIPE_INDEX);

	mmc_ret = bam_pipe_fifo_init(&bam, bam.pipe[MMC_BOOT_BAM_WRITE_PIPE_INDEX].pipe_num);

	if (mmc_ret)
	{
		dprintf(CRITICAL, "MMC: BAM Write pipe fifo init error\n");
		goto mmc_bam_init_error;
	}

	mmc_boot_dml_init();

	mmc_bam_init_error:

	return mmc_ret;
}

static int mmc_bam_transfer_data(unsigned int *data_ptr,
                                 unsigned int data_len,
			                     unsigned int dir)
{
	uint32_t mmc_ret;
	uint32_t offset;

	mmc_ret = MMC_BOOT_E_SUCCESS;

	if(dir == MMC_BOOT_DATA_READ)
	{
		/* Check BAM IRQ status reg to verify the desc has been processed */
		mmc_ret = bam_wait_for_interrupt(&bam,
					MMC_BOOT_BAM_READ_PIPE_INDEX, P_PRCSD_DESC_EN_MASK);

		if (mmc_ret != BAM_RESULT_SUCCESS)
		{
			dprintf(CRITICAL, "BAM transfer error \n");
			mmc_ret = MMC_BOOT_E_FAILURE;
			goto mmc_bam_transfer_err;
		}

		mmc_boot_dml_wait_producer_idle();

		/* Update BAM pipe fifo offsets */
		offset = bam_read_offset_update(&bam, MMC_BOOT_BAM_READ_PIPE_INDEX);

		/* Reset DPSM */
		writel(0, MMC_BOOT_MCI_DATA_CTL);

		/* Wait for the MMC_BOOT_MCI_DATA_CTL write to go through. */
		mmc_mclk_reg_wr_delay();

		dprintf(SPEW, "Offset value is %d \n", offset);
	}
	else
	{
		/* Check BAM IRQ status reg to verify the desc has been processed */
		mmc_ret = bam_wait_for_interrupt(&bam,
					MMC_BOOT_BAM_WRITE_PIPE_INDEX, P_TRNSFR_END_EN_MASK);

		if (mmc_ret != BAM_RESULT_SUCCESS)
		{
			dprintf(CRITICAL, "BAM transfer error \n");
			mmc_ret = MMC_BOOT_E_FAILURE;
			goto mmc_bam_transfer_err;
		}

		/* Update BAM pipe fifo offsets */
		offset = bam_read_offset_update(&bam, MMC_BOOT_BAM_WRITE_PIPE_INDEX);

		dprintf(SPEW, "Offset value is %d \n", offset);
	}

mmc_bam_transfer_err:

	return mmc_ret;
}

static unsigned int
mmc_boot_bam_setup_desc(unsigned int *data_ptr,
                        unsigned int data_len,
                        unsigned char direction)
{
	unsigned int mmc_ret = MMC_BOOT_E_SUCCESS;

	if (direction == MMC_BOOT_DATA_READ)
	{
		mmc_boot_dml_producer_trans_init(1, data_len);
		mmc_ret = bam_add_desc(&bam, MMC_BOOT_BAM_READ_PIPE_INDEX,
					(unsigned char *)data_ptr, data_len);
	}
	else
	{
		mmc_boot_dml_consumer_trans_init();
		mmc_ret = bam_add_desc(&bam, MMC_BOOT_BAM_WRITE_PIPE_INDEX,
					(unsigned char *)data_ptr, data_len);
	}

	/* Update return value enums */
	if (mmc_ret != BAM_RESULT_SUCCESS)
	{
		dprintf(CRITICAL, "MMC BAM transfer error: %d\n", mmc_ret);
		mmc_ret = MMC_BOOT_E_FAILURE;
	}
}

#endif

/*
 * Check if card supports DDR mode
 */
uint8_t card_supports_ddr_mode()
{
	if (IS_BIT_SET_EXT_CSD(MMC_DEVICE_TYPE, 2) ||
		IS_BIT_SET_EXT_CSD(MMC_DEVICE_TYPE, 3))
		return 1;
	else
		return 0;
}

/*
 * Check if card suppports HS200 mode
 */
uint8_t card_supports_hs200_mode()
{
	if (IS_BIT_SET_EXT_CSD(MMC_DEVICE_TYPE, 4) ||
		IS_BIT_SET_EXT_CSD(MMC_DEVICE_TYPE, 5))
		return 1;
	else
		return 0;
}

/* Return the density of the mmc device */
uint64_t mmc_get_device_capacity()
{
	return mmc_card.capacity;
}

/* Return the block size of the mmc device */
uint32_t mmc_get_device_blocksize()
{
	return mmc_card.block_size;
}

void mmc_put_card_to_sleep(void)
{
	uint32_t mmc_ret;
	struct mmc_boot_command cmd = {0};

	cmd.cmd_index = CMD7_SELECT_DESELECT_CARD;
	cmd.argument = 0x00000000;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_NONE;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS)
	{
		dprintf(CRITICAL, "card deselect error: %d\n", mmc_ret);
		return;
	}

	cmd.cmd_index = CMD5_SLEEP_AWAKE;
	cmd.argument = (mmc_card.rca << MMC_CARD_RCA_BIT) | MMC_CARD_SLEEP;
	cmd.cmd_type = MMC_BOOT_CMD_ADDRESS;
	cmd.resp_type = MMC_BOOT_RESP_R1B;

	/* send command */
	mmc_ret = mmc_boot_send_command(&cmd);
	if (mmc_ret != MMC_BOOT_E_SUCCESS)
		dprintf(CRITICAL, "card sleep error: %d\n", mmc_ret);
}
