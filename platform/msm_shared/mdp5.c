/* Copyright (c) 2012-2016, The Linux Foundation. All rights reserved.
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
#include <arch/defines.h>

#define MDSS_MDP_MAX_PREFILL_FETCH	25

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

static inline bool is_software_pixel_ext_config_needed()
{
	return (MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_107) || MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_114) || MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_116) || MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_115));
}

static inline bool has_fixed_size_smp()
{
	return (MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_107) || MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_114) || MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_116) || MDSS_IS_MAJOR_MINOR_MATCHING(readl(MDP_HW_REV),
		MDSS_MDP_HW_REV_115));
}

uint32_t mdss_mdp_intf_offset()
{
	uint32_t mdss_mdp_intf_off;
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);

	if ((mdss_mdp_rev == MDSS_MDP_HW_REV_106) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_108) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_111) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_112) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_114) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_116) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_115))
		mdss_mdp_intf_off = 0x59100;
	else if (mdss_mdp_rev >= MDSS_MDP_HW_REV_102)
		mdss_mdp_intf_off = 0;
	else
		mdss_mdp_intf_off = 0xEC00;

	return mdss_mdp_intf_off;
}

static uint32_t mdss_mdp_get_ppb_offset()
{
	uint32_t mdss_mdp_ppb_off = 0;
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);

	/* return MMSS_MDP_PPB0_CONFIG offset from MDSS base */
	if ((mdss_mdp_rev == MDSS_MDP_HW_REV_108) ||
	    (mdss_mdp_rev == MDSS_MDP_HW_REV_111))
		mdss_mdp_ppb_off = 0x1420;
	else if (mdss_mdp_rev == MDSS_MDP_HW_REV_110)
		mdss_mdp_ppb_off = 0x1334;
	else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_107))
		mdss_mdp_ppb_off = 0x1330;
	else
		dprintf(CRITICAL,"Invalid PPB0_CONFIG offset\n");

	return mdss_mdp_ppb_off;
}

static uint32_t mdss_mdp_vbif_qos_remap_get_offset()
{
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);

	if ((mdss_mdp_rev == MDSS_MDP_HW_REV_110) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_111) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_114) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_115) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_116))
		return 0xB0020;
	else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_107))
		return 0xB0000;
	else
		return 0xC8020;
}

void mdp_clk_gating_ctrl(void)
{
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);
	if (MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_107))
		return;

	writel(0x40000000, MDP_CLK_CTRL0);
	udelay(20);
	writel(0x40000040, MDP_CLK_CTRL0);
	writel(0x40000000, MDP_CLK_CTRL1);
	writel(0x00400000, MDP_CLK_CTRL3);
	udelay(20);
	writel(0x00404000, MDP_CLK_CTRL3);
	writel(0x40000000, MDP_CLK_CTRL4);
}

static void mdp_select_pipe_type(struct msm_panel_info *pinfo,
				uint32_t *left_pipe, uint32_t *right_pipe)
{
	switch (pinfo->pipe_type) {
		case MDSS_MDP_PIPE_TYPE_RGB:
			*left_pipe = MDP_VP_0_RGB_0_BASE;
			*right_pipe = MDP_VP_0_RGB_1_BASE;
			break;
		case MDSS_MDP_PIPE_TYPE_DMA:
			*left_pipe = MDP_VP_0_DMA_0_BASE;
			*right_pipe = MDP_VP_0_DMA_1_BASE;
			break;
		case MDSS_MDP_PIPE_TYPE_VIG:
		default:
			*left_pipe = MDP_VP_0_VIG_0_BASE;
			*right_pipe = MDP_VP_0_VIG_1_BASE;
			break;
	}
}

static void mdss_mdp_set_flush(struct msm_panel_info *pinfo,
				uint32_t *ctl0_reg_val, uint32_t *ctl1_reg_val)
{
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);
	bool dual_pipe_single_ctl = pinfo->lcdc.dual_pipe &&
		!pinfo->mipi.dual_dsi && !pinfo->lcdc.split_display;
	switch (pinfo->pipe_type) {
		case MDSS_MDP_PIPE_TYPE_RGB:
			if (dual_pipe_single_ctl)
				*ctl0_reg_val = 0x220D8;
			else
				*ctl0_reg_val = 0x22048;
			*ctl1_reg_val = 0x24090;

			if (pinfo->lcdc.dst_split)
				*ctl0_reg_val |= BIT(4);
			break;
		case MDSS_MDP_PIPE_TYPE_DMA:
			if (dual_pipe_single_ctl)
				*ctl0_reg_val = 0x238C0;
			else
				*ctl0_reg_val = 0x22840;
			*ctl1_reg_val = 0x25080;
			if (pinfo->lcdc.dst_split)
				*ctl0_reg_val |= BIT(12);
			break;
		case MDSS_MDP_PIPE_TYPE_VIG:
		default:
			if (dual_pipe_single_ctl)
				*ctl0_reg_val = 0x220C3;
			else
				*ctl0_reg_val = 0x22041;
			*ctl1_reg_val = 0x24082;
			if (pinfo->lcdc.dst_split)
				*ctl0_reg_val |= BIT(1);
			break;
	}
	/* For targets from MDP v1.5, MDP INTF registers are double buffered */
	if ((mdss_mdp_rev == MDSS_MDP_HW_REV_106) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_108) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_111) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_112)) {
		if (pinfo->dest == DISPLAY_2) {
			*ctl0_reg_val |= BIT(31);
			*ctl1_reg_val |= BIT(30);
		} else {
			*ctl0_reg_val |= BIT(30);
			*ctl1_reg_val |= BIT(31);
		}
	} else if ((mdss_mdp_rev == MDSS_MDP_HW_REV_105) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_109) ||
		MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev,
			MDSS_MDP_HW_REV_107) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_114) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_116) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_115) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_110)) {
		if (pinfo->dest == DISPLAY_2) {
			*ctl0_reg_val |= BIT(29);
			*ctl1_reg_val |= BIT(30);
		} else {
			*ctl0_reg_val |= BIT(30);
			*ctl1_reg_val |= BIT(29);
		}
	}
}

static void mdss_source_pipe_config(struct fbcon_config *fb, struct msm_panel_info
		*pinfo, uint32_t pipe_base)
{
	uint32_t img_size, out_size, stride;
	uint32_t fb_off = 0;
	uint32_t flip_bits = 0;
	uint32_t src_xy = 0, dst_xy = 0;
	uint32_t height, width;

	height = fb->height - pinfo->border_top - pinfo->border_bottom;
	width = fb->width - pinfo->border_left - pinfo->border_right;

	/* write active region size*/
	img_size = (height << 16) | width;
	out_size = img_size;
	if (pinfo->lcdc.dual_pipe) {
		if ((pipe_base == MDP_VP_0_RGB_1_BASE) ||
		    (pipe_base == MDP_VP_0_DMA_1_BASE) ||
		    (pipe_base == MDP_VP_0_VIG_1_BASE)) {
			fb_off = (pinfo->xres / 2);
			out_size = (height << 16) + (pinfo->lm_split[1]);
		} else {
			out_size = (height << 16) + (pinfo->lm_split[0]);
		}
	}

	stride = (fb->stride * fb->bpp/8);

	if (fb_off == 0) {	/* left */
		dst_xy = (pinfo->border_top << 16) | pinfo->border_left;
		src_xy = dst_xy;
	} else {	/* right */
		dst_xy = (pinfo->border_top << 16);
		src_xy = (pinfo->border_top << 16) | fb_off;
	}

	dprintf(SPEW,"%s: src=%x fb_off=%x src_xy=%x dst_xy=%x\n",
			 __func__, out_size, fb_off, src_xy, dst_xy);
	writel((uint32_t) fb->base, pipe_base + PIPE_SSPP_SRC0_ADDR);
	writel(stride, pipe_base + PIPE_SSPP_SRC_YSTRIDE);
	writel(img_size, pipe_base + PIPE_SSPP_SRC_IMG_SIZE);
	writel(out_size, pipe_base + PIPE_SSPP_SRC_SIZE);
	writel(out_size, pipe_base + PIPE_SSPP_SRC_OUT_SIZE);
	writel(src_xy, pipe_base + PIPE_SSPP_SRC_XY);
	writel(dst_xy, pipe_base + PIPE_SSPP_OUT_XY);

	/* Tight Packing 3bpp 0-Alpha 8-bit R B G */
	writel(0x0002243F, pipe_base + PIPE_SSPP_SRC_FORMAT);
	writel(0x00020001, pipe_base + PIPE_SSPP_SRC_UNPACK_PATTERN);

	/* bit(0) is set if hflip is required.
	 * bit(1) is set if vflip is required.
	 */
	if (pinfo->orientation & 0x1)
		flip_bits |= MDSS_MDP_OP_MODE_FLIP_LR;
	if (pinfo->orientation & 0x2)
		flip_bits |= MDSS_MDP_OP_MODE_FLIP_UD;

	if (is_software_pixel_ext_config_needed()) {
		flip_bits |= BIT(31);
		writel(out_size, pipe_base + PIPE_SW_PIXEL_EXT_C0_REQ);
		writel(out_size, pipe_base + PIPE_SW_PIXEL_EXT_C1C2_REQ);
		writel(out_size, pipe_base + PIPE_SW_PIXEL_EXT_C3_REQ);
		/* configure phase step 1 for all color components */
		writel(0x200000, pipe_base + PIPE_COMP0_3_PHASE_STEP_X);
		writel(0x200000, pipe_base + PIPE_COMP0_3_PHASE_STEP_Y);
		writel(0x200000, pipe_base + PIPE_COMP1_2_PHASE_STEP_X);
		writel(0x200000, pipe_base + PIPE_COMP1_2_PHASE_STEP_Y);
	}
	writel(flip_bits, pipe_base + PIPE_SSPP_SRC_OP_MODE);
}

static void mdss_vbif_setup()
{
	uint32_t mdp_hw_rev = readl(MDP_HW_REV);
	int access_secure = false;
	if (!MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_107))
		access_secure = restore_secure_cfg(SECURE_DEVICE_MDSS);

	if (!access_secure) {
		dprintf(SPEW, "MDSS VBIF registers unlocked by TZ.\n");

		/* Force VBIF Clocks on, needed for 8974 and 8x26 */
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
			writel(0x00000003, VBIF_VBIF_DDR_RND_RBN_QOS_ARB);
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

static void mdp_select_pipe_client_id(struct msm_panel_info *pinfo,
		uint32_t *left_sspp_client_id, uint32_t *right_sspp_client_id)
{
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);
	if (MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_101) ||
		MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_106) ||
		MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_108) ||
		MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_111) ||
		MDSS_IS_MAJOR_MINOR_MATCHING(mdss_mdp_rev, MDSS_MDP_HW_REV_112)) {
		switch (pinfo->pipe_type) {
			case MDSS_MDP_PIPE_TYPE_RGB:
				*left_sspp_client_id = 0x7; /* 7 */
				*right_sspp_client_id = 0x8; /* 8 */
				break;
			case MDSS_MDP_PIPE_TYPE_DMA:
				*left_sspp_client_id = 0x4; /* 4 */
				*right_sspp_client_id = 0xD; /* 13 */
				break;
			case MDSS_MDP_PIPE_TYPE_VIG:
			default:
				*left_sspp_client_id = 0x1; /* 1 */
				*right_sspp_client_id = 0x9; /* 9 */
				break;
		}
	} else {
		switch (pinfo->pipe_type) {
			case MDSS_MDP_PIPE_TYPE_RGB:
				*left_sspp_client_id = 0x10; /* 16 */
				*right_sspp_client_id = 0x11; /* 17 */
				break;
			case MDSS_MDP_PIPE_TYPE_DMA:
				*left_sspp_client_id = 0xA; /* 10 */
				*right_sspp_client_id = 0xD; /* 13 */
				break;
			case MDSS_MDP_PIPE_TYPE_VIG:
			default:
				*left_sspp_client_id = 0x1; /* 1 */
				*right_sspp_client_id = 0x4; /* 4 */
				break;
		}
	}
}

static void mdp_select_pipe_xin_id(struct msm_panel_info *pinfo,
		uint32_t *left_pipe_xin_id, uint32_t *right_pipe_xin_id)
{
	switch (pinfo->pipe_type) {
		case MDSS_MDP_PIPE_TYPE_RGB:
			*left_pipe_xin_id = 0x1; /* 1 */
			*right_pipe_xin_id = 0x5; /* 5 */
			break;
		case MDSS_MDP_PIPE_TYPE_DMA:
			*left_pipe_xin_id = 0x2; /* 2 */
			*right_pipe_xin_id = 0xA; /* 10 */
			break;
		case MDSS_MDP_PIPE_TYPE_VIG:
		default:
			*left_pipe_xin_id = 0x0; /* 0 */
			*right_pipe_xin_id = 0x4; /* 4 */
			break;
	}
}

static void mdss_smp_setup(struct msm_panel_info *pinfo, uint32_t left_pipe,
		uint32_t right_pipe)

{
	uint32_t left_sspp_client_id, right_sspp_client_id;
	uint32_t bpp = 3, free_smp_offset = 0, xres = MDSS_MAX_LINE_BUF_WIDTH;
	uint32_t smp_cnt, smp_size = 4096, fixed_smp_cnt = 0;
	uint32_t mdss_mdp_rev = readl(MDP_HW_REV);

	if ((mdss_mdp_rev == MDSS_MDP_HW_REV_106) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_112)) {
		/* 8Kb per SMP on 8916/8952 */
		smp_size = 8192;
	} else if ((mdss_mdp_rev == MDSS_MDP_HW_REV_108) ||
		(mdss_mdp_rev == MDSS_MDP_HW_REV_111)) {
		/* 10Kb per SMP on 8939/8956 */
		smp_size = 10240;
	} else if ((mdss_mdp_rev >= MDSS_MDP_HW_REV_103) &&
		(mdss_mdp_rev < MDSS_MDP_HW_REV_200)) {
		smp_size = 8192;
		free_smp_offset = 0xC;
		if (pinfo->pipe_type == MDSS_MDP_PIPE_TYPE_RGB)
			fixed_smp_cnt = 2;
		else
			fixed_smp_cnt = 0;
	}

	mdp_select_pipe_client_id(pinfo,
			&left_sspp_client_id, &right_sspp_client_id);

	/* Each pipe driving half the screen */
	if (pinfo->lcdc.dual_pipe)
		xres = pinfo->lm_split[0];

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
		xres = pinfo->lm_split[1];

		smp_cnt = (xres * bpp * 2) + smp_size - 1;
		smp_cnt /= smp_size;

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

static void mdss_intf_tg_setup(struct msm_panel_info *pinfo, uint32_t intf_base)
{
	uint32_t hsync_period, vsync_period;
	uint32_t hsync_start_x, hsync_end_x;
	uint32_t display_hctl, hsync_ctl, display_vstart, display_vend;
	uint32_t adjust_xres = 0;
	uint32_t upper = 0, lower = 0;

	struct lcdc_panel_info *lcdc = NULL;
	struct intf_timing_params itp = {0};

	if (pinfo == NULL)
		return;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return;

	adjust_xres = pinfo->xres;
	if (pinfo->lcdc.split_display) {
		if (pinfo->lcdc.dst_split) {
			adjust_xres /= 2;
		} else if(pinfo->lcdc.dual_pipe) {
			if (intf_base == (MDP_INTF_1_BASE + mdss_mdp_intf_offset()))
				adjust_xres = pinfo->lm_split[0];
			else
				adjust_xres = pinfo->lm_split[1];
		}

		if (intf_base == (MDP_INTF_1_BASE + mdss_mdp_intf_offset())) {
			if (pinfo->lcdc.pipe_swap) {
				lower |= BIT(4);
				upper |= BIT(8);
			} else {
				lower |= BIT(8);
				upper |= BIT(4);
			}
			writel(lower, MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL);
			writel(upper, MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL);
			writel(0x1, MDP_REG_SPLIT_DISPLAY_EN);
		}
	}

	if (pinfo->lcdc.dst_split && (intf_base == (MDP_INTF_1_BASE + mdss_mdp_intf_offset()))) {
		uint32_t ppb_offset = mdss_mdp_get_ppb_offset();
		writel(BIT(5), REG_MDP(ppb_offset)); /* MMSS_MDP_PPB0_CNTL */
		writel(BIT(16) | (0x3 << 20), REG_MDP(ppb_offset + 0x4)); /* MMSS_MDP_PPB0_CONFIG */
	}

	if (!pinfo->fbc.enabled || !pinfo->fbc.comp_ratio)
		pinfo->fbc.comp_ratio = 1;

	itp.xres = (adjust_xres / pinfo->fbc.comp_ratio);
	itp.yres = pinfo->yres;
	itp.width =((adjust_xres + pinfo->lcdc.xres_pad) / pinfo->fbc.comp_ratio);

	if (pinfo->compression_mode == COMPRESSION_DSC) {
		itp.xres = pinfo->dsc.pclk_per_line;
		itp.width = pinfo->dsc.pclk_per_line;
	}

	itp.height = pinfo->yres + pinfo->lcdc.yres_pad;
	itp.h_back_porch = pinfo->lcdc.h_back_porch;
	itp.h_front_porch = pinfo->lcdc.h_front_porch;
	itp.v_back_porch =  pinfo->lcdc.v_back_porch;
	itp.v_front_porch = pinfo->lcdc.v_front_porch;
	itp.hsync_pulse_width = pinfo->lcdc.h_pulse_width;
	itp.vsync_pulse_width = pinfo->lcdc.v_pulse_width;

	itp.border_clr = pinfo->lcdc.border_clr;
	itp.underflow_clr = pinfo->lcdc.underflow_clr;
	itp.hsync_skew = pinfo->lcdc.hsync_skew;

	hsync_period = itp.hsync_pulse_width + itp.h_back_porch +
			itp.width + itp.h_front_porch;

	vsync_period = itp.vsync_pulse_width + itp.v_back_porch +
			itp.height + itp.v_front_porch;

	hsync_start_x =
		itp.hsync_pulse_width +
		itp.h_back_porch;
	hsync_end_x =
		hsync_period - itp.h_front_porch - 1;

	display_vstart = (itp.vsync_pulse_width +
			itp.v_back_porch)
		* hsync_period + itp.hsync_skew;
	display_vend = ((vsync_period - itp.v_front_porch) * hsync_period)
		+ itp.hsync_skew - 1;

	if (intf_base == (MDP_INTF_0_BASE + mdss_mdp_intf_offset())) { /* eDP */
		display_vstart += itp.hsync_pulse_width + itp.h_back_porch;
		display_vend -= itp.h_front_porch;
	}

	hsync_ctl = (hsync_period << 16) | itp.hsync_pulse_width;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;

	writel(hsync_ctl, MDP_HSYNC_CTL + intf_base);
	writel(vsync_period*hsync_period, MDP_VSYNC_PERIOD_F0 +
			intf_base);
	writel(0x00, MDP_VSYNC_PERIOD_F1 + intf_base);
	writel(itp.vsync_pulse_width*hsync_period,
			MDP_VSYNC_PULSE_WIDTH_F0 +
			intf_base);
	writel(0x00, MDP_VSYNC_PULSE_WIDTH_F1 + intf_base);
	writel(display_hctl, MDP_DISPLAY_HCTL + intf_base);
	writel(display_vstart, MDP_DISPLAY_V_START_F0 +
			intf_base);
	writel(0x00, MDP_DISPLAY_V_START_F1 + intf_base);
	writel(display_vend, MDP_DISPLAY_V_END_F0 +
			intf_base);
	writel(0x00, MDP_DISPLAY_V_END_F1 + intf_base);
	writel(0x00, MDP_ACTIVE_HCTL + intf_base);
	writel(0x00, MDP_ACTIVE_V_START_F0 + intf_base);
	writel(0x00, MDP_ACTIVE_V_START_F1 + intf_base);
	writel(0x00, MDP_ACTIVE_V_END_F0 + intf_base);
	writel(0x00, MDP_ACTIVE_V_END_F1 + intf_base);
	writel(0xFF, MDP_UNDERFFLOW_COLOR + intf_base);

	if (intf_base == (MDP_INTF_0_BASE + mdss_mdp_intf_offset())) /* eDP */
		writel(0x212A, MDP_PANEL_FORMAT + intf_base);
	else
		writel(0x213F, MDP_PANEL_FORMAT + intf_base);
}

static void mdss_intf_fetch_start_config(struct msm_panel_info *pinfo,
					uint32_t intf_base)
{
	uint32_t mdp_hw_rev = readl(MDP_HW_REV);
	uint32_t v_total, h_total, fetch_start, vfp_start;
	uint32_t prefetch_avail, prefetch_needed;
	uint32_t adjust_xres = 0;
	uint32_t fetch_enable = BIT(31);

	struct lcdc_panel_info *lcdc = NULL;

	if (pinfo == NULL)
		return;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return;

	/*
	 * MDP programmable fetch is for MDP with rev >= 1.05.
	 * Programmable fetch is not needed if vertical back porch
	 * plus vertical puls width is >= 25.
	 */
	if (mdp_hw_rev < MDSS_MDP_HW_REV_105 ||
			(lcdc->v_back_porch + lcdc->v_pulse_width) >=
			MDSS_MDP_MAX_PREFILL_FETCH)
		return;

	adjust_xres = pinfo->xres;
	if (pinfo->lcdc.split_display) {
		if (pinfo->lcdc.dst_split) {
			adjust_xres /= 2;
		} else if(pinfo->lcdc.dual_pipe) {
			if (intf_base == (MDP_INTF_1_BASE + mdss_mdp_intf_offset()))
				adjust_xres = pinfo->lm_split[0];
			else
				adjust_xres = pinfo->lm_split[1];
		}
	}

	if (pinfo->compression_mode == COMPRESSION_DSC) {
		adjust_xres = pinfo->dsc.pclk_per_line;
	} else if (pinfo->compression_mode == COMPRESSION_FBC) {
		if (pinfo->fbc.enabled && pinfo->fbc.comp_ratio)
			adjust_xres /= pinfo->fbc.comp_ratio;
	}

	/*
	 * Fetch should always be outside the active lines. If the fetching
	 * is programmed within active region, hardware behavior is unknown.
	 */
	v_total = lcdc->v_pulse_width + lcdc->v_back_porch + pinfo->yres +
							lcdc->v_front_porch;
	h_total = lcdc->h_pulse_width + lcdc->h_back_porch + adjust_xres +
							lcdc->h_front_porch;
	vfp_start = lcdc->v_pulse_width + lcdc->v_back_porch + pinfo->yres;

	prefetch_avail = v_total - vfp_start;
	prefetch_needed = MDSS_MDP_MAX_PREFILL_FETCH -
		lcdc->v_back_porch -
		lcdc->v_pulse_width;

	/*
	 * In some cases, vertical front porch is too high. In such cases limit
	 * the mdp fetch lines  as the last (25 - vbp - vpw) lines of vertical front porch.
	 */
	if (prefetch_avail > prefetch_needed)
		prefetch_avail = prefetch_needed;

	fetch_start = (v_total - prefetch_avail) * h_total + 1;

	if (pinfo->dfps.panel_dfps.enabled)
		fetch_enable |= BIT(23);

	writel_relaxed(fetch_start, MDP_PROG_FETCH_START + intf_base);
	writel_relaxed(fetch_enable, MDP_INTF_CONFIG + intf_base);
}

void mdss_layer_mixer_setup(struct fbcon_config *fb, struct msm_panel_info
		*pinfo)
{
	uint32_t mdp_rgb_size, height, width;
	uint32_t left_staging_level, right_staging_level;

	height = fb->height;
	width = fb->width;

	if (pinfo->lcdc.dual_pipe && !pinfo->lcdc.dst_split)
		width = pinfo->lm_split[0];

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

	switch (pinfo->pipe_type) {
		case MDSS_MDP_PIPE_TYPE_RGB:
			left_staging_level = 0x0000200;
			right_staging_level = 0x1000;
			break;
		case MDSS_MDP_PIPE_TYPE_DMA:
			left_staging_level = 0x0040000;
			right_staging_level = 0x200000;
			break;
		case MDSS_MDP_PIPE_TYPE_VIG:
		default:
			left_staging_level = 0x1;
			right_staging_level = 0x8;
			break;
	}

	/*
	 * When ping-pong split is enabled and two pipes are used,
	 * both the pipes need to be staged on the same layer mixer.
	 */
	if (pinfo->lcdc.dual_pipe && pinfo->lcdc.dst_split)
		left_staging_level |= right_staging_level;

	/* Base layer for layer mixer 0 */
	writel(left_staging_level, MDP_CTL_0_BASE + CTL_LAYER_0);

	if (pinfo->lcdc.dual_pipe && !pinfo->lcdc.dst_split) {
		/* write active region size*/
		mdp_rgb_size = (height << 16) | pinfo->lm_split[1];

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

		/* Base layer for layer mixer 1 */
		if (pinfo->lcdc.split_display)
			writel(right_staging_level, MDP_CTL_1_BASE + CTL_LAYER_1);
		else
			writel(right_staging_level, MDP_CTL_0_BASE + CTL_LAYER_1);
	}
}

void mdss_fbc_cfg(struct msm_panel_info *pinfo)
{
	uint32_t mode = 0;
	uint32_t budget_ctl = 0;
	uint32_t lossy_mode = 0;
	struct fbc_panel_info *fbc;
	uint32_t enc_mode, width;

	fbc = &pinfo->fbc;

	if (!pinfo->fbc.enabled)
		return;

	/* enc_mode defines FBC version. 0 = FBC 1.0 and 1 = FBC 2.0 */
	enc_mode = (fbc->comp_ratio == 2) ? 0 : 1;

	width = pinfo->xres;
	if (enc_mode)
		width = (pinfo->xres/fbc->comp_ratio);

	if (pinfo->mipi.dual_dsi)
		width /= 2;

	mode = ((width) << 16) | ((fbc->slice_height) << 11) |
		((fbc->pred_mode) << 10) | (enc_mode) << 9 |
		((fbc->comp_mode) << 8) | ((fbc->qerr_enable) << 7) |
		((fbc->cd_bias) << 4) | ((fbc->pat_enable) << 3) |
		((fbc->vlc_enable) << 2) | ((fbc->bflc_enable) << 1) | 1;

	dprintf(SPEW, "width = %d, slice height = %d, pred_mode =%d, enc_mode = %d, \
			comp_mode %d, qerr_enable = %d, cd_bias = %d\n",
			width, fbc->slice_height, fbc->pred_mode, enc_mode,
			fbc->comp_mode, fbc->qerr_enable, fbc->cd_bias);
	dprintf(SPEW, "pat_enable %d, vlc_enable = %d, bflc_enable = %d\n",
			fbc->pat_enable, fbc->vlc_enable, fbc->bflc_enable);

	budget_ctl = ((fbc->line_x_budget) << 12) |
		((fbc->block_x_budget) << 8) | fbc->block_budget;

	lossy_mode = (((fbc->max_pred_err) << 28) | (fbc->lossless_mode_thd) << 16) |
		((fbc->lossy_mode_thd) << 8) |
		((fbc->lossy_rgb_thd) << 4) | fbc->lossy_mode_idx;

	dprintf(SPEW, "mode= 0x%x, budget_ctl = 0x%x, lossy_mode= 0x%x\n",
			mode, budget_ctl, lossy_mode);
	writel(mode, MDP_PP_0_BASE + MDSS_MDP_REG_PP_FBC_MODE);
	writel(budget_ctl, MDP_PP_0_BASE + MDSS_MDP_REG_PP_FBC_BUDGET_CTL);
	writel(lossy_mode, MDP_PP_0_BASE + MDSS_MDP_REG_PP_FBC_LOSSY_MODE);

	if (pinfo->mipi.dual_dsi) {
		writel(mode, MDP_PP_1_BASE + MDSS_MDP_REG_PP_FBC_MODE);
		writel(budget_ctl, MDP_PP_1_BASE +
				MDSS_MDP_REG_PP_FBC_BUDGET_CTL);
		writel(lossy_mode, MDP_PP_1_BASE +
				MDSS_MDP_REG_PP_FBC_LOSSY_MODE);
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
			MDSS_MDP_HW_REV_106) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_108) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_111) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_112))
		map = 0xE4;
	else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_105) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_109) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_107) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_110))
		map = 0xA4;
	else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev,
			MDSS_MDP_HW_REV_103))
		map = 0xFA;
	else
		return;

	writel(map, MDP_QOS_REMAPPER_CLASS_0);
}

void mdss_vbif_qos_remapper_setup(struct msm_panel_info *pinfo)
{
	uint32_t mask, reg_val, i;
	uint32_t left_pipe_xin_id, right_pipe_xin_id;
	uint32_t mdp_hw_rev = readl(MDP_HW_REV);
	uint32_t vbif_qos[4] = {0, 0, 0, 0};
	uint32_t vbif_offset;

	mdp_select_pipe_xin_id(pinfo,
			&left_pipe_xin_id, &right_pipe_xin_id);

	if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_106) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_108) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_111) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_112)) {
		vbif_qos[0] = 2;
		vbif_qos[1] = 2;
		vbif_qos[2] = 2;
		vbif_qos[3] = 2;
	} else if (MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_105) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_109) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_107) ||
		 MDSS_IS_MAJOR_MINOR_MATCHING(mdp_hw_rev, MDSS_MDP_HW_REV_110)) {
		vbif_qos[0] = 1;
		vbif_qos[1] = 2;
		vbif_qos[2] = 2;
		vbif_qos[3] = 2;
	} else {
		return;
	}

	vbif_offset = mdss_mdp_vbif_qos_remap_get_offset();

	for (i = 0; i < 4; i++) {
		/* VBIF_VBIF_QOS_REMAP_00 */
		reg_val = readl(REG_MDP(vbif_offset) + i*4);
		mask = 0x3 << (left_pipe_xin_id * 2);
		reg_val &= ~(mask);
		reg_val |= vbif_qos[i] << (left_pipe_xin_id * 2);

		if (pinfo->lcdc.dual_pipe) {
			mask = 0x3 << (right_pipe_xin_id * 2);
			reg_val &= ~(mask);
			reg_val |= vbif_qos[i] << (right_pipe_xin_id * 2);
		}
		writel(reg_val, REG_MDP(vbif_offset) + i*4);
	}
}

static uint32_t mdss_mdp_ctl_out_sel(struct msm_panel_info *pinfo,
	int is_main_ctl)
{
	uint32_t mctl_intf_sel;
	uint32_t sctl_intf_sel;

	if ((pinfo->dest == DISPLAY_2) ||
		((pinfo->dest = DISPLAY_1) && (pinfo->lcdc.pipe_swap))) {
		mctl_intf_sel = BIT(4) | BIT(5); /* Interface 2 */
		sctl_intf_sel = BIT(5); /* Interface 1 */
	} else {
		mctl_intf_sel = BIT(5); /* Interface 1 */
		sctl_intf_sel = BIT(4) | BIT(5); /* Interface 2 */
	}
	dprintf(SPEW, "%s: main ctl dest=%s sec ctl dest=%s\n", __func__,
		(mctl_intf_sel & BIT(4)) ? "Intf2" : "Intf1",
		(sctl_intf_sel & BIT(4)) ? "Intf2" : "Intf1");
	return is_main_ctl ? mctl_intf_sel : sctl_intf_sel;
}

static void mdp_set_intf_base(struct msm_panel_info *pinfo,
	uint32_t *intf_sel, uint32_t *sintf_sel,
	uint32_t *intf_base, uint32_t *sintf_base)
{
	if (pinfo->dest == DISPLAY_2) {
		*intf_sel = BIT(16);
		*sintf_sel = BIT(8);
		*intf_base = MDP_INTF_2_BASE + mdss_mdp_intf_offset();
		*sintf_base = MDP_INTF_1_BASE + mdss_mdp_intf_offset();
	} else {
		*intf_sel = BIT(8);
		*sintf_sel = BIT(16);
		*intf_base = MDP_INTF_1_BASE + mdss_mdp_intf_offset();
		*sintf_base = MDP_INTF_2_BASE + mdss_mdp_intf_offset();
	}
	dprintf(SPEW, "%s: main intf=%s, sec intf=%s\n", __func__,
		(pinfo->dest == DISPLAY_2) ? "Intf2" : "Intf1",
		(pinfo->dest == DISPLAY_2) ? "Intf1" : "Intf2");
}

int mdp_dsi_video_config(struct msm_panel_info *pinfo,
		struct fbcon_config *fb)
{
	uint32_t intf_sel, sintf_sel;
	uint32_t intf_base, sintf_base;
	uint32_t left_pipe, right_pipe;
	uint32_t reg;

	mdp_set_intf_base(pinfo, &intf_sel, &sintf_sel, &intf_base, &sintf_base);

	mdss_intf_tg_setup(pinfo, intf_base);
	mdss_intf_fetch_start_config(pinfo, intf_base);

	if (pinfo->mipi.dual_dsi) {
		mdss_intf_tg_setup(pinfo, sintf_base);
		mdss_intf_fetch_start_config(pinfo, sintf_base);
	}

	mdp_clk_gating_ctrl();

	mdp_select_pipe_type(pinfo, &left_pipe, &right_pipe);
	mdss_vbif_setup();
	if (!has_fixed_size_smp())
		mdss_smp_setup(pinfo, left_pipe, right_pipe);

	mdss_qos_remapper_setup();
	mdss_vbif_qos_remapper_setup(pinfo);

	mdss_source_pipe_config(fb, pinfo, left_pipe);

	if (pinfo->lcdc.dual_pipe)
		mdss_source_pipe_config(fb, pinfo, right_pipe);

	mdss_layer_mixer_setup(fb, pinfo);

	reg = 0x1f00 | mdss_mdp_ctl_out_sel(pinfo, 1);

	/* enable 3D mux for dual_pipe but single interface config */
	if (pinfo->lcdc.dual_pipe && !pinfo->mipi.dual_dsi &&
		!pinfo->lcdc.split_display) {

		if (pinfo->num_dsc_enc != 2)
			reg |= BIT(19) | BIT(20);
	}

	writel(reg, MDP_CTL_0_BASE + CTL_TOP);

	if ((pinfo->compression_mode == COMPRESSION_DSC) &&
	    pinfo->dsc.mdp_dsc_config) {
		struct dsc_desc *dsc = &pinfo->dsc;

		if (pinfo->lcdc.dual_pipe && !pinfo->mipi.dual_dsi &&
		    !pinfo->lcdc.split_display && (pinfo->num_dsc_enc == 2)) {
			dsc->mdp_dsc_config(pinfo, MDP_PP_0_BASE,
				MDP_DSC_0_BASE, true, true);
			dsc->mdp_dsc_config(pinfo, MDP_PP_1_BASE,
				MDP_DSC_1_BASE, true, true);

		} else if (pinfo->lcdc.dual_pipe && pinfo->mipi.dual_dsi &&
		    pinfo->lcdc.split_display && (pinfo->num_dsc_enc == 1)) {
			dsc->mdp_dsc_config(pinfo, MDP_PP_0_BASE,
				MDP_DSC_0_BASE, false, false);
			dsc->mdp_dsc_config(pinfo, MDP_PP_1_BASE,
				MDP_DSC_1_BASE, false, false);

		} else {
			dsc->mdp_dsc_config(pinfo, MDP_PP_0_BASE,
				MDP_DSC_0_BASE, false, false);
		}
	} else if (pinfo->compression_mode == COMPRESSION_FBC) {
		if (pinfo->fbc.enabled)
			mdss_fbc_cfg(pinfo);
	}

	/*
	 * if dst_split is enabled, intf 1 & 2 needs to be enabled but
	 * CTL_1 path should not be set since CTL_0 itself is going
	 * to split after DSPP block and drive both intf.
	 */
	if (pinfo->mipi.dual_dsi) {
		if (!pinfo->lcdc.dst_split) {
			reg = 0x1f00 | mdss_mdp_ctl_out_sel(pinfo,0);
			writel(reg, MDP_CTL_1_BASE + CTL_TOP);
		}
		intf_sel |= sintf_sel; /* INTF 2 enable */
	}

	writel(intf_sel, MDP_DISP_INTF_SEL);

	writel(0x1111, MDP_VIDEO_INTF_UNDERFLOW_CTL);
	writel(0x01, MDP_UPPER_NEW_ROI_PRIOR_RO_START);
	writel(0x01, MDP_LOWER_NEW_ROI_PRIOR_TO_START);

	return 0;
}

int mdp_edp_config(struct msm_panel_info *pinfo, struct fbcon_config *fb)
{
	uint32_t left_pipe, right_pipe;

	mdss_intf_tg_setup(pinfo, MDP_INTF_0_BASE);

	mdp_select_pipe_type(pinfo, &left_pipe, &right_pipe);
	mdp_clk_gating_ctrl();

	mdss_vbif_setup();
	mdss_smp_setup(pinfo, left_pipe, right_pipe);

	mdss_qos_remapper_setup();
	mdss_vbif_qos_remapper_setup(pinfo);

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

int mdss_hdmi_config(struct msm_panel_info *pinfo, struct fbcon_config *fb)
{
	uint32_t left_pipe, right_pipe, out_size;

	mdss_intf_tg_setup(pinfo, MDP_INTF_3_BASE + mdss_mdp_intf_offset());
	mdss_intf_fetch_start_config(pinfo, MDP_INTF_3_BASE + mdss_mdp_intf_offset());
	pinfo->pipe_type = MDSS_MDP_PIPE_TYPE_RGB;
	mdp_select_pipe_type(pinfo, &left_pipe, &right_pipe);

	mdp_clk_gating_ctrl();
	mdss_vbif_setup();

	mdss_smp_setup(pinfo, left_pipe, right_pipe);

	mdss_qos_remapper_setup();

	mdss_source_pipe_config(fb, pinfo, left_pipe);
	if (pinfo->lcdc.dual_pipe)
		mdss_source_pipe_config(fb, pinfo, right_pipe);

	mdss_layer_mixer_setup(fb, pinfo);

	if (pinfo->lcdc.dual_pipe)
		writel(0x181F40, MDP_CTL_0_BASE + CTL_TOP);
	else
		writel(0x40, MDP_CTL_0_BASE + CTL_TOP);

	writel(BIT(24) | BIT(25), MDP_DISP_INTF_SEL);
	writel(0x11111, MDP_VIDEO_INTF_UNDERFLOW_CTL);
	writel(0x01, MDP_UPPER_NEW_ROI_PRIOR_RO_START);
	writel(0x01, MDP_LOWER_NEW_ROI_PRIOR_TO_START);

	/**
	 * Program the CDM hardware block in HDMI bypass mode, and enable
	 * the HDMI packer.
	 */
	writel(0x01, CDM_HDMI_PACK_OP_MODE);
	writel(0x00, MDP_OUT_CTL_0);
	writel(0x00, MDP_INTF_3_INTF_CONFIG);
	out_size = (pinfo->xres & 0xFFFF) | ((pinfo->yres & 0xFFFF) << 16);
	writel(out_size, CDM_CDWN2_OUT_SIZE);
	writel(0x80, CDM_CDWN2_OP_MODE);
	writel(0x3FF0000, CDM_CDWN2_CLAMP_OUT);
	writel(0x0, CDM_CSC_10_OP_MODE);

	return 0;
}

int mdp_dsi_cmd_config(struct msm_panel_info *pinfo,
                struct fbcon_config *fb)
{
	uint32_t intf_sel, sintf_sel;
	uint32_t intf_base, sintf_base;
	uint32_t reg;
	int ret = NO_ERROR;
	uint32_t left_pipe, right_pipe;

	struct lcdc_panel_info *lcdc = NULL;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	mdp_set_intf_base(pinfo, &intf_sel, &sintf_sel, &intf_base, &sintf_base);

	if (pinfo->lcdc.split_display) {
		reg = BIT(1); /* Command mode */
		if (pinfo->lcdc.dst_split)
			reg |= BIT(2); /* Enable SMART_PANEL_FREE_RUN mode */
		if (pinfo->lcdc.pipe_swap)
			reg |= BIT(4); /* Use intf2 as trigger */
		else
			reg |= BIT(8); /* Use intf1 as trigger */
		writel(reg, MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL);
		writel(reg, MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL);
		writel(0x1, MDP_REG_SPLIT_DISPLAY_EN);
	}

	if (pinfo->lcdc.dst_split) {
		uint32_t ppb_offset = mdss_mdp_get_ppb_offset();
		writel(BIT(5), REG_MDP(ppb_offset)); /* MMSS_MDP_PPB0_CNTL */
		writel(BIT(16) | (0x3 << 20), REG_MDP(ppb_offset + 0x4)); /* MMSS_MDP_PPB0_CONFIG */
	}

	mdp_clk_gating_ctrl();

	if (pinfo->mipi.dual_dsi)
		intf_sel |= sintf_sel; /* INTF 2 enable */

	writel(intf_sel, MDP_DISP_INTF_SEL);

	mdp_select_pipe_type(pinfo, &left_pipe, &right_pipe);
	mdss_vbif_setup();
	if (!has_fixed_size_smp())
		mdss_smp_setup(pinfo, left_pipe, right_pipe);
	mdss_qos_remapper_setup();
	mdss_vbif_qos_remapper_setup(pinfo);

	mdss_source_pipe_config(fb, pinfo, left_pipe);

	if (pinfo->lcdc.dual_pipe)
		mdss_source_pipe_config(fb, pinfo, right_pipe);

	mdss_layer_mixer_setup(fb, pinfo);

	writel(0x213F, MDP_PANEL_FORMAT + intf_base);
	reg = 0x21f00 | mdss_mdp_ctl_out_sel(pinfo, 1);

	/* enable 3D mux for dual_pipe but single interface config */
	if (pinfo->lcdc.dual_pipe && !pinfo->mipi.dual_dsi &&
		!pinfo->lcdc.split_display) {

		if (pinfo->num_dsc_enc != 2)
			reg |= BIT(19) | BIT(20);
	}

	writel(reg, MDP_CTL_0_BASE + CTL_TOP);

	if ((pinfo->compression_mode == COMPRESSION_DSC) &&
	    pinfo->dsc.mdp_dsc_config) {
		struct dsc_desc *dsc = &pinfo->dsc;

		if (pinfo->lcdc.dual_pipe && !pinfo->mipi.dual_dsi &&
		    !pinfo->lcdc.split_display && (pinfo->num_dsc_enc == 2)) {

			dsc->mdp_dsc_config(pinfo, MDP_PP_0_BASE,
				MDP_DSC_0_BASE, true, true);
			dsc->mdp_dsc_config(pinfo, MDP_PP_1_BASE,
				MDP_DSC_1_BASE, true, true);
		} else {
			dsc->mdp_dsc_config(pinfo, MDP_PP_0_BASE,
				MDP_DSC_0_BASE, false, false);
		}
	} else if (pinfo->compression_mode == COMPRESSION_FBC) {
		if (pinfo->fbc.enabled)
			mdss_fbc_cfg(pinfo);
	}

	if (pinfo->mipi.dual_dsi) {
		writel(0x213F, sintf_base + MDP_PANEL_FORMAT);
		if (!pinfo->lcdc.dst_split) {
			reg = 0x21f00 | mdss_mdp_ctl_out_sel(pinfo, 0);
			writel(reg, MDP_CTL_1_BASE + CTL_TOP);
		}
	}

	return ret;
}

int mdp_dsi_video_on(struct msm_panel_info *pinfo)
{
	uint32_t ctl0_reg_val, ctl1_reg_val;
	uint32_t timing_engine_en;

	mdss_mdp_set_flush(pinfo, &ctl0_reg_val, &ctl1_reg_val);
	writel(ctl0_reg_val, MDP_CTL_0_BASE + CTL_FLUSH);
	if (pinfo->lcdc.dual_pipe && !pinfo->lcdc.dst_split)
		writel(ctl1_reg_val, MDP_CTL_1_BASE + CTL_FLUSH);

	if (pinfo->dest == DISPLAY_1)
		timing_engine_en = MDP_INTF_1_TIMING_ENGINE_EN;
	else
		timing_engine_en = MDP_INTF_2_TIMING_ENGINE_EN;
	writel(0x01, timing_engine_en + mdss_mdp_intf_offset());

	return NO_ERROR;
}

int mdp_dsi_video_off(struct msm_panel_info *pinfo)
{
	uint32_t timing_engine_en;

	if (pinfo->dest == DISPLAY_1)
		timing_engine_en = MDP_INTF_1_TIMING_ENGINE_EN;
	else
		timing_engine_en = MDP_INTF_2_TIMING_ENGINE_EN;

	if(!target_cont_splash_screen())
	{
		writel(0x00000000, timing_engine_en + mdss_mdp_intf_offset());
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

static void mdp_set_cmd_autorefresh_mode(struct msm_panel_info *pinfo)
{
	uint32_t total_lines = 0, vclks_line = 0, cfg = 0;

	if (!pinfo || (pinfo->type != MIPI_CMD_PANEL) ||
				!pinfo->autorefresh_enable)
		return;

	total_lines = pinfo->lcdc.v_front_porch +
			pinfo->lcdc.v_back_porch +
			pinfo->lcdc.v_pulse_width +
			pinfo->border_top + pinfo->border_bottom +
			pinfo->yres;
	total_lines *= pinfo->mipi.frame_rate;

	vclks_line = (total_lines) ? 19200000 / total_lines : 0;
	vclks_line = vclks_line * pinfo->mipi.frame_rate * 100 / 6000;

	cfg = BIT(19) | vclks_line;

	/* Configure tearcheck VSYNC param */
	writel(cfg, MDP_REG_PP_0_SYNC_CONFIG_VSYNC);
	if (pinfo->lcdc.dst_split)
		writel(cfg, MDP_REG_PP_SLAVE_SYNC_CONFIG_VSYNC);
	if (pinfo->lcdc.dual_pipe)
		writel(cfg, MDP_REG_PP_1_SYNC_CONFIG_VSYNC);
	dsb();

	/* Enable autorefresh mode */
	writel((BIT(31) | pinfo->autorefresh_framenum),
			MDP_REG_PP_0_AUTOREFRESH_CONFIG);
	if (pinfo->lcdc.dst_split)
		writel((BIT(31) | pinfo->autorefresh_framenum),
			MDP_REG_PP_SLAVE_AUTOREFRESH_CONFIG);
	if (pinfo->lcdc.dual_pipe)
		writel((BIT(31) | pinfo->autorefresh_framenum),
			MDP_REG_PP_1_AUTOREFRESH_CONFIG);
	dsb();
}

int mdp_dma_on(struct msm_panel_info *pinfo)
{
	uint32_t ctl0_reg_val, ctl1_reg_val;
	mdss_mdp_set_flush(pinfo, &ctl0_reg_val, &ctl1_reg_val);
	writel(ctl0_reg_val, MDP_CTL_0_BASE + CTL_FLUSH);
	if (pinfo->lcdc.dual_pipe && !pinfo->lcdc.dst_split)
		writel(ctl1_reg_val, MDP_CTL_1_BASE + CTL_FLUSH);

        if (pinfo->autorefresh_enable)
		mdp_set_cmd_autorefresh_mode(pinfo);
	writel(0x01, MDP_CTL_0_BASE + CTL_START);

        return NO_ERROR;
}

int mdp_edp_on(struct msm_panel_info *pinfo)
{
	uint32_t ctl0_reg_val, ctl1_reg_val;
	mdss_mdp_set_flush(pinfo, &ctl0_reg_val, &ctl1_reg_val);
	writel(ctl0_reg_val, MDP_CTL_0_BASE + CTL_FLUSH);
	writel(0x01, MDP_INTF_0_TIMING_ENGINE_EN  + mdss_mdp_intf_offset());
	return NO_ERROR;
}

int mdss_hdmi_on(struct msm_panel_info *pinfo)
{
	uint32_t ctl0_reg_val, ctl1_reg_val;

	mdss_mdp_set_flush(pinfo, &ctl0_reg_val, &ctl1_reg_val);
	writel(ctl0_reg_val, MDP_CTL_0_BASE + CTL_FLUSH);

	writel(0x01, MDP_INTF_3_TIMING_ENGINE_EN + mdss_mdp_intf_offset());

	return NO_ERROR;
}

int mdss_hdmi_off(struct msm_panel_info *pinfo)
{
	if(!target_cont_splash_screen())
	{
		writel(0x00000000, MDP_INTF_3_TIMING_ENGINE_EN + mdss_mdp_intf_offset());
		mdelay(60);
		/* Underrun(Interface 0/1/2/3) VSYNC Interrupt Enable  */
		writel(0xFF777713, MDP_INTR_CLEAR);
	}

	writel(0x00000000, MDP_INTR_EN);

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
