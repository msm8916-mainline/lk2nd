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
#include <kernel/thread.h>

static uint32_t ticks_per_sec;

status_t platform_set_periodic_timer(platform_timer_callback callback,
	void *arg, time_t interval)
{

	enter_critical_section();

	qtimer_set_physical_timer(interval, callback, arg);

	exit_critical_section();
	return 0;
}

time_t current_time(void)
{
	return qtimer_current_time();
}

void qtimer_uninit()
{
	disable_qtimer();
}

/* Blocking function to wait until the specified ticks of the timer.
 * Note: ticks to wait for cannot be more than 56 bit.
 *          Should be sufficient for all practical purposes.
 */
static void delay(uint64_t ticks)
{
	volatile uint64_t cnt;
	uint64_t init_cnt;
	uint64_t timeout = 0;

	cnt = qtimer_get_phy_timer_cnt();
	init_cnt = cnt;

	/* Calculate timeout = cnt + ticks (mod 2^56)
	 * to account for timer counter wrapping
	 */
	timeout = (cnt + ticks) &
			(uint64_t)(QTMR_PHY_CNT_MAX_VALUE);

	/* Wait out till the counter wrapping occurs 
	 * in cases where there is a wrapping.
	 */
	while(timeout < cnt && init_cnt <= cnt)
		/* read global counter */
		cnt = qtimer_get_phy_timer_cnt();

	/* Wait till the number of ticks is reached*/
	while(timeout > cnt)
		/* read global counter */
		cnt = qtimer_get_phy_timer_cnt();

}


void mdelay(unsigned msecs)
{
	uint64_t ticks;

	ticks = (msecs * ticks_per_sec) / 1000;

	delay(ticks);
}

void udelay(unsigned usecs)
{
	uint64_t ticks;

	ticks = (usecs * ticks_per_sec) / 1000000;

	delay(ticks);
}

/* Return current time in micro seconds */
bigtime_t current_time_hires(void)
{
	return qtimer_current_time() * 1000000ULL;
}

void qtimer_init()
{
	ticks_per_sec = qtimer_get_frequency();
}

uint32_t qtimer_tick_rate()
{
	return ticks_per_sec;
}
