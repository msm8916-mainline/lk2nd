/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PANEL_HX8399A_1080P_VIDEO_H_

#define _PANEL_HX8399A_1080P_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config hx8399a_1080p_video_panel_data = {
	"qcom,mdss_dsi_hx8399a_1080p_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 866400000, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution hx8399a_1080p_video_panel_res = {
	1080, 1920, 90, 60, 20, 0, 9, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info hx8399a_1080p_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char hx8399a_1080p_video_on_cmd0[] = {
	0x04, 0x00, 0x29, 0xC0,
	0xB9, 0xFF, 0x83, 0x99,
};

static char hx8399a_1080p_video_on_cmd1[] = {
	0x03, 0x00, 0x29, 0xC0,
	0xBA, 0x63, 0x82, 0xFF,
};

static char hx8399a_1080p_video_on_cmd2[] = {
	0x0D, 0x00, 0x29, 0xC0,
	0xB1, 0x00, 0x74, 0x31,
	0x31, 0x44, 0x09, 0x22,
	0x22, 0x71, 0xF1, 0x63,
	0x6D, 0xFF, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd3[] = {
	0x0B, 0x00, 0x29, 0xC0,
	0xB2, 0x00, 0x80, 0x00,
	0x7F, 0x05, 0x07, 0x23,
	0x4D, 0x02, 0x02, 0xFF,
};

static char hx8399a_1080p_video_on_cmd4[] = {
	0x29, 0x00, 0x29, 0xC0,
	0xB4, 0x00, 0xFF, 0x00,
	0x40, 0x00, 0x3E, 0x00,
	0x00, 0x02, 0x00, 0x00,
	0x01, 0x00, 0x10, 0x01,
	0x02, 0x04, 0x31, 0x00,
	0x01, 0x44, 0x00, 0x40,
	0x00, 0x3E, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x01,
	0x00, 0x10, 0x01, 0x02,
	0x04, 0x01, 0x00, 0x01,
	0x44, 0xFF, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd5[] = {
	0x02, 0x00, 0x29, 0xC0,
	0xD2, 0x00, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd6[] = {
	0x20, 0x00, 0x29, 0xC0,
	0xD3, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x30, 0x00,
	0x32, 0x10, 0x05, 0x00,
	0x05, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01,
	0x07, 0x07, 0x03, 0x00,
	0x00, 0x00, 0x05, 0x08,
};

static char hx8399a_1080p_video_on_cmd7[] = {
	0x21, 0x00, 0x29, 0xC0,
	0xD5, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x19, 0x19, 0x18,
	0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x02, 0x01,
	0x00, 0x21, 0x20, 0x30,
	0x30, 0x31, 0x31, 0x32,
	0x32, 0xFF, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd8[] = {
	0x21, 0x00, 0x29, 0xC0,
	0xD6, 0x18, 0x18, 0x40,
	0x40, 0x40, 0x40, 0x40,
	0x40, 0x18, 0x18, 0x19,
	0x19, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x40,
	0x40, 0x00, 0x01, 0x02,
	0x03, 0x20, 0x21, 0x30,
	0x30, 0x31, 0x31, 0x32,
	0x32, 0xFF, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd9[] = {
	0x31, 0x00, 0x29, 0xC0,
	0xD8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0xC0, 0x00, 0x0A,
	0xBF, 0xC0, 0x00, 0x0A,
	0xBF, 0xFF, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd10[] = {
	0x03, 0x00, 0x29, 0xC0,
	0xB6, 0x34, 0x34, 0xFF,
};

static char hx8399a_1080p_video_on_cmd11[] = {
	0x02, 0x00, 0x29, 0xC0,
	0xCC, 0x08, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd12[] = {
	0x2B, 0x00, 0x29, 0xC0,
	0xE0, 0x01, 0x10, 0x15,
	0x2B, 0x32, 0x3E, 0x22,
	0x3C, 0x07, 0x0C, 0x0F,
	0x12, 0x15, 0x13, 0x14,
	0x10, 0x16, 0x0A, 0x17,
	0x06, 0x12, 0x01, 0x10,
	0x15, 0x2B, 0x32, 0x3E,
	0x22, 0x3C, 0x07, 0x0C,
	0x0F, 0x12, 0x15, 0x13,
	0x14, 0x10, 0x16, 0x0A,
	0x17, 0x06, 0x12, 0xFF,
};

static char hx8399a_1080p_video_on_cmd13[] = {
	0x04, 0x00, 0x29, 0xC0,
	0xBF, 0xCF, 0x00, 0x46,
};

static char hx8399a_1080p_video_on_cmd14[] = {
	0x02, 0x00, 0x29, 0xC0,
	0x36, 0xc0, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd15[] = {
	0x02, 0x00, 0x29, 0xC0,
	0x11, 0x00, 0xFF, 0xFF,
};

static char hx8399a_1080p_video_on_cmd16[] = {
	0x02, 0x00, 0x29, 0xC0,
	0x29, 0x00, 0xFF, 0xFF,
};

static struct mipi_dsi_cmd hx8399a_1080p_video_on_command[] = {
	{0x8, hx8399a_1080p_video_on_cmd0, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd1, 0x00},
	{0x14, hx8399a_1080p_video_on_cmd2, 0x00},
	{0x10, hx8399a_1080p_video_on_cmd3, 0x00},
	{0x30, hx8399a_1080p_video_on_cmd4, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd5, 0x00},
	{0x24, hx8399a_1080p_video_on_cmd6, 0x00},
	{0x28, hx8399a_1080p_video_on_cmd7, 0x00},
	{0x28, hx8399a_1080p_video_on_cmd8, 0x00},
	{0x38, hx8399a_1080p_video_on_cmd9, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd10, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd11, 0x00},
	{0x30, hx8399a_1080p_video_on_cmd12, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd13, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd14, 0x00},
	{0x8, hx8399a_1080p_video_on_cmd15, 0x78},
	{0x8, hx8399a_1080p_video_on_cmd16, 0x0A}
};

#define HX8399A_1080P_VIDEO_ON_COMMAND 17


static char hx8399a_1080p_videooff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char hx8399a_1080p_videooff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8399a_1080p_video_off_command[] = {
	{0x4, hx8399a_1080p_videooff_cmd0, 0x32},
	{0x4, hx8399a_1080p_videooff_cmd1, 0x78}
};

#define HX8399A_1080P_VIDEO_OFF_COMMAND 2


static struct command_state hx8399a_1080p_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info hx8399a_1080p_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info hx8399a_1080p_video_video_panel = {
	0, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration hx8399a_1080p_video_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t hx8399a_1080p_video_timings[] = {
	0xf9, 0x3d, 0x34, 0x00, 0x58, 0x4d, 0x36, 0x3f, 0x53, 0x03, 0x04, 0x00
};

static struct panel_timing hx8399a_1080p_video_timing_info = {
	0, 4, 0x1e, 0x38
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence hx8399a_1080p_video_reset_seq = {
	{1, 0, 1, }, {20, 2, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight hx8399a_1080p_video_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#define HX8399A_1080P_VIDEO_SIGNATURE 0xA0000

#endif /*_PANEL_HX8399A_1080P_VIDEO_H_*/
