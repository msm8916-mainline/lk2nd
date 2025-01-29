// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_ILI9806E_FWVGA_HSD_HLT_VIDEO_H_
#define _PANEL_ILI9806E_FWVGA_HSD_HLT_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config ili9806e_fwvga_hsd_hlt_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_ili9806e_fwvga_hsd_hlt_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 50000,
};

static struct panel_resolution ili9806e_fwvga_hsd_hlt_video_panel_res = {
	.panel_width = 480,
	.panel_height = 854,
	.hfront_porch = 80,
	.hback_porch = 100,
	.hpulse_width = 10,
	.hsync_skew = 0,
	.vfront_porch = 20,
	.vback_porch = 16,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info ili9806e_fwvga_hsd_hlt_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char ili9806e_fwvga_hsd_hlt_video_on_cmd_0[] = {
	0x06, 0x00, 0x29, 0xc0, 0xff, 0xff, 0x98, 0x06,
	0x04, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_1[] = {
	0x02, 0x00, 0x29, 0xc0, 0x08, 0x10, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_2[] = {
	0x02, 0x00, 0x29, 0xc0, 0x21, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_3[] = {
	0x02, 0x00, 0x29, 0xc0, 0x30, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_4[] = {
	0x02, 0x00, 0x29, 0xc0, 0x31, 0x02, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_5[] = {
	0x02, 0x00, 0x29, 0xc0, 0x40, 0x18, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_6[] = {
	0x02, 0x00, 0x29, 0xc0, 0x41, 0x33, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_7[] = {
	0x02, 0x00, 0x29, 0xc0, 0x42, 0x03, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_8[] = {
	0x02, 0x00, 0x29, 0xc0, 0x43, 0x09, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_9[] = {
	0x02, 0x00, 0x29, 0xc0, 0x44, 0x07, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_10[] = {
	0x02, 0x00, 0x29, 0xc0, 0x45, 0x16, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_11[] = {
	0x02, 0x00, 0x29, 0xc0, 0x50, 0x78, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_12[] = {
	0x02, 0x00, 0x29, 0xc0, 0x51, 0x78, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_13[] = {
	0x02, 0x00, 0x29, 0xc0, 0x52, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_14[] = {
	0x02, 0x00, 0x29, 0xc0, 0x53, 0x45, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_15[] = {
	0x02, 0x00, 0x29, 0xc0, 0x60, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_16[] = {
	0x02, 0x00, 0x29, 0xc0, 0x61, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_17[] = {
	0x02, 0x00, 0x29, 0xc0, 0x62, 0x08, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_18[] = {
	0x02, 0x00, 0x29, 0xc0, 0x63, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_19[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa0, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_20[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa1, 0x08, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_21[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa2, 0x12, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_22[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa3, 0x10, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_23[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa4, 0x09, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_24[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa5, 0x19, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_25[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa6, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_26[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa7, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_27[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa8, 0x02, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_28[] = {
	0x02, 0x00, 0x29, 0xc0, 0xa9, 0x07, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_29[] = {
	0x02, 0x00, 0x29, 0xc0, 0xaa, 0x05, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_30[] = {
	0x02, 0x00, 0x29, 0xc0, 0xab, 0x06, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_31[] = {
	0x02, 0x00, 0x29, 0xc0, 0xac, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_32[] = {
	0x02, 0x00, 0x29, 0xc0, 0xad, 0x33, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_33[] = {
	0x02, 0x00, 0x29, 0xc0, 0xae, 0x2f, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_34[] = {
	0x02, 0x00, 0x29, 0xc0, 0xaf, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_35[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_36[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc1, 0x07, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_37[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc2, 0x10, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_38[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc3, 0x0f, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_39[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x08, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_40[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x13, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_41[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc6, 0x07, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_42[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc7, 0x07, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_43[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc8, 0x05, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_44[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc9, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_45[] = {
	0x02, 0x00, 0x29, 0xc0, 0xca, 0x09, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_46[] = {
	0x02, 0x00, 0x29, 0xc0, 0xcb, 0x05, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_47[] = {
	0x02, 0x00, 0x29, 0xc0, 0xcc, 0x0e, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_48[] = {
	0x02, 0x00, 0x29, 0xc0, 0xcd, 0x27, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_49[] = {
	0x02, 0x00, 0x29, 0xc0, 0xce, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_50[] = {
	0x02, 0x00, 0x29, 0xc0, 0xcf, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_51[] = {
	0x06, 0x00, 0x29, 0xc0, 0xff, 0xff, 0x98, 0x06,
	0x04, 0x06, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_52[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x21, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_53[] = {
	0x02, 0x00, 0x29, 0xc0, 0x01, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_54[] = {
	0x02, 0x00, 0x29, 0xc0, 0x02, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_55[] = {
	0x02, 0x00, 0x29, 0xc0, 0x03, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_56[] = {
	0x02, 0x00, 0x29, 0xc0, 0x04, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_57[] = {
	0x02, 0x00, 0x29, 0xc0, 0x05, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_58[] = {
	0x02, 0x00, 0x29, 0xc0, 0x06, 0x80, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_59[] = {
	0x02, 0x00, 0x29, 0xc0, 0x07, 0x06, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_60[] = {
	0x02, 0x00, 0x29, 0xc0, 0x08, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_61[] = {
	0x02, 0x00, 0x29, 0xc0, 0x09, 0x80, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_62[] = {
	0x02, 0x00, 0x29, 0xc0, 0x0a, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_63[] = {
	0x02, 0x00, 0x29, 0xc0, 0x0b, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_64[] = {
	0x02, 0x00, 0x29, 0xc0, 0x0c, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_65[] = {
	0x02, 0x00, 0x29, 0xc0, 0x0d, 0x0a, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_66[] = {
	0x02, 0x00, 0x29, 0xc0, 0x0e, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_67[] = {
	0x02, 0x00, 0x29, 0xc0, 0x0f, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_68[] = {
	0x02, 0x00, 0x29, 0xc0, 0x10, 0xf0, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_69[] = {
	0x02, 0x00, 0x29, 0xc0, 0x11, 0xf4, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_70[] = {
	0x02, 0x00, 0x29, 0xc0, 0x12, 0x04, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_71[] = {
	0x02, 0x00, 0x29, 0xc0, 0x13, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_72[] = {
	0x02, 0x00, 0x29, 0xc0, 0x14, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_73[] = {
	0x02, 0x00, 0x29, 0xc0, 0x15, 0xc0, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_74[] = {
	0x02, 0x00, 0x29, 0xc0, 0x16, 0x08, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_75[] = {
	0x02, 0x00, 0x29, 0xc0, 0x17, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_76[] = {
	0x02, 0x00, 0x29, 0xc0, 0x18, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_77[] = {
	0x02, 0x00, 0x29, 0xc0, 0x19, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_78[] = {
	0x02, 0x00, 0x29, 0xc0, 0x1a, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_79[] = {
	0x02, 0x00, 0x29, 0xc0, 0x1b, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_80[] = {
	0x02, 0x00, 0x29, 0xc0, 0x1c, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_81[] = {
	0x02, 0x00, 0x29, 0xc0, 0x1d, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_82[] = {
	0x02, 0x00, 0x29, 0xc0, 0x20, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_83[] = {
	0x02, 0x00, 0x29, 0xc0, 0x21, 0x23, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_84[] = {
	0x02, 0x00, 0x29, 0xc0, 0x22, 0x45, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_85[] = {
	0x02, 0x00, 0x29, 0xc0, 0x23, 0x67, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_86[] = {
	0x02, 0x00, 0x29, 0xc0, 0x24, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_87[] = {
	0x02, 0x00, 0x29, 0xc0, 0x25, 0x23, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_88[] = {
	0x02, 0x00, 0x29, 0xc0, 0x26, 0x45, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_89[] = {
	0x02, 0x00, 0x29, 0xc0, 0x27, 0x67, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_90[] = {
	0x02, 0x00, 0x29, 0xc0, 0x30, 0x01, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_91[] = {
	0x02, 0x00, 0x29, 0xc0, 0x31, 0x11, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_92[] = {
	0x02, 0x00, 0x29, 0xc0, 0x32, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_93[] = {
	0x02, 0x00, 0x29, 0xc0, 0x33, 0xee, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_94[] = {
	0x02, 0x00, 0x29, 0xc0, 0x34, 0xff, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_95[] = {
	0x02, 0x00, 0x29, 0xc0, 0x35, 0xbb, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_96[] = {
	0x02, 0x00, 0x29, 0xc0, 0x36, 0xca, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_97[] = {
	0x02, 0x00, 0x29, 0xc0, 0x37, 0xdd, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_98[] = {
	0x02, 0x00, 0x29, 0xc0, 0x38, 0xac, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_99[] = {
	0x02, 0x00, 0x29, 0xc0, 0x39, 0x76, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_100[] = {
	0x02, 0x00, 0x29, 0xc0, 0x3a, 0x67, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_101[] = {
	0x02, 0x00, 0x29, 0xc0, 0x3b, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_102[] = {
	0x02, 0x00, 0x29, 0xc0, 0x3c, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_103[] = {
	0x02, 0x00, 0x29, 0xc0, 0x3d, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_104[] = {
	0x02, 0x00, 0x29, 0xc0, 0x3e, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_105[] = {
	0x02, 0x00, 0x29, 0xc0, 0x3f, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_106[] = {
	0x02, 0x00, 0x29, 0xc0, 0x40, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_107[] = {
	0x02, 0x00, 0x29, 0xc0, 0x52, 0x10, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_108[] = {
	0x02, 0x00, 0x29, 0xc0, 0x53, 0x10, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_109[] = {
	0x06, 0x00, 0x29, 0xc0, 0xff, 0xff, 0x98, 0x06,
	0x04, 0x07, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_110[] = {
	0x02, 0x00, 0x29, 0xc0, 0x18, 0x1d, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_111[] = {
	0x02, 0x00, 0x29, 0xc0, 0x17, 0x22, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_112[] = {
	0x02, 0x00, 0x29, 0xc0, 0x02, 0x77, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_113[] = {
	0x02, 0x00, 0x29, 0xc0, 0x26, 0xb2, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_114[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe1, 0x79, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_115[] = {
	0x02, 0x00, 0x29, 0xc0, 0x06, 0x13, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_116[] = {
	0x06, 0x00, 0x29, 0xc0, 0xff, 0xff, 0x98, 0x06,
	0x04, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_117[] = {
	0x02, 0x00, 0x29, 0xc0, 0x36, 0x00, 0xff, 0xff
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_118[] = {
	0x11, 0x00, 0x05, 0x80
};
static char ili9806e_fwvga_hsd_hlt_video_on_cmd_119[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd ili9806e_fwvga_hsd_hlt_video_on_command[] = {
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_0), ili9806e_fwvga_hsd_hlt_video_on_cmd_0, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_1), ili9806e_fwvga_hsd_hlt_video_on_cmd_1, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_2), ili9806e_fwvga_hsd_hlt_video_on_cmd_2, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_3), ili9806e_fwvga_hsd_hlt_video_on_cmd_3, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_4), ili9806e_fwvga_hsd_hlt_video_on_cmd_4, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_5), ili9806e_fwvga_hsd_hlt_video_on_cmd_5, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_6), ili9806e_fwvga_hsd_hlt_video_on_cmd_6, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_7), ili9806e_fwvga_hsd_hlt_video_on_cmd_7, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_8), ili9806e_fwvga_hsd_hlt_video_on_cmd_8, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_9), ili9806e_fwvga_hsd_hlt_video_on_cmd_9, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_10), ili9806e_fwvga_hsd_hlt_video_on_cmd_10, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_11), ili9806e_fwvga_hsd_hlt_video_on_cmd_11, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_12), ili9806e_fwvga_hsd_hlt_video_on_cmd_12, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_13), ili9806e_fwvga_hsd_hlt_video_on_cmd_13, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_14), ili9806e_fwvga_hsd_hlt_video_on_cmd_14, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_15), ili9806e_fwvga_hsd_hlt_video_on_cmd_15, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_16), ili9806e_fwvga_hsd_hlt_video_on_cmd_16, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_17), ili9806e_fwvga_hsd_hlt_video_on_cmd_17, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_18), ili9806e_fwvga_hsd_hlt_video_on_cmd_18, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_19), ili9806e_fwvga_hsd_hlt_video_on_cmd_19, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_20), ili9806e_fwvga_hsd_hlt_video_on_cmd_20, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_21), ili9806e_fwvga_hsd_hlt_video_on_cmd_21, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_22), ili9806e_fwvga_hsd_hlt_video_on_cmd_22, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_23), ili9806e_fwvga_hsd_hlt_video_on_cmd_23, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_24), ili9806e_fwvga_hsd_hlt_video_on_cmd_24, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_25), ili9806e_fwvga_hsd_hlt_video_on_cmd_25, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_26), ili9806e_fwvga_hsd_hlt_video_on_cmd_26, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_27), ili9806e_fwvga_hsd_hlt_video_on_cmd_27, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_28), ili9806e_fwvga_hsd_hlt_video_on_cmd_28, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_29), ili9806e_fwvga_hsd_hlt_video_on_cmd_29, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_30), ili9806e_fwvga_hsd_hlt_video_on_cmd_30, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_31), ili9806e_fwvga_hsd_hlt_video_on_cmd_31, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_32), ili9806e_fwvga_hsd_hlt_video_on_cmd_32, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_33), ili9806e_fwvga_hsd_hlt_video_on_cmd_33, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_34), ili9806e_fwvga_hsd_hlt_video_on_cmd_34, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_35), ili9806e_fwvga_hsd_hlt_video_on_cmd_35, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_36), ili9806e_fwvga_hsd_hlt_video_on_cmd_36, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_37), ili9806e_fwvga_hsd_hlt_video_on_cmd_37, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_38), ili9806e_fwvga_hsd_hlt_video_on_cmd_38, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_39), ili9806e_fwvga_hsd_hlt_video_on_cmd_39, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_40), ili9806e_fwvga_hsd_hlt_video_on_cmd_40, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_41), ili9806e_fwvga_hsd_hlt_video_on_cmd_41, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_42), ili9806e_fwvga_hsd_hlt_video_on_cmd_42, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_43), ili9806e_fwvga_hsd_hlt_video_on_cmd_43, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_44), ili9806e_fwvga_hsd_hlt_video_on_cmd_44, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_45), ili9806e_fwvga_hsd_hlt_video_on_cmd_45, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_46), ili9806e_fwvga_hsd_hlt_video_on_cmd_46, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_47), ili9806e_fwvga_hsd_hlt_video_on_cmd_47, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_48), ili9806e_fwvga_hsd_hlt_video_on_cmd_48, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_49), ili9806e_fwvga_hsd_hlt_video_on_cmd_49, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_50), ili9806e_fwvga_hsd_hlt_video_on_cmd_50, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_51), ili9806e_fwvga_hsd_hlt_video_on_cmd_51, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_52), ili9806e_fwvga_hsd_hlt_video_on_cmd_52, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_53), ili9806e_fwvga_hsd_hlt_video_on_cmd_53, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_54), ili9806e_fwvga_hsd_hlt_video_on_cmd_54, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_55), ili9806e_fwvga_hsd_hlt_video_on_cmd_55, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_56), ili9806e_fwvga_hsd_hlt_video_on_cmd_56, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_57), ili9806e_fwvga_hsd_hlt_video_on_cmd_57, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_58), ili9806e_fwvga_hsd_hlt_video_on_cmd_58, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_59), ili9806e_fwvga_hsd_hlt_video_on_cmd_59, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_60), ili9806e_fwvga_hsd_hlt_video_on_cmd_60, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_61), ili9806e_fwvga_hsd_hlt_video_on_cmd_61, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_62), ili9806e_fwvga_hsd_hlt_video_on_cmd_62, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_63), ili9806e_fwvga_hsd_hlt_video_on_cmd_63, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_64), ili9806e_fwvga_hsd_hlt_video_on_cmd_64, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_65), ili9806e_fwvga_hsd_hlt_video_on_cmd_65, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_66), ili9806e_fwvga_hsd_hlt_video_on_cmd_66, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_67), ili9806e_fwvga_hsd_hlt_video_on_cmd_67, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_68), ili9806e_fwvga_hsd_hlt_video_on_cmd_68, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_69), ili9806e_fwvga_hsd_hlt_video_on_cmd_69, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_70), ili9806e_fwvga_hsd_hlt_video_on_cmd_70, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_71), ili9806e_fwvga_hsd_hlt_video_on_cmd_71, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_72), ili9806e_fwvga_hsd_hlt_video_on_cmd_72, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_73), ili9806e_fwvga_hsd_hlt_video_on_cmd_73, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_74), ili9806e_fwvga_hsd_hlt_video_on_cmd_74, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_75), ili9806e_fwvga_hsd_hlt_video_on_cmd_75, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_76), ili9806e_fwvga_hsd_hlt_video_on_cmd_76, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_77), ili9806e_fwvga_hsd_hlt_video_on_cmd_77, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_78), ili9806e_fwvga_hsd_hlt_video_on_cmd_78, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_79), ili9806e_fwvga_hsd_hlt_video_on_cmd_79, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_80), ili9806e_fwvga_hsd_hlt_video_on_cmd_80, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_81), ili9806e_fwvga_hsd_hlt_video_on_cmd_81, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_82), ili9806e_fwvga_hsd_hlt_video_on_cmd_82, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_83), ili9806e_fwvga_hsd_hlt_video_on_cmd_83, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_84), ili9806e_fwvga_hsd_hlt_video_on_cmd_84, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_85), ili9806e_fwvga_hsd_hlt_video_on_cmd_85, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_86), ili9806e_fwvga_hsd_hlt_video_on_cmd_86, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_87), ili9806e_fwvga_hsd_hlt_video_on_cmd_87, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_88), ili9806e_fwvga_hsd_hlt_video_on_cmd_88, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_89), ili9806e_fwvga_hsd_hlt_video_on_cmd_89, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_90), ili9806e_fwvga_hsd_hlt_video_on_cmd_90, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_91), ili9806e_fwvga_hsd_hlt_video_on_cmd_91, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_92), ili9806e_fwvga_hsd_hlt_video_on_cmd_92, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_93), ili9806e_fwvga_hsd_hlt_video_on_cmd_93, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_94), ili9806e_fwvga_hsd_hlt_video_on_cmd_94, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_95), ili9806e_fwvga_hsd_hlt_video_on_cmd_95, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_96), ili9806e_fwvga_hsd_hlt_video_on_cmd_96, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_97), ili9806e_fwvga_hsd_hlt_video_on_cmd_97, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_98), ili9806e_fwvga_hsd_hlt_video_on_cmd_98, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_99), ili9806e_fwvga_hsd_hlt_video_on_cmd_99, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_100), ili9806e_fwvga_hsd_hlt_video_on_cmd_100, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_101), ili9806e_fwvga_hsd_hlt_video_on_cmd_101, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_102), ili9806e_fwvga_hsd_hlt_video_on_cmd_102, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_103), ili9806e_fwvga_hsd_hlt_video_on_cmd_103, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_104), ili9806e_fwvga_hsd_hlt_video_on_cmd_104, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_105), ili9806e_fwvga_hsd_hlt_video_on_cmd_105, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_106), ili9806e_fwvga_hsd_hlt_video_on_cmd_106, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_107), ili9806e_fwvga_hsd_hlt_video_on_cmd_107, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_108), ili9806e_fwvga_hsd_hlt_video_on_cmd_108, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_109), ili9806e_fwvga_hsd_hlt_video_on_cmd_109, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_110), ili9806e_fwvga_hsd_hlt_video_on_cmd_110, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_111), ili9806e_fwvga_hsd_hlt_video_on_cmd_111, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_112), ili9806e_fwvga_hsd_hlt_video_on_cmd_112, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_113), ili9806e_fwvga_hsd_hlt_video_on_cmd_113, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_114), ili9806e_fwvga_hsd_hlt_video_on_cmd_114, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_115), ili9806e_fwvga_hsd_hlt_video_on_cmd_115, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_116), ili9806e_fwvga_hsd_hlt_video_on_cmd_116, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_117), ili9806e_fwvga_hsd_hlt_video_on_cmd_117, 0 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_118), ili9806e_fwvga_hsd_hlt_video_on_cmd_118, 120 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_on_cmd_119), ili9806e_fwvga_hsd_hlt_video_on_cmd_119, 50 },
};

static char ili9806e_fwvga_hsd_hlt_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char ili9806e_fwvga_hsd_hlt_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd ili9806e_fwvga_hsd_hlt_video_off_command[] = {
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_off_cmd_0), ili9806e_fwvga_hsd_hlt_video_off_cmd_0, 50 },
	{ sizeof(ili9806e_fwvga_hsd_hlt_video_off_cmd_1), ili9806e_fwvga_hsd_hlt_video_off_cmd_1, 120 },
};

static struct command_state ili9806e_fwvga_hsd_hlt_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info ili9806e_fwvga_hsd_hlt_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info ili9806e_fwvga_hsd_hlt_video_video_panel = {
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

static struct lane_configuration ili9806e_fwvga_hsd_hlt_video_lane_config = {
	.dsi_lanes = 2,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 0,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t ili9806e_fwvga_hsd_hlt_video_timings[] = {
	0x73, 0x19, 0x10, 0x00, 0x3c, 0x46, 0x14, 0x1c, 0x1c, 0x03, 0x04, 0x00
};

static struct panel_timing ili9806e_fwvga_hsd_hlt_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence ili9806e_fwvga_hsd_hlt_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 20, 20 },
	.pin_direction = 2,
};

static struct backlight ili9806e_fwvga_hsd_hlt_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_ili9806e_fwvga_hsd_hlt_video_select(struct panel_struct *panel,
							     struct msm_panel_info *pinfo,
							     struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &ili9806e_fwvga_hsd_hlt_video_panel_data;
	panel->panelres = &ili9806e_fwvga_hsd_hlt_video_panel_res;
	panel->color = &ili9806e_fwvga_hsd_hlt_video_color;
	panel->videopanel = &ili9806e_fwvga_hsd_hlt_video_video_panel;
	panel->commandpanel = &ili9806e_fwvga_hsd_hlt_video_command_panel;
	panel->state = &ili9806e_fwvga_hsd_hlt_video_state;
	panel->laneconfig = &ili9806e_fwvga_hsd_hlt_video_lane_config;
	panel->paneltiminginfo = &ili9806e_fwvga_hsd_hlt_video_timing_info;
	panel->panelresetseq = &ili9806e_fwvga_hsd_hlt_video_reset_seq;
	panel->backlightinfo = &ili9806e_fwvga_hsd_hlt_video_backlight;
	pinfo->mipi.panel_on_cmds = ili9806e_fwvga_hsd_hlt_video_on_command;
	pinfo->mipi.panel_off_cmds = ili9806e_fwvga_hsd_hlt_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(ili9806e_fwvga_hsd_hlt_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(ili9806e_fwvga_hsd_hlt_video_off_command);
	memcpy(phy_db->timing, ili9806e_fwvga_hsd_hlt_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_ILI9806E_FWVGA_HSD_HLT_VIDEO_H_ */
