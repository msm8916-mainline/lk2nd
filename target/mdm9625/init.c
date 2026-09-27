/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <board.h>
#include <platform.h>
#include <target.h>
#include <smem.h>
#include <baseband.h>
#include <lib/ptable.h>
#include <qpic_nand.h>
#include <ctype.h>
#include <string.h>
#include <pm8x41.h>
#include <reg.h>
#include <platform/timer.h>

extern void smem_ptable_init(void);
extern void smem_add_modem_partitions(struct ptable *flash_ptable);

static struct ptable flash_ptable;

/* PMIC config data */
#define PMIC_ARB_CHANNEL_NUM                          0
#define PMIC_ARB_OWNER_ID                             0

/* NANDc BAM pipe numbers */
#define DATA_CONSUMER_PIPE                            0
#define DATA_PRODUCER_PIPE                            1
#define CMD_PIPE                                      2

/* NANDc BAM pipe groups */
#define DATA_PRODUCER_PIPE_GRP                        0
#define DATA_CONSUMER_PIPE_GRP                        0
#define CMD_PIPE_GRP                                  1

/* NANDc EE */
#define QPIC_NAND_EE                                  0

/* NANDc max desc length. */
#define QPIC_NAND_MAX_DESC_LEN                        0x7FFF

#define LAST_NAND_PTN_LEN_PATTERN                     0xFFFFFFFF

struct qpic_nand_init_config config;

void update_ptable_names(void)
{
	uint32_t ptn_index;
	struct ptentry *ptentry_ptr = flash_ptable.parts;
	struct ptentry *boot_ptn;
	unsigned i;
	uint32_t len;

	/* Change all names to lower case. */
	for (ptn_index = 0; ptn_index != (uint32_t)flash_ptable.count; ptn_index++)
	{
		len = strlen(ptentry_ptr[ptn_index].name);

		for (i = 0; i < len; i++)
		{
			if (isupper(ptentry_ptr[ptn_index].name[i]))
			{
				ptentry_ptr[ptn_index].name[i] = tolower(ptentry_ptr[ptn_index].name[i]);
			}
		}

		/* SBL fills in the last partition length as 0xFFFFFFFF.
		 * Update the length field based on the number of blocks on the flash.
		 */
		if ((uint32_t)(ptentry_ptr[ptn_index].length) == LAST_NAND_PTN_LEN_PATTERN)
		{
			ptentry_ptr[ptn_index].length = flash_num_blocks() - ptentry_ptr[ptn_index].start;
		}
	}

	/* Rename apps ptn to boot. */
	boot_ptn = ptable_find(&flash_ptable, "apps");
	strcpy(boot_ptn->name, "boot");

	/* Rename appsbl ptn to aboot. */
	boot_ptn = ptable_find(&flash_ptable, "appsbl");
	strcpy(boot_ptn->name, "aboot");
}

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(3, 0, MSM_UART2_BASE);
#endif
}


/* init */
void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	config.pipes.read_pipe = DATA_PRODUCER_PIPE;
	config.pipes.write_pipe = DATA_CONSUMER_PIPE;
	config.pipes.cmd_pipe = CMD_PIPE;

	config.pipes.read_pipe_grp = DATA_PRODUCER_PIPE_GRP;
	config.pipes.write_pipe_grp = DATA_CONSUMER_PIPE_GRP;
	config.pipes.cmd_pipe_grp = CMD_PIPE_GRP;

	config.bam_base = MSM_NAND_BAM_BASE;
	config.nand_base = MSM_NAND_BASE;
	config.ee = QPIC_NAND_EE;
	config.max_desc_len = QPIC_NAND_MAX_DESC_LEN;

	qpic_nand_init(&config);

	ptable_init(&flash_ptable);

	smem_ptable_init();

	smem_add_modem_partitions(&flash_ptable);

	update_ptable_names();

	flash_set_ptable(&flash_ptable);
}

/* reboot */
void reboot_device(unsigned reboot_reason)
{
	uint32_t version = board_soc_version();

	/* Write the reboot reason */
	if(version >= 0x20000)
		writel(reboot_reason, RESTART_REASON_ADDR_V2);
	else
		writel(reboot_reason, RESTART_REASON_ADDR);

	/* Configure PMIC for warm reset */
	/* PM 8019 v1 aligns with PM8941 v2.
	 * This call should be based on the pmic version
	 * when PM8019 v2 is available.
	 */
	pm8x41_v2_reset_configure(PON_PSHOLD_WARM_RESET);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
	return;
}

/* Identify the current target */
void target_detect(struct board_data *board)
{
	/* This property is filled as part of board.c */
}

unsigned board_machtype(void)
{
	return  board_target_id();
}

/* Identify the baseband being used */
void target_baseband_detect(struct board_data *board)
{
	/* Check for baseband variants. Default to MSM */
	if (board->platform_subtype == HW_PLATFORM_SUBTYPE_UNKNOWN)
	{
			board->baseband = BASEBAND_MSM;
	}
	else
	{
		dprintf(CRITICAL, "Could not identify baseband id (%d)\n",
				board->platform_subtype);
		ASSERT(0);
	}
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;
	uint32_t version = board_soc_version();

	/* Read reboot reason and scrub it */
	if(version >= 0x20000) {
		restart_reason = readl(RESTART_REASON_ADDR_V2);
		writel(0x00, RESTART_REASON_ADDR_V2);
	}
	else {
		restart_reason = readl(RESTART_REASON_ADDR);
		writel(0x00, RESTART_REASON_ADDR);
	}

	return restart_reason;
}

