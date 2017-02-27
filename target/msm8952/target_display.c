/* Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.
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
#include <string.h>
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
#include <regulator.h>
#include <platform/clock.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <target/display.h>
#include <qtimer.h>
#include <platform.h>

#include "include/panel.h"
#include "include/display_resource.h"
#include "gcdb_display.h"

#define TRULY_720P_VID_PANEL "truly_720p_video"
#define TRULY_720P_CMD_PANEL "truly_720p_cmd"

#define HDMI_ADV_PANEL_STRING "1:dsi:0:qcom,mdss_dsi_adv7533_1080p:1:none:cfg:single_dsi"
#define TRULY_VID_PANEL_STRING "1:dsi:0:qcom,mdss_dsi_truly_720p_video:1:none:cfg:single_dsi"
#define TRULY_CMD_PANEL_STRING "1:dsi:0:qcom,mdss_dsi_truly_720p_cmd:1:none:cfg:single_dsi"

/*---------------------------------------------------------------------------*/
/* GPIO configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct gpio_pin reset_gpio = {
  "msmgpio", 0, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio = {
  "msmgpio", 90, 3, 1, 0, 1
};

static struct gpio_pin bkl_gpio = {
  "msmgpio", 91, 3, 1, 0, 1
};

static struct gpio_pin lcd_mode_gpio = {
  "msmgpio", 107, 3, 1, 0, 1
};

#define VCO_DELAY_USEC 1000
#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3
#define PMIC_WLED_SLAVE_ID 3

#define DSI0_BASE_ADJUST -0x4000
#define DSI0_PHY_BASE_ADJUST -0x4100
#define DSI0_PHY_PLL_BASE_ADJUST -0x3900
#define DSI0_PHY_REGULATOR_BASE_ADJUST -0x3C00

static void mdss_dsi_uniphy_pll_sw_reset_8952(uint32_t pll_base)
{
	writel(0x01, pll_base + 0x0068); /* PLL TEST CFG */
	mdelay(1);
	writel(0x00, pll_base + 0x0068); /* PLL TEST CFG */
	mdelay(1);
}

static void dsi_pll_toggle_lock_detect_8952(uint32_t pll_base)
{
	writel(0x04, pll_base + 0x0064); /* LKDetect CFG2 */
	udelay(1);
	writel(0x05, pll_base + 0x0064); /* LKDetect CFG2 */
	udelay(512);
}

static void dsi_pll_sw_reset_8952(uint32_t pll_base)
{
	writel(0x01, pll_base + 0x0068); /* PLL TEST CFG */
	udelay(1);
	writel(0x00, pll_base + 0x0068); /* PLL TEST CFG */
	udelay(1);
}

static uint32_t dsi_pll_lock_status_8956(uint32_t pll_base)
{
	uint32_t counter, status;

	status = readl(pll_base + 0x00c0) & 0x01;
	for (counter = 0; counter < 5 && !status; counter++) {
		udelay(100);
		status = readl(pll_base + 0x00c0) & 0x01;
	}

	return status;
}

static uint32_t gf_1_dsi_pll_enable_sequence_8952(uint32_t pll_base)
{
	uint32_t rc;

	dsi_pll_sw_reset_8952(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x14, pll_base + 0x0070); /* CAL CFG1*/
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(3);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(500);

	dsi_pll_toggle_lock_detect_8952(pll_base);
	rc = readl(pll_base + 0x00c0) & 0x01;

	return rc;
}

static uint32_t gf_2_dsi_pll_enable_sequence_8952(uint32_t pll_base)
{
	uint32_t rc;

	dsi_pll_sw_reset_8952(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x04, pll_base + 0x0070); /* CAL CFG1*/
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(3);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(500);

	dsi_pll_toggle_lock_detect_8952(pll_base);
	rc = readl(pll_base + 0x00c0) & 0x01;

	return rc;
}

static uint32_t tsmc_dsi_pll_enable_sequence_8952(uint32_t pll_base)
{
	uint32_t rc;

	dsi_pll_sw_reset_8952(pll_base);
	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */

	writel(0x34, pll_base + 0x0070); /* CAL CFG1*/
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(500);

	dsi_pll_toggle_lock_detect_8952(pll_base);
	rc = readl(pll_base + 0x00c0) & 0x01;

	return rc;
}


static uint32_t dsi_pll_enable_seq_8952(uint32_t pll_base)
{
	uint32_t pll_locked = 0;
	uint32_t counter = 0;

	do {
		pll_locked = tsmc_dsi_pll_enable_sequence_8952(pll_base);

		dprintf(SPEW, "TSMC pll locked status is %d\n", pll_locked);
		++counter;
	} while (!pll_locked && (counter < 3));

	if(!pll_locked) {
		counter = 0;
		do {
			pll_locked = gf_1_dsi_pll_enable_sequence_8952(pll_base);

			dprintf(SPEW, "GF P1 pll locked status is %d\n", pll_locked);
			++counter;
		} while (!pll_locked && (counter < 3));
	}

	if(!pll_locked) {
		counter = 0;
		do {
			pll_locked = gf_2_dsi_pll_enable_sequence_8952(pll_base);

			dprintf(SPEW, "GF P2 pll locked status is %d\n", pll_locked);
			++counter;
		} while (!pll_locked && (counter < 3));
	}

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_8956(uint32_t pll_base)
{
	/*
	 * PLL power up sequence
	 * Add necessary delays recommended by h/w team
	 */

	/* Lock Detect setting */
	writel(0x0d, pll_base + 0x0064); /* LKDetect CFG2 */
	writel(0x34, pll_base + 0x0070); /* PLL CAL_CFG1 */
	writel(0x10, pll_base + 0x005c); /* LKDetect CFG0 */
	writel(0x1a, pll_base + 0x0060); /* LKDetect CFG1 */

	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(300);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(300);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(300);
	writel(0x07, pll_base + 0x0020); /* GLB CFG */
	udelay(300);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(1000);

	return dsi_pll_lock_status_8956(pll_base);
}

static int msm8952_wled_backlight_ctrl(uint8_t enable)
{
	uint8_t slave_id = PMIC_WLED_SLAVE_ID;	/* pmi */

	pm8x41_wled_config_slave_id(slave_id);
	qpnp_wled_enable_backlight(enable);
	qpnp_ibb_enable(enable);
	return NO_ERROR;
}

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	uint32_t ret = NO_ERROR;

	if (bl->bl_interface_type == BL_DCS)
		return ret;

	ret = msm8952_wled_backlight_ctrl(enable);

	return ret;
}

static int32_t mdss_dsi_pll_config(uint32_t pll_base, uint32_t ctl_base,
		struct mdss_dsi_pll_config *pll_data)
{
	int32_t ret = 0;
	if (!platform_is_msm8956())
		mdss_dsi_uniphy_pll_sw_reset_8952(pll_base);
	else
		dsi_pll_sw_reset_8952(pll_base);
	mdss_dsi_auto_pll_config(pll_base, ctl_base, pll_data);
	if (platform_is_msm8956())
		ret = dsi_pll_enable_seq_8956(pll_base);
	else
		ret = dsi_pll_enable_seq_8952(pll_base);

	return ret;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int32_t ret = 0, flags;
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	if (pinfo->dest == DISPLAY_2) {
		flags = MMSS_DSI_CLKS_FLAG_DSI1;
		if (pinfo->mipi.dual_dsi)
			flags |= MMSS_DSI_CLKS_FLAG_DSI0;
	} else {
		flags = MMSS_DSI_CLKS_FLAG_DSI0;
		if (pinfo->mipi.dual_dsi)
			flags |= MMSS_DSI_CLKS_FLAG_DSI1;
	}

	pll_data = pinfo->mipi.dsi_pll_config;
	pll_data->vco_delay = VCO_DELAY_USEC;

	/* SSC parameters */
	if (platform_is_msm8937() || platform_is_msm8917()) {
		pll_data->ssc_en = true;
		pll_data->is_center_spread = false;
		pll_data->ssc_freq = 30000;
		pll_data->ssc_ppm = 5000;
	}

	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdss_bus_clocks_enable();
		mdp_clock_enable();
		ret = restore_secure_cfg(SECURE_DEVICE_MDSS);
		if (ret) {
			dprintf(CRITICAL,
				"%s: Failed to restore MDP security configs",
				__func__);
			mdp_clock_disable();
			mdss_bus_clocks_disable();
			mdp_gdsc_ctrl(0);
			return ret;
		}

		ret = mdss_dsi_pll_config(pinfo->mipi.pll_base,
			pinfo->mipi.ctl_base, pll_data);
		if (!ret)
			dprintf(CRITICAL, "Not able to enable master pll\n");

		if (platform_is_msm8956() && pinfo->mipi.dual_dsi &&
			!platform_is_msm8976_v_1_1()) {
				ret = mdss_dsi_pll_config(pinfo->mipi.spll_base,
					pinfo->mipi.sctl_base, pll_data);
			if (!ret)
				dprintf(CRITICAL, "Not able to enable second pll\n");
		}

		gcc_dsi_clocks_enable(flags, pinfo->mipi.use_dsi1_pll,
			pll_data->pclk_m, pll_data->pclk_n, pll_data->pclk_d);
	} else if(!target_cont_splash_screen()) {
		gcc_dsi_clocks_disable(flags);
		mdp_clock_disable();
		mdss_bus_clocks_disable();
		mdp_gdsc_ctrl(enable);
	}

	return 0;
}

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	uint32_t hw_id = board_hardware_id();
	uint32_t hw_subtype = board_hardware_subtype();

	if (platform_is_msm8956()) {
		reset_gpio.pin_id = 25;
		bkl_gpio.pin_id = 66;
	} else if (platform_is_msm8937()) {
		reset_gpio.pin_id = 60;
		bkl_gpio.pin_id = 98;
		enable_gpio.pin_id = 99;
	} else if (platform_is_msm8917()) {
		reset_gpio.pin_id = 60;
		bkl_gpio.pin_id = 98;
		pinfo->mipi.use_enable_gpio = 0;
	} else if ((hw_id == HW_PLATFORM_QRD) &&
		   (hw_subtype == HW_PLATFORM_SUBTYPE_POLARIS)) {
		enable_gpio.pin_id = 19;
	}

	if (enable) {
		if (pinfo->mipi.use_enable_gpio && !platform_is_msm8956()) {
			gpio_tlmm_config(enable_gpio.pin_id, 0,
				enable_gpio.pin_direction, enable_gpio.pin_pull,
				enable_gpio.pin_strength,
				enable_gpio.pin_state);

			gpio_set_dir(enable_gpio.pin_id, 2);
		}

		gpio_tlmm_config(bkl_gpio.pin_id, 0,
				bkl_gpio.pin_direction, bkl_gpio.pin_pull,
				bkl_gpio.pin_strength, bkl_gpio.pin_state);

		gpio_set_dir(bkl_gpio.pin_id, 2);

		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);

		gpio_set_dir(reset_gpio.pin_id, 2);

		/* reset */
		for (int i = 0; i < RESET_GPIO_SEQ_LEN; i++) {
			if (resetseq->pin_state[i] == GPIO_STATE_LOW)
				gpio_set_dir(reset_gpio.pin_id, GPIO_STATE_LOW);
			else
				gpio_set_dir(reset_gpio.pin_id, GPIO_STATE_HIGH);
			mdelay(resetseq->sleep[i]);
		}

		if (platform_is_msm8956()) {
			gpio_tlmm_config(lcd_mode_gpio.pin_id, 0,
				lcd_mode_gpio.pin_direction, lcd_mode_gpio.pin_pull,
				lcd_mode_gpio.pin_strength, lcd_mode_gpio.pin_state);

			if (pinfo->lcdc.split_display || pinfo->lcdc.dst_split)
				gpio_set_dir(lcd_mode_gpio.pin_id, GPIO_STATE_LOW);
			else
				gpio_set_dir(lcd_mode_gpio.pin_id, GPIO_STATE_HIGH);
		}
	} else if(!target_cont_splash_screen()) {
		gpio_set_dir(reset_gpio.pin_id, 0);
		if (pinfo->mipi.use_enable_gpio && !platform_is_msm8956())
			gpio_set_dir(enable_gpio.pin_id, 0);
		if (platform_is_msm8956())
			gpio_set_dir(lcd_mode_gpio.pin_id, 0);
	}

	return ret;
}

static int wled_init(struct msm_panel_info *pinfo)
{
	struct qpnp_wled_config_data config = {0};
	struct labibb_desc *labibb;
	int display_type = 0;
	bool swire_control = 0;
	bool wled_avdd_control = 0;
	int rc = NO_ERROR;

	labibb = pinfo->labibb;

	if (labibb)
		display_type = labibb->amoled_panel;

	if (display_type) {
		swire_control = labibb->swire_control;
		wled_avdd_control = true;
	} else {
		swire_control = false;
		wled_avdd_control = false;
	}

	config.display_type = display_type;
	config.lab_init_volt = 4600000;	/* fixed, see pmi register */
	config.ibb_init_volt = 1400000;	/* fixed, see pmi register */
	config.lab_ibb_swire_control = swire_control;
	config.wled_avdd_control = wled_avdd_control;

	if(!swire_control) {
		if (labibb && labibb->force_config) {
			config.lab_min_volt = labibb->lab_min_volt;
			config.lab_max_volt = labibb->lab_max_volt;
			config.ibb_min_volt = labibb->ibb_min_volt;
			config.ibb_max_volt = labibb->ibb_max_volt;
			config.pwr_up_delay = labibb->pwr_up_delay;
			config.pwr_down_delay = labibb->pwr_down_delay;
			config.ibb_discharge_en = labibb->ibb_discharge_en;
		} else {
			/* default */
			config.pwr_up_delay = 3;
			config.pwr_down_delay =  3;
			config.ibb_discharge_en = 1;
			if (display_type) {	/* amoled */
				config.lab_min_volt = 4600000;
				config.lab_max_volt = 4600000;
				config.ibb_min_volt = 4000000;
				config.ibb_max_volt = 4000000;
			} else { /* lcd */
				config.lab_min_volt = 5500000;
				config.lab_max_volt = 5500000;
				config.ibb_min_volt = 5500000;
				config.ibb_max_volt = 5500000;
			}
		}
	}

	dprintf(SPEW, "%s: %d %d %d %d %d %d %d %d %d %d\n", __func__,
		config.display_type,
		config.lab_min_volt, config.lab_max_volt,
		config.ibb_min_volt, config.ibb_max_volt,
		config.lab_init_volt, config.ibb_init_volt,
		config.pwr_up_delay, config.pwr_down_delay,
		config.ibb_discharge_en);

	/* QPNP WLED init for display backlight */
	pm8x41_wled_config_slave_id(PMIC_WLED_SLAVE_ID);

	rc = qpnp_wled_init(&config);

	return rc;
}

int target_dsi_phy_config(struct mdss_dsi_phy_ctrl *phy_db)
{
	memcpy(phy_db->regulator, panel_regulator_settings, REGULATOR_SIZE);
	memcpy(phy_db->ctrl, panel_physical_ctrl, PHYSICAL_SIZE);
	memcpy(phy_db->strength, panel_strength_ctrl, STRENGTH_SIZE);
	memcpy(phy_db->bistCtrl, panel_bist_ctrl, BIST_SIZE);
	memcpy(phy_db->laneCfg, panel_lane_config, LANE_SIZE);
	return NO_ERROR;
}

int target_display_get_base_offset(uint32_t base)
{
	if(platform_is_msm8956() || platform_is_msm8937() ||
			platform_is_msm8917()) {
		if (base == MIPI_DSI0_BASE)
			return DSI0_BASE_ADJUST;
		else if (base == DSI0_PHY_BASE)
			return DSI0_PHY_BASE_ADJUST;
		else if (base == DSI0_PLL_BASE)
			return DSI0_PHY_PLL_BASE_ADJUST;
		else if (base == DSI0_REGULATOR_BASE)
			return DSI0_PHY_REGULATOR_BASE_ADJUST;
	}

	return 0;
}

int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo)
{
	int rc = 0;
	uint32_t ldo_num = REG_LDO6 | REG_LDO17;

	if (platform_is_msm8956())
		ldo_num |= REG_LDO1;
	else
		ldo_num |= REG_LDO2;

	if (enable) {
		regulator_enable(ldo_num);
		mdelay(10);
		rc = wled_init(pinfo);
		if (rc) {
			dprintf(CRITICAL, "%s: wled init failed\n", __func__);
			return rc;
		}
		rc = qpnp_ibb_enable(true); /*5V boost*/
		if (rc) {
			dprintf(CRITICAL, "%s: qpnp_ibb failed\n", __func__);
			return rc;
		}
		mdelay(50);
	} else {
		/*
		 * LDO1, LDO2 and LDO6 are shared with other subsystems.
		 * Do not disable them.
		 */
		regulator_disable(REG_LDO17);
	}

	return NO_ERROR;
}

bool target_display_panel_node(char *pbuf, uint16_t buf_size)
{
	int prefix_string_len = strlen(DISPLAY_CMDLINE_PREFIX);
	bool ret = true;
	struct oem_panel_data oem = mdss_dsi_get_oem_data();
	uint32_t platform_subtype = board_hardware_subtype();

	/* default to hdmi for apq iot */
	if ((HW_PLATFORM_SUBTYPE_SAP == platform_subtype) ||
		(HW_PLATFORM_SUBTYPE_SAP_NOPMI == platform_subtype)) {
		if (!strcmp(oem.panel, "")) {
			if (buf_size < (prefix_string_len +
				strlen(HDMI_ADV_PANEL_STRING))) {
				dprintf(CRITICAL, "HDMI command line argument \
					is greater than buffer size\n");
				return false;
			}
			strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
			buf_size -= prefix_string_len;
			pbuf += prefix_string_len;
			strlcpy(pbuf, HDMI_ADV_PANEL_STRING, buf_size);
		} else if (!strcmp(oem.panel, TRULY_720P_VID_PANEL)) {
			if (buf_size < (prefix_string_len +
				strlen(TRULY_VID_PANEL_STRING))) {
				dprintf(CRITICAL, "TRULY VIDEO command line \
					argument is greater than \
					buffer size\n");
				return false;
			}
			strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
			buf_size -= prefix_string_len;
			pbuf += prefix_string_len;
			strlcpy(pbuf, TRULY_VID_PANEL_STRING, buf_size);
		} else if (!strcmp(oem.panel, TRULY_720P_CMD_PANEL)) {
			if (buf_size < (prefix_string_len +
				strlen(TRULY_CMD_PANEL_STRING))) {
				dprintf(CRITICAL, "TRULY CMD command line argument \
					argument is greater than \
					buffer size\n");
				return false;
			}
			strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
			buf_size -= prefix_string_len;
			pbuf += prefix_string_len;
			strlcpy(pbuf, TRULY_CMD_PANEL_STRING, buf_size);
		}
	} else {
		ret = gcdb_display_cmdline_arg(pbuf, buf_size);
	}

	return ret;
}

void target_display_init(const char *panel_name)
{
	struct oem_panel_data oem;
	int32_t ret = 0;
	uint32_t panel_loop = 0;
	uint32_t platform_subtype = board_hardware_subtype();

	set_panel_cmd_string(panel_name);
	oem = mdss_dsi_get_oem_data();

	if (!strcmp(oem.panel, NO_PANEL_CONFIG)
		|| !strcmp(oem.panel, SIM_VIDEO_PANEL)
		|| !strcmp(oem.panel, SIM_CMD_PANEL)
		|| oem.skip) {
		dprintf(INFO, "Selected panel: %s\nSkip panel configuration\n",
			oem.panel);
		return;
	}

	if ((HW_PLATFORM_SUBTYPE_SAP == platform_subtype) ||
		(HW_PLATFORM_SUBTYPE_SAP_NOPMI == platform_subtype)) {
		dprintf(INFO, "%s: Platform subtype %d\n",
			__func__, platform_subtype);
		return;
	}

	do {
		target_force_cont_splash_disable(false);
		ret = gcdb_display_init(oem.panel, MDP_REV_50, (void *)MIPI_FB_ADDR);
		if (!ret || ret == ERR_NOT_SUPPORTED) {
			break;
		} else {
			target_force_cont_splash_disable(true);
			msm_display_off();
		}
	} while (++panel_loop <= oem_panel_max_auto_detect_panels());

	if (!oem.cont_splash) {
		dprintf(INFO, "Forcing continuous splash disable\n");
		target_force_cont_splash_disable(true);
	}
}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
