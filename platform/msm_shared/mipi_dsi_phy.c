/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
#include <debug.h>
#include <reg.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>

#if (DISPLAY_TYPE_MDSS == 0)
#define MIPI_DSI0_BASE MIPI_DSI_BASE
#define MIPI_DSI1_BASE MIPI_DSI_BASE
#endif

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

void mdss_dsi_phy_sw_reset(uint32_t ctl_base)
{
	/* start phy sw reset */
	writel(0x0001, ctl_base + 0x012c);
	udelay(1000);

	/* end phy sw reset */
	writel(0x0000, ctl_base + 0x012c);
	udelay(100);
}

void mdss_dsi_uniphy_pll_lock_detect_setting(uint32_t ctl_base)
{
	writel(0x04, ctl_base + 0x0264); /* LKDetect CFG2 */
	udelay(100);
	writel(0x05, ctl_base + 0x0264); /* LKDetect CFG2 */
	mdelay(1);
}

void mdss_dsi_uniphy_pll_sw_reset(uint32_t ctl_base)
{
	writel(0x01, ctl_base + 0x0268); /* PLL TEST CFG */
	udelay(1);
	writel(0x00, ctl_base + 0x0268); /* PLL TEST CFG */
	udelay(1);
}

int mdss_dsi_phy_regulator_init(struct mdss_dsi_phy_ctrl *pd)
{
	/* DSI0 and DSI1 have a common regulator */

	uint32_t off = 0x0580;	/* phy regulator ctrl settings */

	/* Regulator ctrl 0 */
	writel(0x00, MIPI_DSI0_BASE + off + (4 * 0));
	/* Regulator ctrl - CAL_PWD_CFG */
	writel(pd->regulator[6], MIPI_DSI0_BASE + off + (4 * 6));
	/* Regulator ctrl - TEST */
	writel(pd->regulator[5], MIPI_DSI0_BASE + off + (4 * 5));
	/* Regulator ctrl 3 */
	writel(pd->regulator[3], MIPI_DSI0_BASE + off + (4 * 3));
	/* Regulator ctrl 2 */
	writel(pd->regulator[2], MIPI_DSI0_BASE + off + (4 * 2));
	/* Regulator ctrl 1 */
	writel(pd->regulator[1], MIPI_DSI0_BASE + off + (4 * 1));
	/* Regulator ctrl 0 */
	writel(pd->regulator[0], MIPI_DSI0_BASE + off + (4 * 0));
	/* Regulator ctrl 4 */
	writel(pd->regulator[4], MIPI_DSI0_BASE + off + (4 * 4));
	dmb();
}

int mdss_dsi_v2_phy_init(struct mipi_dsi_panel_config *pinfo, uint32_t ctl_base)
{
	struct mdss_dsi_phy_ctrl *pd;
	uint32_t i, ln, off = 0, offset;

	pd = pinfo->mdss_dsi_phy_config;
	/* DSI PHY configuration */
	off = 0x480;
	writel(pd->strength[0], ctl_base + off + (4 * 0));
	writel(pd->strength[1], ctl_base + off + (4 * 2));

	off = 0x470;
	writel(0x10, ctl_base + off + (4 * 3));
	writel(0x5F, ctl_base + off + (4 * 0));

	off = 0x500;
	/* use LDO mode */
	writel(0x25, ctl_base + 0x4B0);
	for (i = 0; i < 5; i++)
		writel(pd->regulator[i], ctl_base + off + (4 * i));

	mipi_dsi_calibration();

	/* 4 lanes + clk lane configuration */
	/* lane config n * (0 - 4) & DataPath setup */
	for (ln = 0; ln < 5; ln++) {
		off = 0x0300 + (ln * 0x40);
		for (i = 0; i < 9; i++) {
			offset = i + (ln * 9);
			writel(pd->laneCfg[offset], ctl_base + off);
			dmb();
			off += 4;
		}
	}

	off = 0x440;
	for (i = 0; i < 12; i++)
		writel(pd->timing[i], ctl_base + off + (4 * i));

	if (1 == pinfo->num_of_lanes)
		writel(0x8, ctl_base + 0x200 + (4 * 11));


	if (pinfo->lane_swap)
		writel(pinfo->lane_swap, ctl_base + 0x0ac);

	/* T_CLK_POST, T_CLK_PRE for CLK lane P/N HS 200 mV timing
	length should > data lane HS timing length */
	writel(0x41b, ctl_base + 0x0c0);
	return 0;
}

int mdss_dsi_phy_init(struct mipi_dsi_panel_config *pinfo, uint32_t ctl_base)
{
	struct mdss_dsi_phy_ctrl *pd;
	uint32_t i, off = 0, ln, offset;

	if (mdp_get_revision() == MDP_REV_304)
		return mdss_dsi_v2_phy_init(pinfo, ctl_base);

	pd = (pinfo->mdss_dsi_phy_config);

	/* Strength ctrl 0 */
	writel(pd->strength[0], ctl_base + 0x0484);

	mdss_dsi_phy_regulator_init(pd);

	/* Strength ctrl 0 */
	writel(0x00, ctl_base + 0x04dc);

	off = 0x0440;	/* phy timing ctrl 0 - 11 */
	for (i = 0; i < 12; i++) {
		writel(pd->timing[i], ctl_base + off);
		dmb();
		off += 4;
	}

	/* MMSS_DSI_0_PHY_DSIPHY_CTRL_1 */
	writel(0x00, ctl_base + 0x0474);
	/* MMSS_DSI_0_PHY_DSIPHY_CTRL_0 */
	writel(0x5f, ctl_base + 0x0470);

	/* Strength ctrl 1 */
	writel(pd->strength[1], ctl_base + 0x0488);
	dmb();
	/* 4 lanes + clk lane configuration */
	/* lane config n * (0 - 4) & DataPath setup */
	for (ln = 0; ln < 5; ln++) {
		off = 0x0300 + (ln * 0x40);
		for (i = 0; i < 9; i++) {
			offset = i + (ln * 9);
			writel(pd->laneCfg[offset], ctl_base + off);
			dmb();
			off += 4;
		}
	}

	/* MMSS_DSI_0_PHY_DSIPHY_CTRL_0 */
	writel(0x5f, ctl_base + 0x0470);

	/* DSI_PHY_DSIPHY_GLBL_TEST_CTRL */
	if (ctl_base == MIPI_DSI0_BASE)
		writel(0x01, ctl_base + 0x04d4);
	else
		writel(0x00, ctl_base + 0x04d4);

	dmb();

	off = 0x04b4;	/* phy BIST ctrl 0 - 5 */
	for (i = 0; i < 6; i++) {
		writel(pd->bistCtrl[i], ctl_base + off);
		off += 4;
	}
	dmb();

	/* DSI_0_CLKOUT_TIMING_CTRL */
	writel(0x41b, ctl_base + 0x0c4);
	dmb();

}
