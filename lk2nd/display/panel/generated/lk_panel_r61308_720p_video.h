// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_R61308_720P_VIDEO_H_
#define _PANEL_R61308_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config r61308_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_r61308_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 50000,
};

static struct panel_resolution r61308_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 120,
	.hback_porch = 50,
	.hpulse_width = 2,
	.hsync_skew = 0,
	.vfront_porch = 6,
	.vback_porch = 14,
	.vpulse_width = 1,
	/* Borders not supported yet */
};

static struct color_info r61308_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char r61308_720p_video_on_cmd_0[] = {
	0x11, 0x00, 0x05, 0x80
};
static char r61308_720p_video_on_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0x36, 0x00, 0xff, 0xff
};
static char r61308_720p_video_on_cmd_2[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3a, 0x07, 0xff, 0xff
};
static char r61308_720p_video_on_cmd_3[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb0, 0x04, 0xff, 0xff
};
static char r61308_720p_video_on_cmd_4[] = {
	0x08, 0x00, 0x29, 0xc0, 0xc1, 0x50, 0x02, 0x22,
	0x00, 0x00, 0xed, 0x11
};
static char r61308_720p_video_on_cmd_5[] = {
	0x19, 0x00, 0x29, 0xc0, 0xc8, 0x1a, 0x24, 0x29,
	0x2d, 0x32, 0x37, 0x14, 0x13, 0x10, 0x0c, 0x0a,
	0x06, 0x1a, 0x24, 0x28, 0x2d, 0x32, 0x37, 0x14,
	0x13, 0x10, 0x0c, 0x0a, 0x06, 0xff, 0xff, 0xff
};
static char r61308_720p_video_on_cmd_6[] = {
	0x09, 0x00, 0x29, 0xc0, 0xcb, 0x10, 0x20, 0x40,
	0x80, 0xa0, 0xc0, 0xd0, 0xe0, 0xff, 0xff, 0xff
};
static char r61308_720p_video_on_cmd_7[] = {
	0x04, 0x00, 0x29, 0xc0, 0xcc, 0xc8, 0xd8, 0xff
};
static char r61308_720p_video_on_cmd_8[] = {
	0x08, 0x00, 0x29, 0xc0, 0xcd, 0x1c, 0x1e, 0x1e,
	0x1d, 0x1c, 0x1e, 0x1e
};
static char r61308_720p_video_on_cmd_9[] = {
	0x08, 0x00, 0x29, 0xc0, 0xce, 0x1e, 0x1e, 0x1e,
	0x1d, 0x1d, 0x1e, 0x1e
};
static char r61308_720p_video_on_cmd_10[] = {
	0x08, 0x00, 0x29, 0xc0, 0xcf, 0x1e, 0x1f, 0x20,
	0x20, 0x20, 0x20, 0x21
};
static char r61308_720p_video_on_cmd_11[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb0, 0x03, 0xff, 0xff
};
static char r61308_720p_video_on_cmd_12[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd r61308_720p_video_on_command[] = {
	{ sizeof(r61308_720p_video_on_cmd_0), r61308_720p_video_on_cmd_0, 120 },
	{ sizeof(r61308_720p_video_on_cmd_1), r61308_720p_video_on_cmd_1, 0 },
	{ sizeof(r61308_720p_video_on_cmd_2), r61308_720p_video_on_cmd_2, 0 },
	{ sizeof(r61308_720p_video_on_cmd_3), r61308_720p_video_on_cmd_3, 0 },
	{ sizeof(r61308_720p_video_on_cmd_4), r61308_720p_video_on_cmd_4, 0 },
	{ sizeof(r61308_720p_video_on_cmd_5), r61308_720p_video_on_cmd_5, 0 },
	{ sizeof(r61308_720p_video_on_cmd_6), r61308_720p_video_on_cmd_6, 0 },
	{ sizeof(r61308_720p_video_on_cmd_7), r61308_720p_video_on_cmd_7, 0 },
	{ sizeof(r61308_720p_video_on_cmd_8), r61308_720p_video_on_cmd_8, 0 },
	{ sizeof(r61308_720p_video_on_cmd_9), r61308_720p_video_on_cmd_9, 0 },
	{ sizeof(r61308_720p_video_on_cmd_10), r61308_720p_video_on_cmd_10, 0 },
	{ sizeof(r61308_720p_video_on_cmd_11), r61308_720p_video_on_cmd_11, 0 },
	{ sizeof(r61308_720p_video_on_cmd_12), r61308_720p_video_on_cmd_12, 10 },
};

static char r61308_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char r61308_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd r61308_720p_video_off_command[] = {
	{ sizeof(r61308_720p_video_off_cmd_0), r61308_720p_video_off_cmd_0, 50 },
	{ sizeof(r61308_720p_video_off_cmd_1), r61308_720p_video_off_cmd_1, 120 },
};

static struct command_state r61308_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info r61308_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info r61308_720p_video_video_panel = {
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

static struct lane_configuration r61308_720p_video_lane_config = {
	.dsi_lanes = 3,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t r61308_720p_video_timings[] = {
	0x9b, 0x22, 0x18, 0x00, 0x4a, 0x4c, 0x1c, 0x26, 0x1d, 0x03, 0x04, 0x00
};

static struct panel_timing r61308_720p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1f,
};

static struct panel_reset_sequence r61308_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 5, 10, 30 },
	.pin_direction = 2,
};

static struct backlight r61308_720p_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_r61308_720p_video_select(struct panel_struct *panel,
						  struct msm_panel_info *pinfo,
						  struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &r61308_720p_video_panel_data;
	panel->panelres = &r61308_720p_video_panel_res;
	panel->color = &r61308_720p_video_color;
	panel->videopanel = &r61308_720p_video_video_panel;
	panel->commandpanel = &r61308_720p_video_command_panel;
	panel->state = &r61308_720p_video_state;
	panel->laneconfig = &r61308_720p_video_lane_config;
	panel->paneltiminginfo = &r61308_720p_video_timing_info;
	panel->panelresetseq = &r61308_720p_video_reset_seq;
	panel->backlightinfo = &r61308_720p_video_backlight;
	pinfo->mipi.panel_on_cmds = r61308_720p_video_on_command;
	pinfo->mipi.panel_off_cmds = r61308_720p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(r61308_720p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(r61308_720p_video_off_command);
	memcpy(phy_db->timing, r61308_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_R61308_720P_VIDEO_H_ */
