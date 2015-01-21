/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_HX8394D_QHD_VIDEO_H_

#define _PANEL_HX8394D_QHD_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config hx8394d_qhd_video_panel_data = {
	"qcom,mdss_dsi_hx8394d_qhd_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution hx8394d_qhd_video_panel_res = {
	540, 960, 52, 100, 24, 0, 8, 20, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info hx8394d_qhd_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char hx8394d_qhd_video_on_cmd0[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xb9, 0xff, 0x83, 0x94,
};


static char hx8394d_qhd_video_on_cmd1[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xba, 0x33, 0x83, 0xFF,
};

static char hx8394d_qhd_video_on_cmd2[] = {
	0x10, 0x00, 0x39, 0xC0,
	0xb1, 0x6c, 0x12, 0x12,
	0x37, 0x04, 0x11, 0xf1,
	0x80, 0xec, 0x94, 0x23,
	0x80, 0xc0, 0xd2, 0x18,
};


static char hx8394d_qhd_video_on_cmd3[] = {
	0x0c, 0x00, 0x39, 0xC0,
	0xb2, 0x00, 0x64, 0x0e,
	0x0d, 0x32, 0x23, 0x08,
	0x08, 0x1c, 0x4d, 0x00,
};


static char hx8394d_qhd_video_on_cmd4[] = {
	0x0d, 0x00, 0x39, 0xC0,
	0xb4, 0x00, 0xff, 0x03,
	0x50, 0x03, 0x50, 0x03,
	0x50, 0x01, 0x6a, 0x01,
	0x6a, 0xFF, 0xFF, 0xFF,
};


static char hx8394d_qhd_video_on_cmd5[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xbc, 0x07, 0xFF, 0xFF,
};


static char hx8394d_qhd_video_on_cmd6[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xbf, 0x41, 0x0e, 0x01,
};


static char hx8394d_qhd_video_on_cmd7[] = {
	0x1f, 0x00, 0x39, 0xC0,
	0xd3, 0x00, 0x07, 0x00,
	0x00, 0x00, 0x10, 0x00,
	0x32, 0x10, 0x05, 0x00,
	0x00, 0x32, 0x10, 0x00,
	0x00, 0x00, 0x32, 0x10,
	0x00, 0x00, 0x00, 0x36,
	0x03, 0x09, 0x09, 0x37,
	0x00, 0x00, 0x37, 0xFF,
};


static char hx8394d_qhd_video_on_cmd8[] = {
	0x2d, 0x00, 0x39, 0xC0,
	0xd5, 0x02, 0x03, 0x00,
	0x01, 0x06, 0x07, 0x04,
	0x05, 0x20, 0x21, 0x22,
	0x23, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x24,
	0x25, 0x18, 0x18, 0x19,
	0x19, 0xFF, 0xFF, 0xFF,
};


static char hx8394d_qhd_video_on_cmd9[] = {
	0x2d, 0x00, 0x39, 0xC0,
	0xd6, 0x05, 0x04, 0x07,
	0x06, 0x01, 0x00, 0x03,
	0x02, 0x23, 0x22, 0x21,
	0x20, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x58,
	0x58, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x18, 0x25,
	0x24, 0x19, 0x19, 0x18,
	0x18, 0xFF, 0xFF, 0xFF,
};


static char hx8394d_qhd_video_on_cmd10[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xcc, 0x09, 0xFF, 0xFF,
};


static char hx8394d_qhd_video_on_cmd11[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xc0, 0x30, 0x14, 0xFF,
};

static char hx8394d_qhd_video_on_cmd12[] = {
	0x05, 0x00, 0x39, 0xC0,
	0xc7, 0x00, 0xc0, 0x40,
	0xc0, 0xFF, 0xFF, 0xFF,
};

static char hx8394d_qhd_video_on_cmd13[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xb6, 0x43, 0x43, 0xFF,
};

static char hx8394d_qhd_video_on_cmd14[] = {
	0x11, 0x00, 0x05, 0x80
};

static char hx8394d_qhd_video_on_cmd15[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd hx8394d_qhd_video_on_command[] = {
	{ 0x8 , hx8394d_qhd_video_on_cmd0, 0x00},
	{ 0x8 , hx8394d_qhd_video_on_cmd1, 0x00},
	{ 0x14 , hx8394d_qhd_video_on_cmd2, 0x00},
	{ 0x10 , hx8394d_qhd_video_on_cmd3, 0x00},
	{ 0x14 , hx8394d_qhd_video_on_cmd4, 0x00},
	{ 0x8 , hx8394d_qhd_video_on_cmd5, 0x00},
	{ 0x8 , hx8394d_qhd_video_on_cmd6, 0x00},
	{ 0x24 , hx8394d_qhd_video_on_cmd7, 0x00},
	{ 0x34 , hx8394d_qhd_video_on_cmd8, 0x00},
	{ 0x34 , hx8394d_qhd_video_on_cmd9, 0x00},
	{ 0x8 , hx8394d_qhd_video_on_cmd10, 0x00},
	{ 0x8 , hx8394d_qhd_video_on_cmd11, 0x00},
	{ 0xc , hx8394d_qhd_video_on_cmd12, 0x00},
	{ 0x8 , hx8394d_qhd_video_on_cmd13, 0x00},
	{ 0x4 , hx8394d_qhd_video_on_cmd14, 0xc8},
	{ 0x4 , hx8394d_qhd_video_on_cmd15, 0x0a}
};
#define HX8394D_QHD_VIDEO_ON_COMMAND 16

static struct command_state hx8394d_qhd_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info hx8394d_qhd_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info hx8394d_qhd_video_video_panel = {
	1, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration hx8394d_qhd_video_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t hx8394d_qhd_video_timings[] = {
	0x79, 0x1a, 0x12, 0x00, 0x3e, 0x42, 0x16, 0x1e, 0x15, 0x03, 0x04, 0x00
};

static struct mipi_dsi_cmd hx8394d_qhd_video_rotation[] = {

};
#define HX8394D_QHD_VIDEO_ROTATION 0


static struct panel_timing hx8394d_qhd_video_timing_info = {
	0, 4, 0x04, 0x1b
};

static struct panel_reset_sequence hx8394d_qhd_video_panel_reset_seq = {
{
	1, 0, 1, }, { 20, 20, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight hx8394d_qhd_video_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#define HX8394D_QHD_VIDEO_SIGNATURE 0xFFFF

#endif /*_HX8394D_QHD_VIDEO_H_*/
