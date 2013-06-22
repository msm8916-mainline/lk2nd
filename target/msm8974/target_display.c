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
#include <msm_panel.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <board.h>
#include <mdp5.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <target/display.h>

static struct msm_fb_panel_data panel;
static uint8_t display_enable;

extern int msm_display_init(struct msm_fb_panel_data *pdata);
extern int msm_display_off();
extern int mdss_dsi_uniphy_pll_config(uint32_t ctl_base);
extern int mdss_sharp_dsi_uniphy_pll_config(uint32_t ctl_base);

static struct pm8x41_wled_data wled_ctrl = {
	.mod_scheme      = 0xC3,
	.led1_brightness = (0x0F << 8) | 0xEF,
	.led2_brightness = (0x0F << 8) | 0xEF,
	.led3_brightness = (0x0F << 8) | 0xEF,
	.max_duty_cycle  = 0x01,
	.ovp = 0x2,
	.full_current_scale = 0x19
};

static int msm8974_backlight_on()
{
	pm8x41_wled_config(&wled_ctrl);
	pm8x41_wled_sink_control(1);
	pm8x41_wled_iled_sync_control(1);
	pm8x41_wled_enable(1);

	return 0;
}

static int msm8974_mdss_dsi_panel_clock(uint8_t enable)
{
	uint32_t dual_dsi = panel.panel_info.mipi.dual_dsi;
	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdp_clock_init();
		mdss_dsi_uniphy_pll_config(MIPI_DSI0_BASE);
		if (panel.panel_info.mipi.dual_dsi &&
				!(panel.panel_info.mipi.broadcast))
			mdss_dsi_uniphy_pll_config(MIPI_DSI1_BASE);
		mmss_clock_init(DSI0_PHY_PLL_OUT, dual_dsi);
	} else if(!target_cont_splash_screen()) {
		// * Add here for continuous splash  *
		mmss_clock_disable(dual_dsi);
		mdp_clock_disable(dual_dsi);
		mdp_gdsc_ctrl(enable);
	}

	return 0;
}

static int msm8974_mdss_sharp_dsi_panel_clock(uint8_t enable)
{
	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdp_clock_init();
		mdss_sharp_dsi_uniphy_pll_config(MIPI_DSI0_BASE);
		mmss_clock_init(DSI0_PHY_PLL_OUT);
	} else if (!target_cont_splash_screen()) {
		/* Add here for continuous splash  */
		mmss_clock_disable();
		mdp_clock_disable();
		mdp_gdsc_ctrl(enable);
	}

	return 0;
}

/* Pull DISP_RST_N high to get panel out of reset */
static void msm8974_mdss_mipi_panel_reset(uint8_t enable)
{
	struct pm8x41_gpio gpio19_param = {
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = PM_GPIO_OUT_CMOS,
		.out_strength = PM_GPIO_OUT_DRIVE_MED,
	};

	pm8x41_gpio_config(19, &gpio19_param);
	if (enable) {
		gpio_tlmm_config(58, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA, GPIO_DISABLE);

		pm8x41_gpio_set(19, PM_GPIO_FUNC_HIGH);
		mdelay(2);
		pm8x41_gpio_set(19, PM_GPIO_FUNC_LOW);
		mdelay(5);
		pm8x41_gpio_set(19, PM_GPIO_FUNC_HIGH);
		mdelay(2);
		gpio_set(58, 2);
	} else {
		gpio19_param.out_strength = PM_GPIO_OUT_DRIVE_LOW;
		pm8x41_gpio_config(19, &gpio19_param);
		pm8x41_gpio_set(19, PM_GPIO_FUNC_LOW);
		gpio_set(58, 2);
	}
}

static int msm8974_mipi_panel_power(uint8_t enable)
{

	struct pm8x41_ldo ldo2  = LDO(PM8x41_LDO2, NLDO_TYPE);
	struct pm8x41_ldo ldo12 = LDO(PM8x41_LDO12, PLDO_TYPE);
	struct pm8x41_ldo ldo22 = LDO(PM8x41_LDO22, PLDO_TYPE);

	if (enable) {

		/* Enable backlight */
		msm8974_backlight_on();

		/* Turn on LDO8 for lcd1 mipi vdd */
		dprintf(SPEW, " Setting LDO22\n");
		pm8x41_ldo_set_voltage(&ldo22, 3000000);
		pm8x41_ldo_control(&ldo22, enable);

		dprintf(SPEW, " Setting LDO12\n");
		/* Turn on LDO23 for lcd1 mipi vddio */
		pm8x41_ldo_set_voltage(&ldo12, 1800000);
		pm8x41_ldo_control(&ldo12, enable);

		dprintf(SPEW, " Setting LDO2\n");
		/* Turn on LDO2 for vdda_mipi_dsi */
		pm8x41_ldo_set_voltage(&ldo2, 1200000);
		pm8x41_ldo_control(&ldo2, enable);

		dprintf(SPEW, " Panel Reset \n");
		/* Panel Reset */
		msm8974_mdss_mipi_panel_reset(enable);
		dprintf(SPEW, " Panel Reset Done\n");
	} else {
		msm8974_mdss_mipi_panel_reset(enable);
		pm8x41_wled_enable(enable);
		pm8x41_ldo_control(&ldo2, enable);
		pm8x41_ldo_control(&ldo22, enable);

	}

	return 0;
}

void display_init(void)
{
	uint32_t hw_id = board_hardware_id();
	uint32_t soc_ver = board_soc_version();

	dprintf(INFO, "display_init(),target_id=%d.\n", hw_id);

	switch (hw_id) {
	case HW_PLATFORM_MTP:
	case HW_PLATFORM_FLUID:
	case HW_PLATFORM_SURF:
		mipi_toshiba_video_720p_init(&(panel.panel_info));
		panel.clk_func = msm8974_mdss_dsi_panel_clock;
		panel.power_func = msm8974_mipi_panel_power;
		panel.fb.base = MIPI_FB_ADDR;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_50;
		break;
	case HW_PLATFORM_DRAGON:
		mipi_sharp_video_qhd_init(&(panel.panel_info));
		wled_ctrl.ovp = 0x1; /* 32V */
		wled_ctrl.full_current_scale = 0x14; /* 20mA */
		panel.clk_func = msm8974_mdss_sharp_dsi_panel_clock;
		panel.power_func = msm8974_mipi_panel_power;
		panel.fb.base = MIPI_FB_ADDR;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_50;
		break;
	default:
		return;
	};

	if (msm_display_init(&panel)) {
		dprintf(CRITICAL, "Display init failed!\n");
		return;
	}

	display_enable = 1;
}

void display_shutdown(void)
{
	if (display_enable)
		msm_display_off();
}
