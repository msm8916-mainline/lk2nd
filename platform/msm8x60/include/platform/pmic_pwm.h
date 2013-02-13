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

#ifndef __PMIC_PWM_H
#define __PMIC_PWM_H

#define USEC_PER_SEC		1000000L
#define NSEC_PER_SEC		1000000000L
#define NSEC_PER_USEC		1000

#define PWM_FREQ_HZ		300
#define PWM_LEVEL		15

#define NUM_CLOCKS		3
#define NUM_PRE_DIVIDE		3

#define NUM_LPG_CTL_REGS		7

#define PRE_DIVIDE_0		2
#define PRE_DIVIDE_1		3
#define PRE_DIVIDE_2		5

#define PRE_DIVIDE_MIN		PRE_DIVIDE_0
#define PRE_DIVIDE_MAX		PRE_DIVIDE_2

#define PM_PWM_M_MIN		0
#define PM_PWM_M_MAX		7

#define NSEC_1000HZ		(NSEC_PER_SEC / 1000)
#define NSEC_32768HZ		(NSEC_PER_SEC / 32768)
#define NSEC_19P2MHZ		(NSEC_PER_SEC / 19200000)

#define CLK_PERIOD_MIN		NSEC_19P2MHZ
#define CLK_PERIOD_MAX		NSEC_1000HZ

#define MIN_MPT		((PRE_DIVIDE_MIN * CLK_PERIOD_MIN) << PM_PWM_M_MIN)
#define MAX_MPT		((PRE_DIVIDE_MAX * CLK_PERIOD_MAX) << PM_PWM_M_MAX)

/* The MAX value is computation limit. Hardware limit is 393 seconds. */
#define PM_PWM_PERIOD_MAX		(274 * USEC_PER_SEC)
/* The MIN value is hardware limit. */
#define PM_PWM_PERIOD_MIN		7	/* micro seconds */

#define PWM_PERIOD_USEC		(USEC_PER_SEC / PWM_FREQ_HZ)
#define PWM_DUTY_LEVEL		(PWM_PERIOD_USEC / PWM_LEVEL)

/* Control 0 */
#define PM_PWM_1KHZ_COUNT_MASK		0xF0
#define PM_PWM_1KHZ_COUNT_SHIFT		4

#define PM_PWM_1KHZ_COUNT_MAX		15

#define PM_PWM_OUTPUT_EN		0x08
#define PM_PWM_PWM_EN		0x04
#define PM_PWM_RAMP_GEN_EN		0x02
#define PM_PWM_RAMP_START		0x01

#define PM_PWM_PWM_START		(PM_PWM_OUTPUT_EN \
				| PM_PWM_PWM_EN)
#define PM_PWM_RAMP_GEN_START	(PM_PWM_RAMP_GEN_EN \
				| PM_PWM_RAMP_START)

/* Control 1 */
#define PM_PWM_REVERSE_EN		0x80
#define PM_PWM_BYPASS_LUT		0x40
#define PM_PWM_HIGH_INDEX_MASK		0x3F

/* Control 2 */
#define PM_PWM_LOOP_EN		0x80
#define PM_PWM_RAMP_UP		0x40
#define PM_PWM_LOW_INDEX_MASK		0x3F

/* Control 3 */
#define PM_PWM_VALUE_BIT7_0		0xFF
#define PM_PWM_VALUE_BIT5_0		0x3F

/* Control 4 */
#define PM_PWM_VALUE_BIT8		0x80

#define PM_PWM_CLK_SEL_MASK		0x60
#define PM_PWM_CLK_SEL_SHIFT		5

#define PM_PWM_CLK_SEL_NO		0
#define PM_PWM_CLK_SEL_1KHZ		1
#define PM_PWM_CLK_SEL_32KHZ		2
#define PM_PWM_CLK_SEL_19P2MHZ		3

#define PM_PWM_PREDIVIDE_MASK		0x18
#define PM_PWM_PREDIVIDE_SHIFT		3

#define PM_PWM_PREDIVIDE_2		0
#define PM_PWM_PREDIVIDE_3		1
#define PM_PWM_PREDIVIDE_5		2
#define PM_PWM_PREDIVIDE_6		3

#define PM_PWM_M_MASK		0x07
#define PM_PWM_M_MIN		0
#define PM_PWM_M_MAX		7

/* Control 5 */
#define PM_PWM_PAUSE_COUNT_HI_MASK		0xFC
#define PM_PWM_PAUSE_COUNT_HI_SHIFT		2

#define PM_PWM_PAUSE_ENABLE_HIGH		0x02
#define PM_PWM_SIZE_9_BIT		0x01

/* Control 6 */
#define PM_PWM_PAUSE_COUNT_LO_MASK		0xFC
#define PM_PWM_PAUSE_COUNT_LO_SHIFT		2

#define PM_PWM_PAUSE_ENABLE_LOW		0x02
#define PM_PWM_RESERVED		0x01

#define PM_PWM_PAUSE_COUNT_MAX		56	/* < 2^6 = 64 */

struct pm_pwm_config {
	int pwm_size;		/* round up to 6 or 9 for 6/9-bit PWM SIZE */
	int clk;
	int pre_div;
	int pre_div_exp;
	int pwm_value;
	int bypass_lut;
	uint8_t pwm_ctl[NUM_LPG_CTL_REGS];
};

#endif
