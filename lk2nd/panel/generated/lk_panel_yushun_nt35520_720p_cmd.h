// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.

#ifndef _PANEL_YUSHUN_NT35520_720P_CMD_H_
#define _PANEL_YUSHUN_NT35520_720P_CMD_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config yushun_nt35520_720p_cmd_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_yushun_NT35520_720p_cmd",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0, /* Changed to video mode */
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution yushun_nt35520_720p_cmd_panel_res = {
	.panel_width = 720,
	.panel_height = 1280,
	.hfront_porch = 90,
	.hback_porch = 88,
	.hpulse_width = 12,
	.hsync_skew = 0,
	.vfront_porch = 20,
	.vback_porch = 16,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info yushun_nt35520_720p_cmd_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char yushun_nt35520_720p_cmd_on_cmd_0[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_1[] = {
	0x05, 0x00, 0x29, 0x40, 0xff, 0xaa, 0x55, 0xa5,
	0x80, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_2[] = {
	0x02, 0x00, 0x29, 0x40, 0x6f, 0x13, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_3[] = {
	0x02, 0x00, 0x29, 0x40, 0xf7, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_4[] = {
	0x02, 0x00, 0x29, 0x40, 0x6f, 0x01, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_5[] = {
	0x02, 0x00, 0x29, 0x40, 0xf3, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_6[] = {
	0x03, 0x00, 0x29, 0x40, 0xb1, 0x68 | 1 << 4, 0x21, 0xff /* video mode */
};
static char yushun_nt35520_720p_cmd_on_cmd_7[] = {
	0x06, 0x00, 0x29, 0x40, 0xbd, 0x02, 0x67, 0x20,
	0x20, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_8[] = {
	0x02, 0x00, 0x29, 0x40, 0x6f, 0x02, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_9[] = {
	0x02, 0x00, 0x29, 0x40, 0xb8, 0x08, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_10[] = {
	0x03, 0x00, 0x29, 0x40, 0xbb, 0x11, 0x11, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_11[] = {
	0x03, 0x00, 0x29, 0x40, 0xbc, 0x00, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_12[] = {
	0x02, 0x00, 0x29, 0x40, 0xb6, 0x01, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_13[] = {
	0x02, 0x00, 0x29, 0x40, 0xc8, 0x83, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_14[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x01, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_15[] = {
	0x03, 0x00, 0x29, 0x40, 0xb0, 0x0f, 0x0f, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_16[] = {
	0x03, 0x00, 0x29, 0x40, 0xb1, 0x0f, 0x0f, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_17[] = {
	0x02, 0x00, 0x29, 0x40, 0xce, 0x66, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_18[] = {
	0x02, 0x00, 0x29, 0x40, 0xc0, 0x0c, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_19[] = {
	0x03, 0x00, 0x29, 0x40, 0xb5, 0x05, 0x05, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_20[] = {
	0x02, 0x00, 0x29, 0x40, 0xbe, 0x3e, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_21[] = {
	0x03, 0x00, 0x29, 0x40, 0xb3, 0x28, 0x28, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_22[] = {
	0x03, 0x00, 0x29, 0x40, 0xb4, 0x19, 0x19, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_23[] = {
	0x03, 0x00, 0x29, 0x40, 0xb9, 0x34, 0x34, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_24[] = {
	0x03, 0x00, 0x29, 0x40, 0xba, 0x24, 0x24, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_25[] = {
	0x03, 0x00, 0x29, 0x40, 0xbc, 0x70, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_26[] = {
	0x03, 0x00, 0x29, 0x40, 0xbd, 0x70, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_27[] = {
	0x02, 0x00, 0x29, 0x40, 0xca, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_28[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x02, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_29[] = {
	0x02, 0x00, 0x29, 0x40, 0xee, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_30[] = {
	0x11, 0x00, 0x29, 0x40, 0xb0, 0x00, 0xd2, 0x00,
	0xde, 0x00, 0xf0, 0x01, 0x01, 0x01, 0x11, 0x01,
	0x29, 0x01, 0x41, 0x01, 0x68, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_31[] = {
	0x11, 0x00, 0x29, 0x40, 0xb1, 0x01, 0x87, 0x01,
	0xb8, 0x01, 0xe2, 0x02, 0x23, 0x02, 0x57, 0x02,
	0x5a, 0x02, 0x8e, 0x02, 0xc6, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_32[] = {
	0x11, 0x00, 0x29, 0x40, 0xb2, 0x02, 0xef, 0x03,
	0x20, 0x03, 0x3e, 0x03, 0x63, 0x03, 0x7e, 0x03,
	0x9c, 0x03, 0xaf, 0x03, 0xbf, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_33[] = {
	0x05, 0x00, 0x29, 0x40, 0xb3, 0x03, 0xfc, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_34[] = {
	0x11, 0x00, 0x29, 0x40, 0xb4, 0x00, 0x07, 0x00,
	0x50, 0x00, 0x84, 0x00, 0xa6, 0x00, 0xc1, 0x00,
	0xe7, 0x01, 0x08, 0x01, 0x3b, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_35[] = {
	0x11, 0x00, 0x29, 0x40, 0xb5, 0x01, 0x63, 0x01,
	0xa0, 0x01, 0xcf, 0x02, 0x18, 0x02, 0x50, 0x02,
	0x52, 0x02, 0x88, 0x02, 0xc1, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_36[] = {
	0x11, 0x00, 0x29, 0x40, 0xb6, 0x02, 0xea, 0x03,
	0x1b, 0x03, 0x3a, 0x03, 0x60, 0x03, 0x79, 0x03,
	0x98, 0x03, 0xa9, 0x03, 0xc6, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_37[] = {
	0x05, 0x00, 0x29, 0x40, 0xb7, 0x03, 0xd6, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_38[] = {
	0x11, 0x00, 0x29, 0x40, 0xb8, 0x00, 0xb6, 0x00,
	0xc1, 0x00, 0xd3, 0x00, 0xe5, 0x00, 0xf5, 0x01,
	0x0f, 0x01, 0x29, 0x01, 0x51, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_39[] = {
	0x11, 0x00, 0x29, 0x40, 0xb9, 0x01, 0x73, 0x01,
	0xa9, 0x01, 0xd6, 0x02, 0x1c, 0x02, 0x52, 0x02,
	0x55, 0x02, 0x8a, 0x02, 0xc4, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_40[] = {
	0x11, 0x00, 0x29, 0x40, 0xba, 0x02, 0xef, 0x03,
	0x24, 0x03, 0x4b, 0x03, 0x73, 0x03, 0x87, 0x03,
	0x9f, 0x03, 0xa3, 0x03, 0xbd, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_41[] = {
	0x05, 0x00, 0x29, 0x40, 0xbb, 0x03, 0xf2, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_42[] = {
	0x11, 0x00, 0x29, 0x40, 0xbc, 0x00, 0xd2, 0x00,
	0xde, 0x00, 0xf0, 0x01, 0x01, 0x01, 0x11, 0x01,
	0x29, 0x01, 0x41, 0x01, 0x68, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_43[] = {
	0x11, 0x00, 0x29, 0x40, 0xbd, 0x01, 0x87, 0x01,
	0xb8, 0x01, 0xe2, 0x02, 0x23, 0x02, 0x57, 0x02,
	0x5a, 0x02, 0x8e, 0x02, 0xc6, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_44[] = {
	0x11, 0x00, 0x29, 0x40, 0xbe, 0x02, 0xef, 0x03,
	0x20, 0x03, 0x3e, 0x03, 0x63, 0x03, 0x7e, 0x03,
	0x9c, 0x03, 0xaf, 0x03, 0xbf, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_45[] = {
	0x05, 0x00, 0x29, 0x40, 0xbf, 0x03, 0xfc, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_46[] = {
	0x11, 0x00, 0x29, 0x40, 0xc0, 0x00, 0x07, 0x00,
	0x50, 0x00, 0x84, 0x00, 0xa6, 0x00, 0xc1, 0x00,
	0xe7, 0x01, 0x08, 0x01, 0x3b, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_47[] = {
	0x11, 0x00, 0x29, 0x40, 0xc1, 0x01, 0x63, 0x01,
	0xa0, 0x01, 0xcf, 0x02, 0x18, 0x02, 0x50, 0x02,
	0x52, 0x02, 0x88, 0x02, 0xc1, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_48[] = {
	0x11, 0x00, 0x29, 0x40, 0xc2, 0x02, 0xea, 0x03,
	0x1b, 0x03, 0x3a, 0x03, 0x60, 0x03, 0x79, 0x03,
	0x98, 0x03, 0xa9, 0x03, 0xc6, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_49[] = {
	0x05, 0x00, 0x29, 0x40, 0xc3, 0x03, 0xd6, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_50[] = {
	0x11, 0x00, 0x29, 0x40, 0xc4, 0x00, 0xb6, 0x00,
	0xc1, 0x00, 0xd3, 0x00, 0xe5, 0x00, 0xf5, 0x01,
	0x0f, 0x01, 0x29, 0x01, 0x51, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_51[] = {
	0x11, 0x00, 0x29, 0x40, 0xc5, 0x01, 0x73, 0x01,
	0xa9, 0x01, 0xd6, 0x02, 0x1c, 0x02, 0x52, 0x02,
	0x55, 0x02, 0x8a, 0x02, 0xc4, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_52[] = {
	0x11, 0x00, 0x29, 0x40, 0xc6, 0x02, 0xef, 0x03,
	0x24, 0x03, 0x4b, 0x03, 0x73, 0x03, 0x87, 0x03,
	0x9f, 0x03, 0xa3, 0x03, 0xbd, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_53[] = {
	0x05, 0x00, 0x29, 0xc0, 0xc7, 0x03, 0xf2, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_54[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_55[] = {
	0x03, 0x00, 0x29, 0x40, 0xb0, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_56[] = {
	0x03, 0x00, 0x29, 0x40, 0xb1, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_57[] = {
	0x03, 0x00, 0x29, 0x40, 0xb2, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_58[] = {
	0x03, 0x00, 0x29, 0x40, 0xb3, 0x2e, 0x09, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_59[] = {
	0x03, 0x00, 0x29, 0x40, 0xb4, 0x0b, 0x23, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_60[] = {
	0x03, 0x00, 0x29, 0x40, 0xb5, 0x1d, 0x1f, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_61[] = {
	0x03, 0x00, 0x29, 0x40, 0xb6, 0x11, 0x17, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_62[] = {
	0x03, 0x00, 0x29, 0x40, 0xb7, 0x13, 0x19, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_63[] = {
	0x03, 0x00, 0x29, 0x40, 0xb8, 0x01, 0x03, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_64[] = {
	0x03, 0x00, 0x29, 0x40, 0xb9, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_65[] = {
	0x03, 0x00, 0x29, 0x40, 0xba, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_66[] = {
	0x03, 0x00, 0x29, 0x40, 0xbb, 0x02, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_67[] = {
	0x03, 0x00, 0x29, 0x40, 0xbc, 0x18, 0x12, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_68[] = {
	0x03, 0x00, 0x29, 0x40, 0xbd, 0x16, 0x10, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_69[] = {
	0x03, 0x00, 0x29, 0x40, 0xbe, 0x1e, 0x1c, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_70[] = {
	0x03, 0x00, 0x29, 0x40, 0xbf, 0x22, 0x0a, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_71[] = {
	0x03, 0x00, 0x29, 0x40, 0xc0, 0x08, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_72[] = {
	0x03, 0x00, 0x29, 0x40, 0xc1, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_73[] = {
	0x03, 0x00, 0x29, 0x40, 0xc2, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_74[] = {
	0x03, 0x00, 0x29, 0x40, 0xc3, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_75[] = {
	0x03, 0x00, 0x29, 0x40, 0xe5, 0x25, 0x24, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_76[] = {
	0x03, 0x00, 0x29, 0x40, 0xc4, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_77[] = {
	0x03, 0x00, 0x29, 0x40, 0xc5, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_78[] = {
	0x03, 0x00, 0x29, 0x40, 0xc6, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_79[] = {
	0x03, 0x00, 0x29, 0x40, 0xc7, 0x2e, 0x02, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_80[] = {
	0x03, 0x00, 0x29, 0x40, 0xc8, 0x00, 0x24, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_81[] = {
	0x03, 0x00, 0x29, 0x40, 0xc9, 0x1e, 0x1c, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_82[] = {
	0x03, 0x00, 0x29, 0x40, 0xca, 0x18, 0x12, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_83[] = {
	0x03, 0x00, 0x29, 0x40, 0xcb, 0x16, 0x10, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_84[] = {
	0x03, 0x00, 0x29, 0x40, 0xcc, 0x0a, 0x08, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_85[] = {
	0x03, 0x00, 0x29, 0x40, 0xcd, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_86[] = {
	0x03, 0x00, 0x29, 0x40, 0xce, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_87[] = {
	0x03, 0x00, 0x29, 0x40, 0xcf, 0x09, 0x0b, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_88[] = {
	0x03, 0x00, 0x29, 0x40, 0xd0, 0x11, 0x17, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_89[] = {
	0x03, 0x00, 0x29, 0x40, 0xd1, 0x13, 0x19, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_90[] = {
	0x03, 0x00, 0x29, 0x40, 0xd2, 0x1d, 0x1f, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_91[] = {
	0x03, 0x00, 0x29, 0x40, 0xd3, 0x25, 0x01, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_92[] = {
	0x03, 0x00, 0x29, 0x40, 0xd4, 0x03, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_93[] = {
	0x03, 0x00, 0x29, 0x40, 0xd5, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_94[] = {
	0x03, 0x00, 0x29, 0x40, 0xd6, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_95[] = {
	0x03, 0x00, 0x29, 0x40, 0xd7, 0x2e, 0x2e, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_96[] = {
	0x03, 0x00, 0x29, 0x40, 0xe6, 0x22, 0x23, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_97[] = {
	0x06, 0x00, 0x29, 0x40, 0xd8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_98[] = {
	0x06, 0x00, 0x29, 0x40, 0xd9, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_99[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe7, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_100[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_101[] = {
	0x02, 0x00, 0x29, 0x40, 0xed, 0x30, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_102[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_103[] = {
	0x03, 0x00, 0x29, 0x40, 0xb0, 0x20, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_104[] = {
	0x03, 0x00, 0x29, 0x40, 0xb1, 0x20, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_105[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_106[] = {
	0x03, 0x00, 0x29, 0x40, 0xb0, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_107[] = {
	0x02, 0x00, 0x29, 0x40, 0xb8, 0x00, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_108[] = {
	0x06, 0x00, 0x29, 0x40, 0xbd, 0x0f, 0x03, 0x03,
	0x00, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_109[] = {
	0x03, 0x00, 0x29, 0x40, 0xb1, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_110[] = {
	0x03, 0x00, 0x29, 0x40, 0xb9, 0x00, 0x03, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_111[] = {
	0x03, 0x00, 0x29, 0x40, 0xb2, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_112[] = {
	0x03, 0x00, 0x29, 0x40, 0xba, 0x00, 0x03, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_113[] = {
	0x03, 0x00, 0x29, 0x40, 0xb3, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_114[] = {
	0x03, 0x00, 0x29, 0x40, 0xbb, 0x00, 0x00, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_115[] = {
	0x03, 0x00, 0x29, 0x40, 0xb4, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_116[] = {
	0x03, 0x00, 0x29, 0x40, 0xb5, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_117[] = {
	0x03, 0x00, 0x29, 0x40, 0xb6, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_118[] = {
	0x03, 0x00, 0x29, 0x40, 0xb7, 0x17, 0x06, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_119[] = {
	0x03, 0x00, 0x29, 0x40, 0xbc, 0x00, 0x01, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_120[] = {
	0x02, 0x00, 0x29, 0x40, 0xe5, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_121[] = {
	0x02, 0x00, 0x29, 0x40, 0xe6, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_122[] = {
	0x02, 0x00, 0x29, 0x40, 0xe7, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_123[] = {
	0x02, 0x00, 0x29, 0x40, 0xe8, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_124[] = {
	0x02, 0x00, 0x29, 0x40, 0xe9, 0x0a, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_125[] = {
	0x02, 0x00, 0x29, 0x40, 0xea, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_126[] = {
	0x02, 0x00, 0x29, 0x40, 0xeb, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_127[] = {
	0x02, 0x00, 0x29, 0xc0, 0xec, 0x06, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_128[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_129[] = {
	0x02, 0x00, 0x29, 0x40, 0xc0, 0x07, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_130[] = {
	0x02, 0x00, 0x29, 0x40, 0xc1, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_131[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_132[] = {
	0x06, 0x00, 0x29, 0x40, 0xb2, 0x04, 0x00, 0x52,
	0x01, 0xce, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_133[] = {
	0x06, 0x00, 0x29, 0x40, 0xb3, 0x04, 0x00, 0x52,
	0x01, 0xce, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_134[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_135[] = {
	0x02, 0x00, 0x29, 0x40, 0xc4, 0x82, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_136[] = {
	0x02, 0x00, 0x29, 0x40, 0xc5, 0x80, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_137[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_138[] = {
	0x06, 0x00, 0x29, 0x40, 0xb6, 0x04, 0x00, 0x52,
	0x01, 0xce, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_139[] = {
	0x06, 0x00, 0x29, 0x40, 0xb7, 0x04, 0x00, 0x52,
	0x01, 0xce, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_140[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_141[] = {
	0x03, 0x00, 0x29, 0x40, 0xc8, 0x03, 0x20, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_142[] = {
	0x03, 0x00, 0x29, 0x40, 0xc9, 0x01, 0x21, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_143[] = {
	0x03, 0x00, 0x29, 0x40, 0xca, 0x03, 0x20, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_144[] = {
	0x03, 0x00, 0x29, 0x40, 0xcb, 0x07, 0x20, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_145[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_146[] = {
	0x02, 0x00, 0x29, 0x40, 0xc4, 0x60, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_147[] = {
	0x02, 0x00, 0x29, 0x40, 0xc5, 0x40, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_148[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_149[] = {
	0x06, 0x00, 0x29, 0x40, 0xba, 0x44, 0x00, 0x4b,
	0x01, 0xbe, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_150[] = {
	0x06, 0x00, 0x29, 0x40, 0xbb, 0x44, 0x00, 0x4b,
	0x01, 0xbe, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_151[] = {
	0x06, 0x00, 0x29, 0x40, 0xbc, 0x53, 0x00, 0x03,
	0x00, 0x4b, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_152[] = {
	0x06, 0x00, 0x29, 0x40, 0xbd, 0x53, 0x00, 0x03,
	0x00, 0x4b, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_153[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_154[] = {
	0x0c, 0x00, 0x29, 0x40, 0xd1, 0x03, 0x05, 0x00,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char yushun_nt35520_720p_cmd_on_cmd_155[] = {
	0x0c, 0x00, 0x29, 0x40, 0xd2, 0x03, 0x05, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char yushun_nt35520_720p_cmd_on_cmd_156[] = {
	0x0c, 0x00, 0x29, 0x40, 0xd3, 0x03, 0x05, 0x04,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char yushun_nt35520_720p_cmd_on_cmd_157[] = {
	0x0c, 0x00, 0x29, 0x40, 0xd4, 0x03, 0x05, 0x04,
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char yushun_nt35520_720p_cmd_on_cmd_158[] = {
	0x06, 0x00, 0x29, 0x40, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_159[] = {
	0x02, 0x00, 0x29, 0x40, 0xc4, 0x40, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_160[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x40, 0xff, 0xff
};
static char yushun_nt35520_720p_cmd_on_cmd_161[] = {
	0x11, 0x00, 0x05, 0x80
};
static char yushun_nt35520_720p_cmd_on_cmd_162[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd yushun_nt35520_720p_cmd_on_command[] = {
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_0), yushun_nt35520_720p_cmd_on_cmd_0, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_1), yushun_nt35520_720p_cmd_on_cmd_1, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_2), yushun_nt35520_720p_cmd_on_cmd_2, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_3), yushun_nt35520_720p_cmd_on_cmd_3, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_4), yushun_nt35520_720p_cmd_on_cmd_4, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_5), yushun_nt35520_720p_cmd_on_cmd_5, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_6), yushun_nt35520_720p_cmd_on_cmd_6, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_7), yushun_nt35520_720p_cmd_on_cmd_7, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_8), yushun_nt35520_720p_cmd_on_cmd_8, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_9), yushun_nt35520_720p_cmd_on_cmd_9, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_10), yushun_nt35520_720p_cmd_on_cmd_10, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_11), yushun_nt35520_720p_cmd_on_cmd_11, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_12), yushun_nt35520_720p_cmd_on_cmd_12, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_13), yushun_nt35520_720p_cmd_on_cmd_13, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_14), yushun_nt35520_720p_cmd_on_cmd_14, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_15), yushun_nt35520_720p_cmd_on_cmd_15, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_16), yushun_nt35520_720p_cmd_on_cmd_16, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_17), yushun_nt35520_720p_cmd_on_cmd_17, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_18), yushun_nt35520_720p_cmd_on_cmd_18, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_19), yushun_nt35520_720p_cmd_on_cmd_19, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_20), yushun_nt35520_720p_cmd_on_cmd_20, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_21), yushun_nt35520_720p_cmd_on_cmd_21, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_22), yushun_nt35520_720p_cmd_on_cmd_22, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_23), yushun_nt35520_720p_cmd_on_cmd_23, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_24), yushun_nt35520_720p_cmd_on_cmd_24, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_25), yushun_nt35520_720p_cmd_on_cmd_25, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_26), yushun_nt35520_720p_cmd_on_cmd_26, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_27), yushun_nt35520_720p_cmd_on_cmd_27, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_28), yushun_nt35520_720p_cmd_on_cmd_28, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_29), yushun_nt35520_720p_cmd_on_cmd_29, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_30), yushun_nt35520_720p_cmd_on_cmd_30, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_31), yushun_nt35520_720p_cmd_on_cmd_31, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_32), yushun_nt35520_720p_cmd_on_cmd_32, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_33), yushun_nt35520_720p_cmd_on_cmd_33, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_34), yushun_nt35520_720p_cmd_on_cmd_34, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_35), yushun_nt35520_720p_cmd_on_cmd_35, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_36), yushun_nt35520_720p_cmd_on_cmd_36, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_37), yushun_nt35520_720p_cmd_on_cmd_37, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_38), yushun_nt35520_720p_cmd_on_cmd_38, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_39), yushun_nt35520_720p_cmd_on_cmd_39, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_40), yushun_nt35520_720p_cmd_on_cmd_40, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_41), yushun_nt35520_720p_cmd_on_cmd_41, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_42), yushun_nt35520_720p_cmd_on_cmd_42, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_43), yushun_nt35520_720p_cmd_on_cmd_43, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_44), yushun_nt35520_720p_cmd_on_cmd_44, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_45), yushun_nt35520_720p_cmd_on_cmd_45, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_46), yushun_nt35520_720p_cmd_on_cmd_46, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_47), yushun_nt35520_720p_cmd_on_cmd_47, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_48), yushun_nt35520_720p_cmd_on_cmd_48, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_49), yushun_nt35520_720p_cmd_on_cmd_49, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_50), yushun_nt35520_720p_cmd_on_cmd_50, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_51), yushun_nt35520_720p_cmd_on_cmd_51, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_52), yushun_nt35520_720p_cmd_on_cmd_52, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_53), yushun_nt35520_720p_cmd_on_cmd_53, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_54), yushun_nt35520_720p_cmd_on_cmd_54, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_55), yushun_nt35520_720p_cmd_on_cmd_55, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_56), yushun_nt35520_720p_cmd_on_cmd_56, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_57), yushun_nt35520_720p_cmd_on_cmd_57, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_58), yushun_nt35520_720p_cmd_on_cmd_58, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_59), yushun_nt35520_720p_cmd_on_cmd_59, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_60), yushun_nt35520_720p_cmd_on_cmd_60, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_61), yushun_nt35520_720p_cmd_on_cmd_61, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_62), yushun_nt35520_720p_cmd_on_cmd_62, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_63), yushun_nt35520_720p_cmd_on_cmd_63, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_64), yushun_nt35520_720p_cmd_on_cmd_64, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_65), yushun_nt35520_720p_cmd_on_cmd_65, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_66), yushun_nt35520_720p_cmd_on_cmd_66, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_67), yushun_nt35520_720p_cmd_on_cmd_67, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_68), yushun_nt35520_720p_cmd_on_cmd_68, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_69), yushun_nt35520_720p_cmd_on_cmd_69, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_70), yushun_nt35520_720p_cmd_on_cmd_70, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_71), yushun_nt35520_720p_cmd_on_cmd_71, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_72), yushun_nt35520_720p_cmd_on_cmd_72, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_73), yushun_nt35520_720p_cmd_on_cmd_73, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_74), yushun_nt35520_720p_cmd_on_cmd_74, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_75), yushun_nt35520_720p_cmd_on_cmd_75, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_76), yushun_nt35520_720p_cmd_on_cmd_76, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_77), yushun_nt35520_720p_cmd_on_cmd_77, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_78), yushun_nt35520_720p_cmd_on_cmd_78, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_79), yushun_nt35520_720p_cmd_on_cmd_79, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_80), yushun_nt35520_720p_cmd_on_cmd_80, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_81), yushun_nt35520_720p_cmd_on_cmd_81, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_82), yushun_nt35520_720p_cmd_on_cmd_82, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_83), yushun_nt35520_720p_cmd_on_cmd_83, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_84), yushun_nt35520_720p_cmd_on_cmd_84, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_85), yushun_nt35520_720p_cmd_on_cmd_85, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_86), yushun_nt35520_720p_cmd_on_cmd_86, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_87), yushun_nt35520_720p_cmd_on_cmd_87, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_88), yushun_nt35520_720p_cmd_on_cmd_88, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_89), yushun_nt35520_720p_cmd_on_cmd_89, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_90), yushun_nt35520_720p_cmd_on_cmd_90, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_91), yushun_nt35520_720p_cmd_on_cmd_91, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_92), yushun_nt35520_720p_cmd_on_cmd_92, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_93), yushun_nt35520_720p_cmd_on_cmd_93, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_94), yushun_nt35520_720p_cmd_on_cmd_94, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_95), yushun_nt35520_720p_cmd_on_cmd_95, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_96), yushun_nt35520_720p_cmd_on_cmd_96, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_97), yushun_nt35520_720p_cmd_on_cmd_97, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_98), yushun_nt35520_720p_cmd_on_cmd_98, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_99), yushun_nt35520_720p_cmd_on_cmd_99, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_100), yushun_nt35520_720p_cmd_on_cmd_100, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_101), yushun_nt35520_720p_cmd_on_cmd_101, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_102), yushun_nt35520_720p_cmd_on_cmd_102, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_103), yushun_nt35520_720p_cmd_on_cmd_103, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_104), yushun_nt35520_720p_cmd_on_cmd_104, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_105), yushun_nt35520_720p_cmd_on_cmd_105, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_106), yushun_nt35520_720p_cmd_on_cmd_106, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_107), yushun_nt35520_720p_cmd_on_cmd_107, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_108), yushun_nt35520_720p_cmd_on_cmd_108, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_109), yushun_nt35520_720p_cmd_on_cmd_109, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_110), yushun_nt35520_720p_cmd_on_cmd_110, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_111), yushun_nt35520_720p_cmd_on_cmd_111, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_112), yushun_nt35520_720p_cmd_on_cmd_112, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_113), yushun_nt35520_720p_cmd_on_cmd_113, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_114), yushun_nt35520_720p_cmd_on_cmd_114, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_115), yushun_nt35520_720p_cmd_on_cmd_115, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_116), yushun_nt35520_720p_cmd_on_cmd_116, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_117), yushun_nt35520_720p_cmd_on_cmd_117, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_118), yushun_nt35520_720p_cmd_on_cmd_118, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_119), yushun_nt35520_720p_cmd_on_cmd_119, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_120), yushun_nt35520_720p_cmd_on_cmd_120, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_121), yushun_nt35520_720p_cmd_on_cmd_121, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_122), yushun_nt35520_720p_cmd_on_cmd_122, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_123), yushun_nt35520_720p_cmd_on_cmd_123, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_124), yushun_nt35520_720p_cmd_on_cmd_124, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_125), yushun_nt35520_720p_cmd_on_cmd_125, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_126), yushun_nt35520_720p_cmd_on_cmd_126, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_127), yushun_nt35520_720p_cmd_on_cmd_127, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_128), yushun_nt35520_720p_cmd_on_cmd_128, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_129), yushun_nt35520_720p_cmd_on_cmd_129, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_130), yushun_nt35520_720p_cmd_on_cmd_130, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_131), yushun_nt35520_720p_cmd_on_cmd_131, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_132), yushun_nt35520_720p_cmd_on_cmd_132, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_133), yushun_nt35520_720p_cmd_on_cmd_133, 120 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_134), yushun_nt35520_720p_cmd_on_cmd_134, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_135), yushun_nt35520_720p_cmd_on_cmd_135, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_136), yushun_nt35520_720p_cmd_on_cmd_136, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_137), yushun_nt35520_720p_cmd_on_cmd_137, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_138), yushun_nt35520_720p_cmd_on_cmd_138, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_139), yushun_nt35520_720p_cmd_on_cmd_139, 120 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_140), yushun_nt35520_720p_cmd_on_cmd_140, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_141), yushun_nt35520_720p_cmd_on_cmd_141, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_142), yushun_nt35520_720p_cmd_on_cmd_142, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_143), yushun_nt35520_720p_cmd_on_cmd_143, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_144), yushun_nt35520_720p_cmd_on_cmd_144, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_145), yushun_nt35520_720p_cmd_on_cmd_145, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_146), yushun_nt35520_720p_cmd_on_cmd_146, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_147), yushun_nt35520_720p_cmd_on_cmd_147, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_148), yushun_nt35520_720p_cmd_on_cmd_148, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_149), yushun_nt35520_720p_cmd_on_cmd_149, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_150), yushun_nt35520_720p_cmd_on_cmd_150, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_151), yushun_nt35520_720p_cmd_on_cmd_151, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_152), yushun_nt35520_720p_cmd_on_cmd_152, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_153), yushun_nt35520_720p_cmd_on_cmd_153, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_154), yushun_nt35520_720p_cmd_on_cmd_154, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_155), yushun_nt35520_720p_cmd_on_cmd_155, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_156), yushun_nt35520_720p_cmd_on_cmd_156, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_157), yushun_nt35520_720p_cmd_on_cmd_157, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_158), yushun_nt35520_720p_cmd_on_cmd_158, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_159), yushun_nt35520_720p_cmd_on_cmd_159, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_160), yushun_nt35520_720p_cmd_on_cmd_160, 0 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_161), yushun_nt35520_720p_cmd_on_cmd_161, 120 },
	{ sizeof(yushun_nt35520_720p_cmd_on_cmd_162), yushun_nt35520_720p_cmd_on_cmd_162, 0 },
};

static char yushun_nt35520_720p_cmd_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char yushun_nt35520_720p_cmd_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd yushun_nt35520_720p_cmd_off_command[] = {
	{ sizeof(yushun_nt35520_720p_cmd_off_cmd_0), yushun_nt35520_720p_cmd_off_cmd_0, 50 },
	{ sizeof(yushun_nt35520_720p_cmd_off_cmd_1), yushun_nt35520_720p_cmd_off_cmd_1, 120 },
};

static struct command_state yushun_nt35520_720p_cmd_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info yushun_nt35520_720p_cmd_command_panel = {
	/* FIXME: This is a command mode panel */
};

static struct videopanel_info yushun_nt35520_720p_cmd_video_panel = {
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

static struct lane_configuration yushun_nt35520_720p_cmd_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t yushun_nt35520_720p_cmd_timings[] = {
	0x7a, 0x1a, 0x12, 0x00, 0x40, 0x42, 0x16, 0x1e, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing yushun_nt35520_720p_cmd_timing_info = {
	.tclk_post = 0x20,
	.tclk_pre = 0x2d,
};

static struct panel_reset_sequence yushun_nt35520_720p_cmd_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 20, 20 },
	.pin_direction = 2,
};

static struct backlight yushun_nt35520_720p_cmd_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_yushun_nt35520_720p_cmd_select(struct panel_struct *panel,
							struct msm_panel_info *pinfo,
							struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &yushun_nt35520_720p_cmd_panel_data;
	panel->panelres = &yushun_nt35520_720p_cmd_panel_res;
	panel->color = &yushun_nt35520_720p_cmd_color;
	panel->videopanel = &yushun_nt35520_720p_cmd_video_panel;
	panel->commandpanel = &yushun_nt35520_720p_cmd_command_panel;
	panel->state = &yushun_nt35520_720p_cmd_state;
	panel->laneconfig = &yushun_nt35520_720p_cmd_lane_config;
	panel->paneltiminginfo = &yushun_nt35520_720p_cmd_timing_info;
	panel->panelresetseq = &yushun_nt35520_720p_cmd_reset_seq;
	panel->backlightinfo = &yushun_nt35520_720p_cmd_backlight;
	pinfo->mipi.panel_cmds = yushun_nt35520_720p_cmd_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(yushun_nt35520_720p_cmd_on_command);
	memcpy(phy_db->timing, yushun_nt35520_720p_cmd_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_YUSHUN_NT35520_720P_CMD_H_ */
