/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_AUO_390P_CMD_H_

#define _PANEL_AUO_390P_CMD_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config auo_390p_cmd_panel_data = {
	"qcom,mdss_dsi_auo_390p_cmd", "dsi:0:", "qcom,mdss-dsi-panel",
	11, 1, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution auo_390p_cmd_panel_res = {
	390, 390, 4, 4, 4, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info auo_390p_cmd_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char auo_390p_cmd_on_cmd0[] = {
	0xfe, 0x01, 0x015, 0x80,
};

static char auo_390p_cmd_on_cmd1[] = {
	0x0a, 0xf0, 0x015, 0x80,
};

static char auo_390p_cmd_on_cmd2[] = {
	0xfe, 0x00, 0x15, 0x80,
};


static char auo_390p_cmd_on_cmd3[] = {
	0x35, 0x00, 0x15, 0x80,
};


static char auo_390p_cmd_on_cmd4[] = {
	0x05, 0x00, 0x29, 0xC0,
	0x2a, 0x00, 0x04, 0x01,
	0x89, 0xFF, 0xFF, 0xFF,
};


static char auo_390p_cmd_on_cmd5[] = {
	0x05, 0x00, 0x29, 0xC0,
	0x2b, 0x00, 0x00, 0x01,
	0x85, 0xFF, 0xFF, 0xFF,
};


static char auo_390p_cmd_on_cmd6[] = {
	0x05, 0x00, 0x29, 0xC0,
	0x30, 0x00, 0x00, 0x01,
	0x85, 0xFF, 0xFF, 0xFF,
};


static char auo_390p_cmd_on_cmd7[] = {
	0x05, 0x00, 0x29, 0xC0,
	0x31, 0x00, 0x04, 0x01,
	0x89, 0xFF, 0xFF, 0xFF,
};


static char auo_390p_cmd_on_cmd8[] = {
	0x12, 0x00, 0x05, 0x80,
};


static char auo_390p_cmd_on_cmd9[] = {
	0x53, 0x20, 0x015, 0x80,
};


static char auo_390p_cmd_on_cmd10[] = {
	0x11, 0x00, 0x05, 0x80,
};


static char auo_390p_cmd_on_cmd11[] = {
	0x29, 0x00, 0x05, 0x80,
};


static char auo_390p_cmd_on_cmd12[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xf0, 0x55, 0xaa, 0x52,
	0x08, 0x01, 0xff, 0xff,
};


static char auo_390p_cmd_on_cmd13[] = {
	0x07, 0x00, 0x39, 0xC0,
	0xff, 0x00, 0x55, 0xaa,
	0x52, 0x08, 0x01, 0xff,
};


static struct mipi_dsi_cmd auo_390p_cmd_on_command[] = {
	{ 0x4 , auo_390p_cmd_on_cmd0, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd1, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd2, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd3, 0x00},
	{ 0xc , auo_390p_cmd_on_cmd4, 0x00},
	{ 0xc , auo_390p_cmd_on_cmd5, 0x78},
	{ 0xc , auo_390p_cmd_on_cmd6, 0x00},
	{ 0xc , auo_390p_cmd_on_cmd7, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd8, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd9, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd10, 0x00},
	{ 0x4 , auo_390p_cmd_on_cmd11, 0x00},
	{ 0xc , auo_390p_cmd_on_cmd12, 0x00},
	{ 0xc , auo_390p_cmd_on_cmd13, 0x00},
};

#define AUO_390P_CMD_ON_COMMAND 14


static char auo_390p_cmd_off_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};


static char auo_390p_cmd_off_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};


static struct mipi_dsi_cmd auo_390p_cmd_off_command[] = {
	{ 0x4 , auo_390p_cmd_off_cmd0, 0x0},
	{ 0x4 , auo_390p_cmd_off_cmd1, 0x0}
};

#define AUO_390P_CMD_OFF_COMMAND 2


static struct command_state auo_390p_cmd_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info auo_390p_cmd_command_panel = {
	0x1, 0x1, 0x1, 0, 0, 0, 0, 0, 0, 0x1, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info auo_390p_cmd_video_panel = {
	0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration auo_390p_cmd_lane_config = {
	1, 0, 1, 0, 0, 0, 0
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t auo_390p_cmd_timings[] = {
	0x5F, 0x12, 0x0A, 0x00, 0x32, 0x34, 0x10, 0x16, 0x0F, 0x03, 0x04, 0x00
};

static struct panel_timing auo_390p_cmd_timing_info = {
	0, 4, 0x05, 0x11
};

static const uint32_t auo_390p_14nm_cmd_timings[] = {
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x18, 0x7, 0x8, 0x5, 0x3, 0x4, 0xa0,
};

static struct panel_reset_sequence auo_390p_cmd_panel_reset_seq = {
	{ 1, 0, 1, }, { 200, 200, 200, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight auo_390p_cmd_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#define AUO_390P_CMD_SIGNATURE 0xFFFF

#endif /*_AUO_390P_CMD_H_*/
