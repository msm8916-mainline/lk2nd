// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved.

#ifndef _PANEL_BOE_OTM8019A_5P0_FWVGA_VIDEO_H_
#define _PANEL_BOE_OTM8019A_5P0_FWVGA_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config boe_otm8019a_5p0_fwvga_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_boe_otm8019a_5p0_fwvga_video",
	.panel_controller = "dsi:0",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution boe_otm8019a_5p0_fwvga_video_panel_res = {
	.panel_width = 480,
	.panel_height = 854,
	.hfront_porch = 92,
	.hback_porch = 88,
	.hpulse_width = 12,
	.hsync_skew = 0,
	.vfront_porch = 18,
	.vback_porch = 18,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info boe_otm8019a_5p0_fwvga_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char boe_otm8019a_5p0_fwvga_video_on_cmd_0[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_1[] = {
	0x04, 0x00, 0x29, 0xc0, 0xff, 0x80, 0x19, 0x01
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_2[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_3[] = {
	0x03, 0x00, 0x29, 0xc0, 0xff, 0x80, 0x19, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_4[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x03, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_5[] = {
	0x02, 0x00, 0x29, 0xc0, 0xff, 0x01, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_6[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_7[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb3, 0x02, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_8[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x92, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_9[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb3, 0x45, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_10[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa2, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_11[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0x1b, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_12[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_13[] = {
	0x0a, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0x58, 0x00,
	0x14, 0x16, 0x00, 0x58, 0x14, 0x16, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_14[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb4, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_15[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_16[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb5, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_17[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x18, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_18[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x81, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_19[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x04, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_20[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x8a, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_21[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x40, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_22[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_23[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc1, 0x03, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_24[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_25[] = {
	0x07, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0x15, 0x00,
	0x00, 0x00, 0x03, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_26[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_27[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x30, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_28[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_29[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc1, 0xe8, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_30[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x98, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_31[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_32[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa9, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_33[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x0a, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_34[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_35[] = {
	0x04, 0x00, 0x29, 0xc0, 0xc1, 0x20, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_36[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xe1, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_37[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc0, 0x40, 0x30, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_38[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_39[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb6, 0xb4, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_40[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x87, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_41[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x18, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_42[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x89, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_43[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x08, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_44[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x82, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_45[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x03, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_46[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_47[] = {
	0x06, 0x00, 0x29, 0xc0, 0xc5, 0x4e, 0x29, 0x00,
	0x7b, 0x44, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_48[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_49[] = {
	0x02, 0x00, 0x29, 0xc0, 0xd8, 0x5f, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_50[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x01, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_51[] = {
	0x02, 0x00, 0x29, 0xc0, 0xd8, 0x5f, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_52[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x81, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_53[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc1, 0x03, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_54[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa1, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_55[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc1, 0x08, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_56[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb1, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_57[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x29, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_58[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_59[] = {
	0x15, 0x00, 0x29, 0xc0, 0xe1, 0x1c, 0x2a, 0x34,
	0x40, 0x4d, 0x5c, 0x5c, 0x82, 0x73, 0x8b, 0x7a,
	0x65, 0x77, 0x55, 0x52, 0x41, 0x34, 0x29, 0x23,
	0x1f, 0xff, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_60[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_61[] = {
	0x15, 0x00, 0x29, 0xc0, 0xe2, 0x1c, 0x2a, 0x34,
	0x40, 0x4d, 0x5c, 0x5c, 0x82, 0x73, 0x8b, 0x7a,
	0x65, 0x77, 0x55, 0x52, 0x41, 0x34, 0x29, 0x23,
	0x1f, 0xff, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_62[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_63[] = {
	0x0d, 0x00, 0x29, 0xc0, 0xce, 0x86, 0x01, 0x00,
	0x85, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_64[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_65[] = {
	0x0f, 0x00, 0x29, 0xc0, 0xce, 0x18, 0x05, 0x83,
	0x5a, 0x86, 0x04, 0x00, 0x18, 0x04, 0x83, 0x5b,
	0x86, 0x04, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_66[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_67[] = {
	0x0f, 0x00, 0x29, 0xc0, 0xce, 0x18, 0x03, 0x83,
	0x5c, 0x86, 0x04, 0x00, 0x18, 0x02, 0x83, 0x5d,
	0x86, 0x04, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_68[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_69[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcf, 0x01, 0x01, 0x20,
	0x20, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_70[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_71[] = {
	0x02, 0x00, 0x29, 0xc0, 0xcf, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_72[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_73[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_74[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_75[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_76[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_77[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_78[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_79[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_80[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_81[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcb, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_82[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_83[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_84[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xe0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_85[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcb, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_86[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xf0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_87[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcb, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_88[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x80, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_89[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x26, 0x09,
	0x0b, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_90[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x90, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_91[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_92[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xa0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_93[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x25, 0x02, 0x0c, 0x0a, 0x26, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_94[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_95[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x25, 0x0a,
	0x0c, 0x02, 0x26, 0x00, 0x00, 0x00, 0x00, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_96[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xc0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_97[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_98[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xd0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_99[] = {
	0x10, 0x00, 0x29, 0xc0, 0xcc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x26, 0x01, 0x0b, 0x09, 0x25, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_100[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb0, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_101[] = {
	0x02, 0x00, 0x29, 0xc0, 0xca, 0x09, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_102[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0xb3, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_103[] = {
	0x02, 0x00, 0x29, 0xc0, 0xca, 0x50, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_104[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_105[] = {
	0x02, 0x00, 0x29, 0xc0, 0xfb, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_106[] = {
	0x02, 0x00, 0x29, 0xc0, 0x00, 0x00, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_107[] = {
	0x04, 0x00, 0x29, 0xc0, 0xff, 0xff, 0xff, 0xff
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_108[] = {
	0x51, 0x00, 0x15, 0x80
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_109[] = {
	0x53, 0x24, 0x15, 0x80
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_110[] = {
	0x55, 0x01, 0x15, 0x80
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_111[] = {
	0x5e, 0x28, 0x15, 0x80
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_112[] = {
	0x11, 0x00, 0x05, 0x80
};
static char boe_otm8019a_5p0_fwvga_video_on_cmd_113[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boe_otm8019a_5p0_fwvga_video_on_command[] = {
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_0), boe_otm8019a_5p0_fwvga_video_on_cmd_0, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_1), boe_otm8019a_5p0_fwvga_video_on_cmd_1, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_2), boe_otm8019a_5p0_fwvga_video_on_cmd_2, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_3), boe_otm8019a_5p0_fwvga_video_on_cmd_3, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_4), boe_otm8019a_5p0_fwvga_video_on_cmd_4, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_5), boe_otm8019a_5p0_fwvga_video_on_cmd_5, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_6), boe_otm8019a_5p0_fwvga_video_on_cmd_6, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_7), boe_otm8019a_5p0_fwvga_video_on_cmd_7, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_8), boe_otm8019a_5p0_fwvga_video_on_cmd_8, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_9), boe_otm8019a_5p0_fwvga_video_on_cmd_9, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_10), boe_otm8019a_5p0_fwvga_video_on_cmd_10, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_11), boe_otm8019a_5p0_fwvga_video_on_cmd_11, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_12), boe_otm8019a_5p0_fwvga_video_on_cmd_12, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_13), boe_otm8019a_5p0_fwvga_video_on_cmd_13, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_14), boe_otm8019a_5p0_fwvga_video_on_cmd_14, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_15), boe_otm8019a_5p0_fwvga_video_on_cmd_15, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_16), boe_otm8019a_5p0_fwvga_video_on_cmd_16, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_17), boe_otm8019a_5p0_fwvga_video_on_cmd_17, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_18), boe_otm8019a_5p0_fwvga_video_on_cmd_18, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_19), boe_otm8019a_5p0_fwvga_video_on_cmd_19, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_20), boe_otm8019a_5p0_fwvga_video_on_cmd_20, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_21), boe_otm8019a_5p0_fwvga_video_on_cmd_21, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_22), boe_otm8019a_5p0_fwvga_video_on_cmd_22, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_23), boe_otm8019a_5p0_fwvga_video_on_cmd_23, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_24), boe_otm8019a_5p0_fwvga_video_on_cmd_24, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_25), boe_otm8019a_5p0_fwvga_video_on_cmd_25, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_26), boe_otm8019a_5p0_fwvga_video_on_cmd_26, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_27), boe_otm8019a_5p0_fwvga_video_on_cmd_27, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_28), boe_otm8019a_5p0_fwvga_video_on_cmd_28, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_29), boe_otm8019a_5p0_fwvga_video_on_cmd_29, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_30), boe_otm8019a_5p0_fwvga_video_on_cmd_30, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_31), boe_otm8019a_5p0_fwvga_video_on_cmd_31, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_32), boe_otm8019a_5p0_fwvga_video_on_cmd_32, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_33), boe_otm8019a_5p0_fwvga_video_on_cmd_33, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_34), boe_otm8019a_5p0_fwvga_video_on_cmd_34, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_35), boe_otm8019a_5p0_fwvga_video_on_cmd_35, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_36), boe_otm8019a_5p0_fwvga_video_on_cmd_36, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_37), boe_otm8019a_5p0_fwvga_video_on_cmd_37, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_38), boe_otm8019a_5p0_fwvga_video_on_cmd_38, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_39), boe_otm8019a_5p0_fwvga_video_on_cmd_39, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_40), boe_otm8019a_5p0_fwvga_video_on_cmd_40, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_41), boe_otm8019a_5p0_fwvga_video_on_cmd_41, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_42), boe_otm8019a_5p0_fwvga_video_on_cmd_42, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_43), boe_otm8019a_5p0_fwvga_video_on_cmd_43, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_44), boe_otm8019a_5p0_fwvga_video_on_cmd_44, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_45), boe_otm8019a_5p0_fwvga_video_on_cmd_45, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_46), boe_otm8019a_5p0_fwvga_video_on_cmd_46, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_47), boe_otm8019a_5p0_fwvga_video_on_cmd_47, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_48), boe_otm8019a_5p0_fwvga_video_on_cmd_48, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_49), boe_otm8019a_5p0_fwvga_video_on_cmd_49, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_50), boe_otm8019a_5p0_fwvga_video_on_cmd_50, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_51), boe_otm8019a_5p0_fwvga_video_on_cmd_51, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_52), boe_otm8019a_5p0_fwvga_video_on_cmd_52, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_53), boe_otm8019a_5p0_fwvga_video_on_cmd_53, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_54), boe_otm8019a_5p0_fwvga_video_on_cmd_54, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_55), boe_otm8019a_5p0_fwvga_video_on_cmd_55, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_56), boe_otm8019a_5p0_fwvga_video_on_cmd_56, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_57), boe_otm8019a_5p0_fwvga_video_on_cmd_57, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_58), boe_otm8019a_5p0_fwvga_video_on_cmd_58, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_59), boe_otm8019a_5p0_fwvga_video_on_cmd_59, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_60), boe_otm8019a_5p0_fwvga_video_on_cmd_60, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_61), boe_otm8019a_5p0_fwvga_video_on_cmd_61, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_62), boe_otm8019a_5p0_fwvga_video_on_cmd_62, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_63), boe_otm8019a_5p0_fwvga_video_on_cmd_63, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_64), boe_otm8019a_5p0_fwvga_video_on_cmd_64, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_65), boe_otm8019a_5p0_fwvga_video_on_cmd_65, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_66), boe_otm8019a_5p0_fwvga_video_on_cmd_66, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_67), boe_otm8019a_5p0_fwvga_video_on_cmd_67, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_68), boe_otm8019a_5p0_fwvga_video_on_cmd_68, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_69), boe_otm8019a_5p0_fwvga_video_on_cmd_69, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_70), boe_otm8019a_5p0_fwvga_video_on_cmd_70, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_71), boe_otm8019a_5p0_fwvga_video_on_cmd_71, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_72), boe_otm8019a_5p0_fwvga_video_on_cmd_72, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_73), boe_otm8019a_5p0_fwvga_video_on_cmd_73, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_74), boe_otm8019a_5p0_fwvga_video_on_cmd_74, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_75), boe_otm8019a_5p0_fwvga_video_on_cmd_75, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_76), boe_otm8019a_5p0_fwvga_video_on_cmd_76, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_77), boe_otm8019a_5p0_fwvga_video_on_cmd_77, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_78), boe_otm8019a_5p0_fwvga_video_on_cmd_78, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_79), boe_otm8019a_5p0_fwvga_video_on_cmd_79, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_80), boe_otm8019a_5p0_fwvga_video_on_cmd_80, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_81), boe_otm8019a_5p0_fwvga_video_on_cmd_81, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_82), boe_otm8019a_5p0_fwvga_video_on_cmd_82, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_83), boe_otm8019a_5p0_fwvga_video_on_cmd_83, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_84), boe_otm8019a_5p0_fwvga_video_on_cmd_84, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_85), boe_otm8019a_5p0_fwvga_video_on_cmd_85, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_86), boe_otm8019a_5p0_fwvga_video_on_cmd_86, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_87), boe_otm8019a_5p0_fwvga_video_on_cmd_87, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_88), boe_otm8019a_5p0_fwvga_video_on_cmd_88, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_89), boe_otm8019a_5p0_fwvga_video_on_cmd_89, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_90), boe_otm8019a_5p0_fwvga_video_on_cmd_90, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_91), boe_otm8019a_5p0_fwvga_video_on_cmd_91, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_92), boe_otm8019a_5p0_fwvga_video_on_cmd_92, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_93), boe_otm8019a_5p0_fwvga_video_on_cmd_93, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_94), boe_otm8019a_5p0_fwvga_video_on_cmd_94, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_95), boe_otm8019a_5p0_fwvga_video_on_cmd_95, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_96), boe_otm8019a_5p0_fwvga_video_on_cmd_96, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_97), boe_otm8019a_5p0_fwvga_video_on_cmd_97, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_98), boe_otm8019a_5p0_fwvga_video_on_cmd_98, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_99), boe_otm8019a_5p0_fwvga_video_on_cmd_99, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_100), boe_otm8019a_5p0_fwvga_video_on_cmd_100, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_101), boe_otm8019a_5p0_fwvga_video_on_cmd_101, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_102), boe_otm8019a_5p0_fwvga_video_on_cmd_102, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_103), boe_otm8019a_5p0_fwvga_video_on_cmd_103, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_104), boe_otm8019a_5p0_fwvga_video_on_cmd_104, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_105), boe_otm8019a_5p0_fwvga_video_on_cmd_105, 50 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_106), boe_otm8019a_5p0_fwvga_video_on_cmd_106, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_107), boe_otm8019a_5p0_fwvga_video_on_cmd_107, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_108), boe_otm8019a_5p0_fwvga_video_on_cmd_108, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_109), boe_otm8019a_5p0_fwvga_video_on_cmd_109, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_110), boe_otm8019a_5p0_fwvga_video_on_cmd_110, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_111), boe_otm8019a_5p0_fwvga_video_on_cmd_111, 0 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_112), boe_otm8019a_5p0_fwvga_video_on_cmd_112, 120 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_on_cmd_113), boe_otm8019a_5p0_fwvga_video_on_cmd_113, 20 },
};

static char boe_otm8019a_5p0_fwvga_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char boe_otm8019a_5p0_fwvga_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boe_otm8019a_5p0_fwvga_video_off_command[] = {
	{ sizeof(boe_otm8019a_5p0_fwvga_video_off_cmd_0), boe_otm8019a_5p0_fwvga_video_off_cmd_0, 20 },
	{ sizeof(boe_otm8019a_5p0_fwvga_video_off_cmd_1), boe_otm8019a_5p0_fwvga_video_off_cmd_1, 120 },
};

static struct command_state boe_otm8019a_5p0_fwvga_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info boe_otm8019a_5p0_fwvga_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info boe_otm8019a_5p0_fwvga_video_video_panel = {
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

static struct lane_configuration boe_otm8019a_5p0_fwvga_video_lane_config = {
	.dsi_lanes = 2,
	.dsi_lanemap = 1,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 0,
	.lane3_state = 0,
	.force_clk_lane_hs = 0,
};

static const uint32_t boe_otm8019a_5p0_fwvga_video_timings[] = {
	0x7a, 0x1a, 0x12, 0x00, 0x40, 0x42, 0x16, 0x1e, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing boe_otm8019a_5p0_fwvga_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1d,
};

static struct panel_reset_sequence boe_otm8019a_5p0_fwvga_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 1, 20, 120 },
	.pin_direction = 2,
};

static struct backlight boe_otm8019a_5p0_fwvga_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 250,
};

static inline void panel_boe_otm8019a_5p0_fwvga_video_select(struct panel_struct *panel,
							     struct msm_panel_info *pinfo,
							     struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &boe_otm8019a_5p0_fwvga_video_panel_data;
	panel->panelres = &boe_otm8019a_5p0_fwvga_video_panel_res;
	panel->color = &boe_otm8019a_5p0_fwvga_video_color;
	panel->videopanel = &boe_otm8019a_5p0_fwvga_video_video_panel;
	panel->commandpanel = &boe_otm8019a_5p0_fwvga_video_command_panel;
	panel->state = &boe_otm8019a_5p0_fwvga_video_state;
	panel->laneconfig = &boe_otm8019a_5p0_fwvga_video_lane_config;
	panel->paneltiminginfo = &boe_otm8019a_5p0_fwvga_video_timing_info;
	panel->panelresetseq = &boe_otm8019a_5p0_fwvga_video_reset_seq;
	panel->backlightinfo = &boe_otm8019a_5p0_fwvga_video_backlight;
	pinfo->mipi.panel_cmds = boe_otm8019a_5p0_fwvga_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(boe_otm8019a_5p0_fwvga_video_on_command);
	memcpy(phy_db->timing, boe_otm8019a_5p0_fwvga_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_BOE_OTM8019A_5P0_FWVGA_VIDEO_H_ */
