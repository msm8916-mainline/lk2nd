/*
 * Copyright (c) 2011, The Linux Foundation. All rights reserved.
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

#ifndef __DEV_LCDC_H
#define __DEV_LCDC_H

#include <platform/iomap.h>
#include <msm_panel.h>

#define DEFAULT_LCD_TIMING 0

#define MDP_DMA_P_CONFIG                      REG_MDP(0x90000)
#define MDP_DMA_P_OUT_XY                      REG_MDP(0x90010)
#define MDP_DMA_P_SIZE                        REG_MDP(0x90004)
#define MDP_DMA_P_BUF_ADDR                    REG_MDP(0x90008)
#define MDP_DMA_P_BUF_Y_STRIDE                REG_MDP(0x9000C)
#define MDP_DMA_P_OP_MODE                     REG_MDP(0x90070)

#define MDP_LCDC_EN                           REG_MDP(LCDC_BASE + 0x00)
#define MDP_LCDC_HSYNC_CTL                    REG_MDP(LCDC_BASE + 0x04)
#define MDP_LCDC_VSYNC_PERIOD                 REG_MDP(LCDC_BASE + 0x08)
#define MDP_LCDC_VSYNC_PULSE_WIDTH            REG_MDP(LCDC_BASE + 0x0C)
#define MDP_LCDC_DISPLAY_HCTL                 REG_MDP(LCDC_BASE + 0x10)
#define MDP_LCDC_DISPLAY_V_START              REG_MDP(LCDC_BASE + 0x14)
#define MDP_LCDC_DISPLAY_V_END                REG_MDP(LCDC_BASE + 0x18)
#define MDP_LCDC_ACTIVE_HCTL                  REG_MDP(LCDC_BASE + 0x1C)
#define MDP_LCDC_ACTIVE_V_START               REG_MDP(LCDC_BASE + 0x20)
#define MDP_LCDC_ACTIVE_V_END                 REG_MDP(LCDC_BASE + 0x24)
#define MDP_LCDC_BORDER_CLR                   REG_MDP(LCDC_BASE + 0x28)
#define MDP_LCDC_UNDERFLOW_CTL                REG_MDP(LCDC_BASE + 0x2C)
#define MDP_LCDC_HSYNC_SKEW                   REG_MDP(LCDC_BASE + 0x30)
#define MDP_LCDC_TEST_CTL                     REG_MDP(LCDC_BASE + 0x34)
#define MDP_LCDC_CTL_POLARITY                 REG_MDP(LCDC_BASE + 0x38)
#define MDP_LCDC_TEST_COL_VAR1                REG_MDP(LCDC_BASE + 0x3C)
#define MDP_LCDC_UNDERFLOW_HIDING_CTL         REG_MDP(LCDC_BASE + 0x44)
#define MDP_LCDC_LOST_PIXEL_CNT_VALUE         REG_MDP(LCDC_BASE + 0x48)

#define DMA_DSTC0G_8BITS                      (BIT(1)|BIT(0))
#define DMA_DSTC1B_8BITS                      (BIT(3)|BIT(2))
#define DMA_DSTC2R_8BITS                      (BIT(5)|BIT(4))
#define DMA_DSTC0G_6BITS                      (BIT(1))
#define DMA_DSTC1B_6BITS                      (BIT(3))
#define DMA_DSTC2R_6BITS                      (BIT(5))
#define DMA_DITHER_EN                         BIT(24)
#define DMA_OUT_SEL_LCDC                      BIT(20)
#define DMA_IBUF_FORMAT_RGB888                (0 << 25)
#define DMA_IBUF_FORMAT_RGB565                (1 << 25)
#define CLR_G                                 0x0
#define CLR_B                                 0x1
#define CLR_R                                 0x2
#define DMA_PACK_ALIGN_LSB                    0

#define MDP_GET_PACK_PATTERN(a,x,y,z,bit) \
	(((a)<<(bit*3))|((x)<<(bit*2))|((y)<<bit)|(z))
#define MDP_GET_PACK_PATTERN(a, x, y, z, bit) \
	(((a)<<(bit*3))|((x)<<(bit*2))|((y)<<bit)|(z))
#define DMA_PACK_PATTERN_RGB \
	(MDP_GET_PACK_PATTERN(0, CLR_R, CLR_G, CLR_B, 2)<<8)
#define MDP_RGB_888_FORMAT \
	(BIT(17) | (1<<14) | (2<<9) | \
	(0<<8) | (0<<6) | (3<<4) | \
	(3<<2) | (3<<0))
#define MDP_RGB_565_FORMAT \
	(BIT(17) | (1<<14) | (1<<9) | \
	(0<<8) | (0<<6) | (1<<4) | \
	(1<<2) | (2<<0))


/* used for setting custom timing parameters for different panels */
struct lcdc_timing_parameters
{
	unsigned  lcdc_fb_width;
	unsigned  lcdc_fb_height;

	unsigned  lcdc_hsync_pulse_width_dclk;
	unsigned  lcdc_hsync_back_porch_dclk;
	unsigned  lcdc_hsync_front_porch_dclk;
	unsigned  lcdc_hsync_skew_dclk;

	unsigned  lcdc_vsync_pulse_width_lines;
	unsigned  lcdc_vsync_back_porch_lines;
	unsigned  lcdc_vsync_front_porch_lines;
};

#endif /* __DEV_LCDC_H */
