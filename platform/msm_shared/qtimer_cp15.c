/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
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

static platform_timer_callback timer_callback;
static void *timer_arg;
static time_t timer_interval;
/* time in ms from start of LK. */
static volatile uint32_t current_time;
static uint32_t tick_count;

extern void isb();

static enum handler_return qtimer_irq(void *arg)
{
	current_time += timer_interval;

	/* Program the down counter again to get
	 * an interrupt after timer_interval msecs
	 */

	__asm__ volatile("mcr p15, 0, %0, c14, c2, 0" : :"r" (tick_count));

	isb();

	return timer_callback(timer_arg, current_time);
}

/* Programs the Physical Secure Down counter timer.
 * interval : Counter ticks till expiry interrupt is fired.
 */
void qtimer_set_physical_timer(time_t msecs_interval,
	platform_timer_callback tmr_callback,
	void *tmr_arg)
{
	uint32_t ctrl;

	/* Save the timer interval and call back data*/
	tick_count = msecs_interval * qtimer_tick_rate() / 1000;;
	timer_interval = msecs_interval;
	timer_arg = tmr_arg;
	timer_callback = tmr_callback;

	/* Program CTRL Register */
	ctrl =0;
	ctrl |= QTMR_TIMER_CTRL_ENABLE;
	ctrl &= ~QTMR_TIMER_CTRL_INT_MASK;

	__asm__ volatile("mcr p15, 0, %0, c14, c2, 1" : :"r" (ctrl));

	isb();

	/* Set Physical Down Counter */
	__asm__ volatile("mcr p15, 0, %0, c14, c2, 0" : :"r" (tick_count));

	isb();

	/* Register for timer interrupts */

	register_int_handler(INT_QTMR_NON_SECURE_PHY_TIMER_EXP, qtimer_irq, 0);
	unmask_interrupt(INT_QTMR_NON_SECURE_PHY_TIMER_EXP);

	return;

}

void disable_qtimer()
{
	uint32_t ctrl;

	mask_interrupt(INT_QTMR_NON_SECURE_PHY_TIMER_EXP);

	/* program cntrl register */
	ctrl = 0;
	ctrl |= ~QTMR_TIMER_CTRL_ENABLE;
	ctrl &= QTMR_TIMER_CTRL_INT_MASK;

	__asm__ volatile("mcr p15, 0, %0, c14, c2, 1" : :"r" (ctrl));

	isb();

}

/* Function to return the frequency of the timer */
uint32_t qtimer_get_frequency()
{
	uint32_t freq;

	/* Read the Global counter frequency */
	__asm__ volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (freq));

	return freq;

}

inline __ALWAYS_INLINE uint64_t qtimer_get_phy_timer_cnt()
{
	uint32_t phy_cnt_lo;
	uint32_t phy_cnt_hi;

	__asm__ volatile("mrrc p15,0,%0,%1, c14":
		"=r"(phy_cnt_lo),"=r"(phy_cnt_hi));
	return ((uint64_t)phy_cnt_hi << 32) | phy_cnt_lo;
}

uint32_t qtimer_current_time()
{
	return current_time;
}
