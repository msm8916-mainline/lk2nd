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

#define DDR_LDO_ID 2
#define QFPROM_LDO_ID 6

#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3
#define PWM_DUTY_US 13
#define PWM_PERIOD_US 27

static uint32_t dsi_pll_enable_seq_m(uint32_t pll_base)
{
	uint32_t i = 0;
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);
	pll_locked = readl(pll_base + 0x00c0) & 0x01;
	for (i = 0; (i < 4) && !pll_locked; i++) {
		writel(0x07, pll_base + 0x0020); /* GLB CFG */
		if (i != 0)
			writel(0x34, pll_base + 0x00070); /* CAL CFG1*/
		udelay(1);
		writel(0x0f, pll_base + 0x0020); /* GLB CFG */
		udelay(1000);
		mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);
		pll_locked = readl(pll_base + 0x00c0) & 0x01;
	}

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_d(uint32_t pll_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x07, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x07, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);
	pll_locked = readl(pll_base + 0x00c0) & 0x01;

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_f1(uint32_t pll_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0d, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);
	pll_locked = readl(pll_base + 0x00c0) & 0x01;

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_c(uint32_t pll_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);
	pll_locked = readl(pll_base + 0x00c0) & 0x01;

	return pll_locked;
}

static uint32_t dsi_pll_enable_seq_e(uint32_t pll_base)
{
	uint32_t pll_locked = 0;

	mdss_dsi_uniphy_pll_sw_reset(pll_base);

	/*
	 * Add hardware recommended delays between register writes for
	 * the updates to take effect. These delays are necessary for the
	 * PLL to successfully lock
	 */
	writel(0x01, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x05, pll_base + 0x0020); /* GLB CFG */
	udelay(200);
	writel(0x0d, pll_base + 0x0020); /* GLB CFG */
	udelay(1);
	writel(0x0f, pll_base + 0x0020); /* GLB CFG */
	udelay(1000);

	mdss_dsi_uniphy_pll_lock_detect_setting(pll_base);
	pll_locked = readl(pll_base + 0x00c0) & 0x01;

	return pll_locked;
}

int target_backlight_ctrl(struct backlight *bl, uint8_t enable)
{
	struct pm8x41_mpp mpp;
	int rc;

	mpp.base = PM8x41_MMP4_BASE;
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
	return 0;
}

static void dsi_pll_enable_seq(uint32_t pll_base)
{
	if (dsi_pll_enable_seq_m(pll_base)) {
	} else if (dsi_pll_enable_seq_d(pll_base)) {
	} else if (dsi_pll_enable_seq_d(pll_base)) {
	} else if (dsi_pll_enable_seq_f1(pll_base)) {
	} else if (dsi_pll_enable_seq_c(pll_base)) {
	} else if (dsi_pll_enable_seq_e(pll_base)) {
	} else {
		dprintf(CRITICAL, "Not able to enable the pll\n");
	}
}

int target_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int32_t ret = 0;
	struct mdss_dsi_pll_config *pll_data;
	dprintf(SPEW, "target_panel_clock\n");

	pll_data = pinfo->mipi.dsi_pll_config;

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
		mdss_dsi_auto_pll_config(DSI0_PLL_BASE,
						MIPI_DSI0_BASE, pll_data);
		dsi_pll_enable_seq(DSI0_PLL_BASE);
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

int target_panel_reset(uint8_t enable, struct panel_reset_sequence *resetseq,
						struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	uint32_t hw_id = board_hardware_id();
	uint32_t hw_subtype = board_hardware_subtype();

	if (enable) {
		if (pinfo->mipi.use_enable_gpio) {
			gpio_tlmm_config(enable_gpio.pin_id, 0,
				enable_gpio.pin_direction, enable_gpio.pin_pull,
				enable_gpio.pin_strength,
				enable_gpio.pin_state);

			gpio_set_dir(enable_gpio.pin_id, 2);
		}

		if(hw_id == HW_PLATFORM_QRD &&
			hw_subtype == HW_PLATFORM_SUBTYPE_SKUH) {
			/* for tps65132 ENP */
			gpio_tlmm_config(enp_gpio.pin_id, 0,
				enp_gpio.pin_direction, enp_gpio.pin_pull,
				enp_gpio.pin_strength,
				enp_gpio.pin_state);
			gpio_set_dir(enp_gpio.pin_id, 2);

			/* for tps65132 ENN */
			gpio_tlmm_config(enn_gpio.pin_id, 0,
				enn_gpio.pin_direction, enn_gpio.pin_pull,
				enn_gpio.pin_strength,
				enn_gpio.pin_state);
			gpio_set_dir(enn_gpio.pin_id, 2);
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
	} else if(!target_cont_splash_screen()) {
		gpio_set_dir(reset_gpio.pin_id, 0);
		if (pinfo->mipi.use_enable_gpio)
			gpio_set_dir(enable_gpio.pin_id, 0);

		if(hw_id == HW_PLATFORM_QRD &&
			hw_subtype == HW_PLATFORM_SUBTYPE_SKUH) {
			gpio_set_dir(enp_gpio.pin_id, 0); /* ENP */
			gpio_set_dir(enn_gpio.pin_id, 0); /* ENN */
		}
	}

	return ret;
}

int target_ldo_ctrl(uint8_t enable)
{
	uint32_t ret = NO_ERROR;
	uint32_t ldocounter = 0;
	uint32_t pm8x41_ldo_base = 0x13F00;

	while (ldocounter < TOTAL_LDO_DEFINED) {
		dprintf(SPEW, "Setting %i\n",
				ldo_entry_array[ldocounter].ldo_id);
		struct pm8x41_ldo ldo_entry = LDO((pm8x41_ldo_base +
			0x100 * ldo_entry_array[ldocounter].ldo_id),
			ldo_entry_array[ldocounter].ldo_type);


		/* Set voltage during power on */
		if (enable) {
			/* TODO: Set the LDO voltage before enabling it */
			pm8x41_ldo_control(&ldo_entry, enable);

		} else if(!target_cont_splash_screen() &&
			ldo_entry_array[ldocounter].ldo_id != DDR_LDO_ID &&
			ldo_entry_array[ldocounter].ldo_id != QFPROM_LDO_ID) {
			pm8x41_ldo_control(&ldo_entry, enable);
		}
		ldocounter++;
	}

	return ret;
}

bool target_display_panel_node(char *panel_name, char *pbuf, uint16_t buf_size)
{
	return gcdb_display_cmdline_arg(pbuf, buf_size);
}

void target_display_init(const char *panel_name)
{
	uint32_t panel_loop = 0;
	uint32_t ret = 0;
	do {
		ret = gcdb_display_init(panel_name, MDP_REV_50, MIPI_FB_ADDR);
		if (!ret || ret == ERR_NOT_SUPPORTED) {
			break;
		} else {
			target_force_cont_splash_disable(true);
			msm_display_off();
			target_force_cont_splash_disable(false);
		}
	} while (++panel_loop <= oem_panel_max_auto_detect_panels());
}

void target_display_shutdown(void)
{
	gcdb_display_shutdown();
}
