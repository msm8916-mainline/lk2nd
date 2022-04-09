// SPDX-License-Identifier: GPL-2.0-only
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved.

#ifndef _PANEL_TIANMA_NT35521_5P5_720P_VIDEO_H_
#define _PANEL_TIANMA_NT35521_5P5_720P_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_display.h>
#include <panel.h>
#include <string.h>

static struct panel_config tianma_nt35521_5p5_720p_video_panel_data = {
	.panel_node_id = "qcom,mdss_dsi_tianma_nt35521_5p5_720p_video",
	.panel_controller = "dsi:0:",
	.panel_compatible = "qcom,mdss-dsi-panel",
	.panel_type = 0,
	.panel_destination = "DISPLAY_1",
	/* .panel_orientation not supported yet */
	.panel_framerate = 60,
	.panel_lp11_init = 1,
	.panel_init_delay = 0,
};

static struct panel_resolution tianma_nt35521_5p5_720p_video_panel_res = {
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

static struct color_info tianma_nt35521_5p5_720p_video_color = {
	.color_format = 24,
	.color_order = DSI_RGB_SWAP_RGB,
	.underflow_color = 0xff,
	/* Borders and pixel packing not supported yet */
};

static char tianma_nt35521_5p5_720p_video_on_cmd_0[] = {
	0x05, 0x00, 0x29, 0xc0, 0xff, 0xaa, 0x55, 0xa5,
	0x80, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_1[] = {
	0x03, 0x00, 0x29, 0xc0, 0x6f, 0x11, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_2[] = {
	0x03, 0x00, 0x29, 0xc0, 0xf7, 0x20, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_3[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x11, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_4[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf3, 0x01, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_5[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_6[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb1, 0x60, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_7[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbd, 0x01, 0xa0, 0x0c,
	0x08, 0x01, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_8[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x02, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_9[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb8, 0x01, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_10[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbb, 0x11, 0x11, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_11[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0x00, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_12[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb6, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_13[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x01, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_14[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x09, 0x09, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_15[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x09, 0x09, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_16[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb3, 0x28, 0x28, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_17[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb4, 0x0f, 0x0f, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_18[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb5, 0x03, 0x03, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_19[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb9, 0x34, 0x34, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_20[] = {
	0x03, 0x00, 0x29, 0xc0, 0xba, 0x15, 0x15, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_21[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0x58, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_22[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbd, 0x58, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_23[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x04, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_24[] = {
	0x02, 0x00, 0x29, 0xc0, 0xca, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_25[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x02, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_26[] = {
	0x02, 0x00, 0x29, 0xc0, 0xee, 0x03, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_27[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb0, 0x00, 0x05, 0x00,
	0x2e, 0x00, 0x43, 0x00, 0x6c, 0x00, 0x86, 0x00,
	0xaf, 0x00, 0xd0, 0x01, 0x02, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_28[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb1, 0x01, 0x2c, 0x01,
	0x67, 0x01, 0x96, 0x01, 0xe4, 0x02, 0x22, 0x02,
	0x24, 0x02, 0x60, 0x02, 0x9e, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_29[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb2, 0x02, 0xc5, 0x02,
	0xf8, 0x03, 0x1d, 0x03, 0x4e, 0x03, 0x68, 0x03,
	0x7d, 0x03, 0xa2, 0x03, 0xc7, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_30[] = {
	0x05, 0x00, 0x29, 0xc0, 0xb3, 0x03, 0xd7, 0x03,
	0xdb, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_31[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb4, 0x00, 0x99, 0x00,
	0xa3, 0x00, 0xb8, 0x00, 0xc8, 0x00, 0xd7, 0x00,
	0xf1, 0x01, 0x07, 0x01, 0x2c, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_32[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb5, 0x01, 0x4b, 0x01,
	0x7f, 0x01, 0xab, 0x01, 0xf2, 0x02, 0x2b, 0x02,
	0x2d, 0x02, 0x64, 0x02, 0xa2, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_33[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb6, 0x02, 0xc9, 0x02,
	0xfa, 0x03, 0x1c, 0x03, 0x49, 0x03, 0x65, 0x03,
	0x78, 0x03, 0x9e, 0x03, 0xc4, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_34[] = {
	0x05, 0x00, 0x29, 0xc0, 0xb7, 0x03, 0xda, 0x03,
	0xdb, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_35[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb8, 0x00, 0x02, 0x00,
	0x03, 0x00, 0x11, 0x00, 0x41, 0x00, 0x62, 0x00,
	0x92, 0x00, 0xb5, 0x00, 0xec, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_36[] = {
	0x11, 0x00, 0x29, 0xc0, 0xb9, 0x01, 0x17, 0x01,
	0x58, 0x01, 0x8a, 0x01, 0xdd, 0x02, 0x1e, 0x02,
	0x1f, 0x02, 0x5b, 0x02, 0x9b, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_37[] = {
	0x11, 0x00, 0x29, 0xc0, 0xba, 0x02, 0xc5, 0x02,
	0xf9, 0x03, 0x22, 0x03, 0x5c, 0x03, 0x8f, 0x03,
	0xfd, 0x03, 0xfd, 0x03, 0xfd, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_38[] = {
	0x05, 0x00, 0x29, 0xc0, 0xbb, 0x03, 0xfe, 0x03,
	0xfe, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_39[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x02, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_40[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf7, 0x47, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_41[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x0a, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_42[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf7, 0x02, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_43[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x17, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_44[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf4, 0x70, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_45[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x11, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_46[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf3, 0x01, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_47[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x03, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_48[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x20, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_49[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x20, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_50[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb2, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_51[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb3, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_52[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb4, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_53[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb5, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_54[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb6, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_55[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb7, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_56[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb8, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_57[] = {
	0x06, 0x00, 0x29, 0xc0, 0xb9, 0x05, 0x00, 0x00,
	0x00, 0x90, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_58[] = {
	0x06, 0x00, 0x29, 0xc0, 0xba, 0x53, 0x01, 0x00,
	0x01, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_59[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbb, 0x53, 0x01, 0x00,
	0x01, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_60[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbc, 0x53, 0x01, 0x00,
	0x01, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_61[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbd, 0x53, 0x01, 0x00,
	0x01, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_62[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x60, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_63[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x40, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_64[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc6, 0x60, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_65[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc7, 0x40, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_66[] = {
	0x02, 0x00, 0x29, 0xc0, 0x6f, 0x01, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_67[] = {
	0x02, 0x00, 0x29, 0xc0, 0xf9, 0x46, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_68[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_69[] = {
	0x02, 0x00, 0x29, 0xc0, 0xed, 0x30, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_70[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe5, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_71[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_72[] = {
	0x02, 0x00, 0x29, 0xc0, 0xb8, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_73[] = {
	0x06, 0x00, 0x29, 0xc0, 0xbd, 0x03, 0x03, 0x01,
	0x00, 0x03, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_74[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_75[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb9, 0x00, 0x03, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_76[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb2, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_77[] = {
	0x03, 0x00, 0x29, 0xc0, 0xba, 0x00, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_78[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb3, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_79[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbb, 0x00, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_80[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb4, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_81[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb5, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_82[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb6, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_83[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb7, 0x17, 0x06, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_84[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0x00, 0x03, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_85[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe5, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_86[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe6, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_87[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe7, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_88[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe8, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_89[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe9, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_90[] = {
	0x02, 0x00, 0x29, 0xc0, 0xea, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_91[] = {
	0x02, 0x00, 0x29, 0xc0, 0xeb, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_92[] = {
	0x02, 0x00, 0x29, 0xc0, 0xec, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_93[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc0, 0x0b, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_94[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc1, 0x09, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_95[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc2, 0x0b, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_96[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc3, 0x09, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_97[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc4, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_98[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc5, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_99[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc6, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_100[] = {
	0x02, 0x00, 0x29, 0xc0, 0xc7, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_101[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc8, 0x08, 0x20, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_102[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc9, 0x04, 0x20, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_103[] = {
	0x03, 0x00, 0x29, 0xc0, 0xca, 0x07, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_104[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcb, 0x03, 0x00, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_105[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd1, 0x00, 0x05, 0x00,
	0x07, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_106[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd2, 0x00, 0x05, 0x04,
	0x07, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_107[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd3, 0x00, 0x00, 0x0a,
	0x07, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_108[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd4, 0x00, 0x00, 0x0a,
	0x07, 0x10, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_109[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x05, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_110[] = {
	0x08, 0x00, 0x29, 0xc0, 0xd0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
static char tianma_nt35521_5p5_720p_video_on_cmd_111[] = {
	0x0c, 0x00, 0x29, 0xc0, 0xd5, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char tianma_nt35521_5p5_720p_video_on_cmd_112[] = {
	0x0c, 0x00, 0x29, 0xc0, 0xd6, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char tianma_nt35521_5p5_720p_video_on_cmd_113[] = {
	0x0c, 0x00, 0x29, 0xc0, 0xd7, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static char tianma_nt35521_5p5_720p_video_on_cmd_114[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_115[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x06, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_116[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb0, 0x12, 0x10, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_117[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb1, 0x18, 0x16, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_118[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb2, 0x00, 0x02, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_119[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb3, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_120[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb4, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_121[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb5, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_122[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb6, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_123[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb7, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_124[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb8, 0x31, 0x08, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_125[] = {
	0x03, 0x00, 0x29, 0xc0, 0xb9, 0x2e, 0x2d, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_126[] = {
	0x03, 0x00, 0x29, 0xc0, 0xba, 0x2d, 0x2e, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_127[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbb, 0x09, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_128[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbc, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_129[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbd, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_130[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbe, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_131[] = {
	0x03, 0x00, 0x29, 0xc0, 0xbf, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_132[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc0, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_133[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc1, 0x03, 0x01, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_134[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc2, 0x17, 0x19, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_135[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc3, 0x11, 0x13, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_136[] = {
	0x03, 0x00, 0x29, 0xc0, 0xe5, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_137[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc4, 0x17, 0x19, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_138[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc5, 0x11, 0x13, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_139[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc6, 0x03, 0x01, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_140[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc7, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_141[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc8, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_142[] = {
	0x03, 0x00, 0x29, 0xc0, 0xc9, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_143[] = {
	0x03, 0x00, 0x29, 0xc0, 0xca, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_144[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcb, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_145[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcc, 0x31, 0x09, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_146[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcd, 0x2d, 0x2e, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_147[] = {
	0x03, 0x00, 0x29, 0xc0, 0xce, 0x2e, 0x2d, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_148[] = {
	0x03, 0x00, 0x29, 0xc0, 0xcf, 0x08, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_149[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd0, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_150[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd1, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_151[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd2, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_152[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd3, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_153[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd4, 0x31, 0x31, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_154[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd5, 0x00, 0x02, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_155[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd6, 0x12, 0x10, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_156[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd7, 0x18, 0x16, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_157[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_158[] = {
	0x06, 0x00, 0x29, 0xc0, 0xd9, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_159[] = {
	0x02, 0x00, 0x29, 0xc0, 0xe7, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_160[] = {
	0x06, 0x00, 0x29, 0xc0, 0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_161[] = {
	0x05, 0x00, 0x29, 0xc0, 0xe6, 0xff, 0xff, 0xfa,
	0xfa, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_162[] = {
	0x0b, 0x00, 0x29, 0xc0, 0xe8, 0xf3, 0xe8, 0xe0,
	0xd8, 0xce, 0xc4, 0xba, 0xb0, 0xa6, 0x9c, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_163[] = {
	0x11, 0x00, 0x29, 0xc0, 0xcc, 0x41, 0x36, 0x87,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x40, 0x08, 0xa5, 0x05, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_164[] = {
	0x11, 0x00, 0x29, 0xc0, 0xd1, 0x00, 0x01, 0x01,
	0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02,
	0x02, 0x02, 0x01, 0x01, 0x00, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_165[] = {
	0x0d, 0x00, 0x29, 0xc0, 0xd7, 0x00, 0x01, 0x02,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_166[] = {
	0x0e, 0x00, 0x29, 0xc0, 0xd8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x01, 0x00, 0xff, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_167[] = {
	0x03, 0x00, 0x29, 0xc0, 0xd9, 0x02, 0x09, 0xff
};
static char tianma_nt35521_5p5_720p_video_on_cmd_168[] = {
	0x55, 0x81, 0x15, 0x80
};
static char tianma_nt35521_5p5_720p_video_on_cmd_169[] = {
	0x53, 0x24, 0x15, 0x80
};
static char tianma_nt35521_5p5_720p_video_on_cmd_170[] = {
	0x51, 0x00, 0x15, 0x80
};
static char tianma_nt35521_5p5_720p_video_on_cmd_171[] = {
	0x5e, 0x28, 0x15, 0x80
};
static char tianma_nt35521_5p5_720p_video_on_cmd_172[] = {
	0x11, 0x00, 0x05, 0x80
};
static char tianma_nt35521_5p5_720p_video_on_cmd_173[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd tianma_nt35521_5p5_720p_video_on_command[] = {
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_0), tianma_nt35521_5p5_720p_video_on_cmd_0, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_1), tianma_nt35521_5p5_720p_video_on_cmd_1, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_2), tianma_nt35521_5p5_720p_video_on_cmd_2, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_3), tianma_nt35521_5p5_720p_video_on_cmd_3, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_4), tianma_nt35521_5p5_720p_video_on_cmd_4, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_5), tianma_nt35521_5p5_720p_video_on_cmd_5, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_6), tianma_nt35521_5p5_720p_video_on_cmd_6, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_7), tianma_nt35521_5p5_720p_video_on_cmd_7, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_8), tianma_nt35521_5p5_720p_video_on_cmd_8, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_9), tianma_nt35521_5p5_720p_video_on_cmd_9, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_10), tianma_nt35521_5p5_720p_video_on_cmd_10, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_11), tianma_nt35521_5p5_720p_video_on_cmd_11, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_12), tianma_nt35521_5p5_720p_video_on_cmd_12, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_13), tianma_nt35521_5p5_720p_video_on_cmd_13, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_14), tianma_nt35521_5p5_720p_video_on_cmd_14, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_15), tianma_nt35521_5p5_720p_video_on_cmd_15, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_16), tianma_nt35521_5p5_720p_video_on_cmd_16, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_17), tianma_nt35521_5p5_720p_video_on_cmd_17, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_18), tianma_nt35521_5p5_720p_video_on_cmd_18, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_19), tianma_nt35521_5p5_720p_video_on_cmd_19, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_20), tianma_nt35521_5p5_720p_video_on_cmd_20, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_21), tianma_nt35521_5p5_720p_video_on_cmd_21, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_22), tianma_nt35521_5p5_720p_video_on_cmd_22, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_23), tianma_nt35521_5p5_720p_video_on_cmd_23, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_24), tianma_nt35521_5p5_720p_video_on_cmd_24, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_25), tianma_nt35521_5p5_720p_video_on_cmd_25, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_26), tianma_nt35521_5p5_720p_video_on_cmd_26, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_27), tianma_nt35521_5p5_720p_video_on_cmd_27, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_28), tianma_nt35521_5p5_720p_video_on_cmd_28, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_29), tianma_nt35521_5p5_720p_video_on_cmd_29, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_30), tianma_nt35521_5p5_720p_video_on_cmd_30, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_31), tianma_nt35521_5p5_720p_video_on_cmd_31, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_32), tianma_nt35521_5p5_720p_video_on_cmd_32, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_33), tianma_nt35521_5p5_720p_video_on_cmd_33, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_34), tianma_nt35521_5p5_720p_video_on_cmd_34, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_35), tianma_nt35521_5p5_720p_video_on_cmd_35, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_36), tianma_nt35521_5p5_720p_video_on_cmd_36, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_37), tianma_nt35521_5p5_720p_video_on_cmd_37, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_38), tianma_nt35521_5p5_720p_video_on_cmd_38, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_39), tianma_nt35521_5p5_720p_video_on_cmd_39, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_40), tianma_nt35521_5p5_720p_video_on_cmd_40, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_41), tianma_nt35521_5p5_720p_video_on_cmd_41, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_42), tianma_nt35521_5p5_720p_video_on_cmd_42, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_43), tianma_nt35521_5p5_720p_video_on_cmd_43, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_44), tianma_nt35521_5p5_720p_video_on_cmd_44, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_45), tianma_nt35521_5p5_720p_video_on_cmd_45, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_46), tianma_nt35521_5p5_720p_video_on_cmd_46, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_47), tianma_nt35521_5p5_720p_video_on_cmd_47, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_48), tianma_nt35521_5p5_720p_video_on_cmd_48, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_49), tianma_nt35521_5p5_720p_video_on_cmd_49, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_50), tianma_nt35521_5p5_720p_video_on_cmd_50, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_51), tianma_nt35521_5p5_720p_video_on_cmd_51, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_52), tianma_nt35521_5p5_720p_video_on_cmd_52, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_53), tianma_nt35521_5p5_720p_video_on_cmd_53, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_54), tianma_nt35521_5p5_720p_video_on_cmd_54, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_55), tianma_nt35521_5p5_720p_video_on_cmd_55, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_56), tianma_nt35521_5p5_720p_video_on_cmd_56, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_57), tianma_nt35521_5p5_720p_video_on_cmd_57, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_58), tianma_nt35521_5p5_720p_video_on_cmd_58, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_59), tianma_nt35521_5p5_720p_video_on_cmd_59, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_60), tianma_nt35521_5p5_720p_video_on_cmd_60, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_61), tianma_nt35521_5p5_720p_video_on_cmd_61, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_62), tianma_nt35521_5p5_720p_video_on_cmd_62, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_63), tianma_nt35521_5p5_720p_video_on_cmd_63, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_64), tianma_nt35521_5p5_720p_video_on_cmd_64, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_65), tianma_nt35521_5p5_720p_video_on_cmd_65, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_66), tianma_nt35521_5p5_720p_video_on_cmd_66, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_67), tianma_nt35521_5p5_720p_video_on_cmd_67, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_68), tianma_nt35521_5p5_720p_video_on_cmd_68, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_69), tianma_nt35521_5p5_720p_video_on_cmd_69, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_70), tianma_nt35521_5p5_720p_video_on_cmd_70, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_71), tianma_nt35521_5p5_720p_video_on_cmd_71, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_72), tianma_nt35521_5p5_720p_video_on_cmd_72, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_73), tianma_nt35521_5p5_720p_video_on_cmd_73, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_74), tianma_nt35521_5p5_720p_video_on_cmd_74, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_75), tianma_nt35521_5p5_720p_video_on_cmd_75, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_76), tianma_nt35521_5p5_720p_video_on_cmd_76, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_77), tianma_nt35521_5p5_720p_video_on_cmd_77, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_78), tianma_nt35521_5p5_720p_video_on_cmd_78, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_79), tianma_nt35521_5p5_720p_video_on_cmd_79, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_80), tianma_nt35521_5p5_720p_video_on_cmd_80, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_81), tianma_nt35521_5p5_720p_video_on_cmd_81, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_82), tianma_nt35521_5p5_720p_video_on_cmd_82, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_83), tianma_nt35521_5p5_720p_video_on_cmd_83, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_84), tianma_nt35521_5p5_720p_video_on_cmd_84, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_85), tianma_nt35521_5p5_720p_video_on_cmd_85, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_86), tianma_nt35521_5p5_720p_video_on_cmd_86, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_87), tianma_nt35521_5p5_720p_video_on_cmd_87, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_88), tianma_nt35521_5p5_720p_video_on_cmd_88, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_89), tianma_nt35521_5p5_720p_video_on_cmd_89, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_90), tianma_nt35521_5p5_720p_video_on_cmd_90, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_91), tianma_nt35521_5p5_720p_video_on_cmd_91, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_92), tianma_nt35521_5p5_720p_video_on_cmd_92, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_93), tianma_nt35521_5p5_720p_video_on_cmd_93, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_94), tianma_nt35521_5p5_720p_video_on_cmd_94, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_95), tianma_nt35521_5p5_720p_video_on_cmd_95, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_96), tianma_nt35521_5p5_720p_video_on_cmd_96, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_97), tianma_nt35521_5p5_720p_video_on_cmd_97, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_98), tianma_nt35521_5p5_720p_video_on_cmd_98, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_99), tianma_nt35521_5p5_720p_video_on_cmd_99, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_100), tianma_nt35521_5p5_720p_video_on_cmd_100, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_101), tianma_nt35521_5p5_720p_video_on_cmd_101, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_102), tianma_nt35521_5p5_720p_video_on_cmd_102, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_103), tianma_nt35521_5p5_720p_video_on_cmd_103, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_104), tianma_nt35521_5p5_720p_video_on_cmd_104, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_105), tianma_nt35521_5p5_720p_video_on_cmd_105, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_106), tianma_nt35521_5p5_720p_video_on_cmd_106, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_107), tianma_nt35521_5p5_720p_video_on_cmd_107, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_108), tianma_nt35521_5p5_720p_video_on_cmd_108, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_109), tianma_nt35521_5p5_720p_video_on_cmd_109, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_110), tianma_nt35521_5p5_720p_video_on_cmd_110, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_111), tianma_nt35521_5p5_720p_video_on_cmd_111, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_112), tianma_nt35521_5p5_720p_video_on_cmd_112, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_113), tianma_nt35521_5p5_720p_video_on_cmd_113, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_114), tianma_nt35521_5p5_720p_video_on_cmd_114, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_115), tianma_nt35521_5p5_720p_video_on_cmd_115, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_116), tianma_nt35521_5p5_720p_video_on_cmd_116, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_117), tianma_nt35521_5p5_720p_video_on_cmd_117, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_118), tianma_nt35521_5p5_720p_video_on_cmd_118, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_119), tianma_nt35521_5p5_720p_video_on_cmd_119, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_120), tianma_nt35521_5p5_720p_video_on_cmd_120, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_121), tianma_nt35521_5p5_720p_video_on_cmd_121, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_122), tianma_nt35521_5p5_720p_video_on_cmd_122, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_123), tianma_nt35521_5p5_720p_video_on_cmd_123, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_124), tianma_nt35521_5p5_720p_video_on_cmd_124, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_125), tianma_nt35521_5p5_720p_video_on_cmd_125, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_126), tianma_nt35521_5p5_720p_video_on_cmd_126, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_127), tianma_nt35521_5p5_720p_video_on_cmd_127, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_128), tianma_nt35521_5p5_720p_video_on_cmd_128, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_129), tianma_nt35521_5p5_720p_video_on_cmd_129, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_130), tianma_nt35521_5p5_720p_video_on_cmd_130, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_131), tianma_nt35521_5p5_720p_video_on_cmd_131, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_132), tianma_nt35521_5p5_720p_video_on_cmd_132, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_133), tianma_nt35521_5p5_720p_video_on_cmd_133, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_134), tianma_nt35521_5p5_720p_video_on_cmd_134, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_135), tianma_nt35521_5p5_720p_video_on_cmd_135, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_136), tianma_nt35521_5p5_720p_video_on_cmd_136, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_137), tianma_nt35521_5p5_720p_video_on_cmd_137, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_138), tianma_nt35521_5p5_720p_video_on_cmd_138, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_139), tianma_nt35521_5p5_720p_video_on_cmd_139, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_140), tianma_nt35521_5p5_720p_video_on_cmd_140, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_141), tianma_nt35521_5p5_720p_video_on_cmd_141, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_142), tianma_nt35521_5p5_720p_video_on_cmd_142, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_143), tianma_nt35521_5p5_720p_video_on_cmd_143, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_144), tianma_nt35521_5p5_720p_video_on_cmd_144, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_145), tianma_nt35521_5p5_720p_video_on_cmd_145, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_146), tianma_nt35521_5p5_720p_video_on_cmd_146, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_147), tianma_nt35521_5p5_720p_video_on_cmd_147, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_148), tianma_nt35521_5p5_720p_video_on_cmd_148, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_149), tianma_nt35521_5p5_720p_video_on_cmd_149, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_150), tianma_nt35521_5p5_720p_video_on_cmd_150, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_151), tianma_nt35521_5p5_720p_video_on_cmd_151, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_152), tianma_nt35521_5p5_720p_video_on_cmd_152, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_153), tianma_nt35521_5p5_720p_video_on_cmd_153, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_154), tianma_nt35521_5p5_720p_video_on_cmd_154, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_155), tianma_nt35521_5p5_720p_video_on_cmd_155, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_156), tianma_nt35521_5p5_720p_video_on_cmd_156, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_157), tianma_nt35521_5p5_720p_video_on_cmd_157, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_158), tianma_nt35521_5p5_720p_video_on_cmd_158, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_159), tianma_nt35521_5p5_720p_video_on_cmd_159, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_160), tianma_nt35521_5p5_720p_video_on_cmd_160, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_161), tianma_nt35521_5p5_720p_video_on_cmd_161, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_162), tianma_nt35521_5p5_720p_video_on_cmd_162, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_163), tianma_nt35521_5p5_720p_video_on_cmd_163, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_164), tianma_nt35521_5p5_720p_video_on_cmd_164, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_165), tianma_nt35521_5p5_720p_video_on_cmd_165, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_166), tianma_nt35521_5p5_720p_video_on_cmd_166, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_167), tianma_nt35521_5p5_720p_video_on_cmd_167, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_168), tianma_nt35521_5p5_720p_video_on_cmd_168, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_169), tianma_nt35521_5p5_720p_video_on_cmd_169, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_170), tianma_nt35521_5p5_720p_video_on_cmd_170, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_171), tianma_nt35521_5p5_720p_video_on_cmd_171, 0 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_172), tianma_nt35521_5p5_720p_video_on_cmd_172, 120 },
	{ sizeof(tianma_nt35521_5p5_720p_video_on_cmd_173), tianma_nt35521_5p5_720p_video_on_cmd_173, 20 },
};

static char tianma_nt35521_5p5_720p_video_off_cmd_0[] = {
	0x28, 0x00, 0x05, 0x80
};
static char tianma_nt35521_5p5_720p_video_off_cmd_1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd tianma_nt35521_5p5_720p_video_off_command[] = {
	{ sizeof(tianma_nt35521_5p5_720p_video_off_cmd_0), tianma_nt35521_5p5_720p_video_off_cmd_0, 20 },
	{ sizeof(tianma_nt35521_5p5_720p_video_off_cmd_1), tianma_nt35521_5p5_720p_video_off_cmd_1, 120 },
};

static struct command_state tianma_nt35521_5p5_720p_video_state = {
	.oncommand_state = 0,
	.offcommand_state = 1,
};

static struct commandpanel_info tianma_nt35521_5p5_720p_video_command_panel = {
	/* Unused, this is a video mode panel */
};

static struct videopanel_info tianma_nt35521_5p5_720p_video_video_panel = {
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

static struct lane_configuration tianma_nt35521_5p5_720p_video_lane_config = {
	.dsi_lanes = 4,
	.dsi_lanemap = 0,
	.lane0_state = 1,
	.lane1_state = 1,
	.lane2_state = 1,
	.lane3_state = 1,
	.force_clk_lane_hs = 0,
};

static const uint32_t tianma_nt35521_5p5_720p_video_timings[] = {
	0x7a, 0x1a, 0x12, 0x00, 0x40, 0x42, 0x16, 0x1e, 0x14, 0x03, 0x04, 0x00
};

static struct panel_timing tianma_nt35521_5p5_720p_video_timing_info = {
	.tclk_post = 0x04,
	.tclk_pre = 0x1c,
};

static struct panel_reset_sequence tianma_nt35521_5p5_720p_video_reset_seq = {
	.pin_state = { 1, 0, 1 },
	.sleep = { 1, 20, 120 },
	.pin_direction = 2,
};

static struct backlight tianma_nt35521_5p5_720p_video_backlight = {
	.bl_interface_type = BL_DCS,
	.bl_min_level = 1,
	.bl_max_level = 250,
};

static inline void panel_tianma_nt35521_5p5_720p_video_select(struct panel_struct *panel,
							      struct msm_panel_info *pinfo,
							      struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &tianma_nt35521_5p5_720p_video_panel_data;
	panel->panelres = &tianma_nt35521_5p5_720p_video_panel_res;
	panel->color = &tianma_nt35521_5p5_720p_video_color;
	panel->videopanel = &tianma_nt35521_5p5_720p_video_video_panel;
	panel->commandpanel = &tianma_nt35521_5p5_720p_video_command_panel;
	panel->state = &tianma_nt35521_5p5_720p_video_state;
	panel->laneconfig = &tianma_nt35521_5p5_720p_video_lane_config;
	panel->paneltiminginfo = &tianma_nt35521_5p5_720p_video_timing_info;
	panel->panelresetseq = &tianma_nt35521_5p5_720p_video_reset_seq;
	panel->backlightinfo = &tianma_nt35521_5p5_720p_video_backlight;
	pinfo->mipi.panel_cmds = tianma_nt35521_5p5_720p_video_on_command;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(tianma_nt35521_5p5_720p_video_on_command);
	memcpy(phy_db->timing, tianma_nt35521_5p5_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
}

#endif /* _PANEL_TIANMA_NT35521_5P5_720P_VIDEO_H_ */
