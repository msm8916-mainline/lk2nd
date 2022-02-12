// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved.

#ifndef _PANEL_BOOYI_OTM1287_720P_VIDEO_H_
#define _PANEL_BOOYI_OTM1287_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config booyi_otm1287_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_booyi_OTM1287_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution booyi_otm1287_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 90,
	.hback_porch = 90,
	.hpulse_width = 10,
	.hsync_skew = 0,
	.vfront_porch = 20,
	.vback_porch = 16,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info booyi_otm1287_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char booyi_otm1287_720p_video_on_cmd_0[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_1[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x12, 0x87, 0x01
};
static char booyi_otm1287_720p_video_on_cmd_2[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_3[] = {
	0x03, 0x00, 0x39, 0xc0, 0xff, 0x12, 0x87, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_4[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x92, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_5[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x30, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_7[] = {
	0x0a, 0x00, 0x39, 0xc0, 0xc0, 0x00, 0x64, 0x00,
	0x0f, 0x11, 0x00, 0x64, 0x0f, 0x11, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_9[] = {
	0x07, 0x00, 0x39, 0xc0, 0xc0, 0x00, 0x5c, 0x00,
	0x01, 0x00, 0x04, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_10[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa4, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_11[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb3, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_13[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc0, 0x00, 0x55, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_14[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x81, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x55, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_17[] = {
	0x05, 0x00, 0x39, 0xc0, 0xf5, 0x02, 0x11, 0x02,
	0x15, 0xff, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_18[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_19[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x50, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_20[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x94, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_21[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x66, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_22[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb2, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_23[] = {
	0x03, 0x00, 0x39, 0xc0, 0xf5, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_24[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb6, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_25[] = {
	0x03, 0x00, 0x39, 0xc0, 0xf5, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_26[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x94, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_27[] = {
	0x03, 0x00, 0x39, 0xc0, 0xf5, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_28[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd2, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_29[] = {
	0x03, 0x00, 0x39, 0xc0, 0xf5, 0x06, 0x15, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_30[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb4, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_31[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0xcc, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_32[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_33[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xc4, 0x05, 0x10, 0x06,
	0x02, 0x05, 0x15, 0x10, 0x05, 0x10, 0x07, 0x02,
	0x05, 0x15, 0x10, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_34[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_35[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc4, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_36[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x91, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_37[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc5, 0x19, 0x52, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_38[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_39[] = {
	0x03, 0x00, 0x39, 0xc0, 0xd8, 0xbc, 0xbc, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_40[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb3, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_41[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x84, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_42[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xbb, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_43[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x8a, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_44[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x82, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_45[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x0a, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_46[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_47[] = {
	0x15, 0x00, 0x39, 0xc0, 0xe1, 0x05, 0x44, 0x54,
	0x61, 0x72, 0x7f, 0x81, 0xa9, 0x98, 0xb0, 0x55,
	0x41, 0x56, 0x38, 0x3a, 0x2e, 0x23, 0x19, 0x0c,
	0x05, 0xff, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_48[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_49[] = {
	0x15, 0x00, 0x39, 0xc0, 0xe2, 0x05, 0x44, 0x54,
	0x61, 0x72, 0x80, 0x80, 0xa9, 0x99, 0xb0, 0x54,
	0x41, 0x56, 0x38, 0x3a, 0x2f, 0x23, 0x1a, 0x0d,
	0x05, 0xff, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_50[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_51[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd9, 0x71, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_52[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_53[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_54[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_55[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_56[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_57[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_58[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_59[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_60[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_61[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_62[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_63[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_64[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xe0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_65[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x05, 0x05, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_66[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xf0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_67[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xcb, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_68[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_69[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x09, 0x0b, 0x0d,
	0x0f, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_70[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_71[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x2e, 0x2d, 0x0a, 0x0c, 0x0e, 0x10,
	0x02, 0x04, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_72[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_73[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x2e, 0x2d, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_74[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_75[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x10, 0x0e, 0x0c,
	0x0a, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_76[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_77[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x2d, 0x2e, 0x0f, 0x0d, 0x0b, 0x09,
	0x03, 0x01, 0x00, 0x00
};
static char booyi_otm1287_720p_video_on_cmd_78[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_79[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x2d, 0x2e, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_80[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_81[] = {
	0x0d, 0x00, 0x39, 0xc0, 0xce, 0x8b, 0x03, 0x00,
	0x8a, 0x03, 0x00, 0x89, 0x03, 0x00, 0x88, 0x03,
	0x00, 0xff, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_82[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_83[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_84[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_85[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x07, 0x84,
	0xfc, 0x8b, 0x04, 0x00, 0x38, 0x06, 0x84, 0xfd,
	0x8b, 0x04, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_86[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_87[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x05, 0x84,
	0xfe, 0x8b, 0x04, 0x00, 0x38, 0x04, 0x84, 0xff,
	0x8b, 0x04, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_88[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_89[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x03, 0x85,
	0x00, 0x8b, 0x04, 0x00, 0x38, 0x02, 0x85, 0x01,
	0x8b, 0x04, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_90[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_91[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x01, 0x85,
	0x02, 0x8b, 0x04, 0x00, 0x38, 0x00, 0x85, 0x03,
	0x8b, 0x04, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_92[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_93[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcf, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_94[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_95[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcf, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_96[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_97[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcf, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_98[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_99[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xcf, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_100[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_101[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xcf, 0x01, 0x01, 0x20,
	0x20, 0x00, 0x00, 0x01, 0x01, 0x00, 0x02, 0x02
};
static char booyi_otm1287_720p_video_on_cmd_102[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb5, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_103[] = {
	0x07, 0x00, 0x39, 0xc0, 0xc5, 0x33, 0xf1, 0xff,
	0x33, 0xf1, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_104[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb1, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_105[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0x05, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_106[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_107[] = {
	0x02, 0x00, 0x39, 0xc0, 0x35, 0x01, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_108[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_109[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0xff, 0xff, 0xff
};
static char booyi_otm1287_720p_video_on_cmd_110[] = {
	0x11, 0x00, 0x05, 0x80
};
static char booyi_otm1287_720p_video_on_cmd_111[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd booyi_otm1287_720p_video_on_command[] = {
	{ sizeof(booyi_otm1287_720p_video_on_cmd_0), booyi_otm1287_720p_video_on_cmd_0, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_1), booyi_otm1287_720p_video_on_cmd_1, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_2), booyi_otm1287_720p_video_on_cmd_2, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_3), booyi_otm1287_720p_video_on_cmd_3, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_4), booyi_otm1287_720p_video_on_cmd_4, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_5), booyi_otm1287_720p_video_on_cmd_5, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_6), booyi_otm1287_720p_video_on_cmd_6, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_7), booyi_otm1287_720p_video_on_cmd_7, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_8), booyi_otm1287_720p_video_on_cmd_8, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_9), booyi_otm1287_720p_video_on_cmd_9, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_10), booyi_otm1287_720p_video_on_cmd_10, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_11), booyi_otm1287_720p_video_on_cmd_11, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_12), booyi_otm1287_720p_video_on_cmd_12, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_13), booyi_otm1287_720p_video_on_cmd_13, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_14), booyi_otm1287_720p_video_on_cmd_14, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_15), booyi_otm1287_720p_video_on_cmd_15, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_16), booyi_otm1287_720p_video_on_cmd_16, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_17), booyi_otm1287_720p_video_on_cmd_17, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_18), booyi_otm1287_720p_video_on_cmd_18, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_19), booyi_otm1287_720p_video_on_cmd_19, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_20), booyi_otm1287_720p_video_on_cmd_20, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_21), booyi_otm1287_720p_video_on_cmd_21, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_22), booyi_otm1287_720p_video_on_cmd_22, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_23), booyi_otm1287_720p_video_on_cmd_23, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_24), booyi_otm1287_720p_video_on_cmd_24, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_25), booyi_otm1287_720p_video_on_cmd_25, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_26), booyi_otm1287_720p_video_on_cmd_26, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_27), booyi_otm1287_720p_video_on_cmd_27, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_28), booyi_otm1287_720p_video_on_cmd_28, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_29), booyi_otm1287_720p_video_on_cmd_29, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_30), booyi_otm1287_720p_video_on_cmd_30, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_31), booyi_otm1287_720p_video_on_cmd_31, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_32), booyi_otm1287_720p_video_on_cmd_32, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_33), booyi_otm1287_720p_video_on_cmd_33, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_34), booyi_otm1287_720p_video_on_cmd_34, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_35), booyi_otm1287_720p_video_on_cmd_35, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_36), booyi_otm1287_720p_video_on_cmd_36, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_37), booyi_otm1287_720p_video_on_cmd_37, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_38), booyi_otm1287_720p_video_on_cmd_38, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_39), booyi_otm1287_720p_video_on_cmd_39, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_40), booyi_otm1287_720p_video_on_cmd_40, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_41), booyi_otm1287_720p_video_on_cmd_41, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_42), booyi_otm1287_720p_video_on_cmd_42, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_43), booyi_otm1287_720p_video_on_cmd_43, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_44), booyi_otm1287_720p_video_on_cmd_44, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_45), booyi_otm1287_720p_video_on_cmd_45, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_46), booyi_otm1287_720p_video_on_cmd_46, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_47), booyi_otm1287_720p_video_on_cmd_47, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_48), booyi_otm1287_720p_video_on_cmd_48, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_49), booyi_otm1287_720p_video_on_cmd_49, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_50), booyi_otm1287_720p_video_on_cmd_50, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_51), booyi_otm1287_720p_video_on_cmd_51, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_52), booyi_otm1287_720p_video_on_cmd_52, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_53), booyi_otm1287_720p_video_on_cmd_53, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_54), booyi_otm1287_720p_video_on_cmd_54, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_55), booyi_otm1287_720p_video_on_cmd_55, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_56), booyi_otm1287_720p_video_on_cmd_56, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_57), booyi_otm1287_720p_video_on_cmd_57, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_58), booyi_otm1287_720p_video_on_cmd_58, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_59), booyi_otm1287_720p_video_on_cmd_59, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_60), booyi_otm1287_720p_video_on_cmd_60, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_61), booyi_otm1287_720p_video_on_cmd_61, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_62), booyi_otm1287_720p_video_on_cmd_62, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_63), booyi_otm1287_720p_video_on_cmd_63, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_64), booyi_otm1287_720p_video_on_cmd_64, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_65), booyi_otm1287_720p_video_on_cmd_65, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_66), booyi_otm1287_720p_video_on_cmd_66, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_67), booyi_otm1287_720p_video_on_cmd_67, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_68), booyi_otm1287_720p_video_on_cmd_68, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_69), booyi_otm1287_720p_video_on_cmd_69, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_70), booyi_otm1287_720p_video_on_cmd_70, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_71), booyi_otm1287_720p_video_on_cmd_71, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_72), booyi_otm1287_720p_video_on_cmd_72, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_73), booyi_otm1287_720p_video_on_cmd_73, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_74), booyi_otm1287_720p_video_on_cmd_74, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_75), booyi_otm1287_720p_video_on_cmd_75, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_76), booyi_otm1287_720p_video_on_cmd_76, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_77), booyi_otm1287_720p_video_on_cmd_77, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_78), booyi_otm1287_720p_video_on_cmd_78, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_79), booyi_otm1287_720p_video_on_cmd_79, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_80), booyi_otm1287_720p_video_on_cmd_80, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_81), booyi_otm1287_720p_video_on_cmd_81, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_82), booyi_otm1287_720p_video_on_cmd_82, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_83), booyi_otm1287_720p_video_on_cmd_83, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_84), booyi_otm1287_720p_video_on_cmd_84, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_85), booyi_otm1287_720p_video_on_cmd_85, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_86), booyi_otm1287_720p_video_on_cmd_86, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_87), booyi_otm1287_720p_video_on_cmd_87, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_88), booyi_otm1287_720p_video_on_cmd_88, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_89), booyi_otm1287_720p_video_on_cmd_89, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_90), booyi_otm1287_720p_video_on_cmd_90, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_91), booyi_otm1287_720p_video_on_cmd_91, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_92), booyi_otm1287_720p_video_on_cmd_92, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_93), booyi_otm1287_720p_video_on_cmd_93, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_94), booyi_otm1287_720p_video_on_cmd_94, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_95), booyi_otm1287_720p_video_on_cmd_95, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_96), booyi_otm1287_720p_video_on_cmd_96, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_97), booyi_otm1287_720p_video_on_cmd_97, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_98), booyi_otm1287_720p_video_on_cmd_98, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_99), booyi_otm1287_720p_video_on_cmd_99, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_100), booyi_otm1287_720p_video_on_cmd_100, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_101), booyi_otm1287_720p_video_on_cmd_101, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_102), booyi_otm1287_720p_video_on_cmd_102, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_103), booyi_otm1287_720p_video_on_cmd_103, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_104), booyi_otm1287_720p_video_on_cmd_104, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_105), booyi_otm1287_720p_video_on_cmd_105, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_106), booyi_otm1287_720p_video_on_cmd_106, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_107), booyi_otm1287_720p_video_on_cmd_107, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_108), booyi_otm1287_720p_video_on_cmd_108, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_109), booyi_otm1287_720p_video_on_cmd_109, 1 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_110), booyi_otm1287_720p_video_on_cmd_110, 120 },
	{ sizeof(booyi_otm1287_720p_video_on_cmd_111), booyi_otm1287_720p_video_on_cmd_111, 0 },
};

static char booyi_otm1287_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char booyi_otm1287_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd booyi_otm1287_720p_video_off_command[] = {
	{ sizeof(booyi_otm1287_720p_video_off_cmd_0), booyi_otm1287_720p_video_off_cmd_0, 50 },
	{ sizeof(booyi_otm1287_720p_video_off_cmd_1), booyi_otm1287_720p_video_off_cmd_1, 120 },
};

static struct command_state booyi_otm1287_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info booyi_otm1287_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info booyi_otm1287_720p_video_video_panel = {
	.hsync_pulse = 0,
	.hfp_power_mode = 0,
	.hbp_power_mode = 0,
	.hsa_power_mode = 0,
	.bllp_eof_power_mode = 1,
	.bllp_power_mode = 1,
	.traffic_mode = 0,
	/* This is bllp_eof_power_mode and bllp_power_mode combined */
	.bllp_eof_power = 1 << 3 | 1 << 0,
};

static struct lane_configuration booyi_otm1287_720p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t booyi_otm1287_720p_video_timings[] = {
	0x82, 0x1a, 0x12, 0x00, 0x40, 0x42, 0x16, 0x1e, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing booyi_otm1287_720p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence booyi_otm1287_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 10, 20 },
	.pin_direction = 2,
};

static struct backlight booyi_otm1287_720p_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 4095,
};

static inline void panel_booyi_otm1287_720p_video_select(struct panel_struct *panel,
							 struct msm_panel_info *pinfo,
							 struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &booyi_otm1287_720p_video_panel_data;
	panel->panelres = &booyi_otm1287_720p_video_panel_res;
	panel->color = &booyi_otm1287_720p_video_color;
	panel->videopanel = &booyi_otm1287_720p_video_video_panel;
	panel->commandpanel = &booyi_otm1287_720p_video_command_panel;
	panel->state = &booyi_otm1287_720p_video_state;
	panel->laneconfig = &booyi_otm1287_720p_video_lane_config;
	panel->paneltiminginfo = &booyi_otm1287_720p_video_timing_info;
	panel->panelresetseq = &booyi_otm1287_720p_video_reset_seq;
	panel->backlightinfo = &booyi_otm1287_720p_video_backlight;
	pinfo->mipi.panel_cmds = booyi_otm1287_720p_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(booyi_otm1287_720p_video_on_command);
	memcpy(phy_db->timing, booyi_otm1287_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_BOOYI_OTM1287_720P_VIDEO_H_ */
