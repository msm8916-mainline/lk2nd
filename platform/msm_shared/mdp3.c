/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
#include <target/display.h>
#include <platform/timer.h>
#include <platform/iomap.h>

static int mdp_rev;

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

	dprintf(SPEW, "MDP3.0.3 for DSI Video Mode\n");

	hsync_period = img_width + hsync_porch0_fp + hsync_porch0_bp + 1;
	vsync_period_intmd = img_height + vsync_porch0_fp + vsync_porch0_bp + 1;
	vsync_period = vsync_period_intmd * hsync_period;

	// ------------- programming MDP_DMA_P_CONFIG ---------------------
	writel(0x1800bf, MDP_DMA_P_CONFIG);	// rgb888

	writel(0x00000000, MDP_DMA_P_OUT_XY);
	writel(img_height << 16 | img_width, MDP_DMA_P_SIZE);
	writel(input_img_addr, MDP_DMA_P_BUF_ADDR);
	writel(img_width_full_size * ystride, MDP_DMA_P_BUF_Y_STRIDE);
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

void mdp_disable(void)
{
	writel(0x00000000, MDP_DSI_VIDEO_EN);
}

void mdp_shutdown(void)
{
	mdp_disable();
	mdelay(60);
	writel(0x00000000, MDP_INTR_ENABLE);
	writel(0x01ffffff, MDP_INTR_CLEAR);
}

void mdp_set_revision(int rev)
{
	mdp_rev = rev;
}

int mdp_get_revision(void)
{
	return mdp_rev;
}
