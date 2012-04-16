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
#ifndef _PLATFORM_MSM_SHARED_MDP_4_H_
#define _PLATFORM_MSM_SHARED_MDP_4_H_

#include <dev/fbcon.h>
#include <msm_panel.h>

//TODO: Make a global PASS / FAIL define
#define PASS                        0
#define FAIL                        1

#define MDP_OVERLAYPROC0_START                REG_MDP(0x00004)
#define MDP_DMA_P_START                       REG_MDP(0x0000C)
#define MDP_DMA_S_START                       REG_MDP(0x00010)
#define MDP_AXI_RDMASTER_CONFIG               REG_MDP(0x00028)
#define MDP_AXI_WRMASTER_CONFIG               REG_MDP(0x00030)
#define MDP_DISP_INTF_SEL                     REG_MDP(0x00038)
#define MDP_MAX_RD_PENDING_CMD_CONFIG         REG_MDP(0x0004C)
#define MDP_INTR_ENABLE                       REG_MDP(0x00050)
#define MDP_DSI_CMD_MODE_ID_MAP               REG_MDP(0x000A0)
#define MDP_DSI_CMD_MODE_TRIGGER_EN           REG_MDP(0x000A4)
#define MDP_OVERLAYPROC0_CFG                  REG_MDP(0x10004)
#define MDP_OVERLAYPROC0_OPMODE               REG_MDP(0x10014)
#define MDP_LAYERMIXER_IN_CFG                 REG_MDP(0x10100)
#define MDP_OVERLAY_REG_FLUSH                 REG_MDP(0x18000)

#define MDP_RGB1_SRC_SIZE                     REG_MDP(0x40000)
#define MDP_RGB1_SRC_XY                       REG_MDP(0x40004)
#define MDP_RGB1_OUT_SIZE                     REG_MDP(0x40008)
#define MDP_RGB1_OUT_XY                       REG_MDP(0x4000C)
#define MDP_RGB1_SRCP0_ADDR                   REG_MDP(0x40010)
#define MDP_RGB1_SRCP0_ADDR_1                 REG_MDP(0x40014)
#define MDP_RGB1_SRCP0_ADDR_2                 REG_MDP(0x40018)
#define MDP_RGB1_SRC_YSTRIDE1                 REG_MDP(0x40040)
#define MDP_RGB1_SRC_FORMAT                   REG_MDP(0x40050)
#define MDP_RGB1_CONSTANT_COLOR               REG_MDP(0x41008)

#define MDP_DMA_P_CONFIG                      REG_MDP(0x90000)
#define MDP_DMA_P_OUT_XY                      REG_MDP(0x90010)
#define MDP_DMA_P_SIZE                        REG_MDP(0x90004)
#define MDP_DMA_P_BUF_ADDR                    REG_MDP(0x90008)
#define MDP_DMA_P_BUF_Y_STRIDE                REG_MDP(0x9000C)
#define MDP_DMA_P_OP_MODE                     REG_MDP(0x90070)

#define MDP_LCDC_EN                           REG_MDP(0xC0000)
#define MDP_LCDC_HSYNC_CTL                    REG_MDP(0xC0004)
#define MDP_LCDC_VSYNC_PERIOD                 REG_MDP(0xC0008)
#define MDP_LCDC_VSYNC_PULSE_WIDTH            REG_MDP(0xC000C)
#define MDP_LCDC_DISPLAY_HCTL                 REG_MDP(0xC0010)
#define MDP_LCDC_DISPLAY_V_START              REG_MDP(0xC0014)
#define MDP_LCDC_DISPLAY_V_END                REG_MDP(0xC0018)
#define MDP_LCDC_ACTIVE_HCTL                  REG_MDP(0xC001C)
#define MDP_LCDC_ACTIVE_V_START               REG_MDP(0xC0020)
#define MDP_LCDC_ACTIVE_V_END                 REG_MDP(0xC0024)
#define MDP_LCDC_BORDER_CLR                   REG_MDP(0xC0028)
#define MDP_LCDC_UNDERFLOW_CTL                REG_MDP(0xC002C)
#define MDP_LCDC_HSYNC_SKEW                   REG_MDP(0xC0030)
#define MDP_LCDC_TEST_CTL                     REG_MDP(0xC0034)
#define MDP_LCDC_CTL_POLARITY                 REG_MDP(0xC0038)
#define MDP_LCDC_TEST_COL_VAR1                REG_MDP(0xC003C)
#define MDP_LCDC_UNDERFLOW_HIDING_CTL         REG_MDP(0xC0044)
#define MDP_LCDC_LOST_PIXEL_CNT_VALUE         REG_MDP(0xC0048)

#define MDP_DSI_VIDEO_EN                      REG_MDP(0xE0000)
#define MDP_DSI_VIDEO_HSYNC_CTL               REG_MDP(0xE0004)
#define MDP_DSI_VIDEO_VSYNC_PERIOD            REG_MDP(0xE0008)
#define MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH       REG_MDP(0xE000C)
#define MDP_DSI_VIDEO_DISPLAY_HCTL            REG_MDP(0xE0010)
#define MDP_DSI_VIDEO_DISPLAY_V_START         REG_MDP(0xE0014)
#define MDP_DSI_VIDEO_DISPLAY_V_END           REG_MDP(0xE0018)
#define MDP_DSI_VIDEO_ACTIVE_HCTL             REG_MDP(0xE001C)
#define MDP_DSI_VIDEO_BORDER_CLR              REG_MDP(0xE0028)
#define MDP_DSI_VIDEO_HSYNC_SKEW              REG_MDP(0xE0030)
#define MDP_DSI_VIDEO_CTL_POLARITY            REG_MDP(0xE0038)
#define MDP_DSI_VIDEO_TEST_CTL                REG_MDP(0xE0034)

#define MDP_TEST_MODE_CLK                     REG_MDP(0xF0000)
#define MDP_INTR_STATUS                       REG_MDP(0x00054)

#define DMA_DSTC0G_8BITS (BIT(1)|BIT(0))
#define DMA_DSTC1B_8BITS (BIT(3)|BIT(2))
#define DMA_DSTC2R_8BITS (BIT(5)|BIT(4))
#define CLR_G 0x0
#define CLR_B 0x1
#define CLR_R 0x2
#define MDP_GET_PACK_PATTERN(a, x, y, z, bit)	\
	(((a)<<(bit*3))|((x)<<(bit*2))|((y)<<bit)|(z))
#define DMA_PACK_ALIGN_LSB 0
#define DMA_PACK_PATTERN_RGB					\
	(MDP_GET_PACK_PATTERN(0, CLR_R, CLR_G, CLR_B, 2)<<8)
#define DMA_DITHER_EN			BIT(24)
#define DMA_OUT_SEL_LCDC		BIT(20)
#define DMA_IBUF_FORMAT_RGB888	(0 << 25)
#define DMA_IBUF_FORMAT_RGB565	(1 << 25)
#define MDP_RGB_888_FORMAT	\
	(BIT(17) | (1<<14) | (2<<9) | \
	(0<<8) | (0<<6) | (3<<4) | \
	(3<<2) | (3<<0))
#define MDP_RGB_565_FORMAT	\
	(BIT(17) | (1<<14) | (1<<9) | \
	(0<<8) | (0<<6) | (1<<4) | \
	(1<<2) | (2<<0))

void mdp_setup_dma_p_video_config(unsigned short pack_pattern,
				unsigned short img_width,
				unsigned short img_height,
				unsigned long input_img_addr,
				unsigned short img_width_full_size,
				unsigned char ystride);
int mdp_setup_dma_p_video_mode(unsigned short disp_width,
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
				unsigned short pack_pattern,
				unsigned char ystride);
int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
			unsigned short num_of_lanes);
void mdp_shutdown(void);
void mdp_disable(void);
void mdp_start_dma(void);
int mdp_dsi_video_off();
int mdp_dsi_video_on();
int mdp_dsi_video_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_lcdc_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_lcdc_on();
int mdp_lcdc_off();
void mdp_set_revision(int rev);
int mdp_get_revision();
#endif
