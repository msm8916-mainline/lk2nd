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
 *   * Neither the name of The Linux Foundation nor the names of its
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

#include <kernel/timer.h>
#include <qtimer.h>
#include <bits.h>
#include <debug.h>
#include <smem.h>
#include <spmi.h>
#include <platform/iomap.h>
#include <board.h>
#include <target.h>
#include <pm_vib.h>

#define VIB_LDO_BASE			(PMI_SECOND_SLAVE_ADDR_BASE + 0x5700)
#define QPNP_VIB_LDO_STATUS1_REG	(VIB_LDO_BASE + 0x08)
#define QPNP_VIB_LDO_VSET_LB_REG	(VIB_LDO_BASE + 0x40)
#define QPNP_VIB_LDO_VSET_UB_REG	(VIB_LDO_BASE + 0x41)
#define QPNP_VIB_LDO_EN_CTL_REG		(VIB_LDO_BASE + 0x46)

#define QPNP_VIB_LDO_VSET_LB_MASK	0xFF
#define QPNP_VIB_LDO_VSET_UB_MASK	0xFF
#define QPNP_VIB_LDO_EN_CTL_MASK	0x80
#define QPNP_VIB_LDO_EN			0x80
#define QPNP_VIB_LDO_DIS		0x00

#define VREG_READY BIT(7)

#define MAX_WAIT_FOR_VREG_READY_US	1000
#define VREG_READY_STEP_DELAY_US	100
#define VIB_LDO_OVERDRIVE_VOLTAGE_MV	3500
#define VIB_LDO_NOMINAL_VOLTAGE_MV	3000
#define OVERDRIVE_TIME_US		30000

/* Turn on vibrator */
void pm_vib_ldo_turn_on(void)
{
	uint8_t status;
	uint8_t vreg_timer_count = 0;

	if (!target_is_pmi_enabled())
		return;

	/* Set overdrive voltage*/
	pmic_spmi_reg_mask_write(QPNP_VIB_LDO_VSET_LB_REG,
				QPNP_VIB_LDO_VSET_LB_MASK,
				(VIB_LDO_OVERDRIVE_VOLTAGE_MV & 0xff));
	pmic_spmi_reg_mask_write(QPNP_VIB_LDO_VSET_UB_REG,
				QPNP_VIB_LDO_VSET_UB_MASK,
				((VIB_LDO_OVERDRIVE_VOLTAGE_MV >> 8) & 0xff));

	/* Set Register VIB_LDO_EN_CTL to enable vibrator */
	pmic_spmi_reg_mask_write(QPNP_VIB_LDO_EN_CTL_REG,
				QPNP_VIB_LDO_EN_CTL_MASK,
				QPNP_VIB_LDO_EN);

	/* Wait for VREG_READY*/
	while (1) {
		status = pmic_spmi_reg_read(QPNP_VIB_LDO_STATUS1_REG);
		if ( status & VREG_READY ) {
			break;
		}
		else if ( vreg_timer_count < (MAX_WAIT_FOR_VREG_READY_US /
						VREG_READY_STEP_DELAY_US) ) {
			vreg_timer_count++;
			udelay(VREG_READY_STEP_DELAY_US);
		}
		else {
			dprintf(CRITICAL, "LDO failed to start in 1 msec, "
				"turning off vibrator\n");
			pm_vib_ldo_turn_off();
			return;
		}
	}

	udelay(OVERDRIVE_TIME_US);

	/* Set normal voltage */
	pmic_spmi_reg_mask_write(QPNP_VIB_LDO_VSET_LB_REG,
				QPNP_VIB_LDO_VSET_LB_MASK,
				(VIB_LDO_NOMINAL_VOLTAGE_MV & 0xff));
	pmic_spmi_reg_mask_write(QPNP_VIB_LDO_VSET_UB_REG,
				QPNP_VIB_LDO_VSET_UB_MASK,
				((VIB_LDO_NOMINAL_VOLTAGE_MV >> 8) & 0xff));

	vreg_timer_count = 0;
	/* Wait for VREG_READY*/
	while (1) {
		status = pmic_spmi_reg_read(QPNP_VIB_LDO_STATUS1_REG);
		if ( status & VREG_READY ) {
			break;
		}
		else if ( vreg_timer_count < (MAX_WAIT_FOR_VREG_READY_US /
						VREG_READY_STEP_DELAY_US) ) {
			vreg_timer_count++;
			udelay(VREG_READY_STEP_DELAY_US);
		}
		else {
			dprintf(CRITICAL, "LDO failed to start in 1 msec, "
				"turning off vibrator\n");
			pm_vib_ldo_turn_off();
			return;
		}
	}

}

/* Turn off vibrator */
void pm_vib_ldo_turn_off(void)
{
	if (!target_is_pmi_enabled())
		return;

	/* Clear Register VIB_LDO_EN_CTL to disable vibrator */
	pmic_spmi_reg_mask_write(QPNP_VIB_LDO_EN_CTL_REG,
				QPNP_VIB_LDO_EN_CTL_MASK,
				QPNP_VIB_LDO_DIS);
}
