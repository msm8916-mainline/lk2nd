/* Copyright (c) 2012, The Linux Foundation. All rights reserved.

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

#include <debug.h>
#include <reg.h>
#include <compiler.h>
#include <qtimer.h>
#include <platform/irqs.h>
#include <platform/iomap.h>
#include <platform/interrupts.h>
#include <qtimer_mmap_hw.h>

static platform_timer_callback timer_callback;
static void *timer_arg;
static time_t timer_interval;
/* time in ms from start of LK. */
static volatile uint32_t current_time;
static uint32_t tick_count;

extern void dsb();
static void qtimer_enable();

static enum handler_return qtimer_irq(void *arg)
{
	current_time += timer_interval;

	/* Program the down counter again to get
	 * an interrupt after timer_interval msecs
	 */
	writel(tick_count, QTMR_V1_CNTP_TVAL);
	dsb();

	return timer_callback(timer_arg, current_time);
}


/* Programs the Physical Secure Down counter timer.
 * interval : Counter ticks till expiry interrupt is fired.
 */
void qtimer_set_physical_timer(time_t msecs_interval,
							   platform_timer_callback tmr_callback,
							   void *tmr_arg)
{
	qtimer_disable();

	/* Save the timer interval and call back data*/
	tick_count = msecs_interval * qtimer_tick_rate() / 1000;;
	timer_interval = msecs_interval;
	timer_arg = tmr_arg;
	timer_callback = tmr_callback;

	/* Set Physical Down Counter */
	writel(tick_count, QTMR_V1_CNTP_TVAL);
	dsb();

	register_int_handler(INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP, qtimer_irq, 0);

	unmask_interrupt(INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP);

	qtimer_enable();
}


/* Function to return the frequency of the timer */
uint32_t qtimer_get_frequency()
{
	uint32_t freq;

	/* Read the Global counter frequency */
	/* freq = readl(QTMR_V1_CNTFRQ); */
	/* TODO: remove this when bootchaint sets up the frequency. */
	freq = 19200000;

	return freq;
}

static void qtimer_enable()
{
	uint32_t ctrl;

	ctrl = readl(QTMR_V1_CNTP_CTL);

	/* Program CTRL Register */
	ctrl |= QTMR_TIMER_CTRL_ENABLE;
	ctrl &= ~QTMR_TIMER_CTRL_INT_MASK;

	writel(ctrl, QTMR_V1_CNTP_CTL);
	dsb();
}

void qtimer_disable()
{
	uint32_t ctrl;

	ctrl = readl(QTMR_V1_CNTP_CTL);

	/* program cntrl register */
	ctrl &= ~QTMR_TIMER_CTRL_ENABLE;
	ctrl |= QTMR_TIMER_CTRL_INT_MASK;

	writel(ctrl, QTMR_V1_CNTP_CTL);
	dsb();
}

inline __ALWAYS_INLINE uint64_t qtimer_get_phy_timer_cnt()
{
	uint32_t phy_cnt_lo;
	uint32_t phy_cnt_hi_1;
	uint32_t phy_cnt_hi_2;

	do {
		phy_cnt_hi_1 = readl(QTMR_V1_CNTPCT_HI);
		phy_cnt_lo = readl(QTMR_V1_CNTPCT_LO);
		phy_cnt_hi_2 = readl(QTMR_V1_CNTPCT_HI);
    } while (phy_cnt_hi_1 != phy_cnt_hi_2);

	return ((uint64_t)phy_cnt_hi_1 << 32) | phy_cnt_lo;
}

uint32_t qtimer_current_time()
{
	return current_time;
}
