 /* Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <qpnp_wled.h>
#include <pm8x41_wled.h>
#include <qtimer.h>
#include <target.h>

static int qpnp_wled_avdd_target_voltages[NUM_SUPPORTED_AVDD_VOLTAGES] = {
	7900, 7600, 7300, 6400, 6100, 5800,
};

static uint8_t qpnp_wled_ovp_reg_settings[NUM_SUPPORTED_AVDD_VOLTAGES] = {
	0x0, 0x0, 0x1, 0x2, 0x2, 0x3,
};

static int qpnp_wled_avdd_trim_adjustments[NUM_SUPPORTED_AVDD_VOLTAGES] = {
	3, 0, -2, 7, 3, 3,
};

static int fls(uint16_t n)
{
	int i = 0;
	for (; n; n >>= 1, i++);
	  return i;
}

static struct qpnp_wled *gwled;
static int qpnp_labibb_regulator_set_voltage(struct qpnp_wled *wled);

static int qpnp_wled_sec_access(struct qpnp_wled *wled, uint16_t base_addr)
{
	uint8_t reg = QPNP_WLED_SEC_UNLOCK;

	pm8x41_wled_reg_write(QPNP_WLED_SEC_ACCESS_REG(base_addr), reg);

	return 0;
}

/* set wled to a level of brightness */
static int qpnp_wled_set_level(struct qpnp_wled *wled, int level)
{
	int i;
	uint8_t reg;

	/* set brightness registers */
	for (i = 0; i < wled->num_strings; i++) {
		reg = level & QPNP_WLED_BRIGHT_LSB_MASK;
		pm8x41_wled_reg_write(QPNP_WLED_BRIGHT_LSB_REG(wled->sink_base,
						wled->strings[i]), reg);

		reg = level >> QPNP_WLED_BRIGHT_MSB_SHIFT;
		reg = reg & QPNP_WLED_BRIGHT_MSB_MASK;
		pm8x41_wled_reg_write(QPNP_WLED_BRIGHT_MSB_REG(wled->sink_base,
						wled->strings[i]), reg);
	}

	/* sync */
	reg = QPNP_WLED_SYNC;
	pm8x41_wled_reg_write(QPNP_WLED_SYNC_REG(wled->sink_base), reg);

	reg = QPNP_WLED_SYNC_RESET;
	pm8x41_wled_reg_write(QPNP_WLED_SYNC_REG(wled->sink_base), reg);

	return 0;
}

static int qpnp_wled_enable(struct qpnp_wled *wled,
				uint16_t base_addr, bool state)
{
	uint8_t reg;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_MODULE_EN_REG(base_addr));
	reg &= QPNP_WLED_MODULE_EN_MASK;
	reg |= (state << QPNP_WLED_MODULE_EN_SHIFT);
	pm8x41_wled_reg_write(QPNP_WLED_MODULE_EN_REG(base_addr), reg);

	return 0;
}

static int qpnp_wled_ibb_swire_rdy(struct qpnp_wled *wled,
				uint16_t base_addr, bool state)
{
	uint8_t reg;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_MODULE_EN_REG(base_addr));
	/* Do not enable IBB module when SWIRE ready is set */
	reg &= ~(QPNP_IBB_SWIRE_RDY_MASK | QPNP_IBB_MODULE_EN_MASK);
	reg |= (state << QPNP_IBB_SWIRE_RDY_SHIFT);
	pm8x41_wled_reg_write(QPNP_WLED_MODULE_EN_REG(base_addr), reg);

	return 0;
}

int qpnp_ibb_enable(bool state)
{
	int rc = 0;
	uint8_t reg;

	if (!gwled) {
		dprintf(CRITICAL, "%s: wled is not initialized yet\n", __func__);
		return ERROR;
	}

	/* enable lab */
	if (gwled->ibb_bias_active) {
		rc = qpnp_wled_enable(gwled, gwled->lab_base, state);
		udelay(QPNP_WLED_LAB_START_DLY_US + 1);
		if (rc < 0)
			return rc;
	} else {
		reg = pm8x41_wled_reg_read(QPNP_WLED_LAB_IBB_RDY_REG(gwled->lab_base));

		reg &= QPNP_WLED_MODULE_EN_MASK;
		reg |= (state << QPNP_WLED_MODULE_EN_SHIFT);
		pm8x41_wled_reg_write(QPNP_WLED_LAB_IBB_RDY_REG(gwled->lab_base), reg);
	}

	if (gwled->disp_type_amoled && gwled->lab_ibb_swire_control)
		rc = qpnp_wled_ibb_swire_rdy(gwled, gwled->ibb_base, state);
	else
		rc = qpnp_wled_enable(gwled, gwled->ibb_base, state);

	return rc;
}

/* enable / disable wled brightness */
void qpnp_wled_enable_backlight(int enable)
{
	int rc;

	if (!gwled) {
		dprintf(CRITICAL, "%s: wled is not initialized yet\n", __func__);
		return;
	}

	if (enable) {
		rc = qpnp_wled_set_level(gwled, QPNP_WLED_MAX_BR_LEVEL);
		if (rc) {
			dprintf(CRITICAL,"wled set level failed\n");
			return;
		}
	}

	if (!gwled->disp_type_amoled || !gwled->wled_avdd_control) {
		rc = qpnp_wled_enable(gwled, gwled->ctrl_base, enable);
		if (rc) {
			dprintf(CRITICAL, "wled %sable failed\n",
						enable ? "en" : "dis");
			return;
		}
	}

}

static int qpnp_wled_set_display_type(struct qpnp_wled *wled, uint16_t base_addr)
{
	uint8_t reg = 0;
	int rc;

	/* display type */
	reg = pm8x41_wled_reg_read(QPNP_WLED_DISP_SEL_REG(base_addr));

	reg &= QPNP_WLED_DISP_SEL_MASK;
	reg |= (wled->disp_type_amoled << QPNP_WLED_DISP_SEL_SHIFT);

	rc = qpnp_wled_sec_access(wled, base_addr);
	if (rc)
		return rc;

	pm8x41_wled_reg_write(QPNP_WLED_DISP_SEL_REG(base_addr), reg);

	return 0;
}

static int qpnp_wled_module_ready(struct qpnp_wled *wled, uint16_t base_addr, bool state)
{
	uint8_t reg;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_MODULE_RDY_REG(base_addr));
	reg &= QPNP_WLED_MODULE_RDY_MASK;
	reg |= (state << QPNP_WLED_MODULE_RDY_SHIFT);
	pm8x41_wled_reg_write(QPNP_WLED_MODULE_RDY_REG(base_addr), reg);

	return 0;
}

/* Configure WLED registers */
static int qpnp_wled_config(struct qpnp_wled *wled)
{
	int rc, i, temp;
	uint8_t reg = 0;

	/* Configure display type */
	rc = qpnp_wled_set_display_type(wled, wled->ctrl_base);
	if (rc < 0)
		return rc;

	/* Recommended WLED MDOS settings for AMOLED */
	if (wled->disp_type_amoled) {
		pm8x41_wled_reg_write(QPNP_WLED_VLOOP_COMP_RES(wled->ctrl_base),
			0x8F);
		pm8x41_wled_reg_write(QPNP_WLED_VLOOP_COMP_GM(wled->ctrl_base),
			0x81);
		pm8x41_wled_reg_write(QPNP_WLED_PSM_CTRL(wled->ctrl_base),
			0x83);
		pm8x41_wled_reg_write(QPNP_WLED_PSM_EN(wled->ctrl_base),
			0x80);

		rc = qpnp_wled_sec_access(wled, wled->ctrl_base);
		if (rc)
			return rc;
		pm8x41_wled_reg_write(QPNP_WLED_TEST4(wled->ctrl_base), 0x13);
	}

	/* Configure the FEEDBACK OUTPUT register */
	reg = pm8x41_wled_reg_read(
			QPNP_WLED_FDBK_OP_REG(wled->ctrl_base));
	reg &= QPNP_WLED_FDBK_OP_MASK;
	reg |= wled->fdbk_op;
	pm8x41_wled_reg_write(QPNP_WLED_FDBK_OP_REG(wled->ctrl_base), reg);

	/* Configure the VREF register */
	if (wled->vref_mv < QPNP_WLED_VREF_MIN_MV)
		wled->vref_mv = QPNP_WLED_VREF_MIN_MV;
	else if (wled->vref_mv > QPNP_WLED_VREF_MAX_MV)
		wled->vref_mv = QPNP_WLED_VREF_MAX_MV;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_VREF_REG(wled->ctrl_base));
	reg &= QPNP_WLED_VREF_MASK;
	temp = wled->vref_mv - QPNP_WLED_VREF_MIN_MV;
	reg |= (temp / QPNP_WLED_VREF_STEP_MV);
	pm8x41_wled_reg_write(QPNP_WLED_VREF_REG(wled->ctrl_base), reg);

	/* Configure the ILIM register */
	if (wled->ilim_ma < QPNP_WLED_ILIM_MIN_MA)
		wled->ilim_ma = QPNP_WLED_ILIM_MIN_MA;
	else if (wled->ilim_ma > QPNP_WLED_ILIM_MAX_MA)
		wled->ilim_ma = QPNP_WLED_ILIM_MAX_MA;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_ILIM_REG(wled->ctrl_base));
	temp = (wled->ilim_ma / QPNP_WLED_ILIM_STEP_MA);
	if (temp != (reg & ~QPNP_WLED_ILIM_MASK)) {
		reg &= QPNP_WLED_ILIM_MASK;
		reg |= temp;
		reg |= QPNP_WLED_ILIM_OVERWRITE;
		pm8x41_wled_reg_write(QPNP_WLED_ILIM_REG(wled->ctrl_base), reg);
	}

	/* Configure the MAX BOOST DUTY register */
	if (wled->boost_duty_ns < QPNP_WLED_BOOST_DUTY_MIN_NS)
		wled->boost_duty_ns = QPNP_WLED_BOOST_DUTY_MIN_NS;
	else if (wled->boost_duty_ns > QPNP_WLED_BOOST_DUTY_MAX_NS)
		wled->boost_duty_ns = QPNP_WLED_BOOST_DUTY_MAX_NS;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_BOOST_DUTY_REG(wled->ctrl_base));
	reg &= QPNP_WLED_BOOST_DUTY_MASK;
	reg |= (wled->boost_duty_ns / QPNP_WLED_BOOST_DUTY_STEP_NS);
	pm8x41_wled_reg_write(QPNP_WLED_BOOST_DUTY_REG(wled->ctrl_base), reg);

	/* Configure the SWITCHING FREQ register */
	if (wled->switch_freq_khz == QPNP_WLED_SWITCH_FREQ_1600_KHZ)
		temp = QPNP_WLED_SWITCH_FREQ_1600_KHZ_CODE;
	else
		temp = QPNP_WLED_SWITCH_FREQ_800_KHZ_CODE;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_SWITCH_FREQ_REG(wled->ctrl_base));
	reg &= QPNP_WLED_SWITCH_FREQ_MASK;
	reg |= temp;
	pm8x41_wled_reg_write(QPNP_WLED_SWITCH_FREQ_REG(wled->ctrl_base), reg);

	/* Configure the OVP register */
	if (wled->ovp_mv <= QPNP_WLED_OVP_17800_MV) {
		wled->ovp_mv = QPNP_WLED_OVP_17800_MV;
		temp = 3;
	} else if (wled->ovp_mv <= QPNP_WLED_OVP_19400_MV) {
		wled->ovp_mv = QPNP_WLED_OVP_19400_MV;
		temp = 2;
	} else if (wled->ovp_mv <= QPNP_WLED_OVP_29500_MV) {
		wled->ovp_mv = QPNP_WLED_OVP_29500_MV;
		temp = 1;
	} else {
		wled->ovp_mv = QPNP_WLED_OVP_31000_MV;
		temp = 0;
	}

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_OVP_REG(wled->ctrl_base));
	reg &= QPNP_WLED_OVP_MASK;
	reg |= temp;
	pm8x41_wled_reg_write(QPNP_WLED_OVP_REG(wled->ctrl_base), reg);

	if (wled->disp_type_amoled) {
		for (i = 0; i < NUM_SUPPORTED_AVDD_VOLTAGES; i++) {
			if (QPNP_WLED_AVDD_DEFAULT_VOLTAGE_MV == qpnp_wled_avdd_target_voltages[i])
				break;
		}
		if (i == NUM_SUPPORTED_AVDD_VOLTAGES)
		{
			dprintf(CRITICAL, "Invalid avdd target voltage specified \n");
			return ERR_NOT_VALID;
		}
		/* Update WLED_OVP register based on desired target voltage */
		reg = qpnp_wled_ovp_reg_settings[i];
		pm8x41_wled_reg_write(QPNP_WLED_OVP_REG(wled->ctrl_base), reg);
		/* Update WLED_TRIM register based on desired target voltage */
		reg = pm8x41_wled_reg_read(
			QPNP_WLED_REF_7P7_TRIM_REG(wled->ctrl_base));
		reg += qpnp_wled_avdd_trim_adjustments[i];
		if ((int8_t)reg < QPNP_WLED_AVDD_MIN_TRIM_VALUE)
			reg = QPNP_WLED_AVDD_MIN_TRIM_VALUE;
		else if((int8_t)reg > QPNP_WLED_AVDD_MAX_TRIM_VALUE)
			reg = QPNP_WLED_AVDD_MAX_TRIM_VALUE;

		rc = qpnp_wled_sec_access(wled, wled->ctrl_base);
		if (rc)
			return rc;

		temp = pm8x41_wled_reg_read(
			QPNP_WLED_REF_7P7_TRIM_REG(wled->ctrl_base));
		temp &= ~QPNP_WLED_7P7_TRIM_MASK;
		temp |= (reg & QPNP_WLED_7P7_TRIM_MASK);
		pm8x41_wled_reg_write(QPNP_WLED_REF_7P7_TRIM_REG(wled->ctrl_base), temp);
		/* Write to spare to avoid reconfiguration in HLOS */
		reg = pm8x41_wled_reg_read(
			QPNP_WLED_CTRL_SPARE_REG(wled->ctrl_base));
		reg |= QPNP_WLED_AVDD_SET_BIT;
		pm8x41_wled_reg_write(QPNP_WLED_CTRL_SPARE_REG(wled->ctrl_base), reg);
	}

	/* Configure the MODULATION register */
	if (wled->mod_freq_khz <= QPNP_WLED_MOD_FREQ_1200_KHZ) {
		wled->mod_freq_khz = QPNP_WLED_MOD_FREQ_1200_KHZ;
		temp = 3;
	} else if (wled->mod_freq_khz <= QPNP_WLED_MOD_FREQ_2400_KHZ) {
		wled->mod_freq_khz = QPNP_WLED_MOD_FREQ_2400_KHZ;
		temp = 2;
	} else if (wled->mod_freq_khz <= QPNP_WLED_MOD_FREQ_9600_KHZ) {
		wled->mod_freq_khz = QPNP_WLED_MOD_FREQ_9600_KHZ;
		temp = 1;
	} else {
		wled->mod_freq_khz = QPNP_WLED_MOD_FREQ_19200_KHZ;
		temp = 0;
	}
	reg = pm8x41_wled_reg_read(QPNP_WLED_MOD_REG(wled->sink_base));

	reg &= QPNP_WLED_MOD_FREQ_MASK;
	reg |= (temp << QPNP_WLED_MOD_FREQ_SHIFT);

	reg &= QPNP_WLED_PHASE_STAG_MASK;
	reg |= (wled->en_phase_stag << QPNP_WLED_PHASE_STAG_SHIFT);

	reg &= QPNP_WLED_DIM_RES_MASK;
	reg |= (wled->en_9b_dim_res << QPNP_WLED_DIM_RES_SHIFT);

	if (wled->dim_mode == QPNP_WLED_DIM_HYBRID) {
		reg &= QPNP_WLED_DIM_HYB_MASK;
		reg |= (1 << QPNP_WLED_DIM_HYB_SHIFT);
	} else {
		reg &= QPNP_WLED_DIM_HYB_MASK;
		reg |= (0 << QPNP_WLED_DIM_HYB_SHIFT);
		reg &= QPNP_WLED_DIM_ANA_MASK;
		reg |= wled->dim_mode;
	}

	pm8x41_wled_reg_write(QPNP_WLED_MOD_REG(wled->sink_base), reg);

	/* Configure the HYBRID THRESHOLD register */
	if (wled->hyb_thres < QPNP_WLED_HYB_THRES_MIN)
		wled->hyb_thres = QPNP_WLED_HYB_THRES_MIN;
	else if (wled->hyb_thres > QPNP_WLED_HYB_THRES_MAX)
		wled->hyb_thres = QPNP_WLED_HYB_THRES_MAX;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_HYB_THRES_REG(wled->sink_base));

	reg &= QPNP_WLED_HYB_THRES_MASK;
	temp = fls(wled->hyb_thres / QPNP_WLED_HYB_THRES_MIN) - 1;
	reg |= temp;
	pm8x41_wled_reg_write(QPNP_WLED_HYB_THRES_REG(wled->sink_base), reg);

	for (i = 0; i < wled->num_strings; i++) {
		if (wled->strings[i] >= QPNP_WLED_MAX_STRINGS) {
			dprintf(CRITICAL,"Invalid string number\n");
			return ERR_NOT_VALID;
		}

		/* MODULATOR */
		reg = pm8x41_wled_reg_read(
				QPNP_WLED_MOD_EN_REG(wled->sink_base,
						wled->strings[i]));
		reg &= QPNP_WLED_MOD_EN_MASK;
		reg |= (QPNP_WLED_MOD_EN << QPNP_WLED_MOD_EN_SHFT);
		pm8x41_wled_reg_write(QPNP_WLED_MOD_EN_REG(wled->sink_base,
						wled->strings[i]), reg);

		/* SYNC DELAY */
		if (wled->sync_dly_us > QPNP_WLED_SYNC_DLY_MAX_US)
			wled->sync_dly_us = QPNP_WLED_SYNC_DLY_MAX_US;

		reg = pm8x41_wled_reg_read(
				QPNP_WLED_SYNC_DLY_REG(wled->sink_base,
						wled->strings[i]));
		reg &= QPNP_WLED_SYNC_DLY_MASK;
		temp = wled->sync_dly_us / QPNP_WLED_SYNC_DLY_STEP_US;
		reg |= temp;
		pm8x41_wled_reg_write(QPNP_WLED_SYNC_DLY_REG(wled->sink_base,
						wled->strings[i]), reg);

		/* FULL SCALE CURRENT */
		if (wled->fs_curr_ua > QPNP_WLED_FS_CURR_MAX_UA)
			wled->fs_curr_ua = QPNP_WLED_FS_CURR_MAX_UA;

		reg = pm8x41_wled_reg_read(
				QPNP_WLED_FS_CURR_REG(wled->sink_base,
						wled->strings[i]));
		reg &= QPNP_WLED_FS_CURR_MASK;
		temp = wled->fs_curr_ua / QPNP_WLED_FS_CURR_STEP_UA;
		reg |= temp;
		pm8x41_wled_reg_write(QPNP_WLED_FS_CURR_REG(wled->sink_base,
						wled->strings[i]), reg);

		/* CABC */
		reg = pm8x41_wled_reg_read(
				QPNP_WLED_CABC_REG(wled->sink_base,
						wled->strings[i]));
		reg &= QPNP_WLED_CABC_MASK;
		reg |= (wled->en_cabc << QPNP_WLED_CABC_SHIFT);
		pm8x41_wled_reg_write(QPNP_WLED_CABC_REG(wled->sink_base,
						wled->strings[i]), reg);

		/* Enable CURRENT SINK */
		reg = pm8x41_wled_reg_read(
				QPNP_WLED_CURR_SINK_REG(wled->sink_base));
		temp = wled->strings[i] + QPNP_WLED_CURR_SINK_SHIFT;
		reg |= (1 << temp);
		pm8x41_wled_reg_write(QPNP_WLED_CURR_SINK_REG(wled->sink_base), reg);
	}

	/* LAB fast precharge */
	reg = pm8x41_wled_reg_read(
			QPNP_WLED_LAB_FAST_PC_REG(wled->lab_base));
	reg &= QPNP_WLED_LAB_FAST_PC_MASK;
	reg |= (wled->lab_fast_precharge << QPNP_WLED_LAB_FAST_PC_SHIFT);
	/* LAB max precharge  time */
	reg &= QPNP_WLED_PRECHARGE_MASK;
	reg |= (wled->lab_max_precharge_time);
	pm8x41_wled_reg_write(QPNP_WLED_LAB_FAST_PC_REG(wled->lab_base), reg);

	/* Configure lab display type */
	rc = qpnp_wled_set_display_type(wled, wled->lab_base);
	if (rc < 0)
		return rc;

	/* make LAB module ready */
	rc = qpnp_wled_module_ready(wled, wled->lab_base, true);
	if (rc < 0)
		return rc;

	/* Disable LAB pulse skipping for AMOLED */
	if (wled->disp_type_amoled)
		pm8x41_wled_reg_write(wled->lab_base +
			QPNP_LABIBB_PS_CTL, 0x00);

	/* IBB active bias */
	if (wled->ibb_pwrup_dly_ms > QPNP_WLED_IBB_PWRUP_DLY_MAX_MS)
		wled->ibb_pwrup_dly_ms = QPNP_WLED_IBB_PWRUP_DLY_MAX_MS;

	if (wled->ibb_pwrdn_dly_ms > QPNP_WLED_IBB_PWRDN_DLY_MAX_MS)
		wled->ibb_pwrdn_dly_ms = QPNP_WLED_IBB_PWRDN_DLY_MAX_MS;

	reg = pm8x41_wled_reg_read(
			QPNP_WLED_IBB_BIAS_REG(wled->ibb_base));

	reg &= QPNP_WLED_IBB_BIAS_MASK;
	reg |= (!wled->ibb_bias_active << QPNP_WLED_IBB_BIAS_SHIFT);

	temp = wled->ibb_pwrup_dly_ms;
	reg &= QPNP_WLED_IBB_PWRUP_DLY_MASK;
	reg |= (temp << QPNP_WLED_IBB_PWRUP_DLY_SHIFT);
	/* Power down delay bits could already be set, clear them before
	 * or'ing new values
	 */
	reg &= ~(PWRDN_DLY2_MASK);
	reg |= wled->ibb_pwrdn_dly_ms;
	reg |= (wled->ibb_discharge_en << 2);

	rc = qpnp_wled_sec_access(wled, wled->ibb_base);
	if (rc)
		return rc;

	pm8x41_wled_reg_write(QPNP_WLED_IBB_BIAS_REG(wled->ibb_base), reg);

	/* Configure ibb display type */
	rc = qpnp_wled_set_display_type(wled, wled->ibb_base);
	if (rc < 0)
		return rc;

	/* make IBB module ready */
	rc = qpnp_wled_module_ready(wled, wled->ibb_base, true);
	if (rc < 0)
		return rc;

	rc = qpnp_labibb_regulator_set_voltage(wled);
	if (rc < 0)
		return rc;

	return 0;
}

/* Setup wled default parameters */
static int qpnp_wled_setup(struct qpnp_wled *wled, struct qpnp_wled_config_data *config)
{
	int i;

	wled->sink_base = QPNP_WLED_SINK_BASE;
	wled->ctrl_base = QPNP_WLED_CTRL_BASE;
	wled->ibb_base = QPNP_WLED_IBB_BASE;
	wled->lab_base = QPNP_WLED_LAB_BASE;
	wled->fdbk_op = QPNP_WLED_FDBK_AUTO;
	wled->vref_mv = QPNP_WLED_DFLT_VREF_MV;
	wled->switch_freq_khz = QPNP_WLED_SWITCH_FREQ_800_KHZ;
	wled->ovp_mv = QPNP_WLED_OVP_29500_MV;
	wled->ilim_ma = QPNP_WLED_DFLT_ILIM_MA;
	wled->boost_duty_ns = QPNP_WLED_BOOST_DUTY_MIN_NS;
	wled->mod_freq_khz = QPNP_WLED_MOD_FREQ_19200_KHZ;
	wled->dim_mode = QPNP_WLED_DIM_HYBRID;
	wled->dim_shape = QPNP_WLED_DIM_SHAPE_LINEAR;

	if (wled->dim_mode == QPNP_WLED_DIM_HYBRID) {
		wled->hyb_thres = QPNP_WLED_DFLT_HYB_THRES;
	}

	wled->sync_dly_us = 800;
	wled->fs_curr_ua = 16000;
	wled->en_9b_dim_res = 0;
	wled->en_phase_stag = true;
	wled->en_cabc = 0;

	wled->num_strings = QPNP_WLED_MAX_STRINGS;
		for (i = 0; i < wled->num_strings; i++)
			wled->strings[i] = i;

	wled->ibb_bias_active = false;
	wled->lab_fast_precharge = false;
	wled->lab_max_precharge_time = QPNP_WLED_PRECHARGE_US500;
	wled->ibb_pwrup_dly_ms = config->pwr_up_delay;
	wled->ibb_pwrdn_dly_ms = config->pwr_down_delay;
	wled->ibb_discharge_en = config->ibb_discharge_en;
	wled->disp_type_amoled = config->display_type;
	wled->lab_min_volt = config->lab_min_volt;
	wled->lab_max_volt = config->lab_max_volt;
	wled->ibb_min_volt = config->ibb_min_volt;
	wled->ibb_max_volt = config->ibb_max_volt;
	wled->ibb_init_volt = config->ibb_init_volt;
	wled->lab_init_volt = config->lab_init_volt;
	wled->lab_ibb_swire_control = config->lab_ibb_swire_control;
	wled->wled_avdd_control = config->wled_avdd_control;

	return 0;
}

int qpnp_wled_init(struct qpnp_wled_config_data *config)
{
	int rc;
	struct qpnp_wled *wled;

	if(!target_is_pmi_enabled())
		return ERR_NOT_FOUND;

	wled = malloc(sizeof(struct qpnp_wled));
	if (!wled)
		return ERR_NO_MEMORY;

	memset(wled, 0, sizeof(struct qpnp_wled));

	rc = qpnp_wled_setup(wled, config);
	if (rc) {
		dprintf(CRITICAL, "Setting WLED parameters failed\n");
		return rc;
	}

	rc = qpnp_wled_config(wled);
	if (rc) {
		dprintf(CRITICAL, "wled config failed\n");
		return rc;
	}

	gwled = wled;

	return rc;
}

static int qpnp_labibb_regulator_set_voltage(struct qpnp_wled *wled)
{
	int rc = -1;
	uint32_t new_uV;
	uint8_t val, mask=0;

	if (!wled->disp_type_amoled || !wled->lab_ibb_swire_control) {
		if (wled->lab_min_volt < wled->lab_init_volt) {
			dprintf(CRITICAL,"qpnp_lab_regulator_set_voltage failed, min_uV %d is less than init volt %d\n",
			wled->lab_min_volt, wled->lab_init_volt);
			return rc;
		}

		val = (((wled->lab_min_volt - wled->lab_init_volt) +
		(IBB_LAB_VREG_STEP_SIZE - 1)) / IBB_LAB_VREG_STEP_SIZE);
		new_uV = val * IBB_LAB_VREG_STEP_SIZE + wled->lab_init_volt;

		if (new_uV > wled->lab_max_volt) {
			dprintf(CRITICAL,"qpnp_ibb_regulator_set_voltage unable to set voltage (%d %d)\n",
			wled->lab_min_volt, wled->lab_max_volt);
			return rc;
		}
		val |= QPNP_LAB_OUTPUT_OVERRIDE_EN;
		mask = pm8x41_wled_reg_read(wled->lab_base +
				QPNP_LABIBB_OUTPUT_VOLTAGE);
		mask &= ~(QPNP_LAB_SET_VOLTAGE_MASK
				| QPNP_LAB_OUTPUT_OVERRIDE_EN);
		mask |= val & (QPNP_LAB_SET_VOLTAGE_MASK
				| QPNP_LAB_OUTPUT_OVERRIDE_EN);

		pm8x41_wled_reg_write(wled->lab_base +
				QPNP_LABIBB_OUTPUT_VOLTAGE, mask);
		udelay(2);

		/*
		 * IBB Set Voltage.
		 * For AMOLED panels, the IBB voltage needs to be
		 * controlled by panel.
		 */
		if (wled->ibb_min_volt < wled->ibb_init_volt) {
			dprintf(CRITICAL, "qpnp_ibb_regulator_set_voltage failed, min_uV %d is less than init volt %d\n",
			wled->ibb_min_volt, wled->ibb_init_volt);
			return rc;
		}

		val = (((wled->ibb_min_volt - wled->ibb_init_volt) +
			(IBB_LAB_VREG_STEP_SIZE - 1)) / IBB_LAB_VREG_STEP_SIZE);
		new_uV = val * IBB_LAB_VREG_STEP_SIZE + wled->ibb_init_volt;
		if (new_uV > wled->ibb_max_volt) {
			dprintf(CRITICAL, "qpnp_ibb_regulator_set_voltage unable to set voltage %d %d\n",
			wled->ibb_min_volt, wled->ibb_max_volt);
			return rc;
		}
		val |= QPNP_LAB_OUTPUT_OVERRIDE_EN;
		mask = pm8x41_wled_reg_read(wled->ibb_base +
			QPNP_LABIBB_OUTPUT_VOLTAGE);
		udelay(2);
		mask &= ~(QPNP_IBB_SET_VOLTAGE_MASK |
			QPNP_LAB_OUTPUT_OVERRIDE_EN);
		mask |= (val & (QPNP_IBB_SET_VOLTAGE_MASK |
			QPNP_LAB_OUTPUT_OVERRIDE_EN));

		pm8x41_wled_reg_write(wled->ibb_base +
			QPNP_LABIBB_OUTPUT_VOLTAGE, mask);
	} else {
		pm8x41_wled_reg_write(wled->ibb_base +
			QPNP_LABIBB_OUTPUT_VOLTAGE, 0x00);
	}

	return 0;
}
