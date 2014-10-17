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

static struct backlight edp_bklt = {
	0, 1, 4095, 100, 1, "PMIC_8941"
};

static uint32_t dsi_pll_lock_status(uint32_t pll_base)
{
	uint32_t counter, status;

	udelay(100);
	mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);

	status = readl(pll_base + 0x00c0) & 0x01;
	for (counter = 0; counter < 5 && !status; counter++) {
		udelay(100);
		status = readl(pll_base + 0x00c0) & 0x01;
	}

	return status;
}

static uint32_t dsi_pll_enable_seq_b(uint32_t pll_base)
{
	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(1);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x07, pll_base + 0x0020); /* GLB CFG */
	udelay(500);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(500);

	return dsi_pll_lock_status(pll_base);
}

static uint32_t dsi_pll_enable_seq_d(uint32_t pll_base)
{
	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(1);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x07, pll_base + 0x0020); /* GLB CFG */
	udelay(250);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x07, pll_base + 0x0020); /* GLB CFG */
	udelay(500);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(500);

	return dsi_pll_lock_status(pll_base);
}

static void dsi_pll_enable_seq(uint32_t pll_base)
{
	uint32_t counter, status;

	for (counter = 0; counter < 3; counter++) {
		status = dsi_pll_enable_seq_b(pll_base);
		if (status)
			break;
		status = dsi_pll_enable_seq_d(pll_base);
		if (status)
			break;
		status = dsi_pll_enable_seq_d(pll_base);
		if(status)
			break;
	}

	if (!status)
		dprintf(CRITICAL, "Pll lock sequence failed\n");
}

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	struct pm8x41_gpio pwmgpio_param = {
		.direction = PM_GPIO_DIR_OUT,
		.function = PM_GPIO_FUNC_1,
		.vin_sel = 2,	/* VIN_2 */
		.pull = PM_GPIO_PULL_UP_1_5 | PM_GPIO_PULLDOWN_10,
		.output_buffer = PM_GPIO_OUT_CMOS,
		.out_strength = 0x03,
	};

	if (enable) {
		pm8x41_gpio_config(pwm_gpio.pin_id, &pwmgpio_param);

		/* lpg channel 3 */
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x41, 0x33); /* LPG_PWM_SIZE_CLK, */
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x42, 0x01); /* LPG_PWM_FREQ_PREDIV */
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x43, 0x20); /* LPG_PWM_TYPE_CONFIG */
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x44, 0xcc); /* LPG_VALUE_LSB */
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x45, 0x00);  /* LPG_VALUE_MSB */
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x46, 0xe4); /* LPG_ENABLE_CONTROL */
	} else {
		pm8x41_lpg_write(PWM_BL_LPG_CHAN_ID, 0x46, 0x0); /* LPG_ENABLE_CONTROL */
	}

	return NO_ERROR;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t ret;
	struct mdss_dsi_pll_config *pll_data;
	uint32_t dual_dsi = pinfo->mipi.dual_dsi;
	dprintf(SPEW, "target_panel_clock\n");

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
		mdss_dsi_auto_pll_config(DSI0_PLL_BASE,
						MIPI_DSI0_BASE, pll_data);
		dsi_pll_enable_seq(DSI0_PLL_BASE);
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

/* Pull DISP_RST_N high to get panel out of reset */
int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
					struct msm_panel_info *pinfo)
{
	uint32_t i = 0;

	if (enable) {
		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);

		gpio_tlmm_config(enable_gpio.pin_id, 0,
			enable_gpio.pin_direction, enable_gpio.pin_pull,
			enable_gpio.pin_strength, enable_gpio.pin_state);

		gpio_tlmm_config(bkl_gpio.pin_id, 0,
			bkl_gpio.pin_direction, bkl_gpio.pin_pull,
			bkl_gpio.pin_strength, bkl_gpio.pin_state);

		gpio_set(enable_gpio.pin_id, 2);
		gpio_set(bkl_gpio.pin_id, 2);
		/* reset */
		for (i = 0; i < RESET_GPIO_SEQ_LEN; i++) {
			if (resetseq->pin_state[i] == GPIO_STATE_LOW)
				gpio_set(reset_gpio.pin_id, GPIO_STATE_LOW);
			else
				gpio_set(reset_gpio.pin_id, GPIO_STATE_HIGH);
			mdelay(resetseq->sleep[i]);
		}
	} else {
		gpio_set(reset_gpio.pin_id, 0);
		gpio_set(enable_gpio.pin_id, 0);
		gpio_set(bkl_gpio.pin_id, 0);
	}

	return NO_ERROR;
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
		} else if(ldo_entry_array[ldocounter].ldo_id != HFPLL_LDO_ID) {
			pm8x41_ldo_control(&ldo_entry, enable);
		}
		ldocounter++;
	}

	return NO_ERROR;
}

int target_display_pre_on()
{
	writel(0x000000FA, MDP_QOS_REMAPPER_CLASS_0);
	writel(0x00000055, MDP_QOS_REMAPPER_CLASS_1);
	writel(0xC0000CCD, MDP_CLK_CTRL0);
	writel(0xD0000CCC, MDP_CLK_CTRL1);
	writel(0x00CCCCCC, MDP_CLK_CTRL2);
	writel(0x000000CC, MDP_CLK_CTRL6);
	writel(0x0CCCC0C0, MDP_CLK_CTRL3);
	writel(0xCCCCC0C0, MDP_CLK_CTRL4);
	writel(0xCCCCC0C0, MDP_CLK_CTRL5);
	writel(0x00CCC000, MDP_CLK_CTRL7);

	writel(0x00080808, VBIF_VBIF_IN_RD_LIM_CONF0);
	writel(0x08000808, VBIF_VBIF_IN_RD_LIM_CONF1);
	writel(0x00080808, VBIF_VBIF_IN_RD_LIM_CONF2);
	writel(0x00000808, VBIF_VBIF_IN_RD_LIM_CONF3);
	writel(0x10000000, VBIF_VBIF_IN_WR_LIM_CONF0);
	writel(0x00100000, VBIF_VBIF_IN_WR_LIM_CONF1);
	writel(0x10000000, VBIF_VBIF_IN_WR_LIM_CONF2);
	writel(0x00000000, VBIF_VBIF_IN_WR_LIM_CONF3);
	writel(0x00013fff, VBIF_VBIF_ABIT_SHORT);
	writel(0x000000A4, VBIF_VBIF_ABIT_SHORT_CONF);
	writel(0x00003FFF, VBIF_VBIF_GATE_OFF_WRREQ_EN);
	writel(0x00000003, VBIF_VBIF_DDR_RND_RBN_QOS_ARB);

	return NO_ERROR;
}

int target_hdmi_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t ret;

	dprintf(SPEW, "%s: target_panel_clock\n", __func__);

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

		hdmi_phy_reset();
		hdmi_pll_config();
		hdmi_vco_enable();
		hdmi_clk_enable();
	} else if(!target_cont_splash_screen()) {
		/* Disable clocks if continuous splash off */
		hdmi_clk_disable();
		hdmi_vco_disable();
		mdp_clock_disable();
		mmss_bus_clock_disable();
		mdp_gdsc_ctrl(enable);
	}

	return NO_ERROR;
}

static void target_hdmi_mvs_enable(bool enable)
{
	struct pm8x41_mvs mvs;
	mvs.base = PM8x41_MVS1_BASE;

	if (enable)
		pm8x41_enable_mvs(&mvs, MVS_ENABLE);
	else
		pm8x41_enable_mvs(&mvs, MVS_DISABLE);
}

static void target_hdmi_vreg_enable(bool enable)
{
	struct pm8x41_mpp mpp;
	mpp.base = PM8x41_MMP3_BASE;

	if (enable) {
		mpp.mode = MPP_HIGH;
		mpp.vin = MPP_VIN2;
		pm8x41_config_output_mpp(&mpp);
		pm8x41_enable_mpp(&mpp, MPP_ENABLE);
	} else {
		pm8x41_enable_mpp(&mpp, MPP_DISABLE);
	}
}

int target_hdmi_regulator_ctrl(bool enable)
{
	target_hdmi_mvs_enable(enable);
	target_hdmi_vreg_enable(enable);

	return 0;
}

int target_hdmi_gpio_ctrl(bool enable)
{
	gpio_tlmm_config(hdmi_cec_gpio.pin_id, 1,	/* gpio 31, CEC */
		hdmi_cec_gpio.pin_direction, hdmi_cec_gpio.pin_pull,
		hdmi_cec_gpio.pin_strength, hdmi_cec_gpio.pin_state);

	gpio_tlmm_config(hdmi_ddc_clk_gpio.pin_id, 1,	/* gpio 32, DDC CLK */
		hdmi_ddc_clk_gpio.pin_direction, hdmi_ddc_clk_gpio.pin_pull,
		hdmi_ddc_clk_gpio.pin_strength, hdmi_ddc_clk_gpio.pin_state);


	gpio_tlmm_config(hdmi_ddc_data_gpio.pin_id, 1,	/* gpio 33, DDC DATA */
		hdmi_ddc_data_gpio.pin_direction, hdmi_ddc_data_gpio.pin_pull,
		hdmi_ddc_data_gpio.pin_strength, hdmi_ddc_data_gpio.pin_state);

	gpio_tlmm_config(hdmi_hpd_gpio.pin_id, 1,	/* gpio 34, HPD */
		hdmi_hpd_gpio.pin_direction, hdmi_hpd_gpio.pin_pull,
		hdmi_hpd_gpio.pin_strength, hdmi_hpd_gpio.pin_state);

	gpio_set(hdmi_cec_gpio.pin_id,      hdmi_cec_gpio.pin_direction);
	gpio_set(hdmi_ddc_clk_gpio.pin_id,  hdmi_ddc_clk_gpio.pin_direction);
	gpio_set(hdmi_ddc_data_gpio.pin_id, hdmi_ddc_data_gpio.pin_direction);
	gpio_set(hdmi_hpd_gpio.pin_id,      hdmi_hpd_gpio.pin_direction);

	/* MUX */
	gpio_tlmm_config(hdmi_mux_lpm_gpio.pin_id, 0,   /* gpio 27 MUX LPM */
		hdmi_mux_lpm_gpio.pin_direction, hdmi_mux_lpm_gpio.pin_pull,
		hdmi_mux_lpm_gpio.pin_strength, hdmi_mux_lpm_gpio.pin_state);

	gpio_tlmm_config(hdmi_mux_en_gpio.pin_id, 0,    /* gpio 83 MUX EN */
		hdmi_mux_en_gpio.pin_direction, hdmi_mux_en_gpio.pin_pull,
		hdmi_mux_en_gpio.pin_strength, hdmi_mux_en_gpio.pin_state);

	gpio_tlmm_config(hdmi_mux_sel_gpio.pin_id, 0,   /* gpio 85 MUX SEL */
		hdmi_mux_sel_gpio.pin_direction, hdmi_mux_sel_gpio.pin_pull,
		hdmi_mux_sel_gpio.pin_strength, hdmi_mux_sel_gpio.pin_state);

	gpio_set(hdmi_mux_lpm_gpio.pin_id, hdmi_mux_lpm_gpio.pin_direction);
	gpio_set(hdmi_mux_en_gpio.pin_id,  hdmi_mux_en_gpio.pin_direction);
	gpio_set(hdmi_mux_sel_gpio.pin_id, hdmi_mux_sel_gpio.pin_direction);

	return NO_ERROR;
}

void target_edp_panel_init(struct msm_panel_info *pinfo)
{
	edp_panel_init(pinfo);
}

int target_edp_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t ret;

	dprintf(SPEW, "%s: target_panel_clock\n", __func__);

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

		edp_clk_enable();
	} else if(!target_cont_splash_screen()) {
		/* Disable clocks if continuous splash off */
		edp_clk_disable();
		mdp_clock_disable();
		mmss_bus_clock_disable();
		mdp_gdsc_ctrl(enable);
	}

	return NO_ERROR;
}

int target_edp_panel_enable(void)
{
	gpio_tlmm_config(enable_gpio.pin_id, 0,		/* gpio 137 */
		enable_gpio.pin_direction, enable_gpio.pin_pull,
		enable_gpio.pin_strength, enable_gpio.pin_state);


	gpio_tlmm_config(edp_hpd_gpio.pin_id, 0,	/* hpd 103 */
		edp_hpd_gpio.pin_direction, edp_hpd_gpio.pin_pull,
		edp_hpd_gpio.pin_strength, edp_hpd_gpio.pin_state);


	gpio_tlmm_config(edp_lvl_en_gpio.pin_id, 0,	/* lvl_en 91 */
		edp_lvl_en_gpio.pin_direction, edp_lvl_en_gpio.pin_pull,
		edp_lvl_en_gpio.pin_strength, edp_lvl_en_gpio.pin_state);

	gpio_set(enable_gpio.pin_id, 2);
	gpio_set(edp_lvl_en_gpio.pin_id, 2);

	return NO_ERROR;
}

int target_edp_panel_disable(void)
{
	gpio_set(edp_lvl_en_gpio.pin_id, 0);
	gpio_set(enable_gpio.pin_id, 0);

	return NO_ERROR;
}

int target_edp_bl_ctrl(int enable)
{
	return target_backlight_ctrl(&edp_bklt, enable);
}

bool target_display_panel_node(char *panel_name, char *pbuf, uint16_t buf_size)
{
	int prefix_string_len = strlen(DISPLAY_CMDLINE_PREFIX);
	bool ret = true;

	panel_name += strspn(panel_name, " ");

	if (!strcmp(panel_name, HDMI_PANEL_NAME)) {
		if (buf_size < (prefix_string_len + LK_OVERRIDE_PANEL_LEN +
				strlen(HDMI_CONTROLLER_STRING))) {
			dprintf(CRITICAL, "command line argument is greater than buffer size\n");
			return false;
		}

		strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
		buf_size -= prefix_string_len;
		strlcat(pbuf, LK_OVERRIDE_PANEL, buf_size);
		buf_size -= LK_OVERRIDE_PANEL_LEN;
		strlcat(pbuf, HDMI_CONTROLLER_STRING, buf_size);
	} else {
		ret = gcdb_display_cmdline_arg(panel_name, pbuf, buf_size);
	}

	return ret;
}

void target_display_init(const char *panel_name)
{
	uint32_t ret = 0;

	panel_name += strspn(panel_name, " ");

	if (!strcmp(panel_name, NO_PANEL_CONFIG)
		|| !strcmp(panel_name, SIM_VIDEO_PANEL)
		|| !strcmp(panel_name, SIM_DUALDSI_VIDEO_PANEL)
		|| !strcmp(panel_name, SIM_CMD_PANEL)
		|| !strcmp(panel_name, SIM_DUALDSI_CMD_PANEL)) {
		dprintf(INFO, "Selected panel: %s\nSkip panel configuration\n",
			panel_name);
		return;
	} else if (!strcmp(panel_name, HDMI_PANEL_NAME)) {
		dprintf(INFO, "%s: HDMI is primary\n", __func__);
		mdss_hdmi_display_init(MDP_REV_50, HDMI_FB_ADDR);
		return;
	}

	ret = gcdb_display_init(panel_name, MDP_REV_50, MIPI_FB_ADDR);
	if (ret) {
		target_force_cont_splash_disable(true);
		msm_display_off();
	}
}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
