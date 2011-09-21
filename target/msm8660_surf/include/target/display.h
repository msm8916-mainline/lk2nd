/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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
#ifndef _TARGET_QSD8660_SURF_DISPLAY_H
#define _TARGET_QSD8660_SURF_DISPLAY_H

#define MIPI_FB_ADDR  0x43E00000

#define TARGET_XRES 1024
#define TARGET_YRES 600

#define LCDC_FB_WIDTH     1024
#define LCDC_FB_HEIGHT    600

#define LCDC_HSYNC_PULSE_WIDTH_DCLK 32
#define LCDC_HSYNC_BACK_PORCH_DCLK  80
#define LCDC_HSYNC_FRONT_PORCH_DCLK 48
#define LCDC_HSYNC_SKEW_DCLK        0

#define LCDC_VSYNC_PULSE_WIDTH_LINES 1
#define LCDC_VSYNC_BACK_PORCH_LINES  4
#define LCDC_VSYNC_FRONT_PORCH_LINES 3

/* Toshiba MIPI panel */
#define TSH_MIPI_FB_WIDTH            480
#define TSH_MIPI_FB_HEIGHT           854

/* NOVATEK MIPI panel */
#define NOV_MIPI_FB_WIDTH            540
#define NOV_MIPI_FB_HEIGHT           960

#define MIPI_HSYNC_PULSE_WIDTH       50
#define MIPI_HSYNC_BACK_PORCH_DCLK   500
#define MIPI_HSYNC_FRONT_PORCH_DCLK  500

#define MIPI_VSYNC_PULSE_WIDTH       5
#define MIPI_VSYNC_BACK_PORCH_LINES  20
#define MIPI_VSYNC_FRONT_PORCH_LINES 20

/* HDMI Panel Macros for 1080p */
#define DTV_FB_HEIGHT     1080
#define DTV_FB_WIDTH      1920
#define DTV_FORMAT_RGB565 0
#define DTV_BPP           16

#endif
