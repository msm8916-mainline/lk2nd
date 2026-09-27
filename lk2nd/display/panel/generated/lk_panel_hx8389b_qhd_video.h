// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_HX8389B_QHD_VIDEO_H_
#define _PANEL_HX8389B_QHD_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config hx8389b_qhd_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_hx8389b_qhd_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution hx8389b_qhd_video_panel_res = {
	.panel_width = 540,
	.panel_height = 960,
	.hfront_porch = 36,
	.hback_porch = 63,
	.hpulse_width = 39,
	.hsync_skew = 0,
	.vfront_porch = 7,
	.vback_porch = 15,
	.vpulse_width = 3,
	/* Borders not supported yet */
};

static struct color_info hx8389b_qhd_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char hx8389b_qhd_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb9, 0xff, 0x83, 0x89
};
static char hx8389b_qhd_video_on_cmd_1[] = {
	0x08, 0x00, 0x39, 0xc0, 0xba, 0x41, 0x93, 0x00,
	0x16, 0xa4, 0x50, 0x18
};
static char hx8389b_qhd_video_on_cmd_2[] = {
	0xc6, 0x08, 0x15, 0x80
};
static char hx8389b_qhd_video_on_cmd_3[] = {
	0xcc, 0x02, 0x15, 0x80
};
static char hx8389b_qhd_video_on_cmd_4[] = {
	0x14, 0x00, 0x39, 0xc0, 0xb1, 0x00, 0x00, 0x07,
	0xef, 0x97, 0x10, 0x11, 0x94, 0xf1, 0x26, 0x2e,
	0x3f, 0x3f, 0x42, 0x01, 0x32, 0xf7, 0x20, 0x80
};
static char hx8389b_qhd_video_on_cmd_5[] = {
	0x04, 0x00, 0x39, 0xc0, 0xde, 0x05, 0x58, 0x10
};
static char hx8389b_qhd_video_on_cmd_6[] = {
	0x08, 0x00, 0x39, 0xc0, 0xb2, 0x00, 0x00, 0x78,
	0x0f, 0x03, 0x3f, 0x80
};
static char hx8389b_qhd_video_on_cmd_7[] = {
	0x18, 0x00, 0x39, 0xc0, 0xb4, 0x80, 0x08, 0x00,
	0x32, 0x10, 0x07, 0x32, 0x10, 0x03, 0x32, 0x10,
	0x07, 0x27, 0x05, 0x48, 0x0b, 0x37, 0x05, 0x48,
	0x14, 0xff, 0x5b, 0x0a
};
static char hx8389b_qhd_video_on_cmd_8[] = {
	0x31, 0x00, 0x39, 0xc0, 0xd5, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x01, 0x60, 0x00, 0x99,
	0x88, 0x88, 0x88, 0x88, 0x23, 0x88, 0x01, 0x88,
	0x67, 0x88, 0x45, 0x01, 0x23, 0x23, 0x45, 0x88,
	0x88, 0x88, 0x88, 0x99, 0x88, 0x88, 0x88, 0x54,
	0x88, 0x76, 0x88, 0x10, 0x88, 0x32, 0x32, 0x10,
	0x88, 0x88, 0x88, 0x88, 0x88, 0xff, 0xff, 0xff
};
static char hx8389b_qhd_video_on_cmd_9[] = {
	0x23, 0x00, 0x39, 0xc0, 0xe0, 0x01, 0x17, 0x1c,
	0x34, 0x32, 0x3f, 0x2c, 0x4b, 0x06, 0x0b, 0x11,
	0x14, 0x16, 0x13, 0x13, 0x1b, 0x1b, 0x01, 0x17,
	0x1c, 0x34, 0x32, 0x3f, 0x2c, 0x4b, 0x06, 0x0b,
	0x11, 0x14, 0x16, 0x13, 0x13, 0x1b, 0x1b, 0xff
};
static char hx8389b_qhd_video_on_cmd_10[] = {
	0x80, 0x00, 0x39, 0xc0, 0xc1, 0x01, 0x00, 0x08,
	0x10, 0x18, 0x20, 0x28, 0x30, 0x34, 0x3b, 0x45,
	0x4b, 0x52, 0x5a, 0x62, 0x6c, 0x75, 0x7e, 0x84,
	0x89, 0x94, 0x9d, 0xa5, 0xac, 0xb7, 0xbc, 0xc3,
	0xcb, 0xd1, 0xda, 0xe3, 0xec, 0xf7, 0xff, 0x00,
	0x00, 0x41, 0x75, 0x5c, 0x1d, 0xbd, 0x15, 0xc0,
	0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x34,
	0x3a, 0x43, 0x49, 0x50, 0x58, 0x60, 0x69, 0x71,
	0x7b, 0x81, 0x87, 0x90, 0x99, 0xa2, 0xa8, 0xb2,
	0xb8, 0xc0, 0xc8, 0xcd, 0xd4, 0xde, 0xe6, 0xf0,
	0xf9, 0x00, 0x00, 0x6a, 0x56, 0x63, 0x0c, 0xc5,
	0xc2, 0xc0, 0x00, 0x08, 0x10, 0x18, 0x20, 0x28,
	0x30, 0x34, 0x3a, 0x43, 0x49, 0x50, 0x58, 0x60,
	0x69, 0x71, 0x7b, 0x81, 0x87, 0x90, 0x99, 0xa2,
	0xa8, 0xb2, 0xb8, 0xc0, 0xc8, 0xcd, 0xd4, 0xde,
	0xe6, 0xf0, 0xf9, 0x00, 0x00, 0x6a, 0x56, 0x63,
	0x0c, 0xc5, 0xc2, 0xc0
};
static char hx8389b_qhd_video_on_cmd_11[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb6, 0x00, 0x93, 0x00,
	0x93, 0xff, 0xff, 0xff
};
static char hx8389b_qhd_video_on_cmd_12[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb7, 0x00, 0x00, 0x4d
};
static char hx8389b_qhd_video_on_cmd_13[] = {
	0x35, 0x01, 0x15, 0x80
};
static char hx8389b_qhd_video_on_cmd_14[] = {
	0x51, 0xff, 0x15, 0x80
};
static char hx8389b_qhd_video_on_cmd_15[] = {
	0x53, 0x2c, 0x15, 0x80
};
static char hx8389b_qhd_video_on_cmd_16[] = {
	0x55, 0x00, 0x15, 0x80
};
static char hx8389b_qhd_video_on_cmd_17[] = {
	0x11, 0x00, 0x05, 0x80
};
static char hx8389b_qhd_video_on_cmd_18[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8389b_qhd_video_on_command[] = {
	{ sizeof(hx8389b_qhd_video_on_cmd_0), hx8389b_qhd_video_on_cmd_0, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_1), hx8389b_qhd_video_on_cmd_1, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_2), hx8389b_qhd_video_on_cmd_2, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_3), hx8389b_qhd_video_on_cmd_3, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_4), hx8389b_qhd_video_on_cmd_4, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_5), hx8389b_qhd_video_on_cmd_5, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_6), hx8389b_qhd_video_on_cmd_6, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_7), hx8389b_qhd_video_on_cmd_7, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_8), hx8389b_qhd_video_on_cmd_8, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_9), hx8389b_qhd_video_on_cmd_9, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_10), hx8389b_qhd_video_on_cmd_10, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_11), hx8389b_qhd_video_on_cmd_11, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_12), hx8389b_qhd_video_on_cmd_12, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_13), hx8389b_qhd_video_on_cmd_13, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_14), hx8389b_qhd_video_on_cmd_14, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_15), hx8389b_qhd_video_on_cmd_15, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_16), hx8389b_qhd_video_on_cmd_16, 1 },
	{ sizeof(hx8389b_qhd_video_on_cmd_17), hx8389b_qhd_video_on_cmd_17, 120 },
	{ sizeof(hx8389b_qhd_video_on_cmd_18), hx8389b_qhd_video_on_cmd_18, 50 },
};

static char hx8389b_qhd_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char hx8389b_qhd_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8389b_qhd_video_off_command[] = {
	{ sizeof(hx8389b_qhd_video_off_cmd_0), hx8389b_qhd_video_off_cmd_0, 50 },
	{ sizeof(hx8389b_qhd_video_off_cmd_1), hx8389b_qhd_video_off_cmd_1, 120 },
};

static struct command_state hx8389b_qhd_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info hx8389b_qhd_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info hx8389b_qhd_video_video_panel = {
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

static struct lane_configuration hx8389b_qhd_video_lane_config = {
	.dsi_lanes = 2,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 0,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t hx8389b_qhd_video_timings[] = {
	0x87, 0x1c, 0x12, 0x00, 0x40, 0x44, 0x16, 0x28, 0x17, 0x03, 0x04, 0x00
};

static struct panel_timing hx8389b_qhd_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence hx8389b_qhd_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 50, 20 },
	.pin_direction = 2,
};

static struct backlight hx8389b_qhd_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_hx8389b_qhd_video_select(struct panel_struct *panel,
						  struct msm_panel_info *pinfo,
						  struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &hx8389b_qhd_video_panel_data;
	panel->panelres = &hx8389b_qhd_video_panel_res;
	panel->color = &hx8389b_qhd_video_color;
	panel->videopanel = &hx8389b_qhd_video_video_panel;
	panel->commandpanel = &hx8389b_qhd_video_command_panel;
	panel->state = &hx8389b_qhd_video_state;
	panel->laneconfig = &hx8389b_qhd_video_lane_config;
	panel->paneltiminginfo = &hx8389b_qhd_video_timing_info;
	panel->panelresetseq = &hx8389b_qhd_video_reset_seq;
	panel->backlightinfo = &hx8389b_qhd_video_backlight;
	pinfo->mipi.panel_on_cmds = hx8389b_qhd_video_on_command;
	pinfo->mipi.panel_off_cmds = hx8389b_qhd_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(hx8389b_qhd_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(hx8389b_qhd_video_off_command);
	memcpy(phy_db->timing, hx8389b_qhd_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_HX8389B_QHD_VIDEO_H_ */
