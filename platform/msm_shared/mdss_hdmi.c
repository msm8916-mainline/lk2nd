/* Copyright (c) 2010-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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
#include <err.h>
#include <debug.h>
#include <reg.h>
#include <msm_panel.h>
#include <platform/timer.h>
#include <platform/clock.h>
#include <platform/iomap.h>

static struct msm_fb_panel_data panel;

/* AVI INFOFRAME DATA */
#define NUM_MODES_AVI 20
#define AVI_MAX_DATA_BYTES 13

enum {
	DATA_BYTE_1,
	DATA_BYTE_2,
	DATA_BYTE_3,
	DATA_BYTE_4,
	DATA_BYTE_5,
	DATA_BYTE_6,
	DATA_BYTE_7,
	DATA_BYTE_8,
	DATA_BYTE_9,
	DATA_BYTE_10,
	DATA_BYTE_11,
	DATA_BYTE_12,
	DATA_BYTE_13,
};

#define IFRAME_PACKET_OFFSET 0x80
/*
 * InfoFrame Type Code:
 * 0x0 - Reserved
 * 0x1 - Vendor Specific
 * 0x2 - Auxiliary Video Information
 * 0x3 - Source Product Description
 * 0x4 - AUDIO
 * 0x5 - MPEG Source
 * 0x6 - NTSC VBI
 * 0x7 - 0xFF - Reserved
 */
#define AVI_IFRAME_TYPE 0x2
#define AVI_IFRAME_VERSION 0x2
#define LEFT_SHIFT_BYTE(x) ((x) << 8)
#define LEFT_SHIFT_WORD(x) ((x) << 16)
#define LEFT_SHIFT_24BITS(x) ((x) << 24)

struct mdss_hdmi_timing_info {
	uint32_t	video_format;
	uint32_t	active_h;
	uint32_t	front_porch_h;
	uint32_t	pulse_width_h;
	uint32_t	back_porch_h;
	uint32_t	active_low_h;
	uint32_t	active_v;
	uint32_t	front_porch_v;
	uint32_t	pulse_width_v;
	uint32_t	back_porch_v;
	uint32_t	active_low_v;
	/* Must divide by 1000 to get the actual frequency in MHZ */
	uint32_t	pixel_freq;
	/* Must divide by 1000 to get the actual frequency in HZ */
	uint32_t	refresh_rate;
	uint32_t	interlaced;
	uint32_t	supported;
};

#define HDMI_VFRMT_1280x720p60_16_9	4
#define HDMI_RESOLUTION_DATA HDMI_VFRMT_1280x720p60_16_9##_TIMING

#define HDMI_VFRMT_1280x720p60_16_9_TIMING				\
	{HDMI_VFRMT_1280x720p60_16_9, 1280, 110, 40, 220, false,	\
		720, 5, 5, 20, false, 74250, 60000, false, true}

uint32_t mdss_hdmi_avi_info_db[] = {
	0x10, 0x28, 0x00, 0x04, 0x00, 0x00, 0x00,
	0xD1, 0x02, 0x00, 0x00, 0x01, 0x05};

static int mdss_hdmi_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	return target_hdmi_panel_clock(enable, pinfo);
}

static int mdss_hdmi_enable_power(uint8_t enable)
{
        int ret = NO_ERROR;

        ret = target_ldo_ctrl(enable);
        if (ret) {
                dprintf(CRITICAL, "LDO control enable failed\n");
                return ret;
        }

        ret = target_hdmi_regulator_ctrl(enable);
        if (ret) {
                dprintf(CRITICAL, "hdmi regulator control enable failed\n");
                return ret;
        }

        dprintf(SPEW, "HDMI Panel power %s done\n", enable ? "on" : "off");

        return ret;
}

static void mdss_hdmi_set_mode(bool on)
{
	uint32_t val = 0;

	if (on) {
		val = 0x3;
		writel(val, HDMI_CTRL);
	} else {
		writel(val, HDMI_CTRL);
	}
}

static void mdss_hdmi_panel_init(struct msm_panel_info *pinfo)
{
	struct mdss_hdmi_timing_info tinfo = HDMI_RESOLUTION_DATA;

	if (!pinfo)
		return;

	pinfo->xres = tinfo.active_h;
	pinfo->yres = tinfo.active_v;
	pinfo->bpp  = 24;
	pinfo->type = HDMI_PANEL;

        pinfo->lcdc.h_back_porch  = tinfo.back_porch_h;
        pinfo->lcdc.h_front_porch = tinfo.front_porch_h;
        pinfo->lcdc.h_pulse_width = tinfo.pulse_width_h;
        pinfo->lcdc.v_back_porch  = tinfo.back_porch_v;
        pinfo->lcdc.v_front_porch = tinfo.front_porch_v;
        pinfo->lcdc.v_pulse_width = tinfo.pulse_width_v;

        pinfo->lcdc.hsync_skew = 0;
        pinfo->lcdc.xres_pad   = 0;
        pinfo->lcdc.yres_pad   = 0;
        pinfo->lcdc.dual_pipe  = 0;
}

void mdss_hdmi_display_init(uint32_t rev, void *base)
{
	mdss_hdmi_panel_init(&(panel.panel_info));

	panel.clk_func   = mdss_hdmi_panel_clock;
	panel.power_func = mdss_hdmi_enable_power;

	panel.fb.width   = panel.panel_info.xres;
	panel.fb.height  = panel.panel_info.yres;
	panel.fb.stride  = panel.panel_info.xres;
	panel.fb.bpp     = panel.panel_info.bpp;
	panel.fb.format  = FB_FORMAT_RGB888;

	panel.fb.base = base;
	panel.mdp_rev = rev;

	msm_display_init(&panel);
}

static int mdss_hdmi_video_setup(void)
{
	uint32_t total_v   = 0;
	uint32_t total_h   = 0;
	uint32_t start_h   = 0;
	uint32_t end_h     = 0;
	uint32_t start_v   = 0;
	uint32_t end_v     = 0;

	struct mdss_hdmi_timing_info tinfo = HDMI_RESOLUTION_DATA;

	total_h = tinfo.active_h + tinfo.front_porch_h +
		tinfo.back_porch_h + tinfo.pulse_width_h - 1;
	total_v = tinfo.active_v + tinfo.front_porch_v +
		tinfo.back_porch_v + tinfo.pulse_width_v - 1;
	if (((total_v << 16) & 0xE0000000) || (total_h & 0xFFFFE000)) {
		dprintf(CRITICAL,
			"%s: total v=%d or h=%d is larger than supported\n",
			__func__, total_v, total_h);
		return ERROR;
	}
	writel((total_v << 16) | (total_h << 0), HDMI_TOTAL);

	start_h = tinfo.back_porch_h + tinfo.pulse_width_h;
	end_h   = (total_h + 1) - tinfo.front_porch_h;
	if (((end_h << 16) & 0xE0000000) || (start_h & 0xFFFFE000)) {
		dprintf(CRITICAL,
			"%s: end_h=%d or start_h=%d is larger than supported\n",
			__func__, end_h, start_h);
		return ERROR;
	}
	writel((end_h << 16) | (start_h << 0), HDMI_ACTIVE_H);

	start_v = tinfo.back_porch_v + tinfo.pulse_width_v - 1;
	end_v   = total_v - tinfo.front_porch_v;
	if (((end_v << 16) & 0xE0000000) || (start_v & 0xFFFFE000)) {
		dprintf(CRITICAL,
			"%s: end_v=%d or start_v=%d is larger than supported\n",
			__func__, end_v, start_v);
		return ERROR;
	}
	writel((end_v << 16) | (start_v << 0), HDMI_ACTIVE_V);

	if (tinfo.interlaced) {
		writel((total_v + 1) << 0, HDMI_V_TOTAL_F2);
		writel(((end_v + 1) << 16) | ((start_v + 1) << 0),
			HDMI_ACTIVE_V_F2);
	} else {
		writel(0, HDMI_V_TOTAL_F2);
		writel(0, HDMI_ACTIVE_V_F2);
	}

	writel(((tinfo.interlaced << 31) & 0x80000000) |
		((tinfo.active_low_h << 29) & 0x20000000) |
		((tinfo.active_low_v << 28) & 0x10000000), HDMI_FRAME_CTRL);

	return 0;
}

void mdss_hdmi_avi_info_frame(void)
{
	uint32_t sum;
	uint32_t reg_val;
	uint8_t checksum;
	uint32_t i;

	sum = IFRAME_PACKET_OFFSET + AVI_IFRAME_TYPE +
		AVI_IFRAME_VERSION + AVI_MAX_DATA_BYTES;

	for (i = 0; i < AVI_MAX_DATA_BYTES; i++)
		sum += mdss_hdmi_avi_info_db[i];

	sum &= 0xFF;
	sum = 256 - sum;
	checksum = (uint8_t) sum;

	reg_val = checksum |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[DATA_BYTE_1]) |
		LEFT_SHIFT_WORD(mdss_hdmi_avi_info_db[DATA_BYTE_2]) |
		LEFT_SHIFT_24BITS(mdss_hdmi_avi_info_db[DATA_BYTE_3]);
	writel(reg_val, HDMI_AVI_INFO0);

	reg_val = mdss_hdmi_avi_info_db[DATA_BYTE_4] |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[DATA_BYTE_5]) |
		LEFT_SHIFT_WORD(mdss_hdmi_avi_info_db[DATA_BYTE_6]) |
		LEFT_SHIFT_24BITS(mdss_hdmi_avi_info_db[DATA_BYTE_7]);
	writel(reg_val, HDMI_AVI_INFO1);

	reg_val = mdss_hdmi_avi_info_db[DATA_BYTE_8] |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[DATA_BYTE_9]) |
		LEFT_SHIFT_WORD(mdss_hdmi_avi_info_db[DATA_BYTE_10]) |
		LEFT_SHIFT_24BITS(mdss_hdmi_avi_info_db[DATA_BYTE_11]);
	writel(reg_val, HDMI_AVI_INFO2);

	reg_val = mdss_hdmi_avi_info_db[DATA_BYTE_12] |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[DATA_BYTE_13]) |
		LEFT_SHIFT_24BITS(AVI_IFRAME_VERSION);
	writel(reg_val, HDMI_AVI_INFO3);

	/* AVI InfFrame enable (every frame) */
	writel(readl(HDMI_INFOFRAME_CTRL0) | BIT(1) | BIT(0),
		HDMI_INFOFRAME_CTRL0);
}

int mdss_hdmi_init(void)
{
	uint32_t hotplug_control;

	mdss_hdmi_set_mode(false);

	hdmi_phy_init();

	// Enable USEC REF timer
	writel(0x0001001B, HDMI_USEC_REFTIMER);

	// Video setup for HDMI
	mdss_hdmi_video_setup();

	// AVI info setup
	mdss_hdmi_avi_info_frame();

	// Write 1 to HDMI_CTRL to enable HDMI
	mdss_hdmi_set_mode(true);

	return 0;
}
