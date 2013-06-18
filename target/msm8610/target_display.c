/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
#include <msm_panel.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <board.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <target/display.h>

static struct msm_fb_panel_data panel;
static uint8_t display_enable;

extern int msm_display_init(struct msm_fb_panel_data *pdata);
extern int msm_display_off();
extern void dsi_phy_init(struct msm_panel_info *pinfo);

static int msm8610_backlight(uint8_t enable)
{
	struct pm8x41_mpp mpp;
	mpp.base = PM8x41_MMP3_BASE;
	mpp.mode = MPP_HIGH;
	mpp.vin = MPP_VIN3;
	if (enable)
 {
		pm8x41_config_output_mpp(&mpp);
		pm8x41_enable_mpp(&mpp, MPP_ENABLE);
	} else {
		pm8x41_enable_mpp(&mpp, MPP_DISABLE);
	}
	/* Need delay before power on regulators */
	mdelay(20);
	return 0;
}

void dsi_calc_clk_rate(uint32_t *dsiclk_rate, uint32_t *byteclk_rate)
{
	uint32_t hbp, hfp, vbp, vfp, hspw, vspw, width, height;
	uint32_t bitclk_rate;
	int frame_rate, lanes;

	width = panel.panel_info.xres;
	height = panel.panel_info.yres;
	hbp = panel.panel_info.lcdc.h_back_porch;
	hfp = panel.panel_info.lcdc.h_front_porch;
	hspw = panel.panel_info.lcdc.h_pulse_width;
	vbp = panel.panel_info.lcdc.v_back_porch;
	vfp = panel.panel_info.lcdc.v_front_porch;
	vspw = panel.panel_info.lcdc.v_pulse_width;
	lanes = panel.panel_info.mipi.num_of_lanes;
	frame_rate = panel.panel_info.mipi.frame_rate;

	bitclk_rate = (width + hbp + hfp + hspw) * (height + vbp + vfp + vspw);
	bitclk_rate *= frame_rate;
	bitclk_rate *= panel.panel_info.bpp;
	bitclk_rate /= lanes;

	*byteclk_rate = bitclk_rate / 8;
	*dsiclk_rate = *byteclk_rate * lanes;
}

static int msm8610_mdss_dsi_panel_clock(uint8_t enable)
{
	uint32_t dsiclk_rate, byteclk_rate;

	if (enable)
	{
		mdp_clock_enable();
		dsi_calc_clk_rate(&dsiclk_rate, &byteclk_rate);
		dsi_clock_enable(dsiclk_rate, byteclk_rate);
	} else if(!target_cont_splash_screen()) {
		dsi_clock_disable();
		mdp_clock_disable();
	}

	return 0;
}

static void msm8610_mdss_mipi_panel_reset(int enable)
{
	dprintf(SPEW, "msm8610_mdss_mipi_panel_reset, enable = %d\n", enable);

	if (enable)
	{
		gpio_tlmm_config(41, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA, GPIO_DISABLE);
		gpio_tlmm_config(7, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA, GPIO_DISABLE);

		/* reset */
		gpio_set(41, 2);
		mdelay(20);
		gpio_set(41, 0);
		udelay(20);
		gpio_set(41, 2);
		mdelay(20);

		if (panel.panel_info.type == MIPI_VIDEO_PANEL)
			gpio_set(7, 2);
		else if (panel.panel_info.type == MIPI_CMD_PANEL)
			gpio_set(7, 0);
	} else if(!target_cont_splash_screen()) {
		gpio_set(7, 0);
		gpio_set(41, 0);
	}
	return;
}

static int msm8610_mipi_panel_power(uint8_t enable)
{
	int ret;
	struct pm8x41_ldo ldo4 = LDO(PM8x41_LDO4, NLDO_TYPE);
	struct pm8x41_ldo ldo14 = LDO(PM8x41_LDO14, PLDO_TYPE);
	struct pm8x41_ldo ldo19 = LDO(PM8x41_LDO19, PLDO_TYPE);

	dprintf(SPEW, "msm8610_mipi_panel_power, enable = %d\n", enable);
	if (enable)
	{
		/* backlight */
		msm8610_backlight(enable);

		/* regulators */
		pm8x41_ldo_set_voltage(&ldo14, 1800000);
		pm8x41_ldo_control(&ldo14, enable);
		pm8x41_ldo_set_voltage(&ldo19, 2850000);
		pm8x41_ldo_control(&ldo19, enable);
		pm8x41_ldo_set_voltage(&ldo4, 1200000);
		pm8x41_ldo_control(&ldo4, enable);

		/* reset */
		msm8610_mdss_mipi_panel_reset(enable);
	} else if(!target_cont_splash_screen()) {
		msm8610_backlight(0);
		msm8610_mdss_mipi_panel_reset(enable);

		pm8x41_ldo_control(&ldo19, enable);
		pm8x41_ldo_control(&ldo14, enable);
		pm8x41_ldo_control(&ldo4, enable);
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
		mipi_truly_video_wvga_init(&(panel.panel_info));
		panel.clk_func = msm8610_mdss_dsi_panel_clock;
		panel.power_func = msm8610_mipi_panel_power;
		panel.fb.base = MIPI_FB_ADDR;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_304;
		break;
	default:
		return;
	};

	if (msm_display_init(&panel))
	{
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
