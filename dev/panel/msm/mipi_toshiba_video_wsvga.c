/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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

#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <platform/iomap.h>
#include <mdp4.h>

int mipi_toshiba_video_wsvga_config(void *pdata)
{
	int ret = NO_ERROR;

	/* 3 Lanes -- Enables Data Lane0, 1, 2 */
	unsigned char lane_en = 7;
	unsigned long low_pwr_stop_mode = 0;

	/* Needed or else will have blank line at top of display */
	unsigned char eof_bllp_pwr = 0x8;

	unsigned char interleav = 0;
	struct lcdc_panel_info *lcdc = NULL;
	struct msm_panel_info *pinfo = (struct msm_panel_info *)pdata;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	ret = mipi_dsi_video_mode_config((pinfo->xres + lcdc->xres_pad),
			(pinfo->yres + lcdc->yres_pad),
			(pinfo->xres),
			(pinfo->yres),
			(lcdc->h_front_porch),
			(lcdc->h_back_porch + lcdc->h_pulse_width),
			(lcdc->v_front_porch),
			(lcdc->v_back_porch + lcdc->v_pulse_width),
			(lcdc->h_pulse_width),
			(lcdc->v_pulse_width),
			pinfo->mipi.dst_format,
			pinfo->mipi.traffic_mode,
			lane_en,
			low_pwr_stop_mode,
			eof_bllp_pwr,
			interleav);
	return ret;
}

int mipi_toshiba_video_wsvga_on()
{
	int ret = NO_ERROR;
	return ret;
}

int mipi_toshiba_video_wsvga_off()
{
	int ret = NO_ERROR;
	return ret;
}

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* 600*1024, RGB888, 3 Lane 55 fps video mode */
	/* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0xab, 0x8a, 0x18, 0x00, 0x92, 0x97, 0x1b, 0x8c,
		0x0c, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x7f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x41, 0x0f, 0x01,
		0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

void mipi_toshiba_video_wsvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = 600;
	pinfo->yres = 1024;
	/*
	 *
	 * Panel's Horizontal input timing requirement is to
	 * include dummy(pad) data of 200 clk in addition to
	 * width and porch/sync width values
	 */
	pinfo->lcdc.xres_pad = 200;
	pinfo->lcdc.yres_pad = 0;

	pinfo->type = MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;
	pinfo->lcdc.h_back_porch = 16;
	pinfo->lcdc.h_front_porch = 23;
	pinfo->lcdc.h_pulse_width = 8;
	pinfo->lcdc.v_back_porch = 2;
	pinfo->lcdc.v_front_porch = 7;
	pinfo->lcdc.v_pulse_width = 2;
	pinfo->lcdc.border_clr = 0;	/* blk */
	pinfo->lcdc.underflow_clr = 0xff;	/* blue */
	pinfo->lcdc.hsync_skew = 0;
	pinfo->clk_rate = 384000000;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	pinfo->mipi.pulse_mode_hsa_he = FALSE;
	pinfo->mipi.hfp_power_stop = FALSE;
	pinfo->mipi.hbp_power_stop = FALSE;
	pinfo->mipi.hsa_power_stop = FALSE;
	pinfo->mipi.eof_bllp_power_stop = FALSE;
	pinfo->mipi.bllp_power_stop = FALSE;
	pinfo->mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo->mipi.vc = 0;
	pinfo->mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo->mipi.data_lane0 = TRUE;
	pinfo->mipi.data_lane1 = TRUE;
	pinfo->mipi.data_lane2 = TRUE;
	pinfo->mipi.t_clk_post = 0x20;
	pinfo->mipi.t_clk_pre = 0x2d;
	pinfo->mipi.stream = 0; /* dma_p */
	pinfo->mipi.mdp_trigger = 0;
	pinfo->mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo->mipi.frame_rate = 55;
	pinfo->mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.num_of_lanes = 3;
	pinfo->mipi.panel_cmds = toshiba_mdt61_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds =
				 ARRAY_SIZE(toshiba_mdt61_video_mode_cmds);

	pinfo->on = mipi_toshiba_video_wsvga_on;
	pinfo->off = mipi_toshiba_video_wsvga_off;
	pinfo->config = mipi_toshiba_video_wsvga_config;

	return;
}
