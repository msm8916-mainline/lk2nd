/* Copyright (c) 2010-2016, The Linux Foundation. All rights reserved.
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

#include <reg.h>
#include <endian.h>
#include <mipi_dsi.h>
#include <dev/fbcon.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <target/display.h>
#include <mdp5.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <err.h>
#include <msm_panel.h>
#include <arch/ops.h>

extern int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
			       unsigned short num_of_lanes);
extern void mdp_shutdown(void);
extern void mdp_start_dma(void);

#if (DISPLAY_TYPE_MDSS == 0)
#define MIPI_DSI0_BASE MIPI_DSI_BASE
#define MIPI_DSI1_BASE MIPI_DSI_BASE
#endif

struct fbcon_config mipi_fb_cfg = {
	.height = 0,
	.width = 0,
	.stride = 0,
	.format = 0,
	.bpp = 0,
	.update_start = NULL,
	.update_done = NULL,
};

static char read_id_a1h_cmd[4] = { 0xA1, 0x00, 0x06, 0xA0 };	/* DTYPE_DCS_READ */
static struct mipi_dsi_cmd read_ddb_start_cmd =
	{sizeof(read_id_a1h_cmd), read_id_a1h_cmd,  0x00};

void secure_writel(uint32_t, uint32_t);
uint32_t secure_readl(uint32_t);

static uint32_t response_value = 0;

static uint32_t mdss_dsi_read_panel_signature(struct mipi_panel_info *mipi)
{
	uint32_t rec_buf[1];
	uint32_t *lp = rec_buf, data;
	uint32_t ret = response_value;
	uint32_t panel_signature = mipi->signature;

#if (DISPLAY_TYPE_MDSS == 1)
	if (ret && ret != panel_signature)
		goto exit_read_signature;

	ret = mdss_dsi_cmds_tx(mipi, &read_ddb_start_cmd, 1, 0);
	if (ret)
		goto exit_read_signature;
	if (!mdss_dsi_cmds_rx(mipi, &lp, 1, 1))
		goto exit_read_signature;

	data = ntohl(*lp);
	data = data >> 8;
	response_value = data;
	if (response_value != panel_signature)
		ret = response_value;

exit_read_signature:
	/* Keep the non detectable panel at the end and set panel signature 0xFFFF */
	if ((panel_signature == 0) || (panel_signature == 0xFFFF))
		ret = 0;
#endif
	return ret;
}

static int mdss_dsi_cmd_dma_trigger_for_panel(char dual_dsi,
	uint32_t ctl_base, uint32_t sctl_base)
{
	uint32_t ReadValue;
	uint32_t count = 0;
	int status = 0;
	uint32_t base = dual_dsi ? sctl_base : ctl_base;

#if (DISPLAY_TYPE_MDSS == 1)
	writel(0x03030303, ctl_base + INT_CTRL);
	writel(0x1, ctl_base + CMD_MODE_DMA_SW_TRIGGER);
	dsb();

	if (dual_dsi) {
		writel(0x03030303, sctl_base + INT_CTRL);
		writel(0x1, sctl_base + CMD_MODE_DMA_SW_TRIGGER);
		dsb();
	}

	ReadValue = readl(base + INT_CTRL) & 0x00000001;
	while (ReadValue != 0x00000001) {
		ReadValue = readl(base + INT_CTRL) & 0x00000001;
		count++;
		if (count > 0xffff) {
			status = FAIL;
			dprintf(CRITICAL,
				"Panel CMD: command mode dma test failed\n");
			return status;
		}
	}

	writel((readl(base + INT_CTRL) | 0x01000001), base + INT_CTRL);
	dprintf(SPEW, "Panel CMD: command mode dma tested successfully\n");
#endif
	return status;
}

static int mdss_dsi_wait4_video_done(uint32_t ctl_base)
{
	unsigned long read;
	unsigned long count = 0;
	int status = 0;

	/* If video mode is not enabled, return here */
	if ((readl(ctl_base + CTRL) & BIT(1)) == 0)
		return 0;

	read = readl(ctl_base + INT_CTRL);
	/* Enable VIDEO MODE DONE MASK and clear the interrupt */
	read = read | DSI_VIDEO_MODE_DONE_MASK | DSI_VIDEO_MODE_DONE_AK;
	writel(read, ctl_base + INT_CTRL);
	dsb();
	do {
		read = readl(ctl_base + INT_CTRL) &
			DSI_VIDEO_MODE_DONE_STAT;
		count++;
		if (count > 0xffff) {
			status = FAIL;
			dprintf(CRITICAL,
				"Panel CMD: Did not recieve video mode done interrupt\n");
			return status;
		}
	} while (!read);

	writel((readl(ctl_base + INT_CTRL) | 0x01000001), ctl_base + INT_CTRL);
	dprintf(SPEW, "Panel wait_4_video_done: Recieved video mode done ack\n");

	/* Skip BLLP 4ms */
	mdelay(4);

	return status;
}

int mdss_dsi_cmds_tx(struct mipi_panel_info *mipi,
	struct mipi_dsi_cmd *cmds, int count, char dual_dsi)
{
	int ret = 0;
#if (DISPLAY_TYPE_MDSS == 1)
	struct mipi_dsi_cmd *cm;
	int i = 0;
	uint8_t pload[256];
	uint32_t off;
	uint32_t size;
	uint32_t ctl_base, sctl_base;

	/* if dest controller is not specified, default to DSI0 */
	if (!mipi) {
		ctl_base = MIPI_DSI0_BASE;
		sctl_base = MIPI_DSI1_BASE;
	} else {
		ctl_base = mipi->ctl_base;
		sctl_base = mipi->sctl_base;
	}

	/* Align pload at 8 byte boundary */
	off = (uint32_t) pload;
	off &= 0x07;
	if (off)
		off = 8 - off;
	off += (uint32_t) pload;

	cm = cmds;
	for (i = 0; i < count; i++) {
		/* Wait for VIDEO_MODE_DONE */
		ret = mdss_dsi_wait4_video_done(ctl_base);
		if (ret)
			goto wait4video_error;

		/* The payload size has to be a multiple of 4 */
		size = cm->size;
		size &= 0x03;
		if (size)
			size = 4 - size;
		size += cm->size;
		memcpy((uint8_t *)off, (cm->payload), size);
		arch_clean_invalidate_cache_range((addr_t)(off), size);
		writel(off, ctl_base + DMA_CMD_OFFSET);
		writel(size, ctl_base + DMA_CMD_LENGTH);
		if (dual_dsi) {
			writel(off, sctl_base + DMA_CMD_OFFSET);
			writel(size, sctl_base + DMA_CMD_LENGTH);
		}
		dsb();
		ret += mdss_dsi_cmd_dma_trigger_for_panel(dual_dsi, ctl_base,
			sctl_base);
		if (cm->wait)
			mdelay(cm->wait);
		else
			udelay(80);
		cm++;
	}
wait4video_error:
#endif
	return ret;
}

int mdss_dsi_cmds_rx(struct mipi_panel_info *mipi, uint32_t **rp, int rp_len,
	int rdbk_len)
{
	uint32_t *lp, data;
	uint32_t *dp;
	int i, off;
	int rlen, res;
	uint32_t ctl_base;

	/* if dest controller is not specified, default to DSI0 */
	if (!mipi)
		ctl_base = MIPI_DSI0_BASE;
	else
		ctl_base = mipi->ctl_base;

	if (rdbk_len > rp_len) {
		return 0;
	}

	if (rdbk_len <= 2)
		rlen = 4;	/* short read */
	else
		rlen = MIPI_DSI_MRPS + 6;	/* 4 bytes header + 2 bytes crc */

	if (rlen > MIPI_DSI_REG_LEN) {
		return 0;
	}

	res = rlen & 0x03;

	rlen += res;		/* 4 byte align */
	lp = *rp;

	rlen += 3;
	rlen >>= 2;

	if (rlen > 4)
		rlen = 4;	/* 4 x 32 bits registers only */

	off = RDBK_DATA0;
	off += ((rlen - 1) * 4);

	for (i = 0; i < rlen; i++) {
		data = readl(ctl_base + off);
		*lp = ntohl(data);	/* to network byte order */
		lp++;

		off -= 4;
	}

	if (rdbk_len > 2) {
		/*First 4 bytes + paded bytes will be header next len bytes would be payload */
		for (i = 0; i < rdbk_len; i++) {
			dp = *rp;
			dp[i] = dp[(res + i) >> 2];
		}
	}
	return rdbk_len;
}

static int mdss_dsi_cmd_bta_sw_trigger(uint32_t ctl_base)
{
	uint32_t data;
	int cnt = 0;
	int err = 0;

	writel(0x01, ctl_base + 0x094);	/* trigger */
	while (cnt < 10000) {
		data = readl(ctl_base + 0x0004);	/*DSI_STATUS */
		if ((data & 0x0010) == 0)
			break;
		cnt++;
	}
	if (cnt == 10000)
		err = 1;
	return err;
}

static void mdss_dsi_force_clk_lane_hs(struct mipi_panel_info *mipi,
		uint32_t dual_dsi)
{
	uint32_t tmp;

	if (dual_dsi) {
		tmp = readl(mipi->sctl_base + LANE_CTL);
		tmp |= BIT(28);
		writel(tmp, mipi->sctl_base + LANE_CTL);
	}

	tmp = readl(mipi->ctl_base + LANE_CTL);
	tmp |= BIT(28);
	writel(tmp, mipi->ctl_base + LANE_CTL);
}

int mdss_dsi_host_init(struct mipi_panel_info *mipi, uint32_t
		dual_dsi, uint32_t broadcast)
{
	uint8_t DMA_STREAM1 = 0;	// for mdp display processor path
	uint8_t EMBED_MODE1 = 1;	// from frame buffer
	uint8_t POWER_MODE2 = 1;	// from frame buffer
	uint8_t PACK_TYPE1 = 0;		// long packet
	uint8_t VC1 = 0;
	uint8_t DT1 = 0;	// non embedded mode
	uint8_t WC1 = 0;	// for non embedded mode only
	uint8_t DLNx_EN;
	uint8_t lane_swap = 0;
	uint32_t timing_ctl = 0;
	uint32_t lane_swap_dsi1 = 0;
	uint32_t ctrl_mode = 0x105;	//Default is command mode to send cmds.

#if (DISPLAY_TYPE_MDSS == 1)
	switch (mipi->num_of_lanes) {
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
	case 4:
		DLNx_EN = 0x0F;	/* 4 lanes */
		break;
	}

	lane_swap = mipi->lane_swap;
	timing_ctl = ((mipi->t_clk_post << 8) | mipi->t_clk_pre);

	if (mipi->cmds_post_tg) {
		/*
		 * Need to send pixel data before sending the ON commands
		 * so need to configure controller to VIDEO MODE.
		 */
		ctrl_mode = 0x103;
	}

	if (dual_dsi) {
		writel(0x0001, mipi->sctl_base + SOFT_RESET);
		writel(0x0000, mipi->sctl_base + SOFT_RESET);

		/* Turn on all DSI Clks */
		writel((0 << 16) | 0x3f, mipi->sctl_base + CLK_CTRL);
		writel(DMA_STREAM1 << 8 | 0x04, mipi->sctl_base + TRIG_CTRL);

		writel(0 << 30 | DLNx_EN << 4 | ctrl_mode, mipi->sctl_base + CTRL);
		writel(broadcast << 31 | EMBED_MODE1 << 28 | POWER_MODE2 << 26
				| PACK_TYPE1 << 24 | VC1 << 22 | DT1 << 16 | WC1,
				mipi->sctl_base + COMMAND_MODE_DMA_CTRL);

		/* for 8939 hw dsi1 has Lane_map as 3210 */
		if (readl(mipi->ctl_base) == DSI_HW_REV_103_1)
			lane_swap_dsi1 = 0x7;
		else
			lane_swap_dsi1 = lane_swap;
		writel(lane_swap_dsi1, mipi->sctl_base + LANE_SWAP_CTL);
		writel(timing_ctl, mipi->sctl_base + TIMING_CTL);

		if ((mipi->mode == DSI_CMD_MODE) &&
				(readl(mipi->sctl_base) >= DSI_HW_REV_103)) {
			uint32_t tmp;
			tmp = readl(mipi->sctl_base + 0x01b8);
			tmp |= BIT(16); /* enable burst mode */
			writel(tmp, mipi->sctl_base + 0x01b8);
		}

		writel(((mipi->rx_eot_ignore & 0x1) << 4) |
			(mipi->tx_eot_append & 0x1),
			mipi->sctl_base + EOT_PACKET_CTRL);
	}

	writel(0x0001, mipi->ctl_base + SOFT_RESET);
	writel(0x0000, mipi->ctl_base + SOFT_RESET);

	/* Turn on all DSI Clks */
	writel((0 << 16) | 0x3f, mipi->ctl_base + CLK_CTRL);
	writel(DMA_STREAM1 << 8 | 0x04, mipi->ctl_base + TRIG_CTRL);

	writel(0 << 30 | DLNx_EN << 4 | ctrl_mode, mipi->ctl_base + CTRL);
	writel(broadcast << 31 | EMBED_MODE1 << 28 | POWER_MODE2 << 26
	       | PACK_TYPE1 << 24 | VC1 << 22 | DT1 << 16 | WC1,
	       mipi->ctl_base + COMMAND_MODE_DMA_CTRL);

	writel(lane_swap, mipi->ctl_base + LANE_SWAP_CTL);
	writel(timing_ctl, mipi->ctl_base + TIMING_CTL);

	if ((mipi->mode == DSI_CMD_MODE) &&
			(readl(mipi->ctl_base) >= DSI_HW_REV_103)) {
		uint32_t tmp;
		tmp = readl(mipi->ctl_base + 0x01b8);
		tmp |= BIT(16); /* enable burst mode */
		writel(tmp, mipi->ctl_base + 0x01b8);
	}

	writel(((mipi->rx_eot_ignore & 0x1) << 4) | (mipi->tx_eot_append & 0x1),
		mipi->ctl_base + EOT_PACKET_CTRL);

#endif

	return 0;
}

void mdss_dsi_panel_shutdown(struct msm_panel_info *pinfo)
{
#if (DISPLAY_TYPE_MDSS == 1)
	unsigned long read_val = 0;
	uint32_t ctl_base = pinfo->mipi.ctl_base;
	uint32_t sctl_base = pinfo->mipi.sctl_base;

	if (pinfo->mipi.panel_off_cmds) {
		/*
		 * Once MDP TG is disabled, reset of DSI controller is
		 * needed before we send panel OFF commands.
		 */
		if (pinfo->type == MIPI_VIDEO_PANEL) {
			read_val = readl(ctl_base + CTRL);
			writel((read_val & ~BIT(0)), ctl_base + CTRL);
			writel(0x0001, ctl_base + SOFT_RESET);
			dsb();
			writel(0x0000, ctl_base + SOFT_RESET);
			dsb();
			/* Enable cmd mode only */
			writel(((read_val & ~BIT(1)) | BIT(2)),
				ctl_base + CTRL);
		}

		if (pinfo->mipi.broadcast) {
			if (pinfo->type == MIPI_VIDEO_PANEL) {
				read_val = readl(sctl_base + CTRL);
				writel((read_val & ~BIT(0)),
					sctl_base + CTRL);

				writel(0x0001, sctl_base + SOFT_RESET);
				dsb();
				writel(0x0000, sctl_base + SOFT_RESET);
				dsb();

				writel(((read_val & ~BIT(1)) | BIT(2)),
					sctl_base + CTRL);
			}
		}
		mdss_dsi_cmds_tx(&pinfo->mipi, pinfo->mipi.panel_off_cmds,
			pinfo->mipi.num_of_panel_off_cmds,
			pinfo->mipi.broadcast);
	}
#endif
}

int mdss_dsi_panel_initialize(struct mipi_panel_info *mipi, uint32_t
		broadcast)
{
	int status = 0;
	uint32_t ctrl_mode = 0;

#if (DISPLAY_TYPE_MDSS == 1)
	if (!mipi->panel_on_cmds)
		goto end;

	ctrl_mode = readl(mipi->ctl_base + CTRL);

	/* Enable command mode before sending the commands. */
	writel(ctrl_mode | 0x04, mipi->ctl_base + CTRL);
	if (broadcast)
		writel(ctrl_mode | 0x04, mipi->sctl_base + CTRL);
	status = mdss_dsi_cmds_tx(mipi, mipi->panel_on_cmds,
			mipi->num_of_panel_on_cmds, broadcast);
	writel(ctrl_mode, mipi->ctl_base + CTRL);
	if (broadcast)
		writel(ctrl_mode, mipi->sctl_base + CTRL);

	if (!broadcast && !status && target_panel_auto_detect_enabled())
		status = mdss_dsi_read_panel_signature(mipi);

end:
#endif
	return status;
}

int mdss_dsi_video_mode_config(struct msm_panel_info *pinfo,
	uint16_t disp_width,
	uint16_t disp_height,
	uint16_t img_width,
	uint16_t img_height,
	uint16_t hsync_porch0_fp,
	uint16_t hsync_porch0_bp,
	uint16_t vsync_porch0_fp,
	uint16_t vsync_porch0_bp,
	uint16_t hsync_width,
	uint16_t vsync_width,
	uint16_t dst_format,
	uint16_t traffic_mode,
	uint8_t lane_en,
	uint8_t pulse_mode_hsa_he,
	uint32_t low_pwr_stop_mode,
	uint8_t eof_bllp_pwr,
	uint8_t interleav,
	uint32_t ctl_base)
{
	int status = 0;

#if (DISPLAY_TYPE_MDSS == 1)
	int last_line_interleave_en = 0;
	struct dsc_desc *dsc = NULL;

	if (pinfo->compression_mode == COMPRESSION_DSC)
		dsc = &pinfo->dsc;

	/*Check if EOF_BLLP_PWR_MODE bit is set*/
	if(eof_bllp_pwr & 0x8)
		last_line_interleave_en = 1;

	writel(0x00000000, ctl_base + CLK_CTRL);
	writel(0x00000002, ctl_base + CLK_CTRL);
	writel(0x00000006, ctl_base + CLK_CTRL);
	writel(0x0000000e, ctl_base + CLK_CTRL);
	writel(0x0000001e, ctl_base + CLK_CTRL);
	writel(0x0000023f, ctl_base + CLK_CTRL);

	writel(0x03f03fe0, ctl_base + ERR_INT_MASK0);

	writel(0x02020202, ctl_base + INT_CTRL);

	/* For 8916/8939, enable DSI timing double buffering */
	if (readl(ctl_base) >= DSI_HW_REV_103 &&
				mdp_get_revision() != MDP_REV_305)
		writel(0x1, ctl_base + TIMING_DB_MODE);

	writel(((disp_width + hsync_porch0_bp) << 16) | hsync_porch0_bp,
			ctl_base + VIDEO_MODE_ACTIVE_H);

	writel(((disp_height + vsync_porch0_bp) << 16) | (vsync_porch0_bp),
			ctl_base + VIDEO_MODE_ACTIVE_V);

	if (mdp_get_revision() >= MDP_REV_41 ||
				mdp_get_revision() == MDP_REV_304 ||
				mdp_get_revision() == MDP_REV_305) {
		writel(((disp_height + vsync_porch0_fp
			+ vsync_porch0_bp - 1) << 16)
			| (disp_width + hsync_porch0_fp
			+ hsync_porch0_bp - 1),
			ctl_base + VIDEO_MODE_TOTAL);
	} else {
		writel(((disp_height + vsync_porch0_fp
			+ vsync_porch0_bp) << 16)
			| (disp_width + hsync_porch0_fp
			+ hsync_porch0_bp),
			ctl_base + VIDEO_MODE_TOTAL);
	}

	writel((hsync_width << 16) | 0, ctl_base + VIDEO_MODE_HSYNC);

	writel(0 << 16 | 0, ctl_base + VIDEO_MODE_VSYNC);

	writel(vsync_width << 16 | 0, ctl_base + VIDEO_MODE_VSYNC_VPOS);

	/* For 8916/8939, flush the DSI timing registers */
	if (readl(ctl_base) >= DSI_HW_REV_103 &&
				mdp_get_revision() != MDP_REV_305)
		writel(0x1, ctl_base + TIMING_FLUSH);

	writel(0x00000100, ctl_base + MISR_VIDEO_CTRL);

	if (mdp_get_revision() >= MDP_REV_41 || mdp_get_revision() == MDP_REV_305) {
		writel(last_line_interleave_en << 31 | pulse_mode_hsa_he << 28 |
				low_pwr_stop_mode << 16 | eof_bllp_pwr << 12 |
				traffic_mode << 8 | dst_format << 4 | 0x0,
				ctl_base + VIDEO_MODE_CTRL);
	} else {
		writel(1 << 28 | 1 << 24 | 1 << 20 | low_pwr_stop_mode << 16 |
				eof_bllp_pwr << 12 | traffic_mode << 8
				| dst_format << 4 | 0x0, ctl_base + VIDEO_MODE_CTRL);
	}

	writel(0x3fd08, ctl_base + HS_TIMER_CTRL);
	writel(0x00010100, ctl_base + MISR_VIDEO_CTRL);

	writel(0x00010100, ctl_base + INT_CTRL);
	writel(0x02010202, ctl_base + INT_CTRL);
	writel(0x02030303, ctl_base + INT_CTRL);

	writel(interleav << 30 | 0 << 24 | 0 << 20 | lane_en << 4
			| 0x103, ctl_base + CTRL);
	if (dsc) {
		if (dsc->dsi_dsc_config)
			dsc->dsi_dsc_config(pinfo->mipi.ctl_base, DSI_VIDEO_MODE, dsc);
	}
#endif

	return status;
}

int mdss_dsi_config(struct msm_fb_panel_data *panel)
{
	int ret = NO_ERROR;
	struct msm_panel_info *pinfo;
	struct mipi_panel_info *mipi;
	struct dsc_desc *dsc = NULL;
	struct mipi_dsi_cmd cmd;

#if (DISPLAY_TYPE_MDSS == 1)
	if (!panel)
		return ERR_INVALID_ARGS;

	pinfo = &(panel->panel_info);
	mipi = &(pinfo->mipi);


	if (pinfo->compression_mode == COMPRESSION_DSC) {
		dsc = &pinfo->dsc;
		if (dsc) {
			if (dsc->dsc2buf)
				dsc->dsc2buf(pinfo);
		}
	}

	dprintf(SPEW, "ctl_base=0x%08x, phy_base=0x%08x\n", mipi->ctl_base,
		mipi->phy_base);

	mdss_dsi_phy_init(mipi);

	ret = mdss_dsi_host_init(mipi, mipi->dual_dsi,
						mipi->broadcast);
	if (ret) {
		dprintf(CRITICAL, "dsi host init error\n");
		goto error;
	}

	mdss_dsi_phy_contention_detection(mipi, mipi->phy_base);
	if (mipi->dual_dsi)
		mdss_dsi_phy_contention_detection(mipi, mipi->sphy_base);

	if (panel->pre_init_func) {
		ret = panel->pre_init_func();
		if (ret) {
			dprintf(CRITICAL, "pre_init_func error\n");
			goto error;
		}
	}

	if (mipi->force_clk_lane_hs)
		mdss_dsi_force_clk_lane_hs(mipi, mipi->dual_dsi);

	if (!mipi->cmds_post_tg) {
		ret = mdss_dsi_panel_initialize(mipi, mipi->broadcast);
		if (ret) {
			dprintf(CRITICAL, "dsi panel init error\n");
			goto error;
		}
	}

	if (dsc) {
		cmd.size = DCS_HDR_LEN + DSC_PPS_LEN;
		cmd.payload = dsc->pps_buf;
		cmd.wait = 0x10;
		mdss_dsi_cmds_tx(mipi, &cmd, 1, mipi->broadcast);
	}

	if (pinfo->rotate && panel->rotate)
		pinfo->rotate();
#endif

error:
	return ret;
}

int mdss_dsi_post_on(struct msm_fb_panel_data *panel)
{
	int ret = 0;
	struct msm_panel_info *pinfo = &(panel->panel_info);

	if (pinfo->mipi.cmds_post_tg) {
		ret = mdss_dsi_panel_initialize(&pinfo->mipi, pinfo->mipi.broadcast);
		if (ret) {
			dprintf(CRITICAL, "dsi panel init error\n");
		}
	}
	return ret;
}

int mdss_dsi_cmd_mode_config(struct msm_panel_info *pinfo,
	uint16_t disp_width,
	uint16_t disp_height,
	uint16_t img_width,
	uint16_t img_height,
	uint16_t dst_format,
	uint8_t ystride,
	uint8_t lane_en,
	uint8_t interleav,
	uint32_t ctl_base)
{
	uint16_t dst_fmt = 0;
	struct dsc_desc *dsc = NULL;
	unsigned int data;

	switch (dst_format) {
	case DSI_VIDEO_DST_FORMAT_RGB565:
		dst_fmt = DSI_CMD_DST_FORMAT_RGB565;
		break;
	case DSI_VIDEO_DST_FORMAT_RGB666:
	case DSI_VIDEO_DST_FORMAT_RGB666_LOOSE:
		dst_fmt = DSI_CMD_DST_FORMAT_RGB666;
		break;
	case DSI_VIDEO_DST_FORMAT_RGB888:
		dst_fmt = DSI_CMD_DST_FORMAT_RGB888;
		break;
	default:
		dprintf(CRITICAL, "unsupported dst format\n");
		return ERROR;
	}

#if (DISPLAY_TYPE_MDSS == 1)
	writel(0x00000000, ctl_base + CLK_CTRL);
	writel(0x00000000, ctl_base + CLK_CTRL);
	writel(0x00000000, ctl_base + CLK_CTRL);
	writel(0x00000000, ctl_base + CLK_CTRL);
	writel(0x00000002, ctl_base + CLK_CTRL);
	writel(0x00000006, ctl_base + CLK_CTRL);
	writel(0x0000000e, ctl_base + CLK_CTRL);
	writel(0x0000001e, ctl_base + CLK_CTRL);
	writel(0x0000023f, ctl_base + CLK_CTRL);

	writel(0, ctl_base + CTRL);

	writel(0x03f03fe0, ctl_base + ERR_INT_MASK0);

	writel(0x02020202, ctl_base + INT_CTRL);

	writel(dst_fmt, ctl_base + COMMAND_MODE_MDP_CTRL);

	if (pinfo->compression_mode == COMPRESSION_DSC)
		dsc = &pinfo->dsc;

	if (dsc) {
		data = dsc->bytes_per_pkt;
		if (pinfo->mipi.insert_dcs_cmd)
			data++;
		data <<= 16;
		data |= 0x039;
		writel(data, ctl_base + COMMAND_MODE_MDP_STREAM0_CTRL);
		writel(data, ctl_base + COMMAND_MODE_MDP_STREAM1_CTRL);
		data = dsc->pic_height << 16;
		data |= dsc->pclk_per_line;
		writel(data, ctl_base + COMMAND_MODE_MDP_STREAM0_TOTAL);
		writel(data, ctl_base + COMMAND_MODE_MDP_STREAM1_TOTAL);

		if (dsc->dsi_dsc_config)
			dsc->dsi_dsc_config(pinfo->mipi.ctl_base, DSI_CMD_MODE, dsc);
	} else {

		writel((img_width * ystride + 1) << 16 | 0x0039,
		       ctl_base + COMMAND_MODE_MDP_STREAM0_CTRL);
		writel((img_width * ystride + 1) << 16 | 0x0039,
		       ctl_base + COMMAND_MODE_MDP_STREAM1_CTRL);
		writel(img_height << 16 | img_width,
		       ctl_base + COMMAND_MODE_MDP_STREAM0_TOTAL);
		writel(img_height << 16 | img_width,
		       ctl_base + COMMAND_MODE_MDP_STREAM1_TOTAL);
	}

	writel(0x13c2c, ctl_base + COMMAND_MODE_MDP_DCS_CMD_CTRL);
	writel(interleav << 30 | 0 << 24 | 0 << 20 | lane_en << 4 | 0x105,
	       ctl_base + CTRL);
	writel(0x14000000, ctl_base + COMMAND_MODE_DMA_CTRL);
	writel(0x10000000, ctl_base + MISR_CMD_CTRL);
#endif
	return 0;
}

int mipi_dsi_on(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	unsigned long ReadValue;
	unsigned long count = 0;

	ReadValue = readl(pinfo->mipi.ctl_base + INT_CTRL) & 0x00010000;

	mdelay(10);

	while (ReadValue != 0x00010000) {
		ReadValue = readl(pinfo->mipi.ctl_base + INT_CTRL) & 0x00010000;
		count++;
		if (count > 0xffff) {
			dprintf(CRITICAL, "Video lane test failed\n");
			return ERROR;
		}
	}

	dprintf(INFO, "Video lane tested successfully\n");
	return ret;
}

int mipi_dsi_off(struct msm_panel_info *pinfo)
{
	if(!target_cont_splash_screen())
	{
		mdss_dsi_panel_shutdown(pinfo);
		writel(0, pinfo->mipi.ctl_base + CLK_CTRL);
		writel(0, pinfo->mipi.ctl_base + CTRL);
	}

	writel(0x1115501, pinfo->mipi.ctl_base + INT_CTRL);
	if (pinfo->mipi.dual_dsi)
		writel(0x1115501, pinfo->mipi.sctl_base + INT_CTRL);

	return NO_ERROR;
}

#if (DISPLAY_TYPE_MDSS == 0)
static int dsi_cmd_dma_trigger_for_panel()
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
		/* Wait for VIDEO_MODE_DONE */
		ret = mdss_dsi_wait4_video_done();
		if (ret)
			goto mipi_cmds_error;

		memcpy((void *)off, (cm->payload), cm->size);
		arch_clean_invalidate_cache_range((addr_t)(off), size);
		writel(off, DSI_DMA_CMD_OFFSET);
		writel(cm->size, DSI_DMA_CMD_LENGTH);	// reg 0x48 for this build
		dsb();
		ret += dsi_cmd_dma_trigger_for_panel();
		dsb();
		if (cm->wait)
			mdelay(cm->wait);
		else
			udelay(80);
		cm++;
	}
mipi_cmds_error:
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

static int mipi_dsi_panel_initialize(struct mipi_dsi_panel_config *pinfo)
{
	uint8_t DMA_STREAM1 = 0;	// for mdp display processor path
	uint8_t EMBED_MODE1 = 1;	// from frame buffer
	uint8_t POWER_MODE2 = 1;	// from frame buffer
	uint8_t PACK_TYPE1;		// long packet
	uint8_t VC1 = 0;
	uint8_t DT1 = 0;	// non embedded mode
	uint8_t WC1 = 0;	// for non embedded mode only
	int status = 0;
	uint8_t DLNx_EN;

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
	case 4:
		DLNx_EN = 0x0F;	/* 4 lanes */
		break;
	}

	PACK_TYPE1 = pinfo->pack;

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

	if (pinfo->panel_on_cmds)
		status = mipi_dsi_cmds_tx(pinfo->panel_on_cmds,
					  pinfo->num_of_panel_on_cmds);

	return status;
}
#endif

void mipi_dsi_shutdown(void)
{
#if (DISPLAY_TYPE_MDSS == 0)
	if(!target_cont_splash_screen())
	{
		mdp_shutdown();
		writel(0x01010101, DSI_INT_CTRL);
		writel(0x13FF3BFF, DSI_ERR_INT_MASK0);

		writel(0, DSI_CLK_CTRL);
		writel(0, DSI_CTRL);
		writel(0, DSIPHY_PLL_CTRL(0));
	}
	else
	{
        /* To keep the splash screen displayed till kernel driver takes
        control, do not turn off the video mode engine and clocks.
        Only disabling the MIPI DSI IRQs */
        writel(0x01010101, DSI_INT_CTRL);
        writel(0x13FF3BFF, DSI_ERR_INT_MASK0);
	}
#endif
}

int mipi_cmd_trigger()
{
#if (DISPLAY_TYPE_MDSS == 0)
	writel(0x1, DSI_CMD_MODE_MDP_SW_TRIGGER);
#endif
	return NO_ERROR;
}

int mipi_config(struct msm_fb_panel_data *panel)
{
	int ret = NO_ERROR;
#if (DISPLAY_TYPE_MDSS == 0)
	struct msm_panel_info *pinfo;
	struct mipi_dsi_panel_config mipi_pinfo;

	if (!panel)
		return ERR_INVALID_ARGS;

	pinfo = &(panel->panel_info);
	mipi_pinfo.mode = pinfo->mipi.mode;
	mipi_pinfo.num_of_lanes = pinfo->mipi.num_of_lanes;
	mipi_pinfo.dsi_phy_config = pinfo->mipi.dsi_phy_db;
	mipi_pinfo.panel_on_cmds = pinfo->mipi.panel_on_cmds;
	mipi_pinfo.num_of_panel_on_cmds = pinfo->mipi.num_of_panel_on_cmds;
	mipi_pinfo.lane_swap = pinfo->mipi.lane_swap;
	mipi_pinfo.pack = 1;

	/* Enable MMSS_AHB_ARB_MATER_PORT_E for
	   arbiter master0 and master 1 request */
#if (!DISPLAY_MIPI_PANEL_RENESAS && !DISPLAY_TYPE_DSI6G && !DISPLAY_TYPE_8610)
	writel(0x00001800, MMSS_SFPB_GPREG);
#endif

	mipi_dsi_phy_init(&mipi_pinfo);

	ret += mipi_dsi_panel_initialize(&mipi_pinfo);

	if (pinfo->rotate && panel->rotate)
		pinfo->rotate();
#endif
	return ret;
}
