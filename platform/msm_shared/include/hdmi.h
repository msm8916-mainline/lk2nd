/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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
 *
 */

#ifndef __PLATFORM_MSM_SHARED_HDMI_H
#define __PLATFORM_MSM_SHARED_HDMI_H

#include <reg.h>
#include <debug.h>
#include <bits.h>
#include <dev/fbcon.h>
#include <target/display.h>

#define FB_ADDR   0x43E00000

#define MDP_GET_PACK_PATTERN(a,x,y,z,bit) (((a)<<(bit*3))|((x)<<(bit*2))|((y)<<bit)|(z))
#define DMA_PACK_ALIGN_LSB 0
#define DMA_PACK_PATTERN_RGB \
        (MDP_GET_PACK_PATTERN(0,CLR_R,CLR_G,CLR_B,2)<<8)
#define CLR_G 0x0
#define CLR_B 0x1
#define CLR_R 0x2
#define DMA_DSTC0G_8BITS (BIT(1)|BIT(0))
#define DMA_DSTC1B_8BITS (BIT(3)|BIT(2))
#define DMA_DSTC2R_8BITS (BIT(5)|BIT(4))
#define BM(m, l) (((((uint32_t)-1) << (31-m)) >> (31-m+l)) << l)
#define BVAL(m, l, val)     (((val) << l) & BM(m, l))
#define CC(m, n) (BVAL((m+1), m, 0x2) * !!(n))
#define NS_MM(n_msb, n_lsb, n, m, d_msb, d_lsb, d, s_msb, s_lsb, s) \
             (BVAL(n_msb, n_lsb, ~(n-m)) | BVAL(d_msb, d_lsb, (d-1)) \
             | BVAL(s_msb, s_lsb, s))

extern int pm8901_mpp_enable();
extern int pm8901_vs_enable();
extern int pm8058_ldo_set_voltage();
extern int pm8058_vreg_enable();
void gpio_tlmm_config(uint32_t gpio, uint8_t func,
		      uint8_t dir, uint8_t pull,
		      uint8_t drvstr, uint32_t enable);

void dtv_on();

struct hdmi_disp_mode_timing_type {
	uint32_t height;
	uint32_t hsync_porch_fp;
	uint32_t hsync_width;
	uint32_t hsync_porch_bp;
	uint32_t width;
	uint32_t vsync_porch_fp;
	uint32_t vsync_width;
	uint32_t vsync_porch_bp;
	uint32_t refresh_rate;
};

static struct fbcon_config fb_cfg = {
	.height = DTV_FB_HEIGHT,
	.width = DTV_FB_WIDTH,
	.stride = DTV_FB_WIDTH,
	.format = DTV_FORMAT_RGB565,
	.bpp = DTV_BPP,
	.update_start = NULL,
	.update_done = NULL,
};

#endif				/* __PLATFORM_MSM_SHARED_HDMI_H */
