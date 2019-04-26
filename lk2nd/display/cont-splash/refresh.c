// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2020-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <bits.h>
#include <debug.h>
#include <dev/fbcon.h>
#include <kernel/event.h>
#include <kernel/thread.h>
#include <reg.h>

#include "cont-splash.h"
#include "mdp.h"

static event_t refresh_event;

static void mdp_refresh(void)
{
#if MDP3 || MDP4
	writel(1, MDP_DMA_P_START);
#elif MDP5
	writel(1, MDP_CTL_0_BASE + CTL_START);
#endif
}

static int mdp_cmd_refresh_loop(void *data)
{
	while (true) {
		/* Limit refresh to 50 Hz to avoid overlapping display updates */
		event_wait(&refresh_event);
		mdp_refresh();
		thread_sleep(20);
	}
	return 0;
}

static void mdp_cmd_signal_refresh(void)
{
	event_signal(&refresh_event, false);
}

static void mdp_cmd_refresh_start(struct fbcon_config *fb)
{
	thread_t *thr;

	event_init(&refresh_event, false, EVENT_FLAG_AUTOUNSIGNAL);

	thr = thread_create("display-refresh", &mdp_cmd_refresh_loop,
			    fb, HIGH_PRIORITY, DEFAULT_STACK_SIZE);
	if (!thr) {
		dprintf(CRITICAL, "Failed to create display-refresh thread\n");
		return;
	}

	thread_resume(thr);
	fb->update_start = mdp_cmd_signal_refresh;
}

bool mdp_start_refresh(struct fbcon_config *fb)
{
	bool cmd_mode, auto_refresh = false;
	uint32_t sel;

#if MDP3
	sel = readl(MDP_DMA_P_CONFIG);
	cmd_mode = BITS_SHIFT(sel, 20, 19) == 0x1; /* OUT_SEL == DSI_CMD? */
#elif MDP4
	sel = readl(MDP_DISP_INTF_SEL);
	cmd_mode = BITS_SHIFT(sel, 1, 0) == 0x2; /* PRIM_INTF_SEL == DSI_CMD? */
#elif MDP5
	sel = readl(MDP_CTL_0_BASE + CTL_TOP);
	cmd_mode = !!(sel & BIT(17)); /* MODE_SEL == Command Mode? */
#endif

#ifdef MDP_AUTOREFRESH_CONFIG_P /* MDP3/MDP4 */
	auto_refresh = !!(readl(MDP_AUTOREFRESH_CONFIG_P) & BIT(28));
#endif
#ifdef MDSS_MDP_REG_PP_AUTOREFRESH_CONFIG /* MDP5 */
	auto_refresh = !!(readl(MDP_PP_0_BASE + MDSS_MDP_REG_PP_AUTOREFRESH_CONFIG) & BIT(31));
#endif

	dprintf(INFO, "Display refresh: cmd mode: %d, auto refresh: %d (sel: %#x)\n",
		cmd_mode, auto_refresh, sel);

#ifdef MDP_DISPLAY_STATUS /* MDP4 */
	if (!cmd_mode && readl(MDP_DISPLAY_STATUS) == 0) {
		dprintf(CRITICAL, "Cannot use continuous splash: display not active\n");
		return false;
	}
#endif

	if (cmd_mode && !auto_refresh)
		mdp_cmd_refresh_start(fb);

	return true;
}
