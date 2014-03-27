/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of The Linux Foundation nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <mdp5.h>
#include <debug.h>
#include <reg.h>
#include <target/display.h>
#include <platform/timer.h>
#include <platform/iomap.h>
#include <dev/lcdc.h>
#include <dev/fbcon.h>
#include <bits.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <err.h>
#include <clock.h>
#include <scm.h>

int restore_secure_cfg(uint32_t id);

static int mdp_rev;

void mdp_set_revision(int rev)
{
	mdp_rev = rev;
}

int mdp_get_revision()
{
	return mdp_rev;
}

uint32_t mdss_mdp_intf_offset()
{
	uint32_t mdss_mdp_intf_off;
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);

	if (mdss_mdp_rev >= MDSS_MDP_HW_REV_102)
		mdss_mdp_intf_off = 0;
	else
		mdss_mdp_intf_off = 0xEC00;

	return mdss_mdp_intf_off;
}

void mdp_clk_gating_ctrl(void)
{
	writel(0x40000000, MDP_CLK_CTRL0);
	udelay(20);
	writel(0x40000040, MDP_CLK_CTRL0);
	writel(0x40000000, MDP_CLK_CTRL1);
	writel(0x00400000, MDP_CLK_CTRL3);
	udelay(20);
	writel(0x00404000, MDP_CLK_CTRL3);
	writel(0x40000000, MDP_CLK_CTRL4);
}

static void mdss_source_pipe_config(struct fbcon_config *fb, struct msm_panel_info
		*pinfo, uint32_t pipe_base)
{
	uint32_t src_size, out_size, stride, pipe_swap;
	uint32_t fb_off = 0;

	/* write active region size*/
	src_size = (fb->height << 16) + fb->width;
	out_size = src_size;

	if (pinfo->lcdc.dual_pipe) {
		out_size = (fb->height << 16) + (fb->width / 2);
		pipe_swap = (pinfo->lcdc.pipe_swap == TRUE) ? 1 : 0;

		if (pipe_swap && ((pipe_base == MDP_VP_0_RGB_0_BASE) ||
				(pipe_base == MDP_VP_0_DMA_0_BASE)))
			fb_off = (pinfo->xres / 2);
		else if (!pipe_swap && ((pipe_base == MDP_VP_0_RGB_1_BASE) ||
				(pipe_base == MDP_VP_0_DMA_1_BASE)))
			fb_off = (pinfo->xres / 2);
	}

	stride = (fb->stride * fb->bpp/8);

	writel(fb->base, pipe_base + PIPE_SSPP_SRC0_ADDR);
	writel(stride, pipe_base + PIPE_SSPP_SRC_YSTRIDE);
	writel(src_size, pipe_base + PIPE_SSPP_SRC_IMG_SIZE);
	writel(out_size, pipe_base + PIPE_SSPP_SRC_SIZE);
	writel(out_size, pipe_base + PIPE_SSPP_SRC_OUT_SIZE);
	writel(fb_off, pipe_base + PIPE_SSPP_SRC_XY);
	writel(0x00, pipe_base + PIPE_SSPP_OUT_XY);

	/* Tight Packing 3bpp 0-Alpha 8-bit R B G */
	writel(0x0002243F, pipe_base + PIPE_SSPP_SRC_FORMAT);
	writel(0x00020001, pipe_base + PIPE_SSPP_SRC_UNPACK_PATTERN);
	writel(0x00, pipe_base + PIPE_SSPP_SRC_OP_MODE);
}

static void mdss_vbif_setup()
{
	int access_secure = restore_secure_cfg(SECURE_DEVICE_MDSS);
	uint32_t mdp_hw_rev = readl(MDP_HW_REV);

	/* TZ returns an errornous ret val even if the VBIF registers were
	 * successfully unlocked. Ignore TZ return value till it's fixed */
	if (!access_secure || 1) {
		dprintf(SPEW, "MDSS VBIF registers unlocked by TZ.\n");

		/* Force VBIF Clocks on */
		if (mdp_hw_rev < MDSS_MDP_HW_REV_103)
			writel(0x1, VBIF_VBIF_DDR_FORCE_CLK_ON);

		/*
		 * Following configuration is needed because on some versions,
		 * recommended reset values are not stored.
		 */
		if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_100)) {
			writel(0x00000707, VBIF_VBIF_DDR_OUT_MAX_BURST);
			writel(0x00000030, VBIF_VBIF_DDR_ARB_CTRL );
			writel(0x00000001, VBIF_VBIF_DDR_RND_RBN_QOS_ARB);
			writel(0x00000FFF, VBIF_VBIF_DDR_OUT_AOOO_AXI_EN);
			writel(0x0FFF0FFF, VBIF_VBIF_DDR_OUT_AX_AOOO);
			writel(0x22222222, VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0);
			writel(0x00002222, VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1);
		} else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_101)) {
			writel(0x00000707, VBIF_VBIF_DDR_OUT_MAX_BURST);
			writel(0x00000003, VBIF_VBIF_DDR_ARB_CTRL);
		}
	}
}

static uint32_t mdss_smp_alloc(uint32_t client_id, uint32_t smp_cnt,
	uint32_t fixed_smp_cnt, uint32_t free_smp_offset)
{
	uint32_t i, j;
	uint32_t reg_val = 0;

	for (i = fixed_smp_cnt, j = 0; i < smp_cnt; i++) {
		/* max 3 MMB per register */
		reg_val |= client_id << (((j++) % 3) * 8);
		if ((j % 3) == 0) {
			writel(reg_val, MMSS_MDP_SMP_ALLOC_W_BASE +
				free_smp_offset);
			writel(reg_val, MMSS_MDP_SMP_ALLOC_R_BASE +
				free_smp_offset);
			reg_val = 0;
			free_smp_offset += 4;
		}
	}

	if (j % 3) {
		writel(reg_val, MMSS_MDP_SMP_ALLOC_W_BASE + free_smp_offset);
		writel(reg_val, MMSS_MDP_SMP_ALLOC_R_BASE + free_smp_offset);
		free_smp_offset += 4;
	}

	return free_smp_offset;
}

static void mdss_smp_setup(struct msm_panel_info *pinfo, uint32_t left_pipe,
		uint32_t right_pipe)

{
	uint32_t left_sspp_client_id, right_sspp_client_id;
	uint32_t bpp = 3, free_smp_offset = 0, xres = MDSS_MAX_LINE_BUF_WIDTH;
	uint32_t smp_cnt, smp_size = 4096, fixed_smp_cnt = 0;
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);

	if ((mdss_mdp_rev >= MDSS_MDP_HW_REV_103) &&
		(mdss_mdp_rev < MDSS_MDP_HW_REV_200)) {
		smp_size = 8192;
		fixed_smp_cnt = 2;
		free_smp_offset = 0xC;
	}

	if (pinfo->use_dma_pipe)
		right_sspp_client_id = 0xD; /* 13 */
	else
		right_sspp_client_id = 0x11; /* 17 */

	if (MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_101))
		left_sspp_client_id = (pinfo->use_dma_pipe) ? 0x4 : 0x07; /* 4 or 7 */
	else
		left_sspp_client_id = (pinfo->use_dma_pipe) ? 0xA : 0x10; /* 10 or 16 */

	/* Each pipe driving half the screen */
	if (pinfo->lcdc.dual_pipe)
		xres /= 2;

	/* bpp = bytes per pixel of input image */
	smp_cnt = (xres * bpp * 2) + smp_size - 1;
	smp_cnt /= smp_size;

	if (smp_cnt > 4) {
		dprintf(CRITICAL, "ERROR: %s: Out of SMP's, cnt=%d! \n", __func__,
				smp_cnt);
		ASSERT(0); /* Max 4 SMPs can be allocated per client */
	}

	writel(smp_cnt * 0x40, left_pipe + REQPRIORITY_FIFO_WATERMARK0);
	writel(smp_cnt * 0x80, left_pipe + REQPRIORITY_FIFO_WATERMARK1);
	writel(smp_cnt * 0xc0, left_pipe + REQPRIORITY_FIFO_WATERMARK2);

	if (pinfo->lcdc.dual_pipe) {
		writel(smp_cnt * 0x40, right_pipe + REQPRIORITY_FIFO_WATERMARK0);
		writel(smp_cnt * 0x80, right_pipe + REQPRIORITY_FIFO_WATERMARK1);
		writel(smp_cnt * 0xc0, right_pipe + REQPRIORITY_FIFO_WATERMARK2);
	}

	free_smp_offset = mdss_smp_alloc(left_sspp_client_id, smp_cnt,
		fixed_smp_cnt, free_smp_offset);
	if (pinfo->lcdc.dual_pipe)
		mdss_smp_alloc(right_sspp_client_id, smp_cnt, fixed_smp_cnt,
			free_smp_offset);
}

void mdss_intf_tg_setup(struct msm_panel_info *pinfo, uint32_t intf_base)
{
	uint32_t hsync_period, vsync_period;
	uint32_t hsync_start_x, hsync_end_x;
	uint32_t display_hctl, active_hctl, hsync_ctl, display_vstart, display_vend;
	uint32_t mdss_mdp_intf_off;
	uint32_t adjust_xres = 0;

	struct lcdc_panel_info *lcdc = NULL;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	adjust_xres = pinfo->xres;
	if (pinfo->lcdc.split_display) {
		adjust_xres /= 2;
		if (intf_base == MDP_INTF_1_BASE) {
			writel(BIT(8), MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL);
			writel(0x0, MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL);
			writel(0x1, MDP_REG_SPLIT_DISPLAY_EN);
		}
	}

	mdss_mdp_intf_off = intf_base + mdss_mdp_intf_offset();

	hsync_period = lcdc->h_pulse_width +
		lcdc->h_back_porch +
		adjust_xres + lcdc->xres_pad + lcdc->h_front_porch;
	vsync_period = (lcdc->v_pulse_width +
			lcdc->v_back_porch +
			pinfo->yres + lcdc->yres_pad +
			lcdc->v_front_porch);

	hsync_start_x =
		lcdc->h_pulse_width +
		lcdc->h_back_porch;
	hsync_end_x =
		hsync_period - lcdc->h_front_porch - 1;

	display_vstart = (lcdc->v_pulse_width +
			lcdc->v_back_porch)
		* hsync_period + lcdc->hsync_skew;
	display_vend = ((vsync_period - lcdc->v_front_porch) * hsync_period)
		+lcdc->hsync_skew - 1;

	if (intf_base == MDP_INTF_0_BASE) { /* eDP */
		display_vstart += lcdc->h_pulse_width + lcdc->h_back_porch;
		display_vend -= lcdc->h_front_porch;
	}

	hsync_ctl = (hsync_period << 16) | lcdc->h_pulse_width;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;

	writel(hsync_ctl, MDP_HSYNC_CTL + mdss_mdp_intf_off);
	writel(vsync_period*hsync_period, MDP_VSYNC_PERIOD_F0 +
			mdss_mdp_intf_off);
	writel(0x00, MDP_VSYNC_PERIOD_F1 + mdss_mdp_intf_off);
	writel(lcdc->v_pulse_width*hsync_period,
			MDP_VSYNC_PULSE_WIDTH_F0 +
			mdss_mdp_intf_off);
	writel(0x00, MDP_VSYNC_PULSE_WIDTH_F1 + mdss_mdp_intf_off);
	writel(display_hctl, MDP_DISPLAY_HCTL + mdss_mdp_intf_off);
	writel(display_vstart, MDP_DISPLAY_V_START_F0 +
			mdss_mdp_intf_off);
	writel(0x00, MDP_DISPLAY_V_START_F1 + mdss_mdp_intf_off);
	writel(display_vend, MDP_DISPLAY_V_END_F0 +
			mdss_mdp_intf_off);
	writel(0x00, MDP_DISPLAY_V_END_F1 + mdss_mdp_intf_off);
	writel(0x00, MDP_ACTIVE_HCTL + mdss_mdp_intf_off);
	writel(0x00, MDP_ACTIVE_V_START_F0 + mdss_mdp_intf_off);
	writel(0x00, MDP_ACTIVE_V_START_F1 + mdss_mdp_intf_off);
	writel(0x00, MDP_ACTIVE_V_END_F0 + mdss_mdp_intf_off);
	writel(0x00, MDP_ACTIVE_V_END_F1 + mdss_mdp_intf_off);
	writel(0xFF, MDP_UNDERFFLOW_COLOR + mdss_mdp_intf_off);

	if (intf_base == MDP_INTF_0_BASE) /* eDP */
		writel(0x212A, MDP_PANEL_FORMAT + mdss_mdp_intf_off);
	else
		writel(0x213F, MDP_PANEL_FORMAT + mdss_mdp_intf_off);
}

void mdss_layer_mixer_setup(struct fbcon_config *fb, struct msm_panel_info
		*pinfo)
{
	uint32_t mdp_rgb_size, height, width, val;

	height = fb->height;
	width = fb->width;

	if (pinfo->lcdc.dual_pipe)
		width /= 2;

	/* write active region size*/
	mdp_rgb_size = (height << 16) | width;

	writel(mdp_rgb_size, MDP_VP_0_MIXER_0_BASE + LAYER_0_OUT_SIZE);
	writel(0x00, MDP_VP_0_MIXER_0_BASE + LAYER_0_OP_MODE);
	writel(0x100, MDP_VP_0_MIXER_0_BASE + LAYER_0_BLEND_OP);
	writel(0xFF, MDP_VP_0_MIXER_0_BASE + LAYER_0_BLEND0_FG_ALPHA);
	writel(0x100, MDP_VP_0_MIXER_0_BASE + LAYER_1_BLEND_OP);
	writel(0xFF, MDP_VP_0_MIXER_0_BASE + LAYER_1_BLEND0_FG_ALPHA);
	writel(0x100, MDP_VP_0_MIXER_0_BASE + LAYER_2_BLEND_OP);
	writel(0xFF, MDP_VP_0_MIXER_0_BASE + LAYER_2_BLEND0_FG_ALPHA);
	writel(0x100, MDP_VP_0_MIXER_0_BASE + LAYER_3_BLEND_OP);
	writel(0xFF, MDP_VP_0_MIXER_0_BASE + LAYER_3_BLEND0_FG_ALPHA);

	/* Baselayer for layer mixer 0 */
	if (pinfo->use_dma_pipe)
		writel(0x0040000, MDP_CTL_0_BASE + CTL_LAYER_0);
	else
		writel(0x0000200, MDP_CTL_0_BASE + CTL_LAYER_0);

	if (pinfo->lcdc.dual_pipe) {
		writel(mdp_rgb_size, MDP_VP_0_MIXER_1_BASE + LAYER_0_OUT_SIZE);
		writel(0x00, MDP_VP_0_MIXER_1_BASE + LAYER_0_OP_MODE);
		writel(0x100, MDP_VP_0_MIXER_1_BASE + LAYER_0_BLEND_OP);
		writel(0xFF, MDP_VP_0_MIXER_1_BASE + LAYER_0_BLEND0_FG_ALPHA);
		writel(0x100, MDP_VP_0_MIXER_1_BASE + LAYER_1_BLEND_OP);
		writel(0xFF, MDP_VP_0_MIXER_1_BASE + LAYER_1_BLEND0_FG_ALPHA);
		writel(0x100, MDP_VP_0_MIXER_1_BASE + LAYER_2_BLEND_OP);
		writel(0xFF, MDP_VP_0_MIXER_1_BASE + LAYER_2_BLEND0_FG_ALPHA);
		writel(0x100, MDP_VP_0_MIXER_1_BASE + LAYER_3_BLEND_OP);
		writel(0xFF, MDP_VP_0_MIXER_1_BASE + LAYER_3_BLEND0_FG_ALPHA);

		/* Baselayer for layer mixer 1 */
		val = pinfo->use_dma_pipe ? 0x200000 : 0x1000;
		if (pinfo->lcdc.split_display)
			writel(val, MDP_CTL_1_BASE + CTL_LAYER_1);
		else
			writel(val, MDP_CTL_0_BASE + CTL_LAYER_1);
	}
}

void mdss_qos_remapper_setup(void)
{
	uint32_t mdp_hw_rev = readl(MDP_HW_REV);
	uint32_t map;

	if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_100) ||
		MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
						MDSS_MDP_HW_REV_102))
		map = 0xE9;
	else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
						MDSS_MDP_HW_REV_101))
		map = 0xA5;
	else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
						MDSS_MDP_HW_REV_103))
		map = 0xFA;
	else
		return;

	writel(map, MDP_QOS_REMAPPER_CLASS_0);
}

int mdp_dsi_video_config(struct msm_panel_info *pinfo,
		struct fbcon_config *fb)
{
	int ret = NO_ERROR;
	struct lcdc_panel_info *lcdc = NULL;
	uint32_t intf_sel = 0x100;
	uint32_t left_pipe, right_pipe;

	mdss_intf_tg_setup(pinfo, MDP_INTF_1_BASE);

	if (pinfo->mipi.dual_dsi)
		mdss_intf_tg_setup(pinfo, MDP_INTF_2_BASE);

	mdp_clk_gating_ctrl();

	if (pinfo->use_dma_pipe) {
		left_pipe = MDP_VP_0_DMA_0_BASE;
		right_pipe = MDP_VP_0_DMA_1_BASE;
	} else {
		left_pipe = MDP_VP_0_RGB_0_BASE;
		right_pipe = MDP_VP_0_RGB_1_BASE;
	}

	mdss_vbif_setup();
	mdss_smp_setup(pinfo, left_pipe, right_pipe);

	mdss_qos_remapper_setup();

	mdss_source_pipe_config(fb, pinfo, left_pipe);

	if (pinfo->lcdc.dual_pipe)
		mdss_source_pipe_config(fb, pinfo, right_pipe);

	mdss_layer_mixer_setup(fb, pinfo);

	writel(0x1F20, MDP_CTL_0_BASE + CTL_TOP);

	if (pinfo->mipi.dual_dsi) {
		writel(0x1F30, MDP_CTL_1_BASE + CTL_TOP);
		intf_sel |= BIT(16); /* INTF 2 enable */
	}

	writel(intf_sel, MDP_DISP_INTF_SEL);

	writel(0x1111, MDP_VIDEO_INTF_UNDERFLOW_CTL);
	writel(0x01, MDP_UPPER_NEW_ROI_PRIOR_RO_START);
	writel(0x01, MDP_LOWER_NEW_ROI_PRIOR_TO_START);

	return 0;
}

int mdp_edp_config(struct msm_panel_info *pinfo, struct fbcon_config *fb)
{
	int ret = NO_ERROR;
	struct lcdc_panel_info *lcdc = NULL;
	uint32_t left_pipe, right_pipe;

	mdss_intf_tg_setup(pinfo, MDP_INTF_0_BASE);

	if (pinfo->use_dma_pipe) {
		left_pipe = MDP_VP_0_DMA_0_BASE;
		right_pipe = MDP_VP_0_DMA_1_BASE;
	} else {
		left_pipe = MDP_VP_0_RGB_0_BASE;
		right_pipe = MDP_VP_0_RGB_1_BASE;
	}

	mdp_clk_gating_ctrl();

	mdss_vbif_setup();
	mdss_smp_setup(pinfo, left_pipe, right_pipe);

	mdss_qos_remapper_setup();

	mdss_source_pipe_config(fb, pinfo, left_pipe);
	if (pinfo->lcdc.dual_pipe)
		mdss_source_pipe_config(fb, pinfo, right_pipe);

	mdss_layer_mixer_setup(fb, pinfo);

	if (pinfo->lcdc.dual_pipe)
		writel(0x181F10, MDP_CTL_0_BASE + CTL_TOP);
	else
		writel(0x1F10, MDP_CTL_0_BASE + CTL_TOP);

	writel(0x9, MDP_DISP_INTF_SEL);
	writel(0x1111, MDP_VIDEO_INTF_UNDERFLOW_CTL);
	writel(0x01, MDP_UPPER_NEW_ROI_PRIOR_RO_START);
	writel(0x01, MDP_LOWER_NEW_ROI_PRIOR_TO_START);

	return 0;
}

int mdp_dsi_cmd_config(struct msm_panel_info *pinfo,
                struct fbcon_config *fb)
{
	uint32_t intf_sel = BIT(8);
	int ret = NO_ERROR;
	uint32_t left_pipe, right_pipe;

	struct lcdc_panel_info *lcdc = NULL;
	uint32_t mdss_mdp_intf_off = 0;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	if (pinfo->lcdc.split_display) {
		writel(0x102, MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL);
		writel(0x2, MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL);
		writel(0x1, MDP_REG_SPLIT_DISPLAY_EN);
	}

	mdss_mdp_intf_off = mdss_mdp_intf_offset();

	mdp_clk_gating_ctrl();

	if (pinfo->mipi.dual_dsi)
		intf_sel |= BIT(16); /* INTF 2 enable */

	writel(intf_sel, MDP_DISP_INTF_SEL);

	if (pinfo->use_dma_pipe) {
		left_pipe = MDP_VP_0_DMA_0_BASE;
		right_pipe = MDP_VP_0_DMA_1_BASE;
	} else {
		left_pipe = MDP_VP_0_RGB_0_BASE;
		right_pipe = MDP_VP_0_RGB_1_BASE;
	}

	mdss_vbif_setup();
	mdss_smp_setup(pinfo, left_pipe, right_pipe);
	mdss_qos_remapper_setup();

	mdss_source_pipe_config(fb, pinfo, left_pipe);

	if (pinfo->lcdc.dual_pipe)
		mdss_source_pipe_config(fb, pinfo, right_pipe);

	mdss_layer_mixer_setup(fb, pinfo);

	writel(0x213F, MDP_INTF_1_BASE + MDP_PANEL_FORMAT + mdss_mdp_intf_off);
	writel(0x21f20, MDP_CTL_0_BASE + CTL_TOP);

	if (pinfo->mipi.dual_dsi) {
		writel(0x213F, MDP_INTF_2_BASE + MDP_PANEL_FORMAT + mdss_mdp_intf_off);
		writel(0x21F30, MDP_CTL_1_BASE + CTL_TOP);
	}

	return ret;
}

int mdp_dsi_video_on(struct msm_panel_info *pinfo)
{
	if (pinfo->use_dma_pipe) {
		writel(0x22840, MDP_CTL_0_BASE + CTL_FLUSH);
		writel(0x25080, MDP_CTL_1_BASE + CTL_FLUSH);
	} else {
		writel(0x22048, MDP_CTL_0_BASE + CTL_FLUSH);
		writel(0x24090, MDP_CTL_1_BASE + CTL_FLUSH);
	}
	writel(0x01, MDP_INTF_1_TIMING_ENGINE_EN  + mdss_mdp_intf_offset());

	return NO_ERROR;
}

int mdp_dsi_video_off()
{
	if(!target_cont_splash_screen())
	{
		writel(0x00000000, MDP_INTF_1_TIMING_ENGINE_EN +
				mdss_mdp_intf_offset());
		mdelay(60);
		/* Ping-Pong done Tear Check Read/Write  */
		/* Underrun(Interface 0/1/2/3) VSYNC Interrupt Enable  */
		writel(0xFF777713, MDP_INTR_CLEAR);
	}

	writel(0x00000000, MDP_INTR_EN);

	return NO_ERROR;
}

int mdp_dsi_cmd_off()
{
	if(!target_cont_splash_screen())
	{
		/* Ping-Pong done Tear Check Read/Write  */
		/* Underrun(Interface 0/1/2/3) VSYNC Interrupt Enable  */
		writel(0xFF777713, MDP_INTR_CLEAR);
	}
	writel(0x00000000, MDP_INTR_EN);

	return NO_ERROR;
}

int mdp_dma_on(struct msm_panel_info *pinfo)
{
	if (pinfo->use_dma_pipe) {
		writel(0x22840, MDP_CTL_0_BASE + CTL_FLUSH);
		writel(0x25080, MDP_CTL_1_BASE + CTL_FLUSH);
	} else {
		writel(0x22048, MDP_CTL_0_BASE + CTL_FLUSH);
		writel(0x24090, MDP_CTL_1_BASE + CTL_FLUSH);
	}
	writel(0x01, MDP_CTL_0_BASE + CTL_START);
	return NO_ERROR;
}

void mdp_disable(void)
{

}

int mdp_edp_on(struct msm_panel_info *pinfo)
{
	if (pinfo->use_dma_pipe)
		writel(0x22840, MDP_CTL_0_BASE + CTL_FLUSH);
	else
		writel(0x22048, MDP_CTL_0_BASE + CTL_FLUSH);
	writel(0x01, MDP_INTF_0_TIMING_ENGINE_EN  + mdss_mdp_intf_offset());
	return NO_ERROR;
}

int mdp_edp_off(void)
{
	if (!target_cont_splash_screen()) {

		writel(0x00000000, MDP_INTF_0_TIMING_ENGINE_EN +
				mdss_mdp_intf_offset());
		mdelay(60);
		/* Ping-Pong done Tear Check Read/Write  */
		/* Underrun(Interface 0/1/2/3) VSYNC Interrupt Enable  */
		writel(0xFF777713, MDP_INTR_CLEAR);
		writel(0x00000000, MDP_INTR_EN);
	}

	writel(0x00000000, MDP_INTR_EN);

	return NO_ERROR;
}
