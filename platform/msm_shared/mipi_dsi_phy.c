/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
#include <debug.h>
#include <reg.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>

static void mipi_dsi_calibration(void)
{
	uint32_t i = 0;
	uint32_t term_cnt = 5000;
	int32_t cal_busy = readl(MIPI_DSI_BASE + 0x550);

	/* DSI1_DSIPHY_REGULATOR_CAL_PWR_CFG */
	writel(0x01, MIPI_DSI_BASE + 0x0518);

	/* DSI1_DSIPHY_CAL_SW_CFG2 */
	writel(0x0, MIPI_DSI_BASE + 0x0534);
	/* DSI1_DSIPHY_CAL_HW_CFG1 */
	writel(0x5a, MIPI_DSI_BASE + 0x053c);
	/* DSI1_DSIPHY_CAL_HW_CFG3 */
	writel(0x10, MIPI_DSI_BASE + 0x0544);
	/* DSI1_DSIPHY_CAL_HW_CFG4 */
	writel(0x01, MIPI_DSI_BASE + 0x0548);
	/* DSI1_DSIPHY_CAL_HW_CFG0 */
	writel(0x01, MIPI_DSI_BASE + 0x0538);

	/* DSI1_DSIPHY_CAL_HW_TRIGGER */
	writel(0x01, MIPI_DSI_BASE + 0x0528);

	/* DSI1_DSIPHY_CAL_HW_TRIGGER */
	writel(0x00, MIPI_DSI_BASE + 0x0528);

	cal_busy = readl(MIPI_DSI_BASE + 0x550);
	while (cal_busy & 0x10) {
		i++;
		if (i > term_cnt) {
			dprintf(CRITICAL, "DSI1 PHY REGULATOR NOT READY,"
					"exceeded polling TIMEOUT!\n");
			break;
		}
		cal_busy = readl(MIPI_DSI_BASE + 0x550);
	}
}

int mipi_dsi_phy_init(struct mipi_dsi_panel_config *pinfo)
{
	struct mipi_dsi_phy_ctrl *pd;
	uint32_t i, off = 0;
	int mdp_rev;

	mdp_rev = mdp_get_revision();

	if (MDP_REV_303 == mdp_rev || MDP_REV_41 == mdp_rev) {
		writel(0x00000001, DSIPHY_SW_RESET);
		writel(0x00000000, DSIPHY_SW_RESET);

		pd = (pinfo->dsi_phy_config);

		off = 0x02cc;		/* regulator ctrl 0 */
		for (i = 0; i < 4; i++) {
			writel(pd->regulator[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		off = 0x0260;		/* phy timig ctrl 0 */
		for (i = 0; i < 11; i++) {
			writel(pd->timing[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		/* T_CLK_POST, T_CLK_PRE for CLK lane P/N HS 200 mV timing
		length should > data lane HS timing length */
		writel(0xa1e, DSI_CLKOUT_TIMING_CTRL);

		off = 0x0290;		/* ctrl 0 */
		for (i = 0; i < 4; i++) {
			writel(pd->ctrl[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		off = 0x02a0;		/* strength 0 */
		for (i = 0; i < 4; i++) {
			writel(pd->strength[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		if (1 == pinfo->num_of_lanes)
			pd->pll[10] |= 0x8;

		off = 0x0204;		/* pll ctrl 1, skip 0 */
		for (i = 1; i < 21; i++) {
			writel(pd->pll[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		/* pll ctrl 0 */
		writel(pd->pll[0], MIPI_DSI_BASE + 0x200);
		writel((pd->pll[0] | 0x01), MIPI_DSI_BASE + 0x200);
		/* lane swp ctrol */
		if (pinfo->lane_swap)
			writel(pinfo->lane_swap, MIPI_DSI_BASE + 0xac);
	} else {
		writel(0x0001, MIPI_DSI_BASE + 0x128);	/* start phy sw reset */
		writel(0x0000, MIPI_DSI_BASE + 0x128);	/* end phy w reset */
		writel(0x0003, MIPI_DSI_BASE + 0x500);	/* regulator_ctrl_0 */
		writel(0x0001, MIPI_DSI_BASE + 0x504);	/* regulator_ctrl_1 */
		writel(0x0001, MIPI_DSI_BASE + 0x508);	/* regulator_ctrl_2 */
		writel(0x0000, MIPI_DSI_BASE + 0x50c);	/* regulator_ctrl_3 */
		writel(0x0100, MIPI_DSI_BASE + 0x510);	/* regulator_ctrl_4 */

		pd = (pinfo->dsi_phy_config);

		off = 0x0480;		/* strength 0 - 2 */
		for (i = 0; i < 3; i++) {
			writel(pd->strength[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		off = 0x0470;		/* ctrl 0 - 3 */
		for (i = 0; i < 4; i++) {
			writel(pd->ctrl[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		off = 0x0500;		/* regulator ctrl 0 - 4 */
		for (i = 0; i < 5; i++) {
			writel(pd->regulator[i], MIPI_DSI_BASE + off);
			off += 4;
		}
		mipi_dsi_calibration();

		off = 0x0204;		/* pll ctrl 1 - 19, skip 0 */
		for (i = 1; i < 20; i++) {
			writel(pd->pll[i], MIPI_DSI_BASE + off);
			off += 4;
		}

		/* pll ctrl 0 */
		writel(pd->pll[0], MIPI_DSI_BASE + 0x200);
		writel((pd->pll[0] | 0x01), MIPI_DSI_BASE + 0x200);

		/* Check that PHY is ready */
		while (!(readl(DSIPHY_PLL_RDY) & 0x01))
			udelay(1);

		writel(0x202D, DSI_CLKOUT_TIMING_CTRL);

		off = 0x0440;		/* phy timing ctrl 0 - 11 */
		for (i = 0; i < 12; i++) {
			writel(pd->timing[i], MIPI_DSI_BASE + off);
			off += 4;
		}
	}
	return 0;
}
