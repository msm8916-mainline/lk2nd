// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_ILI9881C_HD_VIDEO_H_
#define _PANEL_ILI9881C_HD_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config ili9881c_hd_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_ili9881c_hd_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 5000,
};

static struct panel_resolution ili9881c_hd_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 128,
	.hback_porch = 160,
	.hpulse_width = 8,
	.hsync_skew = 0,
	.vfront_porch = 8,
	.vback_porch = 20,
	.vpulse_width = 8,
	/* Borders not supported yet */
};

static struct color_info ili9881c_hd_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char ili9881c_hd_video_on_cmd_0[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x03
};
static char ili9881c_hd_video_on_cmd_1[] = {
	0x01, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_2[] = {
	0x02, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_3[] = {
	0x03, 0x53, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_4[] = {
	0x04, 0x13, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_5[] = {
	0x05, 0x13, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_6[] = {
	0x06, 0x06, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_7[] = {
	0x07, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_8[] = {
	0x08, 0x04, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_9[] = {
	0x09, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_10[] = {
	0x0a, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_11[] = {
	0x0b, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_12[] = {
	0x0c, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_13[] = {
	0x0d, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_14[] = {
	0x0e, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_15[] = {
	0x0f, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_16[] = {
	0x10, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_17[] = {
	0x11, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_18[] = {
	0x12, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_19[] = {
	0x13, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_20[] = {
	0x14, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_21[] = {
	0x15, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_22[] = {
	0x16, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_23[] = {
	0x17, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_24[] = {
	0x18, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_25[] = {
	0x19, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_26[] = {
	0x1a, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_27[] = {
	0x1b, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_28[] = {
	0x1c, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_29[] = {
	0x1d, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_30[] = {
	0x1e, 0xc0, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_31[] = {
	0x1f, 0x80, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_32[] = {
	0x20, 0x04, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_33[] = {
	0x21, 0x0b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_34[] = {
	0x22, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_35[] = {
	0x23, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_36[] = {
	0x24, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_37[] = {
	0x25, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_38[] = {
	0x26, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_39[] = {
	0x27, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_40[] = {
	0x28, 0x55, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_41[] = {
	0x29, 0x03, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_42[] = {
	0x2a, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_43[] = {
	0x2b, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_44[] = {
	0x2c, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_45[] = {
	0x2d, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_46[] = {
	0x2e, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_47[] = {
	0x2f, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_48[] = {
	0x30, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_49[] = {
	0x31, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_50[] = {
	0x32, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_51[] = {
	0x33, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_52[] = {
	0x34, 0x04, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_53[] = {
	0x35, 0x05, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_54[] = {
	0x36, 0x05, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_55[] = {
	0x37, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_56[] = {
	0x38, 0x3c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_57[] = {
	0x39, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_58[] = {
	0x3a, 0x40, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_59[] = {
	0x3b, 0x40, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_60[] = {
	0x3c, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_61[] = {
	0x3d, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_62[] = {
	0x3e, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_63[] = {
	0x3f, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_64[] = {
	0x40, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_65[] = {
	0x41, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_66[] = {
	0x42, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_67[] = {
	0x43, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_68[] = {
	0x44, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_69[] = {
	0x50, 0x01, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_70[] = {
	0x51, 0x23, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_71[] = {
	0x52, 0x45, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_72[] = {
	0x53, 0x67, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_73[] = {
	0x54, 0x89, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_74[] = {
	0x55, 0xab, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_75[] = {
	0x56, 0x01, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_76[] = {
	0x57, 0x23, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_77[] = {
	0x58, 0x45, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_78[] = {
	0x59, 0x67, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_79[] = {
	0x5a, 0x89, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_80[] = {
	0x5b, 0xab, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_81[] = {
	0x5c, 0xcd, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_82[] = {
	0x5d, 0xef, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_83[] = {
	0x5e, 0x01, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_84[] = {
	0x5f, 0x14, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_85[] = {
	0x60, 0x15, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_86[] = {
	0x61, 0x0c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_87[] = {
	0x62, 0x0d, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_88[] = {
	0x63, 0x0e, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_89[] = {
	0x64, 0x0f, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_90[] = {
	0x65, 0x10, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_91[] = {
	0x66, 0x11, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_92[] = {
	0x67, 0x08, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_93[] = {
	0x68, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_94[] = {
	0x69, 0x0a, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_95[] = {
	0x6a, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_96[] = {
	0x6b, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_97[] = {
	0x6c, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_98[] = {
	0x6d, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_99[] = {
	0x6e, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_100[] = {
	0x6f, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_101[] = {
	0x70, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_102[] = {
	0x71, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_103[] = {
	0x72, 0x06, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_104[] = {
	0x73, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_105[] = {
	0x74, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_106[] = {
	0x75, 0x14, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_107[] = {
	0x76, 0x15, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_108[] = {
	0x77, 0x11, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_109[] = {
	0x78, 0x10, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_110[] = {
	0x79, 0x0f, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_111[] = {
	0x7a, 0x0e, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_112[] = {
	0x7b, 0x0d, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_113[] = {
	0x7c, 0x0c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_114[] = {
	0x7d, 0x06, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_115[] = {
	0x7e, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_116[] = {
	0x7f, 0x0a, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_117[] = {
	0x80, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_118[] = {
	0x81, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_119[] = {
	0x82, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_120[] = {
	0x83, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_121[] = {
	0x84, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_122[] = {
	0x85, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_123[] = {
	0x86, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_124[] = {
	0x87, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_125[] = {
	0x88, 0x08, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_126[] = {
	0x89, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_127[] = {
	0x8a, 0x02, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_128[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x04
};
static char ili9881c_hd_video_on_cmd_129[] = {
	0x6c, 0x15, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_130[] = {
	0x6e, 0x3b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_131[] = {
	0x6f, 0x53, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_132[] = {
	0x3a, 0xa4, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_133[] = {
	0x8d, 0x15, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_134[] = {
	0x87, 0xba, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_135[] = {
	0xb2, 0xd1, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_136[] = {
	0x26, 0x76, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_137[] = {
	0x88, 0x0b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_138[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x01
};
static char ili9881c_hd_video_on_cmd_139[] = {
	0x22, 0x0a, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_140[] = {
	0x31, 0x00, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_141[] = {
	0x53, 0x88, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_142[] = {
	0x55, 0x83, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_143[] = {
	0x50, 0xa6, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_144[] = {
	0x51, 0xa6, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_145[] = {
	0x60, 0x14, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_146[] = {
	0xa0, 0x08, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_147[] = {
	0xa1, 0x22, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_148[] = {
	0xa2, 0x30, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_149[] = {
	0xa3, 0x15, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_150[] = {
	0xa4, 0x17, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_151[] = {
	0xa5, 0x2b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_152[] = {
	0xa6, 0x1f, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_153[] = {
	0xa7, 0x1f, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_154[] = {
	0xa8, 0x88, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_155[] = {
	0xa9, 0x1c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_156[] = {
	0xaa, 0x28, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_157[] = {
	0xab, 0x6c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_158[] = {
	0xac, 0x1b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_159[] = {
	0xad, 0x18, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_160[] = {
	0xae, 0x4b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_161[] = {
	0xaf, 0x20, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_162[] = {
	0xb0, 0x26, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_163[] = {
	0xb1, 0x43, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_164[] = {
	0xb2, 0x52, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_165[] = {
	0xb3, 0x2c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_166[] = {
	0xc0, 0x08, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_167[] = {
	0xc1, 0x22, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_168[] = {
	0xc2, 0x30, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_169[] = {
	0xc3, 0x15, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_170[] = {
	0xc4, 0x17, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_171[] = {
	0xc5, 0x2b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_172[] = {
	0xc6, 0x1f, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_173[] = {
	0xc7, 0x1f, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_174[] = {
	0xc8, 0x88, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_175[] = {
	0xc9, 0x1c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_176[] = {
	0xca, 0x28, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_177[] = {
	0xcb, 0x6c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_178[] = {
	0xcc, 0x1b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_179[] = {
	0xcd, 0x18, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_180[] = {
	0xce, 0x4b, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_181[] = {
	0xcf, 0x20, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_182[] = {
	0xd0, 0x26, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_183[] = {
	0xd1, 0x43, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_184[] = {
	0xd2, 0x52, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_185[] = {
	0xd3, 0x2c, 0x15, 0x80
};
static char ili9881c_hd_video_on_cmd_186[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x98, 0x81, 0x00
};
static char ili9881c_hd_video_on_cmd_187[] = {
	0x11, 0x00, 0x05, 0x80
};
static char ili9881c_hd_video_on_cmd_188[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd ili9881c_hd_video_on_command[] = {
	{ sizeof(ili9881c_hd_video_on_cmd_0), ili9881c_hd_video_on_cmd_0, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_1), ili9881c_hd_video_on_cmd_1, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_2), ili9881c_hd_video_on_cmd_2, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_3), ili9881c_hd_video_on_cmd_3, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_4), ili9881c_hd_video_on_cmd_4, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_5), ili9881c_hd_video_on_cmd_5, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_6), ili9881c_hd_video_on_cmd_6, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_7), ili9881c_hd_video_on_cmd_7, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_8), ili9881c_hd_video_on_cmd_8, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_9), ili9881c_hd_video_on_cmd_9, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_10), ili9881c_hd_video_on_cmd_10, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_11), ili9881c_hd_video_on_cmd_11, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_12), ili9881c_hd_video_on_cmd_12, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_13), ili9881c_hd_video_on_cmd_13, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_14), ili9881c_hd_video_on_cmd_14, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_15), ili9881c_hd_video_on_cmd_15, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_16), ili9881c_hd_video_on_cmd_16, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_17), ili9881c_hd_video_on_cmd_17, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_18), ili9881c_hd_video_on_cmd_18, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_19), ili9881c_hd_video_on_cmd_19, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_20), ili9881c_hd_video_on_cmd_20, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_21), ili9881c_hd_video_on_cmd_21, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_22), ili9881c_hd_video_on_cmd_22, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_23), ili9881c_hd_video_on_cmd_23, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_24), ili9881c_hd_video_on_cmd_24, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_25), ili9881c_hd_video_on_cmd_25, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_26), ili9881c_hd_video_on_cmd_26, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_27), ili9881c_hd_video_on_cmd_27, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_28), ili9881c_hd_video_on_cmd_28, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_29), ili9881c_hd_video_on_cmd_29, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_30), ili9881c_hd_video_on_cmd_30, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_31), ili9881c_hd_video_on_cmd_31, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_32), ili9881c_hd_video_on_cmd_32, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_33), ili9881c_hd_video_on_cmd_33, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_34), ili9881c_hd_video_on_cmd_34, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_35), ili9881c_hd_video_on_cmd_35, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_36), ili9881c_hd_video_on_cmd_36, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_37), ili9881c_hd_video_on_cmd_37, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_38), ili9881c_hd_video_on_cmd_38, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_39), ili9881c_hd_video_on_cmd_39, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_40), ili9881c_hd_video_on_cmd_40, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_41), ili9881c_hd_video_on_cmd_41, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_42), ili9881c_hd_video_on_cmd_42, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_43), ili9881c_hd_video_on_cmd_43, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_44), ili9881c_hd_video_on_cmd_44, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_45), ili9881c_hd_video_on_cmd_45, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_46), ili9881c_hd_video_on_cmd_46, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_47), ili9881c_hd_video_on_cmd_47, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_48), ili9881c_hd_video_on_cmd_48, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_49), ili9881c_hd_video_on_cmd_49, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_50), ili9881c_hd_video_on_cmd_50, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_51), ili9881c_hd_video_on_cmd_51, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_52), ili9881c_hd_video_on_cmd_52, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_53), ili9881c_hd_video_on_cmd_53, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_54), ili9881c_hd_video_on_cmd_54, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_55), ili9881c_hd_video_on_cmd_55, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_56), ili9881c_hd_video_on_cmd_56, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_57), ili9881c_hd_video_on_cmd_57, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_58), ili9881c_hd_video_on_cmd_58, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_59), ili9881c_hd_video_on_cmd_59, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_60), ili9881c_hd_video_on_cmd_60, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_61), ili9881c_hd_video_on_cmd_61, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_62), ili9881c_hd_video_on_cmd_62, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_63), ili9881c_hd_video_on_cmd_63, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_64), ili9881c_hd_video_on_cmd_64, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_65), ili9881c_hd_video_on_cmd_65, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_66), ili9881c_hd_video_on_cmd_66, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_67), ili9881c_hd_video_on_cmd_67, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_68), ili9881c_hd_video_on_cmd_68, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_69), ili9881c_hd_video_on_cmd_69, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_70), ili9881c_hd_video_on_cmd_70, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_71), ili9881c_hd_video_on_cmd_71, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_72), ili9881c_hd_video_on_cmd_72, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_73), ili9881c_hd_video_on_cmd_73, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_74), ili9881c_hd_video_on_cmd_74, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_75), ili9881c_hd_video_on_cmd_75, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_76), ili9881c_hd_video_on_cmd_76, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_77), ili9881c_hd_video_on_cmd_77, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_78), ili9881c_hd_video_on_cmd_78, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_79), ili9881c_hd_video_on_cmd_79, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_80), ili9881c_hd_video_on_cmd_80, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_81), ili9881c_hd_video_on_cmd_81, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_82), ili9881c_hd_video_on_cmd_82, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_83), ili9881c_hd_video_on_cmd_83, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_84), ili9881c_hd_video_on_cmd_84, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_85), ili9881c_hd_video_on_cmd_85, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_86), ili9881c_hd_video_on_cmd_86, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_87), ili9881c_hd_video_on_cmd_87, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_88), ili9881c_hd_video_on_cmd_88, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_89), ili9881c_hd_video_on_cmd_89, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_90), ili9881c_hd_video_on_cmd_90, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_91), ili9881c_hd_video_on_cmd_91, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_92), ili9881c_hd_video_on_cmd_92, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_93), ili9881c_hd_video_on_cmd_93, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_94), ili9881c_hd_video_on_cmd_94, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_95), ili9881c_hd_video_on_cmd_95, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_96), ili9881c_hd_video_on_cmd_96, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_97), ili9881c_hd_video_on_cmd_97, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_98), ili9881c_hd_video_on_cmd_98, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_99), ili9881c_hd_video_on_cmd_99, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_100), ili9881c_hd_video_on_cmd_100, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_101), ili9881c_hd_video_on_cmd_101, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_102), ili9881c_hd_video_on_cmd_102, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_103), ili9881c_hd_video_on_cmd_103, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_104), ili9881c_hd_video_on_cmd_104, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_105), ili9881c_hd_video_on_cmd_105, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_106), ili9881c_hd_video_on_cmd_106, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_107), ili9881c_hd_video_on_cmd_107, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_108), ili9881c_hd_video_on_cmd_108, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_109), ili9881c_hd_video_on_cmd_109, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_110), ili9881c_hd_video_on_cmd_110, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_111), ili9881c_hd_video_on_cmd_111, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_112), ili9881c_hd_video_on_cmd_112, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_113), ili9881c_hd_video_on_cmd_113, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_114), ili9881c_hd_video_on_cmd_114, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_115), ili9881c_hd_video_on_cmd_115, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_116), ili9881c_hd_video_on_cmd_116, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_117), ili9881c_hd_video_on_cmd_117, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_118), ili9881c_hd_video_on_cmd_118, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_119), ili9881c_hd_video_on_cmd_119, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_120), ili9881c_hd_video_on_cmd_120, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_121), ili9881c_hd_video_on_cmd_121, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_122), ili9881c_hd_video_on_cmd_122, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_123), ili9881c_hd_video_on_cmd_123, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_124), ili9881c_hd_video_on_cmd_124, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_125), ili9881c_hd_video_on_cmd_125, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_126), ili9881c_hd_video_on_cmd_126, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_127), ili9881c_hd_video_on_cmd_127, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_128), ili9881c_hd_video_on_cmd_128, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_129), ili9881c_hd_video_on_cmd_129, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_130), ili9881c_hd_video_on_cmd_130, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_131), ili9881c_hd_video_on_cmd_131, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_132), ili9881c_hd_video_on_cmd_132, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_133), ili9881c_hd_video_on_cmd_133, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_134), ili9881c_hd_video_on_cmd_134, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_135), ili9881c_hd_video_on_cmd_135, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_136), ili9881c_hd_video_on_cmd_136, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_137), ili9881c_hd_video_on_cmd_137, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_138), ili9881c_hd_video_on_cmd_138, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_139), ili9881c_hd_video_on_cmd_139, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_140), ili9881c_hd_video_on_cmd_140, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_141), ili9881c_hd_video_on_cmd_141, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_142), ili9881c_hd_video_on_cmd_142, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_143), ili9881c_hd_video_on_cmd_143, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_144), ili9881c_hd_video_on_cmd_144, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_145), ili9881c_hd_video_on_cmd_145, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_146), ili9881c_hd_video_on_cmd_146, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_147), ili9881c_hd_video_on_cmd_147, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_148), ili9881c_hd_video_on_cmd_148, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_149), ili9881c_hd_video_on_cmd_149, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_150), ili9881c_hd_video_on_cmd_150, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_151), ili9881c_hd_video_on_cmd_151, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_152), ili9881c_hd_video_on_cmd_152, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_153), ili9881c_hd_video_on_cmd_153, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_154), ili9881c_hd_video_on_cmd_154, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_155), ili9881c_hd_video_on_cmd_155, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_156), ili9881c_hd_video_on_cmd_156, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_157), ili9881c_hd_video_on_cmd_157, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_158), ili9881c_hd_video_on_cmd_158, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_159), ili9881c_hd_video_on_cmd_159, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_160), ili9881c_hd_video_on_cmd_160, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_161), ili9881c_hd_video_on_cmd_161, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_162), ili9881c_hd_video_on_cmd_162, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_163), ili9881c_hd_video_on_cmd_163, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_164), ili9881c_hd_video_on_cmd_164, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_165), ili9881c_hd_video_on_cmd_165, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_166), ili9881c_hd_video_on_cmd_166, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_167), ili9881c_hd_video_on_cmd_167, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_168), ili9881c_hd_video_on_cmd_168, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_169), ili9881c_hd_video_on_cmd_169, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_170), ili9881c_hd_video_on_cmd_170, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_171), ili9881c_hd_video_on_cmd_171, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_172), ili9881c_hd_video_on_cmd_172, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_173), ili9881c_hd_video_on_cmd_173, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_174), ili9881c_hd_video_on_cmd_174, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_175), ili9881c_hd_video_on_cmd_175, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_176), ili9881c_hd_video_on_cmd_176, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_177), ili9881c_hd_video_on_cmd_177, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_178), ili9881c_hd_video_on_cmd_178, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_179), ili9881c_hd_video_on_cmd_179, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_180), ili9881c_hd_video_on_cmd_180, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_181), ili9881c_hd_video_on_cmd_181, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_182), ili9881c_hd_video_on_cmd_182, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_183), ili9881c_hd_video_on_cmd_183, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_184), ili9881c_hd_video_on_cmd_184, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_185), ili9881c_hd_video_on_cmd_185, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_186), ili9881c_hd_video_on_cmd_186, 0 },
	{ sizeof(ili9881c_hd_video_on_cmd_187), ili9881c_hd_video_on_cmd_187, 120 },
	{ sizeof(ili9881c_hd_video_on_cmd_188), ili9881c_hd_video_on_cmd_188, 32 },
};

static char ili9881c_hd_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char ili9881c_hd_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd ili9881c_hd_video_off_command[] = {
	{ sizeof(ili9881c_hd_video_off_cmd_0), ili9881c_hd_video_off_cmd_0, 50 },
	{ sizeof(ili9881c_hd_video_off_cmd_1), ili9881c_hd_video_off_cmd_1, 120 },
};

static struct command_state ili9881c_hd_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info ili9881c_hd_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info ili9881c_hd_video_video_panel = {
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

static struct lane_configuration ili9881c_hd_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t ili9881c_hd_video_timings[] = {
	0x8b, 0x1e, 0x14, 0x00, 0x44, 0x46, 0x18, 0x20, 0x19, 0x03, 0x04, 0x00
};

static struct panel_timing ili9881c_hd_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1c,
};

static struct panel_reset_sequence ili9881c_hd_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 20, 20 },
	.pin_direction = 2,
};

static struct backlight ili9881c_hd_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 4095,
};

static inline void panel_ili9881c_hd_video_select(struct panel_struct *panel,
						  struct msm_panel_info *pinfo,
						  struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &ili9881c_hd_video_panel_data;
	panel->panelres = &ili9881c_hd_video_panel_res;
	panel->color = &ili9881c_hd_video_color;
	panel->videopanel = &ili9881c_hd_video_video_panel;
	panel->commandpanel = &ili9881c_hd_video_command_panel;
	panel->state = &ili9881c_hd_video_state;
	panel->laneconfig = &ili9881c_hd_video_lane_config;
	panel->paneltiminginfo = &ili9881c_hd_video_timing_info;
	panel->panelresetseq = &ili9881c_hd_video_reset_seq;
	panel->backlightinfo = &ili9881c_hd_video_backlight;
	pinfo->mipi.panel_on_cmds = ili9881c_hd_video_on_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(ili9881c_hd_video_on_command);
	memcpy(phy_db->timing, ili9881c_hd_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_ILI9881C_HD_VIDEO_H_ */
