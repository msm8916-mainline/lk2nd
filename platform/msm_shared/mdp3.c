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
#include <mdp3.h>
#include <debug.h>
#include <reg.h>
#include <msm_panel.h>
#include <err.h>
#include <target/display.h>
#include <platform/timer.h>
#include <platform/iomap.h>

static int mdp_rev;

int mdp_dsi_video_config(struct msm_panel_info *pinfo,
		struct fbcon_config *fb)
{
	unsigned long hsync_period;
	unsigned long vsync_period;
	unsigned long vsync_period_intmd;
	struct lcdc_panel_info *lcdc = NULL;
	int ystride = 3;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	dprintf(SPEW, "MDP3.0.3 for DSI Video Mode\n");

	hsync_period = pinfo->xres + lcdc->h_front_porch + \
			lcdc->h_back_porch + 1;
	vsync_period_intmd = pinfo->yres + lcdc->v_front_porch + \
				lcdc->v_back_porch + 1;
	vsync_period = vsync_period_intmd * hsync_period;

	// ------------- programming MDP_DMA_P_CONFIG ---------------------
	writel(0x1800bf, MDP_DMA_P_CONFIG);	// rgb888

	writel(0x00000000, MDP_DMA_P_OUT_XY);
	writel(pinfo->yres << 16 | pinfo->xres, MDP_DMA_P_SIZE);
	writel(MIPI_FB_ADDR, MDP_DMA_P_BUF_ADDR);
	writel(pinfo->xres * ystride, MDP_DMA_P_BUF_Y_STRIDE);
	writel(hsync_period << 16 | lcdc->h_pulse_width, \
			MDP_DSI_VIDEO_HSYNC_CTL);
	writel(vsync_period, MDP_DSI_VIDEO_VSYNC_PERIOD);
	writel(lcdc->v_pulse_width * hsync_period, \
			MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH);
	writel((pinfo->xres + lcdc->h_back_porch - 1) << 16 | \
			lcdc->h_back_porch, MDP_DSI_VIDEO_DISPLAY_HCTL);
	writel(lcdc->v_back_porch * hsync_period, \
			MDP_DSI_VIDEO_DISPLAY_V_START);
	writel((pinfo->yres + lcdc->v_back_porch) * hsync_period,
	       MDP_DSI_VIDEO_DISPLAY_V_END);
	writel(0x00ABCDEF, MDP_DSI_VIDEO_BORDER_CLR);
	writel(0x00000000, MDP_DSI_VIDEO_HSYNC_SKEW);
	writel(0x00000000, MDP_DSI_VIDEO_CTL_POLARITY);
	// end of cmd mdp

	return 0;
}

int mdp_dsi_cmd_config(struct msm_panel_info *pinfo,
                struct fbcon_config *fb)
{
	int ret = 0;
	unsigned short pack_pattern = 0x21;
	unsigned char ystride = 3;

	writel(0x03ffffff, MDP_INTR_ENABLE);

	// ------------- programming MDP_DMA_P_CONFIG ---------------------
	writel(pack_pattern << 8 | 0x3f | (0 << 25)| (1 << 19) | (1 << 7) , MDP_DMA_P_CONFIG);  // rgb888
	writel(0x00000000, MDP_DMA_P_OUT_XY);
	writel(pinfo->yres << 16 | pinfo->xres, MDP_DMA_P_SIZE);
	writel(MIPI_FB_ADDR, MDP_DMA_P_BUF_ADDR);

	writel(pinfo->xres * ystride, MDP_DMA_P_BUF_Y_STRIDE);

	writel(0x10, MDP_DSI_CMD_MODE_ID_MAP);
	writel(0x11, MDP_DSI_CMD_MODE_TRIGGER_EN);
	mdelay(10);

	return ret;
}

void mdp_disable(void)
{
	writel(0x00000000, MDP_DSI_VIDEO_EN);
}

int mdp_dsi_video_off(void)
{
	mdp_disable();
	mdelay(60);
	writel(0x00000000, MDP_INTR_ENABLE);
	writel(0x01ffffff, MDP_INTR_CLEAR);
	return NO_ERROR;
}

int mdp_dsi_cmd_off(void)
{
	mdp_dma_off();
	/*
	 * Allow sometime for the DMA channel to
	 * stop the data transfer
	 */
	mdelay(10);
	writel(0x00000000, MDP_INTR_ENABLE);
	writel(0x01ffffff, MDP_INTR_CLEAR);
	return NO_ERROR;
}

void mdp_set_revision(int rev)
{
	mdp_rev = rev;
}

int mdp_get_revision(void)
{
	return mdp_rev;
}

int mdp_dsi_video_on()
{
	int ret = 0;

	writel(0x00000001, MDP_DSI_VIDEO_EN);

	return ret;
}

int mdp_dma_on()
{
	int ret = 0;

	writel(0x00000001, MDP_DMA_P_START);

	return ret;
}

int mdp_dma_off()
{
	int ret = 0;

	writel(0x00000000, MDP_DMA_P_START);

	return ret;
}
