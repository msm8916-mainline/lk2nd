/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
#include <platform/iomap.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <uart_dm.h>
#include <mmc.h>
#include <platform/gpio.h>
#include <dev/keys.h>
#include <spmi_v2.h>
#include <pm8x41.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>

#if LONG_PRESS_POWER_ON
#include <shutdown_detect.h>
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0
#define TLMM_VOL_UP_BTN_GPIO    85

#define FASTBOOT_MODE           0x77665500
#define PON_SOFT_RB_SPARE       0x88F

static uint32_t mmc_sdc_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };


void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(1, 0, BLSP1_UART1_BASE);
#endif
}

void target_mmc_caps(struct mmc_host *host)
{
	host->caps.ddr_mode = 0;
	host->caps.hs200_mode = 0;
	host->caps.bus_width = MMC_BOOT_BUS_WIDTH_8_BIT;
	host->caps.hs_clk_rate = MMC_CLK_50MHZ;
}

/* Return 1 if vol_up pressed */
static int target_volume_up()
{
	uint8_t status = 0;

	gpio_tlmm_config(TLMM_VOL_UP_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

	/* Wait for the gpio config to take effect - debounce time */
	thread_sleep(10);

	/* Get status of GPIO */
	status = gpio_status(TLMM_VOL_UP_BTN_GPIO);

	/* Active high signal. */
	return status;
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down()
{
	/* Volume down button tied in with PMIC RESIN. */
	return pm8x41_resin_status();
}

static void target_keystatus()
{
	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

/* Configure PMIC and Drop PS_HOLD for shutdown */
void shutdown_device()
{
	dprintf(CRITICAL, "Going down for shutdown.\n");

	/* Configure PMIC for shutdown */
	pm8x41_reset_configure(PON_PSHOLD_SHUTDOWN);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "shutdown failed\n");

	ASSERT(0);
}


void target_init(void)
{
	uint32_t base_addr;
	uint8_t slot;

	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	target_keystatus();

	/* Trying Slot 1*/
	slot = 1;
	base_addr = mmc_sdc_base[slot - 1];
	if (mmc_boot_main(slot, base_addr))
	{

	/* Trying Slot 2 next */
	slot = 2;
	base_addr = mmc_sdc_base[slot - 1];
	if (mmc_boot_main(slot, base_addr)) {
		dprintf(CRITICAL, "mmc init failed!");
		ASSERT(0);
		}
	}
#if LONG_PRESS_POWER_ON
	shutdown_detect();
#endif
}

void target_serialno(unsigned char *buf)
{
	uint32_t serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE_UNKNOWN;
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	/* This is already filled as part of board.c */
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;

	platform = board->platform;

	switch(platform) {
	case MSM8952:
	case MSM8956:
	case MSM8976:
		board->baseband = BASEBAND_MSM;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n",platform);
		ASSERT(0);
	};
}

unsigned target_baseband()
{
	return board_baseband();
}

unsigned check_reboot_mode(void)
{
	uint32_t restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);
	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

unsigned check_hard_reboot_mode(void)
{
	uint8_t hard_restart_reason = 0;
	uint8_t value = 0;

	/* Read reboot reason and scrub it
	  * Bit-5, bit-6 and bit-7 of SOFT_RB_SPARE for hard reset reason
	  */
	value = pm8x41_reg_read(PON_SOFT_RB_SPARE);
	hard_restart_reason = value >> 5;
	pm8x41_reg_write(PON_SOFT_RB_SPARE, value & 0x1f);

	return hard_restart_reason;
}

int set_download_mode(enum dload_mode mode)
{
	int ret = 0;
	ret = scm_dload_mode(mode);

	pm8x41_clear_pmic_watchdog();

	return ret;
}

int emmc_recovery_init(void)
{
	return _emmc_recovery_init();
}

void reboot_device(unsigned reboot_reason)
{
	uint8_t reset_type = 0;
	uint32_t ret = 0;

	/* Need to clear the SW_RESET_ENTRY register and
	 * write to the BOOT_MISC_REG for known reset cases
	 */
	if(reboot_reason != DLOAD)
		scm_dload_mode(NORMAL_MODE);

	writel(reboot_reason, RESTART_REASON_ADDR);

	/* For Reboot-bootloader and Dload cases do a warm reset
	 * For Reboot cases do a hard reset
	 */
	if((reboot_reason == FASTBOOT_MODE) || (reboot_reason == DLOAD))
		reset_type = PON_PSHOLD_WARM_RESET;
	else
		reset_type = PON_PSHOLD_HARD_RESET;

	pm8x41_reset_configure(reset_type);

	ret = scm_halt_pmic_arbiter();
	if (ret)
		dprintf(CRITICAL , "Failed to halt pmic arbiter: %d\n", ret);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

#if USER_FORCE_RESET_SUPPORT
/* Return 1 if it is a force resin triggered by user. */
uint32_t is_user_force_reset(void)
{
	uint8_t poff_reason1 = pm8x41_get_pon_poff_reason1();
	uint8_t poff_reason2 = pm8x41_get_pon_poff_reason2();

	dprintf(SPEW, "poff_reason1: %d\n", poff_reason1);
	dprintf(SPEW, "poff_reason2: %d\n", poff_reason2);
	if (pm8x41_get_is_cold_boot() && (poff_reason1 == KPDPWR_AND_RESIN ||
							poff_reason2 == STAGE3))
		return 1;
	else
		return 0;
}
#endif

unsigned target_pause_for_battery_charge(void)
{
	uint8_t pon_reason = pm8x41_get_pon_reason();
	uint8_t is_cold_boot = pm8x41_get_is_cold_boot();
	dprintf(INFO, "%s : pon_reason is %d cold_boot:%d\n", __func__,
		pon_reason, is_cold_boot);
	/* In case of fastboot reboot,adb reboot or if we see the power key
	* pressed we do not want go into charger mode.
	* fastboot reboot is warm boot with PON hard reset bit not set
	* adb reboot is a cold boot with PON hard reset bit set
	*/
	if (is_cold_boot &&
			(!(pon_reason & HARD_RST)) &&
			(!(pon_reason & KPDPWR_N)) &&
			((pon_reason & USB_CHG) || (pon_reason & DC_CHG) || (pon_reason & CBLPWR_N)))
		return 1;
	else
		return 0;
}
