/* Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.
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
#include <mdp3.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <target/display.h>

#include "include/panel.h"
#include "include/display_resource.h"

#define VCO_DELAY_USEC 1000
#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3
#define PWM_DUTY_US 13
#define PWM_PERIOD_US 27
#define PM8916_VER 0x20000

static void mdss_dsi_uniphy_pll_sw_reset_8909(uint32_t pll_base)
{
	writel(0x01, pll_base + 0x0068); /* PLL TEST CFG */
	mdelay(1);
	writel(0x00, pll_base + 0x0068); /* PLL TEST CFG */
	mdelay(1);
}

static void dsi_pll_toggle_lock_detect_8909(uint32_t pll_base)
{
	writel(0x04, pll_base + 0x0064); /* LKDetect CFG2 */
	udelay(1);
	writel(0x05, pll_base + 0x0064); /* LKDetect CFG2 */
	udelay(512);
}

static void dsi_pll_sw_reset_8909(uint32_t pll_base)
{
	writel(0x01, pll_base + 0x0068); /* PLL TEST CFG */
	udelay(1);
	writel(0x00, pll_base + 0x0068); /* PLL TEST CFG */
}

static uint32_t dsi_pll_enable_seq_1_8909(uint32_t pll_base)
{
	uint32_t rc;

	dsi_pll_sw_reset_8909(pll_base);
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

	dsi_pll_toggle_lock_detect_8909(pll_base);
	rc = readl(pll_base + 0x00c0) & 0x01;

	return rc;
}

static uint32_t dsi_pll_enable_seq_2_8909(uint32_t pll_base)
{
	uint32_t rc;

	dsi_pll_sw_reset_8909(pll_base);

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

	dsi_pll_toggle_lock_detect_8909(pll_base);
	rc = readl(pll_base + 0x00c0) & 0x01;

	return rc;
}

static uint32_t dsi_pll_enable_seq_3_8909(uint32_t pll_base)
{
	uint32_t rc;

	dsi_pll_sw_reset_8909(pll_base);

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

	dsi_pll_toggle_lock_detect_8909(pll_base);
	rc = readl(pll_base + 0x00c0) & 0x01;

	return rc;
}

static uint32_t dsi_pll_enable_seq_8909(uint32_t pll_base)
{
	uint32_t pll_locked = 0;
	uint32_t counter = 0;

	do {
		pll_locked = dsi_pll_enable_seq_1_8909(pll_base);

		dprintf(SPEW, "TSMC pll locked status is %d\n", pll_locked);
		++counter;
	} while (!pll_locked && (counter < 3));

	if (!pll_locked) {
		counter = 0;
		do {
			pll_locked = dsi_pll_enable_seq_2_8909(pll_base);

			dprintf(SPEW, "GF P1 pll locked status is %d\n",
				pll_locked);
			++counter;
		} while (!pll_locked && (counter < 3));
	}

	if (!pll_locked) {
		counter = 0;
		do {
			pll_locked = dsi_pll_enable_seq_3_8909(pll_base);

			dprintf(SPEW, "GF P2 pll locked status is %d\n",
				pll_locked);
			++counter;
		} while (!pll_locked && (counter < 3));
	}

	return pll_locked;
}

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	struct pm8x41_mpp mpp;
	uint32_t hw_id = board_hardware_id();
	struct board_pmic_data pmic_info;
	int rc;

	if (bl->bl_interface_type == BL_DCS)
		return 0;

	board_pmic_info(&pmic_info, 1);
	if (pmic_info.pmic_version == PM8916_VER)
		mpp.base = PM8x41_MMP4_BASE;
	else
		mpp.base = PM8x41_MMP2_BASE;

	mpp.vin = MPP_VIN0;
	if (enable) {
		pm_pwm_enable(false);
		rc = pm_pwm_config(PWM_DUTY_US, PWM_PERIOD_US);
		if (rc < 0)
			mpp.mode = MPP_HIGH;
		else {
			mpp.mode = MPP_DTEST1;
			pm_pwm_enable(true);
		}
		pm8x41_config_output_mpp(&mpp);
		pm8x41_enable_mpp(&mpp, MPP_ENABLE);
	} else {
		pm_pwm_enable(false);
		pm8x41_enable_mpp(&mpp, MPP_DISABLE);
	}
	mdelay(20);

	if (board_hardware_subtype() == HW_PLATFORM_SUBTYPE_IOE)
		bkl_gpio.pin_id = 99;

	if (enable) {
		if (hw_id == HW_PLATFORM_SURF || (hw_id == HW_PLATFORM_MTP)) {
			/* configure backlight gpio for CDP and MTP */

			gpio_tlmm_config(bkl_gpio.pin_id, 0,
				bkl_gpio.pin_direction, bkl_gpio.pin_pull,
				bkl_gpio.pin_strength, bkl_gpio.pin_state);
				gpio_set(bkl_gpio.pin_id, 2);
		}
	}

	return 0;
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int32_t ret = 0;
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;
	pll_data->vco_delay = VCO_DELAY_USEC;

	if (enable) {
		mdp_gdsc_ctrl(enable);
		mdss_bus_clocks_enable();
		mdp_clock_enable();

		/*
		 * Enable auto functional gating
		 * on DSI CMD AXI fetch from DDR
		 */
		writel(0x3fbff, MDP_CGC_EN);
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
		mdss_dsi_uniphy_pll_sw_reset_8909(DSI0_PLL_BASE);
		mdss_dsi_auto_pll_config(DSI0_PLL_BASE,
						MIPI_DSI0_BASE, pll_data);
		if (!dsi_pll_enable_seq_8909(DSI0_PLL_BASE))
			dprintf(CRITICAL, "Not able to enable the pll\n");
		gcc_dsi_clocks_enable(pll_data->pclk_m,
				pll_data->pclk_n,
				pll_data->pclk_d);
	} else if(!target_cont_splash_screen()) {
		gcc_dsi_clocks_disable();
		mdp_clock_disable();
		mdss_bus_clocks_disable();
		mdp_gdsc_ctrl(enable);
	}

	return 0;
}

static int target_panel_reset_skuq(uint8_t enable)
{
	if (enable) {
		gpio_tlmm_config(enp_gpio.pin_id, 0,
			enp_gpio.pin_direction, enp_gpio.pin_pull,
			enp_gpio.pin_strength,
			enp_gpio.pin_state);
		gpio_set(enp_gpio.pin_id, 2);

		gpio_tlmm_config(enn_gpio.pin_id, 0,
			enn_gpio.pin_direction, enn_gpio.pin_pull,
			enn_gpio.pin_strength,
			enn_gpio.pin_state);
		gpio_set(enn_gpio.pin_id, 2);
	} else {
		gpio_set(enp_gpio.pin_id, 0); /* ENP */
		gpio_set(enn_gpio.pin_id, 0); /* ENN */
	}
	return 0;
}

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	uint32_t hw_id = board_hardware_id();
	uint32_t hw_subtype = board_hardware_subtype();
	uint32_t target_id = 0, plat_hw_ver_major = 0;

	if (enable) {
		if ((hw_id == HW_PLATFORM_QRD) && (hw_subtype == 0)) {
			target_id = board_target_id();
			plat_hw_ver_major = ((target_id >> 16) & 0xFF);
			if (plat_hw_ver_major == 6) {
				/* for SKUQ */
				target_panel_reset_skuq(enable);
			}
		}

		if (pinfo->mipi.use_enable_gpio) {
			gpio_tlmm_config(enable_gpio.pin_id, 0,
				enable_gpio.pin_direction, enable_gpio.pin_pull,
				enable_gpio.pin_strength,
				enable_gpio.pin_state);

			gpio_set(enable_gpio.pin_id, 2);
		}

		gpio_tlmm_config(reset_gpio.pin_id, 0,
				reset_gpio.pin_direction, reset_gpio.pin_pull,
				reset_gpio.pin_strength, reset_gpio.pin_state);

		gpio_set(reset_gpio.pin_id, 2);

		/* reset */
		for (int i = 0; i < RESET_GPIO_SEQ_LEN; i++) {
			if (resetseq->pin_state[i] == GPIO_STATE_LOW)
				gpio_set(reset_gpio.pin_id, GPIO_STATE_LOW);
			else
				gpio_set(reset_gpio.pin_id, GPIO_STATE_HIGH);
			mdelay(resetseq->sleep[i]);
		}
	} else if(!target_cont_splash_screen()) {
		gpio_set(reset_gpio.pin_id, 0);
		if (pinfo->mipi.use_enable_gpio)
			gpio_set(enable_gpio.pin_id, 0);

		if ((hw_id == HW_PLATFORM_QRD) && (hw_subtype == 0)) {
			target_id = board_target_id();
			plat_hw_ver_major = ((target_id >> 16) & 0xFF);
			if (plat_hw_ver_major == 6) {
				/* for SKUQ */
				target_panel_reset_skuq(enable);
			}
		}
	}

	return ret;
}

int target_ldo_ctrl(uint8_t enable)
{
	if (enable)
		regulator_enable();     /* L2, L6, and L17 */

	return NO_ERROR;
}

bool target_display_panel_node(char *panel_name, char *pbuf, uint16_t buf_size)
{
	return gcdb_display_cmdline_arg(panel_name, pbuf, buf_size);
}

void target_display_init(const char *panel_name)
{
	uint32_t panel_loop = 0;
	uint32_t ret = 0;

	panel_name += strspn(panel_name, " ");
	if (!strcmp(panel_name, NO_PANEL_CONFIG)
		|| !strcmp(panel_name, SIM_VIDEO_PANEL)
		|| !strcmp(panel_name, SIM_CMD_PANEL)) {
		dprintf(INFO, "Selected %s: Skip panel configuration\n",
				panel_name);
		return;
	}

	do {
		target_force_cont_splash_disable(false);
		ret = gcdb_display_init(panel_name, MDP_REV_305, MIPI_FB_ADDR);
		if (!ret || ret == ERR_NOT_SUPPORTED) {
			break;
		} else {
			target_force_cont_splash_disable(true);
			msm_display_off();
		}
	} while (++panel_loop <= oem_panel_max_auto_detect_panels());
}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
