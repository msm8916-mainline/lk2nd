// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_BOENT35523_800P_VIDEO_H_
#define _PANEL_BOENT35523_800P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config boent35523_800p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_boent35523_800p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution boent35523_800p_video_panel_res = {
	.panel_width = 800,
	.panel_height = 1280,
	.hfront_porch = 50,
	.hback_porch = 48,
	.hpulse_width = 4,
	.hsync_skew = 0,
	.vfront_porch = 8,
	.vback_porch = 16,
	.vpulse_width = 2,
	/* Borders not supported yet */
};

static struct color_info boent35523_800p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char boent35523_800p_video_on_cmd_0[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x01, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0xce, 0x04, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_2[] = {
	0x03, 0x00, 0x39, 0xc0, 0xbc, 0x68, 0x01, 0xff
};
static char boent35523_800p_video_on_cmd_3[] = {
	0x03, 0x00, 0x39, 0xc0, 0xbd, 0x68, 0x01, 0xff
};
static char boent35523_800p_video_on_cmd_4[] = {
	0x05, 0x00, 0x39, 0xc0, 0xff, 0xaa, 0x55, 0xa5,
	0x80, 0xff, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_5[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x09, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf7, 0x82, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x0b, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf7, 0xe0, 0xff, 0xff
};
static char boent35523_800p_video_on_cmd_9[] = {
	0x11, 0x00, 0x05, 0x80
};
static char boent35523_800p_video_on_cmd_10[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boent35523_800p_video_on_command[] = {
	{ sizeof(boent35523_800p_video_on_cmd_0), boent35523_800p_video_on_cmd_0, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_1), boent35523_800p_video_on_cmd_1, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_2), boent35523_800p_video_on_cmd_2, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_3), boent35523_800p_video_on_cmd_3, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_4), boent35523_800p_video_on_cmd_4, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_5), boent35523_800p_video_on_cmd_5, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_6), boent35523_800p_video_on_cmd_6, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_7), boent35523_800p_video_on_cmd_7, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_8), boent35523_800p_video_on_cmd_8, 0 },
	{ sizeof(boent35523_800p_video_on_cmd_9), boent35523_800p_video_on_cmd_9, 120 },
	{ sizeof(boent35523_800p_video_on_cmd_10), boent35523_800p_video_on_cmd_10, 10 },
};

static char boent35523_800p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char boent35523_800p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boent35523_800p_video_off_command[] = {
	{ sizeof(boent35523_800p_video_off_cmd_0), boent35523_800p_video_off_cmd_0, 20 },
	{ sizeof(boent35523_800p_video_off_cmd_1), boent35523_800p_video_off_cmd_1, 120 },
};

static struct command_state boent35523_800p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info boent35523_800p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info boent35523_800p_video_video_panel = {
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

static struct lane_configuration boent35523_800p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t boent35523_800p_video_timings[] = {
	0x7a, 0x1a, 0x12, 0x00, 0x3e, 0x42, 0x16, 0x1e, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing boent35523_800p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence boent35523_800p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 10, 120 },
	.pin_direction = 2,
};

static struct backlight boent35523_800p_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 4095,
};

static inline void panel_boent35523_800p_video_select(struct panel_struct *panel,
						      struct msm_panel_info *pinfo,
						      struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &boent35523_800p_video_panel_data;
	panel->panelres = &boent35523_800p_video_panel_res;
	panel->color = &boent35523_800p_video_color;
	panel->videopanel = &boent35523_800p_video_video_panel;
	panel->commandpanel = &boent35523_800p_video_command_panel;
	panel->state = &boent35523_800p_video_state;
	panel->laneconfig = &boent35523_800p_video_lane_config;
	panel->paneltiminginfo = &boent35523_800p_video_timing_info;
	panel->panelresetseq = &boent35523_800p_video_reset_seq;
	panel->backlightinfo = &boent35523_800p_video_backlight;
	pinfo->mipi.panel_on_cmds = boent35523_800p_video_on_command;
	pinfo->mipi.panel_off_cmds = boent35523_800p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(boent35523_800p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(boent35523_800p_video_off_command);
	memcpy(phy_db->timing, boent35523_800p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_BOENT35523_800P_VIDEO_H_ */
