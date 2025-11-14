// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2014, The Linux Foundation. All rights reserved. (FIXME)

#ifndef _PANEL_BOENT35523B_800P_VIDEO_H_
#define _PANEL_BOENT35523B_800P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config boent35523b_800p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_boent35523b_800p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 0,
	.panel_init_delay = 0,
};

static struct panel_resolution boent35523b_800p_video_panel_res = {
	.panel_width = 800,
	.panel_height = 1280,
	.hfront_porch = 50,
	.hback_porch = 48,
	.hpulse_width = 16,
	.hsync_skew = 0,
	.vfront_porch = 20,
	.vback_porch = 12,
	.vpulse_width = 4,
	/* Borders not supported yet */
};

static struct color_info boent35523b_800p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char boent35523b_800p_video_on_cmd_0[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x00, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_1[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb2, 0x25, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_2[] = {
	0x03, 0x00, 0x39, 0xc0, 0xbb, 0x63, 0x63, 0xff
};
static char boent35523b_800p_video_on_cmd_3[] = {
	0x03, 0x00, 0x39, 0xc0, 0xbc, 0x0f, 0x00, 0xff
};
static char boent35523b_800p_video_on_cmd_4[] = {
	0x05, 0x00, 0x39, 0xc0, 0xbd, 0x01, 0x40, 0x0d,
	0x06, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_5[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x01, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_6[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb3, 0x28, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_7[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb4, 0x14, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_8[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb9, 0x45, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_9[] = {
	0x02, 0x00, 0x39, 0xc0, 0xba, 0x25, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_10[] = {
	0x03, 0x00, 0x39, 0xc0, 0xbc, 0x78, 0x00, 0xff
};
static char boent35523b_800p_video_on_cmd_11[] = {
	0x03, 0x00, 0x39, 0xc0, 0xbd, 0x78, 0x00, 0xff
};
static char boent35523b_800p_video_on_cmd_12[] = {
	0x02, 0x00, 0x39, 0xc0, 0xca, 0x02, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_13[] = {
	0x02, 0x00, 0x39, 0xc0, 0xce, 0x04, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_14[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x02, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_15[] = {
	0x02, 0x00, 0x39, 0xc0, 0xb0, 0x82, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_16[] = {
	0x11, 0x00, 0x39, 0xc0, 0xd1, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x28, 0x00, 0x4a, 0x00, 0x63, 0x00,
	0x8d, 0x00, 0xb0, 0x00, 0xe5, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_17[] = {
	0x11, 0x00, 0x39, 0xc0, 0xd2, 0x01, 0x0f, 0x01,
	0x50, 0x01, 0x83, 0x01, 0xce, 0x02, 0x0b, 0x02,
	0x0c, 0x02, 0x43, 0x02, 0x7d, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_18[] = {
	0x11, 0x00, 0x39, 0xc0, 0xd3, 0x02, 0xa0, 0x02,
	0xd3, 0x02, 0xf5, 0x03, 0x24, 0x03, 0x43, 0x03,
	0x6c, 0x03, 0x86, 0x03, 0xa1, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_19[] = {
	0x05, 0x00, 0x39, 0xc0, 0xd4, 0x03, 0xba, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_20[] = {
	0x11, 0x00, 0x39, 0xc0, 0xe0, 0x00, 0x00, 0x00,
	0x2a, 0x00, 0x54, 0x00, 0x6a, 0x00, 0x83, 0x00,
	0xa7, 0x00, 0xc2, 0x00, 0xf1, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_21[] = {
	0x11, 0x00, 0x39, 0xc0, 0xe1, 0x01, 0x17, 0x01,
	0x56, 0x01, 0x87, 0x01, 0xd0, 0x02, 0x0b, 0x02,
	0x0c, 0x02, 0x43, 0x02, 0x7d, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_22[] = {
	0x11, 0x00, 0x39, 0xc0, 0xe2, 0x02, 0xa2, 0x02,
	0xd5, 0x02, 0xf7, 0x03, 0x28, 0x03, 0x43, 0x03,
	0x6c, 0x03, 0x86, 0x03, 0xa1, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_23[] = {
	0x05, 0x00, 0x39, 0xc0, 0xe3, 0x03, 0xba, 0x03,
	0xff, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_24[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_25[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb0, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_26[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb1, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_27[] = {
	0x08, 0x00, 0x39, 0xc0, 0xb2, 0x01, 0x00, 0x09,
	0x03, 0x00, 0x34, 0x48
};
static char boent35523b_800p_video_on_cmd_28[] = {
	0x08, 0x00, 0x39, 0xc0, 0xb3, 0x01, 0x00, 0x08,
	0x03, 0x00, 0x34, 0x48
};
static char boent35523b_800p_video_on_cmd_29[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xb6, 0xf0, 0x05, 0x04,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x34, 0x48, 0xff
};
static char boent35523b_800p_video_on_cmd_30[] = {
	0x0b, 0x00, 0x39, 0xc0, 0xb7, 0xf0, 0x05, 0x04,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x34, 0x48, 0xff
};
static char boent35523b_800p_video_on_cmd_31[] = {
	0x08, 0x00, 0x39, 0xc0, 0xba, 0x84, 0x04, 0x00,
	0x07, 0x01, 0x34, 0x48
};
static char boent35523b_800p_video_on_cmd_32[] = {
	0x08, 0x00, 0x39, 0xc0, 0xbb, 0x84, 0x04, 0x00,
	0x06, 0x01, 0x34, 0x48
};
static char boent35523b_800p_video_on_cmd_33[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc4, 0x10, 0x00, 0xff
};
static char boent35523b_800p_video_on_cmd_34[] = {
	0x03, 0x00, 0x39, 0xc0, 0xc5, 0x00, 0x00, 0xff
};
static char boent35523b_800p_video_on_cmd_35[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_36[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb0, 0x33, 0x03, 0x00,
	0x03, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_37[] = {
	0x03, 0x00, 0x39, 0xc0, 0xb1, 0x30, 0x00, 0xff
};
static char boent35523b_800p_video_on_cmd_38[] = {
	0x04, 0x00, 0x39, 0xc0, 0xb2, 0x03, 0x01, 0x00
};
static char boent35523b_800p_video_on_cmd_39[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb3, 0x82, 0x00, 0x81,
	0x38, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_40[] = {
	0x05, 0x00, 0x39, 0xc0, 0xb4, 0xd5, 0x75, 0x07,
	0x57, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_41[] = {
	0x07, 0x00, 0x39, 0xc0, 0xb6, 0x01, 0x00, 0xd5,
	0x71, 0x07, 0x57, 0xff
};
static char boent35523b_800p_video_on_cmd_42[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb9, 0x09, 0x00, 0x00,
	0x05, 0x00, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_43[] = {
	0x06, 0x00, 0x39, 0xc0, 0xc0, 0x75, 0x07, 0x00,
	0x57, 0x05, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_44[] = {
	0x02, 0x00, 0x39, 0xc0, 0xc6, 0x20, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_45[] = {
	0x06, 0x00, 0x39, 0xc0, 0xd0, 0x00, 0x25, 0x01,
	0x00, 0x00, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_46[] = {
	0x06, 0x00, 0x39, 0xc0, 0xd1, 0x00, 0x25, 0x02,
	0x00, 0x00, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_47[] = {
	0x06, 0x00, 0x39, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x06, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_48[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb0, 0x32, 0x32, 0x02,
	0x02, 0x03, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_49[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb1, 0x03, 0x33, 0x33,
	0x08, 0x3d, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_50[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb2, 0x0a, 0x0a, 0x3d,
	0x17, 0x17, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_51[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb3, 0x16, 0x16, 0x19,
	0x19, 0x18, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_52[] = {
	0x03, 0x00, 0x39, 0xc0, 0xb4, 0x18, 0x33, 0xff
};
static char boent35523b_800p_video_on_cmd_53[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb5, 0x32, 0x32, 0x00,
	0x00, 0x01, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_54[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb6, 0x01, 0x33, 0x33,
	0x08, 0x3d, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_55[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb7, 0x0a, 0x0a, 0x3d,
	0x11, 0x11, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_56[] = {
	0x06, 0x00, 0x39, 0xc0, 0xb8, 0x10, 0x10, 0x13,
	0x13, 0x12, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_57[] = {
	0x03, 0x00, 0x39, 0xc0, 0xb9, 0x12, 0x33, 0xff
};
static char boent35523b_800p_video_on_cmd_58[] = {
	0x05, 0x00, 0x39, 0xc0, 0xff, 0xaa, 0x55, 0xa5,
	0x80, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_59[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x08, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_60[] = {
	0x02, 0x00, 0x39, 0xc0, 0xfc, 0x00, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_61[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x09, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_62[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf7, 0x82, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_63[] = {
	0x02, 0x00, 0x39, 0xc0, 0x6f, 0x0b, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_64[] = {
	0x02, 0x00, 0x39, 0xc0, 0xf7, 0xe0, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_65[] = {
	0x05, 0x00, 0x39, 0xc0, 0xff, 0xaa, 0x55, 0xa5,
	0x00, 0xff, 0xff, 0xff
};
static char boent35523b_800p_video_on_cmd_66[] = {
	0x11, 0x00, 0x05, 0x80
};
static char boent35523b_800p_video_on_cmd_67[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boent35523b_800p_video_on_command[] = {
	{ sizeof(boent35523b_800p_video_on_cmd_0), boent35523b_800p_video_on_cmd_0, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_1), boent35523b_800p_video_on_cmd_1, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_2), boent35523b_800p_video_on_cmd_2, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_3), boent35523b_800p_video_on_cmd_3, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_4), boent35523b_800p_video_on_cmd_4, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_5), boent35523b_800p_video_on_cmd_5, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_6), boent35523b_800p_video_on_cmd_6, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_7), boent35523b_800p_video_on_cmd_7, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_8), boent35523b_800p_video_on_cmd_8, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_9), boent35523b_800p_video_on_cmd_9, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_10), boent35523b_800p_video_on_cmd_10, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_11), boent35523b_800p_video_on_cmd_11, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_12), boent35523b_800p_video_on_cmd_12, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_13), boent35523b_800p_video_on_cmd_13, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_14), boent35523b_800p_video_on_cmd_14, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_15), boent35523b_800p_video_on_cmd_15, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_16), boent35523b_800p_video_on_cmd_16, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_17), boent35523b_800p_video_on_cmd_17, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_18), boent35523b_800p_video_on_cmd_18, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_19), boent35523b_800p_video_on_cmd_19, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_20), boent35523b_800p_video_on_cmd_20, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_21), boent35523b_800p_video_on_cmd_21, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_22), boent35523b_800p_video_on_cmd_22, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_23), boent35523b_800p_video_on_cmd_23, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_24), boent35523b_800p_video_on_cmd_24, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_25), boent35523b_800p_video_on_cmd_25, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_26), boent35523b_800p_video_on_cmd_26, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_27), boent35523b_800p_video_on_cmd_27, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_28), boent35523b_800p_video_on_cmd_28, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_29), boent35523b_800p_video_on_cmd_29, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_30), boent35523b_800p_video_on_cmd_30, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_31), boent35523b_800p_video_on_cmd_31, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_32), boent35523b_800p_video_on_cmd_32, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_33), boent35523b_800p_video_on_cmd_33, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_34), boent35523b_800p_video_on_cmd_34, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_35), boent35523b_800p_video_on_cmd_35, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_36), boent35523b_800p_video_on_cmd_36, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_37), boent35523b_800p_video_on_cmd_37, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_38), boent35523b_800p_video_on_cmd_38, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_39), boent35523b_800p_video_on_cmd_39, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_40), boent35523b_800p_video_on_cmd_40, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_41), boent35523b_800p_video_on_cmd_41, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_42), boent35523b_800p_video_on_cmd_42, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_43), boent35523b_800p_video_on_cmd_43, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_44), boent35523b_800p_video_on_cmd_44, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_45), boent35523b_800p_video_on_cmd_45, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_46), boent35523b_800p_video_on_cmd_46, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_47), boent35523b_800p_video_on_cmd_47, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_48), boent35523b_800p_video_on_cmd_48, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_49), boent35523b_800p_video_on_cmd_49, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_50), boent35523b_800p_video_on_cmd_50, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_51), boent35523b_800p_video_on_cmd_51, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_52), boent35523b_800p_video_on_cmd_52, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_53), boent35523b_800p_video_on_cmd_53, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_54), boent35523b_800p_video_on_cmd_54, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_55), boent35523b_800p_video_on_cmd_55, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_56), boent35523b_800p_video_on_cmd_56, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_57), boent35523b_800p_video_on_cmd_57, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_58), boent35523b_800p_video_on_cmd_58, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_59), boent35523b_800p_video_on_cmd_59, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_60), boent35523b_800p_video_on_cmd_60, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_61), boent35523b_800p_video_on_cmd_61, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_62), boent35523b_800p_video_on_cmd_62, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_63), boent35523b_800p_video_on_cmd_63, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_64), boent35523b_800p_video_on_cmd_64, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_65), boent35523b_800p_video_on_cmd_65, 0 },
	{ sizeof(boent35523b_800p_video_on_cmd_66), boent35523b_800p_video_on_cmd_66, 120 },
	{ sizeof(boent35523b_800p_video_on_cmd_67), boent35523b_800p_video_on_cmd_67, 10 },
};

static char boent35523b_800p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char boent35523b_800p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd boent35523b_800p_video_off_command[] = {
	{ sizeof(boent35523b_800p_video_off_cmd_0), boent35523b_800p_video_off_cmd_0, 20 },
	{ sizeof(boent35523b_800p_video_off_cmd_1), boent35523b_800p_video_off_cmd_1, 120 },
};

static struct command_state boent35523b_800p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 0,
};

static struct commandpanel_info boent35523b_800p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info boent35523b_800p_video_video_panel = {
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

static struct lane_configuration boent35523b_800p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t boent35523b_800p_video_timings[] = {
	0x7a, 0x1a, 0x12, 0x00, 0x40, 0x42, 0x16, 0x21, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing boent35523b_800p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1b,
};

static struct panel_reset_sequence boent35523b_800p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 20, 10, 120 },
	.pin_direction = 2,
};

static struct backlight boent35523b_800p_video_backlight = {
	.bl_interface_type = BL_PWM,
	.bl_min_level = 1,
	.bl_max_level = 4095,
};

static inline void panel_boent35523b_800p_video_select(struct panel_struct *panel,
						       struct msm_panel_info *pinfo,
						       struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &boent35523b_800p_video_panel_data;
	panel->panelres = &boent35523b_800p_video_panel_res;
	panel->color = &boent35523b_800p_video_color;
	panel->videopanel = &boent35523b_800p_video_video_panel;
	panel->commandpanel = &boent35523b_800p_video_command_panel;
	panel->state = &boent35523b_800p_video_state;
	panel->laneconfig = &boent35523b_800p_video_lane_config;
	panel->paneltiminginfo = &boent35523b_800p_video_timing_info;
	panel->panelresetseq = &boent35523b_800p_video_reset_seq;
	panel->backlightinfo = &boent35523b_800p_video_backlight;
	pinfo->mipi.panel_on_cmds = boent35523b_800p_video_on_command;
	pinfo->mipi.panel_off_cmds = boent35523b_800p_video_off_command;
	pinfo->mipi.num_of_panel_on_cmds = ARRAY_SIZE(boent35523b_800p_video_on_command);
	pinfo->mipi.num_of_panel_off_cmds = ARRAY_SIZE(boent35523b_800p_video_off_command);
	memcpy(phy_db->timing, boent35523b_800p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_BOENT35523B_800P_VIDEO_H_ */
