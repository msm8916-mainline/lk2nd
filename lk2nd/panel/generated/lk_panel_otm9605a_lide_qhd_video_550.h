// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2023 nphuracm (maybe)
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_OTM9605A_LIDE_QHD_VIDEO_550_H_
#define _PANEL_OTM9605A_LIDE_QHD_VIDEO_550_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config otm9605a_lide_qhd_video_550_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_otm9605a_lide_qhd_video_550",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 50000,
};

static struct panel_resolution otm9605a_lide_qhd_video_550_panel_res = {
	.panel_width = 540,
	.panel_height = 960,
	.hfront_porch = 52,
	.hback_porch = 48,
	.hpulse_width = 4,
	.hsync_skew = 0,
	.vfront_porch = 18,
	.vback_porch = 14,
	.vpulse_width = 2,
	/* Borders not supported yet */
};

static struct color_info otm9605a_lide_qhd_video_550_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char otm9605a_lide_qhd_video_550_on_cmd_0[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_1[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x96, 0x05, 0x01
};
static char otm9605a_lide_qhd_video_550_on_cmd_2[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_3[] = {
	0x03, 0x00, 0x39, 0xc0, 0xff, 0x96, 0x05, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_4[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc5, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_5[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0x03, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x89, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x01, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb4, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_9[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x50, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_10[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_11[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc1, 0x36, 0x66, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_13[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x02, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_14[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x9c, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x87, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_17[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x40, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_18[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x84, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_19[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x18, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_20[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x91, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_21[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x76, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_22[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x93, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_23[] = {
	0x04, 0x00, 0x39, 0xc0, 0xc5, 0x03, 0x55, 0x55
};
static char otm9605a_lide_qhd_video_550_on_cmd_24[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb1, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_25[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x28, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_26[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_27[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_28[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb2, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_29[] = {
	0x05, 0x00, 0x39, 0xc0, 0xf5, 0x15, 0x00, 0x15,
	0x00, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_30[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb2, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_31[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x01, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_32[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_33[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_34[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_35[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char otm9605a_lide_qhd_video_550_on_cmd_36[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_37[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char otm9605a_lide_qhd_video_550_on_cmd_38[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_39[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_40[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_41[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x04,
	0x04, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x04,
	0x04, 0x00, 0x00, 0x00
};
static char otm9605a_lide_qhd_video_550_on_cmd_42[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_43[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
	0x00, 0x00, 0x04, 0x04
};
static char otm9605a_lide_qhd_video_550_on_cmd_44[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xe0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_45[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcb, 0x04, 0x04, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_46[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xf0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_47[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcb, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_48[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_49[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcc, 0x00, 0x00, 0x26,
	0x25, 0x02, 0x06, 0x00, 0x00, 0x0a, 0x0e, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_50[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_51[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x0c, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x26, 0x25, 0x01
};
static char otm9605a_lide_qhd_video_550_on_cmd_52[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_53[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x05, 0x00, 0x00,
	0x09, 0x0d, 0x0b, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char otm9605a_lide_qhd_video_550_on_cmd_54[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_55[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcc, 0x00, 0x00, 0x25,
	0x26, 0x05, 0x01, 0x00, 0x00, 0x0f, 0x0b, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_56[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_57[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x0d, 0x09, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x25, 0x26, 0x06
};
static char otm9605a_lide_qhd_video_550_on_cmd_58[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_59[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x02, 0x00, 0x00,
	0x10, 0x0c, 0x0e, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char otm9605a_lide_qhd_video_550_on_cmd_60[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_61[] = {
	0x0d, 0x00, 0x39, 0xc0, 0xce, 0x87, 0x03, 0x28,
	0x86, 0x03, 0x28, 0x00, 0x0f, 0x00, 0x00, 0x0f,
	0x00, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_62[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_63[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x33, 0xbe, 0x28,
	0x33, 0xbf, 0x28, 0xf0, 0x00, 0x00, 0xf0, 0x00,
	0x00, 0x00, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_64[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_65[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x03, 0x83,
	0xc0, 0x8a, 0x18, 0x00, 0x38, 0x02, 0x83, 0xc1,
	0x89, 0x18, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_66[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_67[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x01, 0x83,
	0xc2, 0x88, 0x18, 0x00, 0x38, 0x00, 0x83, 0xc3,
	0x87, 0x18, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_68[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_69[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x30, 0x00, 0x83,
	0xc4, 0x86, 0x18, 0x00, 0x30, 0x01, 0x83, 0xc5,
	0x85, 0x18, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_70[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_71[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x30, 0x02, 0x83,
	0xc6, 0x84, 0x18, 0x00, 0x30, 0x03, 0x83, 0xc7,
	0x83, 0x18, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_72[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_73[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcf, 0x01, 0x01, 0x20,
	0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_74[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_75[] = {
	0x03, 0x00, 0x39, 0xc0, 0xd8, 0x6f, 0x6f, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_76[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_77[] = {
	0x11, 0x00, 0x39, 0xc0, 0xe1, 0x02, 0x09, 0x0e,
	0x0d, 0x06, 0x0e, 0x0b, 0x0a, 0x04, 0x08, 0x0c,
	0x07, 0x0e, 0x0d, 0x06, 0x01, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_78[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_79[] = {
	0x11, 0x00, 0x39, 0xc0, 0xe2, 0x02, 0x08, 0x0d,
	0x0d, 0x05, 0x0f, 0x0b, 0x09, 0x04, 0x08, 0x0c,
	0x07, 0x0e, 0x0d, 0x06, 0x01, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_80[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_81[] = {
	0x02, 0x00, 0x39, 0xc0, 0xd6, 0x48, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_82[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_83[] = {
	0x1d, 0x00, 0x39, 0xc0, 0xca, 0x80, 0xb9, 0xbe,
	0xc3, 0xc8, 0xcd, 0xd2, 0xd7, 0xdc, 0xe1, 0xe6,
	0xeb, 0xf0, 0xf5, 0xff, 0xff, 0xc3, 0xff, 0xc3,
	0xff, 0xc3, 0xff, 0x05, 0x03, 0x05, 0x03, 0x05,
	0x03, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_84[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_85[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x10, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_86[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0xa9, 0xaa, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x89, 0x78, 0x67, 0x56, 0x45, 0x33, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_87[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_88[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x11, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_89[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0xa9, 0xaa, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x89, 0x78, 0x67, 0x56, 0x45, 0x33, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_90[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_91[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x12, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_92[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x99, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x88, 0x88, 0x77, 0x66, 0x45, 0x33, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_93[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_94[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x13, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_95[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x99, 0x89, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x88, 0x88, 0x78, 0x67, 0x45, 0x33, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_96[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_97[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x14, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_98[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x99, 0x88, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x89, 0x88, 0x88, 0x88, 0x77, 0x45, 0x33, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_99[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_100[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x15, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_101[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x99, 0x88, 0x98, 0x99, 0x89, 0x99, 0x99,
	0x89, 0x88, 0x88, 0x88, 0x78, 0x46, 0x33, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_102[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_103[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x16, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_104[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x89, 0x88, 0x98, 0x99, 0x89, 0x98, 0x99,
	0x89, 0x88, 0x88, 0x88, 0x78, 0x56, 0x34, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_105[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_106[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x17, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_107[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x89, 0x88, 0x88, 0x98, 0x99, 0x88, 0x99,
	0x89, 0x88, 0x88, 0x88, 0x88, 0x57, 0x34, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_108[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_109[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x18, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_110[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x89, 0x88, 0x88, 0x88, 0x98, 0x99, 0x99,
	0x88, 0x88, 0x88, 0x88, 0x77, 0x67, 0x45, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_111[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_112[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x19, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_113[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x89, 0x88, 0x88, 0x88, 0x88, 0x98, 0x99,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x67, 0x45, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_114[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_115[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x1a, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_116[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x99, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x99,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x55, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_117[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_118[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x1b, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_119[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x89, 0x88, 0x88, 0x88, 0x88, 0x88, 0x98, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x77, 0x57, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_120[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_121[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x1c, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_122[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x99, 0x99,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x98, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x56, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_123[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_124[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x1d, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_125[] = {
	0x13, 0x00, 0x39, 0xc0, 0xc8, 0x80, 0x98, 0x99,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x67, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_126[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_127[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc7, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_128[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb1, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_129[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0x06, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_130[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb4, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_131[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0x10, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_132[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_133[] = {
	0x22, 0x00, 0x39, 0xc0, 0xec, 0x40, 0x34, 0x44,
	0x44, 0x43, 0x44, 0x34, 0x44, 0x44, 0x43, 0x44,
	0x34, 0x44, 0x44, 0x43, 0x44, 0x34, 0x44, 0x34,
	0x44, 0x44, 0x43, 0x44, 0x34, 0x44, 0x44, 0x43,
	0x44, 0x34, 0x44, 0x44, 0x43, 0x04, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_134[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_135[] = {
	0x22, 0x00, 0x39, 0xc0, 0xed, 0x40, 0x34, 0x44,
	0x34, 0x44, 0x34, 0x44, 0x34, 0x44, 0x44, 0x43,
	0x44, 0x43, 0x44, 0x43, 0x44, 0x34, 0x44, 0x34,
	0x44, 0x34, 0x44, 0x34, 0x44, 0x44, 0x43, 0x44,
	0x43, 0x44, 0x43, 0x44, 0x43, 0x04, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_136[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_137[] = {
	0x22, 0x00, 0x39, 0xc0, 0xee, 0x40, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x04, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_138[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_139[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0xff, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_140[] = {
	0x02, 0x00, 0x39, 0xc0, 0x51, 0x00, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_141[] = {
	0x02, 0x00, 0x39, 0xc0, 0x53, 0x24, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_142[] = {
	0x02, 0x00, 0x39, 0xc0, 0x55, 0x01, 0xff, 0xff
};
static char otm9605a_lide_qhd_video_550_on_cmd_143[] = {
	0x11, 0x00, 0x05, 0x80
};
static char otm9605a_lide_qhd_video_550_on_cmd_144[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd otm9605a_lide_qhd_video_550_on_command[] = {
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_0), otm9605a_lide_qhd_video_550_on_cmd_0, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_1), otm9605a_lide_qhd_video_550_on_cmd_1, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_2), otm9605a_lide_qhd_video_550_on_cmd_2, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_3), otm9605a_lide_qhd_video_550_on_cmd_3, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_4), otm9605a_lide_qhd_video_550_on_cmd_4, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_5), otm9605a_lide_qhd_video_550_on_cmd_5, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_6), otm9605a_lide_qhd_video_550_on_cmd_6, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_7), otm9605a_lide_qhd_video_550_on_cmd_7, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_8), otm9605a_lide_qhd_video_550_on_cmd_8, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_9), otm9605a_lide_qhd_video_550_on_cmd_9, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_10), otm9605a_lide_qhd_video_550_on_cmd_10, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_11), otm9605a_lide_qhd_video_550_on_cmd_11, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_12), otm9605a_lide_qhd_video_550_on_cmd_12, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_13), otm9605a_lide_qhd_video_550_on_cmd_13, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_14), otm9605a_lide_qhd_video_550_on_cmd_14, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_15), otm9605a_lide_qhd_video_550_on_cmd_15, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_16), otm9605a_lide_qhd_video_550_on_cmd_16, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_17), otm9605a_lide_qhd_video_550_on_cmd_17, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_18), otm9605a_lide_qhd_video_550_on_cmd_18, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_19), otm9605a_lide_qhd_video_550_on_cmd_19, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_20), otm9605a_lide_qhd_video_550_on_cmd_20, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_21), otm9605a_lide_qhd_video_550_on_cmd_21, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_22), otm9605a_lide_qhd_video_550_on_cmd_22, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_23), otm9605a_lide_qhd_video_550_on_cmd_23, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_24), otm9605a_lide_qhd_video_550_on_cmd_24, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_25), otm9605a_lide_qhd_video_550_on_cmd_25, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_26), otm9605a_lide_qhd_video_550_on_cmd_26, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_27), otm9605a_lide_qhd_video_550_on_cmd_27, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_28), otm9605a_lide_qhd_video_550_on_cmd_28, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_29), otm9605a_lide_qhd_video_550_on_cmd_29, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_30), otm9605a_lide_qhd_video_550_on_cmd_30, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_31), otm9605a_lide_qhd_video_550_on_cmd_31, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_32), otm9605a_lide_qhd_video_550_on_cmd_32, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_33), otm9605a_lide_qhd_video_550_on_cmd_33, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_34), otm9605a_lide_qhd_video_550_on_cmd_34, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_35), otm9605a_lide_qhd_video_550_on_cmd_35, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_36), otm9605a_lide_qhd_video_550_on_cmd_36, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_37), otm9605a_lide_qhd_video_550_on_cmd_37, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_38), otm9605a_lide_qhd_video_550_on_cmd_38, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_39), otm9605a_lide_qhd_video_550_on_cmd_39, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_40), otm9605a_lide_qhd_video_550_on_cmd_40, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_41), otm9605a_lide_qhd_video_550_on_cmd_41, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_42), otm9605a_lide_qhd_video_550_on_cmd_42, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_43), otm9605a_lide_qhd_video_550_on_cmd_43, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_44), otm9605a_lide_qhd_video_550_on_cmd_44, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_45), otm9605a_lide_qhd_video_550_on_cmd_45, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_46), otm9605a_lide_qhd_video_550_on_cmd_46, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_47), otm9605a_lide_qhd_video_550_on_cmd_47, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_48), otm9605a_lide_qhd_video_550_on_cmd_48, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_49), otm9605a_lide_qhd_video_550_on_cmd_49, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_50), otm9605a_lide_qhd_video_550_on_cmd_50, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_51), otm9605a_lide_qhd_video_550_on_cmd_51, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_52), otm9605a_lide_qhd_video_550_on_cmd_52, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_53), otm9605a_lide_qhd_video_550_on_cmd_53, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_54), otm9605a_lide_qhd_video_550_on_cmd_54, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_55), otm9605a_lide_qhd_video_550_on_cmd_55, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_56), otm9605a_lide_qhd_video_550_on_cmd_56, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_57), otm9605a_lide_qhd_video_550_on_cmd_57, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_58), otm9605a_lide_qhd_video_550_on_cmd_58, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_59), otm9605a_lide_qhd_video_550_on_cmd_59, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_60), otm9605a_lide_qhd_video_550_on_cmd_60, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_61), otm9605a_lide_qhd_video_550_on_cmd_61, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_62), otm9605a_lide_qhd_video_550_on_cmd_62, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_63), otm9605a_lide_qhd_video_550_on_cmd_63, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_64), otm9605a_lide_qhd_video_550_on_cmd_64, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_65), otm9605a_lide_qhd_video_550_on_cmd_65, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_66), otm9605a_lide_qhd_video_550_on_cmd_66, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_67), otm9605a_lide_qhd_video_550_on_cmd_67, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_68), otm9605a_lide_qhd_video_550_on_cmd_68, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_69), otm9605a_lide_qhd_video_550_on_cmd_69, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_70), otm9605a_lide_qhd_video_550_on_cmd_70, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_71), otm9605a_lide_qhd_video_550_on_cmd_71, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_72), otm9605a_lide_qhd_video_550_on_cmd_72, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_73), otm9605a_lide_qhd_video_550_on_cmd_73, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_74), otm9605a_lide_qhd_video_550_on_cmd_74, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_75), otm9605a_lide_qhd_video_550_on_cmd_75, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_76), otm9605a_lide_qhd_video_550_on_cmd_76, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_77), otm9605a_lide_qhd_video_550_on_cmd_77, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_78), otm9605a_lide_qhd_video_550_on_cmd_78, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_79), otm9605a_lide_qhd_video_550_on_cmd_79, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_80), otm9605a_lide_qhd_video_550_on_cmd_80, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_81), otm9605a_lide_qhd_video_550_on_cmd_81, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_82), otm9605a_lide_qhd_video_550_on_cmd_82, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_83), otm9605a_lide_qhd_video_550_on_cmd_83, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_84), otm9605a_lide_qhd_video_550_on_cmd_84, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_85), otm9605a_lide_qhd_video_550_on_cmd_85, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_86), otm9605a_lide_qhd_video_550_on_cmd_86, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_87), otm9605a_lide_qhd_video_550_on_cmd_87, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_88), otm9605a_lide_qhd_video_550_on_cmd_88, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_89), otm9605a_lide_qhd_video_550_on_cmd_89, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_90), otm9605a_lide_qhd_video_550_on_cmd_90, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_91), otm9605a_lide_qhd_video_550_on_cmd_91, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_92), otm9605a_lide_qhd_video_550_on_cmd_92, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_93), otm9605a_lide_qhd_video_550_on_cmd_93, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_94), otm9605a_lide_qhd_video_550_on_cmd_94, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_95), otm9605a_lide_qhd_video_550_on_cmd_95, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_96), otm9605a_lide_qhd_video_550_on_cmd_96, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_97), otm9605a_lide_qhd_video_550_on_cmd_97, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_98), otm9605a_lide_qhd_video_550_on_cmd_98, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_99), otm9605a_lide_qhd_video_550_on_cmd_99, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_100), otm9605a_lide_qhd_video_550_on_cmd_100, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_101), otm9605a_lide_qhd_video_550_on_cmd_101, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_102), otm9605a_lide_qhd_video_550_on_cmd_102, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_103), otm9605a_lide_qhd_video_550_on_cmd_103, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_104), otm9605a_lide_qhd_video_550_on_cmd_104, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_105), otm9605a_lide_qhd_video_550_on_cmd_105, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_106), otm9605a_lide_qhd_video_550_on_cmd_106, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_107), otm9605a_lide_qhd_video_550_on_cmd_107, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_108), otm9605a_lide_qhd_video_550_on_cmd_108, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_109), otm9605a_lide_qhd_video_550_on_cmd_109, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_110), otm9605a_lide_qhd_video_550_on_cmd_110, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_111), otm9605a_lide_qhd_video_550_on_cmd_111, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_112), otm9605a_lide_qhd_video_550_on_cmd_112, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_113), otm9605a_lide_qhd_video_550_on_cmd_113, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_114), otm9605a_lide_qhd_video_550_on_cmd_114, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_115), otm9605a_lide_qhd_video_550_on_cmd_115, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_116), otm9605a_lide_qhd_video_550_on_cmd_116, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_117), otm9605a_lide_qhd_video_550_on_cmd_117, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_118), otm9605a_lide_qhd_video_550_on_cmd_118, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_119), otm9605a_lide_qhd_video_550_on_cmd_119, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_120), otm9605a_lide_qhd_video_550_on_cmd_120, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_121), otm9605a_lide_qhd_video_550_on_cmd_121, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_122), otm9605a_lide_qhd_video_550_on_cmd_122, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_123), otm9605a_lide_qhd_video_550_on_cmd_123, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_124), otm9605a_lide_qhd_video_550_on_cmd_124, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_125), otm9605a_lide_qhd_video_550_on_cmd_125, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_126), otm9605a_lide_qhd_video_550_on_cmd_126, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_127), otm9605a_lide_qhd_video_550_on_cmd_127, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_128), otm9605a_lide_qhd_video_550_on_cmd_128, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_129), otm9605a_lide_qhd_video_550_on_cmd_129, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_130), otm9605a_lide_qhd_video_550_on_cmd_130, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_131), otm9605a_lide_qhd_video_550_on_cmd_131, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_132), otm9605a_lide_qhd_video_550_on_cmd_132, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_133), otm9605a_lide_qhd_video_550_on_cmd_133, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_134), otm9605a_lide_qhd_video_550_on_cmd_134, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_135), otm9605a_lide_qhd_video_550_on_cmd_135, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_136), otm9605a_lide_qhd_video_550_on_cmd_136, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_137), otm9605a_lide_qhd_video_550_on_cmd_137, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_138), otm9605a_lide_qhd_video_550_on_cmd_138, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_139), otm9605a_lide_qhd_video_550_on_cmd_139, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_140), otm9605a_lide_qhd_video_550_on_cmd_140, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_141), otm9605a_lide_qhd_video_550_on_cmd_141, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_142), otm9605a_lide_qhd_video_550_on_cmd_142, 0 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_143), otm9605a_lide_qhd_video_550_on_cmd_143, 120 },
	{ sizeof(otm9605a_lide_qhd_video_550_on_cmd_144), otm9605a_lide_qhd_video_550_on_cmd_144, 20 },
};

static char otm9605a_lide_qhd_video_550_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char otm9605a_lide_qhd_video_550_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd otm9605a_lide_qhd_video_550_off_command[] = {
	{ sizeof(otm9605a_lide_qhd_video_550_off_cmd_0), otm9605a_lide_qhd_video_550_off_cmd_0, 50 },
	{ sizeof(otm9605a_lide_qhd_video_550_off_cmd_1), otm9605a_lide_qhd_video_550_off_cmd_1, 120 },
};

static struct command_state otm9605a_lide_qhd_video_550_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info otm9605a_lide_qhd_video_550_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info otm9605a_lide_qhd_video_550_video_panel = {
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

static struct lane_configuration otm9605a_lide_qhd_video_550_lane_config = {
	.dsi_lanes = 2,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 0,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t otm9605a_lide_qhd_video_550_timings[] = {
	0x2f, 0x1c, 0x12, 0x00, 0x42, 0x44, 0x18, 0x20, 0x17, 0x03, 0x04, 0x00
};

static struct panel_timing otm9605a_lide_qhd_video_550_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x12,
};

static struct panel_reset_sequence otm9605a_lide_qhd_video_550_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 10, 20 },
	.pin_direction = 2,
};

static struct backlight otm9605a_lide_qhd_video_550_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_otm9605a_lide_qhd_video_550_select(struct panel_struct *panel,
							    struct msm_panel_info *pinfo,
							    struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &otm9605a_lide_qhd_video_550_panel_data;
	panel->panelres = &otm9605a_lide_qhd_video_550_panel_res;
	panel->color = &otm9605a_lide_qhd_video_550_color;
	panel->videopanel = &otm9605a_lide_qhd_video_550_video_panel;
	panel->commandpanel = &otm9605a_lide_qhd_video_550_command_panel;
	panel->state = &otm9605a_lide_qhd_video_550_state;
	panel->laneconfig = &otm9605a_lide_qhd_video_550_lane_config;
	panel->paneltiminginfo = &otm9605a_lide_qhd_video_550_timing_info;
	panel->panelresetseq = &otm9605a_lide_qhd_video_550_reset_seq;
	panel->backlightinfo = &otm9605a_lide_qhd_video_550_backlight;
	pinfo->mipi.panel_cmds = otm9605a_lide_qhd_video_550_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(otm9605a_lide_qhd_video_550_on_command);
	memcpy(phy_db->timing, otm9605a_lide_qhd_video_550_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_OTM9605A_LIDE_QHD_VIDEO_550_H_ */
