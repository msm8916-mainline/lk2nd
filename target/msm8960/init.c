/*
 * Copyright (c) 2009, Google Inc.
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
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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
#include <lib/ptable.h>
#include <smem.h>
#include <platform/iomap.h>
#include <reg.h>
#include <dev/keys.h>

#define LINUX_MACHTYPE_8960_SIM     3230
#define LINUX_MACHTYPE_8960_RUMI3   3231
#define LINUX_MACHTYPE_8960_CDP     3396
#define LINUX_MACHTYPE_8960_MTP     3397
#define LINUX_MACHTYPE_8960_FLUID   3398
#define LINUX_MACHTYPE_8960_APQ     3399

extern unsigned int mmc_boot_main(unsigned char slot, unsigned int base);
extern void mdelay(unsigned msecs);
extern void dmb(void);
extern void keypad_init(void);

static unsigned mmc_sdc_base[] = { MSM_SDC1_BASE, MSM_SDC2_BASE, MSM_SDC3_BASE, MSM_SDC4_BASE};

void target_init(void)
{
	unsigned base_addr;
	unsigned char slot;
	dprintf(INFO, "target_init()\n");

	/* Keypad init */
	keys_init();
	keypad_init();

	/* Trying Slot 1 first */
	slot = 1;
	base_addr = mmc_sdc_base[slot-1];
	if(mmc_boot_main(slot, base_addr))
	{
		/* Trying Slot 3 next */
		slot = 3;
		base_addr = mmc_sdc_base[slot-1];
		if(mmc_boot_main(slot, base_addr))
		{
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

unsigned board_machtype(void)
{
	struct smem_board_info_v6 board_info_v6;
	unsigned int board_info_len = 0;
	unsigned smem_status = 0;
	unsigned format = 0;
	unsigned id = HW_PLATFORM_UNKNOWN;
	unsigned mach_id;


	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
					&format, sizeof(format), 0);
	if(!smem_status)
	{
		if (format == 6)
		{
			board_info_len = sizeof(board_info_v6);

			smem_status = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
							&board_info_v6, board_info_len);
			if(!smem_status)
			{
				id = board_info_v6.board_info_v3.hw_platform;
			}
		}
	}

	/* Detect the board we are running on */
	switch(id)
	{
		case HW_PLATFORM_SURF:
			mach_id = LINUX_MACHTYPE_8960_CDP;
			break;
		case HW_PLATFORM_FFA:
			mach_id = LINUX_MACHTYPE_8960_MTP;
			break;
		case HW_PLATFORM_FLUID:
			mach_id = LINUX_MACHTYPE_8960_FLUID;
			break;
		default:
			mach_id = LINUX_MACHTYPE_8960_CDP;
	};

	return mach_id;
}

void reboot_device(unsigned reboot_reason)
{
	/* Reset WDG0 counter */
	writel(1,MSM_WDT0_RST);
	/* Disable WDG0 */
	writel(0,MSM_WDT0_EN);
	/* Set WDG0 bark time */
	writel(0x31F3,MSM_WDT0_BT);
	/* Enable WDG0 */
	writel(3,MSM_WDT0_EN);
	dmb();
	/* Enable WDG output */
	writel(3,MSM_TCSR_BASE + TCSR_WDOG_CFG);
	mdelay(10000);
	dprintf (CRITICAL, "Rebooting failed\n");
	return;
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;
	void *restart_reason_addr = (void*)0x401FFFFC;

	/* Read reboot reason and scrub it */
	restart_reason = readl(restart_reason_addr);
	writel(0x00, restart_reason_addr);

	return restart_reason;
}

void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	if(target_is_emmc_boot())
	{
		serialno =  mmc_get_psn();
		sprintf(buf,"%x",serialno);
	}
}

void target_battery_charging_enable(unsigned enable, unsigned disconnect)
{
}
