/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#include <reg.h>
#include <debug.h>
#include <smem.h>
#include <uart_dm.h>
#include <gsbi.h>
#include <baseband.h>
#include <dev/keys.h>
#include <dev/pm8921.h>
#include <dev/ssbi.h>
#include <platform/iomap.h>
#include <lib/ptable.h>

#define LINUX_MACHTYPE_APQ8064_SIM     3572

extern unsigned int mmc_boot_main(unsigned char slot, unsigned int base);
extern void mdelay(unsigned msecs);
extern void keypad_init(void);

static unsigned mmc_sdc_base[] =
{
	MSM_SDC1_BASE,
	MSM_SDC2_BASE,
	MSM_SDC3_BASE,
	MSM_SDC4_BASE
};

static pm8921_dev_t pmic;
static const uint8_t uart_gsbi_id  = GSBI_ID_3;

void target_init(void)
{
	uint32_t base_addr;
	uint8_t slot;
	dprintf(INFO, "target_init()\n");

	/* Initialize PMIC driver */
	pmic.read  = pa1_ssbi2_read_bytes;
	pmic.write = pa1_ssbi2_write_bytes;

	pm8921_init(&pmic);

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

uint32_t board_machtype(void)
{
	struct smem_board_info_v6 board_info_v6;
	uint32_t board_info_len = 0;
	uint32_t smem_status = 0;
	uint32_t format = 0;
	uint32_t id = HW_PLATFORM_UNKNOWN;
	uint32_t mach_id;


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
		/* APQ8064 machine id not yet defined for CDP etc.
		 * default to simulator.
		 */
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_FFA:
		case HW_PLATFORM_FLUID:
		default:
			mach_id = LINUX_MACHTYPE_APQ8064_SIM;
	};

	return mach_id;
}

void reboot_device(uint32_t reboot_reason)
{
	/* TODO: Not implemented yet. */
	ASSERT(0);
}

uint32_t check_reboot_mode(void)
{
	uint32_t restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);
	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

void target_serialno(unsigned char *buf)
{
	uint32_t serialno;
	if(target_is_emmc_boot())
	{
		serialno =  mmc_get_psn();
		sprintf(buf,"%x",serialno);
	}
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	/* Set the BOOT_DONE flag in PM8921 */
	pm8921_boot_done();
}

/* GSBI to be used for UART */
uint8_t target_uart_gsbi(void)
{
	return uart_gsbi_id;
}

/* Returns type of baseband. APQ for APPs only proc. */
unsigned target_baseband()
{
	return BASEBAND_APQ;
}
