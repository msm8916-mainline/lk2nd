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

#ifndef _PANEL_BYD_1200P_VIDEO_H_

#define _PANEL_BYD_1200P_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config byd_1200p_video_panel_data = {
	"qcom,mdss_dsi_byd_1200p_video", "dsi:0:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution byd_1200p_video_panel_res = {
	1200, 1920, 96, 64, 16, 0, 4, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info byd_1200p_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char byd_1200p_video_on_cmd0  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0x8C, 0x8E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd1  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0xC5, 0x23, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd2  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0xC7, 0x23, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd3  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0xFD, 0x5C, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd4  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0xFA, 0x14, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd5  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0X83, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd6  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0X84, 0X11, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd7  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC0, 0X0E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd8  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC1, 0X12, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd9  [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC2, 0X25, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd10 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC3, 0X34, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd11 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC4, 0X3F, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd12 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC5, 0X49, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd13 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC6, 0X52, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd14 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC7, 0X59, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd15 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC8, 0X60, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd16 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC9, 0XC8, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd17 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCA, 0XC7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd18 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCB, 0XD6, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd19 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCC, 0XD9, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd20 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCD, 0XD7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd21 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCE, 0XD1, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd22 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCF, 0XD3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd23 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD0, 0XD4, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd24 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD1, 0XFF, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd25 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD2, 0X08, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd26 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD3, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd27 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD4, 0X3B, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd28 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD5, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd29 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD6, 0XB3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd30 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD7, 0XBC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd31 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD8, 0XC5, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd32 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD9, 0XD0, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd33 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDA, 0XDB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd34 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDB, 0XE7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd35 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDC, 0XF7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd36 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDD, 0XFE, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd37 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDE, 0X00, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd38 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDF, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd39 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE0, 0X0E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd40 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE1, 0X12, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd41 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE2, 0X25, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd42 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE3, 0X34, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd43 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE4, 0X3F, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd44 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE5, 0X49, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd45 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE6, 0X52, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd46 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE7, 0X59, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd47 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE8, 0X60, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd48 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE9, 0XC8, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd49 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEA, 0XC7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd50 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEB, 0XD6, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd51 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEC, 0XD9, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd52 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XED, 0XD7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd53 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEE, 0XD1, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd54 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEF, 0XD3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd55 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF0, 0XD4, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd56 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF1, 0XFF, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd57 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF2, 0X08, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd58 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF3, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd59 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF4, 0X3B, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd60 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF5, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd61 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF6, 0XB3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd62 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF7, 0XBC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd63 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF8, 0XC5, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd64 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF9, 0XD0, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd65 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFA, 0XDB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd66 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFB, 0XE7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd67 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFC, 0XF7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd68 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFD, 0XFE, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd69 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFE, 0X00, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd70 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFF, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd71 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XA9, 0X4b, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd72 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0X83, 0XBB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd73 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0X84, 0X22, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd74 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC0, 0X0E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd75 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC1, 0X12, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd76 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC2, 0X25, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd77 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC3, 0X34, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd78 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC4, 0X3F, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd79 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC5, 0X49, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd80 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC6, 0X52, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd81 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC7, 0X59, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd82 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC8, 0X60, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd83 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC9, 0XC8, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd84 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCA, 0XC7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd85 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCB, 0XD6, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd86 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCC, 0XD9, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd87 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCD, 0XD7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd88 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCE, 0XD1, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd89 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCF, 0XD3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd90 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD0, 0XD4, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd91 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD1, 0XFF, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd92 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD2, 0X08, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd93 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD3, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd94 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD4, 0X3B, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd95 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD5, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd96 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD6, 0XB3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd97 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD7, 0XBC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd98 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD8, 0XC5, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd99 [] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD9, 0XD0, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd100[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDA, 0XDB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd101[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDB, 0XE7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd102[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDC, 0XF7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd103[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDD, 0XFE, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd104[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDE, 0X00, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd105[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDF, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd106[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE0, 0X0E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd107[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE1, 0X12, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd108[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE2, 0X25, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd109[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE3, 0X34, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd110[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE4, 0X3F, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd111[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE5, 0X49, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd112[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE6, 0X52, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd113[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE7, 0X59, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd114[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE8, 0X60, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd115[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE9, 0XC8, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd116[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEA, 0XC7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd117[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEB, 0XD6, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd118[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEC, 0XD9, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd119[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XED, 0XD7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd120[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEE, 0XD1, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd121[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEF, 0XD3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd122[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF0, 0XD4, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd123[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF1, 0XFF, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd124[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF2, 0X08, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd125[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF3, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd126[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF4, 0X3B, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd127[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF5, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd128[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF6, 0XB3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd129[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF7, 0XBC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd130[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF8, 0XC5, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd131[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF9, 0XD0, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd132[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFA, 0XDB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd133[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFB, 0XE7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd134[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFC, 0XF7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd135[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFD, 0XFE, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd136[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFE, 0X00, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd137[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFF, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd138[] = {
	0x02, 0x00, 0x29, 0xC0,
	0X83, 0XCC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd139[] = {
	0x02, 0x00, 0x29, 0xC0,
	0X84, 0X33, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd140[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC0, 0X0E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd141[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC1, 0X12, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd142[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC2, 0X25, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd143[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC3, 0X34, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd144[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC4, 0X3F, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd145[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC5, 0X49, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd146[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC6, 0X52, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd147[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC7, 0X59, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd148[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC8, 0X60, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd149[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XC9, 0XC8, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd150[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCA, 0XC7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd151[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCB, 0XD6, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd152[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCC, 0XD9, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd153[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCD, 0XD7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd154[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCE, 0XD1, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd155[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XCF, 0XD3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd156[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD0, 0XD4, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd157[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD1, 0XFF, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd158[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD2, 0X08, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd159[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD3, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd160[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD4, 0X3B, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd161[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD5, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd162[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD6, 0XB3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd163[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD7, 0XBC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd164[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD8, 0XC5, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd165[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XD9, 0XD0, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd166[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDA, 0XDB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd167[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDB, 0XE7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd168[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDC, 0XF7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd169[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDD, 0XFE, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd170[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDE, 0X00, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd171[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XDF, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd172[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE0, 0X0E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd173[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE1, 0X12, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd174[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE2, 0X25, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd175[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE3, 0X34, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd176[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE4, 0X3F, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd177[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE5, 0X49, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd178[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE6, 0X52, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd179[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE7, 0X59, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd180[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE8, 0X60, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd181[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XE9, 0XC8, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd182[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEA, 0XC7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd183[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEB, 0XD6, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd184[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEC, 0XD9, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd185[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XED, 0XD7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd186[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEE, 0XD1, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd187[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XEF, 0XD3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd188[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF0, 0XD4, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd189[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF1, 0XFF, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd190[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF2, 0X08, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd191[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF3, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd192[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF4, 0X3B, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd193[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF5, 0XAA, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd194[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF6, 0XB3, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd195[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF7, 0XBC, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd196[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF8, 0XC5, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd197[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XF9, 0XD0, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd198[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFA, 0XDB, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd199[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFB, 0XE7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd200[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFC, 0XF7, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd201[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFD, 0XFE, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd202[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFE, 0X00, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd203[] = {
	0x02, 0x00, 0x29, 0xC0,
	0XFF, 0X2E, 0xFF, 0xFF,
};

static char byd_1200p_video_on_cmd204[] = {
	0x02, 0x00, 0x29, 0xC0,
	0x11, 0x00, 0xFF, 0xFF,};

static char byd_1200p_video_on_cmd205[] = {
	0x02, 0x00, 0x29, 0xC0,
	0x29, 0x00, 0xFF, 0xFF,
};

static struct mipi_dsi_cmd byd_1200p_video_on_command[] = {
	{ 0x8 , byd_1200p_video_on_cmd0,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd1,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd2,  0x10},
	{ 0x8 , byd_1200p_video_on_cmd3,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd4,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd5,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd6,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd7,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd8,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd9,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd10,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd11,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd12,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd13,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd14,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd15,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd16,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd17,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd18,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd19,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd20,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd21,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd22,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd23,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd24,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd25,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd26,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd27,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd28,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd29,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd30,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd31,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd32,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd33,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd34,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd35,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd36,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd37,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd38,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd39,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd40,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd41,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd42,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd43,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd44,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd45,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd46,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd47,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd48,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd49,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd50,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd51,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd52,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd53,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd54,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd55,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd56,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd57,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd58,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd59,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd60,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd61,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd62,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd63,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd64,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd65,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd66,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd67,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd68,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd69,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd70,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd71,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd72,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd73,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd74,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd75,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd76,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd77,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd78,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd79,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd80,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd81,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd82,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd83,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd84,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd85,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd86,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd87,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd88,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd89,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd90,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd91,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd92,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd93,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd94,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd95,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd96,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd97,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd98,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd99,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd100,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd101,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd102,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd103,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd104,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd105,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd106,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd107,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd108,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd109,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd110,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd111,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd112,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd113,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd114,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd115,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd116,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd117,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd118,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd119,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd120,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd121,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd122,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd123,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd124,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd125,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd126,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd127,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd128,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd129,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd130,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd131,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd132,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd133,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd134,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd135,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd136,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd137,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd138,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd139,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd140,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd141,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd142,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd143,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd144,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd145,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd146,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd147,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd148,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd149,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd150,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd151,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd152,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd153,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd154,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd155,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd156,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd157,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd158,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd159,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd160,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd161,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd162,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd163,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd164,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd165,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd166,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd167,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd168,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd169,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd170,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd171,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd172,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd173,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd174,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd175,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd176,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd177,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd178,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd179,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd180,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd181,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd182,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd183,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd184,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd185,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd186,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd187,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd188,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd189,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd190,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd191,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd192,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd193,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd194,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd195,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd196,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd197,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd198,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd199,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd200,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd201,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd202,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd203,  0x00},
	{ 0x8 , byd_1200p_video_on_cmd204,  0xca},
	{ 0x8 , byd_1200p_video_on_cmd205,  0x1f},
};

#define BYD_1200P_VIDEO_ON_COMMAND 206


static char byd_1200p_video_off_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char byd_1200p_video_off_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};


static struct mipi_dsi_cmd byd_1200p_video_off_command[] = {
	{ 0x4 , byd_1200p_video_off_cmd0, 0x0},
	{ 0x4 , byd_1200p_video_off_cmd1, 0x0}
};

#define BYD_1200P_VIDEO_OFF_COMMAND 2

static struct command_state byd_1200p_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info byd_1200p_video_command_panel = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info byd_1200p_video_video_panel = {
	0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration byd_1200p_video_lane_config = {
	4, 0, 1, 1, 1, 1, 1
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t byd_1200p_video_timings[] = {
	0xE6, 0x38, 0x26, 0x00, 0x68, 0x6e, 0x2A, 0x3c, 0x44, 0x03, 0x04, 0x00
};

static struct panel_timing byd_1200p_video_timing_info = {
	0, 4, 0x02, 0x2d
};

static struct panel_reset_sequence byd_1200p_video_panel_reset_seq = {
	{ 0, 1, 0, }, { 200, 200, 200, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight byd_1200p_video_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

#define BYD_1200P_VIDEO_SIGNATURE 0xFFFF

#endif /*_BYD_1200P_VIDEO_H_*/
