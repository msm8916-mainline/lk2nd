/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.

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
#include <sys/types.h>

#include <platform/timer.h>
#include <platform/irqs.h>
#include <platform/iomap.h>
#include <platform/interrupts.h>
#include <kernel/thread.h>

#define QTMR_TIMER_CTRL_ENABLE          (1 << 0)
#define QTMR_TIMER_CTRL_INT_MASK        (1 << 1)

#define PLATFORM_TIMER_TYPE_PHYSICAL     1
#define PLATFORM_TIMER_TYPE_VIRTUAL      2

static platform_timer_callback timer_callback;
static void *timer_arg;
static time_t timer_interval;
static unsigned int timer_type = PLATFORM_TIMER_TYPE_PHYSICAL;
static volatile uint32_t ticks;

static enum handler_return timer_irq(void *arg)
{
	ticks += timer_interval;

	if (timer_type == PLATFORM_TIMER_TYPE_VIRTUAL)
		__asm__("mcr p15, 0, %0, c14, c3, 0"::"r"(timer_interval));
	else if (timer_type == PLATFORM_TIMER_TYPE_PHYSICAL)
		__asm__("mcr p15, 0, %0, c14, c2, 0" : :"r" (timer_interval));

	return timer_callback(timer_arg, ticks);
}

/* Programs the Virtual Down counter timer.
 * interval : Counter ticks till expiry interrupt is fired.
 */
unsigned int platform_set_virtual_timer(uint32_t interval)
{
	uint32_t ctrl;

	/* Program CTRL Register */
	ctrl =0;
	ctrl |= QTMR_TIMER_CTRL_ENABLE;
	ctrl &= ~QTMR_TIMER_CTRL_INT_MASK;

	__asm__("mcr p15, 0, %0, c14, c3, 1"::"r"(ctrl));

	/* Set Virtual Down Counter */
	__asm__("mcr p15, 0, %0, c14, c3, 0"::"r"(interval));

	return INT_QTMR_VIRTUAL_TIMER_EXP;

}

/* Programs the Physical Secure Down counter timer.
 * interval : Counter ticks till expiry interrupt is fired.
 */
unsigned int platform_set_physical_timer(uint32_t interval)
{
	uint32_t ctrl;

	/* Program CTRL Register */
	ctrl =0;
	ctrl |= QTMR_TIMER_CTRL_ENABLE;
	ctrl &= ~QTMR_TIMER_CTRL_INT_MASK;

	__asm__("mcr p15, 0, %0, c14, c2, 1" : :"r" (ctrl));

	/* Set Physical Down Counter */
	__asm__("mcr p15, 0, %0, c14, c2, 0" : :"r" (interval));

	return INT_QTMR_SECURE_PHYSICAL_TIMER_EXP;

}


status_t platform_set_periodic_timer(platform_timer_callback callback,
	void *arg, time_t interval)
{
	uint32_t ppi_num;
	unsigned long ctrl;
	uint32_t tick_count = interval * platform_tick_rate() / 1000;

	enter_critical_section();

	timer_callback = callback;
	timer_arg = arg;
	timer_interval = interval;

	if (timer_type == PLATFORM_TIMER_TYPE_VIRTUAL)
		ppi_num = platform_set_virtual_timer(tick_count);
	else if (timer_type == PLATFORM_TIMER_TYPE_PHYSICAL)
		ppi_num = platform_set_physical_timer(tick_count);

	register_int_handler(ppi_num, timer_irq, 0);
	unmask_interrupt(ppi_num);

	exit_critical_section();
	return 0;
}

time_t current_time(void)
{
	return ticks;
}

void platform_uninit_timer(void)
{
	uint32_t ctrl;

	unmask_interrupt(INT_DEBUG_TIMER_EXP);

	/* program cntrl register */
	ctrl =0;
	ctrl |= ~QTMR_TIMER_CTRL_ENABLE;
	ctrl &= QTMR_TIMER_CTRL_INT_MASK;

	if (timer_type == PLATFORM_TIMER_TYPE_VIRTUAL)
		__asm__("mcr p15, 0, %0, c14, c3, 1"::"r"(ctrl));
	else if (timer_type == PLATFORM_TIMER_TYPE_PHYSICAL)
		__asm__("mcr p15, 0, %0, c14, c2, 1" : :"r" (ctrl));

}

void mdelay(unsigned msecs)
{
	uint32_t phy_cnt_lo, phy_cnt_hi, cnt, timeout = 0;
	uint64_t phy_cnt;
	msecs = msecs *  platform_tick_rate() / 1000;

	do{
	/* read global counter */
	__asm__("mrrc p15,0,%0,%1, c14":"=r"(phy_cnt_lo),"=r"(phy_cnt_hi));
	phy_cnt = ((uint64_t)phy_cnt_hi << 32) | phy_cnt_lo;
	/*Actual counter used in the simulation is only 32 bits
	 * in reality the counter is actually 56 bits.
	 */
	cnt = phy_cnt & (uint32_t)~0;
	if (timeout == 0)
		timeout = cnt + msecs;
	} while (cnt < timeout);

}

void udelay(unsigned usecs)
{
	uint32_t phy_cnt_lo, phy_cnt_hi, cnt, timeout = 0;
	uint64_t phy_cnt;
	usecs = (usecs * platform_tick_rate()) / 1000000;

	do{
	/* read global counter */
	__asm__("mrrc p15,0,%0,%1, c14":"=r"(phy_cnt_lo),"=r"(phy_cnt_hi));
	phy_cnt = ((uint64_t)phy_cnt_hi << 32) | phy_cnt_lo;

	/*Actual counter used in the simulation is only 32 bits
	 * in reality the counter is actually 56 bits.
	 */
	cnt = phy_cnt & (uint32_t)~0;
	if (timeout == 0)
		timeout = cnt + usecs;
	} while (cnt < timeout);
}

/* Return current time in micro seconds */
bigtime_t current_time_hires(void)
{
	return ticks * 1000000ULL;
}
