/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
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

#include <debug.h>
#include <reg.h>
#include <stdlib.h>
#include <string.h>
#include <dev/flash.h>
#include <lib/ptable.h>
#include <nand.h>

#include "dmov.h"

#define VERBOSE 0
#define VERIFY_WRITE 0

static void *flash_spare;
static void *flash_data;
void platform_config_interleaved_mode_gpios(void);

typedef struct dmov_ch dmov_ch;
struct dmov_ch {
	volatile unsigned cmd;
	volatile unsigned result;
	volatile unsigned status;
	volatile unsigned config;
};

static void dmov_prep_ch(dmov_ch * ch, unsigned id)
{
	ch->cmd = DMOV_CMD_PTR(id);
	ch->result = DMOV_RSLT(id);
	ch->status = DMOV_STATUS(id);
	ch->config = DMOV_CONFIG(id);
}

#define SRC_CRCI_NAND_CMD  CMD_SRC_CRCI(DMOV_NAND_CRCI_CMD)
#define DST_CRCI_NAND_CMD  CMD_DST_CRCI(DMOV_NAND_CRCI_CMD)
#define SRC_CRCI_NAND_DATA CMD_SRC_CRCI(DMOV_NAND_CRCI_DATA)
#define DST_CRCI_NAND_DATA CMD_DST_CRCI(DMOV_NAND_CRCI_DATA)

#define CFG0_RAW 0xA80420C0
#define CFG1_RAW 0x5045D

#define CFG0_RAW_BCHECC   0xA80428C0

static unsigned CFG0, CFG1;
static unsigned CFG0_M, CFG1_M;
static unsigned CFG0_A, CFG1_A;
static unsigned NAND_CFG0_RAW, NAND_CFG1_RAW;
static unsigned ECC_BCH_CFG;

static uint32_t enable_bch_ecc;
static unsigned int *bbtbl;

#define CFG1_WIDE_FLASH (1U << 1)

#define paddr(n) ((unsigned) (n))

static int dmov_exec_cmdptr(unsigned id, unsigned *ptr)
{
	dmov_ch ch;
	unsigned n;

	dmov_prep_ch(&ch, id);

	/* Set IRQ_EN low, not using IRQ mode */
	writel(DMOV_CONFIG_FOREC_FLUSH_RSLT | 0x0, ch.config);

	writel(DMOV_CMD_PTR_LIST | DMOV_CMD_ADDR(paddr(ptr)), ch.cmd);

	while (!(readl(ch.status) & DMOV_STATUS_RSLT_VALID)) ;

	n = readl(ch.status);
	while (DMOV_STATUS_RSLT_COUNT(n)) {
		n = readl(ch.result);
		if (n != 0x80000002) {
			dprintf(CRITICAL, "ERROR: result: %x\n", n);
			dprintf(CRITICAL, "ERROR:  flush: %x %x %x %x\n",
				readl(DMOV_FLUSH0(DMOV_NAND_CHAN)),
				readl(DMOV_FLUSH1(DMOV_NAND_CHAN)),
				readl(DMOV_FLUSH2(DMOV_NAND_CHAN)),
				readl(DMOV_FLUSH3(DMOV_NAND_CHAN)));
		}
		n = readl(ch.status);
	}

	return 0;
}

static struct flash_info flash_info;
static unsigned flash_pagesize = 0;
static int interleaved_mode = 0;
static unsigned num_pages_per_blk = 0;
static unsigned num_pages_per_blk_mask = 0;

struct flash_identification {
	unsigned flash_id;
	unsigned mask;
	unsigned density;
	unsigned widebus;
	unsigned pagesize;
	unsigned blksize;
	unsigned oobsize;
	unsigned onenand;
};

static struct flash_identification supported_flash[] = {
	/* Flash ID     ID Mask Density(MB)  Wid Pgsz   Blksz   oobsz onenand   Manuf */
	{0x00000000, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0}, /*ONFI*/ {0x1500aaec, 0xFF00FFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 0},	/*Sams */
	{0x5500baec, 0xFF00FFFF, (256 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Sams */
	{0x1500aa98, 0xFFFFFFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 0},	/*Tosh */
	{0x5500ba98, 0xFFFFFFFF, (256 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Tosh */
	{0xd580b12c, 0xFFFFFFFF, (256 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Micr */
	{0x5590bc2c, 0xFFFFFFFF, (512 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Micr */
	{0x1580aa2c, 0xFFFFFFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 0},	/*Micr */
	{0x1590aa2c, 0xFFFFFFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 0},	/*Micr */
	{0x1590ac2c, 0xFFFFFFFF, (512 << 20), 0, 2048, (2048 << 6), 64, 0},	/*Micr */
	{0x5580baad, 0xFFFFFFFF, (256 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Hynx */
	{0x5510baad, 0xFFFFFFFF, (256 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Hynx */
	{0x004000ec, 0xFFFFFFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 1},	/*Sams */
	{0x005c00ec, 0xFFFFFFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 1},	/*Sams */
	{0x005800ec, 0xFFFFFFFF, (256 << 20), 0, 2048, (2048 << 6), 64, 1},	/*Sams */
	{0x6600bcec, 0xFF00FFFF, (512 << 20), 1, 4096, (4096 << 6), 128, 0},	/*Sams */
	{0x5580ba2c, 0xFFFFFFFF, (256 << 20), 1, 2048, (2048 << 6), 64, 0},	/*Hynx */
	{0x6600b3ec, 0xFFFFFFFF, (1024 << 20), 1, 4096, (4096 << 6), 128, 0},	/*Sams */
	{0x2600482c, 0xFF00FFFF, (2048 << 20), 0, 4096, (4096 << 7), 224, 0},	/*8bit bch ecc */
	/* Note: Width flag is 0 for 8 bit Flash and 1 for 16 bit flash   */
	/* Note: Onenand flag is 0 for NAND Flash and 1 for OneNAND flash       */
	/* Note: The First row will be filled at runtime during ONFI probe      */
};

static void set_nand_configuration(char type)
{
	if (type == TYPE_MODEM_PARTITION) {
		CFG0 = CFG0_M;
		CFG1 = CFG1_M;
	} else {
		CFG0 = CFG0_A;
		CFG1 = CFG1_A;
	}
}

static void flash_nand_read_id(dmov_s * cmdlist, unsigned *ptrlist)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;

	data[0] = 0 | 4;
	data[1] = NAND_CMD_FETCH_ID;
	data[2] = 1;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0xAAD40000;	/* Default value for CFG0 for reading device id */

	/* Read NAND device id */
	cmd[0].cmd = 0 | CMD_OCB;
	cmd[0].src = paddr(&data[7]);
	cmd[0].dst = NAND_DEV0_CFG0;
	cmd[0].len = 4;

	cmd[1].cmd = 0;
	cmd[1].src = NAND_SFLASHC_BURST_CFG;
	cmd[1].dst = paddr(&data[5]);
	cmd[1].len = 4;

	cmd[2].cmd = 0;
	cmd[2].src = paddr(&data[6]);
	cmd[2].dst = NAND_SFLASHC_BURST_CFG;
	cmd[2].len = 4;

	cmd[3].cmd = 0;
	cmd[3].src = paddr(&data[0]);
	cmd[3].dst = NAND_FLASH_CHIP_SELECT;
	cmd[3].len = 4;

	cmd[4].cmd = DST_CRCI_NAND_CMD;
	cmd[4].src = paddr(&data[1]);
	cmd[4].dst = NAND_FLASH_CMD;
	cmd[4].len = 4;

	cmd[5].cmd = 0;
	cmd[5].src = paddr(&data[2]);
	cmd[5].dst = NAND_EXEC_CMD;
	cmd[5].len = 4;

	cmd[6].cmd = SRC_CRCI_NAND_DATA;
	cmd[6].src = NAND_FLASH_STATUS;
	cmd[6].dst = paddr(&data[3]);
	cmd[6].len = 4;

	cmd[7].cmd = 0;
	cmd[7].src = NAND_READ_ID;
	cmd[7].dst = paddr(&data[4]);
	cmd[7].len = 4;

	cmd[8].cmd = CMD_OCU | CMD_LC;
	cmd[8].src = paddr(&data[5]);
	cmd[8].dst = NAND_SFLASHC_BURST_CFG;
	cmd[8].len = 4;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "status: %x\n", data[3]);
#endif

	flash_info.id = data[4];
	flash_info.vendor = data[4] & 0xff;
	flash_info.device = (data[4] >> 8) & 0xff;
	return;
}

static int
flash_nand_block_isbad(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;
	char buf[4];
	unsigned cwperpage;

	cwperpage = (flash_pagesize >> 9);

	/* Check first page of this block */
	if (page & num_pages_per_blk_mask)
		page = page - (page & num_pages_per_blk_mask);

	/* Check bad block marker */
	data[0] = NAND_CMD_PAGE_READ;	/* command */

	/* addr0 */
	if (CFG1 & CFG1_WIDE_FLASH)
		data[1] = enable_bch_ecc ?
		    ((page << 16) | ((532 * (cwperpage - 1)) >> 1)) :
		    ((page << 16) | ((528 * (cwperpage - 1)) >> 1));

	else
		data[1] = enable_bch_ecc ?
		    ((page << 16) | (532 * (cwperpage - 1))) :
		    ((page << 16) | (528 * (cwperpage - 1)));

	data[2] = (page >> 16) & 0xff;	/* addr1        */
	data[3] = 0 | 4;	/* chipsel      */
	data[4] = NAND_CFG0_RAW & ~(7U << 6);	/* cfg0         */
	data[5] = NAND_CFG1_RAW | (CFG1 & CFG1_WIDE_FLASH);	/* cfg1         */
	if (enable_bch_ecc) {
		data[6] = ECC_BCH_CFG;	/* ECC CFG */
	}
	data[7] = 1;
	data[8] = CLEAN_DATA_32;	/* flash status */
	data[9] = CLEAN_DATA_32;	/* buf status   */

	cmd[0].cmd = DST_CRCI_NAND_CMD | CMD_OCB;
	cmd[0].src = paddr(&data[0]);
	cmd[0].dst = NAND_FLASH_CMD;
	cmd[0].len = 16;

	cmd[1].cmd = 0;
	cmd[1].src = paddr(&data[4]);
	cmd[1].dst = NAND_DEV0_CFG0;
	if (enable_bch_ecc) {
		cmd[1].len = 12;
	} else {
		cmd[1].len = 8;
	}

	cmd[2].cmd = 0;
	cmd[2].src = paddr(&data[7]);
	cmd[2].dst = NAND_EXEC_CMD;
	cmd[2].len = 4;

	cmd[3].cmd = SRC_CRCI_NAND_DATA;
	cmd[3].src = NAND_FLASH_STATUS;
	cmd[3].dst = paddr(&data[8]);
	cmd[3].len = 8;

	cmd[4].cmd = CMD_OCU | CMD_LC;
	cmd[4].src = NAND_FLASH_BUFFER + (flash_pagesize - (enable_bch_ecc ?
							    (532 *
							     (cwperpage -
							      1)) : (528 *
								     (cwperpage
								      - 1))));
	cmd[4].dst = paddr(&buf);
	cmd[4].len = 4;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "status: %x\n", data[8]);
#endif

	/* we fail if there was an operation error, a mpu error, or the
	 ** erase success bit was not set.
	 */
	if (data[8] & 0x110)
		return -1;

	/* Check for bad block marker byte */
	if (CFG1 & CFG1_WIDE_FLASH) {
		if (buf[0] != 0xFF || buf[1] != 0xFF)
			return 1;
	} else {
		if (buf[0] != 0xFF)
			return 1;
	}

	return 0;
}

static int
flash_nand_block_isbad_interleave(dmov_s * cmdlist, unsigned *ptrlist,
				  unsigned page)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;
	char buf01[4];
	char buf10[4];
	unsigned cwperpage;

	cwperpage = ((flash_pagesize >> 1) >> 9);

	/* Check first page of this block */
	if (page & 63)
		page = page - (page & 63);

	/* Check bad block marker */
	data[0] = NAND_CMD_PAGE_READ;	/* command */

	/* addr0 */
	if (CFG1 & CFG1_WIDE_FLASH)
		data[1] = (page << 16) | ((528 * (cwperpage - 1)) >> 1);
	else
		data[1] = (page << 16) | (528 * (cwperpage - 1));

	data[2] = (page >> 16) & 0xff;	/* addr1        */
	data[3] = 0 | 4;	/* chipsel CS0  */
	data[4] = 0 | 5;	/* chipsel CS1  */
	data[5] = NAND_CFG0_RAW & ~(7U << 6);	/* cfg0         */
	data[6] = NAND_CFG1_RAW | (CFG1 & CFG1_WIDE_FLASH);	/* cfg1         */
	data[7] = 1;
	data[8] = CLEAN_DATA_32;	/* NC01 flash status */
	data[9] = CLEAN_DATA_32;	/* NC01 buf01 status   */
	data[10] = CLEAN_DATA_32;	/* NC10 flash status */
	data[11] = CLEAN_DATA_32;	/* NC10 buf10 status   */
	data[12] = 0x00000A3C;	/* adm_mux_data_ack_req_nc01 */
	data[13] = 0x0000053C;	/* adm_mux_cmd_ack_req_nc01  */
	data[14] = 0x00000F28;	/* adm_mux_data_ack_req_nc10 */
	data[15] = 0x00000F14;	/* adm_mux_cmd_ack_req_nc10  */
	data[16] = 0x00000FC0;	/* adm_default_mux */
	data[17] = 0x00000805;	/* enable CS1 */
	data[18] = 0x00000801;	/* disable CS1 */

	/* enable CS1 */
	cmd[0].cmd = 0;
	cmd[0].src = paddr(data[17]);
	cmd[0].dst = EBI2_CHIP_SELECT_CFG0;
	cmd[0].len = 4;

	/* Reading last code word from NC01 */
	/* 0xF14 */
	cmd[1].cmd = 0;
	cmd[1].src = paddr(data[15]);
	cmd[1].dst = EBI2_NAND_ADM_MUX;
	cmd[1].len = 4;

	cmd[2].cmd = DST_CRCI_NAND_CMD;
	cmd[2].src = paddr(&data[0]);
	cmd[2].dst = NC01(NAND_FLASH_CMD);
	cmd[2].len = 16;

	cmd[3].cmd = 0;
	cmd[3].src = paddr(&data[5]);
	cmd[3].dst = NC01(NAND_DEV0_CFG0);
	cmd[3].len = 8;

	cmd[4].cmd = 0;
	cmd[4].src = paddr(&data[7]);
	cmd[4].dst = NC01(NAND_EXEC_CMD);
	cmd[4].len = 4;

	/* 0xF28 */
	cmd[5].cmd = 0;
	cmd[5].src = paddr(data[14]);
	cmd[5].dst = EBI2_NAND_ADM_MUX;
	cmd[5].len = 4;

	cmd[6].cmd = SRC_CRCI_NAND_DATA;
	cmd[6].src = NC01(NAND_FLASH_STATUS);
	cmd[6].dst = paddr(&data[8]);
	cmd[6].len = 8;

	cmd[7].cmd = 0;
	cmd[7].src =
	    NC01(NAND_FLASH_BUFFER) + (flash_pagesize -
				       (528 * (cwperpage - 1)));
	cmd[7].dst = paddr(&buf01);
	cmd[7].len = 4;

	/* Reading last code word from NC10 */
	/* 0x53C */
	cmd[8].cmd = 0;
	cmd[8].src = paddr(data[13]);
	cmd[8].dst = EBI2_NAND_ADM_MUX;
	cmd[8].len = 4;

	cmd[9].cmd = DST_CRCI_NAND_CMD;
	cmd[9].src = paddr(&data[0]);
	cmd[9].dst = NC10(NAND_FLASH_CMD);
	cmd[9].len = 12;

	cmd[10].cmd = 0;
	cmd[10].src = paddr(&data[4]);
	cmd[10].dst = NC10(NAND_FLASH_CHIP_SELECT);
	cmd[10].len = 4;

	cmd[11].cmd = 0;
	cmd[11].src = paddr(&data[5]);
	cmd[11].dst = NC10(NAND_DEV1_CFG0);
	cmd[11].len = 8;

	cmd[12].cmd = 0;
	cmd[12].src = paddr(&data[7]);
	cmd[12].dst = NC10(NAND_EXEC_CMD);
	cmd[12].len = 4;

	/* 0xA3C */
	cmd[13].cmd = 0;
	cmd[13].src = paddr(data[12]);
	cmd[13].dst = EBI2_NAND_ADM_MUX;
	cmd[13].len = 4;

	cmd[14].cmd = SRC_CRCI_NAND_DATA;
	cmd[14].src = NC10(NAND_FLASH_STATUS);
	cmd[14].dst = paddr(&data[10]);
	cmd[14].len = 8;

	cmd[15].cmd = 0;
	cmd[15].src =
	    NC10(NAND_FLASH_BUFFER) + (flash_pagesize -
				       (528 * (cwperpage - 1)));
	cmd[15].dst = paddr(&buf10);
	cmd[15].len = 4;

	cmd[16].cmd = 0;
	cmd[16].src = paddr(&data[16]);
	cmd[16].dst = EBI2_NAND_ADM_MUX;
	cmd[16].len = 4;

	/* setting default value */
	cmd[17].cmd = CMD_OCU | CMD_LC;
	cmd[17].src = paddr(&data[18]);
	cmd[17].dst = EBI2_CHIP_SELECT_CFG0;
	cmd[17].len = 4;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "NC01 status: %x\n", data[8]);
	dprintf(INFO, "NC10 status: %x\n", data[10]);
#endif

	/* we fail if there was an operation error, a mpu error, or the
	 ** erase success bit was not set.
	 */
	if ((data[8] & 0x110) || (data[10] & 0x110))
		return -1;

	/* Check for bad block marker byte */
	if (CFG1 & CFG1_WIDE_FLASH) {
		if ((buf01[0] != 0xFF || buf01[1] != 0xFF) ||
		    (buf10[0] != 0xFF || buf10[1] != 0xFF))
			return 1;
	} else {
		if (buf01[0] != 0xFF || buf10[0] != 0xFF)
			return 1;
	}

	return 0;
}

static int
flash_nand_erase_block(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;
	int isbad = 0;

	/* only allow erasing on block boundaries */
	if (page & num_pages_per_blk_mask)
		return -1;

	/* Check for bad block and erase only if block is not marked bad */
	isbad = flash_nand_block_isbad(cmdlist, ptrlist, page);

	if (isbad) {
		dprintf(INFO, "skipping @ %d (bad block)\n",
			page / num_pages_per_blk);
		return -1;
	}

	/* Erase block */
	data[0] = NAND_CMD_BLOCK_ERASE;
	data[1] = page;
	data[2] = 0;
	data[3] = 0 | 4;
	data[4] = 1;
	data[5] = 0xeeeeeeee;
	data[6] = CFG0 & (~(7 << 6));	/* CW_PER_PAGE = 0 */
	data[7] = CFG1;
	data[8] = ECC_BCH_CFG;
	data[9] = 0x00000020;
	data[10] = 0x000000C0;

	cmd[0].cmd = DST_CRCI_NAND_CMD | CMD_OCB;
	cmd[0].src = paddr(&data[0]);
	cmd[0].dst = NAND_FLASH_CMD;
	cmd[0].len = 16;

	cmd[1].cmd = 0;
	cmd[1].src = paddr(&data[6]);
	cmd[1].dst = NAND_DEV0_CFG0;
	if (enable_bch_ecc) {
		cmd[1].len = 12;
	} else {
		cmd[1].len = 8;
	}

	cmd[2].cmd = 0;
	cmd[2].src = paddr(&data[4]);
	cmd[2].dst = NAND_EXEC_CMD;
	cmd[2].len = 4;

	cmd[3].cmd = SRC_CRCI_NAND_DATA;
	cmd[3].src = NAND_FLASH_STATUS;
	cmd[3].dst = paddr(&data[5]);
	cmd[3].len = 4;

	cmd[4].cmd = 0;
	cmd[4].src = paddr(&data[9]);
	cmd[4].dst = NAND_FLASH_STATUS;
	cmd[4].len = 4;

	cmd[5].cmd = CMD_OCU | CMD_LC;
	cmd[5].src = paddr(&data[10]);
	cmd[5].dst = NAND_READ_STATUS;
	cmd[5].len = 4;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "status: %x\n", data[5]);
#endif

	/* we fail if there was an operation error, a mpu error, or the
	 ** erase success bit was not set.
	 */
	if (data[5] & 0x110)
		return -1;
	if (!(data[5] & 0x80))
		return -1;

	return 0;
}

static int
flash_nand_erase_block_interleave(dmov_s * cmdlist, unsigned *ptrlist,
				  unsigned page)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;
	int isbad = 0;

	/* only allow erasing on block boundaries */
	if (page & 63)
		return -1;

	/* Check for bad block and erase only if block is not marked bad */
	isbad = flash_nand_block_isbad(cmdlist, ptrlist, page);

	if (isbad) {
		dprintf(INFO, "skipping @ %d (bad block)\n", page >> 6);
		return -1;
	}

	/* Erase block */
	data[0] = NAND_CMD_BLOCK_ERASE;
	data[1] = page;
	data[2] = 0;
	data[3] = 0 | 4;	/* chipselect CS0 */
	data[4] = 0 | 5;	/* chipselect CS1 */
	data[5] = 1;
	data[6] = 0xeeeeeeee;
	data[7] = 0xeeeeeeee;
	data[8] = CFG0 & (~(7 << 6));	/* CW_PER_PAGE = 0 */
	data[9] = CFG1;
	data[10] = 0x00000A3C;	/* adm_mux_data_ack_req_nc01 */
	data[11] = 0x0000053C;	/* adm_mux_cmd_ack_req_nc01  */
	data[12] = 0x00000F28;	/* adm_mux_data_ack_req_nc10 */
	data[13] = 0x00000F14;	/* adm_mux_cmd_ack_req_nc10  */
	data[14] = 0x00000FC0;	/* adm_default_mux */
	data[15] = 0x00000805;	/* enable CS1 */
	data[16] = 0x00000801;	/* disable CS1 */

	/* enable CS1 */
	cmd[0].cmd = 0 | CMD_OCB;
	cmd[0].src = paddr(data[15]);
	cmd[0].dst = EBI2_CHIP_SELECT_CFG0;
	cmd[0].len = 4;

	/* Reading last code word from NC01 */
	/* 0xF14 */
	cmd[1].cmd = 0;
	cmd[1].src = paddr(data[13]);
	cmd[1].dst = EBI2_NAND_ADM_MUX;
	cmd[1].len = 4;

	cmd[2].cmd = DST_CRCI_NAND_CMD;
	cmd[2].src = paddr(&data[0]);
	cmd[2].dst = NC01(NAND_FLASH_CMD);
	cmd[2].len = 16;

	cmd[3].cmd = 0;
	cmd[3].src = paddr(&data[8]);
	cmd[3].dst = NC01(NAND_DEV0_CFG0);
	cmd[3].len = 8;

	cmd[4].cmd = 0;
	cmd[4].src = paddr(&data[5]);
	cmd[4].dst = NC01(NAND_EXEC_CMD);
	cmd[4].len = 4;

	/* 0xF28 */
	cmd[5].cmd = 0;
	cmd[5].src = paddr(data[12]);
	cmd[5].dst = EBI2_NAND_ADM_MUX;
	cmd[5].len = 4;

	cmd[6].cmd = SRC_CRCI_NAND_DATA;
	cmd[6].src = NC01(NAND_FLASH_STATUS);
	cmd[6].dst = paddr(&data[6]);
	cmd[6].len = 4;

	/* Reading last code word from NC10 */
	/* 0x53C */
	cmd[7].cmd = 0;
	cmd[7].src = paddr(data[11]);
	cmd[7].dst = EBI2_NAND_ADM_MUX;
	cmd[7].len = 4;

	cmd[8].cmd = DST_CRCI_NAND_CMD;
	cmd[8].src = paddr(&data[0]);
	cmd[8].dst = NC10(NAND_FLASH_CMD);
	cmd[8].len = 12;

	cmd[9].cmd = 0;
	cmd[9].src = paddr(&data[4]);
	cmd[9].dst = NC10(NAND_FLASH_CHIP_SELECT);
	cmd[9].len = 4;

	cmd[10].cmd = 0;
	cmd[10].src = paddr(&data[8]);
	cmd[10].dst = NC10(NAND_DEV1_CFG0);
	cmd[10].len = 8;

	cmd[11].cmd = 0;
	cmd[11].src = paddr(&data[5]);
	cmd[11].dst = NC10(NAND_EXEC_CMD);
	cmd[11].len = 4;

	/* 0xA3C */
	cmd[12].cmd = 0;
	cmd[12].src = paddr(data[10]);
	cmd[12].dst = EBI2_NAND_ADM_MUX;
	cmd[12].len = 4;

	cmd[13].cmd = SRC_CRCI_NAND_DATA;
	cmd[13].src = NC10(NAND_FLASH_STATUS);
	cmd[13].dst = paddr(&data[7]);
	cmd[13].len = 4;

	/* adm default mux state */
	/* 0xFCO */
	cmd[14].cmd = 0;
	cmd[14].src = paddr(data[14]);
	cmd[14].dst = EBI2_NAND_ADM_MUX;
	cmd[14].len = 4;

	/* disable CS1 */
	cmd[15].cmd = CMD_OCU | CMD_LC;
	cmd[15].src = paddr(data[16]);
	cmd[15].dst = EBI2_CHIP_SELECT_CFG0;
	cmd[15].len = 4;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "NC01 status: %x\n", data[6]);
	dprintf(INFO, "NC10 status: %x\n", data[7]);
#endif

	/* we fail if there was an operation error, a mpu error, or the
	 ** erase success bit was not set.
	 */
	if (data[6] & 0x110 || data[7] & 0x110)
		return -1;
	if (!(data[6] & 0x80) || !(data[7] & 0x80))
		return -1;

	return 0;
}

struct data_flash_io {
	unsigned cmd;
	unsigned addr0;
	unsigned addr1;
	unsigned chipsel;
	unsigned cfg0;
	unsigned cfg1;
	unsigned ecc_bch_cfg;
	unsigned exec;
	unsigned ecc_cfg;
	unsigned ecc_cfg_save;
	unsigned clrfstatus;
	unsigned clrrstatus;
	struct {
		unsigned flash_status;
		unsigned buffer_status;
	} result[8];
};

struct interleave_data_flash_io {
	uint32_t cmd;
	uint32_t addr0;
	uint32_t addr1;
	uint32_t chipsel_cs0;
	uint32_t chipsel_cs1;
	uint32_t cfg0;
	uint32_t cfg1;
	uint32_t exec;
	uint32_t ecc_cfg;
	uint32_t ecc_cfg_save;
	uint32_t ebi2_chip_select_cfg0;
	uint32_t adm_mux_data_ack_req_nc01;
	uint32_t adm_mux_cmd_ack_req_nc01;
	uint32_t adm_mux_data_ack_req_nc10;
	uint32_t adm_mux_cmd_ack_req_nc10;
	uint32_t adm_default_mux;
	uint32_t default_ebi2_chip_select_cfg0;
	struct {
		uint32_t flash_status;
	} result[16];
};

static int
_flash_nand_read_page(dmov_s * cmdlist, unsigned *ptrlist,
		      unsigned page, void *_addr, void *_spareaddr)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct data_flash_io *data = (void *)(ptrlist + 4);
	unsigned addr = (unsigned)_addr;
	unsigned spareaddr = (unsigned)_spareaddr;
	unsigned n;
	int isbad = 0;
	unsigned cwperpage;
	unsigned block = 0;
	cwperpage = (flash_pagesize >> 9);

	/* Find the block no for the page */
	block = page / num_pages_per_blk;

	/* Check the bad block table for each block
	 * -1: indicates the block needs to be checked if good or bad
	 * 1 : The block is bad
	 * 0 : The block is good
	 */
	if (bbtbl[block] == -1) {
		isbad = flash_nand_block_isbad(cmdlist, ptrlist, page);
		if (isbad) {
			/* Found bad , set the bad table entry */
			bbtbl[block] = 1;
			return -2;
		} else {
			/* Found good block , set the table entry &
			 *  continue reading the data
			 */
			bbtbl[block] = 0;
		}
	} else if (bbtbl[block] == 1) {
		/* If the block is already identified as bad, return error */
		return -2;
	}

	data->cmd = NAND_CMD_PAGE_READ_ECC;
	data->addr0 = page << 16;
	data->addr1 = (page >> 16) & 0xff;
	data->chipsel = 0 | 4;	/* flash0 + undoc bit */

	/* GO bit for the EXEC register */
	data->exec = 1;

	data->cfg0 = CFG0;
	data->cfg1 = CFG1;

	if (enable_bch_ecc) {
		data->ecc_bch_cfg = ECC_BCH_CFG;
	}
	data->ecc_cfg = 0x203;

	/* save existing ecc config */
	cmd->cmd = CMD_OCB;
	cmd->src = NAND_EBI2_ECC_BUF_CFG;
	cmd->dst = paddr(&data->ecc_cfg_save);
	cmd->len = 4;
	cmd++;

	for (n = 0; n < cwperpage; n++) {
		/* write CMD / ADDR0 / ADDR1 / CHIPSEL regs in a burst */
		cmd->cmd = DST_CRCI_NAND_CMD;
		cmd->src = paddr(&data->cmd);
		cmd->dst = NAND_FLASH_CMD;
		cmd->len = ((n == 0) ? 16 : 4);
		cmd++;

		if (n == 0) {
			/* block on cmd ready, set configuration */
			cmd->cmd = 0;
			cmd->src = paddr(&data->cfg0);
			cmd->dst = NAND_DEV0_CFG0;
			if (enable_bch_ecc) {
				cmd->len = 12;
			} else {
				cmd->len = 8;
			}
			cmd++;

			/* set our ecc config */
			cmd->cmd = 0;
			cmd->src = paddr(&data->ecc_cfg);
			cmd->dst = NAND_EBI2_ECC_BUF_CFG;
			cmd->len = 4;
			cmd++;
		}
		/* kick the execute register */
		cmd->cmd = 0;
		cmd->src = paddr(&data->exec);
		cmd->dst = NAND_EXEC_CMD;
		cmd->len = 4;
		cmd++;

		/* block on data ready, then read the status register */
		cmd->cmd = SRC_CRCI_NAND_DATA;
		cmd->src = NAND_FLASH_STATUS;
		cmd->dst = paddr(&data->result[n]);
		cmd->len = 8;
		cmd++;

		/* read data block */
		cmd->cmd = 0;
		cmd->src = NAND_FLASH_BUFFER;
		cmd->dst = addr + n * 516;
		cmd->len =
		    ((n <
		      (cwperpage - 1)) ? 516 : (512 - ((cwperpage - 1) << 2)));
		cmd++;
	}

	/* read extra data */
	cmd->cmd = 0;
	cmd->src = NAND_FLASH_BUFFER + (512 - ((cwperpage - 1) << 2));
	cmd->dst = spareaddr;
	cmd->len = 16;
	cmd++;

	/* restore saved ecc config */
	cmd->cmd = CMD_OCU | CMD_LC;
	cmd->src = paddr(&data->ecc_cfg_save);
	cmd->dst = NAND_EBI2_ECC_BUF_CFG;
	cmd->len = 4;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "read page %d: status: %x %x %x %x\n",
		page, data[5], data[6], data[7], data[8]);
	for (n = 0; n < 4; n++) {
		ptr = (unsigned *)(addr + 512 * n);
		dprintf(INFO, "data%d:	%x %x %x %x\n", n, ptr[0], ptr[1],
			ptr[2], ptr[3]);
		ptr = (unsigned *)(spareaddr + 16 * n);
		dprintf(INFO, "spare data%d	%x %x %x %x\n", n, ptr[0],
			ptr[1], ptr[2], ptr[3]);
	}
#endif

	/* if any of the writes failed (0x10), or there was a
	 ** protection violation (0x100), we lose
	 */
	for (n = 0; n < cwperpage; n++) {
		if (data->result[n].flash_status & 0x110) {
			return -1;
		}
	}

	return 0;
}

static int
flash_nand_read_page_interleave(dmov_s * cmdlist, unsigned *ptrlist,
				unsigned page, void *_addr, void *_spareaddr)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct interleave_data_flash_io *data = (void *)(ptrlist + 4);
	unsigned addr = (unsigned)_addr;
	unsigned spareaddr = (unsigned)_spareaddr;
	unsigned n;
	int isbad = 0;
	unsigned cwperpage;
	cwperpage = (flash_pagesize >> 9);

	/* Check for bad block and read only from a good block */
	isbad = flash_nand_block_isbad(cmdlist, ptrlist, page);
	if (isbad)
		return -2;

	data->cmd = NAND_CMD_PAGE_READ_ECC;
	data->addr0 = page << 16;
	data->addr1 = (page >> 16) & 0xff;
	data->chipsel_cs0 = 0 | 4;	/* flash0 + undoc bit */
	data->chipsel_cs1 = 0 | 5;	/* flash0 + undoc bit */
	data->ebi2_chip_select_cfg0 = 0x00000805;
	data->adm_mux_data_ack_req_nc01 = 0x00000A3C;
	data->adm_mux_cmd_ack_req_nc01 = 0x0000053C;
	data->adm_mux_data_ack_req_nc10 = 0x00000F28;
	data->adm_mux_cmd_ack_req_nc10 = 0x00000F14;
	data->adm_default_mux = 0x00000FC0;
	data->default_ebi2_chip_select_cfg0 = 0x00000801;

	/* GO bit for the EXEC register */
	data->exec = 1;

	data->cfg0 = CFG0;
	data->cfg1 = CFG1;

	data->ecc_cfg = 0x203;

	for (n = 0; n < cwperpage; n++) {
		/* flash + buffer status return words */
		data->result[n].flash_status = 0xeeeeeeee;

		if (n == 0) {
			/* enable CS1 */
			cmd->cmd = CMD_OCB;
			cmd->src = paddr(&data->ebi2_chip_select_cfg0);
			cmd->dst = EBI2_CHIP_SELECT_CFG0;
			cmd->len = 4;
			cmd++;

			/* save existing ecc config */
			cmd->cmd = 0;
			cmd->src = NAND_EBI2_ECC_BUF_CFG;
			cmd->dst = paddr(&data->ecc_cfg_save);
			cmd->len = 4;
			cmd++;

			/* NC01, NC10 --> ADDR0/ADDR1 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->addr0);
			cmd->dst = NC11(NAND_ADDR0);
			cmd->len = 8;
			cmd++;

			/* Select the CS0,
			 * for NC01!
			 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->chipsel_cs0);
			cmd->dst = NC01(NAND_FLASH_CHIP_SELECT);
			cmd->len = 4;
			cmd++;

			/* Select the CS1,
			 * for NC10!
			 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->chipsel_cs1);
			cmd->dst = NC10(NAND_FLASH_CHIP_SELECT);
			cmd->len = 4;
			cmd++;

			cmd->cmd = 0;
			cmd->src = paddr(&data->cfg0);
			cmd->dst = NC01(NAND_DEV0_CFG0);
			cmd->len = 8;
			cmd++;

			/* config DEV1 for CS1 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->cfg0);
			cmd->dst = NC10(NAND_DEV1_CFG0);
			cmd->len = 8;
			cmd++;

			cmd->cmd = 0;
			cmd->src = paddr(&data->ecc_cfg);
			cmd->dst = NC11(NAND_EBI2_ECC_BUF_CFG);
			cmd->len = 4;
			cmd++;

			/* if 'only' the last code word */
			if (n == cwperpage - 1) {
				/* MASK CMD ACK/REQ --> NC01 (0x53C) */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_cmd_ack_req_nc01);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* CMD */
				cmd->cmd = DST_CRCI_NAND_CMD;
				cmd->src = paddr(&data->cmd);
				cmd->dst = NC10(NAND_FLASH_CMD);
				cmd->len = 4;
				cmd++;

				/* kick the execute register for NC10 */
				cmd->cmd = 0;
				cmd->src = paddr(&data->exec);
				cmd->dst = NC10(NAND_EXEC_CMD);
				cmd->len = 4;
				cmd++;

				/* MASK DATA ACK/REQ --> NC01 (0xA3C) */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_data_ack_req_nc01);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* block on data ready from NC10, then
				 * read the status register
				 */
				cmd->cmd = SRC_CRCI_NAND_DATA;
				cmd->src = NC10(NAND_FLASH_STATUS);
				cmd->dst = paddr(&data->result[n]);
				/* NAND_FLASH_STATUS +
				 * NAND_BUFFER_STATUS
				 */
				cmd->len = 4;
				cmd++;
			} else {
				/* MASK CMD ACK/REQ --> NC10 (0xF14) */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_cmd_ack_req_nc10);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* CMD */
				cmd->cmd = DST_CRCI_NAND_CMD;
				cmd->src = paddr(&data->cmd);
				cmd->dst = NC01(NAND_FLASH_CMD);
				cmd->len = 4;
				cmd++;

				/* kick the execute register for NC01 */
				cmd->cmd = 0;
				cmd->src = paddr(&data->exec);
				cmd->dst = NC01(NAND_EXEC_CMD);
				cmd->len = 4;
				cmd++;
			}
		}

		if (n % 2 == 0) {
			/* MASK CMD ACK/REQ --> NC01 (0x53C) */
			cmd->cmd = 0;
			cmd->src = paddr(&data->adm_mux_cmd_ack_req_nc01);
			cmd->dst = EBI2_NAND_ADM_MUX;
			cmd->len = 4;
			cmd++;

			/* CMD */
			cmd->cmd = DST_CRCI_NAND_CMD;
			cmd->src = paddr(&data->cmd);
			cmd->dst = NC10(NAND_FLASH_CMD);
			cmd->len = 4;
			cmd++;

			/* kick the execute register for NC10 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->exec);
			cmd->dst = NC10(NAND_EXEC_CMD);
			cmd->len = 4;
			cmd++;

			/* MASK DATA ACK/REQ --> NC10 (0xF28) */
			cmd->cmd = 0;
			cmd->src = paddr(&data->adm_mux_data_ack_req_nc10);
			cmd->dst = EBI2_NAND_ADM_MUX;
			cmd->len = 4;
			cmd++;

			/* block on data ready from NC01, then
			 * read the status register
			 */
			cmd->cmd = SRC_CRCI_NAND_DATA;
			cmd->src = NC01(NAND_FLASH_STATUS);
			cmd->dst = paddr(&data->result[n]);
			/* NAND_FLASH_STATUS +
			 * NAND_BUFFER_STATUS
			 */
			cmd->len = 4;
			cmd++;

			/* read data block */
			cmd->cmd = 0;
			cmd->src = NC01(NAND_FLASH_BUFFER);
			cmd->dst = addr + n * 516;
			cmd->len =
			    ((n <
			      (cwperpage - 1)) ? 516 : (512 -
							((cwperpage -
							  1) << 2)));
			cmd++;
		} else {
			if (n != cwperpage - 1) {
				/* MASK CMD ACK/REQ -->
				 * NC10 (0xF14)
				 */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_cmd_ack_req_nc10);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* CMD */
				cmd->cmd = DST_CRCI_NAND_CMD;
				cmd->src = paddr(&data->cmd);
				cmd->dst = NC01(NAND_FLASH_CMD);
				cmd->len = 4;
				cmd++;

				/* EXEC */
				cmd->cmd = 0;
				cmd->src = paddr(&data->exec);
				cmd->dst = NC01(NAND_EXEC_CMD);
				cmd->len = 4;
				cmd++;

				/* MASK DATA ACK/REQ -->
				 * NC01 (0xA3C)
				 */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_data_ack_req_nc01);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* block on data ready from NC10
				 * then read the status register
				 */
				cmd->cmd = SRC_CRCI_NAND_DATA;
				cmd->src = NC10(NAND_FLASH_STATUS);
				cmd->dst = paddr(&data->result[n]);
				/* NAND_FLASH_STATUS +
				 * NAND_BUFFER_STATUS
				 */
				cmd->len = 4;
				cmd++;
			} else {
				/* MASK DATA ACK/REQ ->
				 * NC01 (0xA3C)
				 */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_data_ack_req_nc01);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* block on data ready from NC10
				 * then read the status register
				 */
				cmd->cmd = SRC_CRCI_NAND_DATA;
				cmd->src = NC10(NAND_FLASH_STATUS);
				cmd->dst = paddr(&data->result[n]);
				/* NAND_FLASH_STATUS +
				 * NAND_BUFFER_STATUS
				 */
				cmd->len = 4;
				cmd++;
			}
			/* read data block */
			cmd->cmd = 0;
			cmd->src = NC10(NAND_FLASH_BUFFER);
			cmd->dst = addr + n * 516;
			cmd->len =
			    ((n <
			      (cwperpage - 1)) ? 516 : (512 -
							((cwperpage -
							  1) << 2)));
			cmd++;

			if (n == (cwperpage - 1)) {
				/* Use NC10 for reading the
				 * last codeword!!!
				 */
				cmd->cmd = 0;
				cmd->src = NC10(NAND_FLASH_BUFFER) +
				    (512 - ((cwperpage - 1) << 2));
				cmd->dst = spareaddr;
				cmd->len = 16;
				cmd++;
			}
		}
	}
	/* restore saved ecc config */
	cmd->cmd = CMD_OCU | CMD_LC;
	cmd->src = paddr(&data->ecc_cfg_save);
	cmd->dst = NAND_EBI2_ECC_BUF_CFG;
	cmd->len = 4;

	/* ADM --> Default mux state (0xFC0) */
	cmd->cmd = 0;
	cmd->src = paddr(&data->adm_default_mux);
	cmd->dst = EBI2_NAND_ADM_MUX;
	cmd->len = 4;
	cmd++;

	/* disable CS1 */
	cmd->cmd = 0;
	cmd->src = paddr(&data->default_ebi2_chip_select_cfg0);
	cmd->dst = EBI2_CHIP_SELECT_CFG0;
	cmd->len = 4;
	cmd++;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "read page %d: status: %x %x %x %x %x %x %x %x \
	%x %x %x %x %x %x %x %x \n", page, data->result[0].flash_status[0], data->result[1].flash_status[1], data->result[2].flash_status[2], data->result[3].flash_status[3], data->result[4].flash_status[4], data->result[5].flash_status[5], data->result[6].flash_status[6], data->result[7].flash_status[7], data->result[8].flash_status[8], data->result[9].flash_status[9], data->result[10].flash_status[10], data->result[11].flash_status[11], data->result[12].flash_status[12], data->result[13].flash_status[13], data->result[14].flash_status[14], data->result[15].flash_status[15]);

	for (n = 0; n < 4; n++) {
		ptr = (unsigned *)(addr + 512 * n);
		dprintf(INFO, "data%d:	%x %x %x %x\n", n, ptr[0], ptr[1],
			ptr[2], ptr[3]);
		ptr = (unsigned *)(spareaddr + 16 * n);
		dprintf(INFO, "spare data%d	%x %x %x %x\n", n, ptr[0],
			ptr[1], ptr[2], ptr[3]);
	}
#endif

	/* if any of the writes failed (0x10), or there was a
	 ** protection violation (0x100), we lose
	 */
	for (n = 0; n < cwperpage; n++) {
		if (data->result[n].flash_status & 0x110) {
			return -1;
		}
	}

	return 0;
}

static int
_flash_nand_write_page(dmov_s * cmdlist, unsigned *ptrlist, unsigned page,
		       const void *_addr, const void *_spareaddr,
		       unsigned raw_mode)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct data_flash_io *data = (void *)(ptrlist + 4);
	unsigned addr = (unsigned)_addr;
	unsigned spareaddr = (unsigned)_spareaddr;
	unsigned n;
	unsigned cwperpage;
	cwperpage = (flash_pagesize >> 9);
	unsigned modem_partition = 0;
	if (CFG0 == CFG0_M) {
		modem_partition = 1;
	}

	data->cmd = NAND_CMD_PRG_PAGE;
	data->addr0 = page << 16;
	data->addr1 = (page >> 16) & 0xff;
	data->chipsel = 0 | 4;	/* flash0 + undoc bit */
	data->clrfstatus = 0x00000020;
	data->clrrstatus = 0x000000C0;

	if (!raw_mode) {
		data->cfg0 = CFG0;
		data->cfg1 = CFG1;
		if (enable_bch_ecc) {
			data->ecc_bch_cfg = ECC_BCH_CFG;
		}
	} else {
		data->cfg0 =
		    (NAND_CFG0_RAW & ~(7 << 6)) | ((cwperpage - 1) << 6);
		data->cfg1 = NAND_CFG1_RAW | (CFG1 & CFG1_WIDE_FLASH);
	}

	/* GO bit for the EXEC register */
	data->exec = 1;

	if (modem_partition)
		data->ecc_cfg = 0x1FF;
	else
		data->ecc_cfg = 0x203;

	/* save existing ecc config */
	cmd->cmd = CMD_OCB;
	cmd->src = NAND_EBI2_ECC_BUF_CFG;
	cmd->dst = paddr(&data->ecc_cfg_save);
	cmd->len = 4;
	cmd++;

	for (n = 0; n < cwperpage; n++) {
		/* write CMD / ADDR0 / ADDR1 / CHIPSEL regs in a burst */
		cmd->cmd = DST_CRCI_NAND_CMD;
		cmd->src = paddr(&data->cmd);
		cmd->dst = NAND_FLASH_CMD;
		cmd->len = ((n == 0) ? 16 : 4);
		cmd++;

		if (n == 0) {
			/*  set configuration */
			cmd->cmd = 0;
			cmd->src = paddr(&data->cfg0);
			cmd->dst = NAND_DEV0_CFG0;
			if (enable_bch_ecc) {
				cmd->len = 12;
			} else {
				cmd->len = 8;
			}

			cmd++;

			/* set our ecc config */
			cmd->cmd = 0;
			cmd->src = paddr(&data->ecc_cfg);
			cmd->dst = NAND_EBI2_ECC_BUF_CFG;
			cmd->len = 4;
			cmd++;
		}

		/* write data block */
		cmd->cmd = 0;
		cmd->dst = NAND_FLASH_BUFFER;
		if (!raw_mode) {
			if (modem_partition) {
				cmd->src = addr + n * 512;
				cmd->len = 512;
			} else {
				cmd->src = addr + n * 516;
				cmd->len =
				    ((n <
				      (cwperpage - 1)) ? 516 : (512 -
								((cwperpage -
								  1) << 2)));
			}
		} else {
			cmd->src = addr;
			cmd->len = 528;
		}
		cmd++;

		if ((n == (cwperpage - 1)) && (!raw_mode) && (!modem_partition)) {
			/* write extra data */
			cmd->cmd = 0;
			cmd->src = spareaddr;
			cmd->dst =
			    NAND_FLASH_BUFFER + (512 - ((cwperpage - 1) << 2));
			cmd->len = (cwperpage << 2);
			cmd++;
		}

		/* kick the execute register */
		cmd->cmd = 0;
		cmd->src = paddr(&data->exec);
		cmd->dst = NAND_EXEC_CMD;
		cmd->len = 4;
		cmd++;

		/* block on data ready, then read the status register */
		cmd->cmd = SRC_CRCI_NAND_DATA;
		cmd->src = NAND_FLASH_STATUS;
		cmd->dst = paddr(&data->result[n]);
		cmd->len = 8;
		cmd++;

		cmd->cmd = 0;
		cmd->src = paddr(&data->clrfstatus);
		cmd->dst = NAND_FLASH_STATUS;
		cmd->len = 4;
		cmd++;

		cmd->cmd = 0;
		cmd->src = paddr(&data->clrrstatus);
		cmd->dst = NAND_READ_STATUS;
		cmd->len = 4;
		cmd++;
	}

	/* restore saved ecc config */
	cmd->cmd = CMD_OCU | CMD_LC;
	cmd->src = paddr(&data->ecc_cfg_save);
	cmd->dst = NAND_EBI2_ECC_BUF_CFG;
	cmd->len = 4;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "write page %d: status: %x %x %x %x\n",
		page, data[5], data[6], data[7], data[8]);
#endif

	/* if any of the writes failed (0x10), or there was a
	 ** protection violation (0x100), or the program success
	 ** bit (0x80) is unset, we lose
	 */
	for (n = 0; n < cwperpage; n++) {
		if (data->result[n].flash_status & 0x110)
			return -1;
		if (!(data->result[n].flash_status & 0x80))
			return -1;
	}

#if VERIFY_WRITE
	n = _flash_read_page(cmdlist, ptrlist, page, flash_data,
			     flash_data + 2048);
	if (n != 0)
		return -1;
	if (memcmp(flash_data, _addr, 2048) ||
	    memcmp(flash_data + 2048, _spareaddr, 16)) {
		dprintf(CRITICAL, "verify error @ page %d\n", page);
		return -1;
	}
#endif
	return 0;
}

static int
flash_nand_write_page_interleave(dmov_s * cmdlist, unsigned *ptrlist,
				 unsigned page, const void *_addr,
				 const void *_spareaddr, unsigned raw_mode)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct interleave_data_flash_io *data = (void *)(ptrlist + 4);
	unsigned addr = (unsigned)_addr;
	unsigned spareaddr = (unsigned)_spareaddr;
	unsigned n;
	unsigned cwperpage, cwcount;

	cwperpage = (flash_pagesize >> 9) * 2;	/* double for interleave mode */
	cwcount = (cwperpage << 1);

	data->cmd = NAND_CMD_PRG_PAGE;
	data->addr0 = page << 16;
	data->addr1 = (page >> 16) & 0xff;
	data->chipsel_cs0 = 0 | 4;	/* flash0 + undoc bit */
	data->chipsel_cs1 = 0 | 5;	/* flash0 + undoc bit */
	data->ebi2_chip_select_cfg0 = 0x00000805;
	data->adm_mux_data_ack_req_nc01 = 0x00000A3C;
	data->adm_mux_cmd_ack_req_nc01 = 0x0000053C;
	data->adm_mux_data_ack_req_nc10 = 0x00000F28;
	data->adm_mux_cmd_ack_req_nc10 = 0x00000F14;
	data->adm_default_mux = 0x00000FC0;
	data->default_ebi2_chip_select_cfg0 = 0x00000801;

	if (!raw_mode) {
		data->cfg0 = CFG0;
		data->cfg1 = CFG1;
	} else {
		data->cfg0 = (NAND_CFG0_RAW & ~(7 << 6)) | ((cwcount - 1) << 6);
		data->cfg1 = NAND_CFG1_RAW | (CFG1 & CFG1_WIDE_FLASH);
	}

	/* GO bit for the EXEC register */
	data->exec = 1;
	data->ecc_cfg = 0x203;

	for (n = 0; n < cwperpage; n++) {
		/* status return words */
		data->result[n].flash_status = 0xeeeeeeee;

		if (n == 0) {
			/* enable CS1 */
			cmd->cmd = CMD_OCB;
			cmd->src = paddr(&data->ebi2_chip_select_cfg0);
			cmd->dst = EBI2_CHIP_SELECT_CFG0;
			cmd->len = 4;
			cmd++;

			/* save existing ecc config */
			cmd->cmd = 0;
			cmd->src = NC11(NAND_EBI2_ECC_BUF_CFG);
			cmd->dst = paddr(&data->ecc_cfg_save);
			cmd->len = 4;
			cmd++;

			cmd->cmd = 0;
			cmd->src = paddr(&data->ecc_cfg);
			cmd->dst = NC11(NAND_EBI2_ECC_BUF_CFG);
			cmd->len = 4;
			cmd++;

			cmd->cmd = 0;
			cmd->src = paddr(&data->addr0);
			cmd->dst = NC11(NAND_ADDR0);
			cmd->len = 8;
			cmd++;

			/* enable CS0 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->chipsel_cs0);
			cmd->dst = NC01(NAND_FLASH_CHIP_SELECT);
			cmd->len = 4;
			cmd++;

			/* enable CS1 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->chipsel_cs1);
			cmd->dst = NC10(NAND_FLASH_CHIP_SELECT);
			cmd->len = 4;
			cmd++;

			cmd->cmd = 0;
			cmd->src = paddr(&data->cfg0);
			cmd->dst = NC01(NAND_DEV0_CFG0);
			cmd->len = 8;
			cmd++;

			/* config CFG1 for CS1 */
			cmd->cmd = 0;
			cmd->src = paddr(&data->cfg0);
			cmd->dst = NC10(NAND_DEV1_CFG0);
			cmd->len = 8;
			cmd++;
		}

		if (n % 2 == 0) {
			/* MASK CMD ACK/REQ --> NC10 (0xF14) */
			cmd->cmd = 0;
			cmd->src = paddr(&data->adm_mux_cmd_ack_req_nc10);
			cmd->dst = EBI2_NAND_ADM_MUX;
			cmd->len = 4;
			cmd++;

			/* CMD */
			cmd->cmd = DST_CRCI_NAND_CMD;
			cmd->src = paddr(&data->cmd);
			cmd->dst = NC01(NAND_FLASH_CMD);
			cmd->len = 4;
			cmd++;
		} else {
			/* MASK CMD ACK/REQ --> NC01 (0x53C) */
			cmd->cmd = 0;
			cmd->src = paddr(&data->adm_mux_cmd_ack_req_nc01);
			cmd->dst = EBI2_NAND_ADM_MUX;
			cmd->len = 4;
			cmd++;

			/* CMD */
			cmd->cmd = DST_CRCI_NAND_CMD;
			cmd->src = paddr(&data->cmd);
			cmd->dst = NC10(NAND_FLASH_CMD);
			cmd->len = 4;
			cmd++;
		}

		cmd->cmd = 0;
		if (!raw_mode) {
			cmd->src = addr + n * 516;
			cmd->len =
			    ((n <
			      (cwperpage - 1)) ? 516 : (512 -
							((cwperpage -
							  1) << 2)));
		} else {
			cmd->src = addr;
			cmd->len = 528;
		}

		if (n % 2 == 0)
			cmd->dst = NC01(NAND_FLASH_BUFFER);
		else
			cmd->dst = NC10(NAND_FLASH_BUFFER);
		cmd++;

		if ((n == (cwperpage - 1)) && (!raw_mode)) {
			/* write extra data */
			cmd->cmd = 0;
			cmd->src = spareaddr;
			cmd->dst =
			    NC10(NAND_FLASH_BUFFER) + (512 -
						       ((cwperpage - 1) << 2));
			cmd->len = (cwperpage << 2);
			cmd++;
		}

		if (n % 2 == 0) {
			/* kick the NC01 execute register */
			cmd->cmd = 0;
			cmd->src = paddr(&data->exec);
			cmd->dst = NC01(NAND_EXEC_CMD);
			cmd->len = 4;
			cmd++;
			if (n != 0) {
				/* MASK DATA ACK/REQ --> NC01 (0xA3C) */
				cmd->cmd = 0;
				cmd->src =
				    paddr(&data->adm_mux_data_ack_req_nc01);
				cmd->dst = EBI2_NAND_ADM_MUX;
				cmd->len = 4;
				cmd++;

				/* block on data ready from NC10, then
				 * read the status register
				 */
				cmd->cmd = SRC_CRCI_NAND_DATA;
				cmd->src = NC10(NAND_FLASH_STATUS);
				cmd->dst = paddr(&data->result[n - 1]);
				cmd->len = 4;
				cmd++;
			}
		} else {
			/* kick the execute register */
			cmd->cmd = 0;
			cmd->src = paddr(&data->exec);
			cmd->dst = NC10(NAND_EXEC_CMD);
			cmd->len = 4;
			cmd++;

			/* MASK DATA ACK/REQ --> NC10 (0xF28) */
			cmd->cmd = 0;
			cmd->src = paddr(&data->adm_mux_data_ack_req_nc10);
			cmd->dst = EBI2_NAND_ADM_MUX;
			cmd->len = 4;
			cmd++;

			/* block on data ready from NC01, then
			 * read the status register
			 */
			cmd->cmd = SRC_CRCI_NAND_DATA;
			cmd->src = NC01(NAND_FLASH_STATUS);
			cmd->dst = paddr(&data->result[n - 1]);
			cmd->len = 4;
			cmd++;
		}
	}

	/* MASK DATA ACK/REQ --> NC01 (0xA3C) */
	cmd->cmd = 0;
	cmd->src = paddr(&data->adm_mux_data_ack_req_nc01);
	cmd->dst = EBI2_NAND_ADM_MUX;
	cmd->len = 4;
	cmd++;

	/* we should process outstanding request */
	/* block on data ready, then
	 * read the status register
	 */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NC10(NAND_FLASH_STATUS);
	cmd->dst = paddr(&data->result[n - 1]);
	cmd->len = 4;
	cmd++;

	/* restore saved ecc config */
	cmd->cmd = 0;
	cmd->src = paddr(&data->ecc_cfg_save);
	cmd->dst = NAND_EBI2_ECC_BUF_CFG;
	cmd->len = 4;

	/* MASK DATA ACK/REQ --> NC01 (0xFC0) */
	cmd->cmd = 0;
	cmd->src = paddr(&data->adm_default_mux);
	cmd->dst = EBI2_NAND_ADM_MUX;
	cmd->len = 4;
	cmd++;

	/* disable CS1 */
	cmd->cmd = CMD_OCU | CMD_LC;
	cmd->src = paddr(&data->default_ebi2_chip_select_cfg0);
	cmd->dst = EBI2_CHIP_SELECT_CFG0;
	cmd->len = 4;
	cmd++;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "write page %d: status: %x %x %x %x %x %x %x %x \
	%x %x %x %x %x %x %x %x \n", page, data->result[0].flash_status[0], data->result[1].flash_status[1], data->result[2].flash_status[2], data->result[3].flash_status[3], data->result[4].flash_status[4], data->result[5].flash_status[5], data->result[6].flash_status[6], data->result[7].flash_status[7], data->result[8].flash_status[8], data->result[9].flash_status[9], data->result[10].flash_status[10], data->result[11].flash_status[11], data->result[12].flash_status[12], data->result[13].flash_status[13], data->result[14].flash_status[14], data->result[15].flash_status[15]);
#endif

	/* if any of the writes failed (0x10), or there was a
	 ** protection violation (0x100), or the program success
	 ** bit (0x80) is unset, we lose
	 */
	for (n = 0; n < cwperpage; n++) {
		if (data->result[n].flash_status & 0x110)
			return -1;
		if (!(data->result[n].flash_status & 0x80))
			return -1;
	}

#if VERIFY_WRITE
	n = _flash_read_page(cmdlist, ptrlist, page, flash_data,
			     flash_data + 2048);
	if (n != 0)
		return -1;
	if (memcmp(flash_data, _addr, 2048) ||
	    memcmp(flash_data + 2048, _spareaddr, 16)) {
		dprintf(CRITICAL, "verify error @ page %d\n", page);
		return -1;
	}
#endif
	return 0;
}

char empty_buf[528];
static int
flash_nand_mark_badblock(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	memset(empty_buf, 0, 528);
	/* Going to first page of the block */
	if (page & num_pages_per_blk_mask)
		page = page - (page & num_pages_per_blk_mask);
	return _flash_nand_write_page(cmdlist, ptrlist, page, empty_buf, 0, 1);
}

unsigned nand_cfg0;
unsigned nand_cfg1;

static int flash_nand_read_config(dmov_s * cmdlist, unsigned *ptrlist)
{
	static unsigned CFG0_TMP, CFG1_TMP;
	cmdlist[0].cmd = CMD_OCB;
	cmdlist[0].src = NAND_DEV0_CFG0;
	cmdlist[0].dst = paddr(&CFG0_TMP);
	cmdlist[0].len = 4;

	cmdlist[1].cmd = CMD_OCU | CMD_LC;
	cmdlist[1].src = NAND_DEV0_CFG1;
	cmdlist[1].dst = paddr(&CFG1_TMP);
	cmdlist[1].len = 4;

	*ptrlist = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptrlist);

	if ((CFG0_TMP == 0) || (CFG1_TMP == 0)) {
		return -1;
	}

	CFG0_A = CFG0_TMP;
	CFG1_A = CFG1_TMP;
	if (flash_info.type == FLASH_16BIT_NAND_DEVICE) {
		nand_cfg1 |= CFG1_WIDE_FLASH;
	}
	dprintf(INFO, "nandcfg: %x %x (initial)\n", CFG0, CFG1);

	CFG0_A = (((flash_pagesize >> 9) - 1) << 6)	/* 4/8 cw/pg for 2/4k */
	    |(516 << 9)		/* 516 user data bytes */
	    |(10 << 19)		/* 10 parity bytes */
	    |(5 << 27)		/* 5 address cycles */
	    |(0 << 30)		/* Do not read status before data */
	    |(1 << 31)
	    /* Send read cmd */
	    /* 0 spare bytes for 16 bit nand or 1 spare bytes for 8 bit */
	    |((nand_cfg1 & CFG1_WIDE_FLASH) ? (0 << 23) : (enable_bch_ecc ? (2 << 23) : (1 << 23)));	/* 2 spare bytes for 8 bit bch ecc */
	CFG1_A = (0 << 0)	/* Enable ecc */
	    |(7 << 2)		/* 8 recovery cycles */
	    |(0 << 5)		/* Allow CS deassertion */
	    |((flash_pagesize - ((enable_bch_ecc ? 532 : 528) * ((flash_pagesize >> 9) - 1)) + 1) << 6)	/* Bad block marker location */
	    |(0 << 16)		/* Bad block in user data area */
	    |(2 << 17)		/* 6 cycle tWB/tRB */
	    |(nand_cfg1 & CFG1_WIDE_FLASH);	/* preserve wide flash flag */

	NAND_CFG0_RAW = CFG0_RAW;
	NAND_CFG1_RAW = CFG1_RAW;

	if (enable_bch_ecc) {
		CFG1_A |= (1 << 27);	/* Enable BCH engine */
		ECC_BCH_CFG = (0 << 0)	/* Enable ECC */
		    |(0 << 1)	/* Enable/Disable SW reset of ECC engine */
		    |(1 << 4)	/* 8bit ecc */
		    |((nand_cfg1 & CFG1_WIDE_FLASH) ? (14 << 8) : (13 << 8))	/*parity bytes */
		    |(516 << 16)	/* 516 user data bytes */
		    |(1 << 30);	/* Turn on ECC engine clocks always */
		NAND_CFG0_RAW = CFG0_RAW_BCHECC;	/* CW size is increased to 532B */
	}

	dprintf(INFO, "nandcfg(Apps): %x %x (used)\n", CFG0_A, CFG1_A);

	CFG0_M = CFG0_TMP;
	CFG1_M = CFG1_TMP;
	if (flash_info.type == FLASH_16BIT_NAND_DEVICE) {
		nand_cfg1 |= CFG1_WIDE_FLASH;
	}
	CFG0_M = (((flash_pagesize >> 9) - 1) << 6)	/* 4/8 cw/pg for 2/4k */
	    |(512 << 9)		/* 512 user data bytes */
	    |(10 << 19)		/* 10 parity bytes */
	    |(5 << 27)		/* 5 address cycles */
	    |(0 << 30)		/* Do not read status before data */
	    |(1 << 31)		/* Send read cmd */
	    |((nand_cfg1 & CFG1_WIDE_FLASH) ? (4 << 23) : (5 << 23));
	CFG1_M = (0 << 0)	/* Enable ecc */
	    |(7 << 2)		/* 8 recovery cycles */
	    |(0 << 5)		/* Allow CS deassertion */
	    |((flash_pagesize - (528 * ((flash_pagesize >> 9) - 1)) + 1) << 6)	/* Bad block marker location */
	    |(0 << 16)		/* Bad block in user data area */
	    |(2 << 17)		/* 6 cycle tWB/tRB */
	    |(nand_cfg1 & CFG1_WIDE_FLASH);	/* preserve wide flash flag */
	dprintf(INFO, "nandcfg(Modem): %x %x (used)\n", CFG0_M, CFG1_M);
	return 0;
}

/* OneNAND programming functions */

static void flash_onenand_read_id(dmov_s * cmdlist, unsigned *ptrlist)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;

	data[0] = SFLASH_BCFG;
	data[1] =
	    SFLASH_PREPCMD(8, 0, 0, NAND_SFCMD_DATXS, NAND_SFCMD_ASYNC,
			   NAND_SFCMD_REGRD);
	data[2] = (ONENAND_DEVICE_ID << 16) | (ONENAND_MANUFACTURER_ID);
	data[3] = (ONENAND_DATA_BUFFER_SIZE << 16) | (ONENAND_VERSION_ID);
	data[4] =
	    (ONENAND_AMOUNT_OF_BUFFERS << 16) | (ONENAND_BOOT_BUFFER_SIZE);
	data[5] = (CLEAN_DATA_16 << 16) | (ONENAND_TECHNOLOGY);
	data[6] = CLEAN_DATA_32;	//status
	data[7] = CLEAN_DATA_32;	//register read
	data[8] = CLEAN_DATA_32;	//register read
	data[9] = CLEAN_DATA_32;	//register read
	data[10] = CLEAN_DATA_32;	//register read
	data[11] = 1;
	data[12] = 0 | 4;

	/* Setup controller in SFLASH mode */
	cmd[0].cmd = 0 | CMD_OCB;
	cmd[0].src = paddr(&data[0]);
	cmd[0].dst = NAND_SFLASHC_BURST_CFG;
	cmd[0].len = 4;

	/* Enable data mover for controller */
	cmd[1].cmd = 0;
	cmd[1].src = paddr(&data[12]);
	cmd[1].dst = NAND_FLASH_CHIP_SELECT;
	cmd[1].len = 4;

	/* Setup SFLASHC_CMD with xfers in async mode */
	cmd[2].cmd = DST_CRCI_NAND_CMD;
	cmd[2].src = paddr(&data[1]);
	cmd[2].dst = NAND_SFLASHC_CMD;
	cmd[2].len = 4;

	/* Setup to read device information */
	cmd[3].cmd = 0;
	cmd[3].src = paddr(&data[2]);
	cmd[3].dst = NAND_ADDR0;
	cmd[3].len = 8;

	cmd[4].cmd = 0;
	cmd[4].src = paddr(&data[4]);
	cmd[4].dst = NAND_ADDR2;
	cmd[4].len = 8;

	/* Set execute bit */
	cmd[5].cmd = 0;
	cmd[5].src = paddr(&data[11]);
	cmd[5].dst = NAND_SFLASHC_EXEC_CMD;
	cmd[5].len = 4;

	/* Check status */
	cmd[6].cmd = SRC_CRCI_NAND_DATA;
	cmd[6].src = NAND_SFLASHC_STATUS;
	cmd[6].dst = paddr(&data[6]);
	cmd[6].len = 4;

	/* Read result device registers */
	cmd[7].cmd = 0 | CMD_OCU | CMD_LC;
	cmd[7].src = NAND_GENP_REG0;
	cmd[7].dst = paddr(&data[7]);
	cmd[7].len = 16;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

#if VERBOSE
	dprintf(INFO, "status: %x\n", data[6]);
#endif

	flash_info.id = data[7];
	flash_info.vendor = data[7] & CLEAN_DATA_16;
	flash_info.device = (data[7] >> 16) & CLEAN_DATA_16;
	return;
}

struct data_onenand_erase {
	unsigned sfbcfg;
	unsigned sfcmd[4];
	unsigned sfexec;
	unsigned sfstat[4];
	unsigned addr0;
	unsigned addr1;
	unsigned addr2;
	unsigned addr3;
	unsigned addr4;
	unsigned addr5;
	unsigned addr6;
	unsigned data0;
	unsigned data1;
	unsigned data2;
	unsigned data3;
	unsigned data4;
	unsigned data5;
	unsigned data6;
};

static int _flash_onenand_read_page(dmov_s * cmdlist, unsigned *ptrlist,
				    unsigned page, void *_addr,
				    void *_spareaddr, unsigned raw_mode);

static int
flash_onenand_block_isbad(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	unsigned char page_data[2112];
	unsigned char *oobptr = &(page_data[2048]);

	/* Going to first page of the block */
	if (page & num_pages_per_blk_mask)
		page = page - (page & num_pages_per_blk_mask);

	/* Reading page in raw mode */
	if (_flash_onenand_read_page(cmdlist, ptrlist, page, page_data, 0, 1))
		return 1;

	/* Checking if block is bad */
	if ((oobptr[0] != 0xFF) || (oobptr[1] != 0xFF) ||
	    (oobptr[16] != 0xFF) || (oobptr[17] != 0xFF) ||
	    (oobptr[32] != 0xFF) || (oobptr[33] != 0xFF) ||
	    (oobptr[48] != 0xFF) || (oobptr[49] != 0xFF)) {
		return 1;
	}
	return 0;
}

static int
flash_onenand_erase_block(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct data_onenand_erase *data = (void *)ptrlist + 4;
	int isbad = 0;
	unsigned erasesize = (flash_pagesize * num_pages_per_blk);
	unsigned onenand_startaddr1 =
	    DEVICE_FLASHCORE_0 | (page * flash_pagesize) / erasesize;
	unsigned onenand_startaddr8 = 0x0000;
	unsigned onenand_startaddr2 = DEVICE_BUFFERRAM_0 << 15;
	unsigned onenand_startbuffer = DATARAM0_0 << 8;

	unsigned controller_status;
	unsigned interrupt_status;
	unsigned ecc_status;

	if ((page * flash_pagesize) & (erasesize - 1))
		return -1;

	/* Check for bad block and erase only if block is not marked bad */
	isbad = flash_onenand_block_isbad(cmdlist, ptrlist, page);
	if (isbad) {
		dprintf(INFO, "skipping @ %d (bad block)\n",
			page / num_pages_per_blk);
		return -1;
	}

	/*Erase block */
	onenand_startaddr1 = DEVICE_FLASHCORE_0 |
	    ((page * flash_pagesize) / (erasesize));
	onenand_startaddr8 = 0x0000;
	onenand_startaddr2 = DEVICE_BUFFERRAM_0 << 15;
	onenand_startbuffer = DATARAM0_0 << 8;

	data->sfbcfg = SFLASH_BCFG;
	data->sfcmd[0] = SFLASH_PREPCMD(7, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGWR);
	data->sfcmd[1] = SFLASH_PREPCMD(0, 0, 32,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_INTHI);
	data->sfcmd[2] = SFLASH_PREPCMD(3, 7, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGRD);
	data->sfcmd[3] = SFLASH_PREPCMD(4, 10, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGWR);
	data->sfexec = 1;
	data->sfstat[0] = CLEAN_DATA_32;
	data->sfstat[1] = CLEAN_DATA_32;
	data->sfstat[2] = CLEAN_DATA_32;
	data->sfstat[3] = CLEAN_DATA_32;
	data->addr0 =
	    (ONENAND_INTERRUPT_STATUS << 16) | (ONENAND_SYSTEM_CONFIG_1);
	data->addr1 =
	    (ONENAND_START_ADDRESS_8 << 16) | (ONENAND_START_ADDRESS_1);
	data->addr2 = (ONENAND_START_BUFFER << 16) | (ONENAND_START_ADDRESS_2);
	data->addr3 = (ONENAND_ECC_STATUS << 16) | (ONENAND_COMMAND);
	data->addr4 = (ONENAND_CONTROLLER_STATUS << 16) |
	    (ONENAND_INTERRUPT_STATUS);
	data->addr5 =
	    (ONENAND_INTERRUPT_STATUS << 16) | (ONENAND_SYSTEM_CONFIG_1);
	data->addr6 =
	    (ONENAND_START_ADDRESS_3 << 16) | (ONENAND_START_ADDRESS_1);
	data->data0 = (ONENAND_CLRINTR << 16) | (ONENAND_SYSCFG1_ECCENA);
	data->data1 = (onenand_startaddr8 << 16) | (onenand_startaddr1);
	data->data2 = (onenand_startbuffer << 16) | (onenand_startaddr2);
	data->data3 = (CLEAN_DATA_16 << 16) | (ONENAND_CMDERAS);
	data->data4 = (CLEAN_DATA_16 << 16) | (CLEAN_DATA_16);
	data->data5 = (ONENAND_CLRINTR << 16) | (ONENAND_SYSCFG1_ECCENA);
	data->data6 = (ONENAND_STARTADDR3_RES << 16) | (ONENAND_STARTADDR1_RES);

	/***************************************************************/
	/* Write the necessary address registers in the onenand device */
	/***************************************************************/

	/* Enable and configure the SFlash controller */
	cmd->cmd = 0 | CMD_OCB;
	cmd->src = paddr(&data->sfbcfg);
	cmd->dst = NAND_SFLASHC_BURST_CFG;
	cmd->len = 4;
	cmd++;

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[0]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Write the ADDR0 and ADDR1 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr0);
	cmd->dst = NAND_ADDR0;
	cmd->len = 8;
	cmd++;

	/* Write the ADDR2 ADDR3 ADDR4 ADDR5 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr2);
	cmd->dst = NAND_ADDR2;
	cmd->len = 16;
	cmd++;

	/* Write the ADDR6 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr6);
	cmd->dst = NAND_ADDR6;
	cmd->len = 4;
	cmd++;

	/* Write the GENP0, GENP1, GENP2, GENP3, GENP4 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->data0);
	cmd->dst = NAND_GENP_REG0;
	cmd->len = 16;
	cmd++;

	/* Write the FLASH_DEV_CMD4,5,6 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->data4);
	cmd->dst = NAND_DEV_CMD4;
	cmd->len = 12;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[0]);
	cmd->len = 4;
	cmd++;

	/***************************************************************/
	/* Wait for the interrupt from the Onenand device controller    */
	/***************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[1]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[1]);
	cmd->len = 4;
	cmd++;

	/***************************************************************/
	/* Read the necessary status registers from the onenand device */
	/***************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[2]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[2]);
	cmd->len = 4;
	cmd++;

	/* Read the GENP3 register */
	cmd->cmd = 0;
	cmd->src = NAND_GENP_REG3;
	cmd->dst = paddr(&data->data3);
	cmd->len = 4;
	cmd++;

	/* Read the DEVCMD4 register */
	cmd->cmd = 0;
	cmd->src = NAND_DEV_CMD4;
	cmd->dst = paddr(&data->data4);
	cmd->len = 4;
	cmd++;

	/***************************************************************/
	/* Restore the necessary registers to proper values                     */
	/***************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[3]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA | CMD_OCU | CMD_LC;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[3]);
	cmd->len = 4;
	cmd++;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

	ecc_status = (data->data3 >> 16) & 0x0000FFFF;
	interrupt_status = (data->data4 >> 0) & 0x0000FFFF;
	controller_status = (data->data4 >> 16) & 0x0000FFFF;

#if VERBOSE
	dprintf(INFO, "\n%s: sflash status %x %x %x %x\n", __func__,
		data->sfstat[0],
		data->sfstat[1], data->sfstat[2], data->sfstat[3]);

	dprintf(INFO, "%s: controller_status = %x\n", __func__,
		controller_status);
	dprintf(INFO, "%s: interrupt_status = %x\n", __func__,
		interrupt_status);
	dprintf(INFO, "%s: ecc_status = %x\n", __func__, ecc_status);
#endif
	/* Check for errors, protection violations etc */
	if ((controller_status != 0)
	    || (data->sfstat[0] & 0x110)
	    || (data->sfstat[1] & 0x110)
	    || (data->sfstat[2] & 0x110) || (data->sfstat[3] & 0x110)) {
		dprintf(CRITICAL, "%s: ECC/MPU/OP error\n", __func__);
		return -1;
	}

#if VERBOSE
	dprintf(INFO, "status: %x\n", data[5]);
#endif

	return 0;
}

struct data_onenand_read {
	unsigned sfbcfg;
	unsigned sfcmd[9];
	unsigned sfexec;
	unsigned sfstat[9];
	unsigned addr0;
	unsigned addr1;
	unsigned addr2;
	unsigned addr3;
	unsigned addr4;
	unsigned addr5;
	unsigned addr6;
	unsigned data0;
	unsigned data1;
	unsigned data2;
	unsigned data3;
	unsigned data4;
	unsigned data5;
	unsigned data6;
	unsigned macro[5];
};

static int
_flash_onenand_read_page(dmov_s * cmdlist, unsigned *ptrlist,
			 unsigned page, void *_addr, void *_spareaddr,
			 unsigned raw_mode)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct data_onenand_read *data = (void *)(ptrlist + 4);
	unsigned addr = (unsigned)_addr;
	unsigned curr_addr = (unsigned)_addr;
#if VERBOSE
	unsigned spareaddr = (unsigned)_spareaddr;
#endif
	unsigned i;
	unsigned erasesize = (flash_pagesize * num_pages_per_blk);
	unsigned writesize = flash_pagesize;

	unsigned onenand_startaddr1 = DEVICE_FLASHCORE_0 |
	    ((unsigned)(page * flash_pagesize) / erasesize);
	unsigned onenand_startaddr8 = (((unsigned)(page * flash_pagesize) &
					(erasesize - 1)) / writesize) << 2;
	unsigned onenand_startaddr2 = DEVICE_BUFFERRAM_0 << 15;
	unsigned onenand_startbuffer = DATARAM0_0 << 8;
	unsigned onenand_sysconfig1 = (raw_mode == 1) ? ONENAND_SYSCFG1_ECCDIS :
	    ONENAND_SYSCFG1_ECCENA;

	unsigned controller_status;
	unsigned interrupt_status;
	unsigned ecc_status;
	if (raw_mode != 1) {
		int isbad = 0;
		isbad = flash_onenand_block_isbad(cmdlist, ptrlist, page);
		if (isbad)
			return -2;
	}
	//static int oobfree_offset[8] = {2, 14, 18, 30, 34, 46, 50, 62};
	//static int oobfree_length[8] = {3, 2, 3, 2, 3, 2, 3, 2};

	data->sfbcfg = SFLASH_BCFG;
	data->sfcmd[0] = SFLASH_PREPCMD(7, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGWR);
	data->sfcmd[1] = SFLASH_PREPCMD(0, 0, 32,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_INTHI);
	data->sfcmd[2] = SFLASH_PREPCMD(3, 7, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGRD);
	data->sfcmd[3] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATRD);
	data->sfcmd[4] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATRD);
	data->sfcmd[5] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATRD);
	data->sfcmd[6] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATRD);
	data->sfcmd[7] = SFLASH_PREPCMD(32, 0, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATRD);
	data->sfcmd[8] = SFLASH_PREPCMD(4, 10, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGWR);
	data->sfexec = 1;
	data->sfstat[0] = CLEAN_DATA_32;
	data->sfstat[1] = CLEAN_DATA_32;
	data->sfstat[2] = CLEAN_DATA_32;
	data->sfstat[3] = CLEAN_DATA_32;
	data->sfstat[4] = CLEAN_DATA_32;
	data->sfstat[5] = CLEAN_DATA_32;
	data->sfstat[6] = CLEAN_DATA_32;
	data->sfstat[7] = CLEAN_DATA_32;
	data->sfstat[8] = CLEAN_DATA_32;

	data->addr0 =
	    (ONENAND_INTERRUPT_STATUS << 16) | (ONENAND_SYSTEM_CONFIG_1);
	data->addr1 =
	    (ONENAND_START_ADDRESS_8 << 16) | (ONENAND_START_ADDRESS_1);
	data->addr2 = (ONENAND_START_BUFFER << 16) | (ONENAND_START_ADDRESS_2);
	data->addr3 = (ONENAND_ECC_STATUS << 16) | (ONENAND_COMMAND);
	data->addr4 = (ONENAND_CONTROLLER_STATUS << 16) |
	    (ONENAND_INTERRUPT_STATUS);
	data->addr5 =
	    (ONENAND_INTERRUPT_STATUS << 16) | (ONENAND_SYSTEM_CONFIG_1);
	data->addr6 =
	    (ONENAND_START_ADDRESS_3 << 16) | (ONENAND_START_ADDRESS_1);
	data->data0 = (ONENAND_CLRINTR << 16) | (onenand_sysconfig1);
	data->data1 = (onenand_startaddr8 << 16) | (onenand_startaddr1);
	data->data2 = (onenand_startbuffer << 16) | (onenand_startaddr2);
	data->data3 = (CLEAN_DATA_16 << 16) | (ONENAND_CMDLOADSPARE);
	data->data4 = (CLEAN_DATA_16 << 16) | (CLEAN_DATA_16);
	data->data5 = (ONENAND_CLRINTR << 16) | (ONENAND_SYSCFG1_ECCENA);
	data->data6 = (ONENAND_STARTADDR3_RES << 16) | (ONENAND_STARTADDR1_RES);
	data->macro[0] = 0x0200;
	data->macro[1] = 0x0300;
	data->macro[2] = 0x0400;
	data->macro[3] = 0x0500;
	data->macro[4] = 0x8010;

	/*************************************************************/
	/* Write necessary address registers in the onenand device      */
	/*************************************************************/

	/* Enable and configure the SFlash controller */
	cmd->cmd = 0 | CMD_OCB;
	cmd->src = paddr(&data->sfbcfg);
	cmd->dst = NAND_SFLASHC_BURST_CFG;
	cmd->len = 4;
	cmd++;

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[0]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Write the ADDR0 and ADDR1 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr0);
	cmd->dst = NAND_ADDR0;
	cmd->len = 8;
	cmd++;

	/* Write the ADDR2 ADDR3 ADDR4 ADDR5 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr2);
	cmd->dst = NAND_ADDR2;
	cmd->len = 16;
	cmd++;

	/* Write the ADDR6 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr6);
	cmd->dst = NAND_ADDR6;
	cmd->len = 4;
	cmd++;

	/* Write the GENP0, GENP1, GENP2, GENP3 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->data0);
	cmd->dst = NAND_GENP_REG0;
	cmd->len = 16;
	cmd++;

	/* Write the FLASH_DEV_CMD4,5,6 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->data4);
	cmd->dst = NAND_DEV_CMD4;
	cmd->len = 12;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[0]);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Wait for the interrupt from the Onenand device controller */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[1]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[1]);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Read necessary status registers from the onenand device      */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[2]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[2]);
	cmd->len = 4;
	cmd++;

	/* Read the GENP3 register */
	cmd->cmd = 0;
	cmd->src = NAND_GENP_REG3;
	cmd->dst = paddr(&data->data3);
	cmd->len = 4;
	cmd++;

	/* Read the DEVCMD4 register */
	cmd->cmd = 0;
	cmd->src = NAND_DEV_CMD4;
	cmd->dst = paddr(&data->data4);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Read the data ram area from the onenand buffer ram           */
	/*************************************************************/

	if (addr) {

		data->data3 = (CLEAN_DATA_16 << 16) | (ONENAND_CMDLOAD);

		for (i = 0; i < 4; i++) {

			/* Block on cmd ready and write CMD register */
			cmd->cmd = DST_CRCI_NAND_CMD;
			cmd->src = paddr(&data->sfcmd[3 + i]);
			cmd->dst = NAND_SFLASHC_CMD;
			cmd->len = 4;
			cmd++;

			/* Write the MACRO1 register */
			cmd->cmd = 0;
			cmd->src = paddr(&data->macro[i]);
			cmd->dst = NAND_MACRO1_REG;
			cmd->len = 4;
			cmd++;

			/* Kick the execute command */
			cmd->cmd = 0;
			cmd->src = paddr(&data->sfexec);
			cmd->dst = NAND_SFLASHC_EXEC_CMD;
			cmd->len = 4;
			cmd++;

			/* Block on data rdy, & read status register */
			cmd->cmd = SRC_CRCI_NAND_DATA;
			cmd->src = NAND_SFLASHC_STATUS;
			cmd->dst = paddr(&data->sfstat[3 + i]);
			cmd->len = 4;
			cmd++;

			/* Transfer nand ctlr buf contents to usr buf */
			cmd->cmd = 0;
			cmd->src = NAND_FLASH_BUFFER;
			cmd->dst = curr_addr;
			cmd->len = 512;
			curr_addr += 512;
			cmd++;
		}
	}

	/* Read oob bytes in Raw Mode */
	if (raw_mode == 1) {
		/* Block on cmd ready and write CMD register */
		cmd->cmd = DST_CRCI_NAND_CMD;
		cmd->src = paddr(&data->sfcmd[7]);
		cmd->dst = NAND_SFLASHC_CMD;
		cmd->len = 4;
		cmd++;

		/* Write the MACRO1 register */
		cmd->cmd = 0;
		cmd->src = paddr(&data->macro[4]);
		cmd->dst = NAND_MACRO1_REG;
		cmd->len = 4;
		cmd++;

		/* Kick the execute command */
		cmd->cmd = 0;
		cmd->src = paddr(&data->sfexec);
		cmd->dst = NAND_SFLASHC_EXEC_CMD;
		cmd->len = 4;
		cmd++;

		/* Block on data rdy, & read status register */
		cmd->cmd = SRC_CRCI_NAND_DATA;
		cmd->src = NAND_SFLASHC_STATUS;
		cmd->dst = paddr(&data->sfstat[7]);
		cmd->len = 4;
		cmd++;

		/* Transfer nand ctlr buf contents to usr buf */
		cmd->cmd = 0;
		cmd->src = NAND_FLASH_BUFFER;
		cmd->dst = curr_addr;
		cmd->len = 64;
		curr_addr += 64;
		cmd++;
	}

	/*************************************************************/
	/* Restore the necessary registers to proper values               */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[8]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA | CMD_OCU | CMD_LC;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[8]);
	cmd->len = 4;
	cmd++;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

	ecc_status = (data->data3 >> 16) & 0x0000FFFF;
	interrupt_status = (data->data4 >> 0) & 0x0000FFFF;
	controller_status = (data->data4 >> 16) & 0x0000FFFF;

#if VERBOSE
	dprintf(INFO, "\n%s: sflash status %x %x %x %x %x %x %x"
		"%x %x\n", __func__,
		data->sfstat[0],
		data->sfstat[1],
		data->sfstat[2],
		data->sfstat[3],
		data->sfstat[4],
		data->sfstat[5], data->sfstat[6], data->sfstat[7]);

	dprintf(INFO, "%s: controller_status = %x\n", __func__,
		controller_status);
	dprintf(INFO, "%s: interrupt_status = %x\n", __func__,
		interrupt_status);
	dprintf(INFO, "%s: ecc_status = %x\n", __func__, ecc_status);
#endif
	/* Check for errors, protection violations etc */
	if ((controller_status != 0)
	    || (data->sfstat[0] & 0x110)
	    || (data->sfstat[1] & 0x110)
	    || (data->sfstat[2] & 0x110)
	    || ((data->sfstat[3] & 0x110) && (addr))
	    || ((data->sfstat[4] & 0x110) && (addr))
	    || ((data->sfstat[5] & 0x110) &&
		(addr)) || ((data->sfstat[6] & 0x110) && (addr))) {
		dprintf(INFO, "%s: ECC/MPU/OP error\n", __func__);
		return -1;
	}
#if VERBOSE
	dprintf(INFO, "read page %d: status: %x %x %x %x\n",
		page, data[5], data[6], data[7], data[8]);
	for (n = 0; n < 4; n++) {
		ptr = (unsigned *)(addr + 512 * n);
		dprintf(INFO, "data%d:	%x %x %x %x\n", n, ptr[0], ptr[1],
			ptr[2], ptr[3]);
		ptr = (unsigned *)(spareaddr + 16 * n);
		dprintf(INFO, "spare data%d	%x %x %x %x\n", n, ptr[0],
			ptr[1], ptr[2], ptr[3]);
	}
#endif

	return 0;
}

struct data_onenand_write {
	unsigned sfbcfg;
	unsigned sfcmd[9];
	unsigned sfexec;
	unsigned sfstat[9];
	unsigned addr0;
	unsigned addr1;
	unsigned addr2;
	unsigned addr3;
	unsigned addr4;
	unsigned addr5;
	unsigned addr6;
	unsigned data0;
	unsigned data1;
	unsigned data2;
	unsigned data3;
	unsigned data4;
	unsigned data5;
	unsigned data6;
	unsigned macro[5];
};

static int
_flash_onenand_write_page(dmov_s * cmdlist, unsigned *ptrlist,
			  unsigned page, const void *_addr,
			  const void *_spareaddr, unsigned raw_mode)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	struct data_onenand_write *data = (void *)(ptrlist + 4);
	unsigned addr = (unsigned)_addr;
	unsigned addr_curr = (unsigned)_addr;
	char *spareaddr = (char *)_spareaddr;
	unsigned i, j, k;

	unsigned erasesize = (flash_pagesize * num_pages_per_blk);
	unsigned writesize = flash_pagesize;

	unsigned onenand_startaddr1 = (page * flash_pagesize) / erasesize;
	unsigned onenand_startaddr8 = (((unsigned)(page * flash_pagesize) &
					(erasesize - 1)) / writesize) << 2;
	unsigned onenand_startaddr2 = DEVICE_BUFFERRAM_0 << 15;
	unsigned onenand_startbuffer = DATARAM0_0 << 8;
	unsigned onenand_sysconfig1 = (raw_mode == 1) ? ONENAND_SYSCFG1_ECCDIS :
	    ONENAND_SYSCFG1_ECCENA;

	unsigned controller_status;
	unsigned interrupt_status;
	unsigned ecc_status;

	char flash_oob[64];

	unsigned oobfree_offset[8] = { 2, 14, 18, 30, 34, 46, 50, 62 };
	unsigned oobfree_length[8] = { 3, 2, 3, 2, 3, 2, 3, 2 };

	for (i = 0; i < 64; i++)
		flash_oob[i] = 0xFF;

	data->sfbcfg = SFLASH_BCFG;
	data->sfcmd[0] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATWR);
	data->sfcmd[1] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATWR);
	data->sfcmd[2] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATWR);
	data->sfcmd[3] = SFLASH_PREPCMD(256, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATWR);
	data->sfcmd[4] = SFLASH_PREPCMD(32, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_DATWR);
	data->sfcmd[5] = SFLASH_PREPCMD(7, 0, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGWR);
	data->sfcmd[6] = SFLASH_PREPCMD(0, 0, 32,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_INTHI);
	data->sfcmd[7] = SFLASH_PREPCMD(3, 7, 0,
					NAND_SFCMD_DATXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGRD);
	data->sfcmd[8] = SFLASH_PREPCMD(4, 10, 0,
					NAND_SFCMD_CMDXS,
					NAND_SFCMD_ASYNC, NAND_SFCMD_REGWR);
	data->sfexec = 1;

	data->sfstat[0] = CLEAN_DATA_32;
	data->sfstat[1] = CLEAN_DATA_32;
	data->sfstat[2] = CLEAN_DATA_32;
	data->sfstat[3] = CLEAN_DATA_32;
	data->sfstat[4] = CLEAN_DATA_32;
	data->sfstat[5] = CLEAN_DATA_32;
	data->sfstat[6] = CLEAN_DATA_32;
	data->sfstat[7] = CLEAN_DATA_32;
	data->sfstat[8] = CLEAN_DATA_32;
	data->addr0 =
	    (ONENAND_INTERRUPT_STATUS << 16) | (ONENAND_SYSTEM_CONFIG_1);
	data->addr1 =
	    (ONENAND_START_ADDRESS_8 << 16) | (ONENAND_START_ADDRESS_1);
	data->addr2 = (ONENAND_START_BUFFER << 16) | (ONENAND_START_ADDRESS_2);
	data->addr3 = (ONENAND_ECC_STATUS << 16) | (ONENAND_COMMAND);
	data->addr4 = (ONENAND_CONTROLLER_STATUS << 16) |
	    (ONENAND_INTERRUPT_STATUS);
	data->addr5 =
	    (ONENAND_INTERRUPT_STATUS << 16) | (ONENAND_SYSTEM_CONFIG_1);
	data->addr6 =
	    (ONENAND_START_ADDRESS_3 << 16) | (ONENAND_START_ADDRESS_1);
	data->data0 = (ONENAND_CLRINTR << 16) | (onenand_sysconfig1);
	data->data1 = (onenand_startaddr8 << 16) | (onenand_startaddr1);
	data->data2 = (onenand_startbuffer << 16) | (onenand_startaddr2);
	data->data3 = (CLEAN_DATA_16 << 16) | (ONENAND_CMDPROGSPARE);
	data->data3 = (CLEAN_DATA_16 << 16) | (ONENAND_CMDPROGSPARE);
	data->data4 = (CLEAN_DATA_16 << 16) | (CLEAN_DATA_16);
	data->data5 = (ONENAND_CLRINTR << 16) | (ONENAND_SYSCFG1_ECCENA);
	data->data6 = (ONENAND_STARTADDR3_RES << 16) | (ONENAND_STARTADDR1_RES);
	data->macro[0] = 0x0200;
	data->macro[1] = 0x0300;
	data->macro[2] = 0x0400;
	data->macro[3] = 0x0500;
	data->macro[4] = 0x8010;

	/*************************************************************/
	/* Write the data ram area in the onenand buffer ram             */
	/*************************************************************/

	/* Enable and configure the SFlash controller */
	cmd->cmd = 0 | CMD_OCB;
	cmd->src = paddr(&data->sfbcfg);
	cmd->dst = NAND_SFLASHC_BURST_CFG;
	cmd->len = 4;
	cmd++;

	if (addr) {
		data->data3 = (CLEAN_DATA_16 << 16) | (ONENAND_CMDPROG);

		for (i = 0; i < 4; i++) {

			/* Block on cmd ready and write CMD register */
			cmd->cmd = DST_CRCI_NAND_CMD;
			cmd->src = paddr(&data->sfcmd[i]);
			cmd->dst = NAND_SFLASHC_CMD;
			cmd->len = 4;
			cmd++;

			/* Trnsfr usr buf contents to nand ctlr buf */
			cmd->cmd = 0;
			cmd->src = paddr(addr_curr);
			cmd->dst = NAND_FLASH_BUFFER;
			cmd->len = 512;
			if (!raw_mode)
				addr_curr += 512;
			cmd++;

			/* Write the MACRO1 register */
			cmd->cmd = 0;
			cmd->src = paddr(&data->macro[i]);
			cmd->dst = NAND_MACRO1_REG;
			cmd->len = 4;
			cmd++;

			/* Kick the execute command */
			cmd->cmd = 0;
			cmd->src = paddr(&data->sfexec);
			cmd->dst = NAND_SFLASHC_EXEC_CMD;
			cmd->len = 4;
			cmd++;

			/* Block on data rdy, & read status register */
			cmd->cmd = SRC_CRCI_NAND_DATA;
			cmd->src = NAND_SFLASHC_STATUS;
			cmd->dst = paddr(&data->sfstat[i]);
			cmd->len = 4;
			cmd++;

		}
	}

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[4]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	if (spareaddr) {
		// Auto mode
		for (i = 0, k = 0; i < 8; i++) {
			for (j = 0; j < oobfree_length[i]; j++) {
				flash_oob[j + oobfree_offset[i]] = spareaddr[k];
				k++;
			}
		}

		cmd->cmd = 0;
		cmd->src = paddr(&flash_oob);
		cmd->dst = NAND_FLASH_BUFFER;
		cmd->len = 64;
		cmd++;
	}

	if (raw_mode) {
		cmd->cmd = 0;
		cmd->src = paddr(addr_curr);
		cmd->dst = NAND_FLASH_BUFFER;
		cmd->len = 64;
		cmd++;
	}

	/* Write the MACRO1 register */
	cmd->cmd = 0;
	cmd->src = paddr(&data->macro[4]);
	cmd->dst = NAND_MACRO1_REG;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[4]);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Write necessary address registers in the onenand device      */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[5]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Write the ADDR0 and ADDR1 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr0);
	cmd->dst = NAND_ADDR0;
	cmd->len = 8;
	cmd++;

	/* Write the ADDR2 ADDR3 ADDR4 ADDR5 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr2);
	cmd->dst = NAND_ADDR2;
	cmd->len = 16;
	cmd++;

	/* Write the ADDR6 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->addr6);
	cmd->dst = NAND_ADDR6;
	cmd->len = 4;
	cmd++;

	/* Write the GENP0, GENP1, GENP2, GENP3 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->data0);
	cmd->dst = NAND_GENP_REG0;
	cmd->len = 16;
	cmd++;

	/* Write the FLASH_DEV_CMD4,5,6 registers */
	cmd->cmd = 0;
	cmd->src = paddr(&data->data4);
	cmd->dst = NAND_DEV_CMD4;
	cmd->len = 12;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[5]);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Wait for the interrupt from the Onenand device controller */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[6]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[6]);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Read necessary status registers from the onenand device      */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[7]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[7]);
	cmd->len = 4;
	cmd++;

	/* Read the GENP3 register */
	cmd->cmd = 0;
	cmd->src = NAND_GENP_REG3;
	cmd->dst = paddr(&data->data3);
	cmd->len = 4;
	cmd++;

	/* Read the DEVCMD4 register */
	cmd->cmd = 0;
	cmd->src = NAND_DEV_CMD4;
	cmd->dst = paddr(&data->data4);
	cmd->len = 4;
	cmd++;

	/*************************************************************/
	/* Restore the necessary registers to proper values               */
	/*************************************************************/

	/* Block on cmd ready and write CMD register */
	cmd->cmd = DST_CRCI_NAND_CMD;
	cmd->src = paddr(&data->sfcmd[8]);
	cmd->dst = NAND_SFLASHC_CMD;
	cmd->len = 4;
	cmd++;

	/* Kick the execute command */
	cmd->cmd = 0;
	cmd->src = paddr(&data->sfexec);
	cmd->dst = NAND_SFLASHC_EXEC_CMD;
	cmd->len = 4;
	cmd++;

	/* Block on data ready, and read the status register */
	cmd->cmd = SRC_CRCI_NAND_DATA | CMD_OCU | CMD_LC;
	cmd->src = NAND_SFLASHC_STATUS;
	cmd->dst = paddr(&data->sfstat[8]);
	cmd->len = 4;
	cmd++;

	ptr[0] = (paddr(cmdlist) >> 3) | CMD_PTR_LP;

	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);

	ecc_status = (data->data3 >> 16) & 0x0000FFFF;
	interrupt_status = (data->data4 >> 0) & 0x0000FFFF;
	controller_status = (data->data4 >> 16) & 0x0000FFFF;

#if VERBOSE
	dprintf(INFO, "\n%s: sflash status %x %x %x %x %x %x %x %x %x\n",
		__func__, data->sfstat[0], data->sfstat[1], data->sfstat[2],
		data->sfstat[3], data->sfstat[4], data->sfstat[5],
		data->sfstat[6], data->sfstat[7], data->sfstat[8]);

	dprintf(INFO, "%s: controller_status = %x\n", __func__,
		controller_status);
	dprintf(INFO, "%s: interrupt_status = %x\n", __func__,
		interrupt_status);
	dprintf(INFO, "%s: ecc_status = %x\n", __func__, ecc_status);
#endif
	/* Check for errors, protection violations etc */
	if ((controller_status != 0)
	    || (data->sfstat[5] & 0x110)
	    || (data->sfstat[6] & 0x110)
	    || (data->sfstat[7] & 0x110)
	    || (data->sfstat[8] & 0x110)
	    || ((data->sfstat[0] & 0x110) && (addr))
	    || ((data->sfstat[1] & 0x110) && (addr))
	    || ((data->sfstat[2] & 0x110) &&
		(addr)) || ((data->sfstat[3] & 0x110) && (addr))) {
		dprintf(CRITICAL, "%s: ECC/MPU/OP error\n", __func__);
		return -1;
	}

	return 0;
}

static int
flash_onenand_mark_badblock(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	memset(empty_buf, 0, 528);
	/* Going to first page of the block */
	if (page & num_pages_per_blk_mask)
		page = page - (page & num_pages_per_blk_mask);
	return _flash_onenand_write_page(cmdlist, ptrlist, page, empty_buf, 0,
					 1);
}

static int
flash_mark_badblock(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	switch (flash_info.type) {
	case FLASH_8BIT_NAND_DEVICE:
	case FLASH_16BIT_NAND_DEVICE:
		return flash_nand_mark_badblock(cmdlist, ptrlist, page);
	case FLASH_ONENAND_DEVICE:
		return flash_onenand_mark_badblock(cmdlist, ptrlist, page);
	default:
		return -1;
	}
}

unsigned flash_ctrl_hwinfo(dmov_s * cmdlist, unsigned *ptrlist)
{
	dmov_s *cmd = cmdlist;
	unsigned *ptr = ptrlist;
	unsigned *data = ptrlist + 4;

	unsigned rv;

	data[0] = 0xeeeeeeee;

	cmd[0].cmd = CMD_LC | CMD_OCB | CMD_OCU;
	cmd[0].src = NAND_HW_INFO;
	cmd[0].dst = paddr(&data[0]);
	cmd[0].len = 4;

	ptr[0] = (paddr(cmd) >> 3) | CMD_PTR_LP;
	dmov_exec_cmdptr(DMOV_NAND_CHAN, ptr);
	rv = data[0];

	return rv;
}

/* Wrapper functions */
static void flash_read_id(dmov_s * cmdlist, unsigned *ptrlist)
{
	int dev_found = 0;
	unsigned index;

	// Try to read id
	flash_nand_read_id(cmdlist, ptrlist);
	// Check if we support the device
	for (index = 1;
	     index <
	     (sizeof(supported_flash) / sizeof(struct flash_identification));
	     index++) {
		if ((flash_info.id & supported_flash[index].mask) ==
		    (supported_flash[index].
		     flash_id & (supported_flash[index].mask))) {
			dev_found = 1;
			break;
		}
	}

	if (!dev_found) {
		flash_onenand_read_id(cmdlist, ptrlist);
		for (index = 1;
		     index <
		     (sizeof(supported_flash) /
		      sizeof(struct flash_identification)); index++) {
			if ((flash_info.id & supported_flash[index].mask) ==
			    (supported_flash[index].
			     flash_id & (supported_flash[index].mask))) {
				dev_found = 1;
				break;
			}
		}
	}

	if (dev_found) {
		if (supported_flash[index].widebus)
			flash_info.type = FLASH_16BIT_NAND_DEVICE;
		else
			flash_info.type = FLASH_8BIT_NAND_DEVICE;
		if (supported_flash[index].onenand)
			flash_info.type = FLASH_ONENAND_DEVICE;
		flash_info.page_size = supported_flash[index].pagesize;
		flash_pagesize = flash_info.page_size;
		flash_info.block_size = supported_flash[index].blksize;
		flash_info.spare_size = supported_flash[index].oobsize;
		if (flash_info.block_size && flash_info.page_size) {
			flash_info.num_blocks = supported_flash[index].density;
			flash_info.num_blocks /= (flash_info.block_size);
		} else {
			flash_info.num_blocks = 0;
		}
		ASSERT(flash_info.num_blocks);
		// Use this for getting the next/current blocks
		num_pages_per_blk = flash_info.block_size / flash_pagesize;
		num_pages_per_blk_mask = num_pages_per_blk - 1;
		//Look for 8bit BCH ECC Nand, TODO: ECC Correctability >= 8
		if ((flash_ctrl_hwinfo(cmdlist, ptrlist) == 0x307)
		    && flash_info.id == 0x2600482c) {
			enable_bch_ecc = 1;
		}
		return;
	}
	// Flash device is not supported, print flash device info and halt
	if (dev_found == 0) {
		dprintf(CRITICAL, "NAND device is not supported: nandid: 0x%x \
			maker=0x%02x device=0x%02x\n", flash_info.id,
			flash_info.vendor, flash_info.device);
		ASSERT(0);
	}
	dprintf(INFO, "nandid: 0x%x maker=0x%02x device=0x%02x page_size=%d\n",
		flash_info.id, flash_info.vendor, flash_info.device,
		flash_info.page_size);
	dprintf(INFO, "		spare_size=%d block_size=%d num_blocks=%d\n",
		flash_info.spare_size, flash_info.block_size,
		flash_info.num_blocks);
}

static int flash_erase_block(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	switch (flash_info.type) {
	case FLASH_8BIT_NAND_DEVICE:
	case FLASH_16BIT_NAND_DEVICE:
		return flash_nand_erase_block(cmdlist, ptrlist, page);
	case FLASH_ONENAND_DEVICE:
		return flash_onenand_erase_block(cmdlist, ptrlist, page);
	default:
		return -1;
	}
}

static int
_flash_read_page(dmov_s * cmdlist, unsigned *ptrlist,
		 unsigned page, void *_addr, void *_spareaddr)
{
	switch (flash_info.type) {
	case FLASH_8BIT_NAND_DEVICE:
	case FLASH_16BIT_NAND_DEVICE:
		if (interleaved_mode)
			return flash_nand_read_page_interleave(cmdlist, ptrlist,
							       page, _addr,
							       _spareaddr);
		else
			return _flash_nand_read_page(cmdlist, ptrlist, page,
						     _addr, _spareaddr);
	case FLASH_ONENAND_DEVICE:
		return _flash_onenand_read_page(cmdlist, ptrlist, page, _addr,
						_spareaddr, 0);
	default:
		return -1;
	}
}

static int
_flash_block_isbad(dmov_s * cmdlist, unsigned *ptrlist, unsigned page)
{
	switch (flash_info.type) {
	case FLASH_8BIT_NAND_DEVICE:
	case FLASH_16BIT_NAND_DEVICE:
		return flash_nand_block_isbad(cmdlist, ptrlist, page);
	case FLASH_ONENAND_DEVICE:
		return flash_onenand_block_isbad(cmdlist, ptrlist, page);
	default:
		return -1;
	}
}

static int
_flash_write_page(dmov_s * cmdlist, unsigned *ptrlist,
		  unsigned page, const void *_addr, const void *_spareaddr)
{
	switch (flash_info.type) {
	case FLASH_8BIT_NAND_DEVICE:
	case FLASH_16BIT_NAND_DEVICE:
		if (interleaved_mode)
			return flash_nand_write_page_interleave(cmdlist,
								ptrlist, page,
								_addr,
								_spareaddr, 0);
		else
			return _flash_nand_write_page(cmdlist, ptrlist, page,
						      _addr, _spareaddr, 0);
	case FLASH_ONENAND_DEVICE:
		return _flash_onenand_write_page(cmdlist, ptrlist, page, _addr,
						 _spareaddr, 0);
	default:
		return -1;
	}
}

static unsigned *flash_ptrlist;
static dmov_s *flash_cmdlist;

static struct ptable *flash_ptable = NULL;

void flash_init(void)
{
	int i = 0;
	ASSERT(flash_ptable == NULL);

	flash_ptrlist = memalign(32, 1024);
	flash_cmdlist = memalign(32, 1024);
	flash_data = memalign(32, 4096 + 128);
	flash_spare = memalign(32, 128);

	flash_read_id(flash_cmdlist, flash_ptrlist);
	if ((FLASH_8BIT_NAND_DEVICE == flash_info.type)
	    || (FLASH_16BIT_NAND_DEVICE == flash_info.type)) {
		if (flash_nand_read_config(flash_cmdlist, flash_ptrlist)) {
			dprintf(CRITICAL,
				"ERROR: could not read CFG0/CFG1 state\n");
			ASSERT(0);
		}
	}
	/* Create a bad block table */
	bbtbl =
	    (unsigned int *)malloc(sizeof(unsigned int) *
				   flash_info.num_blocks);
	for (i = 0; i < flash_info.num_blocks; i++)
		bbtbl[i] = -1;
}

struct ptable *flash_get_ptable(void)
{
	return flash_ptable;
}

void flash_set_ptable(struct ptable *new_ptable)
{
	ASSERT(flash_ptable == NULL && new_ptable != NULL);
	flash_ptable = new_ptable;
}

struct flash_info *flash_get_info(void)
{
	return &flash_info;
}

int flash_erase(struct ptentry *ptn)
{
	unsigned block = ptn->start;
	unsigned count = ptn->length;

	set_nand_configuration(ptn->type);
	while (count-- > 0) {
		if (flash_erase_block
		    (flash_cmdlist, flash_ptrlist, block * num_pages_per_blk)) {
			dprintf(INFO, "cannot erase @ %d (bad block?)\n",
				block);
		}
		block++;
	}
	return 0;
}

int
flash_read_ext(struct ptentry *ptn, unsigned extra_per_page,
	       unsigned offset, void *data, unsigned bytes)
{
	unsigned page =
	    (ptn->start * num_pages_per_blk) + (offset / flash_pagesize);
	unsigned lastpage = (ptn->start + ptn->length) * num_pages_per_blk;
	unsigned count =
	    (bytes + flash_pagesize - 1 + extra_per_page) / (flash_pagesize +
							     extra_per_page);
	unsigned *spare = (unsigned *)flash_spare;
	unsigned errors = 0;
	unsigned char *image = data;
	unsigned current_block =
	    (page - (page & num_pages_per_blk_mask)) / num_pages_per_blk;
	unsigned start_block = ptn->start;
	int result = 0;
	int isbad = 0;
	int start_block_count = 0;

	set_nand_configuration(TYPE_APPS_PARTITION);

	if (offset & (flash_pagesize - 1))
		return -1;

// Adjust page offset based on number of bad blocks from start to current page
	if (start_block < current_block) {
		start_block_count = (current_block - start_block);
		while (start_block_count
		       && (start_block < (ptn->start + ptn->length))) {
			isbad =
			    _flash_block_isbad(flash_cmdlist, flash_ptrlist,
					       start_block * num_pages_per_blk);
			if (isbad)
				page += num_pages_per_blk;
			else
				start_block_count--;
			start_block++;
		}
	}

	while ((page < lastpage) && !start_block_count) {
		if (count == 0) {
			dprintf(INFO, "flash_read_image: success (%d errors)\n",
				errors);
			return 0;
		}

		result =
		    _flash_read_page(flash_cmdlist, flash_ptrlist, page, image,
				     spare);

		if (result == -1) {
			// bad page, go to next page
			page++;
			errors++;
			continue;
		} else if (result == -2) {
			// bad block, go to next block same offset
			page += num_pages_per_blk;
			errors++;
			continue;
		}

		page++;
		image += flash_pagesize;
		memcpy(image, spare, extra_per_page);
		image += extra_per_page;
		count -= 1;
	}

	/* could not find enough valid pages before we hit the end */
	dprintf(INFO, "flash_read_image: failed (%d errors)\n", errors);
	return 0xffffffff;
}

int
flash_write(struct ptentry *ptn, unsigned extra_per_page, const void *data,
	    unsigned bytes)
{
	unsigned page = ptn->start * num_pages_per_blk;
	unsigned lastpage = (ptn->start + ptn->length) * num_pages_per_blk;
	unsigned *spare = (unsigned *)flash_spare;
	const unsigned char *image = data;
	unsigned wsize = flash_pagesize + extra_per_page;
	unsigned n;
	int r;

	if ((flash_info.type == FLASH_ONENAND_DEVICE)
	    && (ptn->type == TYPE_MODEM_PARTITION)) {
		dprintf(CRITICAL, "flash_write_image: feature not supported\n");
		return -1;
	}

	set_nand_configuration(ptn->type);
	for (n = 0; n < 16; n++)
		spare[n] = 0xffffffff;

	while (bytes > 0) {
		if (bytes < wsize) {
			dprintf(CRITICAL,
				"flash_write_image: image undersized (%d < %d)\n",
				bytes, wsize);
			return -1;
		}
		if (page >= lastpage) {
			dprintf(CRITICAL, "flash_write_image: out of space\n");
			return -1;
		}

		if ((page & num_pages_per_blk_mask) == 0) {
			if (flash_erase_block
			    (flash_cmdlist, flash_ptrlist, page)) {
				dprintf(INFO,
					"flash_write_image: bad block @ %d\n",
					page / num_pages_per_blk);
				page += num_pages_per_blk;
				continue;
			}
		}

		if (extra_per_page) {
			r = _flash_write_page(flash_cmdlist, flash_ptrlist,
					      page, image,
					      image + flash_pagesize);
		} else {
			r = _flash_write_page(flash_cmdlist, flash_ptrlist,
					      page, image, spare);
		}
		if (r) {
			dprintf(INFO,
				"flash_write_image: write failure @ page %d (src %d)\n",
				page, image - (const unsigned char *)data);
			image -= (page & num_pages_per_blk_mask) * wsize;
			bytes += (page & num_pages_per_blk_mask) * wsize;
			page &= ~num_pages_per_blk_mask;
			if (flash_erase_block
			    (flash_cmdlist, flash_ptrlist, page)) {
				dprintf(INFO,
					"flash_write_image: erase failure @ page %d\n",
					page);
			}
			if (ptn->type != TYPE_MODEM_PARTITION) {
				flash_mark_badblock(flash_cmdlist,
						    flash_ptrlist, page);
			}
			dprintf(INFO,
				"flash_write_image: restart write @ page %d (src %d)\n",
				page, image - (const unsigned char *)data);
			page += num_pages_per_blk;
			continue;
		}
		page++;
		image += wsize;
		bytes -= wsize;
	}

	/* erase any remaining pages in the partition */
	page = (page + num_pages_per_blk_mask) & (~num_pages_per_blk_mask);
	while (page < lastpage) {
		if (flash_erase_block(flash_cmdlist, flash_ptrlist, page)) {
			dprintf(INFO, "flash_write_image: bad block @ %d\n",
				page / num_pages_per_blk);
		}
		page += num_pages_per_blk;
	}

	dprintf(INFO, "flash_write_image: success\n");
	return 0;
}

#if 0
static int flash_read_page(unsigned page, void *data, void *extra)
{
	return _flash_read_page(flash_cmdlist, flash_ptrlist, page, data,
				extra);
}
#endif

unsigned flash_page_size(void)
{
	return flash_pagesize;
}

void enable_interleave_mode(int status)
{
	interleaved_mode = status;
	if (status) {
		flash_pagesize *= 2;
		platform_config_interleaved_mode_gpios();
	}
	return;
}

int flash_ecc_bch_enabled()
{
	return enable_bch_ecc;
}
