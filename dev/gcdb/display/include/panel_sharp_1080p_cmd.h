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

#ifndef _PANEL_SHARP_1080P_CMD_H_
#define _PANEL_SHARP_1080P_CMD_H_

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config sharp_1080p_cmd_panel_data = {
	"qcom,mdss_dsi_sharp_1080p_cmd", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 1, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution sharp_1080p_cmd_panel_res = {
	1080, 1920, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info sharp_1080p_cmd_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char sharp_1080p_cmd_on_cmd0[] = {
	0xbb, 0x10, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd1[] = {
	0xb0, 0x03, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd2[] = {
	0x11, 0x00, 0x05, 0x80
};

static char sharp_1080p_cmd_on_cmd3[] = {
	0x51, 0xff, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd4[] = {
	0x53, 0x24, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd5[] = {
	0xff, 0x23, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd6[] = {
	0x08, 0x05, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd7[] = {
	0x46, 0x90, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd8[] = {
	0xff, 0x10, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd9[] = {
	0xff, 0xf0, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd10[] = {
	0x92, 0x01, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd11[] = {
	0xff, 0x10, 0x15, 0x80
};

static char sharp_1080p_cmd_on_cmd12[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd sharp_1080p_cmd_on_command[] = {
	{0x4 , sharp_1080p_cmd_on_cmd0, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd1, 0x00},
	{0x3 , sharp_1080p_cmd_on_cmd2, 0x78},
	{0x4 , sharp_1080p_cmd_on_cmd3, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd4, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd5, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd6, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd7, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd8, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd9, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd10, 0x00},
	{0x4 , sharp_1080p_cmd_on_cmd11, 0x00},
	{0x3 , sharp_1080p_cmd_on_cmd12, 0x28}
};

#define SHARP_1080P_CMD_ON_COMMAND 13


static char sharp_1080p_cmdoff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char sharp_1080p_cmdoff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd sharp_1080p_cmd_off_command[] = {
	{0x4 , sharp_1080p_cmdoff_cmd0, 0},
	{0x4 , sharp_1080p_cmdoff_cmd1, 0}
};

#define SHARP_1080P_CMD_OFF_COMMAND 2


static struct command_state sharp_1080p_cmd_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info sharp_1080p_cmd_command_panel = {
	1, 1, 1, 0, 0, 0x2c, 0x3c, 0, 0, 1, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info sharp_1080p_cmd_video_panel = {
	0, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration sharp_1080p_cmd_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t sharp_1080p_cmd_timings[] = {
	0xe7, 0x36, 0x24, 0x00, 0x66, 0x6a, 0x2a, 0x3a,  0x2d, 0x03, 0x04, 0x00
};

static struct panel_timing sharp_1080p_cmd_timing_info = {
	0x0, 0x04, 0x04, 0x1b
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence sharp_1080p_cmd_panel_reset_seq = {
	{ 1, 0, 1, }, { 10, 10, 10, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/
static struct backlight sharp_1080p_cmd_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#endif /*_PANEL_sharp_1080P_CMD_H_*/
