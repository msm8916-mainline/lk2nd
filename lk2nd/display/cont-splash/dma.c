// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <bits.h>
#include <debug.h>
#include <dev/fbcon.h>
#include <reg.h>

#include "cont-splash.h"
#include "mdp.h"

bool mdp_read_dma_config(struct fbcon_config *fb)
{
	uint32_t config, size, stride, out_xy, format;

	fb->base = (void *)readl(MDP_DMA_P_BUF_ADDR);
	if (!fb->base)
		return false;

	config = readl(MDP_DMA_P_CONFIG);
	size = readl(MDP_DMA_P_SIZE);
	stride = readl(MDP_DMA_P_BUF_Y_STRIDE);
	out_xy = readl(MDP_DMA_P_OUT_XY);
	format = BITS_SHIFT(config, 26, 25);

	dprintf(INFO, "MDP continuous splash detected: DMA_P, base: %p, stride: %d, "
		"size: %dx%d, out: (%d,%d), config: %#x (format: %#x)\n",
		fb->base, stride,
		MDP_X(size), MDP_Y(size), MDP_X(out_xy), MDP_Y(out_xy),
		config, format
	);

	fb->width = MDP_X(size);
	fb->height = MDP_Y(size);

	switch (format) {
	case 0x0: /* RGB888 */
		fb->stride = stride / 3;
		fb->bpp = 3 * 8;
		fb->format = FB_FORMAT_RGB888;
		break;
	case 0x1: /* RGB565 */
		fb->stride = stride / 2;
		fb->bpp = 2 * 8;
		fb->format = FB_FORMAT_RGB565;
		break;
	case 0x2: /* xRGB888 */
		fb->stride = stride / 4;
		fb->bpp = 4 * 8;
		fb->format = FB_FORMAT_RGB888;
		break;
	default:
		dprintf(CRITICAL, "Unsupported MDP DMA format: %#x\n", format);
		return false;
	}

	return true;
}
