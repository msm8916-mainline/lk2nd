// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2023 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_CMI_NT35532_5P5_1080PXA_VIDEO_H_
#define _PANEL_CMI_NT35532_5P5_1080PXA_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config cmi_nt35532_5p5_1080pxa_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_cmi_nt35532_5p5_1080pxa_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution cmi_nt35532_5p5_1080pxa_video_panel_res = {
	.panel_width = 1080,
	.panel_height = 1920,
	.hfront_porch = 94,
	.hback_porch = 85,
	.hpulse_width = 12,
	.hsync_skew = 0,
	.vfront_porch = 26,
	.vback_porch = 8,
	.vpulse_width = 8,
	/* Borders not supported yet */
};

static struct color_info cmi_nt35532_5p5_1080pxa_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char cmi_nt35532_5p5_1080pxa_video_on_cmd_0[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_2[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_3[] = {
	0x02, 0x00, 0x39, 0xc0, 0x01, 0x55, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_4[] = {
	0x02, 0x00, 0x39, 0xc0, 0x02, 0x59, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_5[] = {
	0x02, 0x00, 0x39, 0xc0, 0x04, 0x0c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0x05, 0x3b, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0x06, 0x6e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0x07, 0xc6, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_9[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0d, 0xc5, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_10[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0e, 0xc5, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_11[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0f, 0xe0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0x10, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_13[] = {
	0x02, 0x00, 0x39, 0xc0, 0x11, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_14[] = {
	0x02, 0x00, 0x39, 0xc0, 0x12, 0x5a, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0x16, 0x18, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0x17, 0x18, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_17[] = {
	0x02, 0x00, 0x39, 0xc0, 0x68, 0x13, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_18[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_19[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_20[] = {
	0x02, 0x00, 0x39, 0xc0, 0x75, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_21[] = {
	0x02, 0x00, 0x39, 0xc0, 0x76, 0xea, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_22[] = {
	0x02, 0x00, 0x39, 0xc0, 0x77, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_23[] = {
	0x02, 0x00, 0x39, 0xc0, 0x78, 0xee, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_24[] = {
	0x02, 0x00, 0x39, 0xc0, 0x79, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_25[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7a, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_26[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7b, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_27[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7c, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_28[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7d, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_29[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7e, 0x2f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_30[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7f, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_31[] = {
	0x02, 0x00, 0x39, 0xc0, 0x80, 0x3e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_32[] = {
	0x02, 0x00, 0x39, 0xc0, 0x81, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_33[] = {
	0x02, 0x00, 0x39, 0xc0, 0x82, 0x4c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_34[] = {
	0x02, 0x00, 0x39, 0xc0, 0x83, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_35[] = {
	0x02, 0x00, 0x39, 0xc0, 0x84, 0x58, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_36[] = {
	0x02, 0x00, 0x39, 0xc0, 0x85, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_37[] = {
	0x02, 0x00, 0x39, 0xc0, 0x86, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_38[] = {
	0x02, 0x00, 0x39, 0xc0, 0x87, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_39[] = {
	0x02, 0x00, 0x39, 0xc0, 0x88, 0x89, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_40[] = {
	0x02, 0x00, 0x39, 0xc0, 0x89, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_41[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8a, 0xa8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_42[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8b, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_43[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8c, 0xd9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_44[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8d, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_45[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8e, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_46[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8f, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_47[] = {
	0x02, 0x00, 0x39, 0xc0, 0x90, 0x41, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_48[] = {
	0x02, 0x00, 0x39, 0xc0, 0x91, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_49[] = {
	0x02, 0x00, 0x39, 0xc0, 0x92, 0x72, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_50[] = {
	0x02, 0x00, 0x39, 0xc0, 0x93, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_51[] = {
	0x02, 0x00, 0x39, 0xc0, 0x94, 0x74, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_52[] = {
	0x02, 0x00, 0x39, 0xc0, 0x95, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_53[] = {
	0x02, 0x00, 0x39, 0xc0, 0x96, 0xa0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_54[] = {
	0x02, 0x00, 0x39, 0xc0, 0x97, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_55[] = {
	0x02, 0x00, 0x39, 0xc0, 0x98, 0xcc, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_56[] = {
	0x02, 0x00, 0x39, 0xc0, 0x99, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_57[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9a, 0xeb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_58[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9b, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_59[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9c, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_60[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9d, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_61[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9e, 0x34, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_62[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9f, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_63[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa0, 0x5e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_64[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa2, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_65[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa3, 0x66, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_66[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa4, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_67[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa5, 0x73, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_68[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa6, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_69[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa7, 0x82, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_70[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa9, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_71[] = {
	0x02, 0x00, 0x39, 0xc0, 0xaa, 0x91, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_72[] = {
	0x02, 0x00, 0x39, 0xc0, 0xab, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_73[] = {
	0x02, 0x00, 0x39, 0xc0, 0xac, 0xa4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_74[] = {
	0x02, 0x00, 0x39, 0xc0, 0xad, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_75[] = {
	0x02, 0x00, 0x39, 0xc0, 0xae, 0xbb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_76[] = {
	0x02, 0x00, 0x39, 0xc0, 0xaf, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_77[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0xcd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_78[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb1, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_79[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb2, 0xcf, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_80[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb3, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_81[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb4, 0x76, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_82[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb5, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_83[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb6, 0xda, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_84[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb7, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_85[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb8, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_86[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb9, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_87[] = {
	0x02, 0x00, 0x39, 0xc0, 0xba, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_88[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_89[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbc, 0x2f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_90[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_91[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbe, 0x3e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_92[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbf, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_93[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x4c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_94[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_95[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc2, 0x58, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_96[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc3, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_97[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_98[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_99[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0x89, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_100[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_101[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc8, 0xa8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_102[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc9, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_103[] = {
	0x02, 0x00, 0x39, 0xc0, 0xca, 0xd9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_104[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_105[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_106[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcd, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_107[] = {
	0x02, 0x00, 0x39, 0xc0, 0xce, 0x41, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_108[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcf, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_109[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd0, 0x72, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_110[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd1, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_111[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd2, 0x74, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_112[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd3, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_113[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd4, 0xa0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_114[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd5, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_115[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd6, 0xcc, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_116[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd7, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_117[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd8, 0xeb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_118[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd9, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_119[] = {
	0x02, 0x00, 0x39, 0xc0, 0xda, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_120[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdb, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_121[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdc, 0x34, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_122[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdd, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_123[] = {
	0x02, 0x00, 0x39, 0xc0, 0xde, 0x5e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_124[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdf, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_125[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe0, 0x66, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_126[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe1, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_127[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe2, 0x73, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_128[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe3, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_129[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe4, 0x82, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_130[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe5, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_131[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe6, 0x91, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_132[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe7, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_133[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe8, 0xa4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_134[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe9, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_135[] = {
	0x02, 0x00, 0x39, 0xc0, 0xea, 0xbb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_136[] = {
	0x02, 0x00, 0x39, 0xc0, 0xeb, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_137[] = {
	0x02, 0x00, 0x39, 0xc0, 0xec, 0xcd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_138[] = {
	0x02, 0x00, 0x39, 0xc0, 0xed, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_139[] = {
	0x02, 0x00, 0x39, 0xc0, 0xee, 0xcf, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_140[] = {
	0x02, 0x00, 0x39, 0xc0, 0xef, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_141[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf0, 0xea, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_142[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf1, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_143[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf2, 0xee, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_144[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf3, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_145[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf4, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_146[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf5, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_147[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf6, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_148[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf7, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_149[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf8, 0x2f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_150[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf9, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_151[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfa, 0x3e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_152[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_153[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_154[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_155[] = {
	0x02, 0x00, 0x39, 0xc0, 0x01, 0x4c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_156[] = {
	0x02, 0x00, 0x39, 0xc0, 0x02, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_157[] = {
	0x02, 0x00, 0x39, 0xc0, 0x03, 0x58, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_158[] = {
	0x02, 0x00, 0x39, 0xc0, 0x04, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_159[] = {
	0x02, 0x00, 0x39, 0xc0, 0x05, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_160[] = {
	0x02, 0x00, 0x39, 0xc0, 0x06, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_161[] = {
	0x02, 0x00, 0x39, 0xc0, 0x07, 0x89, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_162[] = {
	0x02, 0x00, 0x39, 0xc0, 0x08, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_163[] = {
	0x02, 0x00, 0x39, 0xc0, 0x09, 0xa8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_164[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0a, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_165[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0b, 0xd9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_166[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0c, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_167[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0d, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_168[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0e, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_169[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0f, 0x41, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_170[] = {
	0x02, 0x00, 0x39, 0xc0, 0x10, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_171[] = {
	0x02, 0x00, 0x39, 0xc0, 0x11, 0x72, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_172[] = {
	0x02, 0x00, 0x39, 0xc0, 0x12, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_173[] = {
	0x02, 0x00, 0x39, 0xc0, 0x13, 0x74, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_174[] = {
	0x02, 0x00, 0x39, 0xc0, 0x14, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_175[] = {
	0x02, 0x00, 0x39, 0xc0, 0x15, 0xa0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_176[] = {
	0x02, 0x00, 0x39, 0xc0, 0x16, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_177[] = {
	0x02, 0x00, 0x39, 0xc0, 0x17, 0xcc, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_178[] = {
	0x02, 0x00, 0x39, 0xc0, 0x18, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_179[] = {
	0x02, 0x00, 0x39, 0xc0, 0x19, 0xeb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_180[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1a, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_181[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1b, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_182[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1c, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_183[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1d, 0x34, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_184[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1e, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_185[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1f, 0x5e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_186[] = {
	0x02, 0x00, 0x39, 0xc0, 0x20, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_187[] = {
	0x02, 0x00, 0x39, 0xc0, 0x21, 0x66, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_188[] = {
	0x02, 0x00, 0x39, 0xc0, 0x22, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_189[] = {
	0x02, 0x00, 0x39, 0xc0, 0x23, 0x73, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_190[] = {
	0x02, 0x00, 0x39, 0xc0, 0x24, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_191[] = {
	0x02, 0x00, 0x39, 0xc0, 0x25, 0x82, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_192[] = {
	0x02, 0x00, 0x39, 0xc0, 0x26, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_193[] = {
	0x02, 0x00, 0x39, 0xc0, 0x27, 0x91, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_194[] = {
	0x02, 0x00, 0x39, 0xc0, 0x28, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_195[] = {
	0x02, 0x00, 0x39, 0xc0, 0x29, 0xa4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_196[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2a, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_197[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2b, 0xbb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_198[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2d, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_199[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2f, 0xcd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_200[] = {
	0x02, 0x00, 0x39, 0xc0, 0x30, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_201[] = {
	0x02, 0x00, 0x39, 0xc0, 0x31, 0xcf, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_202[] = {
	0x02, 0x00, 0x39, 0xc0, 0x32, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_203[] = {
	0x02, 0x00, 0x39, 0xc0, 0x33, 0x76, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_204[] = {
	0x02, 0x00, 0x39, 0xc0, 0x34, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_205[] = {
	0x02, 0x00, 0x39, 0xc0, 0x35, 0xda, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_206[] = {
	0x02, 0x00, 0x39, 0xc0, 0x36, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_207[] = {
	0x02, 0x00, 0x39, 0xc0, 0x37, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_208[] = {
	0x02, 0x00, 0x39, 0xc0, 0x38, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_209[] = {
	0x02, 0x00, 0x39, 0xc0, 0x39, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_210[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3a, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_211[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3b, 0x2f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_212[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3d, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_213[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3f, 0x3e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_214[] = {
	0x02, 0x00, 0x39, 0xc0, 0x40, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_215[] = {
	0x02, 0x00, 0x39, 0xc0, 0x41, 0x4c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_216[] = {
	0x02, 0x00, 0x39, 0xc0, 0x42, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_217[] = {
	0x02, 0x00, 0x39, 0xc0, 0x43, 0x58, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_218[] = {
	0x02, 0x00, 0x39, 0xc0, 0x44, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_219[] = {
	0x02, 0x00, 0x39, 0xc0, 0x45, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_220[] = {
	0x02, 0x00, 0x39, 0xc0, 0x46, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_221[] = {
	0x02, 0x00, 0x39, 0xc0, 0x47, 0x89, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_222[] = {
	0x02, 0x00, 0x39, 0xc0, 0x48, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_223[] = {
	0x02, 0x00, 0x39, 0xc0, 0x49, 0xa8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_224[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4a, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_225[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4b, 0xd9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_226[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4c, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_227[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4d, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_228[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4e, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_229[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4f, 0x41, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_230[] = {
	0x02, 0x00, 0x39, 0xc0, 0x50, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_231[] = {
	0x02, 0x00, 0x39, 0xc0, 0x51, 0x72, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_232[] = {
	0x02, 0x00, 0x39, 0xc0, 0x52, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_233[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x74, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_234[] = {
	0x02, 0x00, 0x39, 0xc0, 0x54, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_235[] = {
	0x02, 0x00, 0x39, 0xc0, 0x55, 0xa0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_236[] = {
	0x02, 0x00, 0x39, 0xc0, 0x56, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_237[] = {
	0x02, 0x00, 0x39, 0xc0, 0x58, 0xcc, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_238[] = {
	0x02, 0x00, 0x39, 0xc0, 0x59, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_239[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5a, 0xeb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_240[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5b, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_241[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5c, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_242[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5d, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_243[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5e, 0x34, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_244[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5f, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_245[] = {
	0x02, 0x00, 0x39, 0xc0, 0x60, 0x5e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_246[] = {
	0x02, 0x00, 0x39, 0xc0, 0x61, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_247[] = {
	0x02, 0x00, 0x39, 0xc0, 0x62, 0x66, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_248[] = {
	0x02, 0x00, 0x39, 0xc0, 0x63, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_249[] = {
	0x02, 0x00, 0x39, 0xc0, 0x64, 0x73, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_250[] = {
	0x02, 0x00, 0x39, 0xc0, 0x65, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_251[] = {
	0x02, 0x00, 0x39, 0xc0, 0x66, 0x82, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_252[] = {
	0x02, 0x00, 0x39, 0xc0, 0x67, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_253[] = {
	0x02, 0x00, 0x39, 0xc0, 0x68, 0x91, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_254[] = {
	0x02, 0x00, 0x39, 0xc0, 0x69, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_255[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6a, 0xa4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_256[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6b, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_257[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6c, 0xbb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_258[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6d, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_259[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6e, 0xcd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_260[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_261[] = {
	0x02, 0x00, 0x39, 0xc0, 0x70, 0xcf, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_262[] = {
	0x02, 0x00, 0x39, 0xc0, 0x71, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_263[] = {
	0x02, 0x00, 0x39, 0xc0, 0x72, 0xea, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_264[] = {
	0x02, 0x00, 0x39, 0xc0, 0x73, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_265[] = {
	0x02, 0x00, 0x39, 0xc0, 0x74, 0xee, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_266[] = {
	0x02, 0x00, 0x39, 0xc0, 0x75, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_267[] = {
	0x02, 0x00, 0x39, 0xc0, 0x76, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_268[] = {
	0x02, 0x00, 0x39, 0xc0, 0x77, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_269[] = {
	0x02, 0x00, 0x39, 0xc0, 0x78, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_270[] = {
	0x02, 0x00, 0x39, 0xc0, 0x79, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_271[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7a, 0x2f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_272[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7b, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_273[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7c, 0x3e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_274[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7d, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_275[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7e, 0x4c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_276[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7f, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_277[] = {
	0x02, 0x00, 0x39, 0xc0, 0x80, 0x58, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_278[] = {
	0x02, 0x00, 0x39, 0xc0, 0x81, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_279[] = {
	0x02, 0x00, 0x39, 0xc0, 0x82, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_280[] = {
	0x02, 0x00, 0x39, 0xc0, 0x83, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_281[] = {
	0x02, 0x00, 0x39, 0xc0, 0x84, 0x89, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_282[] = {
	0x02, 0x00, 0x39, 0xc0, 0x85, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_283[] = {
	0x02, 0x00, 0x39, 0xc0, 0x86, 0xa8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_284[] = {
	0x02, 0x00, 0x39, 0xc0, 0x87, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_285[] = {
	0x02, 0x00, 0x39, 0xc0, 0x88, 0xd9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_286[] = {
	0x02, 0x00, 0x39, 0xc0, 0x89, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_287[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8a, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_288[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8b, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_289[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8c, 0x41, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_290[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8d, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_291[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8e, 0x72, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_292[] = {
	0x02, 0x00, 0x39, 0xc0, 0x8f, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_293[] = {
	0x02, 0x00, 0x39, 0xc0, 0x90, 0x74, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_294[] = {
	0x02, 0x00, 0x39, 0xc0, 0x91, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_295[] = {
	0x02, 0x00, 0x39, 0xc0, 0x92, 0xa0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_296[] = {
	0x02, 0x00, 0x39, 0xc0, 0x93, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_297[] = {
	0x02, 0x00, 0x39, 0xc0, 0x94, 0xcc, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_298[] = {
	0x02, 0x00, 0x39, 0xc0, 0x95, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_299[] = {
	0x02, 0x00, 0x39, 0xc0, 0x96, 0xeb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_300[] = {
	0x02, 0x00, 0x39, 0xc0, 0x97, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_301[] = {
	0x02, 0x00, 0x39, 0xc0, 0x98, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_302[] = {
	0x02, 0x00, 0x39, 0xc0, 0x99, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_303[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9a, 0x34, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_304[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9b, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_305[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9c, 0x5e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_306[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9d, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_307[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9e, 0x66, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_308[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9f, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_309[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa0, 0x73, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_310[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa2, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_311[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa3, 0x82, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_312[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa4, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_313[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa5, 0x91, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_314[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa6, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_315[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa7, 0xa4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_316[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa9, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_317[] = {
	0x02, 0x00, 0x39, 0xc0, 0xaa, 0xbb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_318[] = {
	0x02, 0x00, 0x39, 0xc0, 0xab, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_319[] = {
	0x02, 0x00, 0x39, 0xc0, 0xac, 0xcd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_320[] = {
	0x02, 0x00, 0x39, 0xc0, 0xad, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_321[] = {
	0x02, 0x00, 0x39, 0xc0, 0xae, 0xcf, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_322[] = {
	0x02, 0x00, 0x39, 0xc0, 0xaf, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_323[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0x76, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_324[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb1, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_325[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb2, 0xda, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_326[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb3, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_327[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb4, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_328[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb5, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_329[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb6, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_330[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb7, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_331[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb8, 0x2f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_332[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb9, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_333[] = {
	0x02, 0x00, 0x39, 0xc0, 0xba, 0x3e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_334[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_335[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbc, 0x4c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_336[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_337[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbe, 0x58, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_338[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbf, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_339[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x64, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_340[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_341[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc2, 0x89, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_342[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc3, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_343[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0xa8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_344[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_345[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0xd9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_346[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_347[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc8, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_348[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc9, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_349[] = {
	0x02, 0x00, 0x39, 0xc0, 0xca, 0x41, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_350[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcb, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_351[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x72, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_352[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcd, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_353[] = {
	0x02, 0x00, 0x39, 0xc0, 0xce, 0x74, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_354[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcf, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_355[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd0, 0xa0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_356[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd1, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_357[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd2, 0xcc, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_358[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd3, 0x02, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_359[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd4, 0xeb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_360[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd5, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_361[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd6, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_362[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd7, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_363[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd8, 0x34, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_364[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd9, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_365[] = {
	0x02, 0x00, 0x39, 0xc0, 0xda, 0x5e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_366[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdb, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_367[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdc, 0x66, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_368[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdd, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_369[] = {
	0x02, 0x00, 0x39, 0xc0, 0xde, 0x73, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_370[] = {
	0x02, 0x00, 0x39, 0xc0, 0xdf, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_371[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe0, 0x82, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_372[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe1, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_373[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe2, 0x91, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_374[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe3, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_375[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe4, 0xa4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_376[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe5, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_377[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe6, 0xbb, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_378[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe7, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_379[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe8, 0xcd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_380[] = {
	0x02, 0x00, 0x39, 0xc0, 0xe9, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_381[] = {
	0x02, 0x00, 0x39, 0xc0, 0xea, 0xcf, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_382[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x04, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_383[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_384[] = {
	0x02, 0x00, 0x39, 0xc0, 0x13, 0xf3, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_385[] = {
	0x02, 0x00, 0x39, 0xc0, 0x14, 0xf2, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_386[] = {
	0x02, 0x00, 0x39, 0xc0, 0x15, 0xf1, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_387[] = {
	0x02, 0x00, 0x39, 0xc0, 0x16, 0xf0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_388[] = {
	0x02, 0x00, 0x39, 0xc0, 0x21, 0xff, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_389[] = {
	0x02, 0x00, 0x39, 0xc0, 0x22, 0xf4, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_390[] = {
	0x02, 0x00, 0x39, 0xc0, 0x23, 0xe9, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_391[] = {
	0x02, 0x00, 0x39, 0xc0, 0x24, 0xde, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_392[] = {
	0x02, 0x00, 0x39, 0xc0, 0x25, 0xd3, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_393[] = {
	0x02, 0x00, 0x39, 0xc0, 0x26, 0xc8, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_394[] = {
	0x02, 0x00, 0x39, 0xc0, 0x27, 0xbd, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_395[] = {
	0x02, 0x00, 0x39, 0xc0, 0x28, 0xb2, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_396[] = {
	0x02, 0x00, 0x39, 0xc0, 0x29, 0xa7, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_397[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2a, 0x9c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_398[] = {
	0x02, 0x00, 0x39, 0xc0, 0x07, 0x20, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_399[] = {
	0x02, 0x00, 0x39, 0xc0, 0x08, 0x07, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_400[] = {
	0x02, 0x00, 0x39, 0xc0, 0x46, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_401[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_402[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_403[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_404[] = {
	0x02, 0x00, 0x39, 0xc0, 0x01, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_405[] = {
	0x02, 0x00, 0x39, 0xc0, 0x02, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_406[] = {
	0x02, 0x00, 0x39, 0xc0, 0x03, 0x06, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_407[] = {
	0x02, 0x00, 0x39, 0xc0, 0x04, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_408[] = {
	0x02, 0x00, 0x39, 0xc0, 0x05, 0x16, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_409[] = {
	0x02, 0x00, 0x39, 0xc0, 0x06, 0x18, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_410[] = {
	0x02, 0x00, 0x39, 0xc0, 0x07, 0x1a, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_411[] = {
	0x02, 0x00, 0x39, 0xc0, 0x08, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_412[] = {
	0x02, 0x00, 0x39, 0xc0, 0x09, 0x1e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_413[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0a, 0x20, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_414[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0b, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_415[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0c, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_416[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0d, 0x26, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_417[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0e, 0x28, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_418[] = {
	0x02, 0x00, 0x39, 0xc0, 0x0f, 0x08, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_419[] = {
	0x02, 0x00, 0x39, 0xc0, 0x10, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_420[] = {
	0x02, 0x00, 0x39, 0xc0, 0x11, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_421[] = {
	0x02, 0x00, 0x39, 0xc0, 0x12, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_422[] = {
	0x02, 0x00, 0x39, 0xc0, 0x13, 0x0e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_423[] = {
	0x02, 0x00, 0x39, 0xc0, 0x14, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_424[] = {
	0x02, 0x00, 0x39, 0xc0, 0x15, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_425[] = {
	0x02, 0x00, 0x39, 0xc0, 0x16, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_426[] = {
	0x02, 0x00, 0x39, 0xc0, 0x17, 0x07, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_427[] = {
	0x02, 0x00, 0x39, 0xc0, 0x18, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_428[] = {
	0x02, 0x00, 0x39, 0xc0, 0x19, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_429[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1a, 0x19, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_430[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1b, 0x1b, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_431[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1c, 0x1d, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_432[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1d, 0x1f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_433[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1e, 0x21, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_434[] = {
	0x02, 0x00, 0x39, 0xc0, 0x1f, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_435[] = {
	0x02, 0x00, 0x39, 0xc0, 0x20, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_436[] = {
	0x02, 0x00, 0x39, 0xc0, 0x21, 0x26, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_437[] = {
	0x02, 0x00, 0x39, 0xc0, 0x22, 0x28, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_438[] = {
	0x02, 0x00, 0x39, 0xc0, 0x23, 0x09, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_439[] = {
	0x02, 0x00, 0x39, 0xc0, 0x24, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_440[] = {
	0x02, 0x00, 0x39, 0xc0, 0x25, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_441[] = {
	0x02, 0x00, 0x39, 0xc0, 0x26, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_442[] = {
	0x02, 0x00, 0x39, 0xc0, 0x27, 0x0f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_443[] = {
	0x02, 0x00, 0x39, 0xc0, 0x28, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_444[] = {
	0x02, 0x00, 0x39, 0xc0, 0x29, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_445[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2a, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_446[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2b, 0x09, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_447[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2d, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_448[] = {
	0x02, 0x00, 0x39, 0xc0, 0x2f, 0x19, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_449[] = {
	0x02, 0x00, 0x39, 0xc0, 0x30, 0x17, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_450[] = {
	0x02, 0x00, 0x39, 0xc0, 0x31, 0x21, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_451[] = {
	0x02, 0x00, 0x39, 0xc0, 0x32, 0x1f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_452[] = {
	0x02, 0x00, 0x39, 0xc0, 0x33, 0x1d, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_453[] = {
	0x02, 0x00, 0x39, 0xc0, 0x34, 0x1b, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_454[] = {
	0x02, 0x00, 0x39, 0xc0, 0x35, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_455[] = {
	0x02, 0x00, 0x39, 0xc0, 0x36, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_456[] = {
	0x02, 0x00, 0x39, 0xc0, 0x37, 0x26, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_457[] = {
	0x02, 0x00, 0x39, 0xc0, 0x38, 0x28, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_458[] = {
	0x02, 0x00, 0x39, 0xc0, 0x39, 0x07, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_459[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3a, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_460[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3b, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_461[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3d, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_462[] = {
	0x02, 0x00, 0x39, 0xc0, 0x3f, 0x0f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_463[] = {
	0x02, 0x00, 0x39, 0xc0, 0x40, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_464[] = {
	0x02, 0x00, 0x39, 0xc0, 0x41, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_465[] = {
	0x02, 0x00, 0x39, 0xc0, 0x42, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_466[] = {
	0x02, 0x00, 0x39, 0xc0, 0x43, 0x08, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_467[] = {
	0x02, 0x00, 0x39, 0xc0, 0x44, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_468[] = {
	0x02, 0x00, 0x39, 0xc0, 0x45, 0x18, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_469[] = {
	0x02, 0x00, 0x39, 0xc0, 0x46, 0x16, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_470[] = {
	0x02, 0x00, 0x39, 0xc0, 0x47, 0x20, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_471[] = {
	0x02, 0x00, 0x39, 0xc0, 0x48, 0x1e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_472[] = {
	0x02, 0x00, 0x39, 0xc0, 0x49, 0x1c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_473[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4a, 0x1a, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_474[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4b, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_475[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4c, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_476[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4d, 0x26, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_477[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4e, 0x28, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_478[] = {
	0x02, 0x00, 0x39, 0xc0, 0x4f, 0x06, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_479[] = {
	0x02, 0x00, 0x39, 0xc0, 0x50, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_480[] = {
	0x02, 0x00, 0x39, 0xc0, 0x51, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_481[] = {
	0x02, 0x00, 0x39, 0xc0, 0x52, 0x38, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_482[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x0e, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_483[] = {
	0x02, 0x00, 0x39, 0xc0, 0x54, 0x07, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_484[] = {
	0x02, 0x00, 0x39, 0xc0, 0x55, 0x19, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_485[] = {
	0x02, 0x00, 0x39, 0xc0, 0x59, 0x24, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_486[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5b, 0x4f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_487[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5c, 0x2c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_488[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5d, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_489[] = {
	0x02, 0x00, 0x39, 0xc0, 0x5e, 0x22, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_490[] = {
	0x02, 0x00, 0x39, 0xc0, 0x62, 0x21, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_491[] = {
	0x02, 0x00, 0x39, 0xc0, 0x63, 0x69, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_492[] = {
	0x02, 0x00, 0x39, 0xc0, 0x64, 0x12, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_493[] = {
	0x02, 0x00, 0x39, 0xc0, 0x66, 0x57, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_494[] = {
	0x02, 0x00, 0x39, 0xc0, 0x67, 0x11, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_495[] = {
	0x02, 0x00, 0x39, 0xc0, 0x68, 0x2b, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_496[] = {
	0x02, 0x00, 0x39, 0xc0, 0x69, 0x12, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_497[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6a, 0x05, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_498[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6b, 0x29, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_499[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6c, 0x08, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_500[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6d, 0x18, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_501[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x3c, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_502[] = {
	0x02, 0x00, 0x39, 0xc0, 0x70, 0x03, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_503[] = {
	0x02, 0x00, 0x39, 0xc0, 0x72, 0x22, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_504[] = {
	0x02, 0x00, 0x39, 0xc0, 0x73, 0x22, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_505[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7d, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_506[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7e, 0xaa, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_507[] = {
	0x02, 0x00, 0x39, 0xc0, 0x7f, 0xaa, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_508[] = {
	0x02, 0x00, 0x39, 0xc0, 0x80, 0xaa, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_509[] = {
	0x02, 0x00, 0x39, 0xc0, 0x81, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_510[] = {
	0x02, 0x00, 0x39, 0xc0, 0x85, 0x3f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_511[] = {
	0x02, 0x00, 0x39, 0xc0, 0x86, 0x3f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_512[] = {
	0x02, 0x00, 0x39, 0xc0, 0xa2, 0xb0, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_513[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbd, 0xa6, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_514[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbe, 0x08, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_515[] = {
	0x02, 0x00, 0x39, 0xc0, 0xbf, 0x12, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_516[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc8, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_517[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc9, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_518[] = {
	0x02, 0x00, 0x39, 0xc0, 0xca, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_519[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_520[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcc, 0x09, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_521[] = {
	0x02, 0x00, 0x39, 0xc0, 0xce, 0x18, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_522[] = {
	0x02, 0x00, 0x39, 0xc0, 0xcf, 0x88, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_523[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd0, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_524[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd1, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_525[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd2, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_526[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd3, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_527[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd4, 0x40, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_528[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd6, 0x22, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_529[] = {
	0x02, 0x00, 0x39, 0xc0, 0x90, 0x7b, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_530[] = {
	0x02, 0x00, 0x39, 0xc0, 0x91, 0x10, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_531[] = {
	0x02, 0x00, 0x39, 0xc0, 0x92, 0x10, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_532[] = {
	0x02, 0x00, 0x39, 0xc0, 0x98, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_533[] = {
	0x02, 0x00, 0x39, 0xc0, 0x99, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_534[] = {
	0x02, 0x00, 0x39, 0xc0, 0x9f, 0x0f, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_535[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_536[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_537[] = {
	0x02, 0x00, 0x39, 0xc0, 0xba, 0xc3, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_538[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd3, 0x10, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_539[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd4, 0x1a, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_540[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd5, 0x14, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_541[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd6, 0x14, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_542[] = {
	0x5e, 0x28, 0x15, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_543[] = {
	0x51, 0x00, 0x15, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_544[] = {
	0x53, 0x24, 0x15, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_545[] = {
	0x55, 0x01, 0x15, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_546[] = {
	0x11, 0x00, 0x05, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_547[] = {
	0x29, 0x00, 0x05, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_on_cmd_548[] = {
	0x51, 0xff, 0x15, 0x80	/* Set brightness to maximum 255 */
};

static struct mipi_dsi_cmd cmi_nt35532_5p5_1080pxa_video_on_command[] = {
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_0), cmi_nt35532_5p5_1080pxa_video_on_cmd_0, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_1), cmi_nt35532_5p5_1080pxa_video_on_cmd_1, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_2), cmi_nt35532_5p5_1080pxa_video_on_cmd_2, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_3), cmi_nt35532_5p5_1080pxa_video_on_cmd_3, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_4), cmi_nt35532_5p5_1080pxa_video_on_cmd_4, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_5), cmi_nt35532_5p5_1080pxa_video_on_cmd_5, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_6), cmi_nt35532_5p5_1080pxa_video_on_cmd_6, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_7), cmi_nt35532_5p5_1080pxa_video_on_cmd_7, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_8), cmi_nt35532_5p5_1080pxa_video_on_cmd_8, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_9), cmi_nt35532_5p5_1080pxa_video_on_cmd_9, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_10), cmi_nt35532_5p5_1080pxa_video_on_cmd_10, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_11), cmi_nt35532_5p5_1080pxa_video_on_cmd_11, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_12), cmi_nt35532_5p5_1080pxa_video_on_cmd_12, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_13), cmi_nt35532_5p5_1080pxa_video_on_cmd_13, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_14), cmi_nt35532_5p5_1080pxa_video_on_cmd_14, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_15), cmi_nt35532_5p5_1080pxa_video_on_cmd_15, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_16), cmi_nt35532_5p5_1080pxa_video_on_cmd_16, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_17), cmi_nt35532_5p5_1080pxa_video_on_cmd_17, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_18), cmi_nt35532_5p5_1080pxa_video_on_cmd_18, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_19), cmi_nt35532_5p5_1080pxa_video_on_cmd_19, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_20), cmi_nt35532_5p5_1080pxa_video_on_cmd_20, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_21), cmi_nt35532_5p5_1080pxa_video_on_cmd_21, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_22), cmi_nt35532_5p5_1080pxa_video_on_cmd_22, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_23), cmi_nt35532_5p5_1080pxa_video_on_cmd_23, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_24), cmi_nt35532_5p5_1080pxa_video_on_cmd_24, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_25), cmi_nt35532_5p5_1080pxa_video_on_cmd_25, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_26), cmi_nt35532_5p5_1080pxa_video_on_cmd_26, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_27), cmi_nt35532_5p5_1080pxa_video_on_cmd_27, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_28), cmi_nt35532_5p5_1080pxa_video_on_cmd_28, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_29), cmi_nt35532_5p5_1080pxa_video_on_cmd_29, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_30), cmi_nt35532_5p5_1080pxa_video_on_cmd_30, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_31), cmi_nt35532_5p5_1080pxa_video_on_cmd_31, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_32), cmi_nt35532_5p5_1080pxa_video_on_cmd_32, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_33), cmi_nt35532_5p5_1080pxa_video_on_cmd_33, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_34), cmi_nt35532_5p5_1080pxa_video_on_cmd_34, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_35), cmi_nt35532_5p5_1080pxa_video_on_cmd_35, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_36), cmi_nt35532_5p5_1080pxa_video_on_cmd_36, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_37), cmi_nt35532_5p5_1080pxa_video_on_cmd_37, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_38), cmi_nt35532_5p5_1080pxa_video_on_cmd_38, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_39), cmi_nt35532_5p5_1080pxa_video_on_cmd_39, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_40), cmi_nt35532_5p5_1080pxa_video_on_cmd_40, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_41), cmi_nt35532_5p5_1080pxa_video_on_cmd_41, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_42), cmi_nt35532_5p5_1080pxa_video_on_cmd_42, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_43), cmi_nt35532_5p5_1080pxa_video_on_cmd_43, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_44), cmi_nt35532_5p5_1080pxa_video_on_cmd_44, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_45), cmi_nt35532_5p5_1080pxa_video_on_cmd_45, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_46), cmi_nt35532_5p5_1080pxa_video_on_cmd_46, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_47), cmi_nt35532_5p5_1080pxa_video_on_cmd_47, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_48), cmi_nt35532_5p5_1080pxa_video_on_cmd_48, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_49), cmi_nt35532_5p5_1080pxa_video_on_cmd_49, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_50), cmi_nt35532_5p5_1080pxa_video_on_cmd_50, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_51), cmi_nt35532_5p5_1080pxa_video_on_cmd_51, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_52), cmi_nt35532_5p5_1080pxa_video_on_cmd_52, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_53), cmi_nt35532_5p5_1080pxa_video_on_cmd_53, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_54), cmi_nt35532_5p5_1080pxa_video_on_cmd_54, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_55), cmi_nt35532_5p5_1080pxa_video_on_cmd_55, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_56), cmi_nt35532_5p5_1080pxa_video_on_cmd_56, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_57), cmi_nt35532_5p5_1080pxa_video_on_cmd_57, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_58), cmi_nt35532_5p5_1080pxa_video_on_cmd_58, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_59), cmi_nt35532_5p5_1080pxa_video_on_cmd_59, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_60), cmi_nt35532_5p5_1080pxa_video_on_cmd_60, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_61), cmi_nt35532_5p5_1080pxa_video_on_cmd_61, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_62), cmi_nt35532_5p5_1080pxa_video_on_cmd_62, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_63), cmi_nt35532_5p5_1080pxa_video_on_cmd_63, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_64), cmi_nt35532_5p5_1080pxa_video_on_cmd_64, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_65), cmi_nt35532_5p5_1080pxa_video_on_cmd_65, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_66), cmi_nt35532_5p5_1080pxa_video_on_cmd_66, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_67), cmi_nt35532_5p5_1080pxa_video_on_cmd_67, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_68), cmi_nt35532_5p5_1080pxa_video_on_cmd_68, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_69), cmi_nt35532_5p5_1080pxa_video_on_cmd_69, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_70), cmi_nt35532_5p5_1080pxa_video_on_cmd_70, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_71), cmi_nt35532_5p5_1080pxa_video_on_cmd_71, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_72), cmi_nt35532_5p5_1080pxa_video_on_cmd_72, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_73), cmi_nt35532_5p5_1080pxa_video_on_cmd_73, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_74), cmi_nt35532_5p5_1080pxa_video_on_cmd_74, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_75), cmi_nt35532_5p5_1080pxa_video_on_cmd_75, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_76), cmi_nt35532_5p5_1080pxa_video_on_cmd_76, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_77), cmi_nt35532_5p5_1080pxa_video_on_cmd_77, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_78), cmi_nt35532_5p5_1080pxa_video_on_cmd_78, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_79), cmi_nt35532_5p5_1080pxa_video_on_cmd_79, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_80), cmi_nt35532_5p5_1080pxa_video_on_cmd_80, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_81), cmi_nt35532_5p5_1080pxa_video_on_cmd_81, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_82), cmi_nt35532_5p5_1080pxa_video_on_cmd_82, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_83), cmi_nt35532_5p5_1080pxa_video_on_cmd_83, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_84), cmi_nt35532_5p5_1080pxa_video_on_cmd_84, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_85), cmi_nt35532_5p5_1080pxa_video_on_cmd_85, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_86), cmi_nt35532_5p5_1080pxa_video_on_cmd_86, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_87), cmi_nt35532_5p5_1080pxa_video_on_cmd_87, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_88), cmi_nt35532_5p5_1080pxa_video_on_cmd_88, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_89), cmi_nt35532_5p5_1080pxa_video_on_cmd_89, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_90), cmi_nt35532_5p5_1080pxa_video_on_cmd_90, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_91), cmi_nt35532_5p5_1080pxa_video_on_cmd_91, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_92), cmi_nt35532_5p5_1080pxa_video_on_cmd_92, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_93), cmi_nt35532_5p5_1080pxa_video_on_cmd_93, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_94), cmi_nt35532_5p5_1080pxa_video_on_cmd_94, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_95), cmi_nt35532_5p5_1080pxa_video_on_cmd_95, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_96), cmi_nt35532_5p5_1080pxa_video_on_cmd_96, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_97), cmi_nt35532_5p5_1080pxa_video_on_cmd_97, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_98), cmi_nt35532_5p5_1080pxa_video_on_cmd_98, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_99), cmi_nt35532_5p5_1080pxa_video_on_cmd_99, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_100), cmi_nt35532_5p5_1080pxa_video_on_cmd_100, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_101), cmi_nt35532_5p5_1080pxa_video_on_cmd_101, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_102), cmi_nt35532_5p5_1080pxa_video_on_cmd_102, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_103), cmi_nt35532_5p5_1080pxa_video_on_cmd_103, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_104), cmi_nt35532_5p5_1080pxa_video_on_cmd_104, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_105), cmi_nt35532_5p5_1080pxa_video_on_cmd_105, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_106), cmi_nt35532_5p5_1080pxa_video_on_cmd_106, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_107), cmi_nt35532_5p5_1080pxa_video_on_cmd_107, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_108), cmi_nt35532_5p5_1080pxa_video_on_cmd_108, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_109), cmi_nt35532_5p5_1080pxa_video_on_cmd_109, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_110), cmi_nt35532_5p5_1080pxa_video_on_cmd_110, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_111), cmi_nt35532_5p5_1080pxa_video_on_cmd_111, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_112), cmi_nt35532_5p5_1080pxa_video_on_cmd_112, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_113), cmi_nt35532_5p5_1080pxa_video_on_cmd_113, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_114), cmi_nt35532_5p5_1080pxa_video_on_cmd_114, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_115), cmi_nt35532_5p5_1080pxa_video_on_cmd_115, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_116), cmi_nt35532_5p5_1080pxa_video_on_cmd_116, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_117), cmi_nt35532_5p5_1080pxa_video_on_cmd_117, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_118), cmi_nt35532_5p5_1080pxa_video_on_cmd_118, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_119), cmi_nt35532_5p5_1080pxa_video_on_cmd_119, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_120), cmi_nt35532_5p5_1080pxa_video_on_cmd_120, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_121), cmi_nt35532_5p5_1080pxa_video_on_cmd_121, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_122), cmi_nt35532_5p5_1080pxa_video_on_cmd_122, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_123), cmi_nt35532_5p5_1080pxa_video_on_cmd_123, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_124), cmi_nt35532_5p5_1080pxa_video_on_cmd_124, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_125), cmi_nt35532_5p5_1080pxa_video_on_cmd_125, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_126), cmi_nt35532_5p5_1080pxa_video_on_cmd_126, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_127), cmi_nt35532_5p5_1080pxa_video_on_cmd_127, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_128), cmi_nt35532_5p5_1080pxa_video_on_cmd_128, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_129), cmi_nt35532_5p5_1080pxa_video_on_cmd_129, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_130), cmi_nt35532_5p5_1080pxa_video_on_cmd_130, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_131), cmi_nt35532_5p5_1080pxa_video_on_cmd_131, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_132), cmi_nt35532_5p5_1080pxa_video_on_cmd_132, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_133), cmi_nt35532_5p5_1080pxa_video_on_cmd_133, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_134), cmi_nt35532_5p5_1080pxa_video_on_cmd_134, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_135), cmi_nt35532_5p5_1080pxa_video_on_cmd_135, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_136), cmi_nt35532_5p5_1080pxa_video_on_cmd_136, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_137), cmi_nt35532_5p5_1080pxa_video_on_cmd_137, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_138), cmi_nt35532_5p5_1080pxa_video_on_cmd_138, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_139), cmi_nt35532_5p5_1080pxa_video_on_cmd_139, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_140), cmi_nt35532_5p5_1080pxa_video_on_cmd_140, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_141), cmi_nt35532_5p5_1080pxa_video_on_cmd_141, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_142), cmi_nt35532_5p5_1080pxa_video_on_cmd_142, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_143), cmi_nt35532_5p5_1080pxa_video_on_cmd_143, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_144), cmi_nt35532_5p5_1080pxa_video_on_cmd_144, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_145), cmi_nt35532_5p5_1080pxa_video_on_cmd_145, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_146), cmi_nt35532_5p5_1080pxa_video_on_cmd_146, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_147), cmi_nt35532_5p5_1080pxa_video_on_cmd_147, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_148), cmi_nt35532_5p5_1080pxa_video_on_cmd_148, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_149), cmi_nt35532_5p5_1080pxa_video_on_cmd_149, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_150), cmi_nt35532_5p5_1080pxa_video_on_cmd_150, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_151), cmi_nt35532_5p5_1080pxa_video_on_cmd_151, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_152), cmi_nt35532_5p5_1080pxa_video_on_cmd_152, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_153), cmi_nt35532_5p5_1080pxa_video_on_cmd_153, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_154), cmi_nt35532_5p5_1080pxa_video_on_cmd_154, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_155), cmi_nt35532_5p5_1080pxa_video_on_cmd_155, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_156), cmi_nt35532_5p5_1080pxa_video_on_cmd_156, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_157), cmi_nt35532_5p5_1080pxa_video_on_cmd_157, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_158), cmi_nt35532_5p5_1080pxa_video_on_cmd_158, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_159), cmi_nt35532_5p5_1080pxa_video_on_cmd_159, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_160), cmi_nt35532_5p5_1080pxa_video_on_cmd_160, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_161), cmi_nt35532_5p5_1080pxa_video_on_cmd_161, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_162), cmi_nt35532_5p5_1080pxa_video_on_cmd_162, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_163), cmi_nt35532_5p5_1080pxa_video_on_cmd_163, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_164), cmi_nt35532_5p5_1080pxa_video_on_cmd_164, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_165), cmi_nt35532_5p5_1080pxa_video_on_cmd_165, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_166), cmi_nt35532_5p5_1080pxa_video_on_cmd_166, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_167), cmi_nt35532_5p5_1080pxa_video_on_cmd_167, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_168), cmi_nt35532_5p5_1080pxa_video_on_cmd_168, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_169), cmi_nt35532_5p5_1080pxa_video_on_cmd_169, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_170), cmi_nt35532_5p5_1080pxa_video_on_cmd_170, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_171), cmi_nt35532_5p5_1080pxa_video_on_cmd_171, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_172), cmi_nt35532_5p5_1080pxa_video_on_cmd_172, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_173), cmi_nt35532_5p5_1080pxa_video_on_cmd_173, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_174), cmi_nt35532_5p5_1080pxa_video_on_cmd_174, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_175), cmi_nt35532_5p5_1080pxa_video_on_cmd_175, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_176), cmi_nt35532_5p5_1080pxa_video_on_cmd_176, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_177), cmi_nt35532_5p5_1080pxa_video_on_cmd_177, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_178), cmi_nt35532_5p5_1080pxa_video_on_cmd_178, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_179), cmi_nt35532_5p5_1080pxa_video_on_cmd_179, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_180), cmi_nt35532_5p5_1080pxa_video_on_cmd_180, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_181), cmi_nt35532_5p5_1080pxa_video_on_cmd_181, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_182), cmi_nt35532_5p5_1080pxa_video_on_cmd_182, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_183), cmi_nt35532_5p5_1080pxa_video_on_cmd_183, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_184), cmi_nt35532_5p5_1080pxa_video_on_cmd_184, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_185), cmi_nt35532_5p5_1080pxa_video_on_cmd_185, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_186), cmi_nt35532_5p5_1080pxa_video_on_cmd_186, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_187), cmi_nt35532_5p5_1080pxa_video_on_cmd_187, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_188), cmi_nt35532_5p5_1080pxa_video_on_cmd_188, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_189), cmi_nt35532_5p5_1080pxa_video_on_cmd_189, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_190), cmi_nt35532_5p5_1080pxa_video_on_cmd_190, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_191), cmi_nt35532_5p5_1080pxa_video_on_cmd_191, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_192), cmi_nt35532_5p5_1080pxa_video_on_cmd_192, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_193), cmi_nt35532_5p5_1080pxa_video_on_cmd_193, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_194), cmi_nt35532_5p5_1080pxa_video_on_cmd_194, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_195), cmi_nt35532_5p5_1080pxa_video_on_cmd_195, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_196), cmi_nt35532_5p5_1080pxa_video_on_cmd_196, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_197), cmi_nt35532_5p5_1080pxa_video_on_cmd_197, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_198), cmi_nt35532_5p5_1080pxa_video_on_cmd_198, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_199), cmi_nt35532_5p5_1080pxa_video_on_cmd_199, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_200), cmi_nt35532_5p5_1080pxa_video_on_cmd_200, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_201), cmi_nt35532_5p5_1080pxa_video_on_cmd_201, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_202), cmi_nt35532_5p5_1080pxa_video_on_cmd_202, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_203), cmi_nt35532_5p5_1080pxa_video_on_cmd_203, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_204), cmi_nt35532_5p5_1080pxa_video_on_cmd_204, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_205), cmi_nt35532_5p5_1080pxa_video_on_cmd_205, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_206), cmi_nt35532_5p5_1080pxa_video_on_cmd_206, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_207), cmi_nt35532_5p5_1080pxa_video_on_cmd_207, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_208), cmi_nt35532_5p5_1080pxa_video_on_cmd_208, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_209), cmi_nt35532_5p5_1080pxa_video_on_cmd_209, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_210), cmi_nt35532_5p5_1080pxa_video_on_cmd_210, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_211), cmi_nt35532_5p5_1080pxa_video_on_cmd_211, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_212), cmi_nt35532_5p5_1080pxa_video_on_cmd_212, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_213), cmi_nt35532_5p5_1080pxa_video_on_cmd_213, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_214), cmi_nt35532_5p5_1080pxa_video_on_cmd_214, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_215), cmi_nt35532_5p5_1080pxa_video_on_cmd_215, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_216), cmi_nt35532_5p5_1080pxa_video_on_cmd_216, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_217), cmi_nt35532_5p5_1080pxa_video_on_cmd_217, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_218), cmi_nt35532_5p5_1080pxa_video_on_cmd_218, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_219), cmi_nt35532_5p5_1080pxa_video_on_cmd_219, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_220), cmi_nt35532_5p5_1080pxa_video_on_cmd_220, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_221), cmi_nt35532_5p5_1080pxa_video_on_cmd_221, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_222), cmi_nt35532_5p5_1080pxa_video_on_cmd_222, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_223), cmi_nt35532_5p5_1080pxa_video_on_cmd_223, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_224), cmi_nt35532_5p5_1080pxa_video_on_cmd_224, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_225), cmi_nt35532_5p5_1080pxa_video_on_cmd_225, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_226), cmi_nt35532_5p5_1080pxa_video_on_cmd_226, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_227), cmi_nt35532_5p5_1080pxa_video_on_cmd_227, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_228), cmi_nt35532_5p5_1080pxa_video_on_cmd_228, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_229), cmi_nt35532_5p5_1080pxa_video_on_cmd_229, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_230), cmi_nt35532_5p5_1080pxa_video_on_cmd_230, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_231), cmi_nt35532_5p5_1080pxa_video_on_cmd_231, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_232), cmi_nt35532_5p5_1080pxa_video_on_cmd_232, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_233), cmi_nt35532_5p5_1080pxa_video_on_cmd_233, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_234), cmi_nt35532_5p5_1080pxa_video_on_cmd_234, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_235), cmi_nt35532_5p5_1080pxa_video_on_cmd_235, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_236), cmi_nt35532_5p5_1080pxa_video_on_cmd_236, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_237), cmi_nt35532_5p5_1080pxa_video_on_cmd_237, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_238), cmi_nt35532_5p5_1080pxa_video_on_cmd_238, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_239), cmi_nt35532_5p5_1080pxa_video_on_cmd_239, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_240), cmi_nt35532_5p5_1080pxa_video_on_cmd_240, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_241), cmi_nt35532_5p5_1080pxa_video_on_cmd_241, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_242), cmi_nt35532_5p5_1080pxa_video_on_cmd_242, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_243), cmi_nt35532_5p5_1080pxa_video_on_cmd_243, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_244), cmi_nt35532_5p5_1080pxa_video_on_cmd_244, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_245), cmi_nt35532_5p5_1080pxa_video_on_cmd_245, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_246), cmi_nt35532_5p5_1080pxa_video_on_cmd_246, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_247), cmi_nt35532_5p5_1080pxa_video_on_cmd_247, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_248), cmi_nt35532_5p5_1080pxa_video_on_cmd_248, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_249), cmi_nt35532_5p5_1080pxa_video_on_cmd_249, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_250), cmi_nt35532_5p5_1080pxa_video_on_cmd_250, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_251), cmi_nt35532_5p5_1080pxa_video_on_cmd_251, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_252), cmi_nt35532_5p5_1080pxa_video_on_cmd_252, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_253), cmi_nt35532_5p5_1080pxa_video_on_cmd_253, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_254), cmi_nt35532_5p5_1080pxa_video_on_cmd_254, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_255), cmi_nt35532_5p5_1080pxa_video_on_cmd_255, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_256), cmi_nt35532_5p5_1080pxa_video_on_cmd_256, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_257), cmi_nt35532_5p5_1080pxa_video_on_cmd_257, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_258), cmi_nt35532_5p5_1080pxa_video_on_cmd_258, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_259), cmi_nt35532_5p5_1080pxa_video_on_cmd_259, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_260), cmi_nt35532_5p5_1080pxa_video_on_cmd_260, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_261), cmi_nt35532_5p5_1080pxa_video_on_cmd_261, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_262), cmi_nt35532_5p5_1080pxa_video_on_cmd_262, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_263), cmi_nt35532_5p5_1080pxa_video_on_cmd_263, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_264), cmi_nt35532_5p5_1080pxa_video_on_cmd_264, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_265), cmi_nt35532_5p5_1080pxa_video_on_cmd_265, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_266), cmi_nt35532_5p5_1080pxa_video_on_cmd_266, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_267), cmi_nt35532_5p5_1080pxa_video_on_cmd_267, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_268), cmi_nt35532_5p5_1080pxa_video_on_cmd_268, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_269), cmi_nt35532_5p5_1080pxa_video_on_cmd_269, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_270), cmi_nt35532_5p5_1080pxa_video_on_cmd_270, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_271), cmi_nt35532_5p5_1080pxa_video_on_cmd_271, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_272), cmi_nt35532_5p5_1080pxa_video_on_cmd_272, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_273), cmi_nt35532_5p5_1080pxa_video_on_cmd_273, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_274), cmi_nt35532_5p5_1080pxa_video_on_cmd_274, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_275), cmi_nt35532_5p5_1080pxa_video_on_cmd_275, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_276), cmi_nt35532_5p5_1080pxa_video_on_cmd_276, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_277), cmi_nt35532_5p5_1080pxa_video_on_cmd_277, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_278), cmi_nt35532_5p5_1080pxa_video_on_cmd_278, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_279), cmi_nt35532_5p5_1080pxa_video_on_cmd_279, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_280), cmi_nt35532_5p5_1080pxa_video_on_cmd_280, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_281), cmi_nt35532_5p5_1080pxa_video_on_cmd_281, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_282), cmi_nt35532_5p5_1080pxa_video_on_cmd_282, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_283), cmi_nt35532_5p5_1080pxa_video_on_cmd_283, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_284), cmi_nt35532_5p5_1080pxa_video_on_cmd_284, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_285), cmi_nt35532_5p5_1080pxa_video_on_cmd_285, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_286), cmi_nt35532_5p5_1080pxa_video_on_cmd_286, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_287), cmi_nt35532_5p5_1080pxa_video_on_cmd_287, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_288), cmi_nt35532_5p5_1080pxa_video_on_cmd_288, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_289), cmi_nt35532_5p5_1080pxa_video_on_cmd_289, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_290), cmi_nt35532_5p5_1080pxa_video_on_cmd_290, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_291), cmi_nt35532_5p5_1080pxa_video_on_cmd_291, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_292), cmi_nt35532_5p5_1080pxa_video_on_cmd_292, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_293), cmi_nt35532_5p5_1080pxa_video_on_cmd_293, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_294), cmi_nt35532_5p5_1080pxa_video_on_cmd_294, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_295), cmi_nt35532_5p5_1080pxa_video_on_cmd_295, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_296), cmi_nt35532_5p5_1080pxa_video_on_cmd_296, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_297), cmi_nt35532_5p5_1080pxa_video_on_cmd_297, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_298), cmi_nt35532_5p5_1080pxa_video_on_cmd_298, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_299), cmi_nt35532_5p5_1080pxa_video_on_cmd_299, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_300), cmi_nt35532_5p5_1080pxa_video_on_cmd_300, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_301), cmi_nt35532_5p5_1080pxa_video_on_cmd_301, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_302), cmi_nt35532_5p5_1080pxa_video_on_cmd_302, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_303), cmi_nt35532_5p5_1080pxa_video_on_cmd_303, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_304), cmi_nt35532_5p5_1080pxa_video_on_cmd_304, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_305), cmi_nt35532_5p5_1080pxa_video_on_cmd_305, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_306), cmi_nt35532_5p5_1080pxa_video_on_cmd_306, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_307), cmi_nt35532_5p5_1080pxa_video_on_cmd_307, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_308), cmi_nt35532_5p5_1080pxa_video_on_cmd_308, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_309), cmi_nt35532_5p5_1080pxa_video_on_cmd_309, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_310), cmi_nt35532_5p5_1080pxa_video_on_cmd_310, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_311), cmi_nt35532_5p5_1080pxa_video_on_cmd_311, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_312), cmi_nt35532_5p5_1080pxa_video_on_cmd_312, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_313), cmi_nt35532_5p5_1080pxa_video_on_cmd_313, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_314), cmi_nt35532_5p5_1080pxa_video_on_cmd_314, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_315), cmi_nt35532_5p5_1080pxa_video_on_cmd_315, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_316), cmi_nt35532_5p5_1080pxa_video_on_cmd_316, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_317), cmi_nt35532_5p5_1080pxa_video_on_cmd_317, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_318), cmi_nt35532_5p5_1080pxa_video_on_cmd_318, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_319), cmi_nt35532_5p5_1080pxa_video_on_cmd_319, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_320), cmi_nt35532_5p5_1080pxa_video_on_cmd_320, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_321), cmi_nt35532_5p5_1080pxa_video_on_cmd_321, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_322), cmi_nt35532_5p5_1080pxa_video_on_cmd_322, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_323), cmi_nt35532_5p5_1080pxa_video_on_cmd_323, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_324), cmi_nt35532_5p5_1080pxa_video_on_cmd_324, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_325), cmi_nt35532_5p5_1080pxa_video_on_cmd_325, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_326), cmi_nt35532_5p5_1080pxa_video_on_cmd_326, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_327), cmi_nt35532_5p5_1080pxa_video_on_cmd_327, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_328), cmi_nt35532_5p5_1080pxa_video_on_cmd_328, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_329), cmi_nt35532_5p5_1080pxa_video_on_cmd_329, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_330), cmi_nt35532_5p5_1080pxa_video_on_cmd_330, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_331), cmi_nt35532_5p5_1080pxa_video_on_cmd_331, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_332), cmi_nt35532_5p5_1080pxa_video_on_cmd_332, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_333), cmi_nt35532_5p5_1080pxa_video_on_cmd_333, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_334), cmi_nt35532_5p5_1080pxa_video_on_cmd_334, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_335), cmi_nt35532_5p5_1080pxa_video_on_cmd_335, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_336), cmi_nt35532_5p5_1080pxa_video_on_cmd_336, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_337), cmi_nt35532_5p5_1080pxa_video_on_cmd_337, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_338), cmi_nt35532_5p5_1080pxa_video_on_cmd_338, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_339), cmi_nt35532_5p5_1080pxa_video_on_cmd_339, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_340), cmi_nt35532_5p5_1080pxa_video_on_cmd_340, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_341), cmi_nt35532_5p5_1080pxa_video_on_cmd_341, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_342), cmi_nt35532_5p5_1080pxa_video_on_cmd_342, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_343), cmi_nt35532_5p5_1080pxa_video_on_cmd_343, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_344), cmi_nt35532_5p5_1080pxa_video_on_cmd_344, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_345), cmi_nt35532_5p5_1080pxa_video_on_cmd_345, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_346), cmi_nt35532_5p5_1080pxa_video_on_cmd_346, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_347), cmi_nt35532_5p5_1080pxa_video_on_cmd_347, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_348), cmi_nt35532_5p5_1080pxa_video_on_cmd_348, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_349), cmi_nt35532_5p5_1080pxa_video_on_cmd_349, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_350), cmi_nt35532_5p5_1080pxa_video_on_cmd_350, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_351), cmi_nt35532_5p5_1080pxa_video_on_cmd_351, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_352), cmi_nt35532_5p5_1080pxa_video_on_cmd_352, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_353), cmi_nt35532_5p5_1080pxa_video_on_cmd_353, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_354), cmi_nt35532_5p5_1080pxa_video_on_cmd_354, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_355), cmi_nt35532_5p5_1080pxa_video_on_cmd_355, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_356), cmi_nt35532_5p5_1080pxa_video_on_cmd_356, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_357), cmi_nt35532_5p5_1080pxa_video_on_cmd_357, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_358), cmi_nt35532_5p5_1080pxa_video_on_cmd_358, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_359), cmi_nt35532_5p5_1080pxa_video_on_cmd_359, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_360), cmi_nt35532_5p5_1080pxa_video_on_cmd_360, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_361), cmi_nt35532_5p5_1080pxa_video_on_cmd_361, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_362), cmi_nt35532_5p5_1080pxa_video_on_cmd_362, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_363), cmi_nt35532_5p5_1080pxa_video_on_cmd_363, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_364), cmi_nt35532_5p5_1080pxa_video_on_cmd_364, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_365), cmi_nt35532_5p5_1080pxa_video_on_cmd_365, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_366), cmi_nt35532_5p5_1080pxa_video_on_cmd_366, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_367), cmi_nt35532_5p5_1080pxa_video_on_cmd_367, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_368), cmi_nt35532_5p5_1080pxa_video_on_cmd_368, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_369), cmi_nt35532_5p5_1080pxa_video_on_cmd_369, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_370), cmi_nt35532_5p5_1080pxa_video_on_cmd_370, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_371), cmi_nt35532_5p5_1080pxa_video_on_cmd_371, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_372), cmi_nt35532_5p5_1080pxa_video_on_cmd_372, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_373), cmi_nt35532_5p5_1080pxa_video_on_cmd_373, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_374), cmi_nt35532_5p5_1080pxa_video_on_cmd_374, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_375), cmi_nt35532_5p5_1080pxa_video_on_cmd_375, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_376), cmi_nt35532_5p5_1080pxa_video_on_cmd_376, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_377), cmi_nt35532_5p5_1080pxa_video_on_cmd_377, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_378), cmi_nt35532_5p5_1080pxa_video_on_cmd_378, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_379), cmi_nt35532_5p5_1080pxa_video_on_cmd_379, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_380), cmi_nt35532_5p5_1080pxa_video_on_cmd_380, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_381), cmi_nt35532_5p5_1080pxa_video_on_cmd_381, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_382), cmi_nt35532_5p5_1080pxa_video_on_cmd_382, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_383), cmi_nt35532_5p5_1080pxa_video_on_cmd_383, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_384), cmi_nt35532_5p5_1080pxa_video_on_cmd_384, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_385), cmi_nt35532_5p5_1080pxa_video_on_cmd_385, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_386), cmi_nt35532_5p5_1080pxa_video_on_cmd_386, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_387), cmi_nt35532_5p5_1080pxa_video_on_cmd_387, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_388), cmi_nt35532_5p5_1080pxa_video_on_cmd_388, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_389), cmi_nt35532_5p5_1080pxa_video_on_cmd_389, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_390), cmi_nt35532_5p5_1080pxa_video_on_cmd_390, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_391), cmi_nt35532_5p5_1080pxa_video_on_cmd_391, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_392), cmi_nt35532_5p5_1080pxa_video_on_cmd_392, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_393), cmi_nt35532_5p5_1080pxa_video_on_cmd_393, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_394), cmi_nt35532_5p5_1080pxa_video_on_cmd_394, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_395), cmi_nt35532_5p5_1080pxa_video_on_cmd_395, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_396), cmi_nt35532_5p5_1080pxa_video_on_cmd_396, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_397), cmi_nt35532_5p5_1080pxa_video_on_cmd_397, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_398), cmi_nt35532_5p5_1080pxa_video_on_cmd_398, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_399), cmi_nt35532_5p5_1080pxa_video_on_cmd_399, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_400), cmi_nt35532_5p5_1080pxa_video_on_cmd_400, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_401), cmi_nt35532_5p5_1080pxa_video_on_cmd_401, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_402), cmi_nt35532_5p5_1080pxa_video_on_cmd_402, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_403), cmi_nt35532_5p5_1080pxa_video_on_cmd_403, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_404), cmi_nt35532_5p5_1080pxa_video_on_cmd_404, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_405), cmi_nt35532_5p5_1080pxa_video_on_cmd_405, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_406), cmi_nt35532_5p5_1080pxa_video_on_cmd_406, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_407), cmi_nt35532_5p5_1080pxa_video_on_cmd_407, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_408), cmi_nt35532_5p5_1080pxa_video_on_cmd_408, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_409), cmi_nt35532_5p5_1080pxa_video_on_cmd_409, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_410), cmi_nt35532_5p5_1080pxa_video_on_cmd_410, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_411), cmi_nt35532_5p5_1080pxa_video_on_cmd_411, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_412), cmi_nt35532_5p5_1080pxa_video_on_cmd_412, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_413), cmi_nt35532_5p5_1080pxa_video_on_cmd_413, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_414), cmi_nt35532_5p5_1080pxa_video_on_cmd_414, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_415), cmi_nt35532_5p5_1080pxa_video_on_cmd_415, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_416), cmi_nt35532_5p5_1080pxa_video_on_cmd_416, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_417), cmi_nt35532_5p5_1080pxa_video_on_cmd_417, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_418), cmi_nt35532_5p5_1080pxa_video_on_cmd_418, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_419), cmi_nt35532_5p5_1080pxa_video_on_cmd_419, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_420), cmi_nt35532_5p5_1080pxa_video_on_cmd_420, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_421), cmi_nt35532_5p5_1080pxa_video_on_cmd_421, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_422), cmi_nt35532_5p5_1080pxa_video_on_cmd_422, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_423), cmi_nt35532_5p5_1080pxa_video_on_cmd_423, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_424), cmi_nt35532_5p5_1080pxa_video_on_cmd_424, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_425), cmi_nt35532_5p5_1080pxa_video_on_cmd_425, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_426), cmi_nt35532_5p5_1080pxa_video_on_cmd_426, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_427), cmi_nt35532_5p5_1080pxa_video_on_cmd_427, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_428), cmi_nt35532_5p5_1080pxa_video_on_cmd_428, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_429), cmi_nt35532_5p5_1080pxa_video_on_cmd_429, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_430), cmi_nt35532_5p5_1080pxa_video_on_cmd_430, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_431), cmi_nt35532_5p5_1080pxa_video_on_cmd_431, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_432), cmi_nt35532_5p5_1080pxa_video_on_cmd_432, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_433), cmi_nt35532_5p5_1080pxa_video_on_cmd_433, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_434), cmi_nt35532_5p5_1080pxa_video_on_cmd_434, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_435), cmi_nt35532_5p5_1080pxa_video_on_cmd_435, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_436), cmi_nt35532_5p5_1080pxa_video_on_cmd_436, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_437), cmi_nt35532_5p5_1080pxa_video_on_cmd_437, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_438), cmi_nt35532_5p5_1080pxa_video_on_cmd_438, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_439), cmi_nt35532_5p5_1080pxa_video_on_cmd_439, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_440), cmi_nt35532_5p5_1080pxa_video_on_cmd_440, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_441), cmi_nt35532_5p5_1080pxa_video_on_cmd_441, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_442), cmi_nt35532_5p5_1080pxa_video_on_cmd_442, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_443), cmi_nt35532_5p5_1080pxa_video_on_cmd_443, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_444), cmi_nt35532_5p5_1080pxa_video_on_cmd_444, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_445), cmi_nt35532_5p5_1080pxa_video_on_cmd_445, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_446), cmi_nt35532_5p5_1080pxa_video_on_cmd_446, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_447), cmi_nt35532_5p5_1080pxa_video_on_cmd_447, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_448), cmi_nt35532_5p5_1080pxa_video_on_cmd_448, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_449), cmi_nt35532_5p5_1080pxa_video_on_cmd_449, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_450), cmi_nt35532_5p5_1080pxa_video_on_cmd_450, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_451), cmi_nt35532_5p5_1080pxa_video_on_cmd_451, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_452), cmi_nt35532_5p5_1080pxa_video_on_cmd_452, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_453), cmi_nt35532_5p5_1080pxa_video_on_cmd_453, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_454), cmi_nt35532_5p5_1080pxa_video_on_cmd_454, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_455), cmi_nt35532_5p5_1080pxa_video_on_cmd_455, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_456), cmi_nt35532_5p5_1080pxa_video_on_cmd_456, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_457), cmi_nt35532_5p5_1080pxa_video_on_cmd_457, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_458), cmi_nt35532_5p5_1080pxa_video_on_cmd_458, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_459), cmi_nt35532_5p5_1080pxa_video_on_cmd_459, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_460), cmi_nt35532_5p5_1080pxa_video_on_cmd_460, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_461), cmi_nt35532_5p5_1080pxa_video_on_cmd_461, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_462), cmi_nt35532_5p5_1080pxa_video_on_cmd_462, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_463), cmi_nt35532_5p5_1080pxa_video_on_cmd_463, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_464), cmi_nt35532_5p5_1080pxa_video_on_cmd_464, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_465), cmi_nt35532_5p5_1080pxa_video_on_cmd_465, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_466), cmi_nt35532_5p5_1080pxa_video_on_cmd_466, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_467), cmi_nt35532_5p5_1080pxa_video_on_cmd_467, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_468), cmi_nt35532_5p5_1080pxa_video_on_cmd_468, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_469), cmi_nt35532_5p5_1080pxa_video_on_cmd_469, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_470), cmi_nt35532_5p5_1080pxa_video_on_cmd_470, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_471), cmi_nt35532_5p5_1080pxa_video_on_cmd_471, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_472), cmi_nt35532_5p5_1080pxa_video_on_cmd_472, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_473), cmi_nt35532_5p5_1080pxa_video_on_cmd_473, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_474), cmi_nt35532_5p5_1080pxa_video_on_cmd_474, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_475), cmi_nt35532_5p5_1080pxa_video_on_cmd_475, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_476), cmi_nt35532_5p5_1080pxa_video_on_cmd_476, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_477), cmi_nt35532_5p5_1080pxa_video_on_cmd_477, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_478), cmi_nt35532_5p5_1080pxa_video_on_cmd_478, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_479), cmi_nt35532_5p5_1080pxa_video_on_cmd_479, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_480), cmi_nt35532_5p5_1080pxa_video_on_cmd_480, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_481), cmi_nt35532_5p5_1080pxa_video_on_cmd_481, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_482), cmi_nt35532_5p5_1080pxa_video_on_cmd_482, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_483), cmi_nt35532_5p5_1080pxa_video_on_cmd_483, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_484), cmi_nt35532_5p5_1080pxa_video_on_cmd_484, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_485), cmi_nt35532_5p5_1080pxa_video_on_cmd_485, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_486), cmi_nt35532_5p5_1080pxa_video_on_cmd_486, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_487), cmi_nt35532_5p5_1080pxa_video_on_cmd_487, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_488), cmi_nt35532_5p5_1080pxa_video_on_cmd_488, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_489), cmi_nt35532_5p5_1080pxa_video_on_cmd_489, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_490), cmi_nt35532_5p5_1080pxa_video_on_cmd_490, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_491), cmi_nt35532_5p5_1080pxa_video_on_cmd_491, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_492), cmi_nt35532_5p5_1080pxa_video_on_cmd_492, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_493), cmi_nt35532_5p5_1080pxa_video_on_cmd_493, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_494), cmi_nt35532_5p5_1080pxa_video_on_cmd_494, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_495), cmi_nt35532_5p5_1080pxa_video_on_cmd_495, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_496), cmi_nt35532_5p5_1080pxa_video_on_cmd_496, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_497), cmi_nt35532_5p5_1080pxa_video_on_cmd_497, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_498), cmi_nt35532_5p5_1080pxa_video_on_cmd_498, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_499), cmi_nt35532_5p5_1080pxa_video_on_cmd_499, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_500), cmi_nt35532_5p5_1080pxa_video_on_cmd_500, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_501), cmi_nt35532_5p5_1080pxa_video_on_cmd_501, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_502), cmi_nt35532_5p5_1080pxa_video_on_cmd_502, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_503), cmi_nt35532_5p5_1080pxa_video_on_cmd_503, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_504), cmi_nt35532_5p5_1080pxa_video_on_cmd_504, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_505), cmi_nt35532_5p5_1080pxa_video_on_cmd_505, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_506), cmi_nt35532_5p5_1080pxa_video_on_cmd_506, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_507), cmi_nt35532_5p5_1080pxa_video_on_cmd_507, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_508), cmi_nt35532_5p5_1080pxa_video_on_cmd_508, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_509), cmi_nt35532_5p5_1080pxa_video_on_cmd_509, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_510), cmi_nt35532_5p5_1080pxa_video_on_cmd_510, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_511), cmi_nt35532_5p5_1080pxa_video_on_cmd_511, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_512), cmi_nt35532_5p5_1080pxa_video_on_cmd_512, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_513), cmi_nt35532_5p5_1080pxa_video_on_cmd_513, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_514), cmi_nt35532_5p5_1080pxa_video_on_cmd_514, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_515), cmi_nt35532_5p5_1080pxa_video_on_cmd_515, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_516), cmi_nt35532_5p5_1080pxa_video_on_cmd_516, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_517), cmi_nt35532_5p5_1080pxa_video_on_cmd_517, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_518), cmi_nt35532_5p5_1080pxa_video_on_cmd_518, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_519), cmi_nt35532_5p5_1080pxa_video_on_cmd_519, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_520), cmi_nt35532_5p5_1080pxa_video_on_cmd_520, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_521), cmi_nt35532_5p5_1080pxa_video_on_cmd_521, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_522), cmi_nt35532_5p5_1080pxa_video_on_cmd_522, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_523), cmi_nt35532_5p5_1080pxa_video_on_cmd_523, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_524), cmi_nt35532_5p5_1080pxa_video_on_cmd_524, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_525), cmi_nt35532_5p5_1080pxa_video_on_cmd_525, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_526), cmi_nt35532_5p5_1080pxa_video_on_cmd_526, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_527), cmi_nt35532_5p5_1080pxa_video_on_cmd_527, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_528), cmi_nt35532_5p5_1080pxa_video_on_cmd_528, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_529), cmi_nt35532_5p5_1080pxa_video_on_cmd_529, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_530), cmi_nt35532_5p5_1080pxa_video_on_cmd_530, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_531), cmi_nt35532_5p5_1080pxa_video_on_cmd_531, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_532), cmi_nt35532_5p5_1080pxa_video_on_cmd_532, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_533), cmi_nt35532_5p5_1080pxa_video_on_cmd_533, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_534), cmi_nt35532_5p5_1080pxa_video_on_cmd_534, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_535), cmi_nt35532_5p5_1080pxa_video_on_cmd_535, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_536), cmi_nt35532_5p5_1080pxa_video_on_cmd_536, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_537), cmi_nt35532_5p5_1080pxa_video_on_cmd_537, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_538), cmi_nt35532_5p5_1080pxa_video_on_cmd_538, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_539), cmi_nt35532_5p5_1080pxa_video_on_cmd_539, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_540), cmi_nt35532_5p5_1080pxa_video_on_cmd_540, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_541), cmi_nt35532_5p5_1080pxa_video_on_cmd_541, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_542), cmi_nt35532_5p5_1080pxa_video_on_cmd_542, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_543), cmi_nt35532_5p5_1080pxa_video_on_cmd_543, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_544), cmi_nt35532_5p5_1080pxa_video_on_cmd_544, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_545), cmi_nt35532_5p5_1080pxa_video_on_cmd_545, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_546), cmi_nt35532_5p5_1080pxa_video_on_cmd_546, 120 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_547), cmi_nt35532_5p5_1080pxa_video_on_cmd_547, 20 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_on_cmd_548), cmi_nt35532_5p5_1080pxa_video_on_cmd_548, 0 },
};

static char cmi_nt35532_5p5_1080pxa_video_off_cmd_0[] = {
	0x02, 0x00, 0x39, 0xc0, 0xff, 0x00, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_off_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfb, 0x01, 0xff, 0xff
};
static char cmi_nt35532_5p5_1080pxa_video_off_cmd_2[] = {
	0x28, 0x00, 0x05, 0x80
};
static char cmi_nt35532_5p5_1080pxa_video_off_cmd_3[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd cmi_nt35532_5p5_1080pxa_video_off_command[] = {
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_off_cmd_0), cmi_nt35532_5p5_1080pxa_video_off_cmd_0, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_off_cmd_1), cmi_nt35532_5p5_1080pxa_video_off_cmd_1, 0 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_off_cmd_2), cmi_nt35532_5p5_1080pxa_video_off_cmd_2, 20 },
	{ sizeof(cmi_nt35532_5p5_1080pxa_video_off_cmd_3), cmi_nt35532_5p5_1080pxa_video_off_cmd_3, 120 },
};

static struct command_state cmi_nt35532_5p5_1080pxa_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info cmi_nt35532_5p5_1080pxa_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info cmi_nt35532_5p5_1080pxa_video_video_panel = {
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

static struct lane_configuration cmi_nt35532_5p5_1080pxa_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t cmi_nt35532_5p5_1080pxa_video_timings[] = {
	0xe6, 0x38, 0x26, 0x00, 0x68, 0x6e, 0x2a, 0x3c, 0x2c, 0x03, 0x04, 0x00
};

static struct panel_timing cmi_nt35532_5p5_1080pxa_video_timing_info = {
	.tclk_post = 0x2c,
	.tclk_pre = 0x2b,
};

static struct panel_reset_sequence cmi_nt35532_5p5_1080pxa_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 5, 5, 15 },
	.pin_direction = 2,
};

static struct backlight cmi_nt35532_5p5_1080pxa_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_cmi_nt35532_5p5_1080pxa_video_select(struct panel_struct *panel,
							      struct msm_panel_info *pinfo,
							      struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &cmi_nt35532_5p5_1080pxa_video_panel_data;
	panel->panelres = &cmi_nt35532_5p5_1080pxa_video_panel_res;
	panel->color = &cmi_nt35532_5p5_1080pxa_video_color;
	panel->videopanel = &cmi_nt35532_5p5_1080pxa_video_video_panel;
	panel->commandpanel = &cmi_nt35532_5p5_1080pxa_video_command_panel;
	panel->state = &cmi_nt35532_5p5_1080pxa_video_state;
	panel->laneconfig = &cmi_nt35532_5p5_1080pxa_video_lane_config;
	panel->paneltiminginfo = &cmi_nt35532_5p5_1080pxa_video_timing_info;
	panel->panelresetseq = &cmi_nt35532_5p5_1080pxa_video_reset_seq;
	panel->backlightinfo = &cmi_nt35532_5p5_1080pxa_video_backlight;
	pinfo->mipi.panel_cmds = cmi_nt35532_5p5_1080pxa_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(cmi_nt35532_5p5_1080pxa_video_on_command);
	memcpy(phy_db->timing, cmi_nt35532_5p5_1080pxa_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_CMI_NT35532_5P5_1080PXA_VIDEO_H_ */
