/* Copyright (c) 2015, 2017-2018, The Linux Foundation. All rights reserved.
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

#include <spmi.h>
#include <platform/iomap.h>
#include <pm_vib.h>
#include <target.h>
#include <vibrator.h>
#include <smem.h>

#define HAPTIC_BASE(s_id) (s_id ? (PMI_SECOND_SLAVE_ADDR_BASE + 0xC000):((PMI_SECOND_SLAVE_OFFSET << 16) + 0xC000))
#define QPNP_HAP_EN_CTL_REG(s_id) (HAPTIC_BASE(s_id) + 0x46)
#define QPNP_HAP_EN_CTL2_REG(s_id) (HAPTIC_BASE(s_id) + 0x48)
#define QPNP_HAP_ACT_TYPE_REG(s_id) (HAPTIC_BASE(s_id) + 0x4C)
#define QPNP_HAP_WAV_SHAPE_REG(s_id) (HAPTIC_BASE(s_id) + 0x4D)
#define QPNP_HAP_PLAY_MODE_REG(s_id) (HAPTIC_BASE(s_id) + 0x4E)
#define QPNP_HAP_LRA_AUTO_RES_REG(s_id) (HAPTIC_BASE(s_id) + 0x4F)
#define QPNP_HAP_VMAX_REG(s_id) (HAPTIC_BASE(s_id) + 0x51)
#define QPNP_HAP_ILIM_REG(s_id) (HAPTIC_BASE(s_id) + 0x52)
#define QPNP_HAP_SC_DEB_REG(s_id) (HAPTIC_BASE(s_id) + 0x53)
#define QPNP_HAP_RATE_CFG1_REG(s_id) (HAPTIC_BASE(s_id) + 0x54)
#define QPNP_HAP_RATE_CFG2_REG(s_id) (HAPTIC_BASE(s_id) + 0x55)
#define QPNP_HAP_INT_PWM_REG(s_id) (HAPTIC_BASE(s_id) + 0x56)
#define QPNP_HAP_PWM_CAP_REG(s_id) (HAPTIC_BASE(s_id) + 0x58)
#define QPNP_HAP_BRAKE_REG(s_id) (HAPTIC_BASE(s_id) + 0x5C)
#define QPNP_HAP_PLAY_REG(s_id) (HAPTIC_BASE(s_id) + 0x70)

#define QPNP_HAP_ACT_TYPE_MASK 0x01
#define QPNP_HAP_PLAY_MODE_MASK 0x3F
#define QPNP_HAP_DIRECT 0x0
#define QPNP_HAP_VMAX_MASK 0x3F
#define QPNP_HAP_VMAX 0x22
#define QPNP_HAP_ILIM_MASK 0x01
#define QPNP_HAP_ILIM 0x01
#define QPNP_HAP_SC_DEB_MASK 0x07
#define QPNP_HAP_SC_DEB_8CLK 0x01
#define QPNP_HAP_INT_PWM_MASK 0x03
#define QPNP_HAP_INT_PWM_505KHZ 0x01
#define QPNP_HAP_WAV_SHAPE_MASK 0x01
#define QPNP_HAP_WAV_SHAPE_SQUARE 0x01
#define QPNP_HAP_PWM_CAP_MASK 0x03
#define QPNP_HAP_PWM_CAP_13PF 0x01
#define QPNP_HAP_RATE_CFG1_MASK 0xFF
#define QPNP_HAP_RATE_CFG2_MASK 0x0F
#define QPNP_HAP_EN_BRAKE_EN_MASK 0x01
#define QPNP_HAP_EN_BRAKING_EN 0x01
#define QPNP_HAP_BRAKE_VMAX_MASK 0xFF
#define QPNP_HAP_BRAKE_VMAX 0xF
#define QPNP_HAP_ERM 0x1
#define QPNP_HAP_LRA 0x0
#define QPNP_HAP_PLAY_MASK 0x80
#define QPNP_HAP_PLAY_EN 0x80
#define QPNP_HAP_MASK 0x80
#define QPNP_HAP_EN 0x80
#define QPNP_HAP_PLAY_DIS 0x00
#define QPNP_HAP_DIS 0x00
#define QPNP_HAP_BRAKE_MASK 0xFE
#define QPNP_HAP_LRA_AUTO_DISABLE 0x00
#define QPNP_HAP_LRA_AUTO_MASK 0x70

/* Turn on vibrator */
void pm_haptic_vib_turn_on(void)
{
	struct qpnp_hap vib_config = {0};
	uint32_t pmic = target_get_pmic();
	uint32_t slave_id = 1;

	if(!target_is_pmi_enabled() && (pmic != PMIC_IS_PM660))
		return;

	if (pmic == PMIC_IS_PM660)
		slave_id = 0;

	get_vibration_type(&vib_config);
	/* Configure the ACTUATOR TYPE register as ERM*/
	pmic_spmi_reg_mask_write(QPNP_HAP_ACT_TYPE_REG(slave_id),
					QPNP_HAP_ACT_TYPE_MASK,
					VIB_ERM_TYPE == vib_config.vib_type ? QPNP_HAP_ERM
					: QPNP_HAP_LRA);

	/* Disable auto resonance for ERM */
	pmic_spmi_reg_mask_write(QPNP_HAP_LRA_AUTO_RES_REG(slave_id),
					QPNP_HAP_LRA_AUTO_MASK,
					QPNP_HAP_LRA_AUTO_DISABLE);

	/* Configure the PLAY MODE register as direct*/
	pmic_spmi_reg_mask_write(QPNP_HAP_PLAY_MODE_REG(slave_id),
					QPNP_HAP_PLAY_MODE_MASK,
					QPNP_HAP_DIRECT);

	/* Configure the VMAX register */
	pmic_spmi_reg_mask_write(QPNP_HAP_VMAX_REG(slave_id),
					QPNP_HAP_VMAX_MASK, QPNP_HAP_VMAX);

	/* Sets current limit to 800mA*/
	pmic_spmi_reg_mask_write(QPNP_HAP_ILIM_REG(slave_id),
					QPNP_HAP_ILIM_MASK, QPNP_HAP_ILIM);

	/* Configure the short circuit debounce register as DEB_8CLK*/
	pmic_spmi_reg_mask_write(QPNP_HAP_SC_DEB_REG(slave_id),
					QPNP_HAP_SC_DEB_MASK, QPNP_HAP_SC_DEB_8CLK);

	/* Configure the INTERNAL_PWM register as 505KHZ and 13PF*/
	pmic_spmi_reg_mask_write(QPNP_HAP_INT_PWM_REG(slave_id),
					QPNP_HAP_INT_PWM_MASK, QPNP_HAP_INT_PWM_505KHZ);
	pmic_spmi_reg_mask_write(QPNP_HAP_PWM_CAP_REG(slave_id),
					QPNP_HAP_PWM_CAP_MASK, QPNP_HAP_PWM_CAP_13PF);

	/* Configure the WAVE SHAPE register as SQUARE*/
	pmic_spmi_reg_mask_write(QPNP_HAP_WAV_SHAPE_REG(slave_id),
					QPNP_HAP_WAV_SHAPE_MASK, QPNP_HAP_WAV_SHAPE_SQUARE);

	/* Configure RATE_CFG1 and RATE_CFG2 registers for haptic rate. */
	pmic_spmi_reg_mask_write(QPNP_HAP_RATE_CFG1_REG(slave_id),
					QPNP_HAP_RATE_CFG1_MASK, vib_config.hap_rate_cfg1);
	pmic_spmi_reg_mask_write(QPNP_HAP_RATE_CFG2_REG(slave_id),
					QPNP_HAP_RATE_CFG2_MASK, vib_config.hap_rate_cfg2);

	/* Configure BRAKE register, PATTERN1 & PATTERN2 as VMAX. */
	pmic_spmi_reg_mask_write(QPNP_HAP_EN_CTL2_REG(slave_id),
					QPNP_HAP_EN_BRAKE_EN_MASK, QPNP_HAP_EN_BRAKING_EN);
	pmic_spmi_reg_mask_write(QPNP_HAP_BRAKE_REG(slave_id),
					QPNP_HAP_BRAKE_VMAX_MASK, QPNP_HAP_BRAKE_VMAX);

	/* Enable control register */
	pmic_spmi_reg_mask_write(QPNP_HAP_EN_CTL_REG(slave_id),
					QPNP_HAP_PLAY_MASK, QPNP_HAP_PLAY_EN);

	/* Enable play register */
	pmic_spmi_reg_mask_write(QPNP_HAP_PLAY_REG(slave_id), QPNP_HAP_MASK, QPNP_HAP_EN);
}

/* Turn off vibrator */
void pm_haptic_vib_turn_off(void)
{
	uint32_t pmic = target_get_pmic();
	uint32_t slave_id = 1;

	if(!target_is_pmi_enabled() && (pmic != PMIC_IS_PM660))
		return;

	if (pmic == PMIC_IS_PM660)
		slave_id = 0;

	/* Disable control register */
	pmic_spmi_reg_mask_write(QPNP_HAP_EN_CTL_REG(slave_id),
					QPNP_HAP_PLAY_MASK, QPNP_HAP_PLAY_DIS);

	/* Disable play register */
	pmic_spmi_reg_mask_write(QPNP_HAP_PLAY_REG(slave_id), QPNP_HAP_MASK, QPNP_HAP_DIS);
}
