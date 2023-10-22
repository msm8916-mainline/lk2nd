// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2023 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_ORISE8012A_TMFWVGA_CMD_H_
#define _PANEL_ORISE8012A_TMFWVGA_CMD_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config orise8012a_tmfwvga_cmd_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_orise8012a_tmfwvga_cmd",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 1,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 10000,
};

static struct panel_resolution orise8012a_tmfwvga_cmd_panel_res = {
	.panel_width = 480,
	.panel_height = 854,
	.hfront_porch = 76,
	.hback_porch = 76,
	.hpulse_width = 70,
	.hsync_skew = 0,
	.vfront_porch = 6,
	.vback_porch = 27,
	.vpulse_width = 5,
	/* Borders not supported yet */
};

static struct color_info orise8012a_tmfwvga_cmd_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char orise8012a_tmfwvga_cmd_on_cmd_0[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_1[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0x80, 0x12, 0x01
};
static char orise8012a_tmfwvga_cmd_on_cmd_2[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_3[] = {
	0x03, 0x00, 0x39, 0xc0, 0xff, 0x80, 0x12, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_4[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb4, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_5[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x55, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_7[] = {
	0x04, 0x00, 0x39, 0xc0, 0xc5, 0x02, 0x76, 0x01
};
static char orise8012a_tmfwvga_cmd_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_9[] = {
	0x03, 0x00, 0x39, 0xc0, 0xd8, 0x6a, 0x6a, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_10[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x81, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_11[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0x57, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_13[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb3, 0x02, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_14[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_15[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc1, 0x25, 0x55, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_16[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x81, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_17[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x83, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_18[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x82, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_19[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0xa0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_20[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb1, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_21[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc5, 0x20, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_22[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc6, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_23[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0x01, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_24[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_25[] = {
	0x0d, 0x00, 0x39, 0xc0, 0xce, 0x8a, 0x03, 0x00,
	0x89, 0x03, 0x00, 0x88, 0x03, 0x00, 0x87, 0x03,
	0x00, 0xff, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_26[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_27[] = {
	0x0d, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x0e, 0x00,
	0x38, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_28[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_29[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x06, 0x03,
	0x56, 0x00, 0x18, 0x00, 0x38, 0x05, 0x03, 0x57,
	0x00, 0x18, 0x00, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_30[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_31[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x04, 0x03,
	0x58, 0x00, 0x18, 0x00, 0x38, 0x03, 0x03, 0x59,
	0x00, 0x18, 0x00, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_32[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_33[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x02, 0x03,
	0x5a, 0x00, 0x18, 0x00, 0x38, 0x01, 0x03, 0x5b,
	0x00, 0x18, 0x00, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_34[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_35[] = {
	0x0f, 0x00, 0x39, 0xc0, 0xce, 0x38, 0x00, 0x03,
	0x5c, 0x00, 0x18, 0x00, 0x30, 0x00, 0x03, 0x5d,
	0x00, 0x18, 0x00, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_36[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_37[] = {
	0x0c, 0x00, 0x39, 0xc0, 0xcf, 0x02, 0x02, 0x20,
	0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00
};
static char orise8012a_tmfwvga_cmd_on_cmd_38[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_39[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x15, 0x15, 0x00,
	0x00, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
	0x00, 0x00, 0x00, 0x00
};
static char orise8012a_tmfwvga_cmd_on_cmd_40[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_41[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15,
	0x15, 0x15, 0x15, 0x15
};
static char orise8012a_tmfwvga_cmd_on_cmd_42[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xe0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_43[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcb, 0x15, 0x15, 0x00,
	0x00, 0x15, 0x15, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_44[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_45[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcc, 0x26, 0x25, 0x00,
	0x00, 0x0c, 0x0a, 0x10, 0x0e, 0x02, 0x04, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_46[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_47[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char orise8012a_tmfwvga_cmd_on_cmd_48[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_49[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x05, 0x03, 0x01,
	0x0d, 0x0f, 0x09, 0x0b, 0x00, 0x00, 0x25, 0x26,
	0x00, 0x00, 0x00, 0x00
};
static char orise8012a_tmfwvga_cmd_on_cmd_50[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_51[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xcc, 0x25, 0x26, 0x00,
	0x00, 0x0b, 0x0d, 0x0f, 0x09, 0x01, 0x03, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_52[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_53[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char orise8012a_tmfwvga_cmd_on_cmd_54[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_55[] = {
	0x10, 0x00, 0x39, 0xc0, 0xcc, 0x06, 0x04, 0x02,
	0x0a, 0x10, 0x0e, 0x0c, 0x00, 0x00, 0x26, 0x25,
	0x00, 0x00, 0x00, 0x00
};
static char orise8012a_tmfwvga_cmd_on_cmd_56[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_57[] = {
	0x15, 0x00, 0x39, 0xc0, 0xe1, 0x10, 0x3d, 0x4d,
	0x5b, 0x6d, 0x7a, 0x7c, 0xa6, 0x95, 0xad, 0x58,
	0x44, 0x58, 0x37, 0x36, 0x2b, 0x1e, 0x11, 0x0a,
	0x03, 0xff, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_58[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_59[] = {
	0x15, 0x00, 0x39, 0xc0, 0xe2, 0x10, 0x3d, 0x4d,
	0x5b, 0x6d, 0x7a, 0x7c, 0xa6, 0x95, 0xad, 0x58,
	0x44, 0x58, 0x37, 0x36, 0x2b, 0x1e, 0x11, 0x0a,
	0x03, 0xff, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_60[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_61[] = {
	0x22, 0x00, 0x39, 0xc0, 0xec, 0x40, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
	0x44, 0x44, 0x44, 0x44, 0x44, 0x04, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_62[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_63[] = {
	0x22, 0x00, 0x39, 0xc0, 0xed, 0x40, 0x43, 0x43,
	0x34, 0x34, 0x44, 0x43, 0x43, 0x34, 0x34, 0x34,
	0x44, 0x43, 0x43, 0x34, 0x34, 0x44, 0x43, 0x43,
	0x34, 0x34, 0x44, 0x43, 0x43, 0x34, 0x34, 0x34,
	0x44, 0x43, 0x43, 0x34, 0x34, 0x04, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_64[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_65[] = {
	0x22, 0x00, 0x39, 0xc0, 0xee, 0x40, 0x34, 0x44,
	0x34, 0x44, 0x34, 0x44, 0x34, 0x44, 0x34, 0x44,
	0x34, 0x44, 0x34, 0x44, 0x34, 0x44, 0x34, 0x44,
	0x34, 0x44, 0x34, 0x44, 0x34, 0x44, 0x34, 0x44,
	0x34, 0x44, 0x34, 0x44, 0x34, 0x04, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_66[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x83, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_67[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x50, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_68[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_69[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x30, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_70[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x8a, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_71[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x40, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_72[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x92, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_73[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc4, 0x08, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_74[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xb1, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_75[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0xb2, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_76[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x91, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_77[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc1, 0x08, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_78[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xaa, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_79[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0xaa, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_80[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_81[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc0, 0xc0, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_82[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_83[] = {
	0x03, 0x00, 0x39, 0xc0, 0xff, 0xff, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_84[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_85[] = {
	0x04, 0x00, 0x39, 0xc0, 0xff, 0xff, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_86[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_87[] = {
	0x03, 0x00, 0x39, 0xc0, 0x99, 0x35, 0x45, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_88[] = {
	0x35, 0x00, 0x05, 0x80
};
static char orise8012a_tmfwvga_cmd_on_cmd_89[] = {
	0x11, 0x00, 0x05, 0x80
};
static char orise8012a_tmfwvga_cmd_on_cmd_90[] = {
	0x29, 0x00, 0x05, 0x80
};
static char orise8012a_tmfwvga_cmd_on_cmd_91[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_on_cmd_92[] = {
	0x03, 0x00, 0x39, 0xc0, 0x99, 0x00, 0x00, 0xff
};

static struct mipi_dsi_cmd orise8012a_tmfwvga_cmd_on_command[] = {
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_0), orise8012a_tmfwvga_cmd_on_cmd_0, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_1), orise8012a_tmfwvga_cmd_on_cmd_1, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_2), orise8012a_tmfwvga_cmd_on_cmd_2, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_3), orise8012a_tmfwvga_cmd_on_cmd_3, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_4), orise8012a_tmfwvga_cmd_on_cmd_4, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_5), orise8012a_tmfwvga_cmd_on_cmd_5, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_6), orise8012a_tmfwvga_cmd_on_cmd_6, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_7), orise8012a_tmfwvga_cmd_on_cmd_7, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_8), orise8012a_tmfwvga_cmd_on_cmd_8, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_9), orise8012a_tmfwvga_cmd_on_cmd_9, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_10), orise8012a_tmfwvga_cmd_on_cmd_10, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_11), orise8012a_tmfwvga_cmd_on_cmd_11, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_12), orise8012a_tmfwvga_cmd_on_cmd_12, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_13), orise8012a_tmfwvga_cmd_on_cmd_13, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_14), orise8012a_tmfwvga_cmd_on_cmd_14, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_15), orise8012a_tmfwvga_cmd_on_cmd_15, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_16), orise8012a_tmfwvga_cmd_on_cmd_16, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_17), orise8012a_tmfwvga_cmd_on_cmd_17, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_18), orise8012a_tmfwvga_cmd_on_cmd_18, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_19), orise8012a_tmfwvga_cmd_on_cmd_19, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_20), orise8012a_tmfwvga_cmd_on_cmd_20, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_21), orise8012a_tmfwvga_cmd_on_cmd_21, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_22), orise8012a_tmfwvga_cmd_on_cmd_22, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_23), orise8012a_tmfwvga_cmd_on_cmd_23, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_24), orise8012a_tmfwvga_cmd_on_cmd_24, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_25), orise8012a_tmfwvga_cmd_on_cmd_25, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_26), orise8012a_tmfwvga_cmd_on_cmd_26, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_27), orise8012a_tmfwvga_cmd_on_cmd_27, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_28), orise8012a_tmfwvga_cmd_on_cmd_28, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_29), orise8012a_tmfwvga_cmd_on_cmd_29, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_30), orise8012a_tmfwvga_cmd_on_cmd_30, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_31), orise8012a_tmfwvga_cmd_on_cmd_31, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_32), orise8012a_tmfwvga_cmd_on_cmd_32, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_33), orise8012a_tmfwvga_cmd_on_cmd_33, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_34), orise8012a_tmfwvga_cmd_on_cmd_34, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_35), orise8012a_tmfwvga_cmd_on_cmd_35, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_36), orise8012a_tmfwvga_cmd_on_cmd_36, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_37), orise8012a_tmfwvga_cmd_on_cmd_37, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_38), orise8012a_tmfwvga_cmd_on_cmd_38, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_39), orise8012a_tmfwvga_cmd_on_cmd_39, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_40), orise8012a_tmfwvga_cmd_on_cmd_40, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_41), orise8012a_tmfwvga_cmd_on_cmd_41, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_42), orise8012a_tmfwvga_cmd_on_cmd_42, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_43), orise8012a_tmfwvga_cmd_on_cmd_43, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_44), orise8012a_tmfwvga_cmd_on_cmd_44, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_45), orise8012a_tmfwvga_cmd_on_cmd_45, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_46), orise8012a_tmfwvga_cmd_on_cmd_46, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_47), orise8012a_tmfwvga_cmd_on_cmd_47, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_48), orise8012a_tmfwvga_cmd_on_cmd_48, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_49), orise8012a_tmfwvga_cmd_on_cmd_49, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_50), orise8012a_tmfwvga_cmd_on_cmd_50, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_51), orise8012a_tmfwvga_cmd_on_cmd_51, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_52), orise8012a_tmfwvga_cmd_on_cmd_52, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_53), orise8012a_tmfwvga_cmd_on_cmd_53, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_54), orise8012a_tmfwvga_cmd_on_cmd_54, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_55), orise8012a_tmfwvga_cmd_on_cmd_55, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_56), orise8012a_tmfwvga_cmd_on_cmd_56, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_57), orise8012a_tmfwvga_cmd_on_cmd_57, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_58), orise8012a_tmfwvga_cmd_on_cmd_58, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_59), orise8012a_tmfwvga_cmd_on_cmd_59, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_60), orise8012a_tmfwvga_cmd_on_cmd_60, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_61), orise8012a_tmfwvga_cmd_on_cmd_61, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_62), orise8012a_tmfwvga_cmd_on_cmd_62, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_63), orise8012a_tmfwvga_cmd_on_cmd_63, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_64), orise8012a_tmfwvga_cmd_on_cmd_64, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_65), orise8012a_tmfwvga_cmd_on_cmd_65, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_66), orise8012a_tmfwvga_cmd_on_cmd_66, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_67), orise8012a_tmfwvga_cmd_on_cmd_67, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_68), orise8012a_tmfwvga_cmd_on_cmd_68, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_69), orise8012a_tmfwvga_cmd_on_cmd_69, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_70), orise8012a_tmfwvga_cmd_on_cmd_70, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_71), orise8012a_tmfwvga_cmd_on_cmd_71, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_72), orise8012a_tmfwvga_cmd_on_cmd_72, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_73), orise8012a_tmfwvga_cmd_on_cmd_73, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_74), orise8012a_tmfwvga_cmd_on_cmd_74, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_75), orise8012a_tmfwvga_cmd_on_cmd_75, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_76), orise8012a_tmfwvga_cmd_on_cmd_76, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_77), orise8012a_tmfwvga_cmd_on_cmd_77, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_78), orise8012a_tmfwvga_cmd_on_cmd_78, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_79), orise8012a_tmfwvga_cmd_on_cmd_79, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_80), orise8012a_tmfwvga_cmd_on_cmd_80, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_81), orise8012a_tmfwvga_cmd_on_cmd_81, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_82), orise8012a_tmfwvga_cmd_on_cmd_82, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_83), orise8012a_tmfwvga_cmd_on_cmd_83, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_84), orise8012a_tmfwvga_cmd_on_cmd_84, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_85), orise8012a_tmfwvga_cmd_on_cmd_85, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_86), orise8012a_tmfwvga_cmd_on_cmd_86, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_87), orise8012a_tmfwvga_cmd_on_cmd_87, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_88), orise8012a_tmfwvga_cmd_on_cmd_88, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_89), orise8012a_tmfwvga_cmd_on_cmd_89, 120 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_90), orise8012a_tmfwvga_cmd_on_cmd_90, 20 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_91), orise8012a_tmfwvga_cmd_on_cmd_91, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_on_cmd_92), orise8012a_tmfwvga_cmd_on_cmd_92, 0 },
};

static char orise8012a_tmfwvga_cmd_off_cmd_0[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_off_cmd_1[] = {
	0x03, 0x00, 0x39, 0xc0, 0x99, 0x35, 0x45, 0xff
};
static char orise8012a_tmfwvga_cmd_off_cmd_2[] = {
	0x28, 0x00, 0x05, 0x80
};
static char orise8012a_tmfwvga_cmd_off_cmd_3[] = {
	0x10, 0x00, 0x05, 0x80
};
static char orise8012a_tmfwvga_cmd_off_cmd_4[] = {
	0x02, 0x00, 0x39, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char orise8012a_tmfwvga_cmd_off_cmd_5[] = {
	0x03, 0x00, 0x39, 0xc0, 0x99, 0x00, 0x00, 0xff
};

static struct mipi_dsi_cmd orise8012a_tmfwvga_cmd_off_command[] = {
	{ sizeof(orise8012a_tmfwvga_cmd_off_cmd_0), orise8012a_tmfwvga_cmd_off_cmd_0, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_off_cmd_1), orise8012a_tmfwvga_cmd_off_cmd_1, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_off_cmd_2), orise8012a_tmfwvga_cmd_off_cmd_2, 50 },
	{ sizeof(orise8012a_tmfwvga_cmd_off_cmd_3), orise8012a_tmfwvga_cmd_off_cmd_3, 120 },
	{ sizeof(orise8012a_tmfwvga_cmd_off_cmd_4), orise8012a_tmfwvga_cmd_off_cmd_4, 0 },
	{ sizeof(orise8012a_tmfwvga_cmd_off_cmd_5), orise8012a_tmfwvga_cmd_off_cmd_5, 0 },
};

static struct command_state orise8012a_tmfwvga_cmd_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info orise8012a_tmfwvga_cmd_command_panel = {
	/* FIXME: This is a command mode panel */
};

static struct videopanel_info orise8012a_tmfwvga_cmd_video_panel = {
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

static struct lane_configuration orise8012a_tmfwvga_cmd_lane_config = {
	.dsi_lanes = 2,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 0,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t orise8012a_tmfwvga_cmd_timings[] = {
	0x83, 0x1c, 0x12, 0x00, 0x40, 0x42, 0x18, 0x28, 0x23, 0x03, 0x04, 0x00
};

static struct panel_timing orise8012a_tmfwvga_cmd_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence orise8012a_tmfwvga_cmd_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 20, 20 },
	.pin_direction = 2,
};

static struct backlight orise8012a_tmfwvga_cmd_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 255,
};

static inline void panel_orise8012a_tmfwvga_cmd_select(struct panel_struct *panel,
						       struct msm_panel_info *pinfo,
						       struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &orise8012a_tmfwvga_cmd_panel_data;
	panel->panelres = &orise8012a_tmfwvga_cmd_panel_res;
	panel->color = &orise8012a_tmfwvga_cmd_color;
	panel->videopanel = &orise8012a_tmfwvga_cmd_video_panel;
	panel->commandpanel = &orise8012a_tmfwvga_cmd_command_panel;
	panel->state = &orise8012a_tmfwvga_cmd_state;
	panel->laneconfig = &orise8012a_tmfwvga_cmd_lane_config;
	panel->paneltiminginfo = &orise8012a_tmfwvga_cmd_timing_info;
	panel->panelresetseq = &orise8012a_tmfwvga_cmd_reset_seq;
	panel->backlightinfo = &orise8012a_tmfwvga_cmd_backlight;
	pinfo->mipi.panel_cmds = orise8012a_tmfwvga_cmd_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(orise8012a_tmfwvga_cmd_on_command);
	memcpy(phy_db->timing, orise8012a_tmfwvga_cmd_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_ORISE8012A_TMFWVGA_CMD_H_ */
