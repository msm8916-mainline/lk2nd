/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_HX8399C_HD_PLUS_VIDEO_H_

#define _PANEL_HX8399C_HD_PLUS_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config hx8399c_hd_plus_video_panel_data = {
	"qcom,mdss_dsi_hx8399c_hd_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution hx8399c_hd_plus_video_panel_res = {
	720, 1440, 48, 48, 16, 0, 36, 40, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info hx8399c_hd_plus_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char hx8399c_hd_plus_video_on_cmd0[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB9, 0xFF, 0x83, 0x99,
};

static char hx8399c_hd_plus_video_on_cmd1[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xD2, 0x88, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd2[] = {
	0x0C, 0x00, 0x39, 0xC0,
	0xB1, 0x02, 0x04, 0x72,
	0x92, 0x01, 0x32, 0xAA,
	0x11, 0x11, 0x52, 0x57,
};

static char hx8399c_hd_plus_video_on_cmd3[] = {
	0x10, 0x00, 0x39, 0xC0,
	0xB2, 0x00, 0x80, 0x80,
	0xCC, 0x05, 0x07, 0x5A,
	0x11, 0x10, 0x10, 0x00,
	0x1E, 0x70, 0x03, 0xD4,
};

static char hx8399c_hd_plus_video_on_cmd4[] = {
	0x2D, 0x00, 0x39, 0xC0,
	0xB4, 0x00, 0xFF, 0x59,
	0x59, 0x01, 0xAB, 0x00,
	0x00, 0x09, 0x00, 0x03,
	0x05, 0x00, 0x28, 0x03,
	0x0B, 0x0D, 0x21, 0x03,
	0x02, 0x00, 0x0C, 0xA3,
	0x80, 0x59, 0x59, 0x02,
	0xAB, 0x00, 0x00, 0x09,
	0x00, 0x03, 0x05, 0x00,
	0x28, 0x03, 0x0B, 0x0D,
	0x02, 0x00, 0x0C, 0xA3,
	0x01, 0xFF, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd5[] = {
	0x22, 0x00, 0x39, 0xC0,
	0xD3, 0x00, 0x0C, 0x03,
	0x03, 0x00, 0x00, 0x10,
	0x10, 0x00, 0x00, 0x03,
	0x00, 0x03, 0x00, 0x08,
	0x78, 0x08, 0x78, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x24, 0x02, 0x05, 0x05,
	0x03, 0x00, 0x00, 0x00,
	0x05, 0x40, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd6[] = {
	0x21, 0x00, 0x39, 0xC0,
	0xD5, 0x20, 0x20, 0x19,
	0x19, 0x18, 0x18, 0x02,
	0x03, 0x00, 0x01, 0x24,
	0x24, 0x18, 0x18, 0x18,
	0x18, 0x24, 0x24, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x2F,
	0x2F, 0x30, 0x30, 0x31,
	0x31, 0xFF, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd7[] = {
	0x21, 0x00, 0x39, 0xC0,
	0xD6, 0x24, 0x24, 0x18,
	0x18, 0x19, 0x19, 0x01,
	0x00, 0x03, 0x02, 0x24,
	0x24, 0x18, 0x18, 0x18,
	0x18, 0x20, 0x20, 0x40,
	0x40, 0x40, 0x40, 0x40,
	0x40, 0x40, 0x40, 0x2F,
	0x2F, 0x30, 0x30, 0x31,
	0x31, 0xFF, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd8[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBD, 0x00, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd9[] = {
	0x11, 0x00, 0x39, 0xC0,
	0xD8, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xAA, 0xBA, 0xAA,
	0xAA, 0xAA, 0xBA, 0xAA,
	0xAA, 0xFF, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd10[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBD, 0x01, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd11[] = {
	0x11, 0x00, 0x39, 0xC0,
	0xD8, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x82, 0xEA, 0xAA,
	0xAA, 0x82, 0xEA, 0xAA,
	0xAA, 0xFF, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd12[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBD, 0x02, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd13[] = {
	0x09, 0x00, 0x39, 0xC0,
	0xD8, 0xFF, 0xFF, 0xC0,
	0x3F, 0xFF, 0xFF, 0xC0,
	0x3F, 0xFF, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd14[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xBD, 0x00, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd15[] = {
	0x37, 0x00, 0x39, 0xC0,
	0xE0, 0x01, 0x21, 0x31,
	0x2D, 0x66, 0x6F, 0x7B,
	0x75, 0x7A, 0x81, 0x86,
	0x89, 0x8C, 0x90, 0x95,
	0x97, 0x9A, 0xA1, 0xA2,
	0xAA, 0x9E, 0xAD, 0xB0,
	0x5B, 0x57, 0x63, 0x7A,
	0x01, 0x21, 0x31, 0x2D,
	0x66, 0x6F, 0x7B, 0x75,
	0x7A, 0x81, 0x86, 0x89,
	0x9C, 0x90, 0x95, 0x97,
	0x9A, 0xA1, 0xA2, 0xAA,
	0x9E, 0xAD, 0xB0, 0x5B,
	0x57, 0x63, 0x7A, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd16[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xB6, 0x7E, 0x7E, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd17[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xCC, 0x08, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd18[] = {
	0x02, 0x00, 0x39, 0xC0,
	0x35, 0x00, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd19[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xdd, 0x03, 0xFF, 0xFF,
};

static char hx8399c_hd_plus_video_on_cmd20[] = {
	 0x11, 0x00, 0x05, 0x80,
};

static char hx8399c_hd_plus_video_on_cmd21[] = {
	0x29, 0x00, 0x05, 0x80,
};

static struct mipi_dsi_cmd hx8399c_hd_plus_video_on_command[] = {
	{0x08, hx8399c_hd_plus_video_on_cmd0, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd1, 0x00},
	{0x10, hx8399c_hd_plus_video_on_cmd2, 0x00},
	{0x14, hx8399c_hd_plus_video_on_cmd3, 0x00},
	{0x34, hx8399c_hd_plus_video_on_cmd4, 0x00},
	{0x28, hx8399c_hd_plus_video_on_cmd5, 0x05},
	{0x28, hx8399c_hd_plus_video_on_cmd6, 0x05},
	{0x28, hx8399c_hd_plus_video_on_cmd7, 0x05},
	{0x08, hx8399c_hd_plus_video_on_cmd8, 0x00},
	{0x18, hx8399c_hd_plus_video_on_cmd9, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd10, 0x00},
	{0x18, hx8399c_hd_plus_video_on_cmd11, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd12, 0x00},
	{0x10, hx8399c_hd_plus_video_on_cmd13, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd14, 0x00},
	{0x3C, hx8399c_hd_plus_video_on_cmd15, 0x05},
	{0x08, hx8399c_hd_plus_video_on_cmd16, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd17, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd18, 0x00},
	{0x08, hx8399c_hd_plus_video_on_cmd19, 0x00},
	{0x04, hx8399c_hd_plus_video_on_cmd20, 0x78},
	{0x04, hx8399c_hd_plus_video_on_cmd21, 0x14}
};

#define HX8399C_HD_PLUS_VIDEO_ON_COMMAND 22


static char hx8399c_hd_plus_videooff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char hx8399c_hd_plus_videooff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8399c_hd_plus_video_off_command[] = {
	{0x4, hx8399c_hd_plus_videooff_cmd0, 0x14},
	{0x4, hx8399c_hd_plus_videooff_cmd1, 0x78}
};

#define HX8399C_HD_PLUS_VIDEO_OFF_COMMAND 2


static struct command_state hx8399c_hd_plus_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info hx8399c_hd_plus_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info hx8399c_hd_plus_video_video_panel = {
	0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration hx8399c_hd_plus_video_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t hx8399c_hd_plus_video_timings[] = {
	0x90, 0x1C, 0x12, 0x00, 0x42, 0x44, 0x18, 0x20, 0x16, 0x03, 0x04, 0x00
};

static const uint32_t hx8399c_hd_plus_video_12nm_timings[] = {
	0x09, 0x06, 0x0A, 0x02, 0x00, 0x05, 0x02, 0x08
};

static const uint32_t hx8399c_hd_plus_14nm_video_timings[] = {
	0x1e, 0x1c, 0x04, 0x06, 0x02, 0x03, 0x04, 0xa0,
	0x1e, 0x1c, 0x04, 0x06, 0x02, 0x03, 0x04, 0xa0,
	0x1e, 0x1c, 0x04, 0x06, 0x02, 0x03, 0x04, 0xa0,
	0x1e, 0x1c, 0x04, 0x06, 0x02, 0x03, 0x04, 0xa0,
	0x1e, 0x0e, 0x04, 0x05, 0x02, 0x03, 0x04, 0xa0,
};
static struct panel_timing hx8399c_hd_plus_video_timing_info = {
	0, 4, 0x0e, 0x31
};

static struct labibb_desc hx8399c_hd_plus_video_labibb = {
	0, 1, 5700000, 5700000, 5700000, 5700000, 3, 3, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence hx8399c_hd_plus_video_panel_reset_seq = {
	{1, 0, 1, }, {10, 10, 10, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight hx8399c_hd_plus_video_backlight = {
	0, 1, 4095, 100, 1, "PMIC_8941"
};

#define HX8399C_HD_PLUS_VIDEO_SIGNATURE 0xFFFF

#endif /*_PANEL_HX8399C_HD_PLUS_VIDEO_H_*/
