/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
#include <dev/keys.h>
#include <dev/gpio_keypad.h>
#include <lib/ptable.h>
#include <dev/flash.h>
#include <smem.h>
#include <platform/iomap.h>
#include <reg.h>

#define LINUX_MACHTYPE_8660_SURF    1009002
#define LINUX_MACHTYPE_8660_FFA     1009003
#define LINUX_MACHTYPE_8660_FLUID   1009004
#define LINUX_MACHTYPE_8660_CHARM_SURF   3181
#define LINUX_MACHTYPE_8660_CHARM_FFA    3199

void keypad_init(void);

static int emmc_boot = -1;  /* set to uninitialized */
int target_is_emmc_boot(void);
void debug_led_write(char);
char debug_led_read();
uint32_t platform_id_read (void);

void target_init(void)
{

    dprintf(INFO, "target_init()\n");

    setup_fpga();

    /* Setting Debug LEDs ON */
    debug_led_write(0xFF);
#if (!ENABLE_NANDWRITE)
	keys_init();
	keypad_init();
#endif
    if(mmc_boot_main(MMC_SLOT,MSM_SDC1_BASE))
    {
        dprintf(CRITICAL, "mmc init failed!");
        ASSERT(0);
    }
}

unsigned board_machtype(void)
{
	struct smem_board_info_v5 board_info_v5;
	unsigned int board_info_len = 0;
	unsigned smem_status = 0;
	unsigned format = 0;
	unsigned id = 0;
	unsigned hw_platform = 0;
	unsigned fused_chip = 0;
	unsigned mach_id = LINUX_MACHTYPE_8660_FFA;

	/* Detect external msm if this is a "fusion" */
	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
					&format, sizeof(format), 0);
	if(!smem_status)
	{
		if (format >= 5)
		{
			board_info_len = sizeof(board_info_v5);

			smem_status = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
							&board_info_v5, board_info_len);
			if(!smem_status)
			{
				fused_chip = board_info_v5.fused_chip;
			}
		}
	}

	/* Detect SURF v/s FFA v/s Fluid */
	id = board_info_v5.board_info_v3.hw_platform;
	switch(id)
	{
		case 0x1:
			hw_platform = HW_PLATFORM_SURF;
			break;
		case 0x2:
			hw_platform = HW_PLATFORM_FFA;
			break;
		case 0x3:
			hw_platform = HW_PLATFORM_FLUID;
			break;
		default:
			/* Writing to Debug LED register and reading back to auto detect
			SURF and FFA. If we read back, it is SURF */
			debug_led_write(0xA5);

			if((debug_led_read() & 0xFF) == 0xA5)
			{
				debug_led_write(0);
				hw_platform = HW_PLATFORM_SURF;
			}
			else
				hw_platform = HW_PLATFORM_FFA;
	};

	/* Use hw_platform and fused_chip information to determine machine id */
	switch(fused_chip)
	{
		case UNKNOWN:
			if (hw_platform == HW_PLATFORM_SURF)
				mach_id = LINUX_MACHTYPE_8660_SURF;
			else if (hw_platform == HW_PLATFORM_FFA)
				mach_id = LINUX_MACHTYPE_8660_FFA;
			else if (hw_platform == HW_PLATFORM_FLUID)
				mach_id = LINUX_MACHTYPE_8660_FLUID;
			break;

		case MDM9200:
		case MDM9600:
			if (hw_platform == HW_PLATFORM_SURF)
				mach_id = LINUX_MACHTYPE_8660_CHARM_SURF;
			else if (hw_platform == HW_PLATFORM_FFA)
				mach_id = LINUX_MACHTYPE_8660_CHARM_FFA;
			break;

		default:
			mach_id = LINUX_MACHTYPE_8660_FFA;
	}

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
	void *restart_reason_addr = 0x401FFFFC;

	/* Read reboot reason and scrub it */
	restart_reason = readl(restart_reason_addr);
	writel(0x00, restart_reason_addr);

	return restart_reason;
}

void target_battery_charging_enable(unsigned enable, unsigned disconnect)
{
}

void setup_fpga()
{
    writel(0x147, GPIO_CFG133_ADDR);
    writel(0x144, GPIO_CFG135_ADDR);
    writel(0x144, GPIO_CFG136_ADDR);
    writel(0x144, GPIO_CFG137_ADDR);
    writel(0x144, GPIO_CFG138_ADDR);
    writel(0x144, GPIO_CFG139_ADDR);
    writel(0x144, GPIO_CFG140_ADDR);
    writel(0x144, GPIO_CFG141_ADDR);
    writel(0x144, GPIO_CFG142_ADDR);
    writel(0x144, GPIO_CFG143_ADDR);
    writel(0x144, GPIO_CFG144_ADDR);
    writel(0x144, GPIO_CFG145_ADDR);
    writel(0x144, GPIO_CFG146_ADDR);
    writel(0x144, GPIO_CFG147_ADDR);
    writel(0x144, GPIO_CFG148_ADDR);
    writel(0x144, GPIO_CFG149_ADDR);
    writel(0x144, GPIO_CFG150_ADDR);
    writel(0x147, GPIO_CFG151_ADDR);
    writel(0x147, GPIO_CFG152_ADDR);
    writel(0x147, GPIO_CFG153_ADDR);
    writel(0x3,   GPIO_CFG154_ADDR);
    writel(0x147, GPIO_CFG155_ADDR);
    writel(0x147, GPIO_CFG156_ADDR);
    writel(0x147, GPIO_CFG157_ADDR);
    writel(0x3,   GPIO_CFG158_ADDR);

    writel(0x00000B31, EBI2_CHIP_SELECT_CFG0);
    writel(0xA3030020, EBI2_XMEM_CS3_CFG1);
}

void debug_led_write(char val)
{
    writeb(val,SURF_DEBUG_LED_ADDR);
}

char debug_led_read()
{
    return readb(SURF_DEBUG_LED_ADDR);
}
