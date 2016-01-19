/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_TRULY_1080P_CMD_H_

#define _PANEL_TRULY_1080P_CMD_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config truly_1080p_cmd_panel_data = {
	"qcom,mdss_dsi_truly_1080p_cmd", "dsi:0:", "qcom,mdss-dsi-panel",
	11, 1, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution truly_1080p_cmd_panel_res = {
	1080, 1920, 96, 64, 16, 0, 4, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info truly_1080p_cmd_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char truly_1080p_cmd_on_cmd0[] = {
	0xd6, 0x01, 0x023, 0x80,
};

static char truly_1080p_cmd_on_cmd1[] = {
	0x35, 0x00, 0x015, 0x80,
};

static char truly_1080p_cmd_on_cmd2[] = {
	0x51, 0xff, 0x15, 0x80,
};


static char truly_1080p_cmd_on_cmd3[] = {
	0x53, 0x2c, 0x15, 0x80,
};


static char truly_1080p_cmd_on_cmd4[] = {
	0x55, 0x00, 0x15, 0x80,
};


static char truly_1080p_cmd_on_cmd5[] = {
	0x11, 0x00, 0x05, 0x80,
};


static char truly_1080p_cmd_on_cmd6[] = {
	0xb0, 0x04, 0x23, 0x80,
};


static char truly_1080p_cmd_on_cmd7[] = {
	0x07, 0x00, 0x29, 0xC0,
	0xb3, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF,
};


static char truly_1080p_cmd_on_cmd8[] = {
	0x03, 0x00, 0x29, 0xC0,
	0xb6, 0x3a, 0xd3, 0xFF,
};


static char truly_1080p_cmd_on_cmd9[] = {
	0x03, 0x00, 0x29, 0xc0,
	0xc0, 0x00, 0x00, 0xff,
};


static char truly_1080p_cmd_on_cmd10[] = {
	0x23, 0x00, 0x29, 0xC0,
	0xc1, 0x84, 0x60, 0x10,
	0xeb, 0xff, 0x6f, 0xce,
	0xff, 0xff, 0x17, 0x02,
	0x58, 0x73, 0xae, 0xb1,
	0x20, 0xc6, 0xff, 0xff,
	0x1f, 0xf3, 0xff, 0x5f,
	0x10, 0x10, 0x10, 0x10,
	0x00, 0x02, 0x01, 0x22,
	0x22, 0x00, 0x01, 0xff,
};


static char truly_1080p_cmd_on_cmd11[] = {
	0x08, 0x00, 0x29, 0xC0,
	0xc2, 0x31, 0xf7, 0x80,
	0x06, 0x08, 0x00, 0x00,
};


static char truly_1080p_cmd_on_cmd12[] = {
	0x17, 0x00, 0x29, 0xC0,
	0xc4, 0x70, 0x00, 0x00,
	0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x0c, 0x06,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x04, 0x00, 0x00,
	0x00, 0x0c, 0x06, 0xff,
};


static char truly_1080p_cmd_on_cmd13[] = {
	0x29, 0x00, 0x29, 0xC0,
	0xc6, 0x78, 0x69, 0x00,
	0x69, 0x00, 0x69, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x69, 0x00, 0x69, 0x00,
	0x69, 0x10, 0x19, 0x07,
	0x00, 0x78, 0x00, 0x69,
	0x00, 0x69, 0x00, 0x69,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x69, 0x00, 0x69,
	0x00, 0x69, 0x10, 0x19,
	0x07, 0xff, 0xff, 0xff,
};


static char truly_1080p_cmd_on_cmd14[] = {
	0x0a, 0x00, 0x29, 0xC0,
	0xcb, 0x31, 0xfc, 0x3f,
	0x8c, 0x00, 0x00, 0x00,
	0x00, 0xc0, 0xff, 0xff,
};


static char truly_1080p_cmd_on_cmd15[] = {
	0xcc, 0x0b, 0x23, 0x80,
};


static char truly_1080p_cmd_on_cmd16[] = {
	0x0b, 0x00, 0x29, 0xc0,
	0xd0, 0x11, 0x81, 0xbb,
	0x1e, 0x1e, 0x4c, 0x19,
	0x19, 0x0c, 0x00, 0xff,
};


static char truly_1080p_cmd_on_cmd17[] = {
	0x1a, 0x00, 0x29, 0xC0,
	0xd3, 0x1b, 0x33, 0xbb,
	0xbb, 0xb3, 0x33, 0x33,
	0x33, 0x00, 0x01, 0x00,
	0xa0, 0xd8, 0xa0, 0x0d,
	0x4e, 0x4e, 0x33, 0x3b,
	0x22, 0x72, 0x07, 0x3d,
	0xbf, 0x33, 0xff, 0xff,
};


static char truly_1080p_cmd_on_cmd18[] = {
	0x08, 0x00, 0x29, 0xc0,
	0xd5, 0x06, 0x00, 0x00,
	0x01, 0x51, 0x01, 0x32,
};


static char truly_1080p_cmd_on_cmd19[] = {
	0x1f, 0x00, 0x29, 0xC0,
	0xc7, 0x01, 0x0a, 0x11,
	0x18, 0x26, 0x33, 0x3e,
	0x50, 0x38, 0x42, 0x52,
	0x60, 0x67, 0x6e, 0x77,
	0x01, 0x0a, 0x11, 0x18,
	0x26, 0x33, 0x3e, 0x50,
	0x38, 0x42, 0x52, 0x60,
	0x67, 0x6e, 0x77, 0xff,
};


static char truly_1080p_cmd_on_cmd20[] = {
	0x14, 0x00, 0x29, 0xC0,
	0xc8, 0x01, 0x00, 0x00,
	0x00, 0x00, 0xfc, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xfc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfc, 0x00,
};


static char truly_1080p_cmd_on_cmd21[] = {
	0x29, 0x00, 0x05, 0x80,
};


static struct mipi_dsi_cmd truly_1080p_cmd_on_command[] = {
	{ 0x4 , truly_1080p_cmd_on_cmd0, 0x00},
	{ 0x4 , truly_1080p_cmd_on_cmd1, 0x00},
	{ 0x4 , truly_1080p_cmd_on_cmd2, 0x00},
	{ 0x4 , truly_1080p_cmd_on_cmd3, 0x00},
	{ 0x4 , truly_1080p_cmd_on_cmd4, 0x00},
	{ 0x4 , truly_1080p_cmd_on_cmd5, 0x78},
	{ 0x4 , truly_1080p_cmd_on_cmd6, 0x00},
	{ 0xc , truly_1080p_cmd_on_cmd7, 0x00},
	{ 0x8 , truly_1080p_cmd_on_cmd8, 0x00},
	{ 0x8 , truly_1080p_cmd_on_cmd9, 0x00},
	{ 0x28 , truly_1080p_cmd_on_cmd10, 0x00},
	{ 0xc , truly_1080p_cmd_on_cmd11, 0x00},
	{ 0x1c , truly_1080p_cmd_on_cmd12, 0x00},
	{ 0x30 , truly_1080p_cmd_on_cmd13, 0x00},
	{ 0x10 , truly_1080p_cmd_on_cmd14, 0x00},
	{ 0x4 , truly_1080p_cmd_on_cmd15, 0x00},
	{ 0x10 , truly_1080p_cmd_on_cmd16, 0x00},
	{ 0x20 , truly_1080p_cmd_on_cmd17, 0x00},
	{ 0xc , truly_1080p_cmd_on_cmd18, 0x00},
	{ 0x24 , truly_1080p_cmd_on_cmd19, 0x00},
	{ 0x18 , truly_1080p_cmd_on_cmd20, 0xc8},
	{ 0x4 , truly_1080p_cmd_on_cmd21, 0x14},
};

#define TRULY_1080P_CMD_ON_COMMAND 22


static char truly_1080p_cmd_off_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};


static char truly_1080p_cmd_off_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};


static struct mipi_dsi_cmd truly_1080p_cmd_off_command[] = {
	{ 0x4 , truly_1080p_cmd_off_cmd0, 0x0},
	{ 0x4 , truly_1080p_cmd_off_cmd1, 0x0}
};

#define TRULY_1080P_CMD_OFF_COMMAND 2


static struct command_state truly_1080p_cmd_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info truly_1080p_cmd_command_panel = {
	0x1, 0x1, 0x1, 0, 0, 0, 0, 0, 0, 0x1, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info truly_1080p_cmd_video_panel = {
	0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration truly_1080p_cmd_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t truly_1080p_cmd_timings[] = {
	0xE6, 0x38, 0x26, 0x00, 0x68, 0x6e, 0x2A, 0x3c, 0x44, 0x03, 0x04, 0x00
};

static struct panel_timing truly_1080p_cmd_timing_info = {
	0, 4, 0x02, 0x2d
};

static const uint32_t truly_1080p_14nm_cmd_timings[] = {
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x18, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
};

static struct panel_reset_sequence truly_1080p_cmd_panel_reset_seq = {
	{ 1, 0, 1, }, { 200, 200, 200, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight truly_1080p_cmd_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#define TRULY_1080P_CMD_SIGNATURE 0xFFFF

#endif /*_TRULY_1080P_CMD_H_*/
