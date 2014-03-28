/*
 * * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#ifndef __PMIC_PWM_H
#define __PMIC_PWM_H

#define PM_PWM_SIZE_SEL_MASK				0x04
#define PM_PWM_SIZE_SEL_SHIFT				2

#define PM_PWM_CLK_SEL_MASK				0x03

#define PM_PWM_PREDIVIDE_MASK				0x60
#define PM_PWM_PREDIVIDE_SHIFT				5

#define PM_PWM_M_MASK					0x07

#define PM_PWM_SYNC_MASK				0x01

#define PM_PWM_ENABLE_CTL_MASK				0x80
#define PM_PWM_ENABLE_CTL_SHIFT				7

#define PM_PWM_EN_GLITCH_REMOVAL_MASK			0x20
#define PM_PWM_EN_GLITCH_REMOVAL_SHIFT			5

#define PM_PWM_VALUE_BIT7_0				0xFF
#define PM_PWM_VALUE_BIT8				0x01
#define PM_PWM_VALUE_BIT5_0				0x3F

#define PM_PWM_BASE(x)					(0x1BC00 + (x))
#define PM_PWM_CTL_REG_OFFSET				0x41
#define PM_PWM_SYNC_REG_OFFSET				0x47
#define PM_PWM_ENABLE_CTL_REG_OFFSET			0x46

int pm_pwm_config(unsigned int duty_us, unsigned int period_us);
void pm_pwm_enable(bool enable);

#endif
