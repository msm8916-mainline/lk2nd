/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, The Linux Foundation. All rights reserved.
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

#if PLATFORM_MSM7X30
#define MSM_MDP_BASE1 0xA3F00000
#define LCDC_BASE     0xC0000
#elif PLATFORM_MSM8X60
#define MSM_MDP_BASE1 0x05100000
#define LCDC_BASE     0xC0000
#define LCDC_FB_ADDR  0x43E00000
#else
#define MSM_MDP_BASE1 0xAA200000
#define LCDC_BASE     0xE0000
#endif

#define LCDC_PIXCLK_IN_PS 26
#define LCDC_FB_PHYS      0x16600000
#define LCDC_FB_BPP       16

#define BIT(x)  (1<<(x))
#define DMA_DSTC0G_8BITS (BIT(1)|BIT(0))
#define DMA_DSTC1B_8BITS (BIT(3)|BIT(2))
#define DMA_DSTC2R_8BITS (BIT(5)|BIT(4))
#define CLR_G 0x0
#define CLR_B 0x1
#define CLR_R 0x2
#define MDP_GET_PACK_PATTERN(a,x,y,z,bit) (((a)<<(bit*3))|((x)<<(bit*2))|((y)<<bit)|(z))
#define DMA_PACK_ALIGN_LSB 0
#define DMA_PACK_PATTERN_RGB					\
        (MDP_GET_PACK_PATTERN(0,CLR_R,CLR_G,CLR_B,2)<<8)
#define DMA_DITHER_EN                       BIT(24)
#define DMA_OUT_SEL_LCDC                    BIT(20)
#define DMA_IBUF_FORMAT_RGB565              BIT(25)

#define MDP_RGB_565_FORMAT (BIT(14) | (1<<9) | (0<<8) | (0<<6) | (1<<4) | (1<<2) | (2<<0))

static struct fbcon_config fb_cfg = {
	.height = LCDC_FB_HEIGHT,
	.width = LCDC_FB_WIDTH,
	.stride = LCDC_FB_WIDTH,
	.format = FB_FORMAT_RGB565,
	.bpp = LCDC_FB_BPP,
	.update_start = NULL,
	.update_done = NULL,
};

void lcdc_clock_init(unsigned rate);

struct fbcon_config *lcdc_init_set(struct lcdc_timing_parameters
				   *custom_timing_param)
{
	struct lcdc_timing_parameters timing_param;
	unsigned mdp_rgb_size;

	if (custom_timing_param == DEFAULT_LCD_TIMING) {
		timing_param.lcdc_hsync_pulse_width_dclk =
		    LCDC_HSYNC_PULSE_WIDTH_DCLK;
		timing_param.lcdc_hsync_back_porch_dclk =
		    LCDC_HSYNC_BACK_PORCH_DCLK;
		timing_param.lcdc_hsync_front_porch_dclk =
		    LCDC_HSYNC_FRONT_PORCH_DCLK;
		timing_param.lcdc_hsync_skew_dclk = LCDC_HSYNC_SKEW_DCLK;

		timing_param.lcdc_vsync_pulse_width_lines =
		    LCDC_VSYNC_PULSE_WIDTH_LINES;
		timing_param.lcdc_vsync_back_porch_lines =
		    LCDC_VSYNC_BACK_PORCH_LINES;
		timing_param.lcdc_vsync_front_porch_lines =
		    LCDC_VSYNC_FRONT_PORCH_LINES;
	} else {
		/*use custom timing parameters */
		timing_param.lcdc_hsync_pulse_width_dclk =
		    custom_timing_param->lcdc_hsync_pulse_width_dclk;
		timing_param.lcdc_hsync_back_porch_dclk =
		    custom_timing_param->lcdc_hsync_back_porch_dclk;
		timing_param.lcdc_hsync_front_porch_dclk =
		    custom_timing_param->lcdc_hsync_front_porch_dclk;
		timing_param.lcdc_hsync_skew_dclk =
		    custom_timing_param->lcdc_hsync_skew_dclk;

		timing_param.lcdc_vsync_pulse_width_lines =
		    custom_timing_param->lcdc_vsync_pulse_width_lines;
		timing_param.lcdc_vsync_back_porch_lines =
		    custom_timing_param->lcdc_vsync_back_porch_lines;
		timing_param.lcdc_vsync_front_porch_lines =
		    custom_timing_param->lcdc_vsync_front_porch_lines;

		/* only set when using custom timing since initialized with defaults  */
		fb_cfg.height = custom_timing_param->lcdc_fb_height;
		fb_cfg.width = custom_timing_param->lcdc_fb_width;
	}
	mdp_rgb_size = (fb_cfg.height << 16) + fb_cfg.width;

	dprintf(INFO, "lcdc_init(): panel is %d x %d\n", fb_cfg.width,
		fb_cfg.height);
	fb_cfg.base = LCDC_FB_ADDR;

	writel((unsigned)fb_cfg.base, MSM_MDP_BASE1 + 0x90008);

	writel((fb_cfg.height << 16) | fb_cfg.width, MSM_MDP_BASE1 + 0x90004);
	writel(fb_cfg.width * fb_cfg.bpp / 8, MSM_MDP_BASE1 + 0x9000c);
	writel(0, MSM_MDP_BASE1 + 0x90010);

	writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB | DMA_DITHER_EN |
	       DMA_OUT_SEL_LCDC | DMA_IBUF_FORMAT_RGB565 | DMA_DSTC0G_8BITS |
	       DMA_DSTC1B_8BITS | DMA_DSTC2R_8BITS, MSM_MDP_BASE1 + 0x90000);

	int hsync_period = timing_param.lcdc_hsync_pulse_width_dclk +
	    timing_param.lcdc_hsync_back_porch_dclk +
	    fb_cfg.width + timing_param.lcdc_hsync_front_porch_dclk;
	int vsync_period = (timing_param.lcdc_vsync_pulse_width_lines +
			    timing_param.lcdc_vsync_back_porch_lines +
			    fb_cfg.height +
			    timing_param.lcdc_vsync_front_porch_lines) *
	    hsync_period;
	int hsync_start_x =
	    timing_param.lcdc_hsync_pulse_width_dclk +
	    timing_param.lcdc_hsync_back_porch_dclk;
	int hsync_end_x =
	    hsync_period - timing_param.lcdc_hsync_front_porch_dclk - 1;
	int display_hctl = (hsync_end_x << 16) | hsync_start_x;
	int display_vstart = (timing_param.lcdc_vsync_pulse_width_lines +
			      timing_param.lcdc_vsync_back_porch_lines)
	    * hsync_period + timing_param.lcdc_hsync_skew_dclk;
	int display_vend = vsync_period -
	    (timing_param.lcdc_vsync_front_porch_lines * hsync_period)
	    + timing_param.lcdc_hsync_skew_dclk - 1;

	writel((hsync_period << 16) | timing_param.lcdc_hsync_pulse_width_dclk,
	       MSM_MDP_BASE1 + LCDC_BASE + 0x4);
	writel(vsync_period, MSM_MDP_BASE1 + LCDC_BASE + 0x8);
	writel(timing_param.lcdc_vsync_pulse_width_lines * hsync_period,
	       MSM_MDP_BASE1 + LCDC_BASE + 0xc);
	writel(display_hctl, MSM_MDP_BASE1 + LCDC_BASE + 0x10);
	writel(display_vstart, MSM_MDP_BASE1 + LCDC_BASE + 0x14);
	writel(display_vend, MSM_MDP_BASE1 + LCDC_BASE + 0x18);

#if MDP4
	writel(0xf, MSM_MDP_BASE1 + LCDC_BASE + 0x28);
	writel(0xff, MSM_MDP_BASE1 + LCDC_BASE + 0x2c);
	writel(timing_param.lcdc_hsync_skew_dclk,
	       MSM_MDP_BASE1 + LCDC_BASE + 0x30);
	writel(0x3, MSM_MDP_BASE1 + LCDC_BASE + 0x38);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x1c);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x20);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x24);

	/* setting for single layer direct out mode for rgb565 source */
	writel(0x100, MSM_MDP_BASE1 + 0x10100);
	writel(mdp_rgb_size, MSM_MDP_BASE1 + 0x40000);
	writel(mdp_rgb_size, MSM_MDP_BASE1 + 0x40008);
	writel((int)fb_cfg.base, MSM_MDP_BASE1 + 0x40010);
	writel(fb_cfg.width * fb_cfg.bpp / 8, MSM_MDP_BASE1 + 0x40040);
	writel(0x00, MSM_MDP_BASE1 + 0x41008);
	writel(MDP_RGB_565_FORMAT, MSM_MDP_BASE1 + 0x40050);
	writel(0x1, MSM_MDP_BASE1 + 0x10004);
	writel(0x1, MSM_MDP_BASE1 + 0x10014);

	/* register flush and enable LCDC */
	writel(0x11, MSM_MDP_BASE1 + 0x18000);
	writel(0x1, MSM_MDP_BASE1 + LCDC_BASE + 0x0);

#else
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x28);
	writel(0xff, MSM_MDP_BASE1 + LCDC_BASE + 0x2c);
	writel(timing_param.lcdc_hsync_skew_dclk,
	       MSM_MDP_BASE1 + LCDC_BASE + 0x30);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x38);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x1c);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x20);
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x24);
	writel(1, MSM_MDP_BASE1 + LCDC_BASE + 0x0);
#endif

	return &fb_cfg;
}

struct fbcon_config *lcdc_init(void)
{
	return lcdc_init_set(DEFAULT_LCD_TIMING);
}

void lcdc_shutdown(void)
{
	writel(0, MSM_MDP_BASE1 + LCDC_BASE + 0x0);
}
