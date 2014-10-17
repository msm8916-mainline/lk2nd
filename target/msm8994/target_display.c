/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
#include <qpnp_wled.h>
#include <board.h>
#include <mdp5.h>
#include <scm.h>
#include <endian.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <target/display.h>
#include "include/panel.h"
#include "include/display_resource.h"

#define HFPLL_LDO_ID 12

#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3

#define PWM_DUTY_US 13
#define PWM_PERIOD_US 27
#define PMIC_WLED_SLAVE_ID 3
#define PMIC_MPP_SLAVE_ID 2

static void dsi_pll_20nm_phy_init( uint32_t pll_base, int off)
{
	mdss_dsi_pll_20nm_sw_reset_st_machine(pll_base);

	dmb();

        /* MMSS_DSI_0_PHY_DSIPHY_CTRL_1 */
        writel(0x80, pll_base + off + 0x0174);

        /* MMSS_DSI_0_PHY_DSIPHY_CTRL_1 */
        writel(0x00, pll_base + off + 0x0174);
        udelay(5000);
        /* Strength ctrl 0 */
        writel(0x77, pll_base + off + 0x0184);
        /* MMSS_DSI_0_PHY_DSIPHY_CTRL_0 */
        writel(0x7f, pll_base + off + 0x0170);

        /* DSI_0_PHY_DSIPHY_GLBL_TEST_CTRL */
        writel(0x00, pll_base + off + 0x01d4);

        /* MMSS_DSI_0_PHY_DSIPHY_CTRL_2 */
        writel(0x00, pll_base + off + 0x0178);
}

static uint32_t dsi_pll_20nm_enable_seq(uint32_t pll_base)
{
	uint32_t pll_locked;

        /*
         * PLL power up sequence.
         * Add necessary delays recommeded by hardware.
         */
        writel(0x0D, pll_base + 0x9c); /* MMSS_DSI_PHY_PLL_PLLLOCK_CMP_EN */
        writel(0x07, pll_base + 0x14); /* MMSS_DSI_PHY_PLL_PLL_CNTRL */
        writel(0x00, pll_base + 0x2c); /* MMSS_DSI_PHY_PLL_PLL_BKG_KVCO_CAL_EN */
        udelay(500);

        dsi_pll_20nm_phy_init(pll_base, 0x200); /* Ctrl 0 */
	dmb();

        pll_locked = mdss_dsi_pll_20nm_lock_status(pll_base);
        if (!pll_locked)
                dprintf(INFO, "%s: DSI PLL lock failed\n", __func__);
        else
                dprintf(INFO, "%s: DSI PLL lock Success\n", __func__);

	return  pll_locked;
}

static int msm8994_wled_backlight_ctrl(uint8_t enable)
{
	uint8_t slave_id = 3;	/* pmi */

	if (enable) {
		pm8x41_wled_config_slave_id(slave_id);
		qpnp_wled_enable_backlight(enable);
	}
	qpnp_ibb_enable(enable);
	return NO_ERROR;
}

static int msm8994_pwm_backlight_ctrl(uint8_t enable)
{
	uint8_t slave_id = 3; /* lpg at pmi */

        if (enable) {
		/* mpp-1 had been configured already */
                /* lpg channel 4 */

		 /* LPG_ENABLE_CONTROL */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x46, 0x0);
		mdelay(100);

		 /* LPG_VALUE_LSB, duty cycle = 0x80/0x200 = 1/4 */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x44, 0x80);
		/* LPG_VALUE_MSB */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x45, 0x00);
		/* LPG_PWM_SYNC */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x47, 0x01);

		 /* LPG_PWM_SIZE_CLK, */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x41, 0x13);
		 /* LPG_PWM_FREQ_PREDIV */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x42, 0x02);
		 /* LPG_PWM_TYPE_CONFIG */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x43, 0x20);
		 /* LPG_ENABLE_CONTROL */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x46, 0x04);

		 /* SEC_ACCESS */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0xD0, 0xA5);
		 /* DTEST4, OUT_HI */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0xE5, 0x01);
		 /* LPG_ENABLE_CONTROL */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x46, 0xA4);
        } else {
		 /* LPG_ENABLE_CONTROL */
                pm8x41_lpg_write_sid(slave_id, PWM_BL_LPG_CHAN_ID, 0x46, 0x0);
        }

        return NO_ERROR;
}

void lcd_bklt_reg_enable(void)
{
	uint8_t slave_id = 2;	/* gpio at pmi */

	struct pm8x41_gpio gpio = {
                .direction = PM_GPIO_DIR_OUT,
                .function = PM_GPIO_FUNC_HIGH,
                .vin_sel = 2,   /* VIN_2 */
                .output_buffer = PM_GPIO_OUT_CMOS,
                .out_strength = PM_GPIO_OUT_DRIVE_LOW,
        };

        pm8x41_gpio_config_sid(slave_id, bklt_gpio.pin_id, &gpio);
	pm8x41_gpio_set_sid(slave_id, bklt_gpio.pin_id, 1);
}

void lcd_bklt_reg_disable(void)
{
	uint8_t slave_id = 2;	/* gpio at pmi */

	pm8x41_gpio_set_sid(slave_id, bklt_gpio.pin_id, 0);
}

void lcd_reg_enable(void)
{
       struct pm8x41_gpio gpio = {
                .direction = PM_GPIO_DIR_OUT,
                .function = PM_GPIO_FUNC_HIGH,
                .vin_sel = 2,   /* VIN_2 */
                .output_buffer = PM_GPIO_OUT_CMOS,
                .out_strength = PM_GPIO_OUT_DRIVE_MED,
        };

        pm8x41_gpio_config(lcd_reg_en.pin_id, &gpio);
	pm8x41_gpio_set(lcd_reg_en.pin_id, 1);
}

void lcd_reg_disable(void)
{
	pm8x41_gpio_set(lcd_reg_en.pin_id, 0);
}

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	uint32_t ret = NO_ERROR;
	struct pm8x41_mpp mpp;
	int rc;

	if (!bl) {
		dprintf(CRITICAL, "backlight structure is not available\n");
		return ERR_INVALID_ARGS;
	}

	switch (bl->bl_interface_type) {
	case BL_WLED:
		/* Enable MPP4 */
		pmi8994_config_mpp_slave_id(PMIC_MPP_SLAVE_ID);
	        mpp.base = PM8x41_MMP4_BASE;
		mpp.vin = MPP_VIN2;
		if (enable) {
			pm_pwm_enable(false);
			rc = pm_pwm_config(PWM_DUTY_US, PWM_PERIOD_US);
			if (rc < 0) {
				mpp.mode = MPP_HIGH;
			} else {
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
		/* Enable WLED backlight control */
		ret = msm8994_wled_backlight_ctrl(enable);
		break;
	case BL_PWM:
		/* Enable MPP1 */
		pmi8994_config_mpp_slave_id(PMIC_MPP_SLAVE_ID);
	        mpp.base = PM8x41_MMP1_BASE;
		mpp.vin = MPP_VIN2;
		mpp.mode = MPP_DTEST4;
		if (enable) {
			pm8x41_config_output_mpp(&mpp);
			pm8x41_enable_mpp(&mpp, MPP_ENABLE);
		} else {
			pm8x41_enable_mpp(&mpp, MPP_DISABLE);
		}
		/* Need delay before power on regulators */
		mdelay(20);
		ret = msm8994_pwm_backlight_ctrl(enable);
		break;
	default:
		dprintf(CRITICAL, "backlight type:%d not supported\n",
						bl->bl_interface_type);
		return ERR_NOT_SUPPORTED;
	}

	return ret;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t ret;
	struct mdss_dsi_pll_config *pll_data;
	uint32_t dual_dsi = pinfo->mipi.dual_dsi;

	pll_data = pinfo->mipi.dsi_pll_config;
	if (enable) {
		mdp_gdsc_ctrl(enable);
		mmss_bus_clock_enable();
		mdp_clock_enable();
		ret = restore_secure_cfg(SECURE_DEVICE_MDSS);
		if (ret) {
			dprintf(CRITICAL,
				"%s: Failed to restore MDP security configs",
				__func__);
			mdp_clock_disable();
			mmss_bus_clock_disable();
			mdp_gdsc_ctrl(0);
			return ret;
		}
		mdss_dsi_auto_pll_20nm_config(DSI0_PLL_BASE,
						MIPI_DSI0_BASE, pll_data);
		dsi_pll_20nm_enable_seq(DSI0_PLL_BASE);
		mmss_dsi_clock_enable(DSI0_PHY_PLL_OUT, dual_dsi,
					pll_data->pclk_m,
					pll_data->pclk_n,
					pll_data->pclk_d);
	} else if(!target_cont_splash_screen()) {
		/* Disable clocks if continuous splash off */
		mmss_dsi_clock_disable(dual_dsi);
		mdp_clock_disable();
		mmss_bus_clock_disable();
		mdp_gdsc_ctrl(enable);
	}

	return NO_ERROR;
}

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
					struct msm_panel_info *pinfo)
{
	uint32_t i = 0;

	if (enable) {
		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);
		/* reset */
		for (i = 0; i < RESET_GPIO_SEQ_LEN; i++) {
			if (resetseq->pin_state[i] == GPIO_STATE_LOW)
				gpio_set(reset_gpio.pin_id, GPIO_STATE_LOW);
			else
				gpio_set(reset_gpio.pin_id, GPIO_STATE_HIGH);
			mdelay(resetseq->sleep[i]);
		}
		lcd_bklt_reg_enable();
	} else {
		lcd_bklt_reg_disable();
		gpio_set(reset_gpio.pin_id, 0);
	}

	return NO_ERROR;
}

int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo)
{
	if (enable) {
		regulator_enable();	/* L2, L12, L14, and L28 */
		mdelay(10);
		qpnp_ibb_enable(true);	/* +5V and -5V */
		mdelay(50);

		if (pinfo->lcd_reg_en)
			lcd_reg_enable();
	} else {
		if (pinfo->lcd_reg_en)
			lcd_reg_disable();

		regulator_disable();
	}

	return NO_ERROR;
}

int target_display_pre_on()
{
	writel(0xC0000CCC, MDP_CLK_CTRL0);
	writel(0xC0000CCC, MDP_CLK_CTRL1);
	writel(0x00CCCCCC, MDP_CLK_CTRL2);
	writel(0x000000CC, MDP_CLK_CTRL6);
	writel(0x0CCCC0C0, MDP_CLK_CTRL3);
	writel(0xCCCCC0C0, MDP_CLK_CTRL4);
	writel(0xCCCCC0C0, MDP_CLK_CTRL5);
	writel(0x00CCC000, MDP_CLK_CTRL7);

	return NO_ERROR;
}

bool target_display_panel_node(char *panel_name, char *pbuf, uint16_t buf_size)
{
	int prefix_string_len = strlen(DISPLAY_CMDLINE_PREFIX);
	bool ret = true;

	ret = gcdb_display_cmdline_arg(panel_name, pbuf, buf_size);

	return ret;
}

void target_display_init(const char *panel_name)
{
	panel_name += strspn(panel_name, " ");

	if (!strcmp(panel_name, NO_PANEL_CONFIG)
		|| !strcmp(panel_name, SIM_VIDEO_PANEL)
		|| !strcmp(panel_name, SIM_DUALDSI_VIDEO_PANEL)
		|| !strcmp(panel_name, SIM_CMD_PANEL)
		|| !strcmp(panel_name, SIM_DUALDSI_CMD_PANEL)) {
		dprintf(INFO, "Selected panel: %s\nSkip panel configuration\n",
			panel_name);
		return;
	}
	if (gcdb_display_init(panel_name, MDP_REV_50, MIPI_FB_ADDR)) {
		target_force_cont_splash_disable(true);
		msm_display_off();
	}
}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
