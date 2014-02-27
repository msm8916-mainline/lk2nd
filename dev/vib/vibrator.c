/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 *   * Neither the name of The Linux Foundation, Inc. nor the names of its
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
#include <stdlib.h>
#include <kernel/timer.h>
#include <platform/timer.h>
#include <vibrator.h>
#include <pm_vib.h>

#define CHECK_VIB_TIMER_FREQUENCY    50

static struct timer vib_timer;
static uint32_t vib_timeout;

/* Function to turn on vibrator */
void vib_turn_on()
{
	pm_vib_turn_on();
}

/* Function to turn off vibrator */
void vib_turn_off()
{
	pm_vib_turn_off();
}

/* Function to turn off vibrator when the vib_timer is expired. */
static enum handler_return vib_timer_func(struct timer *v_timer, void *arg)
{
	timer_cancel(&vib_timer);
	vib_turn_off();
	vib_timeout=1;

	return INT_RESCHEDULE;
}

/*
 * Function to turn on vibrator.
 * vibrate_time - the time of phone vibrate.
 */
void vib_timed_turn_on(const uint32_t vibrate_time)
{
	vib_turn_on();
	vib_timeout=0;
	timer_initialize(&vib_timer);
	timer_set_oneshot(&vib_timer, vibrate_time, vib_timer_func, NULL);
}

/* Wait for vibrator timer expired */
void wait_vib_timeout(void)
{
	while (!vib_timeout) {
		/* every 50ms to check if the vibrator timer is timeout*/
		thread_sleep(CHECK_VIB_TIMER_FREQUENCY);
	}
}
