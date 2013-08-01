/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "edp.h"
#include <platform/timer.h>

/* EDP phy configuration settings */
void edp_phy_sw_reset(void)
{
	/* phy sw reset */
	edp_write(EDP_BASE + 0x74, 0x100); /* EDP_PHY_CTRL */
	dmb();
	udelay(1);
	edp_write(EDP_BASE + 0x74, 0x000); /* EDP_PHY_CTRL */
	dmb();
	udelay(1);

	/* phy PLL sw reset */
	edp_write(EDP_BASE + 0x74, 0x001); /* EDP_PHY_CTRL */
	dmb();
	udelay(1);
	edp_write(EDP_BASE + 0x74, 0x000); /* EDP_PHY_CTRL */
	dmb();
	udelay(1);
}

void edp_hw_powerup(int enable)
{
	int ret = 0;

	if (enable) {
		/* EDP_PHY_EDPPHY_GLB_PD_CTL */
		edp_write(EDP_BASE + 0x52c, 0x3f);
		/* EDP_PHY_EDPPHY_GLB_CFG */
		edp_write(EDP_BASE + 0x528, 0x1);
		/* EDP_PHY_PLL_UNIPHY_PLL_GLB_CFG */
		edp_write(EDP_BASE + 0x620, 0xf);
		/* EDP_AUX_CTRL */
		ret = edp_read(EDP_BASE + 0x300);
		edp_write(EDP_BASE + 0x300, ret | 0x1);
	} else {
		/* EDP_PHY_EDPPHY_GLB_PD_CTL */
		edp_write(EDP_BASE + 0x52c, 0xc0);
	}
}

void edp_pll_configure(void)
{
	edp_write(EDP_BASE + 0x664, 0x5); /* UNIPHY_PLL_LKDET_CFG2 */
	edp_write(EDP_BASE + 0x600, 0x1); /* UNIPHY_PLL_REFCLK_CFG */
	edp_write(EDP_BASE + 0x638, 0x36); /* UNIPHY_PLL_SDM_CFG0 */
	edp_write(EDP_BASE + 0x63c, 0x62); /* UNIPHY_PLL_SDM_CFG1 */
	edp_write(EDP_BASE + 0x640, 0x0); /* UNIPHY_PLL_SDM_CFG2 */
	edp_write(EDP_BASE + 0x644, 0x28); /* UNIPHY_PLL_SDM_CFG3 */
	edp_write(EDP_BASE + 0x648, 0x0); /* UNIPHY_PLL_SDM_CFG4 */
	edp_write(EDP_BASE + 0x64c, 0x80); /* UNIPHY_PLL_SSC_CFG0 */
	edp_write(EDP_BASE + 0x650, 0x0); /* UNIPHY_PLL_SSC_CFG1 */
	edp_write(EDP_BASE + 0x654, 0x0); /* UNIPHY_PLL_SSC_CFG2 */
	edp_write(EDP_BASE + 0x658, 0x0); /* UNIPHY_PLL_SSC_CFG3 */
	edp_write(EDP_BASE + 0x66c, 0xa); /* UNIPHY_PLL_CAL_CFG0 */
	edp_write(EDP_BASE + 0x674, 0x1); /* UNIPHY_PLL_CAL_CFG2 */
	edp_write(EDP_BASE + 0x684, 0x5a); /* UNIPHY_PLL_CAL_CFG6 */
	edp_write(EDP_BASE + 0x688, 0x0); /* UNIPHY_PLL_CAL_CFG7 */
	edp_write(EDP_BASE + 0x68c, 0x60); /* UNIPHY_PLL_CAL_CFG8 */
	edp_write(EDP_BASE + 0x690, 0x0); /* UNIPHY_PLL_CAL_CFG9 */
	edp_write(EDP_BASE + 0x694, 0x46); /* UNIPHY_PLL_CAL_CFG10 */
	edp_write(EDP_BASE + 0x698, 0x5); /* UNIPHY_PLL_CAL_CFG11 */
	edp_write(EDP_BASE + 0x65c, 0x10); /* UNIPHY_PLL_LKDET_CFG0 */
	edp_write(EDP_BASE + 0x660, 0x1a); /* UNIPHY_PLL_LKDET_CFG1 */
	edp_write(EDP_BASE + 0x604, 0x0); /* UNIPHY_PLL_POSTDIV1_CFG */
	edp_write(EDP_BASE + 0x624, 0x0); /* UNIPHY_PLL_POSTDIV2_CFG */
	edp_write(EDP_BASE + 0x628, 0x0); /* UNIPHY_PLL_POSTDIV3_CFG */

	edp_write(EDP_BASE + 0x620, 0x1); /* UNIPHY_PLL_GLB_CFG */
	edp_write(EDP_BASE + 0x620, 0x5); /* UNIPHY_PLL_GLB_CFG */
	edp_write(EDP_BASE + 0x620, 0x7); /* UNIPHY_PLL_GLB_CFG */
	edp_write(EDP_BASE + 0x620, 0xf); /* UNIPHY_PLL_GLB_CFG */
}

void edp_enable_mainlink(int enable)
{
	uint32_t data;

	data = edp_read(EDP_BASE + 0x004);
	data &= ~BIT(0);

	if (enable) {
		data |= 0x1;
		edp_write(EDP_BASE + 0x004, data);
		edp_write(EDP_BASE + 0x004, 0x1);
	} else {
		data |= 0x0;
		edp_write(EDP_BASE + 0x004, data);
	}
}

void edp_enable_lane_bist(int lane, int enable)
{
	unsigned char *addr_ln_bist_cfg, *addr_ln_pd_ctrl;

	/* EDP_PHY_EDPPHY_LNn_PD_CTL */
	addr_ln_pd_ctrl = (unsigned char *)(EDP_BASE + 0x404 + (0x40 * lane));
	/* EDP_PHY_EDPPHY_LNn_BIST_CFG0 */
	addr_ln_bist_cfg = (unsigned char *)(EDP_BASE + 0x408 + (0x40 * lane));

	if (enable) {
		edp_write(addr_ln_pd_ctrl, 0x0);
		edp_write(addr_ln_bist_cfg, 0x10);

	} else {
		edp_write(addr_ln_pd_ctrl, 0xf);
		edp_write(addr_ln_bist_cfg, 0x10);
	}
}

void edp_enable_pixel_clk(int enable)
{
	if (!enable) {
		edp_write(MDSS_EDPPIXEL_CBCR, 0); /* CBCR */
		return;
	}

	edp_write(EDP_BASE + 0x624, 0x1); /* PostDiv2 */

	/* Configuring MND for Pixel */
	edp_write(EDPPIXEL_M, 0x3f); /* M value */
	edp_write(EDPPIXEL_N, 0xb); /* N value */
	edp_write(EDPPIXEL_D, 0x0); /* D value */

	/* CFG RCGR */
	edp_write(EDPPIXEL_CFG_RCGR, (5 << 8) | (2 << 12));
	edp_write(EDPPIXEL_CMD_RCGR, 3); /* CMD RCGR */

	edp_write(MDSS_EDPPIXEL_CBCR, 1); /* CBCR */
}

void edp_enable_link_clk(int enable)
{
	if (!enable) {
		edp_write(MDSS_EDPLINK_CBCR, 0); /* CBCR */
		return;
	}

	edp_write(EDPLINK_CFG_RCGR, (4 << 8)); /* CFG RCGR */
	edp_write(EDPLINK_CMD_RCGR, 3); /* CMD RCGR */

	edp_write(MDSS_EDPLINK_CBCR, 1); /* CBCR */
}

void edp_config_clk(void)
{
	edp_enable_link_clk(1);
	edp_enable_pixel_clk(1);
}

void edp_unconfig_clk(void)
{
	edp_enable_link_clk(0);
	edp_enable_pixel_clk(0);
}

void edp_phy_misc_cfg(void)
{
	/* EDP_PHY_EDPPHY_GLB_VM_CFG0 */
	edp_write(EDP_BASE + 0x510, 0x3);
	/* EDP_PHY_EDPPHY_GLB_VM_CFG1 */
	edp_write(EDP_BASE + 0x514, 0x64);
	/* EDP_PHY_EDPPHY_GLB_MISC9 */
	edp_write(EDP_BASE + 0x518, 0x6c);
	/* EDP_MISC1_MISC0 */
	edp_write(EDP_BASE + 0x2c, 0x1);
}
