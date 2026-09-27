// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2024, Nikita Travkin <nikita@trvn.ru> */

#include <string.h>
#include <stdlib.h>
#include <dev/fbcon.h>
#include <target.h>
#include <arch/ops.h>

#include "cont-splash.h"

extern void *target_get_scratch_address(void);
extern void *rgb888_to_xrgb8888(void *out, const void *in, uint32_t npixels);

void fb_convert_to_xrgb8888(struct fbcon_config *fb)
{
	uint32_t *tmp_fb = target_get_scratch_address();

	if (fb->format != FB_FORMAT_RGB888)
		return;

	rgb888_to_xrgb8888(tmp_fb, fb->base, fb->width * fb->height);
	memcpy(fb->base, tmp_fb, fb->stride * 4 * fb->height);
	arch_clean_cache_range((addr_t)fb->base, fb->stride * 4 * fb->height);
}
