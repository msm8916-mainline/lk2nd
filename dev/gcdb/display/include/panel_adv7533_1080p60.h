/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PANEL_ADV7533_1080p60_H_
#define _PANEL_ADV7533_1080p60_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config adv7533_1080p_video_panel_data = {
	"qcom,mdss_dsi_adv7533_1080p", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "NONE"
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution adv7533_1080p_video_panel_res = {
	1920, 1080, 88, 148, 44, 0, 4, 36, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info adv7533_1080p_video_color = {
	24, 0, 0xff, 0, 0, 0
};

static struct mipi_dsi_i2c_cmd adv7533_1080p_common_cfg[] = {
	{ADV7533_MAIN, 0xd6, 0x48, 5},		/* HPD overriden */
	{ADV7533_MAIN, 0x41, 0x10, 5},		/* HDMI normal */
	{ADV7533_CEC_DSI, 0x03, 0x89, 0},	/* HDMI enabled */
	{ADV7533_MAIN, 0x16, 0x20, 0},
	{ADV7533_MAIN, 0x9A, 0xE0, 0},
	{ADV7533_MAIN, 0xBA, 0x70, 0},
	{ADV7533_MAIN, 0xDE, 0x82, 0},
	{ADV7533_MAIN, 0xE4, 0x40, 0},
	{ADV7533_MAIN, 0xE5, 0x80, 0},
	{ADV7533_CEC_DSI, 0x15, 0xD0, 0},
	{ADV7533_CEC_DSI, 0x17, 0xD0, 0},
	{ADV7533_CEC_DSI, 0x24, 0x20, 0},
	{ADV7533_CEC_DSI, 0x57, 0x11, 0},
	/* hdmi or dvi mode: hdmi */
	{ADV7533_MAIN, 0xAF, 0x06, 0},
	{ADV7533_MAIN, 0x40, 0x80, 0},
	{ADV7533_MAIN, 0x4C, 0x04, 0},
	{ADV7533_MAIN, 0x49, 0x02, 0},
	{ADV7533_MAIN, 0x0D, 1 << 6, 0},
	{ADV7533_CEC_DSI, 0x1C, 0x30, 0},
};

#define ADV7533_1080P_CONFIG_COMMANDS 19

static struct mipi_dsi_i2c_cmd adv7533_1080p_tg_i2c_command[] = {
	/*4 Lanes*/
	{ADV7533_CEC_DSI, 0x1C, 0x40, 0},
	/* hsync and vsync active low */
	{ADV7533_MAIN, 0x17, 0x02, 0},
	/* Control for Pixel Clock Divider */
	{ADV7533_CEC_DSI, 0x16, 0x00, 0},
	/* Timing Generator Enable */
	{ADV7533_CEC_DSI, 0x27, 0xCB, 0},
	/* h_width 0x898 2200*/
	{ADV7533_CEC_DSI, 0x28, 0x89, 0},
	{ADV7533_CEC_DSI, 0x29, 0x80, 0},
	/* hsync_width 0x2c 44*/
	{ADV7533_CEC_DSI, 0x2A, 0x02, 0},
	{ADV7533_CEC_DSI, 0x2B, 0xC0, 0},
	/* hfp 0x58 88 */
	{ADV7533_CEC_DSI, 0x2C, 0x05, 0},
	{ADV7533_CEC_DSI, 0x2D, 0x80, 0},
	/* hbp 0x94 148 */
	{ADV7533_CEC_DSI, 0x2E, 0x09, 0},
	{ADV7533_CEC_DSI, 0x2F, 0x40, 0},
	/* v_total 0x465 1125*/
	{ADV7533_CEC_DSI, 0x30, 0x46, 0},
	{ADV7533_CEC_DSI, 0x31, 0x50, 0},
	/* vsync_width 0x05 5*/
	{ADV7533_CEC_DSI, 0x32, 0x00, 0},
	{ADV7533_CEC_DSI, 0x33, 0x50, 0},
	/* vfp 0x04 4  */
	{ADV7533_CEC_DSI, 0x34, 0x00, 0},
	{ADV7533_CEC_DSI, 0x35, 0x40, 0},
	/* vbp 0x24 36 */
	{ADV7533_CEC_DSI, 0x36, 0x02, 0},
	{ADV7533_CEC_DSI, 0x37, 0x40, 0},
	/* Timing Generator Enable */
	{ADV7533_CEC_DSI, 0x27, 0xCB, 0},
	{ADV7533_CEC_DSI, 0x27, 0x8B, 0},
	{ADV7533_CEC_DSI, 0x27, 0xCB, 0},
	/* Reset Internal Timing Generator */
	{ADV7533_MAIN, 0xAF, 0x16, 0},
	/* HDMI Mode Select */
	{ADV7533_CEC_DSI, 0x03, 0x89, 0},
	/* HDMI Output Enable */
	{ADV7533_MAIN, 0x40, 0x80, 0},
	/* GC Packet Enable */
	{ADV7533_MAIN, 0x4C, 0x04, 0},
	/* Colour Depth 24-bit per pixel */
	{ADV7533_MAIN, 0x49, 0x00, 0},
	/* Down Dither Output 8-bit Colour Depth */
	{ADV7533_CEC_DSI, 0x05, 0xF8, 0},
	/* ADI Required Write */
	{ADV7533_CEC_DSI, 0xBE, 0x3D, 0},
	/* Test Pattern Disable (0x55[7] = 0) */
	{ADV7533_CEC_DSI, 0x55, 0x00, 0},
};

#define ADV7533_1080P_TG_COMMANDS 31

static struct command_state adv7533_1080p_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info adv7533_1080p_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info adv7533_1080p_video_video_panel = {
	1, 0, 0, 0, 1, 1, 0, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration adv7533_1080p_video_lane_config = {
	4, 0, 1, 1, 1, 1, 1
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t adv7533_1080p_video_timings[] = {
	0xa9, 0x4A, 0x32, 0x00, 0x82, 0x86, 0x38, 0x4e, 0x3d, 0x03, 0x04, 0x00
};

static struct panel_timing adv7533_1080p_video_timing_info = {
	0x0, 0x04, 0x01, 0x27
};

static const uint32_t adv7533_1080p_thulium_video_timings[] = {
		0x1d, 0x1a, 0x03, 0x05, 0x01, 0x03, 0x04, 0xa0,
		0x1d, 0x1a, 0x03, 0x05, 0x01, 0x03, 0x04, 0xa0,
		0x1d, 0x1a, 0x03, 0x05, 0x01, 0x03, 0x04, 0xa0,
		0x1d, 0x1a, 0x03, 0x05, 0x01, 0x03, 0x04, 0xa0,
		0x1d, 0x1a, 0x03, 0x05, 0x01, 0x03, 0x04, 0xa0,
};

#endif /*_PANEL_ADV7533_1080p60_H_*/

