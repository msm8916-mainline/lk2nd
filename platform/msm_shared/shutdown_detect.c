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
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <kernel/timer.h>
#include <platform/timer.h>
#include <shutdown_detect.h>

/* sleep clock is 32.768 khz, 0x8000 count per second */
#define MPM_SLEEP_TIMETICK_COUNT    0x8000
#define PWRKEY_LONG_PRESS_COUNT     0xC000
#define QPNP_DEFAULT_TIMEOUT        250
#define PWRKEY_DETECT_FREQUENCY     50

static struct timer pon_timer;
static uint32_t pon_timer_complete = 0;

/*
 * Function to check if the the power key is pressed long enough.
 * Return 0 if boot time more than PWRKEY_LONG_PRESS_COUNT.
 * Return 1 if boot time less than PWRKEY_LONG_PRESS_COUNT.
 */
static uint32_t is_pwrkey_time_expired()
{
	/* power on button tied in with PMIC KPDPWR. */
	uint32_t sclk_count = platform_get_sclk_count();

	/* Here check if the long press power-key lasts for 1.5s */
	if (sclk_count > PWRKEY_LONG_PRESS_COUNT)
		return 0;
	else
		return 1;
}

/*
 * Function to check if the power on reason is power key triggered.
 * Return 1 if it is triggered by power key.
 * Return 0 if it is not triggered by power key.
 */
static uint32_t is_pwrkey_pon_reason()
{
	uint8_t pon_reason = pm8x41_get_pon_reason();

	if (pm8x41_get_is_cold_boot() && (pon_reason == KPDPWR_N))
		return 1;
	else
		return 0;
}

/*
 * Main timer handle: check every PWRKEY_DETECT_FREQUENCY to see
 * if power key is pressed.
 * Shutdown the device if power key is release before
 * (PWRKEY_LONG_PRESS_COUNT/MPM_SLEEP_TIMETICK_COUNT) seconds.
 */
static enum handler_return long_press_pwrkey_timer_func(struct timer *p_timer,
	void *arg)
{
	uint32_t sclk_count = platform_get_sclk_count();

	/*
	 * The following condition is treated as the power key
	 * is pressed long enough.
	 * 1. if the power key is pressed last for PWRKEY_LONG_PRESS_COUNT.
	 */
	if (sclk_count > PWRKEY_LONG_PRESS_COUNT) {
		timer_cancel(p_timer);
		pon_timer_complete = 1;
	} else {
		if (pm8x41_get_pwrkey_is_pressed()) {
			/*
			 * For normal man response is > 0.1 secs, so we use 0.05 secs default
			 * for software to be safely detect if there is a key release action.
			 */
			timer_set_oneshot(p_timer, PWRKEY_DETECT_FREQUENCY,
				long_press_pwrkey_timer_func, NULL);
		} else {
			shutdown_device();
		}
	}

	return INT_RESCHEDULE;
}

/*
 * Function to wait until the power key is pressed long enough
 */
static void wait_for_long_pwrkey_pressed()
{
	uint32_t sclk_count = 0;

	while (!pon_timer_complete) {
		sclk_count = platform_get_sclk_count();
		if (sclk_count > PWRKEY_LONG_PRESS_COUNT) {
			timer_cancel(&pon_timer);
			break;
		}
	}
}

/*
 * Function to support for shutdown detection
 * If below condition is met, the function will shut down
 * the device. Otherwise it will do nothing and return to
 * normal boot.
 * condition:
 * 1. it is triggered by power key &&
 * 2. the power key is released before
 * (PWRKEY_LONG_PRESS_COUNT/MPM_SLEEP_TIMETICK_COUNT) seconds.
 */
void shutdown_detect()
{
	/*
	 * If it is booted by power key tirigger.
	 * Initialize pon_timer and call long_press_pwrkey_timer_func
	 * function to check if the power key is last press long enough.
	 */
	if (is_pwrkey_pon_reason() && is_pwrkey_time_expired()) {
		timer_initialize(&pon_timer);
		timer_set_oneshot(&pon_timer, 0, long_press_pwrkey_timer_func, NULL);

		/*
		 * Wait until long press power key timeout
		 *
		 * It will be confused to end users if we shutdown the device
		 * after the splash screen displayed. But it can be moved the
		 * wait here if the boot time is much more considered.
		 */
		wait_for_long_pwrkey_pressed();
	}
}
