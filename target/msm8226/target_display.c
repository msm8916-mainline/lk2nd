/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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
#include <scm.h>
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

static uint32_t dsi_pll_enable_seq_m(uint32_t ctl_base)
{
	uint32_t i = 0;
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(ctl_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
	pll_locked = readl(ctl_base + 0x02c0) & 0x01;
	for (i = 0; (i < 4) && !pll_locked; i++) {
		writel(0x07, ctl_base + 0x0220); /* GLB CFG */
		if (i != 0)
			writel(0x34, ctl_base + 0x00270); /* CAL CFG1*/
		udelay(1);
		writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
		udelay(1000);
		mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
		pll_locked = readl(ctl_base + 0x02c0) & 0x01;
	}

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_d(uint32_t ctl_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(ctl_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x07, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x07, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
	pll_locked = readl(ctl_base + 0x02c0) & 0x01;

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_f1(uint32_t ctl_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(ctl_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0d, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
	pll_locked = readl(ctl_base + 0x02c0) & 0x01;

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_c(uint32_t ctl_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(ctl_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
	pll_locked = readl(ctl_base + 0x02c0) & 0x01;

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_e(uint32_t ctl_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(ctl_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	udelay(200);
	writel(0x0d, ctl_base + 0x0220); /* GLB CFG */
	udelay(1);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
	pll_locked = readl(ctl_base + 0x02c0) & 0x01;

	return pll_locked;
}

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	dprintf(SPEW, "target_backlight_ctrl\n");

	if (!bl) {
		dprintf(CRITICAL, "backlight structure is not available\n");
		return ERR_INVALID_ARGS;
	}

	if (bl->bl_interface_type != BL_WLED) {
		dprintf(CRITICAL, "backlight type:%d not supported\n",
							bl->bl_interface_type);
		return ERR_NOT_SUPPORTED;
	}

	if (enable) {
		pm8x41_wled_config(&wled_ctrl);
		pm8x41_wled_sink_control(enable);
		pm8x41_wled_iled_sync_control(enable);
		pm8x41_wled_led_mod_enable(enable);
	}

	pm8x41_wled_enable(enable);

	return 0;
}

static void dsi_pll_enable_seq(uint32_t ctl_base)
{
	if (dsi_pll_enable_seq_m(ctl_base)) {
	} else if (dsi_pll_enable_seq_d(ctl_base)) {
	} else if (dsi_pll_enable_seq_d(ctl_base)) {
	} else if (dsi_pll_enable_seq_f1(ctl_base)) {
	} else if (dsi_pll_enable_seq_c(ctl_base)) {
	} else if (dsi_pll_enable_seq_e(ctl_base)) {
	} else {
		dprintf(CRITICAL, "Not able to enable the pll\n");
	}
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int32_t ret;
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;

	if (enable) {
		mdp_gdsc_ctrl(enable);
		mmss_bus_clocks_enable();
		mdp_clock_enable();
		ret = restore_secure_cfg(SECURE_DEVICE_MDSS);
		if (ret) {
			dprintf(CRITICAL,
				"%s: Failed to restore MDP security configs",
				__func__);
			mdp_clock_disable();
			mmss_bus_clocks_disable();
			mdp_gdsc_ctrl(0);
			return ret;
		}
		mdss_dsi_auto_pll_config(MIPI_DSI0_BASE, pll_data);
		dsi_pll_enable_seq(MIPI_DSI0_BASE);
		mmss_dsi_clocks_enable(pll_data->pclk_m,
				pll_data->pclk_n,
				pll_data->pclk_d);
	} else if(!target_cont_splash_screen()) {
		mmss_dsi_clocks_disable();
		mdp_clock_disable();
		mmss_bus_clocks_disable();
		mdp_gdsc_ctrl(enable);
	}

	return 0;
}

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	if (enable) {
		if (pinfo->mipi.use_enable_gpio) {
			gpio_tlmm_config(enable_gpio.pin_id, 0,
				enable_gpio.pin_direction, enable_gpio.pin_pull,
				enable_gpio.pin_strength,
				enable_gpio.pin_state);

			gpio_set_dir(enable_gpio.pin_id, 2);
		}

		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);

		gpio_set_dir(reset_gpio.pin_id, 2);

		gpio_set_value(reset_gpio.pin_id, resetseq->pin_state[0]);
		mdelay(resetseq->sleep[0]);
		gpio_set_value(reset_gpio.pin_id, resetseq->pin_state[1]);
		mdelay(resetseq->sleep[1]);
		gpio_set_value(reset_gpio.pin_id, resetseq->pin_state[2]);
		mdelay(resetseq->sleep[2]);
	} else if(!target_cont_splash_screen()) {
		gpio_set_value(reset_gpio.pin_id, 0);
		if (pinfo->mipi.use_enable_gpio)
			gpio_set_value(enable_gpio.pin_id, 0);
	}

	return ret;
}

int target_ldo_ctrl(uint8_t enable)
{
	uint32_t ret = NO_ERROR;
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
        uint32_t panel_loop = 0;
        uint32_t ret = 0;

	do {
		ret = gcdb_display_init(MDP_REV_50, MIPI_FB_ADDR);
		if (!ret || ret == ERR_NOT_SUPPORTED) {
			break;
		} else {
			target_force_cont_splash_disable(true);
			msm_display_off();
			target_force_cont_splash_disable(false);
		}
	} while (++panel_loop <= oem_panel_max_auto_detect_panels());

}

void display_shutdown(void)
{
	gcdb_display_shutdown();
}
