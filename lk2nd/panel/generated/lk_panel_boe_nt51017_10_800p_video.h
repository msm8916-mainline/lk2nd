// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_BOE_NT51017_10_800P_VIDEO_H_
#define _PANEL_BOE_NT51017_10_800P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config boe_nt51017_10_800p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_boe_nt51017_10_800p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution boe_nt51017_10_800p_video_panel_res = {
	.panel_width = 800,
	.panel_height = 1280,
	.hfront_porch = 164,
	.hback_porch = 136,
	.hpulse_width = 8,
	.hsync_skew = 0,
	.vfront_porch = 56,
	.vback_porch = 42,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info boe_nt51017_10_800p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char boe_nt51017_10_800p_video_on_cmd_0[] = {
	0x83, 0x96, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_1[] = {
	0x84, 0x69, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_2[] = {
	0x95, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_3[] = {
	0x91, 0xc0, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_4[] = {
	0x92, 0x57, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_5[] = {
	0x93, 0x20, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_6[] = {
	0xa9, 0xff, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_7[] = {
	0xaa, 0xfa, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_8[] = {
	0xab, 0xf3, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_9[] = {
	0xac, 0xed, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_10[] = {
	0xad, 0xe7, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_11[] = {
	0xae, 0xe2, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_12[] = {
	0xaf, 0xdc, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_13[] = {
	0xb0, 0xd7, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_14[] = {
	0xb1, 0xd1, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_15[] = {
	0xb2, 0xcc, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_16[] = {
	0x99, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_17[] = {
	0x83, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_18[] = {
	0x84, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_19[] = {
	0xf5, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_20[] = {
	0x96, 0x40, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_21[] = {
	0x83, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_22[] = {
	0x84, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_23[] = {
	0x96, 0x00, 0x15, 0x80
};
static char boe_nt51017_10_800p_video_on_cmd_24[] = {
	0xf5, 0xfa, 0x15, 0x80
};

static struct mipi_dsi_cmd boe_nt51017_10_800p_video_on_command[] = {
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_0), boe_nt51017_10_800p_video_on_cmd_0, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_1), boe_nt51017_10_800p_video_on_cmd_1, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_2), boe_nt51017_10_800p_video_on_cmd_2, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_3), boe_nt51017_10_800p_video_on_cmd_3, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_4), boe_nt51017_10_800p_video_on_cmd_4, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_5), boe_nt51017_10_800p_video_on_cmd_5, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_6), boe_nt51017_10_800p_video_on_cmd_6, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_7), boe_nt51017_10_800p_video_on_cmd_7, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_8), boe_nt51017_10_800p_video_on_cmd_8, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_9), boe_nt51017_10_800p_video_on_cmd_9, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_10), boe_nt51017_10_800p_video_on_cmd_10, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_11), boe_nt51017_10_800p_video_on_cmd_11, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_12), boe_nt51017_10_800p_video_on_cmd_12, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_13), boe_nt51017_10_800p_video_on_cmd_13, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_14), boe_nt51017_10_800p_video_on_cmd_14, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_15), boe_nt51017_10_800p_video_on_cmd_15, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_16), boe_nt51017_10_800p_video_on_cmd_16, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_17), boe_nt51017_10_800p_video_on_cmd_17, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_18), boe_nt51017_10_800p_video_on_cmd_18, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_19), boe_nt51017_10_800p_video_on_cmd_19, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_20), boe_nt51017_10_800p_video_on_cmd_20, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_21), boe_nt51017_10_800p_video_on_cmd_21, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_22), boe_nt51017_10_800p_video_on_cmd_22, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_23), boe_nt51017_10_800p_video_on_cmd_23, 0 },
	{ sizeof(boe_nt51017_10_800p_video_on_cmd_24), boe_nt51017_10_800p_video_on_cmd_24, 0 },
};


static struct mipi_dsi_cmd boe_nt51017_10_800p_video_off_command[] = {
};

static struct command_state boe_nt51017_10_800p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info boe_nt51017_10_800p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info boe_nt51017_10_800p_video_video_panel = {
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

static struct lane_configuration boe_nt51017_10_800p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 1,
};

static const uint32_t boe_nt51017_10_800p_video_timings[] = {
	0x98, 0x22, 0x16, 0x00, 0x4a, 0x4c, 0x1c, 0x26, 0x1a, 0x03, 0x04, 0x00
};

static struct panel_timing boe_nt51017_10_800p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence boe_nt51017_10_800p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 1, 20, 120 },
	.pin_direction = 2,
};

static struct backlight boe_nt51017_10_800p_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_boe_nt51017_10_800p_video_select(struct panel_struct *panel,
							  struct msm_panel_info *pinfo,
							  struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &boe_nt51017_10_800p_video_panel_data;
	panel->panelres = &boe_nt51017_10_800p_video_panel_res;
	panel->color = &boe_nt51017_10_800p_video_color;
	panel->videopanel = &boe_nt51017_10_800p_video_video_panel;
	panel->commandpanel = &boe_nt51017_10_800p_video_command_panel;
	panel->state = &boe_nt51017_10_800p_video_state;
	panel->laneconfig = &boe_nt51017_10_800p_video_lane_config;
	panel->paneltiminginfo = &boe_nt51017_10_800p_video_timing_info;
	panel->panelresetseq = &boe_nt51017_10_800p_video_reset_seq;
	panel->backlightinfo = &boe_nt51017_10_800p_video_backlight;
	pinfo->mipi.panel_cmds = boe_nt51017_10_800p_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(boe_nt51017_10_800p_video_on_command);
	memcpy(phy_db->timing, boe_nt51017_10_800p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_BOE_NT51017_10_800P_VIDEO_H_ */
