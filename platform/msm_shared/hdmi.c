/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
#include <platform/timer.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/scm-io.h>

#define MDP4_OVERLAYPROC1_BASE  0x18000
#define MDP4_RGB_BASE           0x40000
#define MDP4_RGB_OFF            0x10000

struct hdmi_disp_mode_timing_type hdmi_timing_default = {
	.height = 1080,
	.hsync_porch_fp = 88,
	.hsync_width = 44,
	.hsync_porch_bp = 148,
	.width = 1920,
	.vsync_porch_fp = 4,
	.vsync_width = 5,
	.vsync_porch_bp = 36,
};

static void hdmi_msm_set_mode(int on)
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

void hdmi_app_clk_init(int on)
{
	uint32_t val = 0;
	if (on) {
		// Enable clocks
		val = secure_readl(MISC_CC2_REG);
		val |= BIT(11);
		secure_writel(val, MISC_CC2_REG);
		udelay(10);
		val = secure_readl(MMSS_AHB_EN_REG);
		val |= BIT(14);
		secure_writel(val, MMSS_AHB_EN_REG);
		udelay(10);
		val = secure_readl(MMSS_AHB_EN_REG);
		val |= BIT(4);
		secure_writel(val, MMSS_AHB_EN_REG);
		udelay(10);
	} else {
		// Disable clocks
		val = secure_readl(MISC_CC2_REG);
		val &= ~(BIT(11));
		secure_writel(val, MISC_CC2_REG);
		udelay(10);
		val = secure_readl(MMSS_AHB_EN_REG);
		val &= ~(BIT(14));
		secure_writel(val, MMSS_AHB_EN_REG);
		udelay(10);
		val = secure_readl(MMSS_AHB_EN_REG);
		val &= ~(BIT(4));
		secure_writel(val, MMSS_AHB_EN_REG);
		udelay(10);
	}
}

static void hdmi_msm_reset_core()
{
	uint32_t reg_val = 0;
	hdmi_msm_set_mode(0);
	// Disable clocks
	hdmi_app_clk_init(0);
	udelay(5);
	// Enable clocks
	hdmi_app_clk_init(1);

	reg_val = secure_readl(SW_RESET_CORE_REG);
	reg_val |= BIT(11);
	secure_writel(reg_val, SW_RESET_CORE_REG);
	udelay(5);
	reg_val = secure_readl(SW_RESET_AHB_REG);
	reg_val |= BIT(9);
	secure_writel(reg_val, SW_RESET_AHB_REG);
	udelay(5);
	reg_val = secure_readl(SW_RESET_AHB_REG);
	reg_val |= BIT(9);
	secure_writel(reg_val, SW_RESET_AHB_REG);
	udelay(20);
	reg_val = secure_readl(SW_RESET_CORE_REG);
	reg_val &= ~(BIT(11));
	secure_writel(reg_val, SW_RESET_CORE_REG);
	udelay(5);
	reg_val = secure_readl(SW_RESET_AHB_REG);
	reg_val &= ~(BIT(9));
	secure_writel(reg_val, SW_RESET_AHB_REG);
	udelay(5);
	reg_val = secure_readl(SW_RESET_AHB_REG);
	reg_val &= ~(BIT(9));
	secure_writel(reg_val, SW_RESET_AHB_REG);
	udelay(5);
}

void hdmi_msm_turn_on(void)
{
	uint32_t hotplug_control;
	hdmi_msm_reset_core();	// Reset the core
	hdmi_msm_init_phy();
	// Enable USEC REF timer
	writel(0x0001001B, HDMI_USEC_REFTIMER);
	// Video setup for HDMI
	hdmi_video_setup();
	// Write 1 to HDMI_CTRL to enable HDMI
	hdmi_msm_set_mode(1);
	dprintf(SPEW, "HDMI Core is: %s\n",
		(readl(HDMI_CTRL) & 0x00000001) ? "on" : "off");
}

struct fbcon_config *hdmi_dtv_init(struct hdmi_disp_mode_timing_type *timing)
{
	uint32_t dtv_width;
	uint32_t dtv_height;
	uint32_t dtv_bpp;
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

	dprintf(SPEW, "In DTV on function\n");

	// Turn on all the clocks
	dtv_on();

	dtv_width = timing->width;
	dtv_height = timing->height;
	dtv_bpp = fb_cfg.bpp;
	fb_cfg.base = FB_ADDR;

	// MDP E config
	writel((unsigned)fb_cfg.base, MDP_BASE + 0xb0008);	//FB Address

	writel(((fb_cfg.height << 16) | fb_cfg.width), MDP_BASE + 0xb0004);
	writel((fb_cfg.width * fb_cfg.bpp / 8), MDP_BASE + 0xb000c);
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
	writel(0x0, rgb_base + 0x0008);
	writel(0x0, rgb_base + 0x000c);
	writel(fb_cfg.base, rgb_base + 0x0010);	//FB address
	writel((fb_cfg.width * fb_cfg.bpp / 8), rgb_base + 0x0040);
	writel(0x24216, rgb_base + 0x0050);	//format
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
	writel(fb_cfg.base, overlay_base + 0x000c);
	writel((fb_cfg.width * fb_cfg.bpp / 8), overlay_base + 0x0010);
	writel(0x10, overlay_base + 0x104);
	writel(0x10, overlay_base + 0x124);
	writel(0x10, overlay_base + 0x144);
	writel(0x1, overlay_base + 0x0004);	/* directout */

	hsync_period =
	    timing->hsync_width + timing->hsync_porch_bp + dtv_width +
	    timing->hsync_porch_fp;
	hsync_ctrl = (hsync_period << 16) | timing->hsync_width;
	hsync_start_x = timing->hsync_width + timing->hsync_porch_bp;
	hsync_end_x = hsync_period - timing->hsync_porch_fp - 1;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;

	vsync_period =
	    (timing->vsync_width + timing->vsync_porch_bp + dtv_height +
	     timing->vsync_porch_fp) * hsync_period;
	display_v_start =
	    (timing->vsync_width + timing->vsync_porch_bp) * hsync_period;
	display_v_end =
	    vsync_period - (timing->vsync_porch_bp * hsync_period) - 1;

	dtv_underflow_clr |= 0x80000000;
	hsync_polarity = 0;
	vsync_polarity = 0;
	data_en_polarity = 0;
	ctrl_polarity =
	    (data_en_polarity << 2) | (vsync_polarity << 1) | (hsync_polarity);

	writel(hsync_ctrl, MDP_BASE + DTV_BASE + 0x4);
	writel(vsync_period, MDP_BASE + DTV_BASE + 0x8);
	writel(timing->vsync_width * hsync_period, MDP_BASE + DTV_BASE + 0xc);
	writel(display_hctl, MDP_BASE + DTV_BASE + 0x18);
	writel(display_v_start, MDP_BASE + DTV_BASE + 0x1c);
	writel(0x25a197, MDP_BASE + DTV_BASE + 0x20);
	writel(dtv_border_clr, MDP_BASE + DTV_BASE + 0x40);
	writel(0x8fffffff, MDP_BASE + DTV_BASE + 0x44);
	writel(dtv_hsync_skew, MDP_BASE + DTV_BASE + 0x48);
	writel(ctrl_polarity, MDP_BASE + DTV_BASE + 0x50);
	writel(0, MDP_BASE + DTV_BASE + 0x2c);
	writel(active_v_start, MDP_BASE + DTV_BASE + 0x30);
	writel(active_v_end, MDP_BASE + DTV_BASE + 0x38);

	/*
	 * Keep Display solid color, for future debugging
	 */
# if 0
	writel(0xffffffff, 0x05151008);
	val = readl(MDP_BASE + 0x50050);
	val |= BIT(22);
	writel(val, MDP_BASE + 0x50050);
	writel(BIT(5), MDP_BASE + 0x18000);
#endif
	/* Enable DTV block */
	writel(0x1, MDP_BASE + DTV_BASE);
	val = BIT(1);
	val |= BIT(5);
	writel(val, MDP_BASE + 0x18000);
	return &fb_cfg;
}

void hdmi_display_shutdown()
{
	writel(0x0, MDP_BASE + DTV_BASE);
	writel(0x8, MDP_BASE + 0x0038);
	writel(0x0, MDP_BASE + 0x10100);
}
