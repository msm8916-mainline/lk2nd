// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_TC7X_ILI9881C_5P0INCH_720P_VIDEO_H_
#define _PANEL_TC7X_ILI9881C_5P0INCH_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config tc7x_ili9881c_5p0inch_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_tc7x_ili9881c_5p0inch_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution tc7x_ili9881c_5p0inch_720p_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 80,
	.hback_porch = 150,
	.hpulse_width = 8,
	.hsync_skew = 0,
	.vfront_porch = 20,
	.vback_porch = 20,
	.vpulse_width = 12,
	/* Borders not supported yet */
};

static struct color_info tc7x_ili9881c_5p0inch_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x03
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0x01, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_2[] = {
	0x02, 0x00, 0x39, 0xc0, 0x02, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_3[] = {
	0x02, 0x00, 0x39, 0xc0, 0x03, 0x73, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_4[] = {
	0x02, 0x00, 0x39, 0xc0, 0x04, 0xd3, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_5[] = {
	0x02, 0x00, 0x39, 0xc0, 0x05, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0x06, 0x0a, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0x07, 0x0e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0x08, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_9[] = {
	0x02, 0x00, 0x39, 0xc0, 0x09, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_10[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0a, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_11[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0b, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0c, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_13[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0d, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_14[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0e, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0f, 0x21, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0x10, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_17[] = {
	0x02, 0x00, 0x39, 0xc0, 0x11, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_18[] = {
	0x02, 0x00, 0x39, 0xc0, 0x12, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_19[] = {
	0x02, 0x00, 0x39, 0xc0, 0x13, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_20[] = {
	0x02, 0x00, 0x39, 0xc0, 0x14, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_21[] = {
	0x02, 0x00, 0x39, 0xc0, 0x15, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_22[] = {
	0x02, 0x00, 0x39, 0xc0, 0x16, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_23[] = {
	0x02, 0x00, 0x39, 0xc0, 0x17, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_24[] = {
	0x02, 0x00, 0x39, 0xc0, 0x18, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_25[] = {
	0x02, 0x00, 0x39, 0xc0, 0x19, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_26[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1a, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_27[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1b, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_28[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1c, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_29[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1d, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_30[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1e, 0x40, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_31[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1f, 0x80, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_32[] = {
	0x02, 0x00, 0x39, 0xc0, 0x20, 0x06, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_33[] = {
	0x02, 0x00, 0x39, 0xc0, 0x21, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_34[] = {
	0x02, 0x00, 0x39, 0xc0, 0x22, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_35[] = {
	0x02, 0x00, 0x39, 0xc0, 0x23, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_36[] = {
	0x02, 0x00, 0x39, 0xc0, 0x24, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_37[] = {
	0x02, 0x00, 0x39, 0xc0, 0x25, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_38[] = {
	0x02, 0x00, 0x39, 0xc0, 0x26, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_39[] = {
	0x02, 0x00, 0x39, 0xc0, 0x27, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_40[] = {
	0x02, 0x00, 0x39, 0xc0, 0x28, 0x33, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_41[] = {
	0x02, 0x00, 0x39, 0xc0, 0x29, 0x03, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_42[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2a, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_43[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2b, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_44[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2c, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_45[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2d, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_46[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2e, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_47[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2f, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_48[] = {
	0x02, 0x00, 0x39, 0xc0, 0x30, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_49[] = {
	0x02, 0x00, 0x39, 0xc0, 0x31, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_50[] = {
	0x02, 0x00, 0x39, 0xc0, 0x32, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_51[] = {
	0x02, 0x00, 0x39, 0xc0, 0x33, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_52[] = {
	0x02, 0x00, 0x39, 0xc0, 0x34, 0x03, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_53[] = {
	0x02, 0x00, 0x39, 0xc0, 0x35, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_54[] = {
	0x02, 0x00, 0x39, 0xc0, 0x36, 0x03, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_55[] = {
	0x02, 0x00, 0x39, 0xc0, 0x37, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_56[] = {
	0x02, 0x00, 0x39, 0xc0, 0x38, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_57[] = {
	0x02, 0x00, 0x39, 0xc0, 0x39, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_58[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3a, 0x40, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_59[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3b, 0x40, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_60[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3c, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_61[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3d, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_62[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3e, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_63[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3f, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_64[] = {
	0x02, 0x00, 0x39, 0xc0, 0x40, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_65[] = {
	0x02, 0x00, 0x39, 0xc0, 0x41, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_66[] = {
	0x02, 0x00, 0x39, 0xc0, 0x42, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_67[] = {
	0x02, 0x00, 0x39, 0xc0, 0x43, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_68[] = {
	0x02, 0x00, 0x39, 0xc0, 0x44, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_69[] = {
	0x02, 0x00, 0x39, 0xc0, 0x50, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_70[] = {
	0x02, 0x00, 0x39, 0xc0, 0x51, 0x23, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_71[] = {
	0x02, 0x00, 0x39, 0xc0, 0x52, 0x45, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_72[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x67, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_73[] = {
	0x02, 0x00, 0x39, 0xc0, 0x54, 0x89, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_74[] = {
	0x02, 0x00, 0x39, 0xc0, 0x55, 0xab, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_75[] = {
	0x02, 0x00, 0x39, 0xc0, 0x56, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_76[] = {
	0x02, 0x00, 0x39, 0xc0, 0x57, 0x23, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_77[] = {
	0x02, 0x00, 0x39, 0xc0, 0x58, 0x45, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_78[] = {
	0x02, 0x00, 0x39, 0xc0, 0x59, 0x67, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_79[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5a, 0x89, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_80[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5b, 0xab, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_81[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5c, 0xcd, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_82[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5d, 0xef, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_83[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5e, 0x11, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_84[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5f, 0x08, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_85[] = {
	0x02, 0x00, 0x39, 0xc0, 0x60, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_86[] = {
	0x02, 0x00, 0x39, 0xc0, 0x61, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_87[] = {
	0x02, 0x00, 0x39, 0xc0, 0x62, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_88[] = {
	0x02, 0x00, 0x39, 0xc0, 0x63, 0x0d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_89[] = {
	0x02, 0x00, 0x39, 0xc0, 0x64, 0x0c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_90[] = {
	0x02, 0x00, 0x39, 0xc0, 0x65, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_91[] = {
	0x02, 0x00, 0x39, 0xc0, 0x66, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_92[] = {
	0x02, 0x00, 0x39, 0xc0, 0x67, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_93[] = {
	0x02, 0x00, 0x39, 0xc0, 0x68, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_94[] = {
	0x02, 0x00, 0x39, 0xc0, 0x69, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_95[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6a, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_96[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6b, 0x0f, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_97[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6c, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_98[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6d, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_99[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6e, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_100[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_101[] = {
	0x02, 0x00, 0x39, 0xc0, 0x70, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_102[] = {
	0x02, 0x00, 0x39, 0xc0, 0x71, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_103[] = {
	0x02, 0x00, 0x39, 0xc0, 0x72, 0x0e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_104[] = {
	0x02, 0x00, 0x39, 0xc0, 0x73, 0x06, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_105[] = {
	0x02, 0x00, 0x39, 0xc0, 0x74, 0x07, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_106[] = {
	0x02, 0x00, 0x39, 0xc0, 0x75, 0x08, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_107[] = {
	0x02, 0x00, 0x39, 0xc0, 0x76, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_108[] = {
	0x02, 0x00, 0x39, 0xc0, 0x77, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_109[] = {
	0x02, 0x00, 0x39, 0xc0, 0x78, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_110[] = {
	0x02, 0x00, 0x39, 0xc0, 0x79, 0x0d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_111[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7a, 0x0c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_112[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7b, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_113[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7c, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_114[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7d, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_115[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7e, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_116[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7f, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_117[] = {
	0x02, 0x00, 0x39, 0xc0, 0x80, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_118[] = {
	0x02, 0x00, 0x39, 0xc0, 0x81, 0x0f, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_119[] = {
	0x02, 0x00, 0x39, 0xc0, 0x82, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_120[] = {
	0x02, 0x00, 0x39, 0xc0, 0x83, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_121[] = {
	0x02, 0x00, 0x39, 0xc0, 0x84, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_122[] = {
	0x02, 0x00, 0x39, 0xc0, 0x85, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_123[] = {
	0x02, 0x00, 0x39, 0xc0, 0x86, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_124[] = {
	0x02, 0x00, 0x39, 0xc0, 0x87, 0x02, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_125[] = {
	0x02, 0x00, 0x39, 0xc0, 0x88, 0x0e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_126[] = {
	0x02, 0x00, 0x39, 0xc0, 0x89, 0x06, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_127[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8a, 0x07, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_128[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x04
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_129[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6c, 0x15, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_130[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6e, 0x1a, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_131[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x33, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_132[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3a, 0xa4, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_133[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8d, 0x14, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_134[] = {
	0x02, 0x00, 0x39, 0xc0, 0x87, 0x2a, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_135[] = {
	0x02, 0x00, 0x39, 0xc0, 0x26, 0x76, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_136[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb2, 0xd1, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_137[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x01
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_138[] = {
	0x02, 0x00, 0x39, 0xc0, 0x22, 0x09, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_139[] = {
	0x02, 0x00, 0x39, 0xc0, 0x31, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_140[] = {
	0x02, 0x00, 0x39, 0xc0, 0x50, 0x96, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_141[] = {
	0x02, 0x00, 0x39, 0xc0, 0x51, 0x96, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_142[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x5d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_143[] = {
	0x02, 0x00, 0x39, 0xc0, 0x55, 0x61, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_144[] = {
	0x02, 0x00, 0x39, 0xc0, 0x60, 0x14, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_145[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa0, 0x40, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_146[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa1, 0x14, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_147[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa2, 0x22, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_148[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa3, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_149[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa4, 0x16, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_150[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa5, 0x23, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_151[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa6, 0x1a, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_152[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa7, 0x1c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_153[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa8, 0x87, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_154[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa9, 0x1d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_155[] = {
	0x02, 0x00, 0x39, 0xc0, 0xaa, 0x29, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_156[] = {
	0x02, 0x00, 0x39, 0xc0, 0xab, 0x7e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_157[] = {
	0x02, 0x00, 0x39, 0xc0, 0xac, 0x1c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_158[] = {
	0x02, 0x00, 0x39, 0xc0, 0xad, 0x1d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_159[] = {
	0x02, 0x00, 0x39, 0xc0, 0xae, 0x4f, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_160[] = {
	0x02, 0x00, 0x39, 0xc0, 0xaf, 0x22, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_161[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0x28, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_162[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb1, 0x4e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_163[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb2, 0x5c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_164[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb3, 0x30, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_165[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x40, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_166[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x14, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_167[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc2, 0x22, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_168[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc3, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_169[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x16, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_170[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x23, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_171[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0x1a, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_172[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x1c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_173[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc8, 0x87, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_174[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc9, 0x1d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_175[] = {
	0x02, 0x00, 0x39, 0xc0, 0xca, 0x29, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_176[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcb, 0x7e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_177[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x1c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_178[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcd, 0x1d, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_179[] = {
	0x02, 0x00, 0x39, 0xc0, 0xce, 0x4f, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_180[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcf, 0x22, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_181[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd0, 0x28, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_182[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd1, 0x4e, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_183[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd2, 0x5c, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_184[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd3, 0x30, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_185[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x00
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_186[] = {
	0x03, 0x00, 0x39, 0xc0, 0x51, 0x0f, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_187[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x24, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_188[] = {
	0x02, 0x00, 0x39, 0xc0, 0x55, 0x01, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_189[] = {
	0x34, 0x00, 0x05, 0x80
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_190[] = {
	0x02, 0x00, 0x39, 0xc0, 0x36, 0x00, 0xff, 0xff
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_191[] = {
	0x11, 0x00, 0x05, 0x80
};
static char tc7x_ili9881c_5p0inch_720p_video_on_cmd_192[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd tc7x_ili9881c_5p0inch_720p_video_on_command[] = {
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_0), tc7x_ili9881c_5p0inch_720p_video_on_cmd_0, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_1), tc7x_ili9881c_5p0inch_720p_video_on_cmd_1, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_2), tc7x_ili9881c_5p0inch_720p_video_on_cmd_2, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_3), tc7x_ili9881c_5p0inch_720p_video_on_cmd_3, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_4), tc7x_ili9881c_5p0inch_720p_video_on_cmd_4, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_5), tc7x_ili9881c_5p0inch_720p_video_on_cmd_5, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_6), tc7x_ili9881c_5p0inch_720p_video_on_cmd_6, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_7), tc7x_ili9881c_5p0inch_720p_video_on_cmd_7, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_8), tc7x_ili9881c_5p0inch_720p_video_on_cmd_8, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_9), tc7x_ili9881c_5p0inch_720p_video_on_cmd_9, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_10), tc7x_ili9881c_5p0inch_720p_video_on_cmd_10, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_11), tc7x_ili9881c_5p0inch_720p_video_on_cmd_11, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_12), tc7x_ili9881c_5p0inch_720p_video_on_cmd_12, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_13), tc7x_ili9881c_5p0inch_720p_video_on_cmd_13, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_14), tc7x_ili9881c_5p0inch_720p_video_on_cmd_14, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_15), tc7x_ili9881c_5p0inch_720p_video_on_cmd_15, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_16), tc7x_ili9881c_5p0inch_720p_video_on_cmd_16, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_17), tc7x_ili9881c_5p0inch_720p_video_on_cmd_17, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_18), tc7x_ili9881c_5p0inch_720p_video_on_cmd_18, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_19), tc7x_ili9881c_5p0inch_720p_video_on_cmd_19, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_20), tc7x_ili9881c_5p0inch_720p_video_on_cmd_20, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_21), tc7x_ili9881c_5p0inch_720p_video_on_cmd_21, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_22), tc7x_ili9881c_5p0inch_720p_video_on_cmd_22, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_23), tc7x_ili9881c_5p0inch_720p_video_on_cmd_23, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_24), tc7x_ili9881c_5p0inch_720p_video_on_cmd_24, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_25), tc7x_ili9881c_5p0inch_720p_video_on_cmd_25, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_26), tc7x_ili9881c_5p0inch_720p_video_on_cmd_26, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_27), tc7x_ili9881c_5p0inch_720p_video_on_cmd_27, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_28), tc7x_ili9881c_5p0inch_720p_video_on_cmd_28, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_29), tc7x_ili9881c_5p0inch_720p_video_on_cmd_29, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_30), tc7x_ili9881c_5p0inch_720p_video_on_cmd_30, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_31), tc7x_ili9881c_5p0inch_720p_video_on_cmd_31, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_32), tc7x_ili9881c_5p0inch_720p_video_on_cmd_32, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_33), tc7x_ili9881c_5p0inch_720p_video_on_cmd_33, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_34), tc7x_ili9881c_5p0inch_720p_video_on_cmd_34, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_35), tc7x_ili9881c_5p0inch_720p_video_on_cmd_35, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_36), tc7x_ili9881c_5p0inch_720p_video_on_cmd_36, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_37), tc7x_ili9881c_5p0inch_720p_video_on_cmd_37, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_38), tc7x_ili9881c_5p0inch_720p_video_on_cmd_38, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_39), tc7x_ili9881c_5p0inch_720p_video_on_cmd_39, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_40), tc7x_ili9881c_5p0inch_720p_video_on_cmd_40, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_41), tc7x_ili9881c_5p0inch_720p_video_on_cmd_41, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_42), tc7x_ili9881c_5p0inch_720p_video_on_cmd_42, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_43), tc7x_ili9881c_5p0inch_720p_video_on_cmd_43, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_44), tc7x_ili9881c_5p0inch_720p_video_on_cmd_44, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_45), tc7x_ili9881c_5p0inch_720p_video_on_cmd_45, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_46), tc7x_ili9881c_5p0inch_720p_video_on_cmd_46, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_47), tc7x_ili9881c_5p0inch_720p_video_on_cmd_47, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_48), tc7x_ili9881c_5p0inch_720p_video_on_cmd_48, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_49), tc7x_ili9881c_5p0inch_720p_video_on_cmd_49, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_50), tc7x_ili9881c_5p0inch_720p_video_on_cmd_50, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_51), tc7x_ili9881c_5p0inch_720p_video_on_cmd_51, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_52), tc7x_ili9881c_5p0inch_720p_video_on_cmd_52, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_53), tc7x_ili9881c_5p0inch_720p_video_on_cmd_53, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_54), tc7x_ili9881c_5p0inch_720p_video_on_cmd_54, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_55), tc7x_ili9881c_5p0inch_720p_video_on_cmd_55, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_56), tc7x_ili9881c_5p0inch_720p_video_on_cmd_56, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_57), tc7x_ili9881c_5p0inch_720p_video_on_cmd_57, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_58), tc7x_ili9881c_5p0inch_720p_video_on_cmd_58, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_59), tc7x_ili9881c_5p0inch_720p_video_on_cmd_59, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_60), tc7x_ili9881c_5p0inch_720p_video_on_cmd_60, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_61), tc7x_ili9881c_5p0inch_720p_video_on_cmd_61, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_62), tc7x_ili9881c_5p0inch_720p_video_on_cmd_62, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_63), tc7x_ili9881c_5p0inch_720p_video_on_cmd_63, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_64), tc7x_ili9881c_5p0inch_720p_video_on_cmd_64, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_65), tc7x_ili9881c_5p0inch_720p_video_on_cmd_65, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_66), tc7x_ili9881c_5p0inch_720p_video_on_cmd_66, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_67), tc7x_ili9881c_5p0inch_720p_video_on_cmd_67, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_68), tc7x_ili9881c_5p0inch_720p_video_on_cmd_68, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_69), tc7x_ili9881c_5p0inch_720p_video_on_cmd_69, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_70), tc7x_ili9881c_5p0inch_720p_video_on_cmd_70, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_71), tc7x_ili9881c_5p0inch_720p_video_on_cmd_71, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_72), tc7x_ili9881c_5p0inch_720p_video_on_cmd_72, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_73), tc7x_ili9881c_5p0inch_720p_video_on_cmd_73, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_74), tc7x_ili9881c_5p0inch_720p_video_on_cmd_74, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_75), tc7x_ili9881c_5p0inch_720p_video_on_cmd_75, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_76), tc7x_ili9881c_5p0inch_720p_video_on_cmd_76, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_77), tc7x_ili9881c_5p0inch_720p_video_on_cmd_77, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_78), tc7x_ili9881c_5p0inch_720p_video_on_cmd_78, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_79), tc7x_ili9881c_5p0inch_720p_video_on_cmd_79, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_80), tc7x_ili9881c_5p0inch_720p_video_on_cmd_80, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_81), tc7x_ili9881c_5p0inch_720p_video_on_cmd_81, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_82), tc7x_ili9881c_5p0inch_720p_video_on_cmd_82, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_83), tc7x_ili9881c_5p0inch_720p_video_on_cmd_83, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_84), tc7x_ili9881c_5p0inch_720p_video_on_cmd_84, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_85), tc7x_ili9881c_5p0inch_720p_video_on_cmd_85, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_86), tc7x_ili9881c_5p0inch_720p_video_on_cmd_86, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_87), tc7x_ili9881c_5p0inch_720p_video_on_cmd_87, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_88), tc7x_ili9881c_5p0inch_720p_video_on_cmd_88, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_89), tc7x_ili9881c_5p0inch_720p_video_on_cmd_89, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_90), tc7x_ili9881c_5p0inch_720p_video_on_cmd_90, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_91), tc7x_ili9881c_5p0inch_720p_video_on_cmd_91, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_92), tc7x_ili9881c_5p0inch_720p_video_on_cmd_92, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_93), tc7x_ili9881c_5p0inch_720p_video_on_cmd_93, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_94), tc7x_ili9881c_5p0inch_720p_video_on_cmd_94, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_95), tc7x_ili9881c_5p0inch_720p_video_on_cmd_95, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_96), tc7x_ili9881c_5p0inch_720p_video_on_cmd_96, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_97), tc7x_ili9881c_5p0inch_720p_video_on_cmd_97, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_98), tc7x_ili9881c_5p0inch_720p_video_on_cmd_98, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_99), tc7x_ili9881c_5p0inch_720p_video_on_cmd_99, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_100), tc7x_ili9881c_5p0inch_720p_video_on_cmd_100, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_101), tc7x_ili9881c_5p0inch_720p_video_on_cmd_101, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_102), tc7x_ili9881c_5p0inch_720p_video_on_cmd_102, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_103), tc7x_ili9881c_5p0inch_720p_video_on_cmd_103, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_104), tc7x_ili9881c_5p0inch_720p_video_on_cmd_104, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_105), tc7x_ili9881c_5p0inch_720p_video_on_cmd_105, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_106), tc7x_ili9881c_5p0inch_720p_video_on_cmd_106, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_107), tc7x_ili9881c_5p0inch_720p_video_on_cmd_107, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_108), tc7x_ili9881c_5p0inch_720p_video_on_cmd_108, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_109), tc7x_ili9881c_5p0inch_720p_video_on_cmd_109, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_110), tc7x_ili9881c_5p0inch_720p_video_on_cmd_110, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_111), tc7x_ili9881c_5p0inch_720p_video_on_cmd_111, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_112), tc7x_ili9881c_5p0inch_720p_video_on_cmd_112, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_113), tc7x_ili9881c_5p0inch_720p_video_on_cmd_113, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_114), tc7x_ili9881c_5p0inch_720p_video_on_cmd_114, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_115), tc7x_ili9881c_5p0inch_720p_video_on_cmd_115, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_116), tc7x_ili9881c_5p0inch_720p_video_on_cmd_116, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_117), tc7x_ili9881c_5p0inch_720p_video_on_cmd_117, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_118), tc7x_ili9881c_5p0inch_720p_video_on_cmd_118, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_119), tc7x_ili9881c_5p0inch_720p_video_on_cmd_119, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_120), tc7x_ili9881c_5p0inch_720p_video_on_cmd_120, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_121), tc7x_ili9881c_5p0inch_720p_video_on_cmd_121, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_122), tc7x_ili9881c_5p0inch_720p_video_on_cmd_122, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_123), tc7x_ili9881c_5p0inch_720p_video_on_cmd_123, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_124), tc7x_ili9881c_5p0inch_720p_video_on_cmd_124, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_125), tc7x_ili9881c_5p0inch_720p_video_on_cmd_125, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_126), tc7x_ili9881c_5p0inch_720p_video_on_cmd_126, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_127), tc7x_ili9881c_5p0inch_720p_video_on_cmd_127, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_128), tc7x_ili9881c_5p0inch_720p_video_on_cmd_128, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_129), tc7x_ili9881c_5p0inch_720p_video_on_cmd_129, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_130), tc7x_ili9881c_5p0inch_720p_video_on_cmd_130, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_131), tc7x_ili9881c_5p0inch_720p_video_on_cmd_131, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_132), tc7x_ili9881c_5p0inch_720p_video_on_cmd_132, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_133), tc7x_ili9881c_5p0inch_720p_video_on_cmd_133, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_134), tc7x_ili9881c_5p0inch_720p_video_on_cmd_134, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_135), tc7x_ili9881c_5p0inch_720p_video_on_cmd_135, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_136), tc7x_ili9881c_5p0inch_720p_video_on_cmd_136, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_137), tc7x_ili9881c_5p0inch_720p_video_on_cmd_137, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_138), tc7x_ili9881c_5p0inch_720p_video_on_cmd_138, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_139), tc7x_ili9881c_5p0inch_720p_video_on_cmd_139, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_140), tc7x_ili9881c_5p0inch_720p_video_on_cmd_140, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_141), tc7x_ili9881c_5p0inch_720p_video_on_cmd_141, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_142), tc7x_ili9881c_5p0inch_720p_video_on_cmd_142, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_143), tc7x_ili9881c_5p0inch_720p_video_on_cmd_143, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_144), tc7x_ili9881c_5p0inch_720p_video_on_cmd_144, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_145), tc7x_ili9881c_5p0inch_720p_video_on_cmd_145, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_146), tc7x_ili9881c_5p0inch_720p_video_on_cmd_146, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_147), tc7x_ili9881c_5p0inch_720p_video_on_cmd_147, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_148), tc7x_ili9881c_5p0inch_720p_video_on_cmd_148, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_149), tc7x_ili9881c_5p0inch_720p_video_on_cmd_149, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_150), tc7x_ili9881c_5p0inch_720p_video_on_cmd_150, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_151), tc7x_ili9881c_5p0inch_720p_video_on_cmd_151, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_152), tc7x_ili9881c_5p0inch_720p_video_on_cmd_152, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_153), tc7x_ili9881c_5p0inch_720p_video_on_cmd_153, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_154), tc7x_ili9881c_5p0inch_720p_video_on_cmd_154, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_155), tc7x_ili9881c_5p0inch_720p_video_on_cmd_155, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_156), tc7x_ili9881c_5p0inch_720p_video_on_cmd_156, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_157), tc7x_ili9881c_5p0inch_720p_video_on_cmd_157, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_158), tc7x_ili9881c_5p0inch_720p_video_on_cmd_158, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_159), tc7x_ili9881c_5p0inch_720p_video_on_cmd_159, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_160), tc7x_ili9881c_5p0inch_720p_video_on_cmd_160, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_161), tc7x_ili9881c_5p0inch_720p_video_on_cmd_161, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_162), tc7x_ili9881c_5p0inch_720p_video_on_cmd_162, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_163), tc7x_ili9881c_5p0inch_720p_video_on_cmd_163, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_164), tc7x_ili9881c_5p0inch_720p_video_on_cmd_164, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_165), tc7x_ili9881c_5p0inch_720p_video_on_cmd_165, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_166), tc7x_ili9881c_5p0inch_720p_video_on_cmd_166, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_167), tc7x_ili9881c_5p0inch_720p_video_on_cmd_167, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_168), tc7x_ili9881c_5p0inch_720p_video_on_cmd_168, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_169), tc7x_ili9881c_5p0inch_720p_video_on_cmd_169, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_170), tc7x_ili9881c_5p0inch_720p_video_on_cmd_170, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_171), tc7x_ili9881c_5p0inch_720p_video_on_cmd_171, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_172), tc7x_ili9881c_5p0inch_720p_video_on_cmd_172, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_173), tc7x_ili9881c_5p0inch_720p_video_on_cmd_173, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_174), tc7x_ili9881c_5p0inch_720p_video_on_cmd_174, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_175), tc7x_ili9881c_5p0inch_720p_video_on_cmd_175, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_176), tc7x_ili9881c_5p0inch_720p_video_on_cmd_176, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_177), tc7x_ili9881c_5p0inch_720p_video_on_cmd_177, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_178), tc7x_ili9881c_5p0inch_720p_video_on_cmd_178, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_179), tc7x_ili9881c_5p0inch_720p_video_on_cmd_179, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_180), tc7x_ili9881c_5p0inch_720p_video_on_cmd_180, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_181), tc7x_ili9881c_5p0inch_720p_video_on_cmd_181, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_182), tc7x_ili9881c_5p0inch_720p_video_on_cmd_182, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_183), tc7x_ili9881c_5p0inch_720p_video_on_cmd_183, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_184), tc7x_ili9881c_5p0inch_720p_video_on_cmd_184, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_185), tc7x_ili9881c_5p0inch_720p_video_on_cmd_185, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_186), tc7x_ili9881c_5p0inch_720p_video_on_cmd_186, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_187), tc7x_ili9881c_5p0inch_720p_video_on_cmd_187, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_188), tc7x_ili9881c_5p0inch_720p_video_on_cmd_188, 0 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_189), tc7x_ili9881c_5p0inch_720p_video_on_cmd_189, 150 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_190), tc7x_ili9881c_5p0inch_720p_video_on_cmd_190, 15 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_191), tc7x_ili9881c_5p0inch_720p_video_on_cmd_191, 150 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_on_cmd_192), tc7x_ili9881c_5p0inch_720p_video_on_cmd_192, 15 },
};

static char tc7x_ili9881c_5p0inch_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char tc7x_ili9881c_5p0inch_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd tc7x_ili9881c_5p0inch_720p_video_off_command[] = {
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_off_cmd_0), tc7x_ili9881c_5p0inch_720p_video_off_cmd_0, 128 },
	{ sizeof(tc7x_ili9881c_5p0inch_720p_video_off_cmd_1), tc7x_ili9881c_5p0inch_720p_video_off_cmd_1, 128 },
};

static struct command_state tc7x_ili9881c_5p0inch_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info tc7x_ili9881c_5p0inch_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info tc7x_ili9881c_5p0inch_720p_video_video_panel = {
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

static struct lane_configuration tc7x_ili9881c_5p0inch_720p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t tc7x_ili9881c_5p0inch_720p_video_timings[] = {
	0x87, 0x1c, 0x12, 0x00, 0x42, 0x44, 0x18, 0x20, 0x17, 0x03, 0x04, 0x00
};

static struct panel_timing tc7x_ili9881c_5p0inch_720p_video_timing_info = {
	.tclk_post = 0x1f,
	.tclk_pre = 0x2d,
};

static struct panel_reset_sequence tc7x_ili9881c_5p0inch_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 20, 20 },
	.pin_direction = 2,
};

uint32_t panel_regulator_settings[] = {
	0x07, 0x09, 0x03, 0x00, 0x20, 0x07, 0x01
};


static struct backlight tc7x_ili9881c_5p0inch_720p_video_backlight = {
	.bl_interface_type = BL_WLED,
	.bl_min_level = 1,
	.bl_max_level = 4095,
};

static inline void panel_tc7x_ili9881c_5p0inch_720p_video_select(struct panel_struct *panel,
								 struct msm_panel_info *pinfo,
								 struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &tc7x_ili9881c_5p0inch_720p_video_panel_data;
	panel->panelres = &tc7x_ili9881c_5p0inch_720p_video_panel_res;
	panel->color = &tc7x_ili9881c_5p0inch_720p_video_color;
	panel->videopanel = &tc7x_ili9881c_5p0inch_720p_video_video_panel;
	panel->commandpanel = &tc7x_ili9881c_5p0inch_720p_video_command_panel;
	panel->state = &tc7x_ili9881c_5p0inch_720p_video_state;
	panel->laneconfig = &tc7x_ili9881c_5p0inch_720p_video_lane_config;
	panel->paneltiminginfo = &tc7x_ili9881c_5p0inch_720p_video_timing_info;
	panel->panelresetseq = &tc7x_ili9881c_5p0inch_720p_video_reset_seq;
	panel->backlightinfo = &tc7x_ili9881c_5p0inch_720p_video_backlight;
	pinfo->mipi.panel_on_cmds = tc7x_ili9881c_5p0inch_720p_video_on_command;
	pinfo->mipi.panel_off_cmds = tc7x_ili9881c_5p0inch_720p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(tc7x_ili9881c_5p0inch_720p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(tc7x_ili9881c_5p0inch_720p_video_off_command);
	memcpy(phy_db->timing, tc7x_ili9881c_5p0inch_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_TC7X_ILI9881C_5P0INCH_720P_VIDEO_H_ */
