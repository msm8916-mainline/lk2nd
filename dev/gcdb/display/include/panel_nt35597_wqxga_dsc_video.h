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

#ifndef _PANEL_NT35597_WQXGA_DSC_VIDEO_H_
#define _PANEL_NT35597_WQXGA_DSC_VIDEO_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config nt35597_wqxga_dsc_video_panel_data = {
	"qcom,mdss_dsi_nt35597_dsc_wqxga_video", "dsi:1:", "qcom,mdss-dsi-panel",
	10, 0, "DISPLAY_2", 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution nt35597_wqxga_dsc_video_panel_res = {
	1440, 2560, 100, 32, 16, 0, 8, 7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info nt35597_wqxga_dsc_video_color = {
	24, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char nt35597_wqxga_dsc_video_on_cmd0[] = {
	0xff, 0x10, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd1[] = {
	0xfb, 0x01, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd2[] = {
	0xba, 0x03, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd3[] = {
	0xe5, 0x01, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd4[] = {
	0xb0, 0x03, 0x15, 0x80
};


static char nt35597_wqxga_dsc_video_on_cmd5[] = {
	0x06, 0x00, 0x39, 0xC0,
	0x3B, 0x03, 0x08, 0x08,
	0x2e, 0x64, 0xff, 0xff,
};

static char nt35597_wqxga_dsc_video_on_cmd6[] = {
	0xff, 0x28, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd7[] = {
	0x7a, 0x02, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd8[] = {
	0xfb, 0x01, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd9[] = {
	0xff, 0x10, 0x15, 0x80
};

#ifdef USE_MANUFACTUR_DSC_CMD
static char nt35597_wqxga_dsc_video_on_cmd10[] = {
	0x11, 0x00, 0x39, 0xC0,
	0xC1, 0x09, 0x20, 0x00,
	0x10, 0x02, 0x00, 0x02,
	0x68, 0x01, 0xBB, 0x00,
	0x0A, 0x06, 0x67, 0x04,
	0xC5, 0xff, 0xff, 0xff,
};

static char nt35597_wqxga_dsc_video_on_cmd11[] = {
	0x03, 0x00, 0x39, 0xC0,
	0xc2, 0x10, 0xf0, 0xff,
};
#endif

static char nt35597_wqxga_dsc_video_on_cmd12[] = {
	0xfb, 0x01, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd13[] = {
	0xc0, 0x03, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd14[] = {
	0xbb, 0x03, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd15[] = {
	0xff, 0xe0, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd16[] = {
	0xfb, 0x01, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd17[] = {
	0x6b, 0x3d, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd18[] = {
	0x6c, 0x3d, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd19[] = {
	0x6d, 0x3d, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd20[] = {
	0x6e, 0x3d, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd21[] = {
	0x6f, 0x3d, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd22[] = {
	0x35, 0x02, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd23[] = {
	0x36, 0x72, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd24[] = {
	0x37, 0x10, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd25[] = {
	0x08, 0xc0, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd26[] = {
	0xff, 0x10, 0x15, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd27[] = {
	0x11, 0x00, 0x05, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd28[] = {
	0x29, 0x00, 0x05, 0x80
};

static char nt35597_wqxga_dsc_video_on_cmd29[] = {
	0x01, 0x00, 0x07, 0x80
};

static struct mipi_dsi_cmd nt35597_wqxga_dsc_video_on_command[] = {
	{0x4, nt35597_wqxga_dsc_video_on_cmd0, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd1, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd2, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd3, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd4, 0x10},
	{0xc, nt35597_wqxga_dsc_video_on_cmd5, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd6, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd7, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd8, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd9, 0x10},
#ifdef USE_MANUFACTUR_DSC_CMD
	{0x18, nt35597_wqxga_dsc_video_on_cmd10, 0x10},
	{0x8, nt35597_wqxga_dsc_video_on_cmd11, 0x10},
#endif
	{0x4, nt35597_wqxga_dsc_video_on_cmd12, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd13, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd14, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd15, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd16, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd17, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd18, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd19, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd20, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd21, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd22, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd23, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd24, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd25, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd26, 0x10},
	{0x4, nt35597_wqxga_dsc_video_on_cmd27, 0x100},
	{0x4, nt35597_wqxga_dsc_video_on_cmd28, 0x100},
	{0x4, nt35597_wqxga_dsc_video_on_cmd29, 0x10},
};

#ifdef USE_MANUFACTUR_DSC_CMD
#define NT35597_WQXGA_DSC_VIDEO_ON_COMMAND 30
#else
#define NT35597_WQXGA_DSC_VIDEO_ON_COMMAND 28
#endif


static char nt35597_wqxga_dsc_video_off_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char nt35597_wqxga_dsc_video_off_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd nt35597_wqxga_dsc_video_off_command[] = {
	{0x4, nt35597_wqxga_dsc_video_off_cmd0, 0x32},
	{0x4, nt35597_wqxga_dsc_video_off_cmd1, 0x78}
};

#define NT35597_WQXGA_DSC_VIDEO_OFF_COMMAND 2

static struct command_state nt35597_wqxga_dsc_video_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info nt35597_wqxga_dsc_video_command_panel = {
	1, 1, 1, 0, 0, 0x2c, 0, 0, 0, 1, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info nt35597_wqxga_dsc_video_video_panel = {
	0, 0, 0, 0, 1, 1, 1, 0, 0x9
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration nt35597_wqxga_dsc_video_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t nt35597_wqxga_dsc_video_timings[] = {
	0xa4, 0x24, 0x18, 0x00, 0x4c, 0x50, 0x1c, 0x28, 0x1c, 0x03, 0x04, 0x00,
};

static const uint32_t nt35597_wqxga_dsc_thulium_video_timings[] = {
	0x20, 0x1d, 0x05, 0x07, 0x03, 0x03, 0x4, 0xa0,
	0x20, 0x1d, 0x05, 0x07, 0x03, 0x03, 0x4, 0xa0,
	0x20, 0x1d, 0x05, 0x07, 0x03, 0x03, 0x4, 0xa0,
	0x20, 0x1d, 0x05, 0x07, 0x03, 0x03, 0x4, 0xa0,
	0x20, 0x12, 0x05, 0x06, 0x03, 0x13, 0x4, 0xa0,
};

static struct panel_timing nt35597_wqxga_dsc_video_timing_info = {
	0x0, 0x04, 0x0b, 0x24
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence nt35597_wqxga_dsc_video_reset_seq = {
	{1, 0, 1, }, {20, 20, 50, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight nt35597_wqxga_dsc_video_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"		/* BL_WLED */
};

static struct labibb_desc nt35597_wqxga_dsc_video_labibb = {
	0, 1, 5500000, 5500000, 5500000, 5500000, 3, 3, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Dynamic fps supported frequencies by panel                                */
/*---------------------------------------------------------------------------*/
static const struct dfps_panel_info nt35597_wqxga_dsc_video_dfps = {
	1, 8, {53, 54, 55, 56, 57, 58, 59, 60}
};

/*---------------------------------------------------------------------------*/
/* DSC									     */
/*---------------------------------------------------------------------------*/
struct dsc_parameters nt35597_wqxga_dsc_video_params0 = {
	1, 1, 0, 16, 720, 8, 8, 2, 1, 0
};

/* 1LM + 1 DSC_ENC */
struct topology_config nt35597_wqxga_dsc_video_config0 = {
	"config0", {-1, -1}, 1, &nt35597_wqxga_dsc_video_params0, false
};

/* 2LM + 3D Mux + 1 DSC_ENC */
struct topology_config nt35597_wqxga_dsc_video_config1 = {
	"config1", {720, 720}, 1, &nt35597_wqxga_dsc_video_params0, false
};

/* 2LM + 2 DSC_ENC + DSC_MERGE */
struct topology_config nt35597_wqxga_dsc_video_config2 = {
	"config2", {720, 720}, 2, &nt35597_wqxga_dsc_video_params0, false
};

#endif
