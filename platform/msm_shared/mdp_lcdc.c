/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <stdlib.h>
#include <reg.h>
#include <platform/iomap.h>
#include <dev/fbcon.h>
#include <target/display.h>
#include <dev/lcdc.h>
#include <msm_panel.h>
#include <err.h>
#include <mdp4.h>
#include <bits.h>

int mdp_lcdc_config(struct msm_panel_info *pinfo,
		struct fbcon_config *fb)
{
	unsigned mdp_rgb_size;
	unsigned mdp_rgb_size_src;
	int hsync_period, vsync_period;
	int hsync_start_x, hsync_end_x;
	int display_hctl, display_vstart, display_vend;
	unsigned mdp_rgb_format = 0;

	int active_x, active_y;
	int active_hstart_x, active_hend_x;
	int active_vstart, active_vend;
	int mdp_rev;

	struct lcdc_panel_info *lcdc = NULL;

	if (pinfo == NULL)
		return;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return;

	mdp_rev = mdp_get_revision();

	mdp_rgb_size = (pinfo->yres << 16) + pinfo->xres;
	mdp_rgb_size_src = (fb->height << 16) + fb->width;

	dprintf(INFO, "Panel is %d x %d\n",
			pinfo->xres + lcdc->xres_pad,
			pinfo->yres + lcdc->yres_pad);

	/* write fb addr in MDP_DMA_P_BUF_ADDR */
	writel(fb->base, MDP_DMA_P_BUF_ADDR);

	/* write active region size*/
	writel(mdp_rgb_size, MDP_DMA_P_SIZE);

	/* set Y-stride value in bytes */
	/* Y-stride is defined as the number of bytes
	   in a line.
	 */
	writel((pinfo->xres * pinfo->bpp/8), MDP_DMA_P_BUF_Y_STRIDE);

	/* Start XY coordinates */
	writel(0, MDP_DMA_P_OUT_XY);

	if (fb->bpp == 16) {
		writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB |
			DMA_DITHER_EN |	DMA_OUT_SEL_LCDC |
			DMA_IBUF_FORMAT_RGB565 | DMA_DSTC0G_6BITS |
			DMA_DSTC1B_6BITS | DMA_DSTC2R_6BITS,
			MDP_DMA_P_CONFIG);
		mdp_rgb_format = MDP_RGB_565_FORMAT;
	} else if (fb->bpp == 24) {
		writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB |
			DMA_DITHER_EN | DMA_OUT_SEL_LCDC |
			DMA_IBUF_FORMAT_RGB888 | DMA_DSTC0G_8BITS |
			DMA_DSTC1B_8BITS | DMA_DSTC2R_8BITS,
			MDP_DMA_P_CONFIG);
		mdp_rgb_format = MDP_RGB_888_FORMAT;
	} else {
		dprintf(CRITICAL, "Unsupported bpp detected!\n");
		return ERR_INVALID_ARGS;
	}

	hsync_period = lcdc->h_pulse_width +
		lcdc->h_back_porch +
		pinfo->xres + lcdc->h_front_porch;
	vsync_period = (lcdc->v_pulse_width +
			lcdc->v_back_porch +
			pinfo->yres +
			lcdc->v_front_porch) *
		hsync_period;
	hsync_start_x =
		lcdc->h_pulse_width +
		lcdc->h_back_porch;
	hsync_end_x =
		hsync_period - lcdc->h_front_porch - 1;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;
	display_vstart = (lcdc->v_pulse_width +
			lcdc->v_back_porch)
		* hsync_period + lcdc->hsync_skew;
	display_vend = vsync_period -
		(lcdc->v_front_porch * hsync_period)
		+lcdc->hsync_skew - 1;

	active_x = (pinfo->xres - fb->width)/2;
	active_y = (pinfo->yres - fb->height)/2;

	active_hstart_x = lcdc->h_pulse_width + lcdc->h_back_porch
					  + active_x;
	active_hend_x = active_hstart_x + fb->width - 1;

	active_vstart = (lcdc->v_pulse_width + lcdc->v_back_porch
			+ active_y) * hsync_period
			+ lcdc->hsync_skew;
	active_vend = active_vstart + (fb->height * hsync_period) - 1;


	/* LCDC specific initalizations */
	writel((hsync_period << 16) | lcdc->h_pulse_width,
			MDP_LCDC_HSYNC_CTL);
	writel(vsync_period, MDP_LCDC_VSYNC_PERIOD);
	writel(lcdc->v_pulse_width * hsync_period,
			MDP_LCDC_VSYNC_PULSE_WIDTH);
	writel(display_hctl, MDP_LCDC_DISPLAY_HCTL);
	writel(display_vstart, MDP_LCDC_DISPLAY_V_START);
	writel(display_vend, MDP_LCDC_DISPLAY_V_END);

	if (mdp_rev >= MDP_REV_40) {
		writel(BIT(31) | (active_hend_x << 16) | active_hstart_x,
			  MDP_LCDC_ACTIVE_HCTL);

		writel(BIT(31) | active_vstart, MDP_LCDC_ACTIVE_V_START);
		writel(active_vend, MDP_LCDC_ACTIVE_V_END);

		writel(0xf, MDP_LCDC_BORDER_CLR);
		writel(0xff, MDP_LCDC_UNDERFLOW_CTL);
		writel(lcdc->hsync_skew,
				MDP_LCDC_HSYNC_SKEW);
		writel(0x3, MDP_LCDC_CTL_POLARITY);
		writel(0, MDP_LCDC_ACTIVE_HCTL);
		writel(0, MDP_LCDC_ACTIVE_V_START);
		writel(0, MDP_LCDC_ACTIVE_V_END);

		/* setting for single layer direct out mode for rgb565 source */
		writel(0x100, MDP_LAYERMIXER_IN_CFG);
		writel(mdp_rgb_size_src, MDP_RGB1_SRC_SIZE);
		writel(mdp_rgb_size, MDP_RGB1_OUT_SIZE);
		writel((int)fb->base, MDP_RGB1_SRCP0_ADDR);
		writel((fb->stride * fb->bpp / 8), MDP_RGB1_SRC_YSTRIDE1);
		writel(0x00, MDP_RGB1_CONSTANT_COLOR);
		writel(mdp_rgb_format, MDP_RGB1_SRC_FORMAT);
		writel(0x1, MDP_OVERLAYPROC0_CFG);
		if (fb->bpp == 16)
			writel(0x1, MDP_OVERLAYPROC0_OPMODE);
		else if (fb->bpp == 24)
			writel(0x0, MDP_OVERLAYPROC0_OPMODE);
		/* register flush and enable LCDC */
		writel(0x11, MDP_OVERLAY_REG_FLUSH);
	} else if (MDP_REV_303 == mdp_rev) {
		writel(0x0, MDP_BASE + LCDC_BASE + 0x28);
		writel(0xff, MDP_BASE + LCDC_BASE + 0x2c);
		writel(lcdc->hsync_skew, MDP_BASE + LCDC_BASE + 0x30);
		writel(0x0, MDP_BASE + LCDC_BASE + 0x38);
		writel(0x0, MDP_BASE + LCDC_BASE + 0x1c);
		writel(0x0, MDP_BASE + LCDC_BASE + 0x20);
		writel(0x0, MDP_BASE + LCDC_BASE + 0x24);
	}
	return NO_ERROR;
}

int mdp_lcdc_on()
{
	writel(0x1, MDP_LCDC_EN);
	return NO_ERROR;
}

int mdp_lcdc_off()
{
	if(!target_cont_splash_screen())
		writel(0x0, MDP_LCDC_EN);
	return NO_ERROR;
}
