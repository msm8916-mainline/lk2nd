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

#ifndef _PANEL_FL10802_FWVGA_VIDEO_H_
#define _PANEL_FL10802_FWVGA_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config fl10802_fwvga_video_panel_data = {
	"qcom,mdss_dsi_fl10802_fwvga_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 1, 7000, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution fl10802_fwvga_video_panel_res = {
	480, 854, 79, 69, 59, 0, 18, 12, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info fl10802_fwvga_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char fl10802_fwvga_video_on_cmd0[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB9, 0xF1, 0x08, 0x01,
};

static char fl10802_fwvga_video_on_cmd1[] = {
	0x05, 0x00, 0x39, 0xC0,
	0xB1, 0x22, 0x1A, 0x1A,
	0x87, 0xFF, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd2[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xB2, 0x22, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd3[] = {
	0x09, 0x00, 0x39, 0xC0,
	0xB3, 0x01, 0x00, 0x06,
	0x06, 0x18, 0x13, 0x39,
	0x35, 0xFF, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd4[] = {
	0x12, 0x00, 0x39, 0xC0,
	0xBA, 0x31, 0x00, 0x44,
	0x25, 0x91, 0x0A, 0x00,
	0x00, 0xC1, 0x00, 0x00,
	0x00, 0x0D, 0x02, 0x4F,
	0xB9, 0xEE, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd5[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xE3, 0x09, 0x09, 0x03,
	0x03, 0x00, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd6[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xB5, 0x07, 0x07, 0xFF,
};

static char fl10802_fwvga_video_on_cmd7[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xB6, 0x3F, 0x3F, 0xFF,
};

static char fl10802_fwvga_video_on_cmd8[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xB8, 0x64, 0x28, 0xFF,
};

static char fl10802_fwvga_video_on_cmd9[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xB4, 0x02, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd10[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xCC, 0x00, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd11[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBC, 0x47, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd12[] = {
	0x34, 0x00, 0x39, 0xC0,
	0xE9, 0x00, 0x00, 0x09,
	0x03, 0x65, 0x0A, 0x8A,
	0x10, 0x01, 0x00, 0x37,
	0x0D, 0x0A, 0x8A, 0x37,
	0x00, 0x00, 0x18, 0x00,
	0x00, 0x00, 0x25, 0x09,
	0x80, 0x40, 0x00, 0x42,
	0x60, 0x00, 0x00, 0x00,
	0x09, 0x81, 0x50, 0x01,
	0x53, 0x70, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static char fl10802_fwvga_video_on_cmd13[] = {
	0x17, 0x00, 0x39, 0xC0,
	0xEA, 0x94, 0x00, 0x00,
	0x00, 0x08, 0x95, 0x10,
	0x07, 0x35, 0x10, 0x00,
	0x00, 0x00, 0x08, 0x94,
	0x00, 0x06, 0x24, 0x00,
	0x00, 0x00, 0x00, 0xFF,
};

static char fl10802_fwvga_video_on_cmd14[] = {
	0x23, 0x00, 0x39, 0xC0,
	0xE0, 0x00, 0x00, 0x11,
	0x0F, 0x10, 0x2A, 0x26,
	0x34, 0x0C, 0x0F, 0x10,
	0x14, 0x17, 0x14, 0x16,
	0x13, 0x17, 0x00, 0x00,
	0x11, 0x0F, 0x10, 0x2A,
	0x26, 0x34, 0x0C, 0x0F,
	0x10, 0x14, 0x17, 0x14,
	0x16, 0x13, 0x17, 0xFF,
};

static char fl10802_fwvga_video_on_cmd15[] = {
	0x80, 0x00, 0x39, 0xC0,
	0xC1, 0x01, 0x00, 0x0C,
	0x17, 0x20, 0x29, 0x31,
	0x39, 0x42, 0x4B, 0x53,
	0x5B, 0x63, 0x6B, 0x74,
	0x7C, 0x85, 0x8E, 0x97,
	0xA0, 0xA8, 0xAF, 0xB7,
	0xBE, 0xC6, 0xCD, 0xD4,
	0xDB, 0xE2, 0xE9, 0xEF,
	0xF6, 0xFB, 0xFF, 0x3C,
	0x4B, 0x1C, 0x27, 0x26,
	0x2F, 0x1D, 0x08, 0x03,
	0x00, 0x0A, 0x13, 0x1B,
	0x24, 0x2B, 0x32, 0x3A,
	0x42, 0x4A, 0x51, 0x59,
	0x61, 0x69, 0x71, 0x79,
	0x81, 0x8A, 0x93, 0x9C,
	0xA4, 0xAC, 0xB4, 0xBC,
	0xC4, 0xCB, 0xD3, 0xDB,
	0xE2, 0xEA, 0xF2, 0xF9,
	0xFF, 0x2C, 0x34, 0x4F,
	0x49, 0x3F, 0x23, 0x14,
	0x3F, 0x03, 0x00, 0x10,
	0x18, 0x1E, 0x25, 0x2C,
	0x32, 0x37, 0x3E, 0x45,
	0x4C, 0x52, 0x59, 0x60,
	0x65, 0x6D, 0x74, 0x7B,
	0x82, 0x89, 0x92, 0x9A,
	0xA2, 0xAA, 0xB2, 0xB9,
	0xC1, 0xCA, 0xD2, 0xDB,
	0xE6, 0xF1, 0xFF, 0x20,
	0x26, 0x1F, 0x19, 0x4E,
	0x45, 0x24, 0x42, 0x03,
};

static char fl10802_fwvga_video_on_cmd16[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xC7, 0xA0, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd17[] = {
	0x02, 0x00, 0x39, 0xC0,
	0x53, 0x24, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd18[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xC8, 0x10, 0x10, 0xFF,
};

static char fl10802_fwvga_video_on_cmd19[] = {
	0x02, 0x00, 0x39, 0xC0,
	0x51, 0x00, 0xFF, 0xFF,
};

static char fl10802_fwvga_video_on_cmd20[] = {
	0x11, 0x00, 0x05, 0x80
};

static char fl10802_fwvga_video_on_cmd21[] = {
	0x29, 0x00, 0x05, 0x80
};

static char fl10802_fwvga_video_on_cmd22[] = {
	0x02, 0x00, 0x39, 0xC0,
	0x51, 0xFF, 0xFF, 0xFF,
};

static struct mipi_dsi_cmd fl10802_fwvga_video_on_command[] = {
	{0x8, fl10802_fwvga_video_on_cmd0, 0x00},
	{0xc, fl10802_fwvga_video_on_cmd1, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd2, 0x00},
	{0x10, fl10802_fwvga_video_on_cmd3, 0x00},
	{0x18, fl10802_fwvga_video_on_cmd4, 0x00},
	{0xc, fl10802_fwvga_video_on_cmd5, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd6, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd7, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd8, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd9, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd10, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd11, 0x00},
	{0x38, fl10802_fwvga_video_on_cmd12, 0x00},
	{0x1c, fl10802_fwvga_video_on_cmd13, 0x00},
	{0x28, fl10802_fwvga_video_on_cmd14, 0x00},
	{0x84, fl10802_fwvga_video_on_cmd15, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd16, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd17, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd18, 0x00},
	{0x8, fl10802_fwvga_video_on_cmd19, 0x00},
	{0x4, fl10802_fwvga_video_on_cmd20, 0x78},
	{0x4, fl10802_fwvga_video_on_cmd21, 0x78},
	{0x8, fl10802_fwvga_video_on_cmd22, 0x00}
};

#define FL10802_FWVGA_VIDEO_ON_COMMAND 23


static char fl10802_fwvga_videooff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char fl10802_fwvga_videooff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd fl10802_fwvga_video_off_command[] = {
	{0x4, fl10802_fwvga_videooff_cmd0, 0x32},
	{0x4, fl10802_fwvga_videooff_cmd1, 0x78}
};

#define FL10802_FWVGA_VIDEO_OFF_COMMAND 2


static struct command_state fl10802_fwvga_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info fl10802_fwvga_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info fl10802_fwvga_video_video_panel = {
	1, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration fl10802_fwvga_video_lane_config = {
	2, 0, 1, 1, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t fl10802_fwvga_video_timings[] = {
	0xaf, 0x23, 0x1c, 0x00, 0x36, 0x33, 0x1d, 0x25, 0x2d, 0x03, 0x04, 0x00
};

static struct panel_timing fl10802_fwvga_video_timing_info = {
	0, 4, 0x1f, 0x30
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence fl10802_fwvga_video_reset_seq = {
	{1, 0, 1, }, {20, 2, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight fl10802_fwvga_video_backlight = {
  BL_DCS, 1, 255, 100, 2, "PMIC_8941"
};

#define FL10802_FWVGA_VIDEO_SIGNATURE 0xFFFF

#endif /*_PANEL_FL10802_FWVGA_VIDEO_H_*/
