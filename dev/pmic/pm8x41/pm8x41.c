/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
 */

#include <bits.h>
#include <debug.h>
#include <reg.h>
#include <spmi.h>
#include <string.h>
#include <pm8x41_hw.h>
#include <pm8x41.h>
#include <platform/timer.h>

static uint8_t mpp_slave_id;

uint8_t pmi8994_config_mpp_slave_id(uint8_t slave_id)
{
	mpp_slave_id = slave_id;
}

/* SPMI helper functions */
uint8_t pm8x41_reg_read(uint32_t addr)
{
	uint8_t val = 0;
	struct pmic_arb_cmd cmd;
	struct pmic_arb_param param;

	cmd.address  = PERIPH_ID(addr);
	cmd.offset   = REG_OFFSET(addr);
	cmd.slave_id = SLAVE_ID(addr);
	cmd.priority = 0;

	param.buffer = &val;
	param.size   = 1;

	pmic_arb_read_cmd(&cmd, &param);

	return val;
}

void pm8x41_reg_write(uint32_t addr, uint8_t val)
{
	struct pmic_arb_cmd cmd;
	struct pmic_arb_param param;

	cmd.address  = PERIPH_ID(addr);
	cmd.offset   = REG_OFFSET(addr);
	cmd.slave_id = SLAVE_ID(addr);
	cmd.priority = 0;

	param.buffer = &val;
	param.size   = 1;

	pmic_arb_write_cmd(&cmd, &param);
}

/* Exported functions */

/* Set the boot done flag */
void pm8x41_set_boot_done()
{
	uint8_t val;

	val  = REG_READ(SMBB_MISC_BOOT_DONE);
	val |= BIT(BOOT_DONE_BIT);
	REG_WRITE(SMBB_MISC_BOOT_DONE, val);
}

/* Configure GPIO */
int pm8x41_gpio_config(uint8_t gpio, struct pm8x41_gpio *config)
{
	uint8_t  val;
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);

	/* Disable the GPIO */
	val  = REG_READ(gpio_base + GPIO_EN_CTL);
	val &= ~BIT(PERPH_EN_BIT);
	REG_WRITE(gpio_base + GPIO_EN_CTL, val);

	/* Select the mode */
	val = config->function | (config->direction << 4);
	REG_WRITE(gpio_base + GPIO_MODE_CTL, val);

	/* Set the right pull */
	val = config->pull;
	REG_WRITE(gpio_base + GPIO_DIG_PULL_CTL, val);

	/* Select the VIN */
	val = config->vin_sel;
	REG_WRITE(gpio_base + GPIO_DIG_VIN_CTL, val);

	if (config->direction == PM_GPIO_DIR_OUT) {
		/* Set the right dig out control */
		val = config->out_strength | (config->output_buffer << 4);
		REG_WRITE(gpio_base + GPIO_DIG_OUT_CTL, val);
	}

	/* Enable the GPIO */
	val  = REG_READ(gpio_base + GPIO_EN_CTL);
	val |= BIT(PERPH_EN_BIT);
	REG_WRITE(gpio_base + GPIO_EN_CTL, val);

	return 0;
}

/* Reads the status of requested gpio */
int pm8x41_gpio_get(uint8_t gpio, uint8_t *status)
{
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);

	*status = REG_READ(gpio_base + GPIO_STATUS);

	/* Return the value of the GPIO pin */
	*status &= BIT(GPIO_STATUS_VAL_BIT);

	dprintf(SPEW, "GPIO %d status is %d\n", gpio, *status);

	return 0;
}

/* Write the output value of the requested gpio */
int pm8x41_gpio_set(uint8_t gpio, uint8_t value)
{
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);
	uint8_t val;

	/* Set the output value of the gpio */
	val = REG_READ(gpio_base + GPIO_MODE_CTL);
	val = (val & ~PM_GPIO_OUTPUT_MASK) | value;
	REG_WRITE(gpio_base + GPIO_MODE_CTL, val);

	return 0;
}

/* Configure PM and PMI GPIO with slave id */
int pm8x41_gpio_config_sid(uint8_t sid, uint8_t gpio, struct pm8x41_gpio *config)
{
	uint8_t  val;
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);

	gpio_base &= 0x0ffff;	/* clear sid */
	gpio_base |= (sid << 16);	/* add sid */

	dprintf(SPEW, "%s: gpio=%d base=%x\n", __func__, gpio, gpio_base);

	/* Disable the GPIO */
	val  = REG_READ(gpio_base + GPIO_EN_CTL);
	val &= ~BIT(PERPH_EN_BIT);
	REG_WRITE(gpio_base + GPIO_EN_CTL, val);

	/* Select the mode */
	val = config->function | (config->direction << 4);
	REG_WRITE(gpio_base + GPIO_MODE_CTL, val);

	/* Set the right pull */
	val = config->pull;
	REG_WRITE(gpio_base + GPIO_DIG_PULL_CTL, val);

	/* Select the VIN */
	val = config->vin_sel;
	REG_WRITE(gpio_base + GPIO_DIG_VIN_CTL, val);

	if (config->direction == PM_GPIO_DIR_OUT) {
		/* Set the right dig out control */
		val = config->out_strength | (config->output_buffer << 4);
		REG_WRITE(gpio_base + GPIO_DIG_OUT_CTL, val);
	}

	/* Enable the GPIO */
	val  = REG_READ(gpio_base + GPIO_EN_CTL);
	val |= BIT(PERPH_EN_BIT);
	REG_WRITE(gpio_base + GPIO_EN_CTL, val);

	return 0;
}

/* Reads the status of requested gpio */
int pm8x41_gpio_get_sid(uint8_t sid, uint8_t gpio, uint8_t *status)
{
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);

	gpio_base &= 0x0ffff;	/* clear sid */
	gpio_base |= (sid << 16);	/* add sid */

	*status = REG_READ(gpio_base + GPIO_STATUS);

	/* Return the value of the GPIO pin */
	*status &= BIT(GPIO_STATUS_VAL_BIT);

	dprintf(SPEW, "GPIO %d status is %d\n", gpio, *status);

	return 0;
}

/* Write the output value of the requested gpio */
int pm8x41_gpio_set_sid(uint8_t sid, uint8_t gpio, uint8_t value)
{
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);
	uint8_t val;

	gpio_base &= 0x0ffff;	/* clear sid */
	gpio_base |= (sid << 16);	/* add sid */

	dprintf(SPEW, "%s: gpio=%d base=%x\n", __func__, gpio, gpio_base);

	/* Set the output value of the gpio */
	val = REG_READ(gpio_base + GPIO_MODE_CTL);
	val = (val & ~PM_GPIO_OUTPUT_MASK) | value;
	REG_WRITE(gpio_base + GPIO_MODE_CTL, val);

	return 0;
}

/* Prepare PON RESIN S2 reset (bite) */
void pm8x41_resin_s2_reset_enable()
{
	uint8_t val;

	/* disable s2 reset */
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);

	/* configure s1 timer to 0 */
	REG_WRITE(PON_RESIN_N_RESET_S1_TIMER, 0x0);

	/* configure s2 timer to 2s */
	REG_WRITE(PON_RESIN_N_RESET_S2_TIMER, PON_RESIN_N_RESET_S2_TIMER_MAX_VALUE);

	/* configure reset type */
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, S2_RESET_TYPE_WARM);

	val = REG_READ(PON_RESIN_N_RESET_S2_CTL);

	/* enable s2 reset */
	val |= BIT(S2_RESET_EN_BIT);
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, val);
}

/* Disable PON RESIN S2 reset. (bite)*/
void pm8x41_resin_s2_reset_disable()
{
	/* disable s2 reset */
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);
}

/* Resin irq status for faulty pmic*/
uint32_t pm8x41_v2_resin_status()
{
	uint8_t rt_sts = 0;

	/* Enable S2 reset so we can detect the volume down key press */
	pm8x41_resin_s2_reset_enable();

	/* Delay before interrupt triggering.
	 * See PON_DEBOUNCE_CTL reg.
	 */
	mdelay(100);

	rt_sts = REG_READ(PON_INT_RT_STS);

	/* Must disable S2 reset otherwise PMIC will reset if key
	 * is held longer than S2 timer.
	 */
	pm8x41_resin_s2_reset_disable();

	return (rt_sts & BIT(RESIN_BARK_INT_BIT));
}

/* Resin pin status */
uint32_t pm8x41_resin_status()
{
	uint8_t rt_sts = 0;

	rt_sts = REG_READ(PON_INT_RT_STS);

	return (rt_sts & BIT(RESIN_ON_INT_BIT));
}

/* Return 1 if power key is pressed */
uint32_t pm8x41_get_pwrkey_is_pressed()
{
	uint8_t pwr_sts = 0;

	pwr_sts = REG_READ(PON_INT_RT_STS);

	if (pwr_sts & BIT(KPDPWR_ON_INT_BIT))
		return 1;
	else
		return 0;
}

void pm8x41_v2_reset_configure(uint8_t reset_type)
{
	uint8_t val;

	/* disable PS_HOLD_RESET */
	REG_WRITE(PON_PS_HOLD_RESET_CTL, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);

	/* configure reset type */
	REG_WRITE(PON_PS_HOLD_RESET_CTL, reset_type);

	val = REG_READ(PON_PS_HOLD_RESET_CTL);

	/* enable PS_HOLD_RESET */
	val |= BIT(S2_RESET_EN_BIT);
	REG_WRITE(PON_PS_HOLD_RESET_CTL, val);
}

void pm8x41_reset_configure(uint8_t reset_type)
{
	/* disable PS_HOLD_RESET */
	REG_WRITE(PON_PS_HOLD_RESET_CTL2, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);

	/* configure reset type */
	REG_WRITE(PON_PS_HOLD_RESET_CTL, reset_type);

	/* enable PS_HOLD_RESET */
	REG_WRITE(PON_PS_HOLD_RESET_CTL2, BIT(S2_RESET_EN_BIT));
}

/*
 * LDO set voltage, takes ldo name & voltage in UV as input
 */
int pm8x41_ldo_set_voltage(struct pm8x41_ldo *ldo, uint32_t voltage)
{
	uint32_t range = 0;
	uint32_t step = 0;
	uint32_t mult = 0;
	uint32_t val = 0;
	uint32_t vmin = 0;

	if (!ldo)
	{
		dprintf(CRITICAL, "LDO pointer is invalid: %p\n", ldo);
		return 1;
	}

	/* Program Normal power mode */
	val = 0x0;
	val = (1 << LDO_NORMAL_PWR_BIT);
	REG_WRITE((ldo->base + LDO_POWER_MODE), val);

	/*
	 * Select range, step & vmin based on input voltage & type of LDO
	 * LDO can operate in low, mid, high power mode
	 */
	if (ldo->type == PLDO_TYPE)
	{
		if (voltage < PLDO_UV_MIN)
		{
			range = 2;
			step = PLDO_UV_STEP_LOW;
			vmin = PLDO_UV_VMIN_LOW;
		}
		else if (voltage < PDLO_UV_MID)
		{
			range = 3;
			step = PLDO_UV_STEP_MID;
			vmin = PLDO_UV_VMIN_MID;
		}
		else
		{
			range = 4;
			step = PLDO_UV_STEP_HIGH;
			vmin = PLDO_UV_VMIN_HIGH;
		}
	}
	else
	{
		range = 2;
		step = NLDO_UV_STEP;
		vmin = NLDO_UV_VMIN_LOW;
	}

	mult = (voltage - vmin) / step;

	/* Set Range in voltage ctrl register */
	val = 0x0;
	val = range << LDO_RANGE_SEL_BIT;
	REG_WRITE((ldo->base + LDO_RANGE_CTRL), val);

	/* Set multiplier in voltage ctrl register */
	val = 0x0;
	val = mult << LDO_VSET_SEL_BIT;
	REG_WRITE((ldo->base + LDO_STEP_CTRL), val);

	return 0;
}

/*
 * Enable or Disable LDO
 */
int pm8x41_ldo_control(struct pm8x41_ldo *ldo, uint8_t enable)
{
	uint32_t val = 0;

	if (!ldo)
	{
		dprintf(CRITICAL, "LDO pointer is invalid: %p\n", ldo);
		return 1;
	}

	/* Enable LDO */
	if (enable)
		val = (1 << LDO_VREG_ENABLE_BIT);
	else
		val = (0 << LDO_VREG_ENABLE_BIT);

	REG_WRITE((ldo->base + LDO_EN_CTL_REG), val);

	return 0;
}

/*
 * lpg channel register write:
 */
void pm8x41_lpg_write(uint8_t chan, uint8_t off, uint8_t val)
{
	uint32_t lpg_base = LPG_N_PERIPHERAL_BASE(chan);

	REG_WRITE(lpg_base + off, val);
}

/*
 * pmi lpg channel register write with slave_id:
 */
void pm8x41_lpg_write_sid(uint8_t sid, uint8_t chan, uint8_t off, uint8_t val)
{
	uint32_t lpg_base = LPG_N_PERIPHERAL_BASE(chan);

	lpg_base &= 0x0ffff;	/* clear sid */
	lpg_base |= (sid << 16);	/* add sid */

	dprintf(SPEW, "%s: lpg=%d base=%x\n", __func__, chan, lpg_base);

	REG_WRITE(lpg_base + off, val);
}

uint8_t pm8x41_get_pmic_rev()
{
	return REG_READ(REVID_REVISION4);
}

uint8_t pm8x41_get_pon_reason()
{
	return REG_READ(PON_PON_REASON1);
}

uint8_t pm8x41_get_pon_poff_reason1()
{
	return REG_READ(PON_POFF_REASON1);
}

uint8_t pm8x41_get_pon_poff_reason2()
{
	return REG_READ(PON_POFF_REASON2);
}

void pm8x41_enable_mvs(struct pm8x41_mvs *mvs, enum mvs_en_ctl enable)
{
	ASSERT(mvs);

	REG_WRITE(mvs->base + MVS_EN_CTL, enable << MVS_EN_CTL_ENABLE_SHIFT);
}

void pm8x41_enable_mpp(struct pm8x41_mpp *mpp, enum mpp_en_ctl enable)
{
	ASSERT(mpp);

	REG_WRITE(((mpp->base + MPP_EN_CTL) + (mpp_slave_id << 16)), enable << MPP_EN_CTL_ENABLE_SHIFT);
}

void pm8x41_config_output_mpp(struct pm8x41_mpp *mpp)
{
	ASSERT(mpp);

	REG_WRITE(((mpp->base + MPP_DIG_VIN_CTL) + (mpp_slave_id << 16)), mpp->vin);

	REG_WRITE(((mpp->base + MPP_MODE_CTL) + (mpp_slave_id << 16)), mpp->mode | (MPP_DIGITAL_OUTPUT << MPP_MODE_CTL_MODE_SHIFT));
}

uint8_t pm8x41_get_is_cold_boot()
{
	if (REG_READ(PON_WARMBOOT_STATUS1) || REG_READ(PON_WARMBOOT_STATUS2)) {
		dprintf(INFO,"%s: Warm boot\n", __func__);
		return 0;
	}
	dprintf(INFO,"%s: cold boot\n", __func__);
	return 1;
}

/* api to control lnbb clock */
void pm8x41_lnbb_clock_ctrl(uint8_t enable)
{
	uint8_t reg;

	reg = REG_READ(LNBB_CLK_EN_CTL);

	if (enable)
	{
		reg |= BIT(LNBB_CLK_EN_BIT);
	}
	else
	{
		reg &= ~BIT(LNBB_CLK_EN_BIT);
	}

	REG_WRITE(LNBB_CLK_EN_CTL, reg);
}

/* api to control diff clock */
void pm8x41_diff_clock_ctrl(uint8_t enable)
{
	uint8_t reg;

	reg = REG_READ(DIFF_CLK1_EN_CTL);

	if (enable)
	{
		reg |= BIT(DIFF_CLK1_EN_BIT);
	}
	else
	{
		reg &= ~BIT(DIFF_CLK1_EN_BIT);
	}

	REG_WRITE(DIFF_CLK1_EN_CTL, reg);
}

void pm8x41_clear_pmic_watchdog(void)
{
	pm8x41_reg_write(PMIC_WD_RESET_S2_CTL2, 0x0);
}

/* API to check for borken battery */
int pm8xxx_is_battery_broken()
{
	uint8_t trkl_default = 0;
	uint8_t vbat_det_default = 0;
	int batt_is_broken = 0;

	/* Store original trickle charging current setting */
	trkl_default = pm8x41_reg_read(PM8XXX_IBAT_ATC_A);
	/* Store original VBAT_DET_LO setting */
	vbat_det_default = pm8x41_reg_read(PM8XXX_VBAT_DET);

	/*Set trickle charge current to 50mA (IBAT_ATC_A = 0x00) */
	pm8x41_reg_write(PM8XXX_IBAT_ATC_A, 0x00);
	/* Set VBAT_DET_LO to 4.3V so that VBAT_DET_HI = 4.52V (VBAT_DET_LO = 0x35) */
	pm8x41_reg_write(PM8XXX_VBAT_DET, VBAT_DET_LO_4_30V);
	/* Unlock SMBBP Secured Register */
	pm8x41_reg_write(PM8XXX_SEC_ACCESS, SEC_ACCESS);
	/* Disable VTRKL_FAULT comp (SMBBP_CHGR_COMP_OVR0 = 0x08) */
	pm8x41_reg_write(PM8XXX_COMP_OVR0, OVR0_DIS_VTRKL_FAULT);
	/* Disable VCP (SMBB_BAT_IF_VCP = 0x00) */
	pm8x41_reg_write(PM8XXX_VCP, 0x00);
	/* Unlock SMBBP Secured Register */
	pm8x41_reg_write(PM8XXX_SEC_ACCESS, SEC_ACCESS);
	/* Force trickle charging (SMBB_CHGR_TRKL_CHG_TEST = 0x01) */
	pm8x41_reg_write(PM8XXX_TRKL_CHG_TEST, CHG_TRICKLE_FORCED_ON);
	/* Wait for vbat to rise */
	mdelay(12);

	/* Check Above VBAT_DET_HIGH status */
	if (pm8x41_reg_read(PM8XXX_VBAT_IN_TSTS) & VBAT_DET_HI_RT_STS)
		batt_is_broken = 1;
	else
		batt_is_broken = 0;

	/* Unlock SMBBP Secured Register */
	pm8x41_reg_write(PM8XXX_SEC_ACCESS, SEC_ACCESS);

	/* Disable force trickle charging */
	pm8x41_reg_write(PM8XXX_TRKL_CHG_TEST, 0x00);
	/* re-enable VCP */
	pm8x41_reg_write(PM8XXX_VCP, VCP_ENABLE);
	/* restore trickle charging default current */
	pm8x41_reg_write(PM8XXX_IBAT_ATC_A, trkl_default);
	/* restore VBAT_DET_LO setting to original value */
	pm8x41_reg_write(PM8XXX_VBAT_DET, vbat_det_default);

	return batt_is_broken;
}
