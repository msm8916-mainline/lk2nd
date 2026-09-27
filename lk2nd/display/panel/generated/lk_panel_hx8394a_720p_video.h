// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_HX8394A_720P_VIDEO_H_
#define _PANEL_HX8394A_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config hx8394a_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_hx8394a_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution hx8394a_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 144,
	.hback_porch = 204,
	.hpulse_width = 10,
	.hsync_skew = 0,
	.vfront_porch = 6,
	.vback_porch = 9,
	.vpulse_width = 1,
	/* Borders not supported yet */
};

static struct color_info hx8394a_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char hx8394a_720p_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb9, 0xff, 0x83, 0x94
};
static char hx8394a_720p_video_on_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbc, 0x07, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_2[] = {
	0x07, 0x00, 0x39, 0xc0, 0xba, 0x13, 0x82, 0x00,
	0x16, 0xc5, 0x40, 0xff
};
static char hx8394a_720p_video_on_cmd_3[] = {
	0x11, 0x00, 0x39, 0xc0, 0xb1, 0x01, 0x00, 0x07,
	0x86, 0x01, 0x11, 0x11, 0x2a, 0x30, 0x3f, 0x3f,
	0x47, 0x12, 0x01, 0xe6, 0xe2, 0xff, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_4[] = {
	0x07, 0x00, 0x39, 0xc0, 0xb2, 0x00, 0xc8, 0x08,
	0x04, 0x00, 0x22, 0xff
};
static char hx8394a_720p_video_on_cmd_5[] = {
	0x21, 0x00, 0x39, 0xc0, 0xd5, 0x00, 0x00, 0x00,
	0x00, 0x0a, 0x00, 0x01, 0x00, 0xcc, 0x00, 0x00,
	0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x01, 0x67, 0x45, 0x23, 0x01,
	0x23, 0x88, 0x88, 0x88, 0x88, 0xff, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_6[] = {
	0x17, 0x00, 0x39, 0xc0, 0xb4, 0x80, 0x06, 0x32,
	0x10, 0x03, 0x32, 0x15, 0x08, 0x32, 0x10, 0x08,
	0x33, 0x04, 0x43, 0x05, 0x37, 0x04, 0x3f, 0x06,
	0x61, 0x61, 0x06, 0xff
};
static char hx8394a_720p_video_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb6, 0xfa, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_8[] = {
	0x2b, 0x00, 0x39, 0xc0, 0xe0, 0x02, 0x07, 0x07,
	0x2b, 0x33, 0x3f, 0x0f, 0x32, 0x04, 0x0a, 0x0d,
	0x11, 0x13, 0x11, 0x13, 0x10, 0x17, 0x02, 0x07,
	0x07, 0x2b, 0x33, 0x3f, 0x0f, 0x32, 0x04, 0x0a,
	0x0d, 0x11, 0x13, 0x11, 0x13, 0x10, 0x17, 0x07,
	0x15, 0x07, 0x11, 0x07, 0x15, 0x07, 0x11, 0xff
};
static char hx8394a_720p_video_on_cmd_9[] = {
	0x80, 0x00, 0x39, 0xc0, 0xc1, 0x01, 0x00, 0x0e,
	0x15, 0x1e, 0x27, 0x30, 0x39, 0x41, 0x48, 0x4f,
	0x57, 0x5e, 0x66, 0x6e, 0x77, 0x7f, 0x87, 0x8e,
	0x96, 0x9e, 0xa5, 0xae, 0xb5, 0xbc, 0xc3, 0xcc,
	0xd4, 0xda, 0xe1, 0xe7, 0xef, 0xf7, 0xff, 0x31,
	0x25, 0x67, 0x5e, 0x1e, 0xd6, 0x10, 0x21, 0xc0,
	0x00, 0x0e, 0x14, 0x1c, 0x25, 0x2d, 0x36, 0x3d,
	0x44, 0x4b, 0x52, 0x59, 0x60, 0x67, 0x71, 0x78,
	0x7f, 0x86, 0x8e, 0x95, 0x9c, 0xa3, 0xab, 0xb1,
	0xb8, 0xbf, 0xc7, 0xcc, 0xd4, 0xdc, 0xe1, 0xe7,
	0xee, 0x12, 0x33, 0xdb, 0xb1, 0x95, 0xa6, 0xe3,
	0x13, 0xc0, 0x00, 0x0e, 0x14, 0x1d, 0x26, 0x2f,
	0x38, 0x40, 0x47, 0x4d, 0x55, 0x5d, 0x64, 0x6b,
	0x75, 0x7d, 0x84, 0x8b, 0x93, 0x9b, 0xa2, 0xa9,
	0xb2, 0xb8, 0xc0, 0xc8, 0xce, 0xd5, 0xdc, 0xe2,
	0xea, 0xf1, 0xf9, 0x2e, 0xa4, 0x38, 0x78, 0x6d,
	0xb7, 0x2d, 0xb1, 0xc0
};
static char hx8394a_720p_video_on_cmd_10[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x09, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_11[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc0, 0x0c, 0x17, 0xff
};
static char hx8394a_720p_video_on_cmd_12[] = {
	0x05, 0x00, 0x39, 0xc0, 0xc7, 0x00, 0x10, 0x00,
	0x10, 0xff, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_13[] = {
	0x03, 0x00, 0x39, 0xc0, 0x44, 0x02, 0x80, 0xff
};
static char hx8394a_720p_video_on_cmd_14[] = {
	0x35, 0x00, 0x05, 0x80
};
static char hx8394a_720p_video_on_cmd_15[] = {
	0x05, 0x00, 0x39, 0xc0, 0xbf, 0x06, 0x02, 0x10,
	0x04, 0xff, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb9, 0x00, 0xff, 0xff
};
static char hx8394a_720p_video_on_cmd_17[] = {
	0x11, 0x00, 0x05, 0x80
};
static char hx8394a_720p_video_on_cmd_18[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8394a_720p_video_on_command[] = {
	{ sizeof(hx8394a_720p_video_on_cmd_0), hx8394a_720p_video_on_cmd_0, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_1), hx8394a_720p_video_on_cmd_1, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_2), hx8394a_720p_video_on_cmd_2, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_3), hx8394a_720p_video_on_cmd_3, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_4), hx8394a_720p_video_on_cmd_4, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_5), hx8394a_720p_video_on_cmd_5, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_6), hx8394a_720p_video_on_cmd_6, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_7), hx8394a_720p_video_on_cmd_7, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_8), hx8394a_720p_video_on_cmd_8, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_9), hx8394a_720p_video_on_cmd_9, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_10), hx8394a_720p_video_on_cmd_10, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_11), hx8394a_720p_video_on_cmd_11, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_12), hx8394a_720p_video_on_cmd_12, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_13), hx8394a_720p_video_on_cmd_13, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_14), hx8394a_720p_video_on_cmd_14, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_15), hx8394a_720p_video_on_cmd_15, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_16), hx8394a_720p_video_on_cmd_16, 0 },
	{ sizeof(hx8394a_720p_video_on_cmd_17), hx8394a_720p_video_on_cmd_17, 150 },
	{ sizeof(hx8394a_720p_video_on_cmd_18), hx8394a_720p_video_on_cmd_18, 20 },
};

static char hx8394a_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char hx8394a_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8394a_720p_video_off_command[] = {
	{ sizeof(hx8394a_720p_video_off_cmd_0), hx8394a_720p_video_off_cmd_0, 50 },
	{ sizeof(hx8394a_720p_video_off_cmd_1), hx8394a_720p_video_off_cmd_1, 120 },
};

static struct command_state hx8394a_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info hx8394a_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info hx8394a_720p_video_video_panel = {
	.hsync_pulse = 1,
	.hfp_power_mode = 0,
	.hbp_power_mode = 1,
	.hsa_power_mode = 0,
	.bllp_eof_power_mode = 1,
	.bllp_power_mode = 1,
	.traffic_mode = 2,
	/* This is bllp_eof_power_mode and bllp_power_mode combined */
	.bllp_eof_power = 1 << 3 | 1 << 0,
};

static struct lane_configuration hx8394a_720p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t hx8394a_720p_video_timings[] = {
	0x8d, 0x24, 0x19, 0x00, 0x34, 0x34, 0x1d, 0x26, 0x2a, 0x03, 0x04, 0x00
};

static struct panel_timing hx8394a_720p_video_timing_info = {
	.tclk_post = 0x1f,
	.tclk_pre = 0x2d,
};

static struct panel_reset_sequence hx8394a_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 5, 20 },
	.pin_direction = 2,
};

static struct backlight hx8394a_720p_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_hx8394a_720p_video_select(struct panel_struct *panel,
						   struct msm_panel_info *pinfo,
						   struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &hx8394a_720p_video_panel_data;
	panel->panelres = &hx8394a_720p_video_panel_res;
	panel->color = &hx8394a_720p_video_color;
	panel->videopanel = &hx8394a_720p_video_video_panel;
	panel->commandpanel = &hx8394a_720p_video_command_panel;
	panel->state = &hx8394a_720p_video_state;
	panel->laneconfig = &hx8394a_720p_video_lane_config;
	panel->paneltiminginfo = &hx8394a_720p_video_timing_info;
	panel->panelresetseq = &hx8394a_720p_video_reset_seq;
	panel->backlightinfo = &hx8394a_720p_video_backlight;
	pinfo->mipi.panel_on_cmds = hx8394a_720p_video_on_command;
	pinfo->mipi.panel_off_cmds = hx8394a_720p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(hx8394a_720p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(hx8394a_720p_video_off_command);
	memcpy(phy_db->timing, hx8394a_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_HX8394A_720P_VIDEO_H_ */
