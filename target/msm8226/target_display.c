/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#include <smem.h>
#include <err.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <board.h>
#include <mdp5.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <target/display.h>

#include "include/panel.h"
#include "include/display_resource.h"

#define HFPLL_LDO_ID 8

static struct pm8x41_wled_data wled_ctrl = {
	.mod_scheme      = 0x00,
	.led1_brightness = (0x0F << 8) | 0xEF,
	.max_duty_cycle  = 0x01,
	.ovp = 0x0,
	.full_current_scale = 0x19,
	.fdbck = 0x1
};

int target_backlight_ctrl(uint8_t enable)
{
	dprintf(SPEW, "target_backlight_ctrl\n");

	pm8x41_wled_config(&wled_ctrl);
	pm8x41_wled_sink_control(1);
	pm8x41_wled_iled_sync_control(1);
	pm8x41_wled_enable(1);

	return 0;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;

	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdp_clock_init();
		mdss_dsi_auto_pll_config(pll_data);
		mmss_clock_auto_pll_init(pll_data->pclk_m,
				pll_data->pclk_n,
				pll_data->pclk_d);
	} else if(!target_cont_splash_screen()) {
		/* Add here for non-continuous splash */
		/* FIXME:Need to disable the clocks.
		 * For now leave the clocks enabled until the kernel
		 * hang issue gets resolved
		 */
	}

	return 0;
}

int target_panel_reset(uint8_t enable,
				struct gpio_pin *resetgpio,
				struct gpio_pin *enablegpio,
				struct panel_reset_sequence *resetseq)
{
	int ret = NO_ERROR;
	if (enable) {
		gpio_tlmm_config(resetgpio->pin_id, 0,
				resetgpio->pin_direction, resetgpio->pin_pull,
				resetgpio->pin_strength, resetgpio->pin_state);

		gpio_set_dir(resetgpio->pin_id, 2);

		gpio_set_value(resetgpio->pin_id, resetseq->pin_state[0]);
		mdelay(resetseq->sleep[0]);
		gpio_set_value(resetgpio->pin_id, resetseq->pin_state[1]);
		mdelay(resetseq->sleep[1]);
		gpio_set_value(resetgpio->pin_id, resetseq->pin_state[2]);
		mdelay(resetseq->sleep[2]);
	} else if(!target_cont_splash_screen()) {
		gpio_set_value(resetgpio->pin_id, 0);
	}

	return ret;
}

int target_ldo_ctrl(uint8_t enable, struct ldo_entry ldo_entry_array[],
			uint8_t ldo_array_size)
{
	uint32_t ret = NO_ERROR;
	uint32_t ldocounter = 0;
	uint32_t pm8x41_ldo_base = 0x13F00;

	while (ldocounter < ldo_array_size) {
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

		} else if(!target_cont_splash_screen() &&
				ldo_entry_array[ldocounter].ldo_id != HFPLL_LDO_ID) {
			pm8x41_ldo_control(&ldo_entry, enable);
		}
		ldocounter++;
	}

	return ret;
}

void display_init(void)
{
	gcdb_display_init(MDP_REV_50, MIPI_FB_ADDR);
}

void display_shutdown(void)
{
	gcdb_display_shutdown();
}
