// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_SMD_549_1080P_ALB_CMD_V0_H_
#define _PANEL_SMD_549_1080P_ALB_CMD_V0_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config smd_549_1080p_alb_cmd_v0_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_mot_smd_549_1080p_alb_cmd_v0",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 1,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution smd_549_1080p_alb_cmd_v0_panel_res = {
	.panel_width = 1080,
	.panel_height = 1920,
	.hfront_porch = 44,
	.hback_porch = 32,
	.hpulse_width = 12,
	.hsync_skew = 0,
	.vfront_porch = 9,
	.vback_porch = 3,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info smd_549_1080p_alb_cmd_v0_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char smd_549_1080p_alb_cmd_v0_on_cmd_0[] = {
	0x11, 0x00, 0x05, 0x80
};
static char smd_549_1080p_alb_cmd_v0_on_cmd_1[] = {
	0x35, 0x00, 0x15, 0x00
};
static char smd_549_1080p_alb_cmd_v0_on_cmd_2[] = {
	0x51, 0x00, 0x15, 0x00
};
static char smd_549_1080p_alb_cmd_v0_on_cmd_3[] = {
	0x55, 0x00, 0x15, 0x00
};
static char smd_549_1080p_alb_cmd_v0_on_cmd_4[] = {
	0x53, 0x20, 0x15, 0x00
};
static char smd_549_1080p_alb_cmd_v0_on_cmd_5[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd smd_549_1080p_alb_cmd_v0_on_command[] = {
	{ sizeof(smd_549_1080p_alb_cmd_v0_on_cmd_0), smd_549_1080p_alb_cmd_v0_on_cmd_0, 20 },
	{ sizeof(smd_549_1080p_alb_cmd_v0_on_cmd_1), smd_549_1080p_alb_cmd_v0_on_cmd_1, 0 },
	{ sizeof(smd_549_1080p_alb_cmd_v0_on_cmd_2), smd_549_1080p_alb_cmd_v0_on_cmd_2, 0 },
	{ sizeof(smd_549_1080p_alb_cmd_v0_on_cmd_3), smd_549_1080p_alb_cmd_v0_on_cmd_3, 0 },
	{ sizeof(smd_549_1080p_alb_cmd_v0_on_cmd_4), smd_549_1080p_alb_cmd_v0_on_cmd_4, 0 },
	{ sizeof(smd_549_1080p_alb_cmd_v0_on_cmd_5), smd_549_1080p_alb_cmd_v0_on_cmd_5, 0 },
};

static char smd_549_1080p_alb_cmd_v0_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char smd_549_1080p_alb_cmd_v0_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd smd_549_1080p_alb_cmd_v0_off_command[] = {
	{ sizeof(smd_549_1080p_alb_cmd_v0_off_cmd_0), smd_549_1080p_alb_cmd_v0_off_cmd_0, 35 },
	{ sizeof(smd_549_1080p_alb_cmd_v0_off_cmd_1), smd_549_1080p_alb_cmd_v0_off_cmd_1, 150 },
};

static struct command_state smd_549_1080p_alb_cmd_v0_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info smd_549_1080p_alb_cmd_v0_command_panel = {
	/* FIXME: This is a command mode panel */
};

static struct videopanel_info smd_549_1080p_alb_cmd_v0_video_panel = {
	.hsync_pulse = 0,
	.hfp_power_mode = 0,
	.hbp_power_mode = 0,
	.hsa_power_mode = 0,
	.bllp_eof_power_mode = 1,
	.bllp_power_mode = 1,
	.traffic_mode = 2,
	/* This is bllp_eof_power_mode and bllp_power_mode combined */
	.bllp_eof_power = 1 << 3 | 1 << 0,
};

static struct lane_configuration smd_549_1080p_alb_cmd_v0_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t smd_549_1080p_alb_cmd_v0_timings[] = {
	0xd5, 0x32, 0x22, 0x00, 0x60, 0x62, 0x26, 0x34, 0x29, 0x03, 0x04, 0x00
};

static struct panel_timing smd_549_1080p_alb_cmd_v0_timing_info = {
	.tclk_post = 0x0d,
	.tclk_pre = 0x2d,
};

static struct panel_reset_sequence smd_549_1080p_alb_cmd_v0_reset_seq = {
	.pin_state = { 0, 1 },
	.sleep = { 1, 6 },
	.pin_direction = 2,
};

static struct backlight smd_549_1080p_alb_cmd_v0_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_smd_549_1080p_alb_cmd_v0_select(struct panel_struct *panel,
							 struct msm_panel_info *pinfo,
							 struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &smd_549_1080p_alb_cmd_v0_panel_data;
	panel->panelres = &smd_549_1080p_alb_cmd_v0_panel_res;
	panel->color = &smd_549_1080p_alb_cmd_v0_color;
	panel->videopanel = &smd_549_1080p_alb_cmd_v0_video_panel;
	panel->commandpanel = &smd_549_1080p_alb_cmd_v0_command_panel;
	panel->state = &smd_549_1080p_alb_cmd_v0_state;
	panel->laneconfig = &smd_549_1080p_alb_cmd_v0_lane_config;
	panel->paneltiminginfo = &smd_549_1080p_alb_cmd_v0_timing_info;
	panel->panelresetseq = &smd_549_1080p_alb_cmd_v0_reset_seq;
	panel->backlightinfo = &smd_549_1080p_alb_cmd_v0_backlight;
	pinfo->mipi.panel_on_cmds = smd_549_1080p_alb_cmd_v0_on_command;
	pinfo->mipi.panel_off_cmds = smd_549_1080p_alb_cmd_v0_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(smd_549_1080p_alb_cmd_v0_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(smd_549_1080p_alb_cmd_v0_off_command);
	memcpy(phy_db->timing, smd_549_1080p_alb_cmd_v0_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_SMD_549_1080P_ALB_CMD_V0_H_ */
