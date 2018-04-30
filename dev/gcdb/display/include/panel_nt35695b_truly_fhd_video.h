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

#ifndef _PANEL_NT35695B_TRULY_FHD_VIDEO_H_

#define _PANEL_NT35695B_TRULY_FHD_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config nt35695b_truly_fhd_video_panel_data = {
  "qcom,mdss_dsi_nt35695b_truly_fhd_video", "dsi:0:", "qcom,mdss-dsi-panel",
  10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution nt35695b_truly_fhd_video_panel_res = {
  1080, 1920, 120, 60, 12, 0, 12, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info nt35695b_truly_fhd_video_color = {
  24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char nt35695b_truly_fhd_video_on_cmd0[] = {
	0xFF, 0x20, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd1[] = {
	0XFB, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd2[] = {
	0x00, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd3[] = {
	0x01, 0x55, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd4[] = {
	0x02, 0x45, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd5[] = {
	0x03, 0x55, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd6[] = {
	0x05, 0x50, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd7[] = {
	0x06, 0xa8, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd8[] = {
	0x07, 0xAD, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd9[] = {
	0x08, 0x0C, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd10[] = {
	0x0B, 0xAA, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd11[] = {
	0x0C, 0xAA, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd12[] = {
	0x0E, 0xB0, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd13[] = {
	0x0F, 0xB3, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd14[] = {
	0x11, 0x28, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd15[] = {
	0x12, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd16[] = {
	0x13, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd17[] = {
	0x14, 0x4A, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd18[] = {
	0x15, 0x12, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd19[] = {
	0x16, 0x12, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd20[] = {
	0x30, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd21[] = {
	0x72, 0x11, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd22[] = {
	0x58, 0x82, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd23[] = {
	0x59, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd24[] = {
	0x5A, 0x02, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd25[] = {
	0x5B, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd26[] = {
	0x5C, 0x82, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd27[] = {
	0x5D, 0x80, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd28[] = {
	0x5E, 0x02, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd29[] = {
	0x5F, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd30[] = {
	0xFF, 0x24, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd31[] = {
	0xFB, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd32[] = {
	0x00, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd33[] = {
	0x01, 0x0B, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd34[] = {
	0x02, 0x0C, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd35[] = {
	0x03, 0x89, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd36[] = {
	0x04, 0x8A, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd37[] = {
	0x05, 0x0F, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd38[] = {
	0x06, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd39[] = {
	0x07, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd40[] = {
	0x08, 0x1C, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd41[] = {
	0X09, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd42[] = {
	0x0A, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd43[] = {
	0x0B, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd44[] = {
	0x0C, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd45[] = {
	0x0D, 0x13, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd46[] = {
	0x0E, 0x15, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd47[] = {
	0x0F, 0x17, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd48[] = {
	0x10, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd49[] = {
	0x11, 0x0B, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd50[] = {
	0x12, 0x0C, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd51[] = {
	0X13, 0x89, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd52[] = {
	0x14, 0x8A, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd53[] = {
	0x15, 0x0F, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd54[] = {
	0x16, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd55[] = {
	0x17, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd56[] = {
	0x18, 0x1C, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd57[] = {
	0x19, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd58[] = {
	0x1A, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd59[] = {
	0x1B, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd60[] = {
	0x1C, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd61[] = {
	0X1D, 0x13, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd62[] = {
	0x1E, 0x15, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd63[] = {
	0x1F, 0x17, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd64[] = {
	0x20, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd65[] = {
	0x21, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd66[] = {
	0x22, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd67[] = {
	0x23, 0x40, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd68[] = {
	0x24, 0x40, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd69[] = {
	0x25, 0x6D, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd70[] = {
	0x26, 0x40, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd71[] = {
	0X27, 0x40, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd72[] = {
	0x29, 0xD8, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd73[] = {
	0x2A, 0x2A, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd74[] = {
	0x4B, 0x03, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd75[] = {
	0x4C, 0x11, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd76[] = {
	0x4D, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd77[] = {
	0x4E, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd78[] = {
	0x4F, 0x01, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd79[] = {
	0x50, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd80[] = {
	0x51, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd81[] = {
	0X52, 0x80, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd82[] = {
	0x53, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd83[] = {
	0x54, 0x07, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd84[] = {
	0x55, 0x25, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd85[] = {
	0x56, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd86[] = {
	0x58, 0x07, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd87[] = {
	0x5b, 0x43, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd88[] = {
	0x5c, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd89[] = {
	0x5F, 0x73, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd90[] = {
	0x60, 0x73, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd91[] = {
	0x63, 0x22, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd92[] = {
	0x64, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd93[] = {
	0x67, 0x08, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd94[] = {
	0x68, 0x04, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd95[] = {
	0x7A, 0x80, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd96[] = {
	0x7B, 0x91, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd97[] = {
	0x7C, 0xD8, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd98[] = {
	0x7D, 0x60, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd99[] = {
	0x93, 0x06, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd100[] = {
	0x94, 0x06, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd101[] = {
	0X8A, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd102[] = {
	0x9B, 0x0F, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd103[] = {
	0xB3, 0xC0, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd104[] = {
	0xB4, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd105[] = {
	0xB5, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd106[] = {
	0xB6, 0x21, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd107[] = {
	0xB7, 0x22, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd108[] = {
	0xB8, 0x07, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd109[] = {
	0xB9, 0x07, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd110[] = {
	0xBA, 0x22, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd111[] = {
	0XBD, 0x20, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd112[] = {
	0xBE, 0x07, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd113[] = {
	0xBF, 0x07, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd114[] = {
	0xC1, 0x6D, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd115[] = {
	0xC4, 0x24, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd116[] = {
	0xE3, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd117[] = {
	0xEC, 0x00, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd118[] = {
	0xFF, 0x10, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd119[] = {
	0xBB, 0x03, 0x15, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd120[] = {
	0x11, 0x00, 0x05, 0x80,
};

static char nt35695b_truly_fhd_video_on_cmd121[] = {
	0x29, 0x00, 0x05, 0x80,
};

static struct mipi_dsi_cmd nt35695b_truly_fhd_video_on_command[] = {
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd0, 0x10},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd1, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd2, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd3, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd4, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd5, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd6, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd7, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd8, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd9, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd10, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd11, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd12, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd13, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd14, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd15, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd16, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd17, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd18, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd19, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd20, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd21, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd22, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd23, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd24, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd25, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd26, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd27, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd28, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd29, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd30, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd31, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd32, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd33, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd34, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd35, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd36, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd37, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd38, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd39, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd40, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd41, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd42, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd43, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd44, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd45, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd46, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd47, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd48, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd49, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd50, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd51, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd52, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd53, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd54, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd55, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd56, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd57, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd58, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd59, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd60, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd61, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd62, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd63, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd64, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd65, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd66, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd67, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd68, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd69, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd70, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd71, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd72, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd73, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd74, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd75, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd76, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd77, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd78, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd79, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd80, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd81, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd82, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd83, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd84, 0x00},
	{ 0x4 ,	nt35695b_truly_fhd_video_on_cmd85, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd86, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd87, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd88, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd89, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd90, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd91, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd92, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd93, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd94, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd95, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd96, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd97, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd98, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd99, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd100, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd101, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd102, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd103, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd104, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd105, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd106, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd107, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd108, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd109, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd110, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd111, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd112, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd113, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd114, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd115, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd116, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd117, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd118, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd119, 0x00},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd120, 0x78},
	{ 0x4 , nt35695b_truly_fhd_video_on_cmd121, 0x78}
};
#define NT35695B_TRULY_FHD_VIDEO_ON_COMMAND 122

static char nt35695b_truly_fhd_video_off_cmd0[] = {
	0x28, 0x00, 0x05, 0x80,
};

static char nt35695b_truly_fhd_video_off_cmd1[] = {
	0x10, 0x00, 0x05, 0x80,
};

static struct mipi_dsi_cmd nt35695b_truly_fhd_video_off_command[] = {
	{ 0x4 , nt35695b_truly_fhd_video_off_cmd0, 0x14},
	{ 0x4 , nt35695b_truly_fhd_video_off_cmd1, 0x78}
};
#define NT35695B_TRULY_FHD_VIDEO_OFF_COMMAND 2


static struct command_state nt35695b_truly_fhd_video_state = {
  0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info nt35695b_truly_fhd_video_command_panel = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info nt35695b_truly_fhd_video_video_panel = {
  0, 0, 0, 0, 1, 1, 2, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration nt35695b_truly_fhd_video_lane_config = {
  4, 0, 1, 1, 1, 1, 0
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t nt35695b_truly_fhd_video_timings[] = {
  0xE6, 0x38, 0x26, 0x00, 0x68, 0x6e, 0x2A, 0x3c, 0x44, 0x03, 0x04, 0x00
};

const uint32_t nt35695b_truly_fhd_video_12nm_timings[] = {
  0x17, 0x0A, 0x0F, 0x06, 0x03, 0x08, 0x06, 0x0E
};

static const uint32_t nt35695b_truly_fhd_14nm_video_timings[] = {
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1e, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x23, 0x1a, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
};

static struct panel_timing nt35695b_truly_fhd_video_timing_info = {
  0, 4, 0x02, 0x2d
};

static struct labibb_desc nt35695b_truly_fhd_video_labibb = {
	0, 1, 5700000, 5700000, 5700000, 5700000, 3, 3, 1, 0
};

static struct panel_reset_sequence nt35695b_truly_fhd_video_panel_reset_seq = {
{ 1, 0, 1, }, { 10, 10, 10, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight nt35695b_truly_fhd_video_backlight = {
  0, 1, 4095, 100, 1, "PMIC_8941"
};

#define NT35695B_TRULY_FHD_VIDEO_SIGNATURE 0xFFFF

#endif /*_NT35695B_TRULY_FHD_VIDEO_H_*/
