// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2016, The Linux Foundation. All rights reserved.

#ifndef _PANEL_R69431_720P_VIDEO_H_
#define _PANEL_R69431_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config r69431_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_r69431_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 200,
};

static struct panel_resolution r69431_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 120,
	.hback_porch = 20,
	.hpulse_width = 10,
	.hsync_skew = 0,
	.vfront_porch = 13,
	.vback_porch = 11,
	.vpulse_width = 1,
	/* Borders not supported yet */
};

static struct color_info r69431_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char r69431_720p_video_on_cmd_0[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb0, 0x00, 0xff, 0xff
};
static char r69431_720p_video_on_cmd_1[] = {
	0x02, 0x00, 0x29, 0xc0, 0xd6, 0x01, 0xff, 0xff
};
static char r69431_720p_video_on_cmd_2[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb3, 0x1c, 0xff, 0xff
};
static char r69431_720p_video_on_cmd_3[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb0, 0x03, 0xff, 0xff
};
static char r69431_720p_video_on_cmd_4[] = {
	0x29, 0x00, 0x05, 0x80
};
static char r69431_720p_video_on_cmd_5[] = {
	0x11, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd r69431_720p_video_on_command[] = {
	{ sizeof(r69431_720p_video_on_cmd_0), r69431_720p_video_on_cmd_0, 0 },
	{ sizeof(r69431_720p_video_on_cmd_1), r69431_720p_video_on_cmd_1, 0 },
	{ sizeof(r69431_720p_video_on_cmd_2), r69431_720p_video_on_cmd_2, 0 },
	{ sizeof(r69431_720p_video_on_cmd_3), r69431_720p_video_on_cmd_3, 0 },
	{ sizeof(r69431_720p_video_on_cmd_4), r69431_720p_video_on_cmd_4, 0 },
	{ sizeof(r69431_720p_video_on_cmd_5), r69431_720p_video_on_cmd_5, 120 },
};

static char r69431_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char r69431_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd r69431_720p_video_off_command[] = {
	{ sizeof(r69431_720p_video_off_cmd_0), r69431_720p_video_off_cmd_0, 20 },
	{ sizeof(r69431_720p_video_off_cmd_1), r69431_720p_video_off_cmd_1, 220 },
};

static struct command_state r69431_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info r69431_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info r69431_720p_video_video_panel = {
	.hsync_pulse = 1,
	.hfp_power_mode = 0,
	.hbp_power_mode = 0,
	.hsa_power_mode = 0,
	.bllp_eof_power_mode = 1,
	.bllp_power_mode = 1,
	.traffic_mode = 0,
	/* This is bllp_eof_power_mode and bllp_power_mode combined */
	.bllp_eof_power = 1 << 3 | 1 << 0,
};

static struct lane_configuration r69431_720p_video_lane_config = {
	.dsi_lanes = 3,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t r69431_720p_video_timings[] = {
	0x98, 0x22, 0x16, 0x00, 0x4a, 0x4c, 0x1a, 0x26, 0x1a, 0x03, 0x04, 0x00
};

static struct panel_timing r69431_720p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1e,
};

static struct panel_reset_sequence r69431_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 5, 5, 10 },
	.pin_direction = 2,
};

static struct backlight r69431_720p_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_r69431_720p_video_select(struct panel_struct *panel,
						  struct msm_panel_info *pinfo,
						  struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &r69431_720p_video_panel_data;
	panel->panelres = &r69431_720p_video_panel_res;
	panel->color = &r69431_720p_video_color;
	panel->videopanel = &r69431_720p_video_video_panel;
	panel->commandpanel = &r69431_720p_video_command_panel;
	panel->state = &r69431_720p_video_state;
	panel->laneconfig = &r69431_720p_video_lane_config;
	panel->paneltiminginfo = &r69431_720p_video_timing_info;
	panel->panelresetseq = &r69431_720p_video_reset_seq;
	panel->backlightinfo = &r69431_720p_video_backlight;
	pinfo->mipi.panel_on_cmds = r69431_720p_video_on_command;
	pinfo->mipi.panel_off_cmds = r69431_720p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(r69431_720p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(r69431_720p_video_off_command);
	memcpy(phy_db->timing, r69431_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_R69431_720P_VIDEO_H_ */
