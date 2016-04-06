/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 *
 */

#ifndef _PANEL_LEAD_FL10802_FWVGA_VIDEO_H_
#define _PANEL_LEAD_FL10802_FWVGA_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config lead_fl10802_fwvga_video_panel_data = {
	"qcom,mdss_dsi_lead_fl10802_fwvga_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 1, 7000, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution lead_fl10802_fwvga_video_panel_res = {
	480, 854, 94, 84, 60, 0, 18, 12, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info lead_fl10802_fwvga_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char lead_fl10802_fwvga_video_on_cmd0[] = {
	0x04, 0x00, 0x29, 0xc0,
	0xb9, 0xf1, 0x08, 0x01,
};

static char lead_fl10802_fwvga_video_on_cmd1[] = {
	0x08, 0x00, 0x29, 0xc0,
	0xb1, 0x22, 0x1a, 0x1a,
	0x87, 0x30, 0x01, 0xab,
};

static char lead_fl10802_fwvga_video_on_cmd2[] = {
	0x02, 0x00, 0x29, 0xc0,
	0xb2, 0x22, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd3[] = {
	0x09, 0x00, 0x29, 0xc0,
	0xb3, 0x01, 0x00, 0x06,
	0x06, 0x18, 0x13, 0x39,
	0x35, 0xff, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd4[] = {
	0x12, 0x00, 0x29, 0xc0,
	0xba, 0x31, 0x00, 0x44,
	0x25, 0x91, 0x0a, 0x00,
	0x00, 0xc1, 0x00, 0x00,
	0x00, 0x0d, 0x02, 0x4f,
	0xb9, 0xee, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd5[] = {
	0x06, 0x00, 0x29, 0xc0,
	0xe3, 0x09, 0x09, 0x03,
	0x03, 0x00, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd6[] = {
	0x02, 0x00, 0x29, 0xc0,
	0xb4, 0x00, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd7[] = {
	0x03, 0x00, 0x29, 0xc0,
	0xb5, 0x07, 0x07, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd8[] = {
	0x03, 0x00, 0x29, 0xc0,
	0xb6, 0x32, 0x32, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd9[] = {
	0x03, 0x00, 0x29, 0xc0,
	0xb8, 0x64, 0x22, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd10[] = {
	0x02, 0x00, 0x29, 0xc0,
	0xc7, 0xa0, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd11[] = {
	0x03, 0x00, 0x29, 0xc0,
	0xc8, 0x10, 0x00, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd12[] = {
	0x02, 0x00, 0x29, 0xc0,
	0xcc, 0x00, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd13[] = {
	0x02, 0x00, 0x29, 0xc0,
	0xbc, 0x47, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd14[] = {
	0x34, 0x00, 0x29, 0xc0,
	0xe9, 0x00, 0x00, 0x09,
	0x03, 0x65, 0x0a, 0x8a,
	0x10, 0x01, 0x00, 0x37,
	0x0d, 0x0a, 0x8a, 0x37,
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

static char lead_fl10802_fwvga_video_on_cmd15[] = {
	0x13, 0x00, 0x29, 0xc0,
	0xea, 0x94, 0x00, 0x00,
	0x00, 0x08, 0x95, 0x10,
	0x07, 0x35, 0x10, 0x00,
	0x00, 0x00, 0x08, 0x94,
	0x00, 0x06, 0x24, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd16[] = {
	0x23, 0x00, 0x29, 0xc0,
	0xe0, 0x00, 0x00, 0x11,
	0x0f, 0x10, 0x2a, 0x26,
	0x34, 0x0c, 0x0f, 0x10,
	0x14, 0x17, 0x14, 0x16,
	0x13, 0x17, 0x00, 0x00,
	0x11, 0x0f, 0x10, 0x2a,
	0x26, 0x34, 0x0c, 0x0f,
	0x10, 0x14, 0x17, 0x14,
	0x16, 0x13, 0x17, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd17[]= {
	0x02, 0x00, 0x39, 0xc0,
	0x51, 0x00, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd18[]= {
	0x02, 0x00, 0x39, 0xc0,
	0x53, 0x24, 0xff, 0xff,
};

static char lead_fl10802_fwvga_video_on_cmd19[] = {
	0x11, 0x00, 0x05, 0x80
};

static char lead_fl10802_fwvga_video_on_cmd20[] = {
	0x29, 0x00, 0x05, 0x80
};

static char lead_fl10802_fwvga_video_on_cmd21[] = {
	0x02, 0x00, 0x39, 0xc0,
	0x51, 0x80, 0xff, 0xff,
};

static struct mipi_dsi_cmd lead_fl10802_fwvga_video_on_command[] = {
	{sizeof(lead_fl10802_fwvga_video_on_cmd0), lead_fl10802_fwvga_video_on_cmd0, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd1), lead_fl10802_fwvga_video_on_cmd1, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd2), lead_fl10802_fwvga_video_on_cmd2, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd3), lead_fl10802_fwvga_video_on_cmd3, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd4), lead_fl10802_fwvga_video_on_cmd4, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd5), lead_fl10802_fwvga_video_on_cmd5, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd6), lead_fl10802_fwvga_video_on_cmd6, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd7), lead_fl10802_fwvga_video_on_cmd7, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd8), lead_fl10802_fwvga_video_on_cmd8, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd9), lead_fl10802_fwvga_video_on_cmd9, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd10), lead_fl10802_fwvga_video_on_cmd10, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd11), lead_fl10802_fwvga_video_on_cmd11, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd12), lead_fl10802_fwvga_video_on_cmd12, 0x05},
	{sizeof(lead_fl10802_fwvga_video_on_cmd13), lead_fl10802_fwvga_video_on_cmd13, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd14), lead_fl10802_fwvga_video_on_cmd14, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd15), lead_fl10802_fwvga_video_on_cmd15, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd16), lead_fl10802_fwvga_video_on_cmd16, 0x05},
	{sizeof(lead_fl10802_fwvga_video_on_cmd17), lead_fl10802_fwvga_video_on_cmd17, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd18), lead_fl10802_fwvga_video_on_cmd18, 0x00},
	{sizeof(lead_fl10802_fwvga_video_on_cmd19), lead_fl10802_fwvga_video_on_cmd19, 0x78},
	{sizeof(lead_fl10802_fwvga_video_on_cmd20), lead_fl10802_fwvga_video_on_cmd20, 0x78},
	{sizeof(lead_fl10802_fwvga_video_on_cmd21), lead_fl10802_fwvga_video_on_cmd21, 0x00},
};

#define LEAD_FL10802_FWVGA_VIDEO_ON_COMMAND 22

static char lead_fl10802_fwvga_video_off_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char lead_fl10802_fwvga_video_off_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd lead_fl10802_fwvga_video_off_command[] = {
	{0x4, lead_fl10802_fwvga_video_off_cmd0, 0x32},
	{0x4, lead_fl10802_fwvga_video_off_cmd1, 0x78}
};

#define LEAD_FL10802_FWVGA_VIDEO_OFF_COMMAND 2

static struct command_state lead_fl10802_fwvga_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info lead_fl10802_fwvga_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info lead_fl10802_fwvga_video_video_panel = {
	1, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration lead_fl10802_fwvga_video_lane_config = {
	2, 0, 1, 1, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t lead_fl10802_fwvga_video_timings[] = {
	0x87, 0x1c, 0x12, 0x00, 0x42, 0x44, 0x18, 0x20, 0x17, 0x03, 0x04, 0x00
};

static struct panel_timing lead_fl10802_fwvga_video_timing_info = {
	0, 4, 0x04, 0x1b
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence lead_fl10802_fwvga_video_reset_seq = {
	{1, 0, 1, }, {20, 10, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight lead_fl10802_fwvga_video_backlight = {
  BL_DCS, 1, 255, 100, 2, "PMIC_8941"
};

#define LEAD_FL10802_FWVGA_VIDEO_SIGNATURE 0xFFFF

#endif /*_PANEL_LEAD_FL10802_FWVGA_VIDEO_H_*/
