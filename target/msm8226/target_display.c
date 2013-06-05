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
#include <msm_panel.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <board.h>
#include <mdp5.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <target/display.h>

static struct msm_fb_panel_data panel;
static uint8_t display_enable;

extern int msm_display_init(struct msm_fb_panel_data *pdata);
extern int msm_display_off();
extern int mdss_dsi_uniphy_pll_config(uint32_t ctl_base);

static struct pm8x41_wled_data wled_ctrl = {
	.mod_scheme      = 0xC3,
	.led1_brightness = (0x0F << 8) | 0xEF,
	.led2_brightness = (0x0F << 8) | 0xEF,
	.led3_brightness = (0x0F << 8) | 0xEF,
	.max_duty_cycle  = 0x01,
	.ovp = 0x2,
	.full_current_scale = 0x19
};

static int msm8226_backlight_on()
{
	pm8x41_wled_config(&wled_ctrl);
	pm8x41_wled_sink_control(1);
	pm8x41_wled_iled_sync_control(1);
	pm8x41_wled_enable(1);

	return 0;
}

static int msm8226_mdss_dsi_panel_clock(uint8_t enable)
{
	dprintf(SPEW, "msm8226_mdss_dsi_panel_clock, enable = %d\n", enable);

	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdp_clock_init();
		mdss_dsi_uniphy_pll_config(MIPI_DSI0_BASE);
		mmss_clock_init(0x100);
	} else if(!target_cont_splash_screen()) {
		/* Add here for non-continuous splash */
		/* FIXME:Need to disable the clocks.
		 * For now leave the clocks enabled until the kernel
		 * hang issue gets resolved
		 */
	}

	return 0;
}

static void msm8226_mdss_mipi_panel_reset(int enable)
{
	dprintf(SPEW, "msm8226_mdss_mipi_panel_reset, enable = %d\n", enable);

	if (enable) {
		gpio_tlmm_config(25, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA, GPIO_DISABLE);
		gpio_set_dir(25, 2); //output

		/* reset */
		gpio_set_value(25, 1);
		mdelay(20);
		gpio_set_value(25, 0);
		mdelay(20);
		gpio_set_value(25, 1);
		mdelay(20);
	} else if(!target_cont_splash_screen()) {
		gpio_set_value(25, 0);
	}

	return;
}

static int msm8226_mipi_panel_power(uint8_t enable)
{
	struct pm8x41_ldo ldo4  = LDO(PM8x41_LDO4, NLDO_TYPE);
	struct pm8x41_ldo ldo8 = LDO(PM8x41_LDO8, PLDO_TYPE);
	struct pm8x41_ldo ldo15 = LDO(PM8x41_LDO15, PLDO_TYPE);

	dprintf(SPEW, "msm8226_mipi_panel_power, enable = %d\n", enable);

	if (enable) {
		/* backlight */
		msm8226_backlight_on();

		pm8x41_ldo_set_voltage(&ldo15, 2800000);
		pm8x41_ldo_control(&ldo15, enable);

		pm8x41_ldo_set_voltage(&ldo8, 1800000);
		pm8x41_ldo_control(&ldo8, enable);

		pm8x41_ldo_set_voltage(&ldo4, 1200000);
		pm8x41_ldo_control(&ldo4, enable);

		/* reset */
		msm8226_mdss_mipi_panel_reset(enable);
	} else if(!target_cont_splash_screen()) {
		msm8226_mdss_mipi_panel_reset(enable);
		pm8x41_wled_enable(enable);
		pm8x41_ldo_control(&ldo4, enable);
		pm8x41_ldo_control(&ldo15, enable);
	}

	return 0;
}

void display_init(void)
{
	uint32_t hw_id = board_hardware_id();

	dprintf(SPEW, "display_init(),target_id=%d.\n", hw_id);

	switch (hw_id) {
	case HW_PLATFORM_QRD:
	case HW_PLATFORM_MTP:
	case HW_PLATFORM_SURF:
#if DISPLAY_TYPE_CMD_MODE
		mipi_nt35590_cmd_720p_init(&(panel.panel_info));
#else
		mipi_nt35590_video_720p_init(&(panel.panel_info));
#endif
		panel.clk_func = msm8226_mdss_dsi_panel_clock;
		panel.power_func = msm8226_mipi_panel_power;
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
