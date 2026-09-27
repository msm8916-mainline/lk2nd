// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_XZ_NT51021_VIDEO_H_
#define _PANEL_XZ_NT51021_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config xz_nt51021_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_xz_nt51021_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution xz_nt51021_video_panel_res = {
	.panel_width = 1200,
	.panel_height = 1920,
	.hfront_porch = 80,
	.hback_porch = 60,
	.hpulse_width = 1,
	.hsync_skew = 0,
	.vfront_porch = 35,
	.vback_porch = 25,
	.vpulse_width = 1,
	/* Borders not supported yet */
};

static struct color_info xz_nt51021_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char xz_nt51021_video_on_cmd_0[] = {
	0x8c, 0x80, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_1[] = {
	0xc7, 0x50, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_2[] = {
	0xc5, 0x50, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_3[] = {
	0x85, 0x04, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_4[] = {
	0x86, 0x08, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_5[] = {
	0xa9, 0x2a, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_6[] = {
	0x83, 0xaa, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_7[] = {
	0x84, 0x11, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_8[] = {
	0x9c, 0x10, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_9[] = {
	0xa9, 0x4b, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_10[] = {
	0x83, 0x00, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_11[] = {
	0x84, 0x00, 0x15, 0x80
};
static char xz_nt51021_video_on_cmd_12[] = {
	0x11, 0x00, 0x05, 0x80
};
static char xz_nt51021_video_on_cmd_13[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd xz_nt51021_video_on_command[] = {
	{ sizeof(xz_nt51021_video_on_cmd_0), xz_nt51021_video_on_cmd_0, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_1), xz_nt51021_video_on_cmd_1, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_2), xz_nt51021_video_on_cmd_2, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_3), xz_nt51021_video_on_cmd_3, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_4), xz_nt51021_video_on_cmd_4, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_5), xz_nt51021_video_on_cmd_5, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_6), xz_nt51021_video_on_cmd_6, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_7), xz_nt51021_video_on_cmd_7, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_8), xz_nt51021_video_on_cmd_8, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_9), xz_nt51021_video_on_cmd_9, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_10), xz_nt51021_video_on_cmd_10, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_11), xz_nt51021_video_on_cmd_11, 0 },
	{ sizeof(xz_nt51021_video_on_cmd_12), xz_nt51021_video_on_cmd_12, 50 },
	{ sizeof(xz_nt51021_video_on_cmd_13), xz_nt51021_video_on_cmd_13, 50 },
};

static char xz_nt51021_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char xz_nt51021_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd xz_nt51021_video_off_command[] = {
	{ sizeof(xz_nt51021_video_off_cmd_0), xz_nt51021_video_off_cmd_0, 50 },
	{ sizeof(xz_nt51021_video_off_cmd_1), xz_nt51021_video_off_cmd_1, 200 },
};

static struct command_state xz_nt51021_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info xz_nt51021_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info xz_nt51021_video_video_panel = {
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

static struct lane_configuration xz_nt51021_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 1,
};

static const uint32_t xz_nt51021_video_timings[] = {
	0x46, 0x3c, 0x28, 0x01, 0x6e, 0x70, 0x2e, 0x40, 0x63, 0x03, 0x04, 0x00
};

static struct panel_timing xz_nt51021_video_timing_info = {
	.tclk_post = 0x02,
	.tclk_pre = 0x60,
};

static struct panel_reset_sequence xz_nt51021_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 10, 5, 5 },
	.pin_direction = 2,
};

static struct backlight xz_nt51021_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_xz_nt51021_video_select(struct panel_struct *panel,
						 struct msm_panel_info *pinfo,
						 struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &xz_nt51021_video_panel_data;
	panel->panelres = &xz_nt51021_video_panel_res;
	panel->color = &xz_nt51021_video_color;
	panel->videopanel = &xz_nt51021_video_video_panel;
	panel->commandpanel = &xz_nt51021_video_command_panel;
	panel->state = &xz_nt51021_video_state;
	panel->laneconfig = &xz_nt51021_video_lane_config;
	panel->paneltiminginfo = &xz_nt51021_video_timing_info;
	panel->panelresetseq = &xz_nt51021_video_reset_seq;
	panel->backlightinfo = &xz_nt51021_video_backlight;
	pinfo->mipi.panel_on_cmds = xz_nt51021_video_on_command;
	pinfo->mipi.panel_off_cmds = xz_nt51021_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(xz_nt51021_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(xz_nt51021_video_off_command);
	memcpy(phy_db->timing, xz_nt51021_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_XZ_NT51021_VIDEO_H_ */
