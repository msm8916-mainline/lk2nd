/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#include "mipi_renesas.h"

int mipi_renesas_cmd_fwvga_config(void *pdata)
{
	int ret = NO_ERROR;

	struct msm_panel_info *pinfo = (struct msm_panel_info *) pdata;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	ret = mipi_dsi_cmd_mode_config((pinfo->xres),
			(pinfo->yres),
			(pinfo->xres),
			(pinfo->yres),
			pinfo->mipi.dst_format,
			pinfo->mipi.traffic_mode);
	return ret;
}

int mipi_renesas_cmd_fwvga_on()
{
	int ret = NO_ERROR;
	return ret;
}

int mipi_renesas_cmd_fwvga_off()
{
	int ret = NO_ERROR;
	return ret;
}

static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* DSI_BIT_CLK at 500MHz, 2 lane, RGB888 */
	{0x03, 0x01, 0x01, 0x00},	/* regulator */
	/* timing   */
	{0xb9, 0x8e, 0x1f, 0x00, 0x98, 0x9c, 0x22,
	0x90, 0x18, 0x03, 0x04},
	{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
	{0xbb, 0x02, 0x06, 0x00},	/* strength */
	/* pll control */
	{0x00, 0xec, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,	/*  --> Two lane configuration */
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};

void mipi_renesas_cmd_fwvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = REN_MIPI_FB_WIDTH;
	pinfo->yres = REN_MIPI_FB_HEIGHT;
	pinfo->mipi.num_of_lanes = 2;

	pinfo->type = MIPI_CMD_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;

	pinfo->mipi.mode = DSI_CMD_MODE;
	pinfo->mipi.traffic_mode = 0;
	pinfo->mipi.dst_format = 0;
	pinfo->mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = 1;
	pinfo->mipi.panel_cmds = renesas_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(renesas_panel_video_mode_cmds);

	pinfo->on = mipi_renesas_cmd_fwvga_on;
	pinfo->off = mipi_renesas_cmd_fwvga_off;
	pinfo->config = mipi_renesas_cmd_fwvga_config;

	return;
}

void mipi_renesas_cmd_hvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = REN_MIPI_FB_WIDTH_HVGA;
	pinfo->yres = REN_MIPI_FB_HEIGHT_HVGA;
	pinfo->mipi.num_of_lanes = 1;

	pinfo->type = MIPI_CMD_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;
	pinfo->clk_rate = 499000000;

	pinfo->mipi.mode = DSI_CMD_MODE;
	pinfo->mipi.traffic_mode = 0;
	pinfo->mipi.dst_format = 0;
	pinfo->mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = 1;
	pinfo->mipi.panel_cmds = renesas_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(renesas_panel_video_mode_cmds);

	pinfo->on = mipi_renesas_cmd_fwvga_on;
	pinfo->off = mipi_renesas_cmd_fwvga_off;
	pinfo->config = mipi_renesas_cmd_fwvga_config;

	return;
}
