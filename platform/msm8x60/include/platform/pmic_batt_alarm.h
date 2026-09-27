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

#ifndef __PMIC_BATT_ALARM_H
#define __PMIC_BATT_ALARM_H

#define BIT(x)			(1 << (x))

/* PM8058 Battery alarm control registers */
#define PM8058_REG_THRESHOLD			0x023
#define PM8058_REG_CTRL1			0x024
#define PM8058_REG_CTRL2			0x0AA
#define PM8058_REG_PWM_CTRL			0x0A3

/* Available voltage threshold values */
#define THRESHOLD_MIN_MV			2500
#define THRESHOLD_MAX_MV			5675
#define THRESHOLD_STEP_MV			25

/* Register bit definitions */

/* Threshold register */
#define THRESHOLD_UPPER_MASK			0xF0
#define THRESHOLD_LOWER_MASK			0x0F
#define THRESHOLD_UPPER_SHIFT			4
#define THRESHOLD_LOWER_SHIFT			0

/* CTRL 1 register */
#define CTRL1_BATT_ALARM_EN_MASK			0x80
#define CTRL1_HOLD_TIME_MASK			0x70
#define CTRL1_STATUS_UPPER_MASK			0x02
#define CTRL1_STATUS_LOWER_MASK			0x01
#define CTRL1_HOLD_TIME_SHIFT			4
#define CTRL1_HOLD_TIME_MIN			0
#define CTRL1_HOLD_TIME_MAX			7

/* CTRL 2 register */
#define CTRL2_COMP_UPPER_DISABLE_MASK			0x80
#define CTRL2_COMP_LOWER_DISABLE_MASK			0x40
#define CTRL2_FINE_STEP_UPPER_MASK			0x30
#define CTRL2_RANGE_EXT_UPPER_MASK			0x08
#define CTRL2_FINE_STEP_LOWER_MASK			0x06
#define CTRL2_RANGE_EXT_LOWER_MASK			0x01
#define CTRL2_FINE_STEP_UPPER_SHIFT			4
#define CTRL2_FINE_STEP_LOWER_SHIFT			1

/* PWM control register */
#define PWM_CTRL_ALARM_EN_MASK			0xC0
#define PWM_CTRL_ALARM_EN_NEVER			0x00
#define PWM_CTRL_ALARM_EN_TCXO			0x40
#define PWM_CTRL_ALARM_EN_PWM			0x80
#define PWM_CTRL_ALARM_EN_ALWAYS			0xC0
#define PWM_CTRL_PRE_MASK			0x38
#define PWM_CTRL_DIV_MASK			0x07
#define PWM_CTRL_PRE_SHIFT			3
#define PWM_CTRL_DIV_SHIFT			0
#define PWM_CTRL_PRE_MIN			0
#define PWM_CTRL_PRE_MAX			7
#define PWM_CTRL_DIV_MIN			1
#define PWM_CTRL_DIV_MAX			7

/* PWM control input range */
#define PWM_CTRL_PRE_INPUT_MIN			2
#define PWM_CTRL_PRE_INPUT_MAX			9
#define PWM_CTRL_DIV_INPUT_MIN			2
#define PWM_CTRL_DIV_INPUT_MAX			8

/* Available voltage threshold values */
#define THRESHOLD_BASIC_MIN_MV			2800
#define THRESHOLD_EXT_MIN_MV			4400

#define DEFAULT_THRESHOLD_LOWER			3200
#define DEFAULT_THRESHOLD_UPPER			4300
#define DEFAULT_HOLD_TIME			HOLD_TIME_16_MS
#define DISABLE_USE_PWM			0
#define ENABLE_USE_PWM			1
#define DEFAULT_PWM_SCALER			9
#define DEFAULT_PWM_DIVIDER			8
#define LOWER_COMP_DISABLE			0
#define UPPER_COMP_DISABLE			0
#define LOWER_COMP_ENABLE			1
#define UPPER_COMP_ENABLE			1

#define PM_BATT_ALARM_STATUS_BELOW_LOWER			BIT(0)
#define PM_BATT_ALARM_STATUS_ABOVE_UPPER			BIT(1)

typedef enum {
	HOLD_TIME_0p125_MS = 0,
	HOLD_TIME_0p25_MS,
	HOLD_TIME_0p5_MS,
	HOLD_TIME_1_MS,
	HOLD_TIME_2_MS,
	HOLD_TIME_4_MS,
	HOLD_TIME_8_MS,
	HOLD_TIME_16_MS,
} pm_batt_alarm_hold_time;

typedef enum {
	ALARM_EN_NEVER = 0,
	ALARM_EN_PWM,
	ALARM_EN_ALWAYS,
} pm_batt_alarm_pwm_ctrl;

struct pm_batt_alarm_device {
	uint8_t reg_threshold;
	uint8_t reg_ctrl1;
	uint8_t reg_ctrl2;
	uint8_t reg_pwm_ctrl;
};

extern int pm8058_mwrite(uint16_t addr, uint8_t val, uint8_t mask,
			 uint8_t * reg_save);
extern int pm8058_read(uint16_t addr, uint8_t * data, uint16_t length);
extern void mdelay(uint32_t ms);

#endif
