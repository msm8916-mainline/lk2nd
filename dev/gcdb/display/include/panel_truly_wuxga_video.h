/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_TRULY_WUXGA_VIDEO_H_

#define _PANEL_TRULY_WUXGA_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/

static struct panel_config truly_wuxga_video_panel_data = {
  "qcom,mdss_dsi_truly_wuxga_video", "dsi:0:", "qcom,mdss-dsi-panel",
  10, 0, "DISPLAY_1", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution truly_wuxga_video_panel_res = {
  1920, 1200, 96, 64, 16, 0, 4, 16, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Color Information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info truly_wuxga_video_color = {
  24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel Command information                                                 */
/*---------------------------------------------------------------------------*/
static char truly_wuxga_video_on_cmd0[] = {
0x00, 0x00, 0x32, 0x80, };

static struct mipi_dsi_cmd truly_wuxga_video_on_command[] = {
{ 0x4 , truly_wuxga_video_on_cmd0, 0x00}
};
#define TRULY_WUXGA_VIDEO_ON_COMMAND 1


static char truly_wuxga_video_off_cmd0[] = {
0x00, 0x00, 0x22, 0x80 };

static struct mipi_dsi_cmd truly_wuxga_video_off_command[] = {
{ 0x4 , truly_wuxga_video_off_cmd0, 0x0}
};
#define TRULY_WUXGA_VIDEO_OFF_COMMAND 1


static struct command_state truly_wuxga_video_state = {
  0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/

static struct commandpanel_info truly_wuxga_video_command_panel = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/

static struct videopanel_info truly_wuxga_video_video_panel = {
  0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane Configuration                                                        */
/*---------------------------------------------------------------------------*/

static struct lane_configuration truly_wuxga_video_lane_config = {
  4, 0, 1, 1, 1, 1, 0
};


/*---------------------------------------------------------------------------*/
/* Panel Timing                                                              */
/*---------------------------------------------------------------------------*/
const uint32_t truly_wuxga_video_timings[] = {
  0xf3, 0x3a, 0x26, 0x00, 0x6c, 0x6e, 0x2c, 0x3e, 0x2f, 0x03, 0x04, 0x00
};

static const uint32_t truly_wuxga_14nm_video_timings[] = {
	0x24, 0x1f, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x24, 0x1f, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x24, 0x1f, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x24, 0x1f, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
	0x24, 0x1c, 0x8, 0x9, 0x5, 0x3, 0x4, 0xa0,
};

static struct panel_timing truly_wuxga_video_timing_info = {
  0, 4, 0x02, 0x2d
};

static struct panel_reset_sequence truly_wuxga_video_panel_reset_seq = {
{ 1, 0, 1, }, { 200, 200, 200, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight Settings                                                        */
/*---------------------------------------------------------------------------*/

static struct backlight truly_wuxga_video_backlight = {
  1, 1, 4095, 100, 1, "PMIC_8941"
};

/*---------------------------------------------------------------------------*/
/* Dynamic fps supported frequencies by panel                                */
/*---------------------------------------------------------------------------*/
static const struct dfps_panel_info truly_wuxga_video_dfps = {
        1, 13, {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60}
};


#define TRULY_WUXGA_VIDEO_SIGNATURE 0xFFFF

#endif /*_TRULY_WUXGA_VIDEO_H_*/
