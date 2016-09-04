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
#include <string.h>
#include <smem.h>
#include <err.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <mdss_hdmi.h>
#include <pm8x41.h>
#include <pm8x41_wled.h>
#include <qpnp_wled.h>
#include <board.h>
#include <mdp5.h>
#include <endian.h>
#include <regulator.h>
#include <qtimer.h>
#include <arch/defines.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <target/display.h>
#include <mipi_dsi_autopll_thulium.h>
#include <mipi_dsi_i2c.h>

#include "include/panel.h"
#include "include/display_resource.h"
#include "gcdb_display.h"

#define GPIO_STATE_LOW 0
#define GPIO_STATE_HIGH 2
#define RESET_GPIO_SEQ_LEN 3

#define PWM_DUTY_US 13
#define PWM_PERIOD_US 27
#define PMIC_WLED_SLAVE_ID 3
#define PMIC_MPP_SLAVE_ID 2

#define MAX_POLL_READS 15
#define POLL_TIMEOUT_US 1000

#define STRENGTH_SIZE_IN_BYTES_8996	10
#define REGULATOR_SIZE_IN_BYTES_8996	5
#define LANE_SIZE_IN_BYTES_8996		20

/*---------------------------------------------------------------------------*/
/* GPIO configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct gpio_pin reset_gpio = {
  "msmgpio", 8, 3, 1, 0, 1
};

static struct gpio_pin lcd_reg_en = {	/* boost regulator */
  "pmi8994_gpios", 8, 3, 1, 0, 1
};

static struct gpio_pin bklt_gpio = {	/* lcd_bklt_reg_en */
  "pm8994_gpios", 14, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio = {
  "msmgpio", 10, 3, 1, 0, 1
};

static struct gpio_pin dsi2hdmi_switch_gpio = {
  "msmgpio", 105, 3, 1, 0, 1
};

/* gpio name, id, strength, direction, pull, state. */
static struct gpio_pin hdmi_cec_gpio = {        /* CEC */
  "msmgpio", 31, 0, 2, 3, 1
};

static struct gpio_pin hdmi_ddc_clk_gpio = {   /* DDC CLK */
  "msmgpio", 32, 0, 2, 3, 1
};

static struct gpio_pin hdmi_ddc_data_gpio = {  /* DDC DATA */
  "msmgpio", 33, 0, 2, 3, 1
};

static struct gpio_pin hdmi_hpd_gpio = {       /* HPD, input */
  "msmgpio", 34, 7, 0, 1, 1
};

static void target_hdmi_ldo_enable(uint8_t enable)
{
	if (enable)
		regulator_enable(REG_LDO12);
	else
		regulator_disable(REG_LDO12);
}

static void target_hdmi_mpp4_enable(uint8_t enable)
{
	struct pm8x41_mpp mpp;

	/* Enable MPP4 */
	pmi8994_config_mpp_slave_id(0);

        mpp.base = PM8x41_MMP4_BASE;
	mpp.vin = MPP_VIN2;
	mpp.mode = MPP_HIGH;;
	if (enable) {
		pm8x41_config_output_mpp(&mpp);
		pm8x41_enable_mpp(&mpp, MPP_ENABLE);
	} else {
		pm8x41_enable_mpp(&mpp, MPP_DISABLE);
	}

	/* Need delay before power on regulators */
	mdelay(20);
}

int target_hdmi_regulator_ctrl(uint8_t enable)
{
	target_hdmi_ldo_enable(enable);

	target_hdmi_mpp4_enable(enable);

	return 0;
}

int target_hdmi_gpio_ctrl(uint8_t enable)
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

	return NO_ERROR;
}

int target_hdmi_pll_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
    if (enable) {
        hdmi_phy_reset();
        hdmi_pll_config(pinfo->clk_rate);
        hdmi_vco_enable();
        hdmi_pixel_clk_enable(pinfo->clk_rate);
    } else if(!target_cont_splash_screen()) {
        /* Disable clocks if continuous splash off */
        hdmi_pixel_clk_disable();
        hdmi_vco_disable();
    }

    return NO_ERROR;
}

int target_hdmi_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	dprintf(SPEW, "%s: target_panel_clock\n", __func__);

	uint32_t board_version = board_soc_version();

	if (board_version == 0x20000 || board_version == 0x20001)
		video_gdsc_enable();

	if (enable) {
		mmss_gdsc_enable();
		mmss_bus_clock_enable();
		mdp_clock_enable();
		hdmi_ahb_core_clk_enable();
	} else if(!target_cont_splash_screen()) {
		hdmi_core_ahb_clk_disable();
		mdp_clock_disable();
		mmss_bus_clock_disable();
		mmss_gdsc_disable();
		if (board_version == 0x20000 || board_version == 0x20001)
			video_gdsc_disable();
	}

	return NO_ERROR;
}

static uint32_t thulium_dsi_pll_lock_status(uint32_t pll_base, uint32_t off,
	uint32_t bit)
{
	uint32_t cnt, status;

	/* check pll lock first */
	for (cnt = 0; cnt < MAX_POLL_READS; cnt++) {
		status = readl(pll_base + off);
		dprintf(SPEW, "%s: pll_base=%x cnt=%d status=%x\n",
				__func__, pll_base, cnt, status);
		status &= BIT(bit); /* bit 5 */
		if (status)
			break;
		udelay(POLL_TIMEOUT_US);
	}

	return status;
}

static uint32_t thulium_dsi_pll_enable_seq(uint32_t phy_base, uint32_t pll_base)
{
	uint32_t pll_locked;

	writel(0x10, phy_base + 0x45c);
	writel(0x01, phy_base + 0x48);
	dmb();

	pll_locked = thulium_dsi_pll_lock_status(pll_base, 0xcc, 5);
	if (pll_locked)
		pll_locked = thulium_dsi_pll_lock_status(pll_base, 0xcc, 0);

	if (!pll_locked)
		dprintf(ERROR, "%s: DSI PLL lock failed\n", __func__);
	else
		dprintf(SPEW, "%s: DSI PLL lock Success\n", __func__);

	return  pll_locked;
}

static int thulium_wled_backlight_ctrl(uint8_t enable)
{
	qpnp_wled_enable_backlight(enable);
	return NO_ERROR;
}

static int thulium_pwm_backlight_ctrl(uint8_t enable)
{
	uint8_t slave_id = 3; /* lpg at pmi */

        if (enable) {
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

static void lcd_reg_enable(void)
{
	uint8_t slave_id = 2;	/* gpio at pmi */

	struct pm8x41_gpio gpio = {
                .direction = PM_GPIO_DIR_OUT,
                .function = PM_GPIO_FUNC_HIGH,
                .vin_sel = 2,   /* VIN_2 */
                .output_buffer = PM_GPIO_OUT_CMOS,
                .out_strength = PM_GPIO_OUT_DRIVE_MED,
        };

        pm8x41_gpio_config_sid(slave_id, lcd_reg_en.pin_id, &gpio);
	pm8x41_gpio_set_sid(slave_id, lcd_reg_en.pin_id, 1);
}

static void lcd_reg_disable(void)
{
	uint8_t slave_id = 2;	/* gpio at pmi */

	pm8x41_gpio_set_sid(slave_id, lcd_reg_en.pin_id, 0);
}

static void lcd_bklt_reg_enable(void)
{
       struct pm8x41_gpio gpio = {
                .direction = PM_GPIO_DIR_OUT,
                .function = PM_GPIO_FUNC_HIGH,
                .vin_sel = 2,   /* VIN_2 */
                .output_buffer = PM_GPIO_OUT_CMOS,
                .out_strength = PM_GPIO_OUT_DRIVE_LOW,
        };

        pm8x41_gpio_config(bklt_gpio.pin_id, &gpio);
	pm8x41_gpio_set(bklt_gpio.pin_id, 1);
}

static void lcd_bklt_reg_disable(void)
{
	pm8x41_gpio_set(bklt_gpio.pin_id, 0);
}

static int dsi2HDMI_i2c_write_regs(struct msm_panel_info *pinfo,
	struct mipi_dsi_i2c_cmd *cfg, int size)
{
	int ret = NO_ERROR;
	int i;
	uint8_t addr;

	if (!cfg || size < 0) {
		dprintf(CRITICAL, "Invalid input: register array is null\n");
		return ERR_INVALID_ARGS;
	}

	for (i = 0; i < size; i++) {
		switch (cfg[i].i2c_addr) {
			case ADV7533_MAIN:
				addr = pinfo->adv7533.i2c_main_addr;
				break;
			case ADV7533_CEC_DSI:
				addr = pinfo->adv7533.i2c_cec_addr;
				break;
			default:
				dprintf(CRITICAL, "Invalid I2C addr in array\n");
				ret = ERR_INVALID_ARGS;
				goto w_regs_fail;
		}

		ret = mipi_dsi_i2c_write_byte(addr, cfg[i].reg,
			cfg[i].val);
		if (ret) {
			dprintf(CRITICAL, "mipi_dsi reg writes failed\n");
			goto w_regs_fail;
		}
		if (cfg[i].sleep_in_ms) {
			udelay(cfg[i].sleep_in_ms*1000);
		}
	}
w_regs_fail:
	return ret;
}

int target_display_dsi2hdmi_program_addr(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	uint8_t i2c_8bits = pinfo->adv7533.i2c_cec_addr << 1;
	ret = mipi_dsi_i2c_write_byte(pinfo->adv7533.i2c_main_addr,
				0xE1, i2c_8bits);
	if (ret) {
		dprintf(CRITICAL, "Error in programming CEC DSI addr\n");
	} else {
		dprintf(SPEW, "CEC address programming successful\n");
	}
	return ret;
}

int target_display_dsi2hdmi_config(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;

	if (!pinfo) {
		dprintf(CRITICAL, "Invalid input: pinfo is null\n");
		return ERR_INVALID_ARGS;
	}

	if (!pinfo->adv7533.program_i2c_addr) {
		ret = target_display_dsi2hdmi_program_addr(pinfo);
		if (ret) {
			dprintf(CRITICAL, "Error in programming cec dsi addr\n");
			return ret;
		} else {
			dprintf(SPEW, "successfully programmed cec dsi addr\n");
			pinfo->adv7533.program_i2c_addr = 1;
		}
	}

	/*
	 * If dsi to HDMI bridge chip connected then
	 * send I2c commands to the chip
	 */
	if (pinfo->adv7533.dsi_setup_cfg_i2c_cmd)
		ret = dsi2HDMI_i2c_write_regs(pinfo, pinfo->adv7533.dsi_setup_cfg_i2c_cmd,
					pinfo->adv7533.num_of_cfg_i2c_cmds);
	if (ret) {
		dprintf(CRITICAL, "Error in writing adv7533 setup registers\n");
		return ret;
	}

	if (pinfo->adv7533.dsi_tg_i2c_cmd)
		ret = dsi2HDMI_i2c_write_regs(pinfo, pinfo->adv7533.dsi_tg_i2c_cmd,
					pinfo->adv7533.num_of_tg_i2c_cmds);
	if (ret) {
		dprintf(CRITICAL, "Error in writing adv7533 timing registers\n");
	}

	return ret;
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
		/* Enable WLED backlight control */
		ret = thulium_wled_backlight_ctrl(enable);
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
		ret = thulium_pwm_backlight_ctrl(enable);
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
	uint32_t flags, dsi_phy_pll_out;
	uint32_t ret = NO_ERROR;
	uint32_t board_version = board_soc_version();
	uint32_t board_hw_id = board_hardware_id();
	bool video_core_enable = false;
	struct dfps_pll_codes *pll_codes = &pinfo->mipi.pll_codes;

	if (pinfo->dest == DISPLAY_2) {
		flags = MMSS_DSI_CLKS_FLAG_DSI1;
		if (pinfo->mipi.dual_dsi)
			flags |= MMSS_DSI_CLKS_FLAG_DSI0;
	} else {
		flags = MMSS_DSI_CLKS_FLAG_DSI0;
		if (pinfo->mipi.dual_dsi)
			flags |= MMSS_DSI_CLKS_FLAG_DSI1;
	}

	/* only required for msm8996 v2 and v2.1 revision */
	video_core_enable = (board_version == 0x20000 || board_version == 0x20001) &&
		!(board_hw_id == MSM8996SG || board_hw_id == APQ8096SG);

	if (!enable) {
		mmss_dsi_clock_disable(flags);

		/* stop pll */
		writel(0x0, pinfo->mipi.phy_base + 0x48);
		dmb();

		goto clks_disable;
	}

	if (video_core_enable)
		video_gdsc_enable();
	mmss_gdsc_enable();
	mmss_bus_clock_enable();
	mdp_clock_enable();
	mdss_dsi_auto_pll_thulium_config(pinfo);

	if (!thulium_dsi_pll_enable_seq(pinfo->mipi.phy_base,
		pinfo->mipi.pll_base)) {
		ret = ERROR;
		dprintf(CRITICAL, "PLL failed to lock!\n");
		goto clks_disable;
	}

	pll_codes->codes[0] = readl_relaxed(pinfo->mipi.pll_base +
			        MMSS_DSI_PHY_PLL_CORE_KVCO_CODE);
	pll_codes->codes[1] = readl_relaxed(pinfo->mipi.pll_base +
			        MMSS_DSI_PHY_PLL_CORE_VCO_TUNE);
	dprintf(SPEW, "codes %d %d\n", pll_codes->codes[0],
			        pll_codes->codes[1]);

	if (pinfo->mipi.use_dsi1_pll)
		dsi_phy_pll_out = DSI1_PHY_PLL_OUT;
	else
		dsi_phy_pll_out = DSI0_PHY_PLL_OUT;
	mmss_dsi_clock_enable(dsi_phy_pll_out, flags);

	return NO_ERROR;

clks_disable:
	mdp_clock_disable();
	mmss_bus_clock_disable();
	mmss_gdsc_disable();
	if (video_core_enable)
		video_gdsc_disable();

	return ret;
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

static void wled_init(struct msm_panel_info *pinfo)
{
	struct qpnp_wled_config_data config = {0};
	struct labibb_desc *labibb;
	int display_type = 0;

	labibb = pinfo->labibb;

	if (labibb)
		display_type = labibb->amoled_panel;

	config.display_type = display_type;
	config.lab_init_volt = 4600000;	/* fixed, see pmi register */
	config.ibb_init_volt = 1400000;	/* fixed, see pmi register */

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

	dprintf(SPEW, "%s: %d %d %d %d %d %d %d %d %d %d\n", __func__,
		config.display_type,
		config.lab_min_volt, config.lab_max_volt,
		config.ibb_min_volt, config.ibb_max_volt,
		config.lab_init_volt, config.ibb_init_volt,
		config.pwr_up_delay, config.pwr_down_delay,
		config.ibb_discharge_en);


	/* QPNP WLED init for display backlight */
	pm8x41_wled_config_slave_id(PMIC_WLED_SLAVE_ID);

	qpnp_wled_init(&config);
}

int target_ldo_ctrl(uint8_t enable, struct msm_panel_info *pinfo)
{
	uint32_t val = BIT(1) | BIT(13) | BIT(27);

	if (enable) {
		regulator_enable(val);
		mdelay(10);
		wled_init(pinfo);
		qpnp_ibb_enable(true);	/* +5V and -5V */
		mdelay(20);

		if (pinfo->lcd_reg_en)
			lcd_reg_enable();
	} else {
		if (pinfo->lcd_reg_en)
			lcd_reg_disable();

		regulator_disable(val);
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

int target_dsi_phy_config(struct mdss_dsi_phy_ctrl *phy_db)
{
	memcpy(phy_db->strength, panel_strength_ctrl, STRENGTH_SIZE_IN_BYTES_8996 *
		sizeof(uint32_t));
	memcpy(phy_db->regulator, panel_regulator_settings,
		REGULATOR_SIZE_IN_BYTES_8996 * sizeof(uint32_t));
	memcpy(phy_db->laneCfg, panel_lane_config, LANE_SIZE_IN_BYTES_8996);
	return NO_ERROR;
}

bool target_display_panel_node(char *pbuf, uint16_t buf_size)
{
	int prefix_string_len = strlen(DISPLAY_CMDLINE_PREFIX);
	bool ret = true;
	struct oem_panel_data oem = mdss_dsi_get_oem_data();
	char vic_buf[HDMI_VIC_LEN] = "0";

	if ((!strcmp(oem.panel, HDMI_PANEL_NAME)) || \
		((!strlen(oem.panel)) && (platform_is_apq8096_mediabox()))) {
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
		buf_size -= strlen(HDMI_CONTROLLER_STRING);
		mdss_hdmi_get_vic(vic_buf);
		strlcat(pbuf, vic_buf, buf_size);
	} else {
		ret = gcdb_display_cmdline_arg(pbuf, buf_size);
	}

	return ret;
}

void target_set_switch_gpio(int enable_dsi2hdmibridge)
{
	gpio_tlmm_config(dsi2hdmi_switch_gpio.pin_id, 0,
				dsi2hdmi_switch_gpio.pin_direction,
				dsi2hdmi_switch_gpio.pin_pull,
				dsi2hdmi_switch_gpio.pin_strength,
				dsi2hdmi_switch_gpio.pin_state);
	gpio_set(enable_gpio.pin_id, GPIO_STATE_HIGH);
	if (enable_dsi2hdmibridge)
		gpio_set(enable_gpio.pin_id, GPIO_STATE_LOW); /* DSI2HDMI Bridge */
	else
		gpio_set(enable_gpio.pin_id, GPIO_STATE_HIGH); /* Normal DSI operation */
}

void target_display_init(const char *panel_name)
{
	struct oem_panel_data oem;

	set_panel_cmd_string(panel_name);
	oem = mdss_dsi_get_oem_data();
	if (!strcmp(oem.panel, NO_PANEL_CONFIG)
		|| !strcmp(oem.panel, SIM_VIDEO_PANEL)
		|| !strcmp(oem.panel, SIM_DUALDSI_VIDEO_PANEL)
		|| !strcmp(oem.panel, SIM_CMD_PANEL)
		|| !strcmp(oem.panel, SIM_DUALDSI_CMD_PANEL)
		|| oem.skip) {
		dprintf(INFO, "Selected panel: %s\nSkip panel configuration\n",
			oem.panel);
		return;
	} else if ((!strcmp(oem.panel, HDMI_PANEL_NAME)) || \
		((!strlen(oem.panel)) && (platform_is_apq8096_mediabox()))) {
		dprintf(INFO, "%s: HDMI is primary\n", __func__);
		mdss_hdmi_display_init(MDP_REV_50, (void *) HDMI_FB_ADDR);
		return;
	}

	if (gcdb_display_init(oem.panel, MDP_REV_50, (void *)MIPI_FB_ADDR)) {
		target_force_cont_splash_disable(true);
		msm_display_off();
	}

	if (!oem.cont_splash) {
		dprintf(INFO, "Forcing continuous splash disable\n");
		target_force_cont_splash_disable(true);
	}
}

void target_display_shutdown(void)
{
	struct oem_panel_data oem = mdss_dsi_get_oem_data();
	if ((!strcmp(oem.panel, HDMI_PANEL_NAME)) || \
		((!strlen(oem.panel)) && (platform_is_apq8096_mediabox()))) {
		msm_display_off();
	} else {
		gcdb_display_shutdown();
	}
}
