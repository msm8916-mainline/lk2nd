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

#include <debug.h>
#include <pm_pwm.h>
#include <pm8x41_hw.h>

#define NSEC_PER_USEC		1000L
#define USEC_PER_SEC		1000000L
#define NSEC_PER_SEC		1000000000L

#define NUM_REF_CLOCKS		3
#define NSEC_1024HZ			(NSEC_PER_SEC / 1024)
#define NSEC_32768HZ		(NSEC_PER_SEC / 32768)
#define NSEC_19P2MHZ		(NSEC_PER_SEC / 19200000)

#define NUM_PRE_DIVIDE		4
#define PRE_DIVIDE_1		1
#define PRE_DIVIDE_3		3
#define PRE_DIVIDE_5		5
#define PRE_DIVIDE_6		6
static unsigned int pt_t[NUM_PRE_DIVIDE][NUM_REF_CLOCKS] = {
	{	PRE_DIVIDE_1 * NSEC_1024HZ,
		PRE_DIVIDE_1 * NSEC_32768HZ,
		PRE_DIVIDE_1 * NSEC_19P2MHZ,
	},
	{	PRE_DIVIDE_3 * NSEC_1024HZ,
		PRE_DIVIDE_3 * NSEC_32768HZ,
		PRE_DIVIDE_3 * NSEC_19P2MHZ,
	},
	{	PRE_DIVIDE_5 * NSEC_1024HZ,
		PRE_DIVIDE_5 * NSEC_32768HZ,
		PRE_DIVIDE_5 * NSEC_19P2MHZ,
	},
	{	PRE_DIVIDE_6 * NSEC_1024HZ,
		PRE_DIVIDE_6 * NSEC_32768HZ,
		PRE_DIVIDE_6 * NSEC_19P2MHZ,
	},
};

enum pwm_ctl_reg {
	SIZE_CLK,
	FREQ_PREDIV_CLK,
	TYPE_CONFIG,
	VALUE_LSB,
	VALUE_MSB,
};

#define NUM_PWM_CTL_REGS	5
struct pm_pwm_config {
	int pwm_size;		/* round up to 6 or 9 for 6/9-bit PWM SIZE */
	int clk;
	int pre_div;
	int pre_div_exp;
	int pwm_value;
	uint8_t pwm_ctl[NUM_PWM_CTL_REGS];
};

static void pm_pwm_reg_write(uint8_t off, uint8_t val)
{
	REG_WRITE(PM_PWM_BASE(off), val);
}

/*
 * PWM Frequency = Clock Frequency / (N * T)
 *	or
 * PWM Period = Clock Period * (N * T)
 *	where
 * N = 2^9 or 2^6 for 9-bit or 6-bit PWM size
 * T = Pre-divide * 2^m, where m = 0..7 (exponent)
 *
 * This is the formula to figure out m for the best pre-divide and clock:
 * (PWM Period / N) = (Pre-divide * Clock Period) * 2^m
 */

#define PRE_DIVIDE_MAX		6
#define CLK_PERIOD_MAX		NSEC_1024HZ
#define PM_PWM_M_MAX		7
#define MAX_MPT				((PRE_DIVIDE_MAX * CLK_PERIOD_MAX) << PM_PWM_M_MAX)
static void pm_pwm_calc_period(unsigned int period_us,
				   struct pm_pwm_config *pwm_config)
{
	int	n, m, clk, div;
	int	best_m, best_div, best_clk;
	unsigned int last_err, cur_err, min_err;
	unsigned int tmp_p, period_n;

	n = 6;

	if (period_us < ((unsigned)(-1) / NSEC_PER_USEC))
		period_n = (period_us * NSEC_PER_USEC) >> n;
	else
		period_n = (period_us >> n) * NSEC_PER_USEC;

	if (period_n >= MAX_MPT) {
		n = 9;
		period_n >>= 3;
	}

	min_err = last_err = (unsigned)(-1);
	best_m = 0;
	best_clk = 0;
	best_div = 0;
	for (clk = 0; clk < NUM_REF_CLOCKS; clk++) {
		for (div = 0; div < NUM_PRE_DIVIDE; div++) {
			/* period_n = (PWM Period / N) */
			/* tmp_p = (Pre-divide * Clock Period) * 2^m */
			tmp_p = pt_t[div][clk];
			for (m = 0; m <= PM_PWM_M_MAX; m++) {
				if (period_n > tmp_p)
					cur_err = period_n - tmp_p;
				else
					cur_err = tmp_p - period_n;

				if (cur_err < min_err) {
					min_err = cur_err;
					best_m = m;
					best_clk = clk;
					best_div = div;
				}

				if (m && cur_err > last_err)
					/* Break for bigger cur_err */
					break;

				last_err = cur_err;
				tmp_p <<= 1;
			}
		}
	}

	pwm_config->pwm_size = n;
	pwm_config->clk = best_clk;
	pwm_config->pre_div = best_div;
	pwm_config->pre_div_exp = best_m;
}

static void pm_pwm_calc_pwm_value(struct pm_pwm_config *pwm_config,
				      unsigned int period_us,
				      unsigned int duty_us)
{
	unsigned int max_pwm_value, tmp;

	/* Figure out pwm_value with overflow handling */
	tmp = 1 << (sizeof(tmp) * 8 - pwm_config->pwm_size);
	if (duty_us < tmp) {
		tmp = duty_us << pwm_config->pwm_size;
		pwm_config->pwm_value = tmp / period_us;
	} else {
		tmp = period_us >> pwm_config->pwm_size;
		pwm_config->pwm_value = duty_us / tmp;
	}
	max_pwm_value = (1 << pwm_config->pwm_size) - 1;
	if (pwm_config->pwm_value > max_pwm_value)
		pwm_config->pwm_value = max_pwm_value;
}

#define PM_PWM_SIZE_9_BIT	1
#define PM_PWM_SIZE_6_BIT	0
static void pm_pwm_config_regs(struct pm_pwm_config *pwm_config)
{
	int i;
	uint8_t reg;

	reg = ((pwm_config->pwm_size > 6 ? PM_PWM_SIZE_9_BIT : PM_PWM_SIZE_6_BIT)
		<< PM_PWM_SIZE_SEL_SHIFT)
		& PM_PWM_SIZE_SEL_MASK;
	reg |= (pwm_config->clk + 1) & PM_PWM_CLK_SEL_MASK;
	pwm_config->pwm_ctl[SIZE_CLK] = reg;

	reg = (pwm_config->pre_div << PM_PWM_PREDIVIDE_SHIFT)
	    & PM_PWM_PREDIVIDE_MASK;
	reg |= pwm_config->pre_div_exp & PM_PWM_M_MASK;
	pwm_config->pwm_ctl[FREQ_PREDIV_CLK] = reg;

	/* Enable glitch removal by default */
	reg = 1 << PM_PWM_EN_GLITCH_REMOVAL_SHIFT
		& PM_PWM_EN_GLITCH_REMOVAL_MASK;
	pwm_config->pwm_ctl[TYPE_CONFIG] = reg;

	if (pwm_config->pwm_size > 6) {
		pwm_config->pwm_ctl[VALUE_LSB] = pwm_config->pwm_value
				& PM_PWM_VALUE_BIT7_0;
		pwm_config->pwm_ctl[VALUE_MSB] = (pwm_config->pwm_value >> 8)
				& PM_PWM_VALUE_BIT8;
	} else
		pwm_config->pwm_ctl[VALUE_LSB] = pwm_config->pwm_value
			    & PM_PWM_VALUE_BIT5_0;

	for (i = 0; i < NUM_PWM_CTL_REGS; i++)
		pm_pwm_reg_write(PM_PWM_CTL_REG_OFFSET + i, pwm_config->pwm_ctl[i]);

	reg = 1 & PM_PWM_SYNC_MASK;
	pm_pwm_reg_write(PM_PWM_SYNC_REG_OFFSET, reg);
}

/* usec: 19.2M, n=6, m=0, pre=2 */
#define PM_PWM_PERIOD_MIN	7
/* 1K, n=9, m=7, pre=6 */
#define PM_PWM_PERIOD_MAX	(384 * USEC_PER_SEC)
int pm_pwm_config(unsigned int duty_us, unsigned int period_us)
{
	struct pm_pwm_config pwm_config;

	if ((duty_us > period_us) || (period_us > PM_PWM_PERIOD_MAX) ||
	    (period_us < PM_PWM_PERIOD_MIN)) {
		dprintf(CRITICAL, "Error in duty cycle and period\n");
		return -1;
	}

	pm_pwm_calc_period(period_us, &pwm_config);
	pm_pwm_calc_pwm_value(&pwm_config, period_us, duty_us);

	dprintf(SPEW, "duty/period=%u/%u usec: pwm_value=%d (of %d)\n",
		duty_us, period_us, pwm_config.pwm_value, 1 << pwm_config.pwm_size);

	pm_pwm_config_regs(&pwm_config);

	return 0;
}

void pm_pwm_enable(bool enable)
{
	uint8_t reg;

	reg = enable << PM_PWM_ENABLE_CTL_SHIFT
		& PM_PWM_ENABLE_CTL_MASK;

	pm_pwm_reg_write(PM_PWM_ENABLE_CTL_REG_OFFSET, reg);
}
