/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
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

#include <reg.h>
#include <endian.h>
#include <mipi_dsi.h>
#include <dev/fbcon.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <target/display.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <err.h>
#include <msm_panel.h>

extern void mdp_disable(void);
extern int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
			       unsigned short num_of_lanes);
extern void mdp_shutdown(void);
extern void mdp_start_dma(void);
extern void dsb(void);

#if DISPLAY_MIPI_PANEL_TOSHIBA
static struct fbcon_config mipi_fb_cfg = {
	.height = TSH_MIPI_FB_HEIGHT,
	.width = TSH_MIPI_FB_WIDTH,
	.stride = TSH_MIPI_FB_WIDTH,
	.format = FB_FORMAT_RGB888,
	.bpp = 24,
	.update_start = NULL,
	.update_done = NULL,
};

struct mipi_dsi_panel_config toshiba_panel_info = {
	.mode = MIPI_VIDEO_MODE,
	.num_of_lanes = 1,
	.dsi_phy_config = &mipi_dsi_toshiba_panel_phy_ctrl,
	.panel_cmds = toshiba_panel_video_mode_cmds,
	.num_of_panel_cmds = ARRAY_SIZE(toshiba_panel_video_mode_cmds),
};
#elif DISPLAY_MIPI_PANEL_NOVATEK_BLUE
static struct fbcon_config mipi_fb_cfg = {
	.height = NOV_MIPI_FB_HEIGHT,
	.width = NOV_MIPI_FB_WIDTH,
	.stride = NOV_MIPI_FB_WIDTH,
	.format = FB_FORMAT_RGB888,
	.bpp = 24,
	.update_start = NULL,
	.update_done = NULL,
};

struct mipi_dsi_panel_config novatek_panel_info = {
	.mode = MIPI_CMD_MODE,
	.num_of_lanes = 2,
	.dsi_phy_config = &mipi_dsi_novatek_panel_phy_ctrl,
	.panel_cmds = novatek_panel_cmd_mode_cmds,
	.num_of_panel_cmds = ARRAY_SIZE(novatek_panel_cmd_mode_cmds),
};
#else
static struct fbcon_config mipi_fb_cfg = {
	.height = 0,
	.width = 0,
	.stride = 0,
	.format = 0,
	.bpp = 0,
	.update_start = NULL,
	.update_done = NULL,
};
#endif

static int cmd_mode_status = 0;
void secure_writel(uint32_t, uint32_t);
uint32_t secure_readl(uint32_t);

struct mipi_dsi_panel_config *get_panel_info(void)
{
#if  DISPLAY_MIPI_PANEL_TOSHIBA
	return &toshiba_panel_info;
#elif DISPLAY_MIPI_PANEL_NOVATEK_BLUE
	return &novatek_panel_info;
#endif
	return NULL;
}

int dsi_cmd_dma_trigger_for_panel()
{
	unsigned long ReadValue;
	unsigned long count = 0;
	int status = 0;

	writel(0x03030303, DSI_INT_CTRL);
	writel(0x1, DSI_CMD_MODE_DMA_SW_TRIGGER);
	dsb();
	ReadValue = readl(DSI_INT_CTRL) & 0x00000001;
	while (ReadValue != 0x00000001) {
		ReadValue = readl(DSI_INT_CTRL) & 0x00000001;
		count++;
		if (count > 0xffff) {
			status = FAIL;
			dprintf(CRITICAL,
				"Panel CMD: command mode dma test failed\n");
			return status;
		}
	}

	writel((readl(DSI_INT_CTRL) | 0x01000001), DSI_INT_CTRL);
	dprintf(SPEW, "Panel CMD: command mode dma tested successfully\n");
	return status;
}

int mipi_dsi_cmds_tx(struct mipi_dsi_cmd *cmds, int count)
{
	int ret = 0;
	struct mipi_dsi_cmd *cm;
	int i = 0;
	char pload[256];
	uint32_t off;

	/* Align pload at 8 byte boundry */
	off = pload;
	off &= 0x07;
	if (off)
		off = 8 - off;
	off += pload;

	cm = cmds;
	for (i = 0; i < count; i++) {
		memcpy((void *)off, (cm->payload), cm->size);
		writel(off, DSI_DMA_CMD_OFFSET);
		writel(cm->size, DSI_DMA_CMD_LENGTH);	// reg 0x48 for this build
		dsb();
		ret += dsi_cmd_dma_trigger_for_panel();
		udelay(80);
		cm++;
	}
	return ret;
}

/*
 * mipi_dsi_cmd_rx: can receive at most 16 bytes
 * per transaction since it only have 4 32bits reigsters
 * to hold data.
 * therefore Maximum Return Packet Size need to be set to 16.
 * any return data more than MRPS need to be break down
 * to multiple transactions.
 */
int mipi_dsi_cmds_rx(char **rp, int len)
{
	uint32_t *lp, data;
	char *dp;
	int i, off, cnt;
	int rlen, res;

	if (len <= 2)
		rlen = 4;	/* short read */
	else
		rlen = MIPI_DSI_MRPS + 6;	/* 4 bytes header + 2 bytes crc */

	if (rlen > MIPI_DSI_REG_LEN) {
		return 0;
	}

	res = rlen & 0x03;

	rlen += res;		/* 4 byte align */
	lp = (uint32_t *) (*rp);

	cnt = rlen;
	cnt += 3;
	cnt >>= 2;

	if (cnt > 4)
		cnt = 4;	/* 4 x 32 bits registers only */

	off = 0x068;		/* DSI_RDBK_DATA0 */
	off += ((cnt - 1) * 4);

	for (i = 0; i < cnt; i++) {
		data = (uint32_t) readl(MIPI_DSI_BASE + off);
		*lp++ = ntohl(data);	/* to network byte order */
		off -= 4;
	}

	if (len > 2) {
		/*First 4 bytes + paded bytes will be header next len bytes would be payload */
		for (i = 0; i < len; i++) {
			dp = *rp;
			dp[i] = dp[4 + res + i];
		}
	}

	return len;
}

static int mipi_dsi_cmd_bta_sw_trigger(void)
{
	uint32_t data;
	int cnt = 0;
	int err = 0;

	writel(0x01, MIPI_DSI_BASE + 0x094);	/* trigger */
	while (cnt < 10000) {
		data = readl(MIPI_DSI_BASE + 0x0004);	/*DSI_STATUS */
		if ((data & 0x0010) == 0)
			break;
		cnt++;
	}
	if (cnt == 10000)
		err = 1;
	return err;
}

static uint32_t mipi_novatek_manufacture_id(void)
{
	char rec_buf[24];
	char *rp = rec_buf;
	uint32_t *lp, data;

	mipi_dsi_cmds_tx(&novatek_panel_manufacture_id_cmd, 1);
	mipi_dsi_cmds_rx(&rp, 3);

	lp = (uint32_t *) rp;
	data = (uint32_t) * lp;
	data = ntohl(data);
	data = data >> 8;
	return data;
}

int mipi_dsi_panel_initialize(struct mipi_dsi_panel_config *pinfo)
{
	unsigned char DMA_STREAM1 = 0;	// for mdp display processor path
	unsigned char EMBED_MODE1 = 1;	// from frame buffer
	unsigned char POWER_MODE2 = 1;	// from frame buffer
	unsigned char PACK_TYPE1 = 1;	// long packet
	unsigned char VC1 = 0;
	unsigned char DT1 = 0;	// non embedded mode
	unsigned short WC1 = 0;	// for non embedded mode only
	int status = 0;
	unsigned char DLNx_EN;

	switch (pinfo->num_of_lanes) {
	default:
	case 1:
		DLNx_EN = 1;	// 1 lane
		break;
	case 2:
		DLNx_EN = 3;	// 2 lane
		break;
	case 3:
		DLNx_EN = 7;	// 3 lane
		break;
	}

	writel(0x0001, DSI_SOFT_RESET);
	writel(0x0000, DSI_SOFT_RESET);

	writel((0 << 16) | 0x3f, DSI_CLK_CTRL);	/* Turn on all DSI Clks */
	writel(DMA_STREAM1 << 8 | 0x04, DSI_TRIG_CTRL);	// reg 0x80 dma trigger: sw
	// trigger 0x4; dma stream1

	writel(0 << 30 | DLNx_EN << 4 | 0x105, DSI_CTRL);	// reg 0x00 for this
	// build
	writel(EMBED_MODE1 << 28 | POWER_MODE2 << 26
	       | PACK_TYPE1 << 24 | VC1 << 22 | DT1 << 16 | WC1,
	       DSI_COMMAND_MODE_DMA_CTRL);

	status = mipi_dsi_cmds_tx(pinfo->panel_cmds, pinfo->num_of_panel_cmds);

	return status;
}

//TODO: Clean up arguments being passed in not being used
int
config_dsi_video_mode(unsigned short disp_width, unsigned short disp_height,
		      unsigned short img_width, unsigned short img_height,
		      unsigned short hsync_porch0_fp,
		      unsigned short hsync_porch0_bp,
		      unsigned short vsync_porch0_fp,
		      unsigned short vsync_porch0_bp,
		      unsigned short hsync_width,
		      unsigned short vsync_width, unsigned short dst_format,
		      unsigned short traffic_mode, unsigned short datalane_num)
{

	unsigned char DST_FORMAT;
	unsigned char TRAFIC_MODE;
	unsigned char DLNx_EN;
	// video mode data ctrl
	int status = 0;
	unsigned long low_pwr_stop_mode = 0;
	unsigned char eof_bllp_pwr = 0x9;
	unsigned char interleav = 0;

	// disable mdp first
	mdp_disable();

	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000002, DSI_CLK_CTRL);
	writel(0x00000006, DSI_CLK_CTRL);
	writel(0x0000000e, DSI_CLK_CTRL);
	writel(0x0000001e, DSI_CLK_CTRL);
	writel(0x0000003e, DSI_CLK_CTRL);

	writel(0, DSI_CTRL);

	writel(0, DSI_ERR_INT_MASK0);

	DST_FORMAT = 0;		// RGB565
	dprintf(SPEW, "DSI_Video_Mode - Dst Format: RGB565\n");

	DLNx_EN = 1;		// 1 lane with clk programming
	dprintf(SPEW, "Data Lane: 1 lane\n");

	TRAFIC_MODE = 0;	// non burst mode with sync pulses
	dprintf(SPEW, "Traffic mode: non burst mode with sync pulses\n");

	writel(0x02020202, DSI_INT_CTRL);

	writel(((img_width + hsync_porch0_bp) << 16) | hsync_porch0_bp,
	       DSI_VIDEO_MODE_ACTIVE_H);

	writel(((img_height + vsync_porch0_bp) << 16) | (vsync_porch0_bp),
	       DSI_VIDEO_MODE_ACTIVE_V);

	writel(((img_height + vsync_porch0_fp + vsync_porch0_bp) << 16)
	       | img_width + hsync_porch0_fp + hsync_porch0_bp,
	       DSI_VIDEO_MODE_TOTAL);

	writel((hsync_width << 16) | 0, DSI_VIDEO_MODE_HSYNC);

	writel(0 << 16 | 0, DSI_VIDEO_MODE_VSYNC);

	writel(vsync_width << 16 | 0, DSI_VIDEO_MODE_VSYNC_VPOS);

	writel(1, DSI_EOT_PACKET_CTRL);

	writel(0x00000100, DSI_MISR_VIDEO_CTRL);

	writel(low_pwr_stop_mode << 16 | eof_bllp_pwr << 12 | TRAFIC_MODE << 8
	       | DST_FORMAT << 4 | 0x0, DSI_VIDEO_MODE_CTRL);

	writel(0x67, DSI_CAL_STRENGTH_CTRL);

	writel(0x80006711, DSI_CAL_CTRL);

	writel(0x00010100, DSI_MISR_VIDEO_CTRL);

	writel(0x00010100, DSI_INT_CTRL);
	writel(0x02010202, DSI_INT_CTRL);

	writel(0x02030303, DSI_INT_CTRL);

	writel(interleav << 30 | 0 << 24 | 0 << 20 | DLNx_EN << 4
	       | 0x103, DSI_CTRL);
	mdelay(10);

	return status;
}

int
config_dsi_cmd_mode(unsigned short disp_width, unsigned short disp_height,
		    unsigned short img_width, unsigned short img_height,
		    unsigned short dst_format,
		    unsigned short traffic_mode, unsigned short datalane_num)
{
	unsigned char DST_FORMAT;
	unsigned char TRAFIC_MODE;
	unsigned char DLNx_EN;
	// video mode data ctrl
	int status = 0;
	unsigned char interleav = 0;
	unsigned char ystride = 0x03;
	// disable mdp first

	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000002, DSI_CLK_CTRL);
	writel(0x00000006, DSI_CLK_CTRL);
	writel(0x0000000e, DSI_CLK_CTRL);
	writel(0x0000001e, DSI_CLK_CTRL);
	writel(0x0000003e, DSI_CLK_CTRL);

	writel(0x10000000, DSI_ERR_INT_MASK0);

	// writel(0, DSI_CTRL);

	// writel(0, DSI_ERR_INT_MASK0);

	DST_FORMAT = 8;		// RGB888
	dprintf(SPEW, "DSI_Cmd_Mode - Dst Format: RGB888\n");

	DLNx_EN = 3;		// 2 lane with clk programming
	dprintf(SPEW, "Data Lane: 2 lane\n");

	TRAFIC_MODE = 0;	// non burst mode with sync pulses
	dprintf(SPEW, "Traffic mode: non burst mode with sync pulses\n");

	writel(0x02020202, DSI_INT_CTRL);

	writel(0x00100000 | DST_FORMAT, DSI_COMMAND_MODE_MDP_CTRL);
	writel((img_width * ystride + 1) << 16 | 0x0039,
	       DSI_COMMAND_MODE_MDP_STREAM0_CTRL);
	writel((img_width * ystride + 1) << 16 | 0x0039,
	       DSI_COMMAND_MODE_MDP_STREAM1_CTRL);
	writel(img_height << 16 | img_width,
	       DSI_COMMAND_MODE_MDP_STREAM0_TOTAL);
	writel(img_height << 16 | img_width,
	       DSI_COMMAND_MODE_MDP_STREAM1_TOTAL);
	writel(0xEE, DSI_CAL_STRENGTH_CTRL);
	writel(0x80000000, DSI_CAL_CTRL);
	writel(0x40, DSI_TRIG_CTRL);
	writel(0x13c2c, DSI_COMMAND_MODE_MDP_DCS_CMD_CTRL);
	writel(interleav << 30 | 0 << 24 | 0 << 20 | DLNx_EN << 4 | 0x105,
	       DSI_CTRL);
	mdelay(10);
	writel(0x10000000, DSI_COMMAND_MODE_DMA_CTRL);
	writel(0x10000000, DSI_MISR_CMD_CTRL);
	writel(0x00000040, DSI_ERR_INT_MASK0);
	writel(0x1, DSI_EOT_PACKET_CTRL);
	// writel(0x0, MDP_OVERLAYPROC0_START);
	mdp_start_dma();
	mdelay(10);
	writel(0x1, DSI_CMD_MODE_MDP_SW_TRIGGER);

	status = 1;
	return status;
}

int mipi_dsi_video_config(unsigned short num_of_lanes)
{

	int status = 0;
	unsigned long ReadValue;
	unsigned long count = 0;
	unsigned long low_pwr_stop_mode = 0;	// low power mode 0x1111 start from
	// bit16, high spd mode 0x0
	unsigned char eof_bllp_pwr = 0x9;	// bit 12, 15, 1:low power stop mode or
	// let cmd mode eng send packets in hs
	// or lp mode
	unsigned short image_wd = mipi_fb_cfg.width;
	unsigned short image_ht = mipi_fb_cfg.height;
	unsigned short display_wd = mipi_fb_cfg.width;
	unsigned short display_ht = mipi_fb_cfg.height;
	unsigned short hsync_porch_fp = MIPI_HSYNC_FRONT_PORCH_DCLK;
	unsigned short hsync_porch_bp = MIPI_HSYNC_BACK_PORCH_DCLK;
	unsigned short vsync_porch_fp = MIPI_VSYNC_FRONT_PORCH_LINES;
	unsigned short vsync_porch_bp = MIPI_VSYNC_BACK_PORCH_LINES;
	unsigned short hsync_width = MIPI_HSYNC_PULSE_WIDTH;
	unsigned short vsync_width = MIPI_VSYNC_PULSE_WIDTH;
	unsigned short dst_format = 0;
	unsigned short traffic_mode = 0;
	unsigned short pack_pattern = 0x12;	//BGR
	unsigned char ystride = 3;

	low_pwr_stop_mode = 0x1111;	// low pwr mode bit16:HSA, bit20:HBA,
	// bit24:HFP, bit28:PULSE MODE, need enough
	// time for swithc from LP to HS
	eof_bllp_pwr = 0x9;	// low power stop mode or let cmd mode eng send
	// packets in hs or lp mode

	status +=
	    config_dsi_video_mode(display_wd, display_ht, image_wd, image_ht,
				  hsync_porch_fp, hsync_porch_bp,
				  vsync_porch_fp, vsync_porch_bp, hsync_width,
				  vsync_width, dst_format, traffic_mode,
				  num_of_lanes);

	status +=
	    mdp_setup_dma_p_video_mode(display_wd, display_ht, image_wd,
				       image_ht, hsync_porch_fp, hsync_porch_bp,
				       vsync_porch_fp, vsync_porch_bp,
				       hsync_width, vsync_width, MIPI_FB_ADDR,
				       image_wd, pack_pattern, ystride);

	ReadValue = readl(DSI_INT_CTRL) & 0x00010000;
	while (ReadValue != 0x00010000) {
		ReadValue = readl(DSI_INT_CTRL) & 0x00010000;
		count++;
		if (count > 0xffff) {
			status = FAIL;
			dprintf(CRITICAL, "Video lane test failed\n");
			return status;
		}
	}

	dprintf(SPEW, "Video lane tested successfully\n");
	return status;
}

int is_cmd_mode_enabled(void)
{
	return cmd_mode_status;
}

#if DISPLAY_MIPI_PANEL_NOVATEK_BLUE
void mipi_dsi_cmd_mode_trigger(void)
{
	int status = 0;
	unsigned short display_wd = mipi_fb_cfg.width;
	unsigned short display_ht = mipi_fb_cfg.height;
	unsigned short image_wd = mipi_fb_cfg.width;
	unsigned short image_ht = mipi_fb_cfg.height;
	unsigned short dst_format = 0;
	unsigned short traffic_mode = 0;
	struct mipi_dsi_panel_config *panel_info = &novatek_panel_info;
	status += mipi_dsi_cmd_config(mipi_fb_cfg, panel_info->num_of_lanes);
	mdelay(50);
	config_dsi_cmd_mode(display_wd, display_ht, image_wd, image_ht,
			    dst_format, traffic_mode,
			    panel_info->num_of_lanes /* num_of_lanes */ );
}
#endif

void mipi_dsi_shutdown(void)
{
#if (!CONT_SPLASH_SCREEN)
	mdp_shutdown();
	writel(0x01010101, DSI_INT_CTRL);
	writel(0x13FF3BFF, DSI_ERR_INT_MASK0);

#if (DISPLAY_MIPI_PANEL_NOVATEK_BLUE \
	 || DISPLAY_MIPI_PANEL_TOSHIBA)
	secure_writel(0x0, DSI_CC_REG);
	secure_writel(0x0, DSI_PIXEL_CC_REG);
#endif
	writel(0, DSI_CLK_CTRL);
	writel(0, DSI_CTRL);
	writel(0, DSIPHY_PLL_CTRL(0));
#else
        /* To keep the splash screen displayed till kernel driver takes
        control, do not turn off the video mode engine and clocks.
        Only disabling the MIPI DSI IRQs */
        writel(0x01010101, DSI_INT_CTRL);
        writel(0x13FF3BFF, DSI_ERR_INT_MASK0);
#endif
}

struct fbcon_config *mipi_init(void)
{
	int status = 0;
	struct mipi_dsi_panel_config *panel_info = get_panel_info();

	if (panel_info == NULL) {
		dprintf(CRITICAL, "Panel info is null\n");
		return NULL;
	}

	/* Enable MMSS_AHB_ARB_MATER_PORT_E for arbiter master0 and master 1 request */
#if (!DISPLAY_MIPI_PANEL_RENESAS)
	writel(0x00001800, MMSS_SFPB_GPREG);
#endif

#if DISPLAY_MIPI_PANEL_TOSHIBA_MDT61
	mipi_dsi_phy_init(panel_info);
#else
	mipi_dsi_phy_ctrl_config(panel_info);
#endif

	status += mipi_dsi_panel_initialize(panel_info);

#if DISPLAY_MIPI_PANEL_NOVATEK_BLUE
	mipi_dsi_cmd_bta_sw_trigger();
	mipi_novatek_manufacture_id();
#endif
	mipi_fb_cfg.base = MIPI_FB_ADDR;

	if (panel_info->mode == MIPI_VIDEO_MODE)
		status += mipi_dsi_video_config(panel_info->num_of_lanes);

	if (panel_info->mode == MIPI_CMD_MODE)
		cmd_mode_status = 1;

	return &mipi_fb_cfg;
}

int mipi_config(struct msm_fb_panel_data *panel)
{
	int ret = NO_ERROR;
	struct msm_panel_info *pinfo;
	struct mipi_dsi_panel_config mipi_pinfo;

	if (!panel)
		return ERR_INVALID_ARGS;

	pinfo = &(panel->panel_info);
	mipi_pinfo.mode = pinfo->mipi.mode;
	mipi_pinfo.num_of_lanes = pinfo->mipi.num_of_lanes;
	mipi_pinfo.dsi_phy_config = pinfo->mipi.dsi_phy_db;
	mipi_pinfo.panel_cmds = pinfo->mipi.panel_cmds;
	mipi_pinfo.num_of_panel_cmds = pinfo->mipi.num_of_panel_cmds;
	mipi_pinfo.lane_swap = pinfo->mipi.lane_swap;

	/* Enable MMSS_AHB_ARB_MATER_PORT_E for
	   arbiter master0 and master 1 request */
#if (!DISPLAY_MIPI_PANEL_RENESAS)
	writel(0x00001800, MMSS_SFPB_GPREG);
#endif

	mipi_dsi_phy_init(&mipi_pinfo);

	ret += mipi_dsi_panel_initialize(&mipi_pinfo);

	if (pinfo->rotate && panel->rotate)
		pinfo->rotate();

	return ret;
}

int mipi_dsi_video_mode_config(unsigned short disp_width,
	unsigned short disp_height,
	unsigned short img_width,
	unsigned short img_height,
	unsigned short hsync_porch0_fp,
	unsigned short hsync_porch0_bp,
	unsigned short vsync_porch0_fp,
	unsigned short vsync_porch0_bp,
	unsigned short hsync_width,
	unsigned short vsync_width,
	unsigned short dst_format,
	unsigned short traffic_mode,
	unsigned char lane_en,
	unsigned low_pwr_stop_mode,
	unsigned char eof_bllp_pwr,
	unsigned char interleav)
{

	int status = 0;

	/* disable mdp first */
	mdp_disable();

	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000002, DSI_CLK_CTRL);
	writel(0x00000006, DSI_CLK_CTRL);
	writel(0x0000000e, DSI_CLK_CTRL);
	writel(0x0000001e, DSI_CLK_CTRL);
	writel(0x0000003e, DSI_CLK_CTRL);

	writel(0, DSI_CTRL);

	writel(0, DSI_ERR_INT_MASK0);

	writel(0x02020202, DSI_INT_CTRL);

	writel(((disp_width + hsync_porch0_bp) << 16) | hsync_porch0_bp,
			DSI_VIDEO_MODE_ACTIVE_H);

	writel(((disp_height + vsync_porch0_bp) << 16) | (vsync_porch0_bp),
			DSI_VIDEO_MODE_ACTIVE_V);

	if (mdp_get_revision() >= MDP_REV_41) {
		writel(((disp_height + vsync_porch0_fp
			+ vsync_porch0_bp - 1) << 16)
			| (disp_width + hsync_porch0_fp
			+ hsync_porch0_bp - 1),
			DSI_VIDEO_MODE_TOTAL);
	} else {
		writel(((disp_height + vsync_porch0_fp
			+ vsync_porch0_bp) << 16)
			| (disp_width + hsync_porch0_fp
			+ hsync_porch0_bp),
			DSI_VIDEO_MODE_TOTAL);
	}

	writel((hsync_width << 16) | 0, DSI_VIDEO_MODE_HSYNC);

	writel(0 << 16 | 0, DSI_VIDEO_MODE_VSYNC);

	writel(vsync_width << 16 | 0, DSI_VIDEO_MODE_VSYNC_VPOS);

	writel(1, DSI_EOT_PACKET_CTRL);

	writel(0x00000100, DSI_MISR_VIDEO_CTRL);

	if (mdp_get_revision() >= MDP_REV_41) {
		writel(low_pwr_stop_mode << 16 |
				eof_bllp_pwr << 12 | traffic_mode << 8
				| dst_format << 4 | 0x0, DSI_VIDEO_MODE_CTRL);
	} else {
		writel(1 << 28 | 1 << 24 | 1 << 20 | low_pwr_stop_mode << 16 |
				eof_bllp_pwr << 12 | traffic_mode << 8
				| dst_format << 4 | 0x0, DSI_VIDEO_MODE_CTRL);
	}

	writel(0x67, DSI_CAL_STRENGTH_CTRL);
	writel(0x80006711, DSI_CAL_CTRL);
	writel(0x00010100, DSI_MISR_VIDEO_CTRL);

	writel(0x00010100, DSI_INT_CTRL);
	writel(0x02010202, DSI_INT_CTRL);
	writel(0x02030303, DSI_INT_CTRL);

	writel(interleav << 30 | 0 << 24 | 0 << 20 | lane_en << 4
			| 0x103, DSI_CTRL);

	return status;
}

int mipi_dsi_cmd_mode_config(unsigned short disp_width,
	unsigned short disp_height,
	unsigned short img_width,
	unsigned short img_height,
	unsigned short dst_format,
	unsigned short traffic_mode)
{
	unsigned char DST_FORMAT;
	unsigned char TRAFIC_MODE;
	unsigned char DLNx_EN;
	// video mode data ctrl
	int status = 0;
	unsigned char interleav = 0;
	unsigned char ystride = 0x03;
	// disable mdp first

	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000000, DSI_CLK_CTRL);
	writel(0x00000002, DSI_CLK_CTRL);
	writel(0x00000006, DSI_CLK_CTRL);
	writel(0x0000000e, DSI_CLK_CTRL);
	writel(0x0000001e, DSI_CLK_CTRL);
	writel(0x0000003e, DSI_CLK_CTRL);

	writel(0x10000000, DSI_ERR_INT_MASK0);


	DST_FORMAT = 8;		// RGB888
	dprintf(SPEW, "DSI_Cmd_Mode - Dst Format: RGB888\n");

	DLNx_EN = 3;		// 2 lane with clk programming
	dprintf(SPEW, "Data Lane: 2 lane\n");

	TRAFIC_MODE = 0;	// non burst mode with sync pulses
	dprintf(SPEW, "Traffic mode: non burst mode with sync pulses\n");

	writel(0x02020202, DSI_INT_CTRL);

	writel(0x00100000 | DST_FORMAT, DSI_COMMAND_MODE_MDP_CTRL);
	writel((img_width * ystride + 1) << 16 | 0x0039,
	       DSI_COMMAND_MODE_MDP_STREAM0_CTRL);
	writel((img_width * ystride + 1) << 16 | 0x0039,
	       DSI_COMMAND_MODE_MDP_STREAM1_CTRL);
	writel(img_height << 16 | img_width,
	       DSI_COMMAND_MODE_MDP_STREAM0_TOTAL);
	writel(img_height << 16 | img_width,
	       DSI_COMMAND_MODE_MDP_STREAM1_TOTAL);
	writel(0xEE, DSI_CAL_STRENGTH_CTRL);
	writel(0x80000000, DSI_CAL_CTRL);
	writel(0x40, DSI_TRIG_CTRL);
	writel(0x13c2c, DSI_COMMAND_MODE_MDP_DCS_CMD_CTRL);
	writel(interleav << 30 | 0 << 24 | 0 << 20 | DLNx_EN << 4 | 0x105,
	       DSI_CTRL);
	writel(0x10000000, DSI_COMMAND_MODE_DMA_CTRL);
	writel(0x10000000, DSI_MISR_CMD_CTRL);
	writel(0x00000040, DSI_ERR_INT_MASK0);
	writel(0x1, DSI_EOT_PACKET_CTRL);

	return NO_ERROR;
}

int mipi_dsi_on()
{
	int ret = NO_ERROR;
	unsigned long ReadValue;
	unsigned long count = 0;

	ReadValue = readl(DSI_INT_CTRL) & 0x00010000;

	mdelay(10);

	while (ReadValue != 0x00010000) {
		ReadValue = readl(DSI_INT_CTRL) & 0x00010000;
		count++;
		if (count > 0xffff) {
			dprintf(CRITICAL, "Video lane test failed\n");
			return ERROR;
		}
	}

	dprintf(SPEW, "Video lane tested successfully\n");
	return ret;
}

int mipi_dsi_off()
{
	writel(0x01010101, DSI_INT_CTRL);
	writel(0x13FF3BFF, DSI_ERR_INT_MASK0);

#if (!CONT_SPLASH_SCREEN)
	writel(0, DSI_CLK_CTRL);
	writel(0, DSI_CTRL);
	writel(0, DSIPHY_PLL_CTRL(0));
#endif

	return NO_ERROR;
}

int mipi_cmd_trigger()
{
	writel(0x1, DSI_CMD_MODE_MDP_SW_TRIGGER);

	return NO_ERROR;
}
