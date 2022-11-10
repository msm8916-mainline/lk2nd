// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_HX8394D_720P_VIDEO_H_
#define _PANEL_HX8394D_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config hx8394d_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_hx8394d_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution hx8394d_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 50,
	.hback_porch = 50,
	.hpulse_width = 50,
	.hsync_skew = 0,
	.vfront_porch = 9,
	.vback_porch = 16,
	.vpulse_width = 2,
	/* Borders not supported yet */
};

static struct color_info hx8394d_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char hx8394d_720p_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb9, 0xff, 0x83, 0x94
};
static char hx8394d_720p_video_on_cmd_1[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xba, 0x73, 0x43, 0xa0,
	0x65, 0xb2, 0x89, 0x09, 0x40, 0x50, 0x00, 0x00
};
static char hx8394d_720p_video_on_cmd_2[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb0, 0x00, 0x00, 0x7d,
	0x0c, 0xff, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_3[] = {
	0x10, 0x00, 0x39, 0xc0, 0xb1, 0x6c, 0x15, 0x15,
	0x24, 0x04, 0x11, 0xf1, 0x80, 0xe4, 0x97, 0x23,
	0x80, 0xc0, 0xd2, 0x58
};
static char hx8394d_720p_video_on_cmd_4[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xb2, 0x00, 0x64, 0x10,
	0x07, 0x22, 0x1c, 0x08, 0x08, 0x1c, 0x4d, 0x00
};
static char hx8394d_720p_video_on_cmd_5[] = {
	0x0d, 0x00, 0x39, 0xc0, 0xb4, 0x00, 0xff, 0x03,
	0x5a, 0x03, 0x5a, 0x03, 0x5a, 0x01, 0x6a, 0x30,
	0x6a, 0xff, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbc, 0x07, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_7[] = {
	0x04, 0x00, 0x39, 0xc0, 0xbf, 0x41, 0x0e, 0x01
};
static char hx8394d_720p_video_on_cmd_8[] = {
	0x1f, 0x00, 0x39, 0xc0, 0xd3, 0x00, 0x06, 0x00,
	0x40, 0x07, 0x08, 0x00, 0x32, 0x10, 0x07, 0x00,
	0x07, 0x54, 0x15, 0x0f, 0x05, 0x04, 0x02, 0x12,
	0x10, 0x05, 0x07, 0x33, 0x33, 0x0b, 0x0b, 0x37,
	0x10, 0x07, 0x07, 0xff
};
static char hx8394d_720p_video_on_cmd_9[] = {
	0x2d, 0x00, 0x39, 0xc0, 0xd5, 0x04, 0x05, 0x06,
	0x07, 0x00, 0x01, 0x02, 0x03, 0x20, 0x21, 0x22,
	0x23, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x19, 0x19, 0x18, 0x18, 0x18, 0x18, 0x1b,
	0x1b, 0x1a, 0x1a, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0xff, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_10[] = {
	0x2d, 0x00, 0x39, 0xc0, 0xd6, 0x03, 0x02, 0x01,
	0x00, 0x07, 0x06, 0x05, 0x04, 0x23, 0x22, 0x21,
	0x20, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x58,
	0x58, 0x18, 0x18, 0x19, 0x19, 0x18, 0x18, 0x1b,
	0x1b, 0x1a, 0x1a, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x18, 0xff, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_11[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x01, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_12[] = {
	0x03, 0x00, 0x39, 0xc0, 0xb6, 0x4c, 0x4c, 0xff
};
static char hx8394d_720p_video_on_cmd_13[] = {
	0x2b, 0x00, 0x39, 0xc0, 0xe0, 0x00, 0x10, 0x16,
	0x2d, 0x33, 0x3f, 0x23, 0x3e, 0x07, 0x0b, 0x0d,
	0x17, 0x0e, 0x12, 0x14, 0x12, 0x13, 0x06, 0x11,
	0x13, 0x18, 0x00, 0x0f, 0x16, 0x2e, 0x33, 0x3f,
	0x23, 0x3d, 0x07, 0x0b, 0x0d, 0x18, 0x0f, 0x12,
	0x14, 0x12, 0x14, 0x07, 0x11, 0x12, 0x17, 0xff
};
static char hx8394d_720p_video_on_cmd_14[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc0, 0x30, 0x14, 0xff
};
static char hx8394d_720p_video_on_cmd_15[] = {
	0x05, 0x00, 0x39, 0xc0, 0xc7, 0x00, 0xc0, 0x40,
	0xc0, 0xff, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdf, 0x8e, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_17[] = {
	0x02, 0x00, 0x39, 0xc0, 0x36, 0x02, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_18[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd2, 0x66, 0xff, 0xff
};
static char hx8394d_720p_video_on_cmd_19[] = {
	0x11, 0x00, 0x05, 0x80
};
static char hx8394d_720p_video_on_cmd_20[] = {
	0x04, 0x00, 0x39, 0xc0, 0xc9, 0x1f, 0x00, 0x14
};
static char hx8394d_720p_video_on_cmd_21[] = {
	0x29, 0x00, 0x05, 0x80
};
static char hx8394d_720p_video_on_cmd_22[] = {
	0x53, 0x24, 0x15, 0x80
};
static char hx8394d_720p_video_on_cmd_23[] = {
	0x55, 0x01, 0x15, 0x80
};
static char hx8394d_720p_video_on_cmd_24[] = {
	0x51, 0xe1, 0x15, 0x80	/* Set brightness to maximum 225 */
};

static struct mipi_dsi_cmd hx8394d_720p_video_on_command[] = {
	{ sizeof(hx8394d_720p_video_on_cmd_0), hx8394d_720p_video_on_cmd_0, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_1), hx8394d_720p_video_on_cmd_1, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_2), hx8394d_720p_video_on_cmd_2, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_3), hx8394d_720p_video_on_cmd_3, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_4), hx8394d_720p_video_on_cmd_4, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_5), hx8394d_720p_video_on_cmd_5, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_6), hx8394d_720p_video_on_cmd_6, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_7), hx8394d_720p_video_on_cmd_7, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_8), hx8394d_720p_video_on_cmd_8, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_9), hx8394d_720p_video_on_cmd_9, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_10), hx8394d_720p_video_on_cmd_10, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_11), hx8394d_720p_video_on_cmd_11, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_12), hx8394d_720p_video_on_cmd_12, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_13), hx8394d_720p_video_on_cmd_13, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_14), hx8394d_720p_video_on_cmd_14, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_15), hx8394d_720p_video_on_cmd_15, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_16), hx8394d_720p_video_on_cmd_16, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_17), hx8394d_720p_video_on_cmd_17, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_18), hx8394d_720p_video_on_cmd_18, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_19), hx8394d_720p_video_on_cmd_19, 200 },
	{ sizeof(hx8394d_720p_video_on_cmd_20), hx8394d_720p_video_on_cmd_20, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_21), hx8394d_720p_video_on_cmd_21, 10 },
	{ sizeof(hx8394d_720p_video_on_cmd_22), hx8394d_720p_video_on_cmd_22, 0 },
	{ sizeof(hx8394d_720p_video_on_cmd_23), hx8394d_720p_video_on_cmd_23, 5 },
	{ sizeof(hx8394d_720p_video_on_cmd_24), hx8394d_720p_video_on_cmd_24, 0 },
};

static char hx8394d_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char hx8394d_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8394d_720p_video_off_command[] = {
	{ sizeof(hx8394d_720p_video_off_cmd_0), hx8394d_720p_video_off_cmd_0, 0 },
	{ sizeof(hx8394d_720p_video_off_cmd_1), hx8394d_720p_video_off_cmd_1, 0 },
};

static struct command_state hx8394d_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info hx8394d_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info hx8394d_720p_video_video_panel = {
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

static struct lane_configuration hx8394d_720p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t hx8394d_720p_video_timings[] = {
	0x7b, 0x1a, 0x10, 0x00, 0x3c, 0x91, 0x1c, 0x1c, 0x15, 0x03, 0x04, 0x00
};

static struct panel_timing hx8394d_720p_video_timing_info = {
	.tclk_post = 0x1f,
	.tclk_pre = 0x2d,
};

static struct panel_reset_sequence hx8394d_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 1, 20 },
	.pin_direction = 2,
};

static struct backlight hx8394d_720p_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 225,
};

static inline void panel_hx8394d_720p_video_select(struct panel_struct *panel,
						   struct msm_panel_info *pinfo,
						   struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &hx8394d_720p_video_panel_data;
	panel->panelres = &hx8394d_720p_video_panel_res;
	panel->color = &hx8394d_720p_video_color;
	panel->videopanel = &hx8394d_720p_video_video_panel;
	panel->commandpanel = &hx8394d_720p_video_command_panel;
	panel->state = &hx8394d_720p_video_state;
	panel->laneconfig = &hx8394d_720p_video_lane_config;
	panel->paneltiminginfo = &hx8394d_720p_video_timing_info;
	panel->panelresetseq = &hx8394d_720p_video_reset_seq;
	panel->backlightinfo = &hx8394d_720p_video_backlight;
	pinfo->mipi.panel_cmds = hx8394d_720p_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(hx8394d_720p_video_on_command);
	memcpy(phy_db->timing, hx8394d_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_HX8394D_720P_VIDEO_H_ */
