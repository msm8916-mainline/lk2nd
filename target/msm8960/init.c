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

#define LINUX_MACHTYPE_8960_SIM     3230
#define LINUX_MACHTYPE_8960_RUMI3   3231

extern unsigned int mmc_boot_main(unsigned char slot, unsigned int base);
extern void mdelay(unsigned msecs);
extern void dmb(void);

void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	if(mmc_boot_main(MMC_SLOT, MSM_SDC1_BASE))
	{
		dprintf(CRITICAL, "mmc init failed!");
		ASSERT(0);
	}
}

unsigned board_machtype(void)
{
/* TODO: Add a run time mechanism to detect if we are running on RUMI3.
 * Until then, PLATFORM_MSM8960_RUMI3 can be defined as compile time
 * option for RUMI3.
 */

#if PLATFORM_MSM8960_RUMI3
	return LINUX_MACHTYPE_8960_RUMI3;
#else
	return LINUX_MACHTYPE_8960_SIM;
#endif
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

void target_battery_charging_enable(unsigned enable, unsigned disconnect)
{
}
