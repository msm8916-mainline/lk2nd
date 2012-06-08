/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
#include <msm_panel.h>
#include <dev/pm8921.h>
#include <board.h>
#include <mdp4.h>

static struct msm_fb_panel_data panel;
static uint8_t display_enable;

extern int msm_display_init(struct msm_fb_panel_data *pdata);
extern int msm_display_off();

static int apq8064_lvds_panel_power(int enable)
{
	if (enable) {
		/* Enable LVS7 */
		pm8921_low_voltage_switch_enable(lvs_7);
		/* Set and enabale LDO2 1.2V for  VDDA_LVDS_PLL*/
		pm8921_ldo_set_voltage(LDO_2, LDO_VOLTAGE_1_2V);

		/* Enable Ext 3.3V -  MSM GPIO 77*/
		apq8064_ext_3p3V_enable();

		apq8064_display_gpio_init();

		/* Configure PMM MPP  3*/
		pm8921_mpp_set_digital_output(mpp_3);
	}

	return 0;
}

static int apq8064_lvds_clock(int enable)
{
	if (enable)
		mdp_clock_init();

	return 0;
}

static int fusion3_mtp_panel_power(int enable)
{
	if (enable) {
		/* Enable LVS7 */
		pm8921_low_voltage_switch_enable(7);

		/* Set and enabale LDO2 1.2V for  VDDA_MIPI_DSI0/1_PLL */
		pm8921_ldo_set_voltage(LDO_2, LDO_VOLTAGE_1_2V);

		/* Set and enabale LDO11 3.0V for  LCD1_MIPI_AVDD */
		pm8921_ldo_set_voltage(LDO_11, LDO_VOLTAGE_3_0V);

		apq8064_display_gpio_init();
	}

	return 0;
}

static int fusion3_mtp_clock(int enable)
{
	if (enable) {
		mdp_clock_init();
		mmss_clock_init();
	} else {
#if (!CONT_SPLASH_SCREEN)
		mmss_clock_disable();
#endif
	}

	return 0;
}

static void msm8960_backlight_on(void)
{
	struct pm8921_gpio backlight_pwm = {
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = 0,
		.output_value = 0,
		.pull = PM_GPIO_PULL_NO,
		.vin_sel = 2,
		.out_strength = PM_GPIO_STRENGTH_HIGH,
		.function = PM_GPIO_FUNC_1,
		.inv_int_pol = 0,
	};

	int rc = pm8921_gpio_config(PM_GPIO(24), &backlight_pwm);
	if (rc)
		dprintf(CRITICAL, "FAIL pm8921_gpio_config(): rc=%d.\n", rc);
}

/* Pull DISP_RST_N high to get panel out of reset */
static void msm8960_mipi_panel_reset(void)
{
	struct pm8921_gpio gpio43_param = {
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = 0,
		.output_value = 1,
		.pull = PM_GPIO_PULL_UP_30,
		.vin_sel = 2,
		.out_strength = PM_GPIO_STRENGTH_HIGH,
		.function = PM_GPIO_FUNC_PAIRED,
		.inv_int_pol = 0,
		.disable_pin = 0,
	};
	pm8921_gpio_config(PM_GPIO(43), &gpio43_param);
}

static int msm8960_mipi_panel_clock(int enable)
{
	if (enable) {
		mdp_clock_init();
		mmss_clock_init();
	} else {
#if (!CONT_SPLASH_SCREEN)
		mmss_clock_disable();
#endif
	}

	return 0;
}

static int msm8960_mipi_panel_power(int enable)
{
	if (enable) {
		msm8960_backlight_on();

		/* Turn on LDO8 for lcd1 mipi vdd */
		pm8921_ldo_set_voltage(LDO_8, LDO_VOLTAGE_3_0V);

		/* Turn on LDO23 for lcd1 mipi vddio */
		pm8921_ldo_set_voltage(LDO_23, LDO_VOLTAGE_1_8V);

		/* Turn on LDO2 for vdda_mipi_dsi */
		pm8921_ldo_set_voltage(LDO_2, LDO_VOLTAGE_1_2V);

		msm8960_mipi_panel_reset();
	}

	return 0;
}

void display_init(void)
{
	dprintf(CRITICAL, "display_init\n");

	switch (board_target_id()) {
	case LINUX_MACHTYPE_8064_CDP:
		lvds_chimei_wxga_init(&(panel.panel_info));
		panel.clk_func = apq8064_lvds_clock;
		panel.power_func = apq8064_lvds_panel_power;
		panel.fb.base = 0x80B00000;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_44;
		break;
	case LINUX_MACHTYPE_8064_MTP:
		mipi_toshiba_video_wsvga_init(&(panel.panel_info));
		panel.clk_func = fusion3_mtp_clock;
		panel.power_func = fusion3_mtp_panel_power;
		panel.fb.base = 0x89000000;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_44;
		break;
	case LINUX_MACHTYPE_8960_CDP:
	case LINUX_MACHTYPE_8960_MTP:
	case LINUX_MACHTYPE_8960_FLUID:
	case LINUX_MACHTYPE_8960_LIQUID:
		mipi_toshiba_video_wsvga_init(&(panel.panel_info));
		panel.clk_func = msm8960_mipi_panel_clock;
		panel.power_func = msm8960_mipi_panel_power;
		panel.fb.base = 0x89000000;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_42;
		break;
	default:
		return;
	};

	if (msm_display_init(&panel)) {
		dprintf(CRITICAL, "Display init failed!\n");
		return;
	}

	display_image_on_screen();
	display_enable = 1;
}

void display_shutdown(void)
{
	if (display_enable)
		msm_display_off();
}

