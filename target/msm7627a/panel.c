/*
 * * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 */
#include <debug.h>
#include <reg.h>
#include <mdp3.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <target/display.h>

void config_renesas_dsi_video_mode(void)
{

	unsigned char dst_format = 3;	/* RGB888 */
	unsigned char traffic_mode = 2;	/* non burst mode with sync start events */
	unsigned char lane_en = 3;	/* 3 Lanes -- Enables Data Lane0, 1, 2 */
	unsigned long low_pwr_stop_mode = 1;
	unsigned char eof_bllp_pwr = 0x9;	/* Needed or else will have blank line at top of display */
	unsigned char interleav = 0;

	unsigned short display_wd = REN_MIPI_FB_WIDTH;
	unsigned short display_ht = REN_MIPI_FB_HEIGHT;
	unsigned short image_wd = REN_MIPI_FB_WIDTH;
	unsigned short image_ht = REN_MIPI_FB_HEIGHT;
	unsigned short hsync_porch_fp = MIPI_HSYNC_FRONT_PORCH_DCLK;
	unsigned short hsync_porch_bp = MIPI_HSYNC_BACK_PORCH_DCLK;
	unsigned short vsync_porch_fp = MIPI_VSYNC_FRONT_PORCH_LINES;
	unsigned short vsync_porch_bp = MIPI_VSYNC_BACK_PORCH_LINES;
	unsigned short hsync_width = MIPI_HSYNC_PULSE_WIDTH;
	unsigned short vsync_width = MIPI_VSYNC_PULSE_WIDTH;

	dprintf(SPEW, "DSI_Video_Mode - Dst Format: RGB888\n");
	dprintf(SPEW, "Traffic mode: burst mode\n");
	if (machine_is_7x25a()) {
		dprintf(CRITICAL,
			"Entered 7x25A in config_renesas_dsi_video_mode\n");
		dprintf(SPEW, "Data Lane: 1 lane\n");
		display_wd = REN_MIPI_FB_WIDTH_HVGA;
		display_ht = REN_MIPI_FB_HEIGHT_HVGA;
		image_wd = REN_MIPI_FB_WIDTH_HVGA;
		image_ht = REN_MIPI_FB_HEIGHT_HVGA;
		hsync_porch_fp = MIPI_HSYNC_FRONT_PORCH_DCLK_HVGA;
		hsync_porch_bp = MIPI_HSYNC_BACK_PORCH_DCLK_HVGA;
		vsync_porch_fp = MIPI_VSYNC_FRONT_PORCH_LINES_HVGA;
		vsync_porch_bp = MIPI_VSYNC_BACK_PORCH_LINES_HVGA;
		hsync_width = MIPI_HSYNC_PULSE_WIDTH_HVGA;
		vsync_width = MIPI_VSYNC_PULSE_WIDTH_HVGA;
		lane_en = 1;
	} else {
		dprintf(SPEW, "Data Lane: 2 lane\n");
	}

	writel(0x00000000, MDP_DSI_VIDEO_EN);

	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000002, DSI_CLK_CTRL);
	writel(0x00000006, DSI_CLK_CTRL);
	writel(0x0000000e, DSI_CLK_CTRL);
	writel(0x0000001e, DSI_CLK_CTRL);
	writel(0x0000003e, DSI_CLK_CTRL);

	writel(0, DSI_CTRL);

	writel(0, DSI_ERR_INT_MASK0);

	writel(0x02020202, DSI_INT_CTRL);

	writel(((hsync_porch_bp + display_wd) << 16) | hsync_porch_bp,
	       DSI_VIDEO_MODE_ACTIVE_H);

	writel(((vsync_porch_bp + display_ht) << 16) | vsync_porch_bp,
	       DSI_VIDEO_MODE_ACTIVE_V);

	writel(((display_ht + vsync_porch_fp + vsync_porch_bp) << 16) |
	       (display_wd + hsync_porch_fp + hsync_porch_bp),
	       DSI_VIDEO_MODE_TOTAL);
	writel((hsync_width) << 16 | 0, DSI_VIDEO_MODE_HSYNC);

	writel(0 << 16 | 0, DSI_VIDEO_MODE_VSYNC);

	writel(vsync_width << 16 | 0, DSI_VIDEO_MODE_VSYNC_VPOS);

	writel(1, DSI_EOT_PACKET_CTRL);

	writel(0x00000100, DSI_MISR_VIDEO_CTRL);

	writel(1 << 28 | 1 << 24 | 1 << 20 | low_pwr_stop_mode << 16 |
	       eof_bllp_pwr << 12 | traffic_mode << 8 | dst_format << 4 | 0x0,
	       DSI_VIDEO_MODE_CTRL);

	writel(0x67, DSI_CAL_STRENGTH_CTRL);
	writel(0x80006711, DSI_CAL_CTRL);
	writel(0x00010100, DSI_MISR_VIDEO_CTRL);

	writel(0x00010100, DSI_INT_CTRL);
	writel(0x02010202, DSI_INT_CTRL);
	writel(0x02030303, DSI_INT_CTRL);

	writel(interleav << 30 | 0 << 24 | 0 << 20 | lane_en << 4
	       | 0x103, DSI_CTRL);
}
