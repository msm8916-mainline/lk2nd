/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
#include <target/display.h>
#include <platform/iomap.h>

#define WVGA_MIPI_FB_WIDTH            480
#define WVGA_MIPI_FB_HEIGHT           800

#define TRULY_PANEL_FRAME_RATE        60
#define TRULY_PANEL_NUM_OF_LANES      2
#define TRULY_PANEL_LANE_SWAP         0
#define TRULY_PANEL_T_CLK_PRE         0x41b
#define TRULY_PANEL_T_CLK_POST        0x0
#define TRULY_PANEL_BPP               24
#define TRULY_PANEL_CLK_RATE          499000000

static char disp_on0[4] = {
	0x01, 0x00, 0x05, 0x80
};
static char disp_on1[4] = {
	0xB0, 0x04, 0x23, 0x80
};
static char disp_on2[8] = {
	0x03, 0x00, 0x29, 0xC0,
	0xB3, 0x02, 0x00, 0xFF
};
static char disp_on3[4] = {
	0xBD, 0x00, 0x23, 0x80
};
static char disp_on4[8] = {
	0x03, 0x00, 0x29, 0xC0,
	0xC0, 0x18, 0x66, 0xFF
};
static char disp_on5[20] = {
	0x10, 0x00, 0x29, 0xC0,
	0xC1, 0x23, 0x31, 0x99,
	0x21, 0x20, 0x00, 0x30,
	0x28, 0x0C, 0x0C, 0x00,
	0x00, 0x00, 0x21, 0x01
};
static char disp_on6[12] = {
	0x07, 0x00, 0x29, 0xC0,
	0xC2, 0x00, 0x06, 0x06,
	0x01, 0x03, 0x00, 0xFF
};
static char disp_on7[32] = {
	0x19, 0x00, 0x29, 0xC0,
	0xC8, 0x04, 0x10, 0x18,
	0x20, 0x2E, 0x46, 0x3C,
	0x28, 0x1F, 0x18, 0x10,
	0x04, 0x04, 0x10, 0x18,
	0x20, 0x2E, 0x46, 0x3C,
	0x28, 0x1F, 0x18, 0x10,
	0x04, 0xFF, 0xFF, 0xFF
};
static char disp_on8[32] = {
	0x19, 0x00, 0x29, 0xC0,
	0xC9, 0x04, 0x10, 0x18,
	0x20, 0x2E, 0x46, 0x3C,
	0x28, 0x1F, 0x18, 0x10,
	0x04, 0x04, 0x10, 0x18,
	0x20, 0x2E, 0x46, 0x3C,
	0x28, 0x1F, 0x18, 0x10,
	0x04, 0xFF, 0xFF, 0xFF
};
static char disp_on9[32] = {
	0x19, 0x00, 0x29, 0xC0,
	0xCA, 0x04, 0x10, 0x18,
	0x20, 0x2E, 0x46, 0x3C,
	0x28, 0x1F, 0x18, 0x10,
	0x04, 0x04, 0x10, 0x18,
	0x20, 0x2E, 0x46, 0x3C,
	0x28, 0x1F, 0x18, 0x10,
	0x04, 0xFF, 0xFF, 0xFF
};
static char disp_on10[24] = {
	0x11, 0x00, 0x29, 0xC0,
	0xD0, 0x29, 0x03, 0xce,
	0xa6, 0x00, 0x43, 0x20,
	0x10, 0x01, 0x00, 0x01,
	0x01, 0x00, 0x03, 0x01,
	0x00, 0xFF, 0xFF, 0xFF
};
static char disp_on11[12] = {
	0x08, 0x00, 0x29, 0xC0,
	0xD1, 0x18, 0x0C, 0x23,
	0x03, 0x75, 0x02, 0x50
};
static char disp_on12[4] = {
	0xD3, 0x11, 0x23, 0x80
};
static char disp_on13[8] = {
	0x03, 0x00, 0x29, 0xC0,
	0xD5, 0x2A, 0x2A, 0xFF
};
static char disp_on14[8] = {
	0x03, 0x00, 0x29, 0xC0,
	0xDE, 0x01, 0x51, 0xFF
};
static char disp_on15[4] = {
	0xE6, 0x51, 0x23, 0x80
};
static char disp_on16[4] = {
	0xFA, 0x03, 0x23, 0x80
};
static char disp_on17[4] = {
	0xD6, 0x28, 0x23, 0x80
};
static char disp_on18[12] = {
	0x05, 0x00, 0x39, 0xC0,
	0x2A, 0x00, 0x00, 0x01,
	0xDF, 0xFF, 0xFF, 0xFF
};
static char disp_on19[12] = {
	0x05, 0x00, 0x39, 0xC0,
	0x2B, 0x00, 0x00, 0x03,
	0x1F, 0xFF, 0xFF, 0xFF
};
static char disp_on20[4] = {
	0x35, 0x00, 0x15, 0x80
};
static char disp_on21[8] = {
	0x03, 0x00, 0x39, 0xc0,
	0x44, 0x00, 0x50, 0xFF
};
static char disp_on22[4] = {
	0x36, 0xC1, 0x15, 0x80
};
static char disp_on23[4] = {
	0x3A, 0x77, 0x15, 0x80
};
static char disp_on24[4] = {
	0x11, 0x00, 0x05, 0x80
};
static char disp_on25[4] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd truly_wvga_panel_video_mode_cmds[] = {
	{sizeof(disp_on0), (char *)disp_on0},
	{sizeof(disp_on1), (char *)disp_on1},
	{sizeof(disp_on2), (char *)disp_on2},
	{sizeof(disp_on3), (char *)disp_on3},
	{sizeof(disp_on4), (char *)disp_on4},
	{sizeof(disp_on5), (char *)disp_on5},
	{sizeof(disp_on6), (char *)disp_on6},
	{sizeof(disp_on7), (char *)disp_on7},
	{sizeof(disp_on8), (char *)disp_on8},
	{sizeof(disp_on9), (char *)disp_on9},
	{sizeof(disp_on10), (char *)disp_on10},
	{sizeof(disp_on11), (char *)disp_on11},
	{sizeof(disp_on12), (char *)disp_on12},
	{sizeof(disp_on13), (char *)disp_on13},
	{sizeof(disp_on14), (char *)disp_on14},
	{sizeof(disp_on15), (char *)disp_on15},
	{sizeof(disp_on16), (char *)disp_on16},
	{sizeof(disp_on17), (char *)disp_on17},
	{sizeof(disp_on18), (char *)disp_on18},
	{sizeof(disp_on19), (char *)disp_on19},
	{sizeof(disp_on20), (char *)disp_on20},
	{sizeof(disp_on21), (char *)disp_on21},
	{sizeof(disp_on22), (char *)disp_on22},
	{sizeof(disp_on23), (char *)disp_on23},
	{sizeof(disp_on24), (char *)disp_on24},
	{sizeof(disp_on25), (char *)disp_on25},
};

int mipi_truly_video_wvga_config(void *pdata)
{
	int ret = NO_ERROR;
	/* 2 Lanes -- Enables Data Lane0, 1 */
	unsigned char lane_en = 3;
	unsigned long low_pwr_stop_mode = 0;

	/* Needed or else will have blank line at top of display */
	unsigned char eof_bllp_pwr = 0x9;

	unsigned char interleav = 0;
	struct lcdc_panel_info *lcdc = NULL;
	struct msm_panel_info *pinfo = (struct msm_panel_info *) pdata;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	ret = mdss_dsi_video_mode_config((pinfo->xres),
			(pinfo->yres),
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
			interleav,
			MIPI_DSI0_BASE);
	return ret;
}

int mipi_truly_video_wvga_on()
{
	int ret = NO_ERROR;
	return ret;
}

int mipi_truly_video_wvga_off()
{
	int ret = NO_ERROR;
	return ret;
}

static struct mdss_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* regulator */
	{0x02, 0x08, 0x05, 0x00, 0x20, 0x03},
	/* timing   */
	{0x5d, 0x12, 0x0c, 0x00, 0x33, 0x38,
		0x10, 0x16, 0x1e, 0x03, 0x04, 0x00},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xff, 0x06},
	/* bist */
	{0x03, 0x03, 0x00, 0x00, 0x0f, 0x00},
	/* lane config */
	{0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x40, 0x67, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, 0x00},
};

void mipi_truly_video_wvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = WVGA_MIPI_FB_WIDTH;
	pinfo->yres = WVGA_MIPI_FB_HEIGHT;
	pinfo->lcdc.h_back_porch = MIPI_HSYNC_BACK_PORCH_DCLK;
	pinfo->lcdc.h_front_porch = MIPI_HSYNC_FRONT_PORCH_DCLK;
	pinfo->lcdc.h_pulse_width = MIPI_HSYNC_PULSE_WIDTH;
	pinfo->lcdc.v_back_porch = MIPI_VSYNC_BACK_PORCH_LINES;
	pinfo->lcdc.v_front_porch = MIPI_VSYNC_FRONT_PORCH_LINES;
	pinfo->lcdc.v_pulse_width = MIPI_VSYNC_PULSE_WIDTH;
	pinfo->mipi.num_of_lanes = TRULY_PANEL_NUM_OF_LANES;
	pinfo->mipi.frame_rate = TRULY_PANEL_FRAME_RATE;

	pinfo->type = MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = TRULY_PANEL_BPP;
	pinfo->clk_rate = TRULY_PANEL_CLK_RATE;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	pinfo->mipi.traffic_mode = 1;
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo->mipi.mdss_dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = TRULY_PANEL_LANE_SWAP;
	pinfo->mipi.t_clk_post = TRULY_PANEL_T_CLK_POST;
	pinfo->mipi.t_clk_pre = TRULY_PANEL_T_CLK_PRE;
	pinfo->mipi.panel_cmds = truly_wvga_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(truly_wvga_panel_video_mode_cmds);

	pinfo->on = mipi_truly_video_wvga_on;
	pinfo->off = mipi_truly_video_wvga_off;
	pinfo->config = mipi_truly_video_wvga_config;

	return;
}
