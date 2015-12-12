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

#ifndef _PANEL_AUO_QVGA_CMD_H_
#define _PANEL_AUO_QVGA_CMD_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config auo_qvga_cmd_panel_data = {
	"qcom,mdss_dsi_auo_qvga_cmd", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 1, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ""
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution auo_qvga_cmd_panel_res = {
	320, 320, 4, 4, 4, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info auo_qvga_cmd_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char auo_qvga_cmd_on_cmd0[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xF0, 0x55, 0xAA, 0x52,
	0x08, 0x00, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd1[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xBD, 0x01, 0x90, 0x14,
	0x14, 0x00, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd2[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xBE, 0x01, 0x90, 0x14,
	0x14, 0x01, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd3[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xBF, 0x01, 0x90, 0x14,
	0x14, 0x00, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd4[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xBB, 0x07, 0x07, 0x07,
};

static char auo_qvga_cmd_on_cmd5[] = {
	0x02, 0x00, 0x39, 0xC0,
	0xC7, 0x40, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd6[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xF0, 0x55, 0xAA, 0x52,
	0x08, 0x02, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd7[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xFE, 0x08, 0x50, 0xFF,
};

static char auo_qvga_cmd_on_cmd8[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xC3, 0xF2, 0x95, 0x04,
};

static char auo_qvga_cmd_on_cmd9[] = {
	0x04, 0x00, 0x15, 0x80,
};

static char auo_qvga_cmd_on_cmd10[] = {
	0x06, 0x00, 0x39, 0xC0,
	0xF0, 0x55, 0xAA, 0x52,
	0x08, 0x01, 0xFF, 0xFF,
};

static char auo_qvga_cmd_on_cmd11[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB0, 0x03, 0x03, 0x03,
};

static char auo_qvga_cmd_on_cmd12[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB1, 0x05, 0x05, 0x05,
};

static char auo_qvga_cmd_on_cmd13[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB2, 0x01, 0x01, 0x01,
};

static char auo_qvga_cmd_on_cmd14[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB4, 0x07, 0x07, 0x07,
};

static char auo_qvga_cmd_on_cmd15[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB5, 0x03, 0x03, 0x03,
};

static char auo_qvga_cmd_on_cmd16[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB6, 0x53, 0x53, 0x53,
};

static char auo_qvga_cmd_on_cmd17[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB7, 0x33, 0x33, 0x33,
};

static char auo_qvga_cmd_on_cmd18[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB8, 0x23, 0x23, 0x23,
};

static char auo_qvga_cmd_on_cmd19[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xB9, 0x03, 0x03, 0x03,
};

static char auo_qvga_cmd_on_cmd20[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xBA, 0x03, 0x03, 0x03,
};

static char auo_qvga_cmd_on_cmd21[] = {
	0x04, 0x00, 0x39, 0xC0,
	0xBE, 0x32, 0x30, 0x70,
};

static char auo_qvga_cmd_on_cmd22[] = {
	0x08, 0x00, 0x39, 0xC0,
	0xCF, 0xFF, 0xD4, 0x95,
	0xEF, 0x4F, 0x00, 0x04,
};

static char auo_qvga_cmd_on_cmd23[] = {
	0x35, 0x01, 0x15, 0x80,
};

static char auo_qvga_cmd_on_cmd24[] = {
	0x36, 0x01, 0x15, 0x80,
};

static char auo_qvga_cmd_on_cmd25[] = {
	0xC0, 0x20, 0x15, 0x80,
};

static char auo_qvga_cmd_on_cmd26[] = {
	0x07, 0x00, 0x39, 0xC0,
	0xC2, 0x17, 0x17, 0x17,
	0x17, 0x17, 0x0B, 0xFF,
};

static char auo_qvga_cmd_on_cmd27[] = {
	0x00, 0x00, 0x32, 0x80,
};

static char auo_qvga_cmd_on_cmd28[] = {
	0x11, 0x00, 0x05, 0x80,
};

static char auo_qvga_cmd_on_cmd29[] = {
	0x29, 0x00, 0x05, 0x80,
};

static struct mipi_dsi_cmd auo_qvga_cmd_on_command[] = {
	{0xc, auo_qvga_cmd_on_cmd0, 0x00},
	{0xc, auo_qvga_cmd_on_cmd1, 0x00},
	{0xc, auo_qvga_cmd_on_cmd2, 0x00},
	{0xc, auo_qvga_cmd_on_cmd3, 0x10},
	{0x8, auo_qvga_cmd_on_cmd4, 0x00},
	{0x8, auo_qvga_cmd_on_cmd5, 0x00},
	{0xc, auo_qvga_cmd_on_cmd6, 0x00},
	{0x8, auo_qvga_cmd_on_cmd7, 0x00},
	{0x8, auo_qvga_cmd_on_cmd8, 0x00},
	{0x4, auo_qvga_cmd_on_cmd9, 0x00},
	{0xc, auo_qvga_cmd_on_cmd10, 0x00},
	{0x8, auo_qvga_cmd_on_cmd11, 0x00},
	{0x8, auo_qvga_cmd_on_cmd12, 0x00},
	{0x8, auo_qvga_cmd_on_cmd13, 0x00},
	{0x8, auo_qvga_cmd_on_cmd14, 0x00},
	{0x8, auo_qvga_cmd_on_cmd15, 0x00},
	{0x8, auo_qvga_cmd_on_cmd16, 0x00},
	{0x8, auo_qvga_cmd_on_cmd17, 0x00},
	{0x8, auo_qvga_cmd_on_cmd18, 0x00},
	{0x8, auo_qvga_cmd_on_cmd19, 0x00},
	{0x8, auo_qvga_cmd_on_cmd20, 0x00},
	{0x8, auo_qvga_cmd_on_cmd21, 0x00},
	{0xc, auo_qvga_cmd_on_cmd22, 0x00},
	{0x4, auo_qvga_cmd_on_cmd23, 0x00},
	{0x4, auo_qvga_cmd_on_cmd24, 0x00},
	{0x4, auo_qvga_cmd_on_cmd25, 0x00},
	{0xc, auo_qvga_cmd_on_cmd26, 0x00},
	{0x4, auo_qvga_cmd_on_cmd27, 0x00},
	{0x4, auo_qvga_cmd_on_cmd28, 0xFF},
	{0x4, auo_qvga_cmd_on_cmd29, 0x00}
};

#define auo_QVGA_CMD_ON_COMMAND 30

static char auo_qvga_cmdoff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char auo_qvga_cmdoff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd auo_qvga_cmd_off_command[] = {
	{0x4, auo_qvga_cmdoff_cmd0, 0x32},
	{0x4, auo_qvga_cmdoff_cmd1, 0x78}
};

#define auo_QVGA_CMD_OFF_COMMAND 2

static struct command_state auo_qvga_cmd_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info auo_qvga_cmd_command_panel = {
	1, 1, 1, 0, 0, 0x2c, 0, 0, 0, 1, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info auo_qvga_cmd_video_panel = {
	1, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration auo_qvga_cmd_lane_config = {
	1, 0, 1, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t auo_qvga_cmd_timings[] = {
	0x5F, 0x12, 0x0A, 0x00, 0x32, 0x34, 0x10, 0x16, 0x0F, 0x03, 0x04, 0x00
};

static struct panel_timing auo_qvga_cmd_timing_info = {
	0, 4, 0x05, 0x15
};

static struct panel_reset_sequence auo_qvga_cmd_panel_reset_seq = {
	{ 1, 0, 1, }, { 20, 20, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight auo_qvga_cmd_backlight = {
	2, 1, 255, 100, 1, "PMIC_8941"
};

#endif /*_PANEL_AUO_QVGA_CMD_H_*/
