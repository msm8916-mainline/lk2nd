/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
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
#include <mmc.h>
#include <platform/timer.h>
#include <reg.h>
#include <dev/keys.h>
#include <dev/pm8921.h>
#include <dev/ssbi.h>
#include <gsbi.h>
#include <target.h>
#include <platform.h>
#include <baseband.h>

/* 8960 */
#define LINUX_MACHTYPE_8960_SIM     3230
#define LINUX_MACHTYPE_8960_RUMI3   3231
#define LINUX_MACHTYPE_8960_CDP     3396
#define LINUX_MACHTYPE_8960_MTP     3397
#define LINUX_MACHTYPE_8960_FLUID   3398
#define LINUX_MACHTYPE_8960_APQ     3399
#define LINUX_MACHTYPE_8960_LIQUID  3535

/* 8627 */
#define LINUX_MACHTYPE_8627_CDP     3861
#define LINUX_MACHTYPE_8627_MTP     3862

/* 8930 */
#define LINUX_MACHTYPE_8930_CDP     3727
#define LINUX_MACHTYPE_8930_MTP     3728
#define LINUX_MACHTYPE_8930_FLUID   3729

/* 8064 */
#define LINUX_MACHTYPE_8064_SIM     3572
#define LINUX_MACHTYPE_8064_RUMI3   3679
#define LINUX_MACHTYPE_8064_CDP     3948
#define LINUX_MACHTYPE_8064_MTP     3949
#define LINUX_MACHTYPE_8064_LIQUID  3951

extern void dmb(void);
extern void msm8960_keypad_init(void);
extern void msm8930_keypad_init(void);
extern void panel_backlight_on(void);

static unsigned mmc_sdc_base[] =
    { MSM_SDC1_BASE, MSM_SDC2_BASE, MSM_SDC3_BASE, MSM_SDC4_BASE };

static uint32_t platform_id;
static uint32_t target_id;

static pm8921_dev_t pmic;

static void target_detect(void);
static uint8_t get_uart_gsbi(void);

void target_early_init(void)
{
	target_detect();

#if WITH_DEBUG_UART
	uart_init(get_uart_gsbi());
#endif
}

void shutdown_device(void)
{
	dprintf(CRITICAL, "Shutdown system.\n");
	pm8921_config_reset_pwr_off(0);

	/* Actually reset the chip */
	writel(0, MSM_PSHOLD_CTL_SU);
	mdelay(5000);

	dprintf(CRITICAL, "Shutdown failed.\n");
}

void target_init(void)
{
	unsigned base_addr;
	unsigned char slot;

	dprintf(INFO, "target_init()\n");

	/* Initialize PMIC driver */
	pmic.read = (pm8921_read_func) & pa1_ssbi2_read_bytes;
	pmic.write = (pm8921_write_func) & pa1_ssbi2_write_bytes;

	pm8921_init(&pmic);

	/* Keypad init */
	keys_init();

	if(platform_id == MSM8960)
	{
		msm8960_keypad_init();
	}
	else if(platform_id == MSM8930)
	{
		msm8930_keypad_init();
	}
	else if(platform_id == APQ8064)
	{
		apq8064_keypad_init();
	}

	/* Display splash screen if enabled */
#if DISPLAY_SPLASH_SCREEN
	if ((platform_id == MSM8960) || (platform_id == MSM8660A)
	    || (platform_id == MSM8260A) || (platform_id == APQ8060A)
	    || (platform_id == MSM8230) || (platform_id == MSM8630)
	    || (platform_id == MSM8930) || (platform_id == APQ8030)
	    || (platform_id == MSM8227) || (platform_id == MSM8627)) {
		panel_backlight_on();
		display_init();
		dprintf(SPEW, "Diplay initialized\n");
		display_image_on_screen();
	}
#endif

	/* Trying Slot 1 first */
	slot = 1;
	base_addr = mmc_sdc_base[slot - 1];
	if (mmc_boot_main(slot, base_addr)) {
		/* Trying Slot 3 next */
		slot = 3;
		base_addr = mmc_sdc_base[slot - 1];
		if (mmc_boot_main(slot, base_addr)) {
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

unsigned board_machtype(void)
{
	return target_id;
}

void target_detect(void)
{
	struct smem_board_info_v6 board_info_v6;
	unsigned int board_info_len = 0;
	unsigned smem_status = 0;
	unsigned format = 0;
	unsigned id = HW_PLATFORM_UNKNOWN;

	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (!smem_status) {
		if (format == 6) {
			board_info_len = sizeof(board_info_v6);

			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v6,
						  board_info_len);
			if (!smem_status) {
				id = board_info_v6.board_info_v3.hw_platform;
			}
		}
	}

	platform_id = board_info_v6.board_info_v3.msm_id;

	/* Detect the board we are running on */
	if ((platform_id == MSM8960) || (platform_id == MSM8660A)
	    || (platform_id == MSM8260A) || (platform_id == APQ8060A)) {
		switch (id) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8960_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8960_MTP;
			break;
		case HW_PLATFORM_FLUID:
			target_id = LINUX_MACHTYPE_8960_FLUID;
			break;
		case HW_PLATFORM_LIQUID:
			target_id = LINUX_MACHTYPE_8960_LIQUID;
			break;
		default:
			target_id = LINUX_MACHTYPE_8960_CDP;
		}
	} else if ((platform_id == MSM8230) || (platform_id == MSM8630)
		   || (platform_id == MSM8930) || (platform_id == APQ8030)) {
		switch (id) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8930_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8930_MTP;
			break;
		case HW_PLATFORM_FLUID:
			target_id = LINUX_MACHTYPE_8930_FLUID;
			break;
		default:
			target_id = LINUX_MACHTYPE_8930_CDP;
		}
	} else if ((platform_id == MSM8227) || (platform_id == MSM8627)) {
		switch (id) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8627_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8627_MTP;
			break;
		default:
			target_id = LINUX_MACHTYPE_8627_CDP;
		}
	} else if (platform_id == APQ8064) {
		switch (id) {
		case HW_PLATFORM_SURF:
			target_id = LINUX_MACHTYPE_8064_CDP;
			break;
		case HW_PLATFORM_MTP:
			target_id = LINUX_MACHTYPE_8064_MTP;
			break;
		case HW_PLATFORM_LIQUID:
			target_id = LINUX_MACHTYPE_8064_LIQUID;
			break;
		default:
			target_id = LINUX_MACHTYPE_8064_CDP;
		}
	} else {
		dprintf(CRITICAL, "platform_id (%d) is not identified.\n",
			platform_id);
		ASSERT(0);
	}
}

unsigned target_baseband()
{
	struct smem_board_info_v6 board_info_v6;
	unsigned int board_info_len = 0;
	unsigned smem_status = 0;
	unsigned format = 0;
	unsigned baseband = BASEBAND_MSM;

	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (!smem_status) {
		if (format >= 6) {
			board_info_len = sizeof(board_info_v6);

			smem_status =
			    smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
						  &board_info_v6,
						  board_info_len);
			if (!smem_status) {
				/* Check for MDM or APQ baseband variants.  Default to MSM */
				if (board_info_v6.platform_subtype ==
				    HW_PLATFORM_SUBTYPE_MDM)
					baseband = BASEBAND_MDM;
				else if (board_info_v6.board_info_v3.msm_id ==
					 APQ8060)
					baseband = BASEBAND_APQ;
				else if (board_info_v6.board_info_v3.msm_id ==
					 APQ8064)
					baseband = BASEBAND_APQ;
				else
					baseband = BASEBAND_MSM;
			}
		}
	}
	return baseband;
}

static unsigned target_check_power_on_reason(void)
{
	unsigned power_on_status = 0;
	unsigned int status_len = sizeof(power_on_status);
	unsigned smem_status;

	smem_status = smem_read_alloc_entry(SMEM_POWER_ON_STATUS_INFO,
					    &power_on_status, status_len);

	if (smem_status) {
		dprintf(CRITICAL,
			"ERROR: unable to read shared memory for power on reason\n");
	}
	dprintf(INFO, "Power on reason %u\n", power_on_status);
	return power_on_status;
}

void reboot_device(unsigned reboot_reason)
{
	writel(reboot_reason, RESTART_REASON_ADDR);

	/* Actually reset the chip */
	pm8921_config_reset_pwr_off(1);
	writel(0, MSM_PSHOLD_CTL_SU);
	mdelay(10000);

	dprintf(CRITICAL, "PSHOLD failed, trying watchdog reset\n");
	writel(1, MSM_WDT0_RST);
	writel(0, MSM_WDT0_EN);
	writel(0x31F3, MSM_WDT0_BT);
	writel(3, MSM_WDT0_EN);
	dmb();
	writel(3, MSM_TCSR_BASE + TCSR_WDOG_CFG);
	mdelay(10000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

unsigned check_reboot_mode(void)
{
	unsigned restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);
	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

unsigned target_pause_for_battery_charge(void)
{
	if (target_check_power_on_reason() == PWR_ON_EVENT_WALL_CHG)
		return 1;

	return 0;
}

void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

void target_battery_charging_enable(unsigned enable, unsigned disconnect)
{
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	/* Set the BOOT_DONE flag in PM8921 */
	pm8921_boot_done();
}

uint8_t get_uart_gsbi(void)
{
	switch (target_id) {
	case LINUX_MACHTYPE_8960_SIM:
	case LINUX_MACHTYPE_8960_RUMI3:
	case LINUX_MACHTYPE_8960_CDP:
	case LINUX_MACHTYPE_8960_MTP:
	case LINUX_MACHTYPE_8960_FLUID:
	case LINUX_MACHTYPE_8960_APQ:
	case LINUX_MACHTYPE_8960_LIQUID:

		return GSBI_ID_5;

	case LINUX_MACHTYPE_8930_CDP:
	case LINUX_MACHTYPE_8930_MTP:
	case LINUX_MACHTYPE_8930_FLUID:

		return GSBI_ID_5;

	case LINUX_MACHTYPE_8064_SIM:
	case LINUX_MACHTYPE_8064_RUMI3:

		return GSBI_ID_3;

	case LINUX_MACHTYPE_8627_CDP:
	case LINUX_MACHTYPE_8627_MTP:

		return GSBI_ID_5;

	default:
		dprintf(CRITICAL, "uart gsbi not defined for target: %d\n",
			target_id);

		ASSERT(0);
	}
}
