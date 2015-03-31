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
#include <dev/keys.h>
#include <spmi_v2.h>
#include <pm8x41.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <reboot.h>

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

/* Return true if it is triggered by alarm. */
uint32_t check_alarm_boot(void)
{
	/* Check reboot reason and power on reason */
	if (pm8x41_get_is_cold_boot()) {
		if (pm8x41_get_pon_reason() == RTC_TRG)
			return 1;
	} else {
		if (readl(RESTART_REASON_ADDR) == ALARM_BOOT)
			return 1;
	}

	return 0;
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
	if((reboot_reason == FASTBOOT_MODE) || (reboot_reason == DLOAD) || (reboot_reason == RECOVERY_MODE))
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

