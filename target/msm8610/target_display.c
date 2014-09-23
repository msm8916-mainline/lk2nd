/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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
 *
 */

#include <debug.h>
#include <smem.h>
#include <err.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <board.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <pm_pwm.h>
#include <target/display.h>

#include "include/panel.h"
#include "include/display_resource.h"

#define MODE_GPIO_STATE_ENABLE 1

#define MODE_GPIO_STATE_DISABLE 2
#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3

#define PWM_DUTY_US 13
#define PWM_PERIOD_US 27

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	struct pm8x41_mpp mpp;
	int rc;

	mpp.base = PM8x41_MMP3_BASE;
	mpp.vin = MPP_VIN3;
	if (enable) {
		pm_pwm_enable(false);
		rc = pm_pwm_config(PWM_DUTY_US, PWM_PERIOD_US);
		if (rc < 0)
			mpp.mode = MPP_HIGH;
		else {
			mpp.mode = MPP_DTEST1;
			pm_pwm_enable(true);
		}
		pm8x41_config_output_mpp(&mpp);
		pm8x41_enable_mpp(&mpp, MPP_ENABLE);
	} else {
		pm_pwm_enable(false);
		pm8x41_enable_mpp(&mpp, MPP_DISABLE);
	}
	/* Need delay before power on regulators */
	mdelay(20);
	return 0;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;

	if (enable) {
		mdp_clock_enable();
		dsi_clock_enable(
			pll_data->byte_clock * pinfo->mipi.num_of_lanes,
			pll_data->byte_clock);
	} else if(!target_cont_splash_screen()) {
		dsi_clock_disable();
		mdp_clock_disable();
	}

	return 0;
}

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo)
{
	uint8_t i = 0;
	dprintf(SPEW, "msm8610_mdss_mipi_panel_reset, enable = %d\n", enable);

	if (enable) {
		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);

		gpio_tlmm_config(mode_gpio.pin_id, 0,
				mode_gpio.pin_direction, mode_gpio.pin_pull,
				mode_gpio.pin_strength, mode_gpio.pin_state);

		/* reset */
		for (i = 0; i < RESET_GPIO_SEQ_LEN; i++) {
			if (resetseq->pin_state[i] == GPIO_STATE_LOW)
				gpio_set(reset_gpio.pin_id, GPIO_STATE_LOW);
			else
				gpio_set(reset_gpio.pin_id, GPIO_STATE_HIGH);
			mdelay(resetseq->sleep[i]);
		}

		if (pinfo->mipi.mode_gpio_state == MODE_GPIO_STATE_ENABLE)
			gpio_set(mode_gpio.pin_id, 2);
		else if (pinfo->mipi.mode_gpio_state == MODE_GPIO_STATE_DISABLE)
			gpio_set(mode_gpio.pin_id, 0);
	} else if(!target_cont_splash_screen()) {
		gpio_set(reset_gpio.pin_id, 0);
		gpio_set(mode_gpio.pin_id, 0);
	}
	return 0;
}

int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t ldocounter = 0;
	uint32_t pm8x41_ldo_base = 0x13F00;

	while (ldocounter < TOTAL_LDO_DEFINED) {
		struct pm8x41_ldo ldo_entry = LDO((pm8x41_ldo_base +
			0x100 * ldo_entry_array[ldocounter].ldo_id),
			ldo_entry_array[ldocounter].ldo_type);

		dprintf(SPEW, "Setting %s\n",
				ldo_entry_array[ldocounter].ldo_id);

		/* Set voltage during power on */
		if (enable) {
			pm8x41_ldo_set_voltage(&ldo_entry,
					ldo_entry_array[ldocounter].ldo_voltage);
			pm8x41_ldo_control(&ldo_entry, enable);
		} else if(!target_cont_splash_screen()) {
			pm8x41_ldo_control(&ldo_entry, enable);
		}
		ldocounter++;
	}

	return 0;
}

bool target_display_panel_node(char *panel_name, char *pbuf, uint16_t buf_size)
{
	return gcdb_display_cmdline_arg(panel_name, pbuf, buf_size);
}

void target_display_init(const char *panel_name)
{
	uint32_t panel_loop = 0;
	uint32_t ret = 0;

	if (!strcmp(panel_name, NO_PANEL_CONFIG)) {
		dprintf(INFO, "Skip panel configuration\n");
		return;
	}

	do {
		target_force_cont_splash_disable(false);
		ret = gcdb_display_init(panel_name, MDP_REV_304, MIPI_FB_ADDR);
		if (ret) {
			/*Panel signature did not match, turn off the display*/
			target_force_cont_splash_disable(true);
			msm_display_off();
		} else {
			break;
		}
	} while (++panel_loop <= oem_panel_max_auto_detect_panels());

}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
