// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <dev/fbcon.h>
#include <platform.h>
#include <platform/clock.h>
#include <reg.h>
#include <target.h>

#include <lk2nd/util/mmu.h>

#include "cont-splash.h"

static bool mdp_prepare_fb(struct fbcon_config *fb)
{
	uint32_t size = fb->stride * fb->bpp/8 * fb->height;
	uintptr_t base = (uintptr_t)fb->base;

	if (fb->stride == 0 || fb->width == 0 || fb->height == 0)
		return false;

	return lk2nd_mmu_map_ram_dynamic("continuous splash", base, size);
}

__WEAK bool mdp_read_dma_config(struct fbcon_config *fb) { return false; }
__WEAK bool mdp_read_pipe_config(struct fbcon_config *fb) { return false; }

static bool mdp_read_config(struct fbcon_config *fb)
{
	return mdp_read_pipe_config(fb) || mdp_read_dma_config(fb);
}

#if !defined(MDP_GDSCR) && defined(MDSS_GDSCR)
#define MDP_GDSCR	MDSS_GDSCR
#endif

void target_display_init(const char *panel_name)
{
	static struct fbcon_config fb;

#ifdef MDP_GDSCR
	/*
	 * Reading MDP registers will fail if necessary clocks/power domains
	 * are not enabled. Check if the MDP power domain (GDSC) is enabled
	 * to try to avoid crashing if the clocks are disabled.
	 */
	if (!(readl(MDP_GDSCR) & GDSC_POWER_ON_BIT)) {
		dprintf(CRITICAL, "No continuous splash: MDP GDSC is not enabled\n");
		return;
	}
#endif

	if (!mdp_read_config(&fb)) {
		dprintf(CRITICAL, "Continuous splash not detected\n");
		return;
	}
	if (!mdp_prepare_fb(&fb) || !mdp_start_refresh(&fb))
		return;

	fbcon_setup(&fb);
	display_image_on_screen();
}
