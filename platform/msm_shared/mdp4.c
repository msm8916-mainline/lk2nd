/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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
#include <mdp4.h>
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

static int mdp_rev;

void
mdp_setup_dma_p_video_config(unsigned short pack_pattern,
			     unsigned short img_width,
			     unsigned short img_height,
			     unsigned long input_img_addr,
			     unsigned short img_width_full_size,
			     unsigned char ystride)
{
	dprintf(SPEW, "MDP4.2 Setup for DSI Video Mode\n");

	// ----- programming MDP_AXI_RDMASTER_CONFIG --------
	/* MDP_AXI_RDMASTER_CONFIG set all master to read from AXI port 0, that's
	   the only port connected */
	//TODO: Seems to still work without this
	writel(0x00290000, MDP_AXI_RDMASTER_CONFIG);
	writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
	writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);

	/* Set up CMD_INTF_SEL, VIDEO_INTF_SEL, EXT_INTF_SEL, SEC_INTF_SEL, PRIM_INTF_SEL */
	writel(0x00000049, MDP_DISP_INTF_SEL);

	/* DMA P */
	writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

	/* RGB 888 */
	writel(pack_pattern << 8 | 0xbf | (0 << 25), MDP_DMA_P_CONFIG);

	writel(0x0, MDP_DMA_P_OUT_XY);

	writel(img_height << 16 | img_width, MDP_DMA_P_SIZE);

	writel(input_img_addr, MDP_DMA_P_BUF_ADDR);

	writel(img_width_full_size * ystride, MDP_DMA_P_BUF_Y_STRIDE);
}

int
mdp_setup_dma_p_video_mode(unsigned short disp_width,
			   unsigned short disp_height,
			   unsigned short img_width,
			   unsigned short img_height,
			   unsigned short hsync_porch0_fp,
			   unsigned short hsync_porch0_bp,
			   unsigned short vsync_porch0_fp,
			   unsigned short vsync_porch0_bp,
			   unsigned short hsync_width,
			   unsigned short vsync_width,
			   unsigned long input_img_addr,
			   unsigned short img_width_full_size,
			   unsigned short pack_pattern, unsigned char ystride)
{

	// unsigned long mdp_intr_status;
	int status = FAIL;
	unsigned long hsync_period;
	unsigned long vsync_period;
	unsigned long vsync_period_intmd;

	dprintf(SPEW, "MDP4.1 for DSI Video Mode\n");

	hsync_period = img_width + hsync_porch0_fp + hsync_porch0_bp + 1;
	vsync_period_intmd = img_height + vsync_porch0_fp + vsync_porch0_bp + 1;
	vsync_period = vsync_period_intmd * hsync_period;

	// ----- programming MDP_AXI_RDMASTER_CONFIG --------
	/* MDP_AXI_RDMASTER_CONFIG set all master to read from AXI port 0, that's
	   the only port connected */
	writel(0x00290000, MDP_AXI_RDMASTER_CONFIG);
	writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
	writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);
	/* sets PRIM_INTF_SEL to 0x1 and SEC_INTF_SEL to 0x2 and DSI_VIDEO_INTF_SEL */
	writel(0x00000049, MDP_DISP_INTF_SEL);
	writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

	// ------------- programming MDP_DMA_P_CONFIG ---------------------
	writel(pack_pattern << 8 | 0xbf | (0 << 25), MDP_DMA_P_CONFIG);	// rgb888

	writel(0x00000000, MDP_DMA_P_OUT_XY);
	writel(img_height << 16 | img_width, MDP_DMA_P_SIZE);
	writel(input_img_addr, MDP_DMA_P_BUF_ADDR);
	writel(img_width_full_size * ystride, MDP_DMA_P_BUF_Y_STRIDE);
	writel(0x00ff0000, MDP_DMA_P_OP_MODE);
	writel(hsync_period << 16 | hsync_width, MDP_DSI_VIDEO_HSYNC_CTL);
	writel(vsync_period, MDP_DSI_VIDEO_VSYNC_PERIOD);
	writel(vsync_width * hsync_period, MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH);
	writel((img_width + hsync_porch0_bp - 1) << 16 | hsync_porch0_bp,
	       MDP_DSI_VIDEO_DISPLAY_HCTL);
	writel(vsync_porch0_bp * hsync_period, MDP_DSI_VIDEO_DISPLAY_V_START);
	writel((img_height + vsync_porch0_bp) * hsync_period,
	       MDP_DSI_VIDEO_DISPLAY_V_END);
	writel(0x00ABCDEF, MDP_DSI_VIDEO_BORDER_CLR);
	writel(0x00000000, MDP_DSI_VIDEO_HSYNC_SKEW);
	writel(0x00000000, MDP_DSI_VIDEO_CTL_POLARITY);
	// end of cmd mdp

	writel(0x00000001, MDP_DSI_VIDEO_EN);	// MDP_DSI_EN ENABLE

	status = PASS;
	return status;
}

int mdp_dsi_cmd_config(struct msm_panel_info *pinfo,
                struct fbcon_config *fb)
{

	int ret = 0;
	unsigned long input_img_addr = MIPI_FB_ADDR;
	unsigned short image_wd = pinfo->xres;
	unsigned short image_ht = pinfo->yres;
	unsigned short pack_pattern = 0x12;
	unsigned char ystride = 3;

	writel(0x03ffffff, MDP_INTR_ENABLE);
	writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

	// ------------- programming MDP_DMA_P_CONFIG ---------------------
	writel(pack_pattern << 8 | 0x3f | (0 << 25), MDP_DMA_P_CONFIG);	// rgb888

	writel(0x00000000, MDP_DMA_P_OUT_XY);
	writel(image_ht << 16 | image_wd, MDP_DMA_P_SIZE);
	writel(input_img_addr, MDP_DMA_P_BUF_ADDR);

	writel(image_wd * ystride, MDP_DMA_P_BUF_Y_STRIDE);

	writel(0x00000000, MDP_DMA_P_OP_MODE);

	writel(0x10, MDP_DSI_CMD_MODE_ID_MAP);
	writel(0x01, MDP_DSI_CMD_MODE_TRIGGER_EN);

	writel(0x0001a000, MDP_AXI_RDMASTER_CONFIG);
	writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
	writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);
	writel(0x8a, MDP_DISP_INTF_SEL);

	return ret;
}

int
mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
		    unsigned short num_of_lanes)
{

	int status = 0;
	unsigned long input_img_addr = MIPI_FB_ADDR;
	unsigned short image_wd = mipi_fb_cfg.width;
	unsigned short image_ht = mipi_fb_cfg.height;
	unsigned short pack_pattern = 0x12;
	unsigned char ystride = 3;

	writel(0x03ffffff, MDP_INTR_ENABLE);
	writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

	// ------------- programming MDP_DMA_P_CONFIG ---------------------
	writel(pack_pattern << 8 | 0x3f | (0 << 25), MDP_DMA_P_CONFIG);	// rgb888

	writel(0x00000000, MDP_DMA_P_OUT_XY);
	writel(image_ht << 16 | image_wd, MDP_DMA_P_SIZE);
	writel(input_img_addr, MDP_DMA_P_BUF_ADDR);

	writel(image_wd * ystride, MDP_DMA_P_BUF_Y_STRIDE);

	writel(0x00000000, MDP_DMA_P_OP_MODE);

	writel(0x10, MDP_DSI_CMD_MODE_ID_MAP);
	writel(0x01, MDP_DSI_CMD_MODE_TRIGGER_EN);

	writel(0x0001a000, MDP_AXI_RDMASTER_CONFIG);
	writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
	writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);
	writel(0x8a, MDP_DISP_INTF_SEL);

	return status;
}

void mdp_disable(void)
{
	writel(0x00000000, MDP_DSI_VIDEO_EN);
}

void mdp_shutdown(void)
{
	mdp_disable();
	mdelay(60);
	writel(0x00000000, MDP_INTR_ENABLE);
	writel(0x00000003, MDP_OVERLAYPROC0_CFG);
}

int mdp_dma_on(void)
{
	int ret = 0;

	writel(0x00000001, MDP_DMA_P_START);

	return ret;
}

int mdp_dma_off(void)
{
	int ret = 0;

	writel(0x00000000, MDP_DMA_P_START);

	return ret;
}

void mdp_start_dma(void)
{
	writel(0x00000001, MDP_DMA_P_START);
}

int mdp_lcdc_config(struct msm_panel_info *pinfo,
		struct fbcon_config *fb)
{
	unsigned mdp_rgb_size;
	unsigned mdp_rgb_size_src;
	int hsync_period, vsync_period;
	int hsync_start_x, hsync_end_x;
	int display_hctl, display_vstart, display_vend;
	unsigned mdp_rgb_format = 0;

	int active_x, active_y;
	int active_hstart_x, active_hend_x;
	int active_vstart, active_vend;

	struct lcdc_panel_info *lcdc = NULL;

	if (pinfo == NULL)
		return;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return;

	mdp_rgb_size = (pinfo->yres << 16) + pinfo->xres;
	mdp_rgb_size_src = (fb->height << 16) + fb->width;

	dprintf(INFO, "Panel is %d x %d\n",
			pinfo->xres + lcdc->xres_pad,
			pinfo->yres + lcdc->yres_pad);

	/* write fb addr in MDP_DMA_P_BUF_ADDR */
	writel(fb->base, MDP_DMA_P_BUF_ADDR);

	/* write active region size*/
	writel(mdp_rgb_size, MDP_DMA_P_SIZE);

	/* set Y-stride value in bytes */
	/* Y-stride is defined as the number of bytes
	   in a line.
	 */
	writel((pinfo->xres * pinfo->bpp/8), MDP_DMA_P_BUF_Y_STRIDE);

	/* Start XY coordinates */
	writel(0, MDP_DMA_P_OUT_XY);

	if (fb->bpp == 16) {
		writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB |
			DMA_DITHER_EN |	DMA_OUT_SEL_LCDC |
			DMA_IBUF_FORMAT_RGB565 | DMA_DSTC0G_8BITS |
			DMA_DSTC1B_8BITS | DMA_DSTC2R_8BITS,
			MDP_DMA_P_CONFIG);
		mdp_rgb_format = MDP_RGB_565_FORMAT;
	} else if (fb->bpp == 24) {
		writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB |
			DMA_DITHER_EN | DMA_OUT_SEL_LCDC |
			DMA_IBUF_FORMAT_RGB888 | DMA_DSTC0G_8BITS |
			DMA_DSTC1B_8BITS | DMA_DSTC2R_8BITS,
			MDP_DMA_P_CONFIG);
		mdp_rgb_format = MDP_RGB_888_FORMAT;
	} else {
		dprintf(CRITICAL, "Unsupported bpp detected!\n");
		return ERR_INVALID_ARGS;
	}

	hsync_period = lcdc->h_pulse_width +
		lcdc->h_back_porch +
		pinfo->xres + lcdc->h_front_porch;
	vsync_period = (lcdc->v_pulse_width +
			lcdc->v_back_porch +
			pinfo->yres +
			lcdc->v_front_porch) *
		hsync_period;
	hsync_start_x =
		lcdc->h_pulse_width +
		lcdc->h_back_porch;
	hsync_end_x =
		hsync_period - lcdc->h_front_porch - 1;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;
	display_vstart = (lcdc->v_pulse_width +
			lcdc->v_back_porch)
		* hsync_period + lcdc->hsync_skew;
	display_vend = vsync_period -
		(lcdc->v_front_porch * hsync_period)
		+lcdc->hsync_skew - 1;

	active_x = (pinfo->xres - fb->width)/2;
	active_y = (pinfo->yres - fb->height)/2;

	active_hstart_x = lcdc->h_pulse_width + lcdc->h_back_porch
					  + active_x;
	active_hend_x = active_hstart_x + fb->width - 1;

	active_vstart = (lcdc->v_pulse_width + lcdc->v_back_porch
			+ active_y) * hsync_period
			+ lcdc->hsync_skew;
	active_vend = active_vstart + (fb->height * hsync_period) - 1;


	/* LCDC specific initalizations */
	writel((hsync_period << 16) | lcdc->h_pulse_width,
			MDP_LCDC_HSYNC_CTL);
	writel(vsync_period, MDP_LCDC_VSYNC_PERIOD);
	writel(lcdc->v_pulse_width * hsync_period,
			MDP_LCDC_VSYNC_PULSE_WIDTH);
	writel(display_hctl, MDP_LCDC_DISPLAY_HCTL);
	writel(display_vstart, MDP_LCDC_DISPLAY_V_START);
	writel(display_vend, MDP_LCDC_DISPLAY_V_END);

	writel(BIT(31) | (active_hend_x << 16) | active_hstart_x,
		  MDP_LCDC_ACTIVE_HCTL);

	writel(BIT(31) | active_vstart, MDP_LCDC_ACTIVE_V_START);
	writel(active_vend, MDP_LCDC_ACTIVE_V_END);

	writel(0xf, MDP_LCDC_BORDER_CLR);
	writel(0xff, MDP_LCDC_UNDERFLOW_CTL);
	writel(lcdc->hsync_skew,
			MDP_LCDC_HSYNC_SKEW);
	writel(0x3, MDP_LCDC_CTL_POLARITY);
	writel(0, MDP_LCDC_ACTIVE_HCTL);
	writel(0, MDP_LCDC_ACTIVE_V_START);
	writel(0, MDP_LCDC_ACTIVE_V_END);

	/* setting for single layer direct out mode for rgb565 source */
	writel(0x100, MDP_LAYERMIXER_IN_CFG);
	writel(mdp_rgb_size_src, MDP_RGB1_SRC_SIZE);
	writel(mdp_rgb_size, MDP_RGB1_OUT_SIZE);
	writel((int)fb->base, MDP_RGB1_SRCP0_ADDR);
	writel((fb->stride * fb->bpp / 8), MDP_RGB1_SRC_YSTRIDE1);
	writel(0x00, MDP_RGB1_CONSTANT_COLOR);
	writel(mdp_rgb_format, MDP_RGB1_SRC_FORMAT);
	writel(0x1, MDP_OVERLAYPROC0_CFG);
	if (fb->bpp == 16)
		writel(0x1, MDP_OVERLAYPROC0_OPMODE);
	else if (fb->bpp == 24)
		writel(0x0, MDP_OVERLAYPROC0_OPMODE);

	/* register flush and enable LCDC */
	writel(0x11, MDP_OVERLAY_REG_FLUSH);
	return NO_ERROR;
}

int mdp_lcdc_on()
{
	writel(0x1, MDP_LCDC_EN);
	return NO_ERROR;
}

int mdp_lcdc_off()
{
	writel(0x0, MDP_LCDC_EN);
	return NO_ERROR;
}

int mdp_dsi_video_config(struct msm_panel_info *pinfo,
		struct fbcon_config *fb)
{
	int ret = NO_ERROR;
	int hsync_period, vsync_period;
	int hsync_start_x, hsync_end_x;
	int display_hctl, display_vstart, display_vend;
	struct lcdc_panel_info *lcdc = NULL;
	unsigned mdp_rgb_size;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	hsync_period = lcdc->h_pulse_width +
		lcdc->h_back_porch +
		pinfo->xres + lcdc->xres_pad + lcdc->h_front_porch;
	vsync_period = (lcdc->v_pulse_width +
			lcdc->v_back_porch +
			pinfo->yres + lcdc->yres_pad +
			lcdc->v_front_porch) * hsync_period;
	hsync_start_x =
		lcdc->h_pulse_width +
		lcdc->h_back_porch;
	hsync_end_x =
		hsync_period - lcdc->h_front_porch - 1;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;
	display_vstart = (lcdc->v_pulse_width +
			lcdc->v_back_porch)
		* hsync_period + lcdc->hsync_skew;
	display_vend = vsync_period -
		(lcdc->v_front_porch * hsync_period)
		+lcdc->hsync_skew - 1;

	/* MDP_AXI_RDMASTER_CONFIG set all master to read from
	   AXI port 0, that's the only port connected */
	writel(0x00290000, MDP_AXI_RDMASTER_CONFIG);
	writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
	writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);

	/* Set up CMD_INTF_SEL, VIDEO_INTF_SEL,
	   EXT_INTF_SEL, SEC_INTF_SEL, PRIM_INTF_SEL */
	writel(0x00000049, MDP_DISP_INTF_SEL);

	/* DMA P */
	writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

	/* write fb addr in MDP_DMA_P_BUF_ADDR */
	writel(fb->base, MDP_DMA_P_BUF_ADDR);

	/* write active region size*/
	mdp_rgb_size = (fb->height << 16) + fb->width;
	writel(mdp_rgb_size, MDP_DMA_P_SIZE);

	/* set Y-stride value in bytes */
	/* Y-stride is defined as the number of bytes
	   in a line.
	 */
	writel((fb->stride * fb->bpp/8), MDP_DMA_P_BUF_Y_STRIDE);

	/* Start XY coordinates */
	writel(0, MDP_DMA_P_OUT_XY);

	if (fb->bpp == 16) {
		writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB |
			DMA_DITHER_EN |	DMA_OUT_SEL_LCDC |
			DMA_IBUF_FORMAT_RGB565 | DMA_DSTC0G_8BITS |
			DMA_DSTC1B_8BITS | DMA_DSTC2R_8BITS,
			MDP_DMA_P_CONFIG);
	} else if (fb->bpp == 24) {
		writel(DMA_PACK_ALIGN_LSB | DMA_PACK_PATTERN_RGB |
			DMA_DITHER_EN |	DMA_OUT_SEL_LCDC |
			DMA_IBUF_FORMAT_RGB888 | DMA_DSTC0G_8BITS |
			DMA_DSTC1B_8BITS | DMA_DSTC2R_8BITS,
			MDP_DMA_P_CONFIG);
	} else {
		dprintf(CRITICAL, "Unsupported bpp detected!\n");
		return ERR_INVALID_ARGS;
	}

	writel(hsync_period << 16 | lcdc->h_pulse_width,
		MDP_DSI_VIDEO_HSYNC_CTL);
	writel(vsync_period, MDP_DSI_VIDEO_VSYNC_PERIOD);
	writel(lcdc->v_pulse_width * hsync_period,
		MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH);
	writel(display_hctl,
		MDP_DSI_VIDEO_DISPLAY_HCTL);
	writel(display_vstart, MDP_DSI_VIDEO_DISPLAY_V_START);
	writel(display_vend,  MDP_DSI_VIDEO_DISPLAY_V_END);

	if (lcdc->xres_pad) {
		writel((1 << 31) |
			(lcdc->h_back_porch + lcdc->h_pulse_width +
			 fb->width - 1) << 16 | lcdc->h_pulse_width +
			lcdc->h_back_porch, MDP_DSI_VIDEO_ACTIVE_HCTL);
	}

	return ret;
}

int mdp_dsi_video_on()
{
	int ret = NO_ERROR;

	writel(0x00000001, MDP_DSI_VIDEO_EN);

	return ret;
}

int mdp_dsi_video_off()
{
#if (!CONT_SPLASH_SCREEN)
	writel(0x00000000, MDP_DSI_VIDEO_EN);
	mdelay(60);
	writel(0x00000000, MDP_INTR_ENABLE);
	writel(0x00000003, MDP_OVERLAYPROC0_CFG);
#endif
	return NO_ERROR;
}

int mdp_dsi_cmd_off()
{
	mdp_dma_off();
	/*
	 * Allow sometime for the DMA channel to
	 * stop the data transfer
	 */
	mdelay(10);
	writel(0x00000000, MDP_INTR_ENABLE);
	writel(0x00000003, MDP_OVERLAYPROC0_CFG);
	return NO_ERROR;
}

void mdp_set_revision(int rev)
{
	mdp_rev = rev;
}

int mdp_get_revision()
{
	return mdp_rev;
}
