/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
#include <debug.h>
#include <platform/iomap.h>
#include <mdp4.h>
#include <clock.h>

#include "mipi_tc358764_dsi2lvds.h"

int mipi_chimei_video_wxga_early_config(void *pdata)
{
	uint32_t tmp;
	uint32_t reg = DSI_LANE_CTRL;
	struct msm_panel_info *pinfo = (struct msm_panel_info *)pdata;

	/* Force DSI-clock ON */
	tmp = readl_relaxed(reg);
	tmp |= (1<<28);
	writel_relaxed(tmp, reg);

	mipi_d2l_dsi_init_sequence(pinfo);
}

int mipi_chimei_video_wxga_config(void *pdata)
{
	int ret = NO_ERROR;
	struct lcdc_panel_info *lcdc = NULL;
	struct msm_panel_info *pinfo = (struct msm_panel_info *)pdata;
	unsigned char lane_en = 0x0F; /* 4 Lanes */
	unsigned long low_pwr_stop_mode = 0;
	unsigned char eof_bllp_pwr = 0;
	unsigned char interleav = 0;

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

int mipi_chimei_video_wxga_on(void)
{
	int ret = 0;

	return ret;
}

int mipi_chimei_video_wxga_off(void)
{
	int ret = 0;

	return ret;
}

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSIPHY_REGULATOR_CTRL */
	.regulator = {0x03, 0x0a, 0x04, 0x00, 0x20}, /* common 8960 */
	/* DSIPHY_CTRL */
	.ctrl = {0x5f, 0x00, 0x00, 0x10}, /* common 8960 */
	/* DSIPHY_STRENGTH_CTRL */
	.strength = {0xff, 0x00, 0x06, 0x00}, /* common 8960 */
	/* DSIPHY_TIMING_CTRL */
	.timing = { 0xB6, 0x8D, 0x1E, /* panel specific */
	0, /* DSIPHY_TIMING_CTRL_3 = 0 */
	0x21, 0x95, 0x21, 0x8F, 0x21, 0x03, 0x04},  /* panel specific */
	/* DSIPHY_PLL_CTRL */
	.pll = { 0x01, /* common 8960 */
	/* VCO */
	0xD8, 0x01, 0x1A, /* panel specific */
	0x00, 0x50, 0x48, 0x63,
	0x71, 0x0F, 0x03, /* Clocks dividers */
	0x00, 0x14, 0x03, 0x00, 0x02, /* common 8960 */
	0x00, 0x20, 0x00, 0x01 }, /* common 8960 */
};

void mipi_chimei_video_wxga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	dprintf(INFO, "mipi-dsi chimei wxga (1366x768) driver ver 1.0.\n");

	/* Landscape */
	pinfo->xres = 1366;
	pinfo->yres = 768;
	pinfo->type =  MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24; /* RGB888 = 24 bits-per-pixel */

	/* bitclk */
	pinfo->clk_rate = 473400000; /* 473.4 MHZ Calculated */

	/*
	 * this panel is operated by DE,
	 * vsycn and hsync are ignored
	 */

	pinfo->lcdc.h_front_porch = 96+2;/* thfp */
	pinfo->lcdc.h_back_porch = 88;	/* thb */
	pinfo->lcdc.h_pulse_width = 40;	/* thpw */

	pinfo->lcdc.v_front_porch = 15;	/* tvfp */
	pinfo->lcdc.v_back_porch = 23;	/* tvb */
	pinfo->lcdc.v_pulse_width = 20;	/* tvpw */

	pinfo->lcdc.border_clr = 0;		/* black */
	pinfo->lcdc.underflow_clr = 0xff;	/* blue */

	pinfo->lcdc.hsync_skew = 0;

	/* mipi - general */
	pinfo->mipi.vc = 0; /* virtual channel */
	pinfo->mipi.rgb_swap = DSI_RGB_SWAP_RGB;
	pinfo->mipi.tx_eot_append = true;
	pinfo->mipi.t_clk_post = 34;		/* Calculated */
	pinfo->mipi.t_clk_pre = 64;		/* Calculated */

	pinfo->mipi.dsi_phy_db = &dsi_video_mode_phy_db;

	/* Four lanes are recomended for 1366x768 at 60 frames per second */
	pinfo->mipi.frame_rate = 60; /* 60 frames per second */
	pinfo->mipi.data_lane0 = true;
	pinfo->mipi.data_lane1 = true;
	pinfo->mipi.data_lane2 = true;
	pinfo->mipi.data_lane3 = true;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	/*
	 * Note: The CMI panel input is RGB888,
	 * thus the DSI-to-LVDS bridge output is RGB888.
	 * This parameter selects the DSI-Core output to the bridge.
	 */
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;

	/* mipi - video mode */
	pinfo->mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo->mipi.pulse_mode_hsa_he = false; /* sync mode */

	pinfo->mipi.hfp_power_stop = false;
	pinfo->mipi.hbp_power_stop = false;
	pinfo->mipi.hsa_power_stop = false;
	pinfo->mipi.eof_bllp_power_stop = false;
	pinfo->mipi.bllp_power_stop = false;

	/* mipi - command mode */
	pinfo->mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo->mipi.interleave_max = 1;
	/* The bridge supports only Generic Read/Write commands */
	pinfo->mipi.insert_dcs_cmd = false;
	pinfo->mipi.wr_mem_continue = 0;
	pinfo->mipi.wr_mem_start = 0;
	pinfo->mipi.stream = false; /* dma_p */
	pinfo->mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
	pinfo->mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	/*
	 * toshiba d2l chip does not need max_pkt_szie dcs cmd
	 * client reply len is directly configure through
	 * RDPKTLN register (0x0404)
	 */
	pinfo->mipi.no_max_pkt_size = 1;
	pinfo->mipi.force_clk_lane_hs = 1;

	pinfo->mipi.num_of_lanes = 4;
	pinfo->mipi.panel_cmds = NULL; /* use in mipi_dsi_panel_initialize() */
	pinfo->mipi.num_of_panel_cmds = 0;


	/* Provide config/on/off callbacks */
	pinfo->on = mipi_chimei_video_wxga_on;
	pinfo->off = mipi_chimei_video_wxga_off;
	pinfo->config = mipi_chimei_video_wxga_config;
	pinfo->early_config = mipi_chimei_video_wxga_early_config;

	return;
}
