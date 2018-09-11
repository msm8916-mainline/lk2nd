/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
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
#include <debug.h>
#include <spmi.h>
#include <qpnp_wled.h>
#include <qpnp_lcdb.h>
#include <qtimer.h>
#include <target.h>

struct qpnp_lcdb *lcdb;

static int qpnp_lcdb_set_voltage(struct qpnp_lcdb *lcdb, uint32_t ldo_v,
								uint32_t ncp_v)
{
	int rc = -1;
	uint32_t new_uV, boost_ref_volt;
	uint8_t val;

	/* Set LDO voltage */
	if (ldo_v < lcdb->ldo_init_volt) {
		dprintf(CRITICAL, "qpnp_ldo_set_voltage failed, min_uV %d is "
			"less than the minimum supported voltage %d\n",
			ldo_v, lcdb->ldo_init_volt);
		return rc;
	}

	val = ((ldo_v - lcdb->ldo_init_volt) +
		LDO_VREG_STEP_SIZE_UV - 1) / LDO_VREG_STEP_SIZE_UV;
	new_uV = val * LDO_VREG_STEP_SIZE_UV + lcdb->ldo_init_volt;
	if (new_uV > lcdb->ldo_max_volt) {
		dprintf(CRITICAL, "qpnp_ldo_set_voltage unable to set voltage "
			"(%d %d)\n", lcdb->ldo_min_volt, lcdb->ldo_max_volt);
		return rc;
	}

	/* Value adjustment as initial 1V has 100mV step and rest 50mV step */
	if ( val > 18 )
		val = val - 9;
	else
		val = val / 2;

	pmic_spmi_reg_mask_write(lcdb->lcdb_base +
				QPNP_LCDB_LDO_OUTPUT_VOLTAGE_REG,
				QPNP_LCDB_LDO_OUTPUT_VOLTAGE_MASK, val);

	udelay(2);

	/* Set NCP voltage */
	if (ncp_v < lcdb->ncp_init_volt) {
		dprintf(CRITICAL, "qpnp_ncp_set_voltage failed, min_uV %d is "
			"less than the minimum supported voltage %d\n",
			ncp_v, lcdb->ncp_init_volt);
		return rc;
	}

	val = ((ncp_v - lcdb->ncp_init_volt) +
			NCP_VREG_STEP_SIZE_UV - 1) / NCP_VREG_STEP_SIZE_UV;
	new_uV = val * NCP_VREG_STEP_SIZE_UV + lcdb->ncp_init_volt;
	if (new_uV > lcdb->ncp_max_volt) {
		dprintf(CRITICAL, "qpnp_ncp_set_voltage unable to set voltage "
			"(%d %d)\n", lcdb->ncp_min_volt, lcdb->ncp_max_volt);
		return rc;
	}

	/* Value adjustment as initial 1V has 100mV step and rest 50mV step */
	if ( val > 18 )
		val = val - 9;
	else
		val = val / 2;

	pmic_spmi_reg_mask_write(lcdb->lcdb_base +
				QPNP_LCDB_NCP_OUTPUT_VOLTAGE_REG,
				QPNP_LCDB_NCP_OUTPUT_VOLTAGE_MASK, val);

	udelay(2);

	/* Set Boost voltage */
	boost_ref_volt = ((lcdb->ldo_max_volt > lcdb->ncp_max_volt) ?
			lcdb->ldo_max_volt : lcdb->ncp_max_volt) +
			LCDB_BOOST_HEADROOM_VOLT_UV;
	if (boost_ref_volt < lcdb->bst_init_volt)
		boost_ref_volt = lcdb->bst_init_volt;

	val = ((boost_ref_volt - lcdb->bst_init_volt) +
		BST_VREG_STEP_SIZE_UV - 1) / BST_VREG_STEP_SIZE_UV;
	new_uV = val * BST_VREG_STEP_SIZE_UV + lcdb->bst_init_volt;

	pmic_spmi_reg_mask_write(lcdb->lcdb_base +
				QPNP_LCDB_BST_OUTPUT_VOLTAGE_REG,
				QPNP_LCDB_BST_OUTPUT_VOLTAGE_MASK, val);

	return 0;
}

static int qpnp_lcdb_step_voltage(struct qpnp_lcdb *lcdb,
					uint32_t step_start_uv)
{
	int rc = 0;
	uint32_t target_ldo_v, target_ncp_v, i;

	for (i = step_start_uv; i <= LCDB_LDO_MAX_VOLTAGE_UV;
						i += LCDB_STEP_UV) {
		target_ldo_v = min(lcdb->ldo_min_volt, i);
		target_ncp_v = min(lcdb->ncp_min_volt, i);
		rc = qpnp_lcdb_set_voltage(lcdb, target_ldo_v, target_ncp_v);

		if (lcdb->ldo_min_volt <= i && lcdb->ncp_min_volt <= i)
			break;

		/* 1ms wait */
		mdelay(1);
	}

	return rc;
}

static int qpnp_lcdb_config(struct qpnp_lcdb *lcdb)
{
	uint8_t reg = 0;

	if (lcdb->lcdb_pwrup_dly_ms > QPNP_LCDB_PWRUP_DLY_MAX_MS)
		lcdb->lcdb_pwrup_dly_ms = QPNP_LCDB_PWRUP_DLY_MAX_MS;

	if (lcdb->lcdb_pwrdn_dly_ms > QPNP_LCDB_PWRDN_DLY_MAX_MS)
		lcdb->lcdb_pwrdn_dly_ms = QPNP_LCDB_PWRDN_DLY_MAX_MS;

	reg = pmic_spmi_reg_read(lcdb->lcdb_base +
			QPNP_LCDB_PWRUP_PWRDN_CTL_REG);

	/* Set power up delay */
	reg &= QPNP_LCDB_PWRUP_DLY_MASK;
	reg |= (lcdb->lcdb_pwrup_dly_ms << LCDB_PWRUP_DLY_SHIFT);

	/* Set power down delay */
	reg &= ~(QPNP_LCDB_PWRDN_DLY_MASK);
	reg |= (lcdb->lcdb_pwrdn_dly_ms);

	pmic_spmi_reg_write(lcdb->lcdb_base + QPNP_LCDB_PWRUP_PWRDN_CTL_REG,
			reg);

	/* Make LCDB module ready */
	pmic_spmi_reg_mask_write(lcdb->lcdb_base + QPNP_LCDB_MODULE_RDY_REG,
		QPNP_LCDB_MODULE_RDY_MASK, LCDB_MODULE_RDY);

	return 0;
}

static int qpnp_lcdb_setup(struct qpnp_lcdb *lcdb,
			struct qpnp_wled_config_data *config)
{
	lcdb->lcdb_base = QPNP_LCDB_BASE;
	lcdb->lcdb_pwrup_dly_ms = config->pwr_up_delay;
	lcdb->lcdb_pwrdn_dly_ms = config->pwr_down_delay;
	lcdb->ldo_min_volt = config->lab_min_volt;
	lcdb->ldo_max_volt = config->lab_max_volt;
	lcdb->ldo_init_volt = LCDB_LDO_INIT_VOLTAGE_UV;
	lcdb->ncp_min_volt = config->ibb_min_volt;
	lcdb->ncp_max_volt = config->ibb_max_volt;
	lcdb->ncp_init_volt = LCDB_NCP_INIT_VOLTAGE_UV;
	lcdb->bst_init_volt = LCDB_BOOST_INIT_VOLTAGE_UV;

	return 0;
}

int qpnp_lcdb_enable(bool state)
{
	int rc = 0;
	uint32_t target_ldo_v, target_ncp_v;

	if (!lcdb) {
		dprintf(CRITICAL, "%s: lcdb is not initialized yet\n", __func__);
		return ERROR;
	}

	if (state) {
		/*
		 * Set LDO/NCP voltage step wise (in steps of 500mV) as a part
		 * of SW WA to prevent loss in accuracy in ADC measurement of
		 * VBATT voltage.
		 * Set minimum of spec voltage or 4.5V.
		 */
		target_ldo_v = min(lcdb->ldo_min_volt, (lcdb->ldo_init_volt +
					LCDB_STEP_UV));
		target_ncp_v = min(lcdb->ncp_min_volt, (lcdb->ncp_init_volt +
					LCDB_STEP_UV));
		rc = qpnp_lcdb_set_voltage(lcdb, target_ldo_v, target_ncp_v);

		/* Enable LCDB */
		pmic_spmi_reg_mask_write(lcdb->lcdb_base + QPNP_LCDB_ENABLE_CTL_REG,
			QPNP_LCDB_ENABLE_CTL_MASK, (state << LCDB_ENABLE_SHIFT));

		/* Initial delay of 10ms */
		mdelay(10);

		/* Start voltage stepping from 5V onwards */
		if (lcdb->ldo_min_volt > (lcdb->ldo_init_volt + LCDB_STEP_UV) ||
			lcdb->ncp_min_volt > (lcdb->ncp_init_volt + LCDB_STEP_UV))
			rc = qpnp_lcdb_step_voltage(lcdb, target_ldo_v +
								LCDB_STEP_UV);
	} else {
		pmic_spmi_reg_mask_write(lcdb->lcdb_base + QPNP_LCDB_ENABLE_CTL_REG,
			QPNP_LCDB_ENABLE_CTL_MASK, (state << LCDB_ENABLE_SHIFT));
	}

	return rc;
}

int qpnp_lcdb_init(struct qpnp_wled_config_data *config)
{
	int rc;

	if (!target_is_pmi_enabled())
		return ERR_NOT_FOUND;

	lcdb = malloc(sizeof(struct qpnp_lcdb));
	if (!lcdb)
		return ERR_NO_MEMORY;

	memset(lcdb, 0, sizeof(struct qpnp_lcdb));

	rc = qpnp_lcdb_setup(lcdb, config);
	if(rc) {
		dprintf(CRITICAL, "Setting LCDB parameters failed\n");
		return rc;
	}

	rc = qpnp_lcdb_config(lcdb);
	if (rc) {
		dprintf(CRITICAL, "lcdb config failed\n");
		return rc;
	}

	return rc;
}
