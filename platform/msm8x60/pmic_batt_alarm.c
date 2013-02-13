/*
 * * Copyright (c) 2011, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <debug.h>
#include <platform/pmic_batt_alarm.h>

static struct pm_batt_alarm_device battdev;

/*
 * Function to set threshold voltages for battery alarm
 */

static int pm_batt_alarm_threshold_set(uint32_t lower_threshold_mV,
				       uint32_t upper_threshold_mV)
{
	uint32_t step, fine_step, rc = -1;
	uint8_t reg_threshold = 0, reg_ctrl2 = 0;

	if (lower_threshold_mV < THRESHOLD_MIN_MV
	    || lower_threshold_mV > THRESHOLD_MAX_MV) {
		dprintf(CRITICAL,
			"lower threshold value, %d mV, is outside of allowable "
			"range: [%d, %d] mV\n", lower_threshold_mV,
			THRESHOLD_MIN_MV, THRESHOLD_MAX_MV);
		goto bail;
	}

	if (upper_threshold_mV < THRESHOLD_MIN_MV
	    || upper_threshold_mV > THRESHOLD_MAX_MV) {
		dprintf(CRITICAL,
			"upper threshold value, %d mV, is outside of allowable "
			"range: [%d, %d] mV\n", upper_threshold_mV,
			THRESHOLD_MIN_MV, THRESHOLD_MAX_MV);
		goto bail;
	}

	if (upper_threshold_mV < lower_threshold_mV) {
		dprintf(CRITICAL,
			"lower threshold value, %d mV, must be <= upper "
			"threshold value, %d mV\n", lower_threshold_mV,
			upper_threshold_mV);
		goto bail;
	}

	/* Determine register settings for lower threshold. */
	if (lower_threshold_mV < THRESHOLD_BASIC_MIN_MV) {
		/* Extended low range */
		reg_ctrl2 |= CTRL2_RANGE_EXT_LOWER_MASK;

		step = (lower_threshold_mV - THRESHOLD_MIN_MV)
		    / THRESHOLD_STEP_MV;

		fine_step = step & 0x3;
		/* Extended low range is for steps 0 to 2 */
		step >>= 2;

	} else if (lower_threshold_mV >= THRESHOLD_EXT_MIN_MV) {
		/* Extended high range */
		reg_ctrl2 |= CTRL2_RANGE_EXT_LOWER_MASK;

		step = (lower_threshold_mV - THRESHOLD_EXT_MIN_MV)
		    / THRESHOLD_STEP_MV;

		fine_step = step & 0x3;
		/* Extended high range is for steps 3 to 15 */
		step = (step >> 2) + 3;

	} else {
		/* Basic range */
		step = (lower_threshold_mV - THRESHOLD_BASIC_MIN_MV)
		    / THRESHOLD_STEP_MV;

		fine_step = step & 0x3;
		step >>= 2;

	}

	reg_threshold |= (step << THRESHOLD_LOWER_SHIFT);
	reg_ctrl2 |= (fine_step << CTRL2_FINE_STEP_LOWER_SHIFT);

	/* Determine register settings for upper threshold. */
	if (upper_threshold_mV < THRESHOLD_BASIC_MIN_MV) {
		/* Extended low range */
		reg_ctrl2 |= CTRL2_RANGE_EXT_UPPER_MASK;

		step = (upper_threshold_mV - THRESHOLD_MIN_MV)
		    / THRESHOLD_STEP_MV;

		fine_step = step & 0x3;
		/* Extended low range is for steps 0 to 2 */
		step >>= 2;

	} else if (upper_threshold_mV >= THRESHOLD_EXT_MIN_MV) {
		/* Extended high range */
		reg_ctrl2 |= CTRL2_RANGE_EXT_UPPER_MASK;

		step = (upper_threshold_mV - THRESHOLD_EXT_MIN_MV)
		    / THRESHOLD_STEP_MV;

		fine_step = step & 0x3;
		/* Extended high range is for steps 3 to 15 */
		step = (step >> 2) + 3;

	} else {
		/* Basic range */
		step = (upper_threshold_mV - THRESHOLD_BASIC_MIN_MV)
		    / THRESHOLD_STEP_MV;

		fine_step = step & 0x3;
		step >>= 2;

	}

	reg_threshold |= (step << THRESHOLD_UPPER_SHIFT);
	reg_ctrl2 |= (fine_step << CTRL2_FINE_STEP_UPPER_SHIFT);

	rc = pm8058_mwrite(PM8058_REG_THRESHOLD, reg_threshold,
			   THRESHOLD_LOWER_MASK | THRESHOLD_UPPER_MASK,
			   &battdev.reg_threshold);
	if (rc) {
		dprintf(CRITICAL, "Error in pm8058_mwrite THRESHOLD\n");
		goto bail;
	}

	rc = pm8058_mwrite(PM8058_REG_CTRL2, reg_ctrl2,
			   CTRL2_FINE_STEP_LOWER_MASK |
			   CTRL2_FINE_STEP_UPPER_MASK |
			   CTRL2_RANGE_EXT_LOWER_MASK |
			   CTRL2_RANGE_EXT_UPPER_MASK, &battdev.reg_ctrl2);

	if (rc)
		dprintf(CRITICAL, "Error in pm8058_mwrite CTRL2\n");

 bail:
	return rc;
}

/*
 * Function to set hold time (hysteresis) for battery alarm
 */

static int pm_batt_alarm_hold_time_set(pm_batt_alarm_hold_time hold_time)
{
	int rc = -1;
	uint8_t reg_ctrl1 = 0;

	if (hold_time < CTRL1_HOLD_TIME_MIN || hold_time > CTRL1_HOLD_TIME_MAX) {

		dprintf(CRITICAL,
			"hold time, %d, is outside of allowable range: "
			"[%d, %d]\n", hold_time, CTRL1_HOLD_TIME_MIN,
			CTRL1_HOLD_TIME_MAX);
		goto bail;
	}

	reg_ctrl1 = hold_time << CTRL1_HOLD_TIME_SHIFT;

	rc = pm8058_mwrite(PM8058_REG_CTRL1, reg_ctrl1,
			   CTRL1_HOLD_TIME_MASK, &battdev.reg_ctrl1);

	if (rc)
		dprintf(CRITICAL, "Error in pm8058_mwrite CTRL1\n");

 bail:
	return rc;
}

/*
 * Function to set PWM clock rate for battery alarm
 */

static int pm_batt_alarm_pwm_rate_set(pm_batt_alarm_pwm_ctrl pwm_ctrl_select,
				      uint32_t clock_scaler,
				      uint32_t clock_divider)
{
	int rc = -1;
	uint8_t reg_pwm_ctrl = 0, mask = 0;

	if (pwm_ctrl_select == ALARM_EN_PWM) {
		if (clock_scaler < PWM_CTRL_PRE_INPUT_MIN
		    || clock_scaler > PWM_CTRL_PRE_INPUT_MAX) {
			dprintf(CRITICAL,
				"PWM clock scaler, %d, is outside of allowable range: "
				"[%d, %d]\n", clock_scaler,
				PWM_CTRL_PRE_INPUT_MIN, PWM_CTRL_PRE_INPUT_MAX);
			goto bail;
		}

		if (clock_divider < PWM_CTRL_DIV_INPUT_MIN
		    || clock_divider > PWM_CTRL_DIV_INPUT_MAX) {
			dprintf(CRITICAL,
				"PWM clock divider, %d, is outside of allowable range: "
				"[%d, %d]\n", clock_divider,
				PWM_CTRL_DIV_INPUT_MIN, PWM_CTRL_DIV_INPUT_MAX);
			goto bail;
		}

		/* Use PWM control. */
		reg_pwm_ctrl = PWM_CTRL_ALARM_EN_PWM;
		mask = PWM_CTRL_ALARM_EN_MASK | PWM_CTRL_PRE_MASK
		    | PWM_CTRL_DIV_MASK;

		clock_scaler -= PWM_CTRL_PRE_INPUT_MIN - PWM_CTRL_PRE_MIN;
		clock_divider -= PWM_CTRL_DIV_INPUT_MIN - PWM_CTRL_DIV_MIN;

		reg_pwm_ctrl |= (clock_scaler << PWM_CTRL_PRE_SHIFT);
		reg_pwm_ctrl |= (clock_divider << PWM_CTRL_DIV_SHIFT);
	} else {
		if (pwm_ctrl_select == ALARM_EN_ALWAYS) {
			reg_pwm_ctrl = PWM_CTRL_ALARM_EN_ALWAYS;
		} else if (pwm_ctrl_select == ALARM_EN_NEVER) {
			reg_pwm_ctrl = PWM_CTRL_ALARM_EN_NEVER;
		}

		mask = PWM_CTRL_ALARM_EN_MASK;
	}

	rc = pm8058_mwrite(PM8058_REG_PWM_CTRL, reg_pwm_ctrl,
			   mask, &battdev.reg_pwm_ctrl);

	if (rc)
		dprintf(CRITICAL, "Error in pm8058_mwrite PWM_CTRL\n");

 bail:
	return rc;
}

/*
 * Function to enable/disable alarm comparators for battery alarm
 */

static int pm_batt_alarm_state_set(uint8_t enable_lower_comparator,
				   uint8_t enable_upper_comparator)
{
	int rc = -1;
	uint8_t reg_ctrl1 = 0, reg_ctrl2 = 0;

	if (!enable_lower_comparator)
		reg_ctrl2 |= CTRL2_COMP_LOWER_DISABLE_MASK;
	if (!enable_upper_comparator)
		reg_ctrl2 |= CTRL2_COMP_UPPER_DISABLE_MASK;

	if (enable_lower_comparator || enable_upper_comparator)
		reg_ctrl1 = CTRL1_BATT_ALARM_EN_MASK;

	rc = pm8058_mwrite(PM8058_REG_CTRL1, reg_ctrl1,
			   CTRL1_BATT_ALARM_EN_MASK, &battdev.reg_ctrl1);
	if (rc) {
		dprintf(CRITICAL, "Error in pm8058_mwrite CTRL1\n");
		goto bail;
	}

	rc = pm8058_mwrite(PM8058_REG_CTRL2, reg_ctrl2,
			   CTRL2_COMP_LOWER_DISABLE_MASK |
			   CTRL2_COMP_UPPER_DISABLE_MASK, &battdev.reg_ctrl2);

	if (rc)
		dprintf(CRITICAL, "Error in pm8058_mwrite CTRL2\n");

 bail:
	return rc;
}

/*
 * Function to read alarm status for battery alarm
 */

int pm_batt_alarm_status_read(uint8_t * status)
{
	int rc = -1;

	rc = pm8058_read(PM8058_REG_CTRL1, &battdev.reg_ctrl1, 1);
	if (rc) {
		dprintf(CRITICAL, "Error in reading CTRL1\n");
		goto bail;
	}

	*status = ((battdev.reg_ctrl1 & CTRL1_STATUS_LOWER_MASK)
		   ? PM_BATT_ALARM_STATUS_BELOW_LOWER : 0)
	    | ((battdev.reg_ctrl1 & CTRL1_STATUS_UPPER_MASK)
	       ? PM_BATT_ALARM_STATUS_ABOVE_UPPER : 0);

	/* Disabling Battery alarm below just for power savings. This can be
	 * removed if this does not matter
	 */

	/* After reading, disabling the comparators and BATT_ALARM_EN */
	rc = pm_batt_alarm_state_set(LOWER_COMP_DISABLE, UPPER_COMP_DISABLE);
	if (rc) {
		dprintf(CRITICAL, "state_set failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_pwm_rate_set(ALARM_EN_NEVER, DEFAULT_PWM_SCALER,
					DEFAULT_PWM_DIVIDER);
	if (rc) {
		dprintf(CRITICAL, "state_set failed, rc=%d\n", rc);
		goto bail;
	}

 bail:
	return rc;
}

/*
 * Function to read and print battery alarm registers for debugging
 */

int pm_batt_alarm_read_regs(struct pm_batt_alarm_device *battdev)
{
	int rc = -1;

	if (battdev) {
		rc = pm8058_read(PM8058_REG_THRESHOLD, &battdev->reg_threshold,
				 1);
		if (rc)
			goto done;

		rc = pm8058_read(PM8058_REG_CTRL1, &battdev->reg_ctrl1, 1);
		if (rc)
			goto done;

		rc = pm8058_read(PM8058_REG_CTRL2, &battdev->reg_ctrl2, 1);
		if (rc)
			goto done;

		rc = pm8058_read(PM8058_REG_PWM_CTRL, &battdev->reg_pwm_ctrl,
				 1);
		if (rc)
			goto done;
	}

 done:
	if (rc)
		dprintf(CRITICAL, "pm_batt_alarm_read_regs read error\n");
	return rc;
}

/*
 * Function for battery alarm initialization
 */

int pm_batt_alarm_init()
{
	int rc = -1;

	rc = pm_batt_alarm_read_regs(&battdev);
	if (rc) {
		dprintf(CRITICAL, "read_regs failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_threshold_set(DEFAULT_THRESHOLD_LOWER,
					 DEFAULT_THRESHOLD_UPPER);
	if (rc) {
		dprintf(CRITICAL, "threshold_set failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_hold_time_set(DEFAULT_HOLD_TIME);
	if (rc) {
		dprintf(CRITICAL, "hold_time_set failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_pwm_rate_set(ALARM_EN_NEVER, DEFAULT_PWM_SCALER,
					DEFAULT_PWM_DIVIDER);
	if (rc) {
		dprintf(CRITICAL, "pwm_rate_set failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_state_set(LOWER_COMP_DISABLE, UPPER_COMP_DISABLE);
	if (rc)
		dprintf(CRITICAL, "state_set failed, rc=%d\n", rc);

 bail:
	return rc;
}

/*
 * Function to configure voltages and change alarm state
 */

int pm_batt_alarm_set_voltage(uint32_t lower_threshold,
			      uint32_t upper_threshold)
{
	int rc = -1;

	rc = pm_batt_alarm_threshold_set(lower_threshold, upper_threshold);
	if (rc) {
		dprintf(CRITICAL, "threshold_set failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_pwm_rate_set(ALARM_EN_ALWAYS, DEFAULT_PWM_SCALER,
					DEFAULT_PWM_DIVIDER);
	if (rc) {
		dprintf(CRITICAL, "pwm_rate_set failed, rc=%d\n", rc);
		goto bail;
	}

	rc = pm_batt_alarm_state_set(LOWER_COMP_ENABLE, UPPER_COMP_ENABLE);
	if (rc)
		dprintf(CRITICAL, "state_set failed, rc=%d\n", rc);

 bail:
	return rc;
}

/*
 * Function to test battery alarms
 */

void pm_ba_test(void)
{
	int rc = 0;
	uint8_t batt_status = 0;

	rc = pm_batt_alarm_init();
	if (rc)
		dprintf(CRITICAL, "pm_batt_alarm_init error\n");

	/* wait till hold time */
	mdelay(16);

	/* 0xe74-> 3700mV, 0x1004-> 4100mv */
	rc = pm_batt_alarm_set_voltage(0xe74, 0x1004);
	if (rc)
		dprintf(CRITICAL, "pm_batt_alarm_set_voltage error\n");

	/* wait till hold time */
	mdelay(16);

	rc = pm_batt_alarm_status_read(&batt_status);
	if (rc)
		dprintf(CRITICAL, "pm_batt_alarm_status_read error\n");
	else
		dprintf(ALWAYS, "batt status: %d\n", batt_status);
}
