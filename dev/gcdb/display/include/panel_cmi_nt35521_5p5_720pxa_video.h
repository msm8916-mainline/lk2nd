// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2023 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_CMI_NT35521_5P5_720PXA_VIDEO_H_
#define _PANEL_CMI_NT35521_5P5_720PXA_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config cmi_nt35521_5p5_720pxa_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_cmi_nt35521_5p5_720pxa_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution cmi_nt35521_5p5_720pxa_video_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 88,
	.hback_porch = 88,
	.hpulse_width = 12,
	.hsync_skew = 0,
	.vfront_porch = 20,
	.vback_porch = 20,
	.vpulse_width = 3,
	/* Borders not supported yet */
};

static struct color_info cmi_nt35521_5p5_720pxa_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char cmi_nt35521_5p5_720pxa_video_on_cmd_0[] = {
	0x05, 0x00, 0x29, 0xc0, 0xff, 0xaa, 0x55, 0xa5,
	0x80, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_1[] = {
	0x03, 0x00, 0x29, 0xc0, 0x6f, 0x11, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_2[] = {
	0x03, 0x00, 0x29, 0xc0, 0xf7, 0x20, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_3[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_4[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf7, 0xa0, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_5[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x19, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_6[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf7, 0x12, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_7[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf4, 0x03, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_8[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_9[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc8, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_10[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x64, 0x21, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_11[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb6, 0x08, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_12[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x02, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_13[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb8, 0x08, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_14[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbb, 0x74, 0x44, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_15[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0x00, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_16[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbd, 0x02, 0xb0, 0x1e,
	0x1e, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_17[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc5, 0x01, 0x07, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_18[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x01, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_19[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x05, 0x05, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_20[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x05, 0x05, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_21[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0xa0, 0x01, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_22[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbd, 0xa0, 0x01, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_23[] = {
	0x02, 0x00, 0x29, 0xc0, 0xca, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_24[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x04, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_25[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb3, 0x37, 0x37, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_26[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb4, 0x0f, 0x0f, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_27[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb6, 0x05, 0x05, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_28[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb9, 0x46, 0x46, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_29[] = {
	0x03, 0x00, 0x29, 0xc0, 0xba, 0x25, 0x25, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_30[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x02, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_31[] = {
	0x02, 0x00, 0x29, 0xc0, 0xee, 0x01, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_32[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb0, 0x00, 0x00, 0x00,
	0x42, 0x00, 0x88, 0x00, 0xa6, 0x00, 0xbf, 0x00,
	0xe6, 0x01, 0x05, 0x01, 0x32, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_33[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb1, 0x01, 0x58, 0x01,
	0x94, 0x01, 0xc1, 0x02, 0x08, 0x02, 0x40, 0x02,
	0x42, 0x02, 0x78, 0x02, 0xb4, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_34[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb2, 0x02, 0xd9, 0x03,
	0x0c, 0x03, 0x30, 0x03, 0x5f, 0x03, 0x7f, 0x03,
	0xa5, 0x03, 0xc0, 0x03, 0xe0, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_35[] = {
	0x05, 0x00, 0x29, 0xc0, 0xb3, 0x03, 0xf6, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_36[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_37[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x29, 0x2a, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_38[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x10, 0x12, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_39[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb2, 0x14, 0x16, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_40[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb3, 0x18, 0x1a, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_41[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb4, 0x08, 0x0a, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_42[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb5, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_43[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb6, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_44[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb7, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_45[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb8, 0x2e, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_46[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb9, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_47[] = {
	0x03, 0x00, 0x29, 0xc0, 0xba, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_48[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbb, 0x01, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_49[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_50[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbd, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_51[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbe, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_52[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbf, 0x0b, 0x09, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_53[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc0, 0x1b, 0x19, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_54[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc1, 0x17, 0x15, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_55[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc2, 0x13, 0x11, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_56[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc3, 0x2a, 0x29, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_57[] = {
	0x03, 0x00, 0x29, 0xc0, 0xe5, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_58[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc4, 0x29, 0x2a, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_59[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc5, 0x1b, 0x19, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_60[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc6, 0x17, 0x15, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_61[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc7, 0x13, 0x11, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_62[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc8, 0x01, 0x0b, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_63[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc9, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_64[] = {
	0x03, 0x00, 0x29, 0xc0, 0xca, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_65[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcb, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_66[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcc, 0x2e, 0x09, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_67[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcd, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_68[] = {
	0x03, 0x00, 0x29, 0xc0, 0xce, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_69[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcf, 0x08, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_70[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd0, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_71[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd1, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_72[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd2, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_73[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd3, 0x0a, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_74[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd4, 0x10, 0x12, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_75[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd5, 0x14, 0x16, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_76[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd6, 0x18, 0x1a, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_77[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd7, 0x2a, 0x29, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_78[] = {
	0x03, 0x00, 0x29, 0xc0, 0xe6, 0x2e, 0x2e, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_79[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_80[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd9, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_81[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe7, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_82[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_83[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x00, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_84[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x00, 0x00, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_85[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb2, 0x05, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_86[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb6, 0x05, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_87[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb7, 0x05, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_88[] = {
	0x06, 0x00, 0x29, 0xc0, 0xba, 0x57, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_89[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbb, 0x57, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_90[] = {
	0x05, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_91[] = {
	0x05, 0x00, 0x29, 0xc0, 0xc1, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_92[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x60, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_93[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x40, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_94[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_95[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbd, 0x03, 0x01, 0x03,
	0x03, 0x03, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_96[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x17, 0x06, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_97[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x17, 0x06, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_98[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb2, 0x17, 0x06, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_99[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb3, 0x17, 0x06, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_100[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb4, 0x17, 0x06, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_101[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb5, 0x17, 0x06, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_102[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb8, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_103[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb9, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_104[] = {
	0x02, 0x00, 0x29, 0xc0, 0xba, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_105[] = {
	0x02, 0x00, 0x29, 0xc0, 0xbb, 0x02, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_106[] = {
	0x02, 0x00, 0x29, 0xc0, 0xbc, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_107[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x07, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_108[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x80, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_109[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0xa4, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_110[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc8, 0x05, 0x30, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_111[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc9, 0x01, 0x31, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_112[] = {
	0x04, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x00, 0x3c
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_113[] = {
	0x04, 0x00, 0x29, 0xc0, 0xcd, 0x00, 0x00, 0x3c
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_114[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd1, 0x00, 0x05, 0x09,
	0x07, 0x10, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_115[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd2, 0x00, 0x05, 0x0e,
	0x07, 0x10, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_116[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe5, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_117[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe6, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_118[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe7, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_119[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe8, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_120[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe9, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_121[] = {
	0x02, 0x00, 0x29, 0xc0, 0xea, 0x06, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_122[] = {
	0x02, 0x00, 0x29, 0xc0, 0xed, 0x30, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_123[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x11, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_124[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf3, 0x01, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_125[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_126[] = {
	0x05, 0x00, 0x29, 0xc0, 0xe6, 0xff, 0xff, 0xfa,
	0xfa, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_127[] = {
	0x0a, 0x00, 0x29, 0xc0, 0xcc, 0x40, 0x36, 0x87,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_128[] = {
	0x11, 0x00, 0x29, 0xc0, 0xd1, 0x00, 0x02, 0x06,
	0x0a, 0x0e, 0x12, 0x15, 0x18, 0x18, 0x18, 0x16,
	0x14, 0x12, 0x0a, 0x04, 0x00, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_129[] = {
	0x0d, 0x00, 0x29, 0xc0, 0xd7, 0x24, 0x23, 0x22,
	0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_130[] = {
	0x0e, 0x00, 0x29, 0xc0, 0xd8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23,
	0x22, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_131[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd9, 0x02, 0x09, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_132[] = {
	0x53, 0x24, 0x15, 0x80
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_133[] = {
	0x51, 0x00, 0x15, 0x80
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_134[] = {
	0x5e, 0x28, 0x15, 0x80
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_135[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x00, 0x00, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_136[] = {
	0x05, 0x00, 0x29, 0xc0, 0xff, 0xaa, 0x55, 0xa5,
	0x00, 0xff, 0xff, 0xff
};
static char cmi_nt35521_5p5_720pxa_video_on_cmd_137[] = {
	0x11, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd cmi_nt35521_5p5_720pxa_video_on_command[] = {
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_0), cmi_nt35521_5p5_720pxa_video_on_cmd_0, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_1), cmi_nt35521_5p5_720pxa_video_on_cmd_1, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_2), cmi_nt35521_5p5_720pxa_video_on_cmd_2, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_3), cmi_nt35521_5p5_720pxa_video_on_cmd_3, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_4), cmi_nt35521_5p5_720pxa_video_on_cmd_4, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_5), cmi_nt35521_5p5_720pxa_video_on_cmd_5, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_6), cmi_nt35521_5p5_720pxa_video_on_cmd_6, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_7), cmi_nt35521_5p5_720pxa_video_on_cmd_7, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_8), cmi_nt35521_5p5_720pxa_video_on_cmd_8, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_9), cmi_nt35521_5p5_720pxa_video_on_cmd_9, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_10), cmi_nt35521_5p5_720pxa_video_on_cmd_10, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_11), cmi_nt35521_5p5_720pxa_video_on_cmd_11, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_12), cmi_nt35521_5p5_720pxa_video_on_cmd_12, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_13), cmi_nt35521_5p5_720pxa_video_on_cmd_13, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_14), cmi_nt35521_5p5_720pxa_video_on_cmd_14, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_15), cmi_nt35521_5p5_720pxa_video_on_cmd_15, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_16), cmi_nt35521_5p5_720pxa_video_on_cmd_16, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_17), cmi_nt35521_5p5_720pxa_video_on_cmd_17, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_18), cmi_nt35521_5p5_720pxa_video_on_cmd_18, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_19), cmi_nt35521_5p5_720pxa_video_on_cmd_19, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_20), cmi_nt35521_5p5_720pxa_video_on_cmd_20, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_21), cmi_nt35521_5p5_720pxa_video_on_cmd_21, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_22), cmi_nt35521_5p5_720pxa_video_on_cmd_22, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_23), cmi_nt35521_5p5_720pxa_video_on_cmd_23, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_24), cmi_nt35521_5p5_720pxa_video_on_cmd_24, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_25), cmi_nt35521_5p5_720pxa_video_on_cmd_25, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_26), cmi_nt35521_5p5_720pxa_video_on_cmd_26, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_27), cmi_nt35521_5p5_720pxa_video_on_cmd_27, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_28), cmi_nt35521_5p5_720pxa_video_on_cmd_28, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_29), cmi_nt35521_5p5_720pxa_video_on_cmd_29, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_30), cmi_nt35521_5p5_720pxa_video_on_cmd_30, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_31), cmi_nt35521_5p5_720pxa_video_on_cmd_31, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_32), cmi_nt35521_5p5_720pxa_video_on_cmd_32, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_33), cmi_nt35521_5p5_720pxa_video_on_cmd_33, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_34), cmi_nt35521_5p5_720pxa_video_on_cmd_34, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_35), cmi_nt35521_5p5_720pxa_video_on_cmd_35, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_36), cmi_nt35521_5p5_720pxa_video_on_cmd_36, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_37), cmi_nt35521_5p5_720pxa_video_on_cmd_37, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_38), cmi_nt35521_5p5_720pxa_video_on_cmd_38, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_39), cmi_nt35521_5p5_720pxa_video_on_cmd_39, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_40), cmi_nt35521_5p5_720pxa_video_on_cmd_40, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_41), cmi_nt35521_5p5_720pxa_video_on_cmd_41, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_42), cmi_nt35521_5p5_720pxa_video_on_cmd_42, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_43), cmi_nt35521_5p5_720pxa_video_on_cmd_43, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_44), cmi_nt35521_5p5_720pxa_video_on_cmd_44, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_45), cmi_nt35521_5p5_720pxa_video_on_cmd_45, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_46), cmi_nt35521_5p5_720pxa_video_on_cmd_46, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_47), cmi_nt35521_5p5_720pxa_video_on_cmd_47, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_48), cmi_nt35521_5p5_720pxa_video_on_cmd_48, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_49), cmi_nt35521_5p5_720pxa_video_on_cmd_49, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_50), cmi_nt35521_5p5_720pxa_video_on_cmd_50, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_51), cmi_nt35521_5p5_720pxa_video_on_cmd_51, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_52), cmi_nt35521_5p5_720pxa_video_on_cmd_52, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_53), cmi_nt35521_5p5_720pxa_video_on_cmd_53, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_54), cmi_nt35521_5p5_720pxa_video_on_cmd_54, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_55), cmi_nt35521_5p5_720pxa_video_on_cmd_55, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_56), cmi_nt35521_5p5_720pxa_video_on_cmd_56, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_57), cmi_nt35521_5p5_720pxa_video_on_cmd_57, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_58), cmi_nt35521_5p5_720pxa_video_on_cmd_58, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_59), cmi_nt35521_5p5_720pxa_video_on_cmd_59, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_60), cmi_nt35521_5p5_720pxa_video_on_cmd_60, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_61), cmi_nt35521_5p5_720pxa_video_on_cmd_61, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_62), cmi_nt35521_5p5_720pxa_video_on_cmd_62, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_63), cmi_nt35521_5p5_720pxa_video_on_cmd_63, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_64), cmi_nt35521_5p5_720pxa_video_on_cmd_64, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_65), cmi_nt35521_5p5_720pxa_video_on_cmd_65, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_66), cmi_nt35521_5p5_720pxa_video_on_cmd_66, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_67), cmi_nt35521_5p5_720pxa_video_on_cmd_67, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_68), cmi_nt35521_5p5_720pxa_video_on_cmd_68, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_69), cmi_nt35521_5p5_720pxa_video_on_cmd_69, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_70), cmi_nt35521_5p5_720pxa_video_on_cmd_70, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_71), cmi_nt35521_5p5_720pxa_video_on_cmd_71, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_72), cmi_nt35521_5p5_720pxa_video_on_cmd_72, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_73), cmi_nt35521_5p5_720pxa_video_on_cmd_73, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_74), cmi_nt35521_5p5_720pxa_video_on_cmd_74, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_75), cmi_nt35521_5p5_720pxa_video_on_cmd_75, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_76), cmi_nt35521_5p5_720pxa_video_on_cmd_76, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_77), cmi_nt35521_5p5_720pxa_video_on_cmd_77, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_78), cmi_nt35521_5p5_720pxa_video_on_cmd_78, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_79), cmi_nt35521_5p5_720pxa_video_on_cmd_79, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_80), cmi_nt35521_5p5_720pxa_video_on_cmd_80, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_81), cmi_nt35521_5p5_720pxa_video_on_cmd_81, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_82), cmi_nt35521_5p5_720pxa_video_on_cmd_82, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_83), cmi_nt35521_5p5_720pxa_video_on_cmd_83, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_84), cmi_nt35521_5p5_720pxa_video_on_cmd_84, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_85), cmi_nt35521_5p5_720pxa_video_on_cmd_85, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_86), cmi_nt35521_5p5_720pxa_video_on_cmd_86, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_87), cmi_nt35521_5p5_720pxa_video_on_cmd_87, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_88), cmi_nt35521_5p5_720pxa_video_on_cmd_88, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_89), cmi_nt35521_5p5_720pxa_video_on_cmd_89, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_90), cmi_nt35521_5p5_720pxa_video_on_cmd_90, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_91), cmi_nt35521_5p5_720pxa_video_on_cmd_91, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_92), cmi_nt35521_5p5_720pxa_video_on_cmd_92, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_93), cmi_nt35521_5p5_720pxa_video_on_cmd_93, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_94), cmi_nt35521_5p5_720pxa_video_on_cmd_94, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_95), cmi_nt35521_5p5_720pxa_video_on_cmd_95, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_96), cmi_nt35521_5p5_720pxa_video_on_cmd_96, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_97), cmi_nt35521_5p5_720pxa_video_on_cmd_97, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_98), cmi_nt35521_5p5_720pxa_video_on_cmd_98, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_99), cmi_nt35521_5p5_720pxa_video_on_cmd_99, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_100), cmi_nt35521_5p5_720pxa_video_on_cmd_100, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_101), cmi_nt35521_5p5_720pxa_video_on_cmd_101, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_102), cmi_nt35521_5p5_720pxa_video_on_cmd_102, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_103), cmi_nt35521_5p5_720pxa_video_on_cmd_103, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_104), cmi_nt35521_5p5_720pxa_video_on_cmd_104, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_105), cmi_nt35521_5p5_720pxa_video_on_cmd_105, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_106), cmi_nt35521_5p5_720pxa_video_on_cmd_106, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_107), cmi_nt35521_5p5_720pxa_video_on_cmd_107, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_108), cmi_nt35521_5p5_720pxa_video_on_cmd_108, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_109), cmi_nt35521_5p5_720pxa_video_on_cmd_109, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_110), cmi_nt35521_5p5_720pxa_video_on_cmd_110, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_111), cmi_nt35521_5p5_720pxa_video_on_cmd_111, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_112), cmi_nt35521_5p5_720pxa_video_on_cmd_112, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_113), cmi_nt35521_5p5_720pxa_video_on_cmd_113, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_114), cmi_nt35521_5p5_720pxa_video_on_cmd_114, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_115), cmi_nt35521_5p5_720pxa_video_on_cmd_115, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_116), cmi_nt35521_5p5_720pxa_video_on_cmd_116, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_117), cmi_nt35521_5p5_720pxa_video_on_cmd_117, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_118), cmi_nt35521_5p5_720pxa_video_on_cmd_118, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_119), cmi_nt35521_5p5_720pxa_video_on_cmd_119, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_120), cmi_nt35521_5p5_720pxa_video_on_cmd_120, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_121), cmi_nt35521_5p5_720pxa_video_on_cmd_121, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_122), cmi_nt35521_5p5_720pxa_video_on_cmd_122, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_123), cmi_nt35521_5p5_720pxa_video_on_cmd_123, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_124), cmi_nt35521_5p5_720pxa_video_on_cmd_124, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_125), cmi_nt35521_5p5_720pxa_video_on_cmd_125, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_126), cmi_nt35521_5p5_720pxa_video_on_cmd_126, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_127), cmi_nt35521_5p5_720pxa_video_on_cmd_127, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_128), cmi_nt35521_5p5_720pxa_video_on_cmd_128, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_129), cmi_nt35521_5p5_720pxa_video_on_cmd_129, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_130), cmi_nt35521_5p5_720pxa_video_on_cmd_130, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_131), cmi_nt35521_5p5_720pxa_video_on_cmd_131, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_132), cmi_nt35521_5p5_720pxa_video_on_cmd_132, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_133), cmi_nt35521_5p5_720pxa_video_on_cmd_133, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_134), cmi_nt35521_5p5_720pxa_video_on_cmd_134, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_135), cmi_nt35521_5p5_720pxa_video_on_cmd_135, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_136), cmi_nt35521_5p5_720pxa_video_on_cmd_136, 0 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_on_cmd_137), cmi_nt35521_5p5_720pxa_video_on_cmd_137, 120 },
};

static char cmi_nt35521_5p5_720pxa_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char cmi_nt35521_5p5_720pxa_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd cmi_nt35521_5p5_720pxa_video_off_command[] = {
	{ sizeof(cmi_nt35521_5p5_720pxa_video_off_cmd_0), cmi_nt35521_5p5_720pxa_video_off_cmd_0, 20 },
	{ sizeof(cmi_nt35521_5p5_720pxa_video_off_cmd_1), cmi_nt35521_5p5_720pxa_video_off_cmd_1, 120 },
};

static struct command_state cmi_nt35521_5p5_720pxa_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info cmi_nt35521_5p5_720pxa_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info cmi_nt35521_5p5_720pxa_video_video_panel = {
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

static struct lane_configuration cmi_nt35521_5p5_720pxa_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t cmi_nt35521_5p5_720pxa_video_timings[] = {
	0x7a, 0x1a, 0x12, 0x00, 0x40, 0x42, 0x16, 0x1e, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing cmi_nt35521_5p5_720pxa_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1c,
};

static struct panel_reset_sequence cmi_nt35521_5p5_720pxa_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 1, 20, 120 },
	.pin_direction = 2,
};

static struct backlight cmi_nt35521_5p5_720pxa_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 250,
};

static inline void panel_cmi_nt35521_5p5_720pxa_video_select(struct panel_struct *panel,
							     struct msm_panel_info *pinfo,
							     struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &cmi_nt35521_5p5_720pxa_video_panel_data;
	panel->panelres = &cmi_nt35521_5p5_720pxa_video_panel_res;
	panel->color = &cmi_nt35521_5p5_720pxa_video_color;
	panel->videopanel = &cmi_nt35521_5p5_720pxa_video_video_panel;
	panel->commandpanel = &cmi_nt35521_5p5_720pxa_video_command_panel;
	panel->state = &cmi_nt35521_5p5_720pxa_video_state;
	panel->laneconfig = &cmi_nt35521_5p5_720pxa_video_lane_config;
	panel->paneltiminginfo = &cmi_nt35521_5p5_720pxa_video_timing_info;
	panel->panelresetseq = &cmi_nt35521_5p5_720pxa_video_reset_seq;
	panel->backlightinfo = &cmi_nt35521_5p5_720pxa_video_backlight;
	pinfo->mipi.panel_cmds = cmi_nt35521_5p5_720pxa_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(cmi_nt35521_5p5_720pxa_video_on_command);
	memcpy(phy_db->timing, cmi_nt35521_5p5_720pxa_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_CMI_NT35521_5P5_720PXA_VIDEO_H_ */
