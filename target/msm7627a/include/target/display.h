/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
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
#ifndef _TARGET_MSM7627_SURF_DISPLAY_H
#define _TARGET_MSM7627_SURF_DISPLAY_H

#define TARGET_XRES 800
#define TARGET_YRES 480

#define LCDC_FB_WIDTH	 800
#define LCDC_FB_HEIGHT	480

#define LCDC_HSYNC_PULSE_WIDTH_DCLK 60
#define LCDC_HSYNC_BACK_PORCH_DCLK  81
#define LCDC_HSYNC_FRONT_PORCH_DCLK 81
#define LCDC_HSYNC_SKEW_DCLK		0

#define LCDC_VSYNC_PULSE_WIDTH_LINES 2
#define LCDC_VSYNC_BACK_PORCH_LINES  20
#define LCDC_VSYNC_FRONT_PORCH_LINES 27

/* RENESAS MIPI panel */
#define REN_MIPI_FB_WIDTH            480
#define REN_MIPI_FB_HEIGHT           864

#define MIPI_HSYNC_PULSE_WIDTH       8
#define MIPI_HSYNC_BACK_PORCH_DCLK   100
#define MIPI_HSYNC_FRONT_PORCH_DCLK  100

#define MIPI_VSYNC_PULSE_WIDTH       1
#define MIPI_VSYNC_BACK_PORCH_LINES  20
#define MIPI_VSYNC_FRONT_PORCH_LINES 20

/* RENESAS MIPI HVGA panel */
#define REN_MIPI_FB_WIDTH_HVGA            480
#define REN_MIPI_FB_HEIGHT_HVGA           320

#define MIPI_HSYNC_PULSE_WIDTH_HVGA       5
#define MIPI_HSYNC_BACK_PORCH_DCLK_HVGA   21
#define MIPI_HSYNC_FRONT_PORCH_DCLK_HVGA  15

#define MIPI_VSYNC_PULSE_WIDTH_HVGA       50
#define MIPI_VSYNC_BACK_PORCH_LINES_HVGA  50
#define MIPI_VSYNC_FRONT_PORCH_LINES_HVGA 101

/* NT35510 MIPI WVGA panel */
#define NT35510_MIPI_FB_WIDTH            480
#define NT35510_MIPI_FB_HEIGHT           800

#define MIPI_FB_ADDR  0x20000000

extern int mipi_dsi_phy_init(struct mipi_dsi_panel_config *);
extern void config_renesas_dsi_video_mode(void);
#endif
