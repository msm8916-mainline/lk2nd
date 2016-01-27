/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation. nor the names of its
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

#include <stdlib.h>
#include <debug.h>
#include <platform.h>
#include <platform/iomap.h>
#include <reg.h>
#include <wdog.h>
#include <target.h>
#include <scm.h>
#include <dload_util.h>
#include <kernel/thread.h>

#define WDOG_FEED_FREQUENCY 1000

static int wdog_feed_handler(void *param) {
	while(1) {
		writel(1, APPS_WDOG_RESET_REG);
		thread_sleep(WDOG_FEED_FREQUENCY);
	}
	return 0;
}

static void wdog_feed_func_thread(void)
{
	static bool is_thread_start;
	thread_t *thr;

	if (!is_thread_start) {
		thr = thread_create("wdogfeed", wdog_feed_handler,
			0, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
		if (!thr) {
			dprintf(CRITICAL, "ERROR: create feed dog thread failed!!\n");
			return;
		}
		thread_resume(thr);
		is_thread_start = true;
	}
}

void msm_wdog_init()
{
	/* Set Bite and Bark times  10s */
	writel(0x77FD3, APPS_WDOG_BARK_VAL_REG);
	writel(0x77FD3, APPS_WDOG_BITE_VAL_REG);

	/* Reset WDOG */
	writel(1, APPS_WDOG_RESET_REG);

	/* Enable WDOG */
	writel((readl(APPS_WDOG_CTL_REG) | 0x1), APPS_WDOG_CTL_REG);

	wdog_feed_func_thread();

	set_download_mode(NORMAL_DLOAD);
}
