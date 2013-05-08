/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
#include <mmc_sdhci.h>
#include <sdhci.h>
#include <partition_parser.h>
#include <platform/iomap.h>
#include <platform/timer.h>

extern void clock_init_mmc(uint32_t);
extern void clock_config_mmc(uint32_t, uint32_t);

/* data access time unit in ns */
static const uint32_t taac_unit[] =
{
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000
};

/* data access time value x 10 */
static const uint32_t taac_value[] =
{
	0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

/* data transfer rate in kbit/s */
static const uint32_t xfer_rate_unit[] =
{
	100, 1000, 10000, 100000, 0, 0, 0, 0
};

/* data transfer rate value x 10*/
static const uint32_t xfer_rate_value[] =
{
	0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80
};

/*
 * Function: mmc decode and save csd
 * Arg     : Card structure & raw csd
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Decodes CSD response received from the card.
 *           Note that we have defined only few of the CSD elements
 *           in csd structure. We'll only decode those values.
 */
static uint32_t mmc_decode_and_save_csd(struct mmc_card *card)
{
	uint32_t mmc_sizeof = 0;
	uint32_t mmc_unit = 0;
	uint32_t mmc_value = 0;
	uint32_t mmc_temp = 0;
	uint32_t *raw_csd = card->raw_csd;

	struct mmc_csd mmc_csd;

	mmc_sizeof = sizeof(uint32_t) * 8;

	mmc_csd.cmmc_structure = UNPACK_BITS(raw_csd, 126, 2, mmc_sizeof);

	if ((card->type == MMC_TYPE_SDHC)
	    || (card->type == MMC_TYPE_STD_SD)) {
		/* Parse CSD according to SD card spec. */

		/* CSD register is little bit differnet for CSD version 2.0 High
		 * Capacity and CSD version 1.0/2.0 Standard memory cards.
		 * In Version 2.0 some of the fields have fixed values and it's
		 * not necessary for host to refer these fields in CSD sent by
		 * card
		 */

		if (mmc_csd.cmmc_structure == 1) {
			/* CSD Version 2.0 */
			mmc_csd.card_cmd_class = UNPACK_BITS(raw_csd, 84, 12, mmc_sizeof);
			/* Fixed value is 9 = 2^9 = 512 */
			mmc_csd.write_blk_len = 512;
			/* Fixed value is 9 = 512 */
			mmc_csd.read_blk_len = 512;
			/* Fixed value: 010b */
			mmc_csd.r2w_factor = 0x2;
			/* Not there in version 2.0 */
			mmc_csd.c_size_mult = 0;
			mmc_csd.c_size  = UNPACK_BITS(raw_csd, 48, 22, mmc_sizeof);
			mmc_csd.nsac_clk_cycle = UNPACK_BITS(raw_csd, 104, 8, mmc_sizeof)
									 * 100;

			mmc_unit = UNPACK_BITS(raw_csd, 112, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 115, 4, mmc_sizeof);
			mmc_csd.taac_ns = (taac_value[mmc_value] * taac_unit[mmc_unit])
							   / 10;

			mmc_csd.erase_blk_len = 1;
			mmc_csd.read_blk_misalign = 0;
			mmc_csd.write_blk_misalign = 0;
			mmc_csd.read_blk_partial = 0;
			mmc_csd.write_blk_partial = 0;

			mmc_unit = UNPACK_BITS(raw_csd, 96, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 99, 4, mmc_sizeof);
			mmc_csd.tran_speed = (xfer_rate_value[mmc_value] *
								 xfer_rate_unit[mmc_unit]) / 10;

			mmc_csd.wp_grp_size = 0x0;
			mmc_csd.wp_grp_enable = 0x0;
			mmc_csd.perm_wp = UNPACK_BITS(raw_csd, 13, 1, mmc_sizeof);
			mmc_csd.temp_wp = UNPACK_BITS(raw_csd, 12, 1, mmc_sizeof);

			/* Calculate the card capcity */
			card->capacity = (1 + mmc_csd.c_size) * 512 * 1024;
		} else {
			/* CSD Version 1.0 */
			mmc_csd.card_cmd_class = UNPACK_BITS(raw_csd, 84, 12, mmc_sizeof);

			mmc_temp = UNPACK_BITS(raw_csd, 22, 4, mmc_sizeof);
			mmc_csd.write_blk_len = (mmc_temp > 8 && mmc_temp < 12) ?
									(1 << mmc_temp) : 512;

			mmc_temp = UNPACK_BITS(raw_csd, 80, 4, mmc_sizeof);
			mmc_csd.read_blk_len = (mmc_temp > 8 && mmc_temp < 12) ?
								   (1 << mmc_temp) : 512;

			mmc_unit = UNPACK_BITS(raw_csd, 112, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 115, 4, mmc_sizeof);
			mmc_csd.taac_ns = (taac_value[mmc_value] * taac_unit[mmc_unit])
							  / 10;

			mmc_unit = UNPACK_BITS(raw_csd, 96, 3, mmc_sizeof);
			mmc_value = UNPACK_BITS(raw_csd, 99, 4, mmc_sizeof);
			mmc_csd.tran_speed = (xfer_rate_value[mmc_value] *
								 xfer_rate_unit[mmc_unit]) / 10;

			mmc_csd.nsac_clk_cycle = UNPACK_BITS(raw_csd, 104, 8, mmc_sizeof)
									 * 100;

			mmc_csd.r2w_factor = UNPACK_BITS(raw_csd, 26, 3, mmc_sizeof);
			mmc_csd.sector_size = UNPACK_BITS(raw_csd, 39, 7, mmc_sizeof) + 1;

			mmc_csd.erase_blk_len = UNPACK_BITS(raw_csd, 46, 1, mmc_sizeof);
			mmc_csd.read_blk_misalign = UNPACK_BITS(raw_csd, 77, 1, mmc_sizeof);
			mmc_csd.write_blk_misalign = UNPACK_BITS(raw_csd, 78, 1, mmc_sizeof);
			mmc_csd.read_blk_partial = UNPACK_BITS(raw_csd, 79, 1, mmc_sizeof);
			mmc_csd.write_blk_partial = UNPACK_BITS(raw_csd, 21, 1, mmc_sizeof);

			mmc_csd.c_size_mult = UNPACK_BITS(raw_csd, 47, 3, mmc_sizeof);
			mmc_csd.c_size = UNPACK_BITS(raw_csd, 62, 12, mmc_sizeof);
			mmc_csd.wp_grp_size = UNPACK_BITS(raw_csd, 32, 7, mmc_sizeof);
			mmc_csd.wp_grp_enable = UNPACK_BITS(raw_csd, 31, 1, mmc_sizeof);
			mmc_csd.perm_wp = UNPACK_BITS(raw_csd, 13, 1, mmc_sizeof);
			mmc_csd.temp_wp = UNPACK_BITS(raw_csd, 12, 1, mmc_sizeof);

			/* Calculate the card capacity */
			mmc_temp = (1 << (mmc_csd.c_size_mult + 2)) * (mmc_csd.c_size + 1);
			card->capacity = mmc_temp * mmc_csd.read_blk_len;
		}
	} else {
		/* Parse CSD according to MMC card spec. */
		mmc_csd.spec_vers = UNPACK_BITS(raw_csd, 122, 4, mmc_sizeof);
		mmc_csd.card_cmd_class = UNPACK_BITS(raw_csd, 84, 12, mmc_sizeof);
		mmc_csd.write_blk_len = 1 << UNPACK_BITS(raw_csd, 22, 4, mmc_sizeof);
		mmc_csd.read_blk_len =  1 << UNPACK_BITS(raw_csd, 80, 4, mmc_sizeof);
		mmc_csd.r2w_factor = UNPACK_BITS(raw_csd, 26, 3, mmc_sizeof);
		mmc_csd.c_size_mult = UNPACK_BITS(raw_csd, 47, 3, mmc_sizeof);
		mmc_csd.c_size = UNPACK_BITS(raw_csd, 62, 12, mmc_sizeof);
		mmc_csd.nsac_clk_cycle = UNPACK_BITS(raw_csd, 104, 8, mmc_sizeof) * 100;

		mmc_unit = UNPACK_BITS(raw_csd, 112, 3, mmc_sizeof);
		mmc_value = UNPACK_BITS(raw_csd, 115, 4, mmc_sizeof);
		mmc_csd.taac_ns = (taac_value[mmc_value] * taac_unit[mmc_unit]) / 10;

		mmc_csd.read_blk_misalign = UNPACK_BITS(raw_csd, 77, 1, mmc_sizeof);
		mmc_csd.write_blk_misalign = UNPACK_BITS(raw_csd, 78, 1, mmc_sizeof);
		mmc_csd.read_blk_partial = UNPACK_BITS(raw_csd, 79, 1, mmc_sizeof);
		mmc_csd.write_blk_partial = UNPACK_BITS(raw_csd, 21, 1, mmc_sizeof);

		/* Ignore -- no use of this value. */
		mmc_csd.tran_speed = 0x00;

		mmc_csd.erase_grp_size = UNPACK_BITS(raw_csd, 42, 5, mmc_sizeof);
		mmc_csd.erase_grp_mult = UNPACK_BITS(raw_csd, 37, 5, mmc_sizeof);
		mmc_csd.wp_grp_size = UNPACK_BITS(raw_csd, 32, 5, mmc_sizeof);
		mmc_csd.wp_grp_enable = UNPACK_BITS(raw_csd, 31, 1, mmc_sizeof);
		mmc_csd.perm_wp = UNPACK_BITS(raw_csd, 13, 1, mmc_sizeof);
		mmc_csd.temp_wp = UNPACK_BITS(raw_csd, 12, 1, mmc_sizeof);

		/* Calculate the card capcity */
		if (mmc_csd.c_size != 0xFFF) {
			/* For cards less than or equal to 2GB */
			mmc_temp = (1 << (mmc_csd.c_size_mult + 2)) * (mmc_csd.c_size + 1);
			card->capacity = mmc_temp * mmc_csd.read_blk_len;
		} else {
			/* For cards greater than 2GB, Ext CSD register's SEC_COUNT
			 * is used to calculate the size.
			 */
			uint64_t sec_count;

			sec_count = (card->ext_csd[MMC_SEC_COUNT4] << MMC_SEC_COUNT4_SHIFT)
						| (card->ext_csd[MMC_SEC_COUNT3] << MMC_SEC_COUNT3_SHIFT)
						| (card->ext_csd[MMC_SEC_COUNT2] << MMC_SEC_COUNT2_SHIFT)
						| card->ext_csd[MMC_SEC_COUNT1];

			card->capacity = sec_count * MMC_BLK_SZ;
		}
	}

	/* save the information in card structure */
	memcpy((struct mmc_csd *)&card->csd,(struct mmc_csd *)&mmc_csd,
			sizeof(struct mmc_csd));

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

	return 0;
}

/*
 * Function: mmc decode & save cid
 * Arg     : card structure & raw cid
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Decode CID sent by the card.
 */
static uint32_t mmc_decode_and_save_cid(struct mmc_card *card,
										uint32_t *raw_cid)
{
	struct mmc_cid mmc_cid;
	uint32_t mmc_sizeof = 0;
	int i = 0;

	if (!raw_cid) {
		return 1;
	}

	mmc_sizeof = sizeof(uint32_t) * 8;

	if ((card->type == MMC_TYPE_SDHC) ||
		(card->type == MMC_TYPE_STD_SD)) {
		mmc_cid.mid = UNPACK_BITS(raw_cid, 120, 8, mmc_sizeof);
		mmc_cid.oid = UNPACK_BITS(raw_cid, 104, 16, mmc_sizeof);

		for (i = 0; i < 5; i++) {
			mmc_cid.pnm[i] = (uint8_t)UNPACK_BITS(raw_cid,
								    (104 - 8 * (i + 1)),
								    8,
								    mmc_sizeof);
		}
		mmc_cid.pnm[5] = 0;
		mmc_cid.pnm[6] = 0;

		mmc_cid.prv = UNPACK_BITS(raw_cid, 56, 8, mmc_sizeof);
		mmc_cid.psn = UNPACK_BITS(raw_cid, 24, 31, mmc_sizeof);
		mmc_cid.month = UNPACK_BITS(raw_cid, 8, 4, mmc_sizeof);
		mmc_cid.year = UNPACK_BITS(raw_cid, 12, 8, mmc_sizeof);
		mmc_cid.year += 2000;
	} else {
		mmc_cid.mid = UNPACK_BITS(raw_cid, 120, 8, mmc_sizeof);
		mmc_cid.oid = UNPACK_BITS(raw_cid, 104, 16, mmc_sizeof);

		for (i = 0; i < 6; i++) {
			mmc_cid.pnm[i] = (uint8_t)UNPACK_BITS(raw_cid, (104 - 8 * (i + 1)),
												  8, mmc_sizeof);
		}
		mmc_cid.pnm[6] = 0;

		mmc_cid.prv = UNPACK_BITS(raw_cid, 48, 8, mmc_sizeof);
		mmc_cid.psn = UNPACK_BITS(raw_cid, 16, 31, mmc_sizeof);
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
	dprintf(SPEW, "Manufacturing date: %d %d\n", mmc_cid.month, mmc_cid.year);

	return 0;
}

/*
 * Function: mmc reset cards
 * Arg     : host structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Reset all the cards to idle condition (CMD 0)
 */
static uint8_t mmc_reset_card(struct sdhci_host *host)
{
	struct mmc_command cmd;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	cmd.cmd_index = CMD0_GO_IDLE_STATE;
	cmd.argument = 0;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_NONE;

	/* send command */
	return sdhci_send_command(host, &cmd);
}

/*
 * Function: mmc operations command
 * Arg     : host & card structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Send CMD1 to know whether the card supports host VDD profile or not.
 */
static uint32_t mmc_send_op_cond(struct sdhci_host *host, struct mmc_card *card)
{
	struct mmc_command cmd;
	uint32_t mmc_resp = 0;
	uint32_t mmc_ret = 0;
	uint32_t mmc_retry = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

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
	cmd.argument = card->ocr;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R3;

	do {
		mmc_ret = sdhci_send_command(host, &cmd);
		if (mmc_ret)
			return mmc_ret;

		/* Command returned success, now it's time to examine response */
		mmc_resp = cmd.resp[0];

		/* Check the response for busy status */
		if (!(mmc_resp & MMC_OCR_BUSY)) {
			mmc_retry++;
			mdelay(1);
			continue;
		} else
			break;
	} while (mmc_retry < MMC_MAX_COMMAND_RETRY);

	/* If we reached here after max retries, we failed to get OCR */
	if (mmc_retry == MMC_MAX_COMMAND_RETRY && !(mmc_resp & MMC_OCR_BUSY)) {
		dprintf(CRITICAL, "Card has busy status set. Init did not complete\n");
		return 1;
	}

	/* Response contains card's ocr. Update card's information */
	card->ocr = mmc_resp;

	if (mmc_resp & MMC_OCR_SEC_MODE)
		card->type = MMC_TYPE_MMCHC;
	else
		card->type = MMC_TYPE_STD_MMC;

	return 0;
}

/*
 * Function: mmc send cid
 * Arg     : host & card structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Request any card to send its uniquie card identification
 *           (CID) number (CMD2).
 */
static uint32_t mmc_all_send_cid(struct sdhci_host *host, struct mmc_card *card)
{
	struct mmc_command cmd;
	uint32_t mmc_ret = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD2 Format:
	 * [31:0] stuff bits
	 */
	cmd.cmd_index = CMD2_ALL_SEND_CID;
	cmd.argument = 0;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R2;

	/* send command */
	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret) {
		return mmc_ret;
	}

	/* Response contains card's 128 bits CID register */
	mmc_ret = mmc_decode_and_save_cid(card, cmd.resp);
	if (mmc_ret) {
		return mmc_ret;
	}

	return 0;
}

/*
 * Function: mmc send relative address
 * Arg     : host & card structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Ask card to send it's relative card address (RCA).
 *           This RCA number is shorter than CID and is used by
 *           the host to address the card in future (CMD3)
 */
static uint32_t mmc_send_relative_address(struct sdhci_host *host,
										  struct mmc_card *card)
{
	struct mmc_command cmd;
	uint32_t mmc_ret = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD3 Format:
	 * [31:0] stuff bits
	 */
	if (card->type == MMC_TYPE_SDHC ||
		card->type == MMC_TYPE_STD_SD) {
		cmd.cmd_index = CMD3_SEND_RELATIVE_ADDR;
		cmd.argument = 0;
		cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
		cmd.resp_type = SDHCI_CMD_RESP_R6;

		/* send command */
		mmc_ret = sdhci_send_command(host, &cmd);
		if (mmc_ret)
			return mmc_ret;

		/* For sD, card will send RCA. Store it */
		card->rca = (cmd.resp[0] >> 16);
	} else {
		cmd.cmd_index = CMD3_SEND_RELATIVE_ADDR;
		cmd.argument = (MMC_RCA << 16);
		card->rca = (cmd.argument >> 16);
		cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
		cmd.resp_type = SDHCI_CMD_RESP_R6;

		/* send command */
		mmc_ret = sdhci_send_command(host, &cmd);
		if (mmc_ret)
			return mmc_ret;
	}

	return 0;
}

/*
 * Function: mmc send csd
 * Arg     : host, card structure & o/p arg to store csd
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Requests card to send it's CSD register's contents. (CMD9)
 */
static uint32_t mmc_send_csd(struct sdhci_host *host, struct mmc_card *card)
{
	struct mmc_command cmd;
	uint32_t mmc_arg = 0;
	uint32_t mmc_ret = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD9 Format:
	 * [31:16] RCA
	 * [15:0] stuff bits
	 */
	mmc_arg |= card->rca << 16;

	cmd.cmd_index = CMD9_SEND_CSD;
	cmd.argument = mmc_arg;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R2;

	/* send command */
	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret)
		return mmc_ret;

	/* response contains the card csd */
	memcpy(card->raw_csd, cmd.resp, sizeof(cmd.resp));

	return 0;
}

/*
 * Function: mmc select card
 * Arg     : host, card structure & RCA
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Selects a card by sending CMD7 to the card with its RCA.
 *           If RCA field is set as 0 ( or any other address ),
 *           the card will be de-selected. (CMD7)
 */
static uint32_t mmc_select_card(struct sdhci_host *host, struct mmc_card *card,
						 uint32_t rca)
{
	struct mmc_command cmd;
	uint32_t mmc_arg = 0;
	uint32_t mmc_ret = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD7 Format:
	 * [31:16] RCA
	 * [15:0] stuff bits
	 */
	mmc_arg |= rca << 16;

	cmd.cmd_index = CMD7_SELECT_DESELECT_CARD;
	cmd.argument = mmc_arg;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;

	/* If we are deselecting card, we do not get response */
	if (rca == card->rca && rca) {
		if (card->type == MMC_TYPE_SDHC ||
			card->type == MMC_TYPE_STD_SD)
			cmd.resp_type = SDHCI_CMD_RESP_R1B;
		else
			cmd.resp_type = SDHCI_CMD_RESP_R1;
	} else
		cmd.resp_type = SDHCI_CMD_RESP_NONE;

	/* send command */
	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret)
		return mmc_ret;

	return 0;
}

/*
 * Function: mmc set block len
 * Arg     : host, card structure & block length
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Send command to set block length.
 */
static uint32_t mmc_set_block_len(struct sdhci_host *host,
								  struct mmc_card *card,
								  uint32_t block_len)
{
	struct mmc_command cmd;
	uint32_t mmc_ret = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD16 Format:
	 * [31:0] block length
	 */

	cmd.cmd_index = CMD16_SET_BLOCKLEN;
	cmd.argument = block_len;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R1;

	/* send command */
	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret)
		return mmc_ret;

	/*
	 * If blocklength is larger than 512 bytes,
	 * the card sets BLOCK_LEN_ERROR bit.
	 */
	if (cmd.resp[0] & MMC_R1_BLOCK_LEN_ERR) {
		dprintf(CRITICAL, "The block length is not supported by the card\n");
		return 1;
	}

	return 0;
}

/*
 * Function: mmc get card status
 * Arg     : host, card structure & o/p argument card status
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Get the current status of the card
 */
static uint32_t mmc_get_card_status(struct sdhci_host *host,
									struct mmc_card *card, uint32_t *status)
{
	struct mmc_command cmd;
	uint32_t mmc_ret = 0;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD13 Format:
	 * [31:16] RCA
	 * [15:0] stuff bits
	 */
	cmd.cmd_index = CMD13_SEND_STATUS;
	cmd.argument = card->rca << 16;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R1;

	/* send command */
	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret)
		return mmc_ret;

	/* Checking ADDR_OUT_OF_RANGE error in CMD13 response */
	if ((cmd.resp[0] >> 31) & 0x01)
		return 1;

	*status = cmd.resp[0];
	return 0;
}

/*
 * Function: mmc get ext csd
 * Arg     : host, card structure & array to hold ext attributes
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Send ext csd command & get the card attributes
 */
static uint32_t mmc_get_ext_csd(struct sdhci_host *host, struct mmc_card *card)
{
	struct mmc_command cmd;
	uint32_t mmc_ret = 0;

	card->ext_csd = memalign(CACHE_LINE, ROUNDUP(512, CACHE_LINE));

	ASSERT(card->ext_csd);

	memset(card->ext_csd, 0, sizeof(card->ext_csd));

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD8 */
	cmd.cmd_index = CMD8_SEND_EXT_CSD;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R1;
	cmd.data.data_ptr = card->ext_csd;
	cmd.data.num_blocks = 1;
	cmd.data_present = 0x1;
	cmd.trans_mode = SDHCI_MMC_READ;

	/* send command */
	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret)
		return mmc_ret;

	return mmc_ret;
}

/*
 * Function: mmc switch command
 * Arg     : Host, card structure, access mode, index & value to be set
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Send switch command to the card to set the ext attribute @ index
 */
static uint32_t mmc_switch_cmd(struct sdhci_host *host, struct mmc_card *card,
							   uint32_t access, uint32_t index, uint32_t value)
{

	struct mmc_command cmd;
	uint32_t mmc_ret = 0;
	uint32_t mmc_status;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

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
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R1B;

	mmc_ret = sdhci_send_command(host, &cmd);
	if (mmc_ret) {
		dprintf(CRITICAL, "CMD6 send failed\n");
		return mmc_ret;
	}

	/* Check if the card completed the switch command processing */
	mmc_ret = mmc_get_card_status(host, card, &mmc_status);
	if (mmc_ret) {
		dprintf(CRITICAL, "Get card status failed\n");
		return mmc_ret;
	}

	if (MMC_CARD_STATUS(mmc_status) != MMC_TRAN_STATE) {
		dprintf(CRITICAL, "Switch cmd failed. Card not in tran state\n");
		mmc_ret = 1;
	}

	if (mmc_status & MMC_SWITCH_FUNC_ERR_FLAG) {
		dprintf(CRITICAL, "Switch cmd failed. Switch Error.\n");
		mmc_ret = 1;
	}

	return mmc_ret;
}

/*
 * Function: mmc set bus width
 * Arg     : Host, card structure & width
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Send switch command to set bus width
 */
static uint32_t mmc_set_bus_width(struct sdhci_host *host,
								  struct mmc_card *card,
								  uint32_t width)
{
	uint32_t mmc_ret = 0;

	mmc_ret = mmc_switch_cmd(host, card, MMC_ACCESS_WRITE,
							 MMC_EXT_MMC_BUS_WIDTH, width);

	if (mmc_ret) {
		dprintf(CRITICAL, "Switch cmd failed\n");
		return mmc_ret;
	}

	return 0;
}


/*
 * Function: mmc card supports ddr mode
 * Arg     : None
 * Return  : 1 if DDR mode is supported, 0 otherwise
 * Flow    : Check the ext csd attributes of the card
 */
static uint8_t mmc_card_supports_hs200_mode(struct mmc_card *card)
{
	if (card->ext_csd[MMC_DEVICE_TYPE] & MMC_HS_HS200_MODE)
		return 1;
	else
		return 0;
}

/*
 * Function: mmc card supports ddr mode
 * Arg     : None
 * Return  : 1 if DDR mode is supported, 0 otherwise
 * Flow    : Check the ext csd attributes of the card
 */
static uint8_t mmc_card_supports_ddr_mode(struct mmc_card *card)
{
	if (card->ext_csd[MMC_DEVICE_TYPE] & MMC_HS_DDR_MODE)
		return 1;
	else
		return 0;
}

/*
 * Function : Enable HS200 mode
 * Arg      : Host, card structure and bus width
 * Return   : 0 on Success, 1 on Failure
 * Flow     :
 *           - Set the bus width to 4/8 bit SDR as supported by the target & host
 *           - Set the HS_TIMING on ext_csd 185 for the card
 */
static uint32_t mmc_set_hs200_mode(struct sdhci_host *host,
								   struct mmc_card *card, uint32_t width)
{
	uint32_t mmc_ret = 0;

	/* Set 4/8 bit SDR bus width */
	mmc_ret = mmc_set_bus_width(host, card, width);
	if (mmc_ret) {
		dprintf(CRITICAL, "Failure to set wide bus for Card(RCA:%x)\n",
						  card->rca);
		return mmc_ret;
	}

	/* Setting HS200 in HS_TIMING using EXT_CSD (CMD6) */
	mmc_ret = mmc_switch_cmd(host, card, MMC_ACCESS_WRITE, MMC_EXT_MMC_HS_TIMING, MMC_HS200_TIMING);

	if (mmc_ret) {
		dprintf(CRITICAL, "Switch cmd returned failure %d\n", __LINE__);
		return mmc_ret;
	}

	/* Enable hs200 mode in controller */
	sdhci_set_sdr_mode(host);

	return mmc_ret;
}

/*
 * Function: mmc set ddr mode
 * Arg     : Host & card structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Set bus width for ddr mode & set controller in DDR mode
*/
static uint8_t mmc_set_ddr_mode(struct sdhci_host *host, struct mmc_card *card)
{
	uint8_t mmc_ret = 0;

	/* Set width for 8 bit DDR mode by default */
	mmc_ret = mmc_set_bus_width(host, card, DATA_DDR_BUS_WIDTH_8BIT);

	if (mmc_ret) {
		dprintf(CRITICAL, "Failure to set DDR mode for Card(RCA:%x)\n",
						  card->rca);
		return mmc_ret;
	}

	sdhci_set_ddr_mode(host);

	return 0;
}

/*
 * Function: mmc set high speed interface
 * Arg     : Host & card structure
 * Return  : None
 * Flow    : Sets the sdcc clock & clock divider in the host controller
 *           Adjust the interface speed to optimal speed
 */
static uint32_t mmc_set_hs_interface(struct sdhci_host *host,
									 struct mmc_card *card)
{
	uint32_t mmc_ret = 0;

	/* Setting HS_TIMING in EXT_CSD (CMD6) */
	mmc_ret = mmc_switch_cmd(host, card, MMC_ACCESS_WRITE,
							 MMC_EXT_MMC_HS_TIMING, MMC_HS_TIMING);

	if (mmc_ret) {
		dprintf(CRITICAL, "Switch cmd returned failure %d\n", __LINE__);
		return mmc_ret;
	}

	return 0;
}

/*
 * Function: mmc_host_init
 * Arg     : mmc device structure
 * Return  : 0 on success, 1 on Failure
 * Flow    : Initialize the host contoller
 *           Set the clock rate to 400 KHZ for init
 */
static uint8_t mmc_host_init(struct mmc_device *dev)
{
	uint8_t mmc_ret = 0;

	struct sdhci_host *host;
	struct mmc_config_data *cfg;

	host = &dev->host;
	cfg = &dev->config;

	/*
	 * Initialize the controller, read the host capabilities
	 * set power on mode
	 */
	sdhci_init(host);

	/* Initialize any clocks needed for SDC controller */
	clock_init_mmc(cfg->slot);

	/* Setup initial freq to 400KHz */
	clock_config_mmc(cfg->slot, cfg->max_clk_rate);

	mmc_ret = sdhci_clk_supply(host, SDHCI_CLK_400KHZ);

	return mmc_ret;
}

/*
 * Function: mmc identify card
 * Arg     : host & card structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Performs card identification process:
 *           1. Get card's unique identification number (CID)
 *           2. Get(for sd)/set (for mmc) relative card address (RCA)
 *           3. Select the card to put it in TRAN state
 */
static uint32_t mmc_identify_card(struct sdhci_host *host, struct mmc_card *card)
{
	uint32_t mmc_return = 0;
	uint32_t raw_csd[4];

	/* Ask card to send its unique card identification (CID) number (CMD2) */
	mmc_return = mmc_all_send_cid(host, card);
	if (mmc_return) {
		dprintf(CRITICAL,"Failure getting card's CID number!\n");
		return mmc_return;
	}

	/* Ask card to send a relative card address (RCA) (CMD3) */
	mmc_return = mmc_send_relative_address(host, card);
	if (mmc_return) {
		dprintf(CRITICAL, "Failure getting card's RCA!\n");
		return mmc_return;
	}

	/* Get card's CSD register (CMD9) */
	mmc_return = mmc_send_csd(host, card);
	if (mmc_return) {
		dprintf(CRITICAL,"Failure getting card's CSD information!\n");
		return mmc_return;
	}

	/* Select the card (CMD7) */
	mmc_return = mmc_select_card(host, card, card->rca);
	if (mmc_return) {
		dprintf(CRITICAL, "Failure selecting the Card with RCA: %x\n",card->rca);
		return mmc_return;
	}

	/* Set the card status as active */
	card->status = MMC_STATUS_ACTIVE;

	return 0;
}

/*
 * Function: mmc_reset_card_and_send_op
 * Arg     : Host & Card structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Routine to initialize MMC card. It resets a card to idle state,
 *           verify operating voltage and set the card in ready state.
 */
static uint32_t mmc_reset_card_and_send_op(struct sdhci_host *host, struct mmc_card *card)
{
	uint32_t mmc_return = 0;

	/* 1. Card Reset - CMD0 */
	mmc_return = mmc_reset_card(host);
	if (mmc_return) {
		dprintf(CRITICAL, "Failure resetting MMC cards!\n");
		return mmc_return;
	}

	/* 2. Card Initialization process */

	/*
	 * Send CMD1 to identify and reject cards that do not match host's VDD range
	 * profile. Cards sends its OCR register in response.
	 */
	mmc_return = mmc_send_op_cond(host, card);

	/* OCR is not received, init could not complete */
	if (mmc_return) {
		dprintf(CRITICAL, "Failure getting OCR response from MMC Card\n");
		return mmc_return;
	}

	return 0;
}

/*
 * Function: mmc_init_card
 * Arg     : mmc device structure
 * Return  : 0 on Success, 1 on Failure
 * Flow    : Performs initialization and identification of eMMC cards connected
 *           to the host.
 */

static uint32_t mmc_card_init(struct mmc_device *dev)
{
	uint32_t mmc_return = 0;
	uint32_t status;
	uint8_t bus_width = 0;

	struct sdhci_host *host;
	struct mmc_card *card;
	struct mmc_config_data *cfg;

	host = &dev->host;
	card = &dev->card;
	cfg  = &dev->config;

	/* Initialize MMC card structure */
	card->status = MMC_STATUS_INACTIVE;

	/* TODO: Get the OCR params from target */
	card->ocr = MMC_OCR_27_36 | MMC_OCR_SEC_MODE;

	/* Reset the card & get the OCR */
	mmc_return = mmc_reset_card_and_send_op(host, card);
	if (mmc_return)
		return mmc_return;

	/* Identify (CMD2, CMD3 & CMD9) and select the card (CMD7) */
	mmc_return = mmc_identify_card(host, card);
	if (mmc_return)
		return mmc_return;

	/* set interface speed */
	mmc_return = mmc_set_hs_interface(host, card);
	if (mmc_return) {
		dprintf(CRITICAL, "Error adjusting interface speed!\n");
		return mmc_return;
	}

	/* Set the sdcc clock to 50 MHZ */
	sdhci_clk_supply(host, SDHCI_CLK_50MHZ);

	/* Now get the extended CSD for the card */
	if ((card->type == MMC_TYPE_STD_MMC) ||
		(card->type == MMC_TYPE_MMCHC)) {
			/* For MMC cards, also get the extended csd */
			mmc_return = mmc_get_ext_csd(host, card);

			if (mmc_return) {
				dprintf(CRITICAL, "Failure getting card's ExtCSD information!\n");
				return mmc_return;
			}
	}

	/* Decode and save the CSD register */
	mmc_return = mmc_decode_and_save_csd(card);
	if (mmc_return) {
		dprintf(CRITICAL, "Failure decoding card's CSD information!\n");
		return mmc_return;
	}


	/* Set the bus width based on host, target capbilities */
	if (cfg->bus_width == DATA_BUS_WIDTH_8BIT && host->caps.bus_width_8bit)
			bus_width = DATA_BUS_WIDTH_8BIT;
	/*
	 * Host contoller by default supports 4 bit & 1 bit mode.
	 * No need to check for host support here
	 */
	else if (cfg->bus_width == DATA_BUS_WIDTH_4BIT)
			bus_width = DATA_BUS_WIDTH_4BIT;
	else
			bus_width = DATA_BUS_WIDTH_1BIT;

	/* Set 4/8 bit SDR bus width in controller */
	mmc_return = sdhci_set_bus_width(host, bus_width);

	if (mmc_return) {
		dprintf(CRITICAL, "Failed to set bus width for host controller\n");
		return 1;
	}

	/* Enable high speed mode in the follwing order:
	 * 1. HS200 mode if supported by host & card
	 * 2. DDR mode host, if supported by host & card
	 * 3. Use normal speed mode with supported bus width
	 */
	if (mmc_card_supports_hs200_mode(card) && host->caps.sdr50_support) {
		mmc_return = mmc_set_hs200_mode(host, card, bus_width);

		if (mmc_return) {
			dprintf(CRITICAL, "Failure to set HS200 mode for Card(RCA:%x)\n",
							  card->rca);
			return mmc_return;
		}
	} else if (mmc_card_supports_ddr_mode(card) && host->caps.ddr_support) {
		mmc_return = mmc_set_ddr_mode(host, card);

		if (mmc_return) {
			dprintf(CRITICAL, "Failure to set DDR mode for Card(RCA:%x)\n",
							  card->rca);
			return mmc_return;
		}
	} else {
		/* Set 4/8 bit bus width for the card */
		mmc_return = mmc_set_bus_width(host, card, bus_width);
		if (mmc_return) {
			dprintf(CRITICAL, "Failure to set wide bus for Card(RCA:%x)\n",
							  card->rca);
			return mmc_return;
		}
	}


	/* Verify TRAN state after changing speed and bus width */
	mmc_return = mmc_get_card_status(host, card, &status);
	if (mmc_return)
		return mmc_return;

	if (MMC_CARD_STATUS(status) != MMC_TRAN_STATE)
		mmc_return = 1;

	return mmc_return;
}

/*
 * Function: mmc display csd
 * Arg     : None
 * Return  : None
 * Flow    : Displays the csd information
 */
static void mmc_display_csd(struct mmc_card *card)
{
	dprintf(SPEW, "erase_grpsize: %d\n", card->csd.erase_grp_size);
	dprintf(SPEW, "erase_grpmult: %d\n", card->csd.erase_grp_mult);
	dprintf(SPEW, "wp_grpsize: %d\n", card->csd.wp_grp_size);
	dprintf(SPEW, "wp_grpen: %d\n", card->csd.wp_grp_enable);
	dprintf(SPEW, "perm_wp: %d\n", card->csd.perm_wp);
	dprintf(SPEW, "temp_wp: %d\n", card->csd.temp_wp);
}

/*
 * Function: mmc_init
 * Arg     : MMC configuration data
 * Return  : Pointer to mmc device
 * Flow    : Entry point to MMC boot process
 *           Initialize the sd host controller
 *           Initialize the mmc card
 *           Set the clock & high speed mode
 */
struct mmc_device *mmc_init(struct mmc_config_data *data)
{
	uint8_t mmc_ret = 0;
	struct mmc_device *dev;

	dev = (struct mmc_device *) malloc (sizeof(struct mmc_device));

	if (!dev) {
		dprintf(CRITICAL, "Error allocating mmc device\n");
		return NULL;
	}

	ASSERT(data);

	memcpy((void*)&dev->config, (void*)data, sizeof(struct mmc_config_data));

	memset((struct mmc_card *)&dev->card, 0, sizeof(struct mmc_card));

	dev->host.base = data->base;

	/* Initialize the host & clock */
	dprintf(SPEW, " Initializing MMC host data structure and clock!\n");

	mmc_ret = mmc_host_init(dev);
	if (mmc_ret) {
		dprintf(CRITICAL, "Error Initializing MMC host : %u\n", mmc_ret);
		return NULL;
	}

	/* Initialize and identify cards connected to host */
	mmc_ret = mmc_card_init(dev);
	if (mmc_ret) {
		dprintf(CRITICAL, "Failed detecting MMC/SDC @ slot%d\n",
						  dev->config.slot);
		return NULL;
	}

	dprintf(INFO, "Done initialization of the card\n");

	mmc_display_csd(&dev->card);

	return dev;
}

/*
 * Function: mmc sdhci read
 * Arg     : mmc device structure, block address, number of blocks & destination
 * Return  : 0 on Success, non zero on success
 * Flow    : Fill in the command structure & send the command
 */
uint32_t mmc_sdhci_read(struct mmc_device *dev, void *dest,
						uint64_t blk_addr, uint32_t num_blocks)
{
	uint32_t mmc_ret = 0;
	struct mmc_command cmd;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD17/18 Format:
	 * [31:0] Data Address
	 */
	if (num_blocks == 1)
		cmd.cmd_index = CMD17_READ_SINGLE_BLOCK;
	else
		cmd.cmd_index = CMD18_READ_MULTIPLE_BLOCK;

	cmd.argument = blk_addr;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R1;
	cmd.trans_mode = SDHCI_MMC_READ;
	cmd.data_present = 0x1;
	cmd.data.data_ptr = dest;
	cmd.data.num_blocks = num_blocks;

	/* send command */
	mmc_ret = sdhci_send_command(&dev->host, &cmd);
	if (mmc_ret) {
		return mmc_ret;
	}

	/* Response contains 32 bit Card status. Here we'll check
		BLOCK_LEN_ERROR and ADDRESS_ERROR */
	if (cmd.resp[0] & MMC_R1_BLOCK_LEN_ERR) {
		dprintf(CRITICAL, "The transferred bytes does not match the block length\n");
		return 1;
	}

	/* Misaligned address not matching block length */
	if (cmd.resp[0] & MMC_R1_ADDR_ERR) {
		dprintf(CRITICAL, "The misaligned address did not match the block length used\n");
		return 1;
	}

	if (MMC_CARD_STATUS(cmd.resp[0]) != MMC_TRAN_STATE) {
		dprintf(CRITICAL, "MMC read failed, card is not in TRAN state\n");
		return 1;
	}

	return mmc_ret;
}

/*
 * Function: mmc sdhci write
 * Arg     : mmc device structure, block address, number of blocks & source
 * Return  : 0 on Success, non zero on success
 * Flow    : Fill in the command structure & send the command
 */
uint32_t mmc_sdhci_write(struct mmc_device *dev, void *src,
						 uint64_t blk_addr, uint32_t num_blocks)
{
	uint32_t mmc_ret = 0;
	struct mmc_command cmd;

	memset((struct mmc_command *)&cmd, 0, sizeof(struct mmc_command));

	/* CMD24/25 Format:
	 * [31:0] Data Address
	 */

	if (num_blocks == 1)
		cmd.cmd_index = CMD24_WRITE_SINGLE_BLOCK;
	else
		cmd.cmd_index = CMD25_WRITE_MULTIPLE_BLOCK;

	cmd.argument = blk_addr;
	cmd.cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd.resp_type = SDHCI_CMD_RESP_R1;
	cmd.trans_mode = SDHCI_MMC_WRITE;
	cmd.data_present = 0x1;
	cmd.data.data_ptr = src;
	cmd.data.num_blocks = num_blocks;

	/* send command */
	mmc_ret = sdhci_send_command(&dev->host, &cmd);
	if (mmc_ret)
		return mmc_ret;

	/* Response contains 32 bit Card status. Here we'll check
		BLOCK_LEN_ERROR and ADDRESS_ERROR */
	if (cmd.resp[0] & MMC_R1_BLOCK_LEN_ERR) {
		dprintf(CRITICAL, "The transferred bytes does not match the block length\n");
		return 1;
	}

	/* Misaligned address not matching block length */
	if (cmd.resp[0] & MMC_R1_ADDR_ERR) {
		dprintf(CRITICAL, "The misaligned address did not match the block length used\n");
		return 1;
	}

	if (MMC_CARD_STATUS(cmd.resp[0]) != MMC_TRAN_STATE) {
		dprintf(CRITICAL, "MMC read failed, card is not in TRAN state\n");
		return 1;
	}

	return mmc_ret;
}
