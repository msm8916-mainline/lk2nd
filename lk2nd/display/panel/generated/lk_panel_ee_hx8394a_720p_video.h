// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_EE_HX8394A_720P_VIDEO_H_
#define _PANEL_EE_HX8394A_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config ee_hx8394a_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_hx8394a_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution ee_hx8394a_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 81,
	.hback_porch = 59,
	.hpulse_width = 60,
	.hsync_skew = 0,
	.vfront_porch = 7,
	.vback_porch = 10,
	.vpulse_width = 2,
	/* Borders not supported yet */
};

static struct color_info ee_hx8394a_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char ee_hx8394a_720p_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb9, 0xff, 0x83, 0x94
};
static char ee_hx8394a_720p_video_on_cmd_1[] = {
	0x11, 0x00, 0x39, 0xc0, 0xba, 0x13, 0x82, 0x00,
	0x16, 0xc5, 0x00, 0x10, 0xff, 0x0f, 0x24, 0x03,
	0x21, 0x24, 0x25, 0x20, 0x08, 0xff, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_2[] = {
	0x11, 0x00, 0x39, 0xc0, 0xb1, 0x01, 0x00, 0x04,
	0x87, 0x01, 0x11, 0x11, 0x2f, 0x37, 0x3f, 0x3f,
	0x47, 0x12, 0x01, 0xe6, 0xe2, 0xff, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_3[] = {
	0x07, 0x00, 0x39, 0xc0, 0xb2, 0x00, 0xc8, 0x08,
	0x04, 0x00, 0x22, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_4[] = {
	0x17, 0x00, 0x39, 0xc0, 0xb4, 0x80, 0x06, 0x32,
	0x10, 0x03, 0x32, 0x15, 0x08, 0x32, 0x10, 0x08,
	0x33, 0x04, 0x43, 0x05, 0x37, 0x04, 0x43, 0x06,
	0x61, 0x61, 0x06, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_5[] = {
	0x05, 0x00, 0x39, 0xc0, 0xbf, 0x06, 0x02, 0x10,
	0x04, 0xff, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_6[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc0, 0x0c, 0x17, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb6, 0x0b, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_8[] = {
	0x21, 0x00, 0x39, 0xc0, 0xd5, 0x00, 0x00, 0x00,
	0x00, 0x0a, 0x00, 0x01, 0x00, 0xcc, 0x00, 0x00,
	0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x01, 0x67, 0x45, 0x23, 0x01,
	0x23, 0x88, 0x88, 0x88, 0x88, 0xff, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_9[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x09, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_10[] = {
	0x05, 0x00, 0x39, 0xc0, 0xc7, 0x00, 0x10, 0x00,
	0x10, 0xff, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_11[] = {
	0x2b, 0x00, 0x39, 0xc0, 0xe0, 0x00, 0x04, 0x06,
	0x2b, 0x33, 0x3f, 0x13, 0x34, 0x0a, 0x0e, 0x0d,
	0x11, 0x13, 0x11, 0x13, 0x10, 0x17, 0x00, 0x04,
	0x06, 0x2b, 0x33, 0x3f, 0x13, 0x34, 0x0a, 0x0e,
	0x0d, 0x11, 0x13, 0x11, 0x13, 0x10, 0x17, 0x0b,
	0x17, 0x07, 0x11, 0x0b, 0x17, 0x07, 0x11, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd4, 0x32, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_13[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc9, 0x0f, 0x02, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_14[] = {
	0x02, 0x00, 0x39, 0xc0, 0x35, 0x00, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0x51, 0x00, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x2c, 0xff, 0xff
};
static char ee_hx8394a_720p_video_on_cmd_17[] = {
	0x11, 0x00, 0x05, 0x80
};
static char ee_hx8394a_720p_video_on_cmd_18[] = {
	0x29, 0x00, 0x05, 0x80
};
static char ee_hx8394a_720p_video_on_cmd_19[] = {
	0x51, 0xe1, 0x15, 0x80	/* Set brightness to maximum 225 */
};

static struct mipi_dsi_cmd ee_hx8394a_720p_video_on_command[] = {
	{ sizeof(ee_hx8394a_720p_video_on_cmd_0), ee_hx8394a_720p_video_on_cmd_0, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_1), ee_hx8394a_720p_video_on_cmd_1, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_2), ee_hx8394a_720p_video_on_cmd_2, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_3), ee_hx8394a_720p_video_on_cmd_3, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_4), ee_hx8394a_720p_video_on_cmd_4, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_5), ee_hx8394a_720p_video_on_cmd_5, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_6), ee_hx8394a_720p_video_on_cmd_6, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_7), ee_hx8394a_720p_video_on_cmd_7, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_8), ee_hx8394a_720p_video_on_cmd_8, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_9), ee_hx8394a_720p_video_on_cmd_9, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_10), ee_hx8394a_720p_video_on_cmd_10, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_11), ee_hx8394a_720p_video_on_cmd_11, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_12), ee_hx8394a_720p_video_on_cmd_12, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_13), ee_hx8394a_720p_video_on_cmd_13, 10 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_14), ee_hx8394a_720p_video_on_cmd_14, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_15), ee_hx8394a_720p_video_on_cmd_15, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_16), ee_hx8394a_720p_video_on_cmd_16, 0 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_17), ee_hx8394a_720p_video_on_cmd_17, 120 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_18), ee_hx8394a_720p_video_on_cmd_18, 100 },
	{ sizeof(ee_hx8394a_720p_video_on_cmd_19), ee_hx8394a_720p_video_on_cmd_19, 0 },
};

static char ee_hx8394a_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char ee_hx8394a_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd ee_hx8394a_720p_video_off_command[] = {
	{ sizeof(ee_hx8394a_720p_video_off_cmd_0), ee_hx8394a_720p_video_off_cmd_0, 10 },
	{ sizeof(ee_hx8394a_720p_video_off_cmd_1), ee_hx8394a_720p_video_off_cmd_1, 150 },
};

static struct command_state ee_hx8394a_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info ee_hx8394a_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info ee_hx8394a_720p_video_video_panel = {
	.hsync_pulse = 1,
	.hfp_power_mode = 0,
	.hbp_power_mode = 0,
	.hsa_power_mode = 0,
	.bllp_eof_power_mode = 1,
	.bllp_power_mode = 1,
	.traffic_mode = 2,
	/* This is bllp_eof_power_mode and bllp_power_mode combined */
	.bllp_eof_power = 1 << 3 | 1 << 0,
};

static struct lane_configuration ee_hx8394a_720p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t ee_hx8394a_720p_video_timings[] = {
	0x8d, 0x24, 0x19, 0x00, 0x34, 0x34, 0x1d, 0x26, 0x2a, 0x03, 0x04, 0x00
};

static struct panel_timing ee_hx8394a_720p_video_timing_info = {
	.tclk_post = 0x1f,
	.tclk_pre = 0x2d,
};

static struct panel_reset_sequence ee_hx8394a_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 1, 20 },
	.pin_direction = 2,
};

static struct backlight ee_hx8394a_720p_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_ee_hx8394a_720p_video_select(struct panel_struct *panel,
						   struct msm_panel_info *pinfo,
						   struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &ee_hx8394a_720p_video_panel_data;
	panel->panelres = &ee_hx8394a_720p_video_panel_res;
	panel->color = &ee_hx8394a_720p_video_color;
	panel->videopanel = &ee_hx8394a_720p_video_video_panel;
	panel->commandpanel = &ee_hx8394a_720p_video_command_panel;
	panel->state = &ee_hx8394a_720p_video_state;
	panel->laneconfig = &ee_hx8394a_720p_video_lane_config;
	panel->paneltiminginfo = &ee_hx8394a_720p_video_timing_info;
	panel->panelresetseq = &ee_hx8394a_720p_video_reset_seq;
	panel->backlightinfo = &ee_hx8394a_720p_video_backlight;
	pinfo->mipi.panel_on_cmds = ee_hx8394a_720p_video_on_command;
	pinfo->mipi.panel_off_cmds = ee_hx8394a_720p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(ee_hx8394a_720p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(ee_hx8394a_720p_video_off_command);
	memcpy(phy_db->timing, ee_hx8394a_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_EE_HX8394A_720P_VIDEO_H_ */
