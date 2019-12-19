/* Copyright (c) 2015, 2019, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <bits.h>
#include <debug.h>
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <pm_smbchg_common.h>
#include <qpnp-smb2.h>
#include <qpnp-fg-gen3.h>
#include <qtimer.h>
#include <reboot.h>
#include <target.h>

#define BATT_CHARGER_STATUS_1_REG	0x1006
#define BATT_CHARGER_STATUS_2_REG	0x1007
#define BATT_IF_INT_RT_STS	0x1210
#define USBIN_PLUGIN_RT_STS_REG	0x1310

#define BATT_STATUS_MASK	0x07
#define DISABLE_CHARGE	0x07

char panel_name[256];

static bool is_battery_present(void)
{
	uint8_t val = 0;

	val = REG_READ(BATT_IF_INT_RT_STS);

	/*
	 * If BATT_TERMINAL_MISSING_RT_STS BIT(5) or BATT_THERM_OR_ID_MISSING_RT_STS BIT(4)
	 * are set, battery is not present.
	 */
	if ((val & BIT(5)) | (val & BIT(4)))
		return false;

	return true;
}

static void dump_jeita_status(void)
{
	uint8_t val = 0;

	val = REG_READ(BATT_CHARGER_STATUS_2_REG);
	dprintf(INFO,"BATT_CHARGER_STATUS_2_REG value : %d\n",val);
}

static bool is_usb_present(void)
{
	uint8_t val = 0;

	val = REG_READ(USBIN_PLUGIN_RT_STS_REG);

	if (val & BIT(4))
		return true;

	return false;
}

static bool is_battery_charging(void)
{
	uint8_t val = 0;

	val = REG_READ(BATT_CHARGER_STATUS_1_REG);
	dprintf(INFO,"BATT_CHARGER_STATUS_1_REG value : %d\n",val);
	val = val & BATT_STATUS_MASK;

	if (val == DISABLE_CHARGE)
		return false;

	return true;
}

void weak_battery_charging(void)
{
	uint32_t current_vbat = 0;
	int batt_curr = 0;

	current_vbat = fg_gen3_get_battery_voltage();
	batt_curr = fg_gen3_get_battery_current();
	dprintf(INFO,"Entering LK charging loop: Battery current=%d mA voltage=%d mV\n",
								batt_curr,current_vbat);

	while (current_vbat < LK_BATT_VOLT_THRESHOLD)
	{
		if (!is_battery_present()) {
			dprintf(CRITICAL, "Battery is not present.Exiting weak battery charging\n");
			break;
		}

		if (!is_usb_present()) {
			dprintf(CRITICAL, "Shutting down because USB is not present\n");
			shutdown_device();
		}

		if (!is_battery_charging()) {
			dump_jeita_status();
			dprintf(CRITICAL, "Shutting down because battery is not charging\n");
			shutdown_device();
		}

		if (!display_initialized) {
			charge_in_progress = true;
			target_display_init(panel_name);
			display_initialized = true;
		}

		current_vbat = fg_gen3_get_battery_voltage();
		batt_curr = fg_gen3_get_battery_current();
		dprintf(INFO, "Battery Charging: current=%d mA voltage=%d mV\n",
							batt_curr,current_vbat);
		mdelay(1000);
	}

	charge_in_progress = false;
	dprintf(INFO,"Battery Charging is completed Booting to HLOS\n");
}