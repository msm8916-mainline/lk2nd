// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_BOEHX8389C_QHD_VIDEO_H_
#define _PANEL_BOEHX8389C_QHD_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config boehx8389c_qhd_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_boehx8389c_qHD_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 10000,
};

static struct panel_resolution boehx8389c_qhd_video_panel_res = {
	.panel_width = 540,
	.panel_height = 960,
	.hfront_porch = 35,
	.hback_porch = 90,
	.hpulse_width = 8,
	.hsync_skew = 0,
	.vfront_porch = 14,
	.vback_porch = 16,
	.vpulse_width = 2,
	/* Borders not supported yet */
};

static struct color_info boehx8389c_qhd_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char boehx8389c_qhd_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb9, 0xff, 0x83, 0x89
};
static char boehx8389c_qhd_video_on_cmd_1[] = {
	0x11, 0x00, 0x39, 0xc0, 0xb1, 0x7f, 0x0a, 0x0a,
	0x31, 0x51, 0x50, 0xd0, 0xec, 0x9c, 0x80, 0x20,
	0x20, 0xf8, 0xaa, 0xaa, 0xa3, 0xff, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_2[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xb2, 0x80, 0x50, 0x05,
	0x03, 0x50, 0x38, 0x11, 0x64, 0x55, 0x09, 0xff
};
static char boehx8389c_qhd_video_on_cmd_3[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xb4, 0x77, 0x5e, 0x77,
	0x5e, 0x77, 0x5e, 0x0e, 0x72, 0x0e, 0x72, 0x8e
};
static char boehx8389c_qhd_video_on_cmd_4[] = {
	0x24, 0x00, 0x39, 0xc0, 0xd3, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x32, 0x10, 0x00, 0x00,
	0x00, 0x03, 0xca, 0x03, 0xca, 0x00, 0x00, 0x00,
	0x00, 0x27, 0x22, 0x01, 0x01, 0x23, 0x00, 0x00,
	0x00, 0x05, 0x08, 0x00, 0x00, 0x0a, 0x00, 0x01
};
static char boehx8389c_qhd_video_on_cmd_5[] = {
	0x27, 0x00, 0x39, 0xc0, 0xd5, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x01, 0x00, 0x03, 0x02, 0x19,
	0x19, 0x19, 0x19, 0x21, 0x20, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0xff
};
static char boehx8389c_qhd_video_on_cmd_6[] = {
	0x27, 0x00, 0x39, 0xc0, 0xd6, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x02, 0x03, 0x00, 0x01, 0x19,
	0x19, 0x18, 0x18, 0x20, 0x21, 0x19, 0x19, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0xff
};
static char boehx8389c_qhd_video_on_cmd_7[] = {
	0x2b, 0x00, 0x39, 0xc0, 0xe0, 0x05, 0x0a, 0x0b,
	0x2c, 0x35, 0x3f, 0x11, 0x34, 0x04, 0x08, 0x0b,
	0x17, 0x0e, 0x11, 0x14, 0x13, 0x14, 0x09, 0x11,
	0x14, 0x17, 0x05, 0x0a, 0x0b, 0x2c, 0x35, 0x3f,
	0x11, 0x34, 0x04, 0x08, 0x0b, 0x17, 0x0e, 0x11,
	0x14, 0x13, 0x14, 0x09, 0x11, 0x14, 0x17, 0xff
};
static char boehx8389c_qhd_video_on_cmd_8[] = {
	0x03, 0x00, 0x39, 0xc0, 0xb6, 0x44, 0x44, 0xff
};
static char boehx8389c_qhd_video_on_cmd_9[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0x00, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_10[] = {
	0x2c, 0x00, 0x39, 0xc0, 0xc1, 0x01, 0x00, 0x08,
	0x10, 0x18, 0x20, 0x2a, 0x31, 0x39, 0x41, 0x49,
	0x51, 0x59, 0x61, 0x68, 0x6f, 0x77, 0x7e, 0x85,
	0x8d, 0x94, 0x9c, 0xa4, 0xad, 0xb5, 0xbc, 0xc4,
	0xcd, 0xd7, 0xde, 0xe6, 0xee, 0xf7, 0xff, 0x15,
	0xb0, 0x45, 0x28, 0x73, 0x60, 0xb4, 0x47, 0xc0
};
static char boehx8389c_qhd_video_on_cmd_11[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0x01, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_12[] = {
	0x2b, 0x00, 0x39, 0xc0, 0xc1, 0x00, 0x08, 0x0f,
	0x17, 0x1f, 0x2a, 0x30, 0x37, 0x40, 0x47, 0x4f,
	0x57, 0x5f, 0x66, 0x6d, 0x74, 0x7b, 0x82, 0x89,
	0x8f, 0x97, 0x9e, 0xa6, 0xae, 0xb5, 0xbc, 0xc3,
	0xcb, 0xd4, 0xdc, 0xe3, 0xea, 0xf2, 0x0e, 0x57,
	0x7a, 0x15, 0x03, 0x60, 0xaf, 0x86, 0x40, 0xff
};
static char boehx8389c_qhd_video_on_cmd_13[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0x02, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_14[] = {
	0x2b, 0x00, 0x39, 0xc0, 0xc1, 0x00, 0x08, 0x10,
	0x18, 0x20, 0x2b, 0x31, 0x3a, 0x43, 0x4b, 0x53,
	0x5c, 0x63, 0x6b, 0x72, 0x7a, 0x81, 0x88, 0x90,
	0x97, 0x9f, 0xa7, 0xb0, 0xb7, 0xbf, 0xc7, 0xd0,
	0xd8, 0xe0, 0xe7, 0xee, 0xf7, 0xff, 0x16, 0xef,
	0x4c, 0xd8, 0x77, 0xf7, 0x13, 0x2e, 0xc0, 0xff
};
static char boehx8389c_qhd_video_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0x00, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd2, 0x77, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_17[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x02, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_18[] = {
	0x05, 0x00, 0x39, 0xc0, 0xc7, 0x00, 0x80, 0x60,
	0xc0, 0xff, 0xff, 0xff
};
static char boehx8389c_qhd_video_on_cmd_19[] = {
	0x11, 0x00, 0x05, 0x80
};
static char boehx8389c_qhd_video_on_cmd_20[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boehx8389c_qhd_video_on_command[] = {
	{ sizeof(boehx8389c_qhd_video_on_cmd_0), boehx8389c_qhd_video_on_cmd_0, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_1), boehx8389c_qhd_video_on_cmd_1, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_2), boehx8389c_qhd_video_on_cmd_2, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_3), boehx8389c_qhd_video_on_cmd_3, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_4), boehx8389c_qhd_video_on_cmd_4, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_5), boehx8389c_qhd_video_on_cmd_5, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_6), boehx8389c_qhd_video_on_cmd_6, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_7), boehx8389c_qhd_video_on_cmd_7, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_8), boehx8389c_qhd_video_on_cmd_8, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_9), boehx8389c_qhd_video_on_cmd_9, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_10), boehx8389c_qhd_video_on_cmd_10, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_11), boehx8389c_qhd_video_on_cmd_11, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_12), boehx8389c_qhd_video_on_cmd_12, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_13), boehx8389c_qhd_video_on_cmd_13, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_14), boehx8389c_qhd_video_on_cmd_14, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_15), boehx8389c_qhd_video_on_cmd_15, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_16), boehx8389c_qhd_video_on_cmd_16, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_17), boehx8389c_qhd_video_on_cmd_17, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_18), boehx8389c_qhd_video_on_cmd_18, 0 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_19), boehx8389c_qhd_video_on_cmd_19, 120 },
	{ sizeof(boehx8389c_qhd_video_on_cmd_20), boehx8389c_qhd_video_on_cmd_20, 20 },
};

static char boehx8389c_qhd_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char boehx8389c_qhd_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boehx8389c_qhd_video_off_command[] = {
	{ sizeof(boehx8389c_qhd_video_off_cmd_0), boehx8389c_qhd_video_off_cmd_0, 20 },
	{ sizeof(boehx8389c_qhd_video_off_cmd_1), boehx8389c_qhd_video_off_cmd_1, 120 },
};

static struct command_state boehx8389c_qhd_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info boehx8389c_qhd_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info boehx8389c_qhd_video_video_panel = {
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

static struct lane_configuration boehx8389c_qhd_video_lane_config = {
	.dsi_lanes = 2,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 0,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t boehx8389c_qhd_video_timings[] = {
	0x8b, 0x1e, 0x14, 0x00, 0x42, 0x46, 0x18, 0x23, 0x1b, 0x03, 0x04, 0x00
};

static struct panel_timing boehx8389c_qhd_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence boehx8389c_qhd_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 20, 25 },
	.pin_direction = 2,
};

static struct backlight boehx8389c_qhd_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_boehx8389c_qhd_video_select(struct panel_struct *panel,
						     struct msm_panel_info *pinfo,
						     struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &boehx8389c_qhd_video_panel_data;
	panel->panelres = &boehx8389c_qhd_video_panel_res;
	panel->color = &boehx8389c_qhd_video_color;
	panel->videopanel = &boehx8389c_qhd_video_video_panel;
	panel->commandpanel = &boehx8389c_qhd_video_command_panel;
	panel->state = &boehx8389c_qhd_video_state;
	panel->laneconfig = &boehx8389c_qhd_video_lane_config;
	panel->paneltiminginfo = &boehx8389c_qhd_video_timing_info;
	panel->panelresetseq = &boehx8389c_qhd_video_reset_seq;
	panel->backlightinfo = &boehx8389c_qhd_video_backlight;
	pinfo->mipi.panel_on_cmds = boehx8389c_qhd_video_on_command;
	pinfo->mipi.panel_off_cmds = boehx8389c_qhd_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(boehx8389c_qhd_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(boehx8389c_qhd_video_off_command);
	memcpy(phy_db->timing, boehx8389c_qhd_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_BOEHX8389C_QHD_VIDEO_H_ */
