/* Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
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
#include <hdmi.h>
#include <msm_panel.h>
#include <platform/timer.h>
#include <platform/clock.h>
#include <platform/iomap.h>

#define MDP4_OVERLAYPROC1_BASE  0x18000
#define MDP4_RGB_BASE           0x40000
#define MDP4_RGB_OFF            0x10000

struct hdmi_disp_mode_timing_type hdmi_timing_default = {
	.height         = 1080,
	.hsync_porch_fp = 88,
	.hsync_width    = 44,
	.hsync_porch_bp = 148,
	.width          = 1920,
	.vsync_porch_fp = 4,
	.vsync_width    = 5,
	.vsync_porch_bp = 36,
	.bpp            = 24,
};

static uint8_t hdmi_msm_avi_iframe_lut[][16] = {
/*	480p60	480i60	576p50	576i50	720p60	 720p50	1080p60	1080i60	1080p50
	1080i50	1080p24	1080p30	1080p25	640x480p 480p60_16_9 576p50_4_3 */
	{0x10,	0x10,	0x10,	0x10,	0x10,	 0x10,	0x10,	0x10,	0x10,
	 0x10,	0x10,	0x10,	0x10,	0x10, 0x10, 0x10}, /*00*/
	{0x18,	0x18,	0x28,	0x28,	0x28,	 0x28,	0x28,	0x28,	0x28,
	 0x28,	0x28,	0x28,	0x28,	0x18, 0x28, 0x18}, /*01*/
	{0x00,	0x04,	0x04,	0x04,	0x04,	 0x04,	0x04,	0x04,	0x04,
	 0x04,	0x04,	0x04,	0x04,	0x88, 0x00, 0x04}, /*02*/
	{0x02,	0x06,	0x11,	0x15,	0x04,	 0x13,	0x10,	0x05,	0x1F,
	 0x14,	0x20,	0x22,	0x21,	0x01, 0x03, 0x11}, /*03*/
	{0x00,	0x01,	0x00,	0x01,	0x00,	 0x00,	0x00,	0x00,	0x00,
	 0x00,	0x00,	0x00,	0x00,	0x00, 0x00, 0x00}, /*04*/
	{0x00,	0x00,	0x00,	0x00,	0x00,	 0x00,	0x00,	0x00,	0x00,
	 0x00,	0x00,	0x00,	0x00,	0x00, 0x00, 0x00}, /*05*/
	{0x00,	0x00,	0x00,	0x00,	0x00,	 0x00,	0x00,	0x00,	0x00,
	 0x00,	0x00,	0x00,	0x00,	0x00, 0x00, 0x00}, /*06*/
	{0xE1,	0xE1,	0x41,	0x41,	0xD1,	 0xd1,	0x39,	0x39,	0x39,
	 0x39,	0x39,	0x39,	0x39,	0xe1, 0xE1, 0x41}, /*07*/
	{0x01,	0x01,	0x02,	0x02,	0x02,	 0x02,	0x04,	0x04,	0x04,
	 0x04,	0x04,	0x04,	0x04,	0x01, 0x01, 0x02}, /*08*/
	{0x00,	0x00,	0x00,	0x00,	0x00,	 0x00,	0x00,	0x00,	0x00,
	 0x00,	0x00,	0x00,	0x00,	0x00, 0x00, 0x00}, /*09*/
	{0x00,	0x00,	0x00,	0x00,	0x00,	 0x00,	0x00,	0x00,	0x00,
	 0x00,	0x00,	0x00,	0x00,	0x00, 0x00, 0x00}, /*10*/
	{0xD1,	0xD1,	0xD1,	0xD1,	0x01,	 0x01,	0x81,	0x81,	0x81,
	 0x81,	0x81,	0x81,	0x81,	0x81, 0xD1, 0xD1}, /*11*/
	{0x02,	0x02,	0x02,	0x02,	0x05,	 0x05,	0x07,	0x07,	0x07,
	 0x07,	0x07,	0x07,	0x07,	0x02, 0x02, 0x02}  /*12*/
};

void hdmi_msm_set_mode(int on)
{
	uint32_t val = 0;
	if (on) {
		val |= 0x00000003;
		writel(val, HDMI_CTRL);
	} else {
		val &= ~0x00000002;
		writel(val, HDMI_CTRL);
	}
}

struct hdmi_disp_mode_timing_type *hdmi_common_init_panel_info()
{
	return &hdmi_timing_default;
}

void hdmi_set_fb_addr(void *addr)
{
	hdmi_timing_default.base = addr;
}

void hdmi_msm_panel_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = hdmi_timing_default.width;
	pinfo->yres = hdmi_timing_default.height;
	pinfo->bpp  = hdmi_timing_default.bpp;
	pinfo->type = HDMI_PANEL;

	pinfo->hdmi.h_back_porch  = hdmi_timing_default.hsync_porch_bp;
	pinfo->hdmi.h_front_porch = hdmi_timing_default.hsync_porch_fp;
	pinfo->hdmi.h_pulse_width = hdmi_timing_default.hsync_width;
	pinfo->hdmi.v_back_porch  = hdmi_timing_default.vsync_porch_bp;
	pinfo->hdmi.v_front_porch = hdmi_timing_default.vsync_porch_fp;
	pinfo->hdmi.v_pulse_width = hdmi_timing_default.vsync_width;
}

void hdmi_frame_ctrl_reg()
{
	uint32_t hdmi_frame_ctrl;

	hdmi_frame_ctrl  = ((0 << 31) & 0x80000000);
	hdmi_frame_ctrl |= ((0 << 29) & 0x20000000);
	hdmi_frame_ctrl |= ((0 << 28) & 0x10000000);
	writel(hdmi_frame_ctrl, HDMI_FRAME_CTRL);
}

void hdmi_video_setup()
{
	uint32_t hsync_total = 0;
	uint32_t vsync_total = 0;
	uint32_t hsync_start = 0;
	uint32_t hsync_end = 0;
	uint32_t vsync_start = 0;
	uint32_t vsync_end = 0;
	uint32_t hvsync_total = 0;
	uint32_t hsync_active = 0;
	uint32_t vsync_active = 0;
	uint32_t hdmi_frame_ctrl = 0;
	uint32_t val;
	struct hdmi_disp_mode_timing_type *hdmi_timing =
	    hdmi_common_init_panel_info();

	hsync_total = hdmi_timing->width + hdmi_timing->hsync_porch_fp
	    + hdmi_timing->hsync_porch_bp + hdmi_timing->hsync_width - 1;
	vsync_total = hdmi_timing->height + hdmi_timing->vsync_porch_fp
	    + hdmi_timing->vsync_porch_bp + hdmi_timing->vsync_width - 1;

	hvsync_total = (vsync_total << 16) & 0x0FFF0000;
	hvsync_total |= (hsync_total << 0) & 0x00000FFF;
	writel(hvsync_total, HDMI_TOTAL);

	hsync_start = hdmi_timing->hsync_porch_bp + hdmi_timing->hsync_width;
	hsync_end = (hsync_total + 1) - hdmi_timing->hsync_porch_fp;
	hsync_active = (hsync_end << 16) & 0x0FFF0000;
	hsync_active |= (hsync_start << 0) & 0x00000FFF;
	writel(hsync_active, HDMI_ACTIVE_HSYNC);

	vsync_start =
	    hdmi_timing->vsync_porch_bp + hdmi_timing->vsync_width - 1;
	vsync_end = vsync_total - hdmi_timing->vsync_porch_fp;
	vsync_active = (vsync_end << 16) & 0x0FFF0000;
	vsync_active |= (vsync_start << 0) & 0x00000FFF;
	writel(vsync_active, HDMI_ACTIVE_VSYNC);

	writel(0, HDMI_VSYNC_TOTAL_F2);
	writel(0, HDMI_VSYNC_ACTIVE_F2);
	hdmi_frame_ctrl_reg();
}

void hdmi_msm_avi_info_frame(void)
{
	/* two header + length + 13 data */
	uint8_t  aviInfoFrame[16];
	uint8_t  checksum;
	uint32_t sum;
	uint32_t regVal;
	uint8_t  i;
	uint8_t  mode = 6; //HDMI_VFRMT_1920x1080p60_16_9

	/* InfoFrame Type = 82 */
	aviInfoFrame[0]  = 0x82;
	/* Version = 2 */
	aviInfoFrame[1]  = 2;
	/* Length of AVI InfoFrame = 13 */
	aviInfoFrame[2]  = 13;

	/* Data Byte 01: 0 Y1 Y0 A0 B1 B0 S1 S0 */
	aviInfoFrame[3]  = hdmi_msm_avi_iframe_lut[0][mode];

	/* Setting underscan bit */
	aviInfoFrame[3] |= 0x02;

	/* Data Byte 02: C1 C0 M1 M0 R3 R2 R1 R0 */
	aviInfoFrame[4]  = hdmi_msm_avi_iframe_lut[1][mode];
	/* Data Byte 03: ITC EC2 EC1 EC0 Q1 Q0 SC1 SC0 */
	aviInfoFrame[5]  = hdmi_msm_avi_iframe_lut[2][mode];
	/* Data Byte 04: 0 VIC6 VIC5 VIC4 VIC3 VIC2 VIC1 VIC0 */
	aviInfoFrame[6]  = hdmi_msm_avi_iframe_lut[3][mode];
	/* Data Byte 05: 0 0 0 0 PR3 PR2 PR1 PR0 */
	aviInfoFrame[7]  = hdmi_msm_avi_iframe_lut[4][mode];
	/* Data Byte 06: LSB Line No of End of Top Bar */
	aviInfoFrame[8]  = hdmi_msm_avi_iframe_lut[5][mode];
	/* Data Byte 07: MSB Line No of End of Top Bar */
	aviInfoFrame[9]  = hdmi_msm_avi_iframe_lut[6][mode];
	/* Data Byte 08: LSB Line No of Start of Bottom Bar */
	aviInfoFrame[10] = hdmi_msm_avi_iframe_lut[7][mode];
	/* Data Byte 09: MSB Line No of Start of Bottom Bar */
	aviInfoFrame[11] = hdmi_msm_avi_iframe_lut[8][mode];
	/* Data Byte 10: LSB Pixel Number of End of Left Bar */
	aviInfoFrame[12] = hdmi_msm_avi_iframe_lut[9][mode];
	/* Data Byte 11: MSB Pixel Number of End of Left Bar */
	aviInfoFrame[13] = hdmi_msm_avi_iframe_lut[10][mode];
	/* Data Byte 12: LSB Pixel Number of Start of Right Bar */
	aviInfoFrame[14] = hdmi_msm_avi_iframe_lut[11][mode];
	/* Data Byte 13: MSB Pixel Number of Start of Right Bar */
	aviInfoFrame[15] = hdmi_msm_avi_iframe_lut[12][mode];

	sum = 0;
	for (i = 0; i < 16; i++)
		sum += aviInfoFrame[i];
	sum &= 0xFF;
	sum = 256 - sum;
	checksum = (uint8_t) sum;

	regVal = aviInfoFrame[5];
	regVal = regVal << 8 | aviInfoFrame[4];
	regVal = regVal << 8 | aviInfoFrame[3];
	regVal = regVal << 8 | checksum;
	writel(regVal, MSM_HDMI_BASE + 0x006C);

	regVal = aviInfoFrame[9];
	regVal = regVal << 8 | aviInfoFrame[8];
	regVal = regVal << 8 | aviInfoFrame[7];
	regVal = regVal << 8 | aviInfoFrame[6];
	writel(regVal, MSM_HDMI_BASE + 0x0070);

	regVal = aviInfoFrame[13];
	regVal = regVal << 8 | aviInfoFrame[12];
	regVal = regVal << 8 | aviInfoFrame[11];
	regVal = regVal << 8 | aviInfoFrame[10];
	writel(regVal, MSM_HDMI_BASE + 0x0074);

	regVal = aviInfoFrame[1];
	regVal = regVal << 16 | aviInfoFrame[15];
	regVal = regVal << 8 | aviInfoFrame[14];
	writel(regVal, MSM_HDMI_BASE + 0x0078);

	/* INFOFRAME_CTRL0[0x002C] */
	/* 0x3 for AVI InfFrame enable (every frame) */
	writel(readl(0x002C) | 0x00000003L, MSM_HDMI_BASE + 0x002C);
}

void hdmi_app_clk_init(int on)
{
	uint32_t val = 0;
	if (on) {
		/* Enable hdmi apps clock */
		val = readl(MISC_CC2_REG);
		val = BIT(11);
		writel(val, MISC_CC2_REG);
		udelay(10);

		/* Enable hdmi master clock */
		val = readl(MMSS_AHB_EN_REG);
		val |= BIT(14);
		writel(val, MMSS_AHB_EN_REG);
		udelay(10);

		/* Enable hdmi slave clock */
		val = readl(MMSS_AHB_EN_REG);
		val |= BIT(4);
		writel(val, MMSS_AHB_EN_REG);
		udelay(10);
	} else {
		// Disable clocks
		val = readl(MISC_CC2_REG);
		val &= ~(BIT(11));
		writel(val, MISC_CC2_REG);
		udelay(10);
		val = readl(MMSS_AHB_EN_REG);
		val &= ~(BIT(14));
		writel(val, MMSS_AHB_EN_REG);
		udelay(10);
		val = readl(MMSS_AHB_EN_REG);
		val &= ~(BIT(4));
		writel(val, MMSS_AHB_EN_REG);
		udelay(10);
	}
}

int hdmi_msm_turn_on(void)
{
	uint32_t hotplug_control;

	hdmi_msm_set_mode(0);

	hdmi_msm_reset_core();	// Reset the core
	hdmi_msm_init_phy();

	// Enable USEC REF timer
	writel(0x0001001B, HDMI_USEC_REFTIMER);

	// Write 1 to HDMI_CTRL to enable HDMI
	hdmi_msm_set_mode(1);

	// Video setup for HDMI
	hdmi_video_setup();

	// AVI info setup
	hdmi_msm_avi_info_frame();

	return 0;
}

int hdmi_dtv_init()
{
	uint32_t hsync_period;
	uint32_t hsync_ctrl;
	uint32_t hsync_start_x;
	uint32_t hsync_end_x;
	uint32_t display_hctl;
	uint32_t vsync_period;
	uint32_t display_v_start;
	uint32_t display_v_end;
	uint32_t hsync_polarity;
	uint32_t vsync_polarity;
	uint32_t data_en_polarity;
	uint32_t ctrl_polarity;
	uint32_t dtv_border_clr = 0;
	uint32_t dtv_underflow_clr = 0;
	uint32_t active_v_start = 0;
	uint32_t active_v_end = 0;
	uint32_t dtv_hsync_skew = 0;
	uint32_t intf, stage, snum, mask, data;
	unsigned char *rgb_base;
	unsigned char *overlay_base;
	uint32_t val;

	struct hdmi_disp_mode_timing_type *timing =
			hdmi_common_init_panel_info();

	// MDP E config
	writel((unsigned)timing->base, MDP_BASE + 0xb0008);	//FB Address
	writel(((timing->height << 16) | timing->width), MDP_BASE + 0xb0004);
	writel((timing->width * timing->bpp / 8), MDP_BASE + 0xb000c);
	writel(0, MDP_BASE + 0xb0010);

	writel(DMA_PACK_PATTERN_RGB | DMA_DSTC0G_8BITS | DMA_DSTC1B_8BITS |
	       DMA_DSTC2R_8BITS, MDP_BASE + 0xb0000);
	writel(0xff0000, MDP_BASE + 0xb0070);
	writel(0xff0000, MDP_BASE + 0xb0074);
	writel(0xff0000, MDP_BASE + 0xb0078);

	// overlay rgb setup RGB2
	rgb_base = MDP_BASE + MDP4_RGB_BASE;
	rgb_base += (MDP4_RGB_OFF * 1);
	writel(((timing->height << 16) | timing->width), rgb_base + 0x0000);
	writel(0x0, rgb_base + 0x0004);
	writel(((timing->height << 16) | timing->width), rgb_base + 0x0008);
	writel(0x0, rgb_base + 0x000c);
	writel(timing->base, rgb_base + 0x0010);	//FB address
	writel((timing->width * timing->bpp / 8), rgb_base + 0x0040);
	writel(0x2443F, rgb_base + 0x0050);	//format
	writel(0x20001, rgb_base + 0x0054);	//pattern
	writel(0x0, rgb_base + 0x0058);
	writel(0x20000000, rgb_base + 0x005c);	//phaseX
	writel(0x20000000, rgb_base + 0x0060);	// phaseY

	// mdp4 mixer setup MDP4_MIXER1
	data = readl(MDP_BASE + 0x10100);
	stage = 9;
	snum = 12;
	mask = 0x0f;
	mask <<= snum;
	stage <<= snum;
	data &= ~mask;
	data |= stage;
	writel(data, MDP_BASE + 0x10100);	// Overlay CFG conf
	data = readl(MDP_BASE + 0x10100);

	// Overlay cfg
	overlay_base = MDP_BASE + MDP4_OVERLAYPROC1_BASE;

	writel(0x0, MDP_BASE + 0x0038);	//EXternal interface select

	data = ((timing->height << 16) | timing->width);
	writel(data, overlay_base + 0x0008);
	writel(timing->base, overlay_base + 0x000c);
	writel((timing->width * timing->bpp / 8), overlay_base + 0x0010);
	writel(0x10, overlay_base + 0x104);
	writel(0x10, overlay_base + 0x124);
	writel(0x10, overlay_base + 0x144);
	writel(0x01, overlay_base + 0x0004);	/* directout */

	hsync_period =
	    timing->hsync_width + timing->hsync_porch_bp + timing->width +
	    timing->hsync_porch_fp;
	hsync_ctrl = (hsync_period << 16) | timing->hsync_width;
	hsync_start_x = timing->hsync_width + timing->hsync_porch_bp;
	hsync_end_x = hsync_period - timing->hsync_porch_fp - 1;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;

	vsync_period =
	    (timing->vsync_width + timing->vsync_porch_bp + timing->height +
	     timing->vsync_porch_fp) * hsync_period;
	display_v_start =
	    (timing->vsync_width + timing->vsync_porch_bp) * hsync_period;
	display_v_end =
	    vsync_period - (timing->vsync_porch_bp * hsync_period) - 1;

	dtv_underflow_clr |= 0x80000000;
	hsync_polarity     = 0;
	vsync_polarity     = 0;
	data_en_polarity   = 0;
	ctrl_polarity      =
	    (data_en_polarity << 2) | (vsync_polarity << 1) | (hsync_polarity);

	writel(hsync_ctrl,      MDP_BASE + DTV_BASE + 0x4);
	writel(vsync_period,    MDP_BASE + DTV_BASE + 0x8);
	writel(timing->vsync_width * hsync_period,
	                        MDP_BASE + DTV_BASE + 0xc);
	writel(display_hctl,    MDP_BASE + DTV_BASE + 0x18);
	writel(display_v_start, MDP_BASE + DTV_BASE + 0x1c);
	writel(0x25a197,        MDP_BASE + DTV_BASE + 0x20);
	writel(dtv_border_clr,  MDP_BASE + DTV_BASE + 0x40);
	writel(0x8fffffff,      MDP_BASE + DTV_BASE + 0x44);
	writel(dtv_hsync_skew,  MDP_BASE + DTV_BASE + 0x48);
	writel(ctrl_polarity,   MDP_BASE + DTV_BASE + 0x50);
	writel(0x0,             MDP_BASE + DTV_BASE + 0x2c);
	writel(active_v_start,  MDP_BASE + DTV_BASE + 0x30);
	writel(active_v_end,    MDP_BASE + DTV_BASE + 0x38);

	/* Enable DTV block */
	writel(0x01, MDP_BASE + DTV_BASE);

	/* Flush mixer/pipes configurations */
	val = BIT(1);
	val |= BIT(5);
	writel(val, MDP_BASE + 0x18000);

	return 0;
}

void hdmi_display_shutdown()
{
	writel(0x0, MDP_BASE + DTV_BASE);
	writel(0x8, MDP_BASE + 0x0038);
	writel(0x0, MDP_BASE + 0x10100);
}
