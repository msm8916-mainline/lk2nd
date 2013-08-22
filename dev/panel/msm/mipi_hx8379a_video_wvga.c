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

#define HX8379A_PANEL_FRAME_RATE        60
#define HX8379A_PANEL_NUM_OF_LANES	2
#define HX8379A_PANEL_LANE_SWAP         1
#define HX8379A_PANEL_T_CLK_PRE         0x41b
#define HX8379A_PANEL_T_CLK_POST        0x0
#define HX8379A_PANEL_BPP               24
#define HX8379A_PANEL_CLK_RATE          499000000

#define MIPI_HSYNC_PULSE_WIDTH      40
#define MIPI_HSYNC_BACK_PORCH_DCLK   100
#define MIPI_HSYNC_FRONT_PORCH_DCLK  70
#define MIPI_VSYNC_PULSE_WIDTH       4
#define MIPI_VSYNC_BACK_PORCH_LINES  6
#define MIPI_VSYNC_FRONT_PORCH_LINES 6

static char disp_on0[8] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB9, 0xFF, 0x83, 0x79
};
static char disp_on1[8] = {
	0x03, 0x00, 0x39, 0xC0,
	0xBA, 0x51, 0x93, 0xFF
};
static char disp_on2[24] = {
	0x14, 0x00, 0x39, 0xC0,
	0xB1, 0x00, 0x50, 0x44,
	0xEA, 0x8D, 0x08, 0x11,
	0x11, 0x11, 0x27, 0x2F,
	0x9A, 0x1A, 0x42, 0x0B,
	0x6E, 0xF1, 0x00, 0xE6
};
static char disp_on3[20] = {
	0x0E, 0x00, 0x39, 0xC0,
	0xB2, 0x00, 0x00, 0x3C,
	0x08, 0x04, 0x19, 0x22,
	0x00, 0xFF, 0x08, 0x04,
	0x19, 0x20, 0xFF, 0xFF
};
static char disp_on4[36] = {
	0x20, 0x00, 0x39, 0xC0,
	0xB4, 0x82, 0x08, 0x00,
	0x32, 0x10, 0x03, 0x32,
	0x13, 0x70, 0x32, 0x10,
	0x08, 0x37, 0x01, 0x28,
	0x07, 0x37, 0x08, 0x3C,
	0x08, 0x44, 0x44, 0x08,
	0x00, 0x40, 0x08, 0x28,
	0x08, 0x30, 0x30, 0x04
};
static char disp_on5[52] = {
	0x30, 0x00, 0x39, 0xC0,
	0xD5, 0x00, 0x00, 0x0A,
	0x00, 0x01, 0x05, 0x00,
	0x03, 0x00, 0x88, 0x88,
	0x88, 0x88, 0x23, 0x01,
	0x67, 0x45, 0x02, 0x13,
	0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x54, 0x76,
	0x10, 0x32, 0x31, 0x20,
	0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char disp_on6[40] = {
	0x24, 0x00, 0x39, 0xC0,
	0xE0, 0x79, 0x05, 0x0F,
	0x14, 0x26, 0x29, 0x3F,
	0x2B, 0x44, 0x04, 0x0E,
	0x12, 0x15, 0x18, 0x16,
	0x16, 0x12, 0x15, 0x05,
	0x0F, 0x14, 0x26, 0x29,
	0x3F, 0x2B, 0x44, 0x04,
	0x0E, 0x12, 0x15, 0x18,
	0x16, 0x16, 0x12, 0x15
};
static char disp_on7[4] = {
	0xCC, 0x02, 0x23, 0x80,
};
static char disp_on8[12] = {
	0x05, 0x00, 0x39, 0xC0,
	0xB6, 0x00, 0x9C, 0x00,
	0x9C, 0xFF, 0xFF, 0xFF
};
static char disp_on9[4] = {
	0x11, 0x00, 0x05, 0x80
};
static char disp_on10[4] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8379a_wvga_panel_video_mode_cmds[] = {
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
};

int mipi_hx8379a_video_wvga_config(void *pdata)
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

int mipi_hx8379a_video_wvga_on()
{
	int ret = NO_ERROR;
	return ret;
}

int mipi_hx8379a_video_wvga_off()
{
	int ret = NO_ERROR;
	return ret;
}

static struct mdss_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* regulator */
	{0x02, 0x08, 0x05, 0x00, 0x20, 0x03},
	/* timing   */
	{0x75, 0x1A, 0x11,  0x00, 0x3D, 0x45,
		0x15, 0x1D, 0x1C, 0x03, 0x04, 0x00},
	/* phy ctrl */
	{0x7f, 0x00, 0x00, 0x00},
	/* strength */
	{0xff, 0x06},
	/* bist */
	{0x03, 0x03, 0x00, 0x00, 0x0f, 0x00},
	/* lane config */
	{	0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
		0x40, 0x67, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, 0x00},
};

void mipi_hx8379a_video_wvga_init(struct msm_panel_info *pinfo)
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
	pinfo->mipi.num_of_lanes = HX8379A_PANEL_NUM_OF_LANES;
	pinfo->mipi.frame_rate = HX8379A_PANEL_FRAME_RATE;

	pinfo->type = MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = HX8379A_PANEL_BPP;
	pinfo->clk_rate = HX8379A_PANEL_CLK_RATE;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	pinfo->mipi.traffic_mode = 2;
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo->mipi.mdss_dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = HX8379A_PANEL_LANE_SWAP;
	pinfo->mipi.t_clk_post = HX8379A_PANEL_T_CLK_POST;
	pinfo->mipi.t_clk_pre = HX8379A_PANEL_T_CLK_PRE;
	pinfo->mipi.panel_cmds = hx8379a_wvga_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(hx8379a_wvga_panel_video_mode_cmds);

	pinfo->on = mipi_hx8379a_video_wvga_on;
	pinfo->off = mipi_hx8379a_video_wvga_off;
	pinfo->config = mipi_hx8379a_video_wvga_config;

	return;
};
