/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
#include <platform/clock.h>
#include <platform/iomap.h>
#include <target/display.h>
#include "include/panel.h"
#include "include/display_resource.h"

static struct msm_fb_panel_data panel;
static uint8_t edp_enable;

#define HFPLL_LDO_ID 12

static struct pm8x41_wled_data wled_ctrl = {
	.mod_scheme      = 0x00,
	.led1_brightness = (0x0F << 8) | 0xEF,
	.led2_brightness = (0x0F << 8) | 0xEF,
	.led3_brightness = (0x0F << 8) | 0xEF,
	.max_duty_cycle  = 0x01,
	.ovp = 0x2,
	.full_current_scale = 0x19
};

static uint32_t dsi_pll_enable_seq(uint32_t ctl_base)
{
	uint32_t rc = 0;

	mdss_dsi_uniphy_pll_sw_reset(ctl_base);

	writel(0x01, ctl_base + 0x0220); /* GLB CFG */
	mdelay(1);
	writel(0x05, ctl_base + 0x0220); /* GLB CFG */
	mdelay(1);
	writel(0x07, ctl_base + 0x0220); /* GLB CFG */
	mdelay(1);
	writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
	mdelay(1);

	mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);

	while (!(readl(ctl_base + 0x02c0) & 0x01)) {
		mdss_dsi_uniphy_pll_sw_reset(ctl_base);
		writel(0x01, ctl_base + 0x0220); /* GLB CFG */
		mdelay(1);
		writel(0x05, ctl_base + 0x0220); /* GLB CFG */
		mdelay(1);
		writel(0x07, ctl_base + 0x0220); /* GLB CFG */
		mdelay(1);
		writel(0x05, ctl_base + 0x0220); /* GLB CFG */
		mdelay(1);
		writel(0x07, ctl_base + 0x0220); /* GLB CFG */
		mdelay(1);
		writel(0x0f, ctl_base + 0x0220); /* GLB CFG */
		mdelay(2);
		mdss_dsi_uniphy_pll_lock_detect_setting(ctl_base);
	}
	return rc;
}

int target_backlight_ctrl(uint8_t enable)
{
	uint32_t platform_id = board_platform_id();
	uint32_t hardware_id = board_hardware_id();
	uint8_t slave_id = 1;

	if (enable) {
		if (platform_id == MSM8974AC)
			if ((hardware_id == HW_PLATFORM_MTP)
			    || (hardware_id == HW_PLATFORM_LIQUID))
				slave_id = 3;

		pm8x41_wled_config_slave_id(slave_id);
		pm8x41_wled_config(&wled_ctrl);
		pm8x41_wled_sink_control(enable);
		pm8x41_wled_iled_sync_control(enable);
	}
	pm8x41_wled_enable(enable);

	return NO_ERROR;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	struct mdss_dsi_pll_config *pll_data;
	uint32_t dual_dsi = pinfo->mipi.dual_dsi;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;
	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdp_clock_init();
		mdss_dsi_uniphy_pll_config(MIPI_DSI0_BASE);
		dsi_pll_enable_seq(MIPI_DSI0_BASE);
		if (panel.panel_info.mipi.dual_dsi &&
				!(panel.panel_info.mipi.broadcast)) {
			mdss_dsi_uniphy_pll_config(MIPI_DSI1_BASE);
			dsi_pll_enable_seq(MIPI_DSI1_BASE);
		}
		mmss_clock_auto_pll_init(DSI0_PHY_PLL_OUT, dual_dsi,
					pll_data->pclk_m,
					pll_data->pclk_n,
					pll_data->pclk_d);
	} else if(!target_cont_splash_screen()) {
		// * Add here for continuous splash  *
		mmss_clock_disable(dual_dsi);
		mdp_clock_disable(dual_dsi);
		mdp_gdsc_ctrl(enable);
	}

	return NO_ERROR;
}

/* Pull DISP_RST_N high to get panel out of reset */
int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
					struct msm_panel_info *pinfo)
{
	uint32_t rst_gpio = reset_gpio.pin_id;
	uint32_t platform_id = board_platform_id();
	uint32_t hardware_id = board_hardware_id();

	struct pm8x41_gpio resetgpio_param = {
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = PM_GPIO_OUT_CMOS,
		.out_strength = PM_GPIO_OUT_DRIVE_MED,
	};

	if (platform_id == MSM8974AC)
		if ((hardware_id == HW_PLATFORM_MTP)
		    || (hardware_id == HW_PLATFORM_LIQUID))
			rst_gpio = 20;

	dprintf(SPEW, "platform_id: %u, rst_gpio: %u\n",
				platform_id, rst_gpio);

	pm8x41_gpio_config(rst_gpio, &resetgpio_param);
	if (enable) {
		gpio_tlmm_config(enable_gpio.pin_id, 0,
			enable_gpio.pin_direction, enable_gpio.pin_pull,
			enable_gpio.pin_strength, enable_gpio.pin_state);

		gpio_set(enable_gpio.pin_id, resetseq->pin_direction);
		pm8x41_gpio_set(rst_gpio, resetseq->pin_state[0]);
		mdelay(resetseq->sleep[0]);
		pm8x41_gpio_set(rst_gpio, resetseq->pin_state[1]);
		mdelay(resetseq->sleep[1]);
		pm8x41_gpio_set(rst_gpio, resetseq->pin_state[2]);
		mdelay(resetseq->sleep[2]);
	} else {
		resetgpio_param.out_strength = PM_GPIO_OUT_DRIVE_LOW;
		pm8x41_gpio_config(rst_gpio, &resetgpio_param);
		pm8x41_gpio_set(rst_gpio, PM_GPIO_FUNC_LOW);
		gpio_set(enable_gpio.pin_id, resetseq->pin_direction);
	}
	return NO_ERROR;
}

int target_ldo_ctrl(uint8_t enable)
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
		} else if(ldo_entry_array[ldocounter].ldo_id != HFPLL_LDO_ID) {
			pm8x41_ldo_control(&ldo_entry, enable);
		}
		ldocounter++;
	}

	return NO_ERROR;
}

static int msm8974_mdss_edp_panel_clock(int enable)
{
	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdp_clock_init();
		edp_clk_enable();
	} else if (!target_cont_splash_screen()) {
		/* Add here for continuous splash */
		edp_clk_disable();
		mdp_clock_disable();
		mdp_gdsc_ctrl(enable);
	}

	return 0;
}

static int msm8974_edp_panel_power(int enable)
{
	struct pm8x41_gpio gpio36_param = {
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = PM_GPIO_OUT_CMOS,
		.out_strength = PM_GPIO_OUT_DRIVE_MED,
	};

	struct pm8x41_ldo ldo12 = LDO(PM8x41_LDO12, PLDO_TYPE);

	if (enable) {
		/* Enable backlight */
		dprintf(SPEW, "Enable Backlight\n");
		pm8x41_gpio_config(36, &gpio36_param);
		pm8x41_gpio_set(36, PM_GPIO_FUNC_HIGH);
		dprintf(SPEW, "Enable Backlight Done\n");

		/* Turn on LDO12 for edp vdda */
		dprintf(SPEW, "Setting LDO12 n");
		pm8x41_ldo_set_voltage(&ldo12, 1800000);
		pm8x41_ldo_control(&ldo12, enable);
		dprintf(SPEW, "Setting LDO12 Done\n");

		/* Panel Enable */
		dprintf(SPEW, "Panel Enable\n");
		gpio_tlmm_config(58, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA,
				GPIO_DISABLE);
		gpio_set(58, 2);
		dprintf(SPEW, "Panel Enable Done\n");
	} else {
		/* Keep LDO12 on, otherwise kernel will not boot */
		gpio_set(58, 0);
		pm8x41_gpio_set(36, PM_GPIO_FUNC_LOW);
	}

	return 0;
}

void display_init(void)
{
	uint32_t hw_id = board_hardware_id();
	switch (hw_id) {
	case HW_PLATFORM_LIQUID:
		edp_panel_init(&(panel.panel_info));
		panel.clk_func = msm8974_mdss_edp_panel_clock;
		panel.power_func = msm8974_edp_panel_power;
		panel.fb.base = (void *)EDP_FB_ADDR;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_50;

		if (msm_display_init(&panel)) {
			dprintf(CRITICAL, "edp init failed!\n");
			return;
		}

		edp_enable = 1;
		break;
	default:
		gcdb_display_init(MDP_REV_50, MIPI_FB_ADDR);
		break;
	}
}

void display_shutdown(void)
{
	uint32_t hw_id = board_hardware_id();
	switch (hw_id) {
	case HW_PLATFORM_LIQUID:
		if (edp_enable)
			msm_display_off();
		break;
	default:
		gcdb_display_shutdown();
		break;
	}
}
