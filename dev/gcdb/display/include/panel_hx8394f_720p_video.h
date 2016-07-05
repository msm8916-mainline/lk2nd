/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.

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

#ifndef _PANEL_HX8394F_720P_VIDEO_H_
#define _PANEL_HX8394F_720P_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config hx8394f_720p_video_panel_data = {
	"qcom,mdss_dsi_hx8394f_720p_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution hx8394f_720p_video_panel_res = {
	720, 1280, 16, 16, 8, 0, 15, 12, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info hx8394f_720p_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char hx8394f_720p_video_on_cmd0[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB9, 0xFF, 0x83, 0x94,
};

static char hx8394f_720p_video_on_cmd1[] = {
	0x0B, 0x00, 0x39, 0xC0,
	0xB1, 0x50, 0x14, 0x74,
	0x09, 0x31, 0x24, 0x71,
	0x31, 0x55, 0x2F, 0xFF,
};

static char hx8394f_720p_video_on_cmd2[] = {
	0x07, 0x00, 0x39, 0xC0,
	0xBA, 0x63, 0x03, 0x68,
	0x6B, 0xB2, 0xC0, 0xFF,
};

static char hx8394f_720p_video_on_cmd3[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xD2, 0x77, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd4[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xB2, 0x00, 0x80, 0x64,
	0x10, 0x07, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd5[] = {
	0x16, 0x00, 0x39, 0xC0,
	0xB4, 0x01, 0x74, 0x01,
	0x74, 0x01, 0x74, 0x01,
	0x0C, 0x86, 0x75, 0x00,
	0x3F, 0x01, 0x74, 0x01,
	0x74, 0x01, 0x74, 0x01,
	0x0C, 0x86, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd6[] = {
	0x22, 0x00, 0x39, 0xC0,
	0xD3, 0x00, 0x00, 0x07,
	0x07, 0x40, 0x1E, 0x08,
	0x00, 0x32, 0x10, 0x08,
	0x00, 0x08, 0x54, 0x15,
	0x10, 0x05, 0x04, 0x02,
	0x12, 0x10, 0x05, 0x07,
	0x23, 0x23, 0x0C, 0x0C,
	0x27, 0x10, 0x07, 0x07,
	0x10, 0x40, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd7[] = {
	0x2D, 0x00, 0x39, 0xC0,
	0xD5, 0x19, 0x19, 0x18,
	0x18, 0x1b, 0x1b, 0x1a,
	0x1a, 0x04, 0x05, 0x06,
	0x07, 0x00, 0x01, 0x02,
	0x03, 0x20, 0x21, 0x18,
	0x18, 0x22, 0x23, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0xFF, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd8[] = {
	0x2D, 0x00, 0x39, 0xC0,
	0xD6, 0x18, 0x18, 0x19,
	0x19, 0x1B, 0x1B, 0x1A,
	0x1A, 0x03, 0x02, 0x01,
	0x00, 0x07, 0x06, 0x05,
	0x04, 0x23, 0x22, 0x18,
	0x18, 0x21, 0x20, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0xFF, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd9[] = {
	0x3B, 0x00, 0x39, 0xC0,
	0xE0, 0x00, 0x03, 0x09,
	0x11, 0x11, 0x14, 0x18,
	0x16, 0x2E, 0x3D, 0x4D,
	0x4D, 0x58, 0x6C, 0x72,
	0x78, 0x88, 0x8B, 0x86,
	0xA4, 0xB2, 0x58, 0x55,
	0x59, 0x5B, 0x5D, 0x60,
	0x64, 0x7F, 0x00, 0x03,
	0x09, 0x0F, 0x11, 0x14,
	0x18, 0x16, 0x2E, 0x3D,
	0x4D, 0x4D, 0x58, 0x6D,
	0x73, 0x78, 0x88, 0x8B,
	0x87, 0xA5, 0xB2, 0x58,
	0x55, 0x58, 0x5B, 0x5D,
	0x61, 0x65, 0x7F, 0xFF,
};

static char hx8394f_720p_video_on_cmd10[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xCC, 0x0B, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd11[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xC0, 0x1F, 0x31, 0xFF,
};

static char hx8394f_720p_video_on_cmd12[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xB6, 0x92, 0x92, 0xFF,
};

static char hx8394f_720p_video_on_cmd13[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBD, 0x01, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd14[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xB1, 0x00, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd15[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBD, 0x00, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd16[] = {
	0x08, 0x00, 0x39, 0xC0,
	0xBF, 0x40, 0x81, 0x50,
	0x00, 0x1A, 0xFC, 0x01,
};

static char hx8394f_720p_video_on_cmd17[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xC6, 0xEF, 0xFF, 0xFF,
};

static char hx8394f_720p_video_on_cmd18[] = {
	0x11, 0x00, 0x05, 0x80
};

static char hx8394f_720p_video_on_cmd19[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8394f_720p_video_on_command[] = {
	{0x8, hx8394f_720p_video_on_cmd0, 0x00},
	{0x10, hx8394f_720p_video_on_cmd1, 0x00},
	{0xc, hx8394f_720p_video_on_cmd2, 0x00},
	{0x8, hx8394f_720p_video_on_cmd3, 0x00},
	{0xc, hx8394f_720p_video_on_cmd4, 0x00},
	{0x1c, hx8394f_720p_video_on_cmd5, 0x00},
	{0x28, hx8394f_720p_video_on_cmd6, 0x00},
	{0x34, hx8394f_720p_video_on_cmd7, 0x00},
	{0x34, hx8394f_720p_video_on_cmd8, 0x00},
	{0x40, hx8394f_720p_video_on_cmd9, 0x00},
	{0x8, hx8394f_720p_video_on_cmd10, 0x00},
	{0x8, hx8394f_720p_video_on_cmd11, 0x00},
	{0x8, hx8394f_720p_video_on_cmd12, 0x00},
	{0x8, hx8394f_720p_video_on_cmd13, 0x00},
	{0x8, hx8394f_720p_video_on_cmd14, 0x00},
	{0x8, hx8394f_720p_video_on_cmd15, 0x00},
	{0xc, hx8394f_720p_video_on_cmd16, 0x00},
	{0x8, hx8394f_720p_video_on_cmd17, 0x00},
	{0x4, hx8394f_720p_video_on_cmd18, 0x78},
	{0x4, hx8394f_720p_video_on_cmd19, 0x14}
};

#define HX8394F_720P_VIDEO_ON_COMMAND 20

static char hx8394f_720p_videooff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char hx8394f_720p_videooff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8394f_720p_video_off_command[] = {
	{0x4, hx8394f_720p_videooff_cmd0, 0x14},
	{0x4, hx8394f_720p_videooff_cmd1, 0x78}
};

#define HX8394F_720P_VIDEO_OFF_COMMAND 2


static struct command_state hx8394f_720p_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info hx8394f_720p_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info hx8394f_720p_video_video_panel = {
	1, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration hx8394f_720p_video_lane_config = {
  4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t hx8394f_720p_video_timings[] = {
	0x6a, 0x16, 0x0e, 0x00, 0x38, 0x3c, 0x12, 0x1a, 0x10, 0x03, 0x04, 0x00
};

static struct panel_timing hx8394f_720p_video_timing_info = {
	0, 4, 0x04, 0x17
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence hx8394f_720p_video_reset_seq = {
	{1, 0, 1, }, {20, 2, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight hx8394f_720p_video_backlight = {
	BL_PWM, 1, 255, 0, 2, 0
};

#define HX8394F_720P_VIDEO_SIGNATURE 0xFFFF

#endif /*_PANEL_HX8394F_720P_VIDEO_H_*/
