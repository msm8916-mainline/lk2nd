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

/* EDP phy configuration settings */

void edp_phy_pll_reset(void)
{
        /* EDP_PHY_CTRL */
        edp_write(EDP_BASE + 0x74, 0x005); /* bit 0, 2 */
	dmb();
	udelay(1);
        edp_write(EDP_BASE + 0x74, 0x000); /* EDP_PHY_CTRL */
}

void edp_mainlink_reset(void)
{
        edp_write(EDP_BASE + 0x04, 0x02 ); /* EDP_MAINLINK_CTRL */
	dmb();
	udelay(1);
        edp_write(EDP_BASE + 0x04, 0 ); /* EDP_MAINLINK_CTRL */
}

void edp_aux_reset(void)
{
        /*reset AUX */
        edp_write(EDP_BASE + 0x300, BIT(1)); /* EDP_AUX_CTRL */
	dmb();
	udelay(1);
        edp_write(EDP_BASE + 0x300, 0); /* EDP_AUX_CTRL */
}

void edp_aux_enable(void)
{
        edp_write(EDP_BASE + 0x300, 0x01); /* EDP_AUX_CTRL */
	dmb();
	udelay(1);
}

void edp_phy_powerup(int enable)
{
        if (enable) {
                /* EDP_PHY_EDPPHY_GLB_PD_CTL */
                edp_write(EDP_BASE + 0x52c, 0x3f);
                /* EDP_PHY_EDPPHY_GLB_CFG */
                edp_write(EDP_BASE + 0x528, 0x1);
                /* EDP_PHY_PLL_UNIPHY_PLL_GLB_CFG */
                edp_write(EDP_BASE + 0x620, 0xf);
        } else {
                /* EDP_PHY_EDPPHY_GLB_PD_CTL */
                edp_write(EDP_BASE + 0x52c, 0xc0);
                edp_write(EDP_BASE + 0x620, 0x0);
        }
}

void edp_lane_power_ctrl(int max_lane, int up)
{
	int i, off;
	unsigned int data;

	if (up)
		data = 0;       /* power up */
	else
		data = 0x7;     /* power down */

	/* EDP_PHY_EDPPHY_LNn_PD_CTL */
	for (i = 0; i < max_lane; i++) {
		off = 0x40 * i;
		edp_write(EDP_BASE + 0x404 + off , data);
	}

	/* power down un used lane */
	data = 0x7;     /* power down */
	for (i = max_lane; i < EDP_MAX_LANE; i++) {
		off = 0x40 * i;
		edp_write(EDP_BASE + 0x404 + off , data);
	}
}

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
		ret = edp_read(EDP_BASE + 0x300);
		edp_write(EDP_BASE + 0x300, ret | 0x1);
	} else {
		/* EDP_PHY_EDPPHY_GLB_PD_CTL */
		edp_write(EDP_BASE + 0x52c, 0xc0);
	}
}

void edp_pll_configure(unsigned int rate)
{

       if (rate == 810000000) {
                edp_write(EDP_BASE + 0x60c, 0x18);
                edp_write(EDP_BASE + 0x664, 0x5);
                edp_write(EDP_BASE + 0x600, 0x0);
                edp_write(EDP_BASE + 0x638, 0x36);
                edp_write(EDP_BASE + 0x63c, 0x69);
                edp_write(EDP_BASE + 0x640, 0xff);
                edp_write(EDP_BASE + 0x644, 0x2f);
                edp_write(EDP_BASE + 0x648, 0x0);
                edp_write(EDP_BASE + 0x66c, 0x0a);
                edp_write(EDP_BASE + 0x674, 0x01);
                edp_write(EDP_BASE + 0x684, 0x5a);
                edp_write(EDP_BASE + 0x688, 0x0);
                edp_write(EDP_BASE + 0x68c, 0x60);
                edp_write(EDP_BASE + 0x690, 0x0);
                edp_write(EDP_BASE + 0x694, 0x2a);
                edp_write(EDP_BASE + 0x698, 0x3);
                edp_write(EDP_BASE + 0x65c, 0x10);
                edp_write(EDP_BASE + 0x660, 0x1a);
                edp_write(EDP_BASE + 0x604, 0x0);
                edp_write(EDP_BASE + 0x624, 0x0);
                edp_write(EDP_BASE + 0x628, 0x0);

                edp_write(EDP_BASE + 0x620, 0x1);
                edp_write(EDP_BASE + 0x620, 0x5);
                edp_write(EDP_BASE + 0x620, 0x7);
                edp_write(EDP_BASE + 0x620, 0xf);

        } else if (rate == 138530000) {
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
	} else {
		dprintf(INFO, "%s: rate=%d is NOT supported\n", __func__, rate);

	}
}


int mdss_edp_phy_pll_ready(void)
{
        int cnt;
        int status;

        cnt = 10;
        while(cnt--) {
                status = edp_read(EDP_BASE + 0x6c0);
                if (status & 0x01)
                        break;
                udelay(100);
        }

        if(cnt == 0) {
                dprintf("%s: PLL NOT ready\n", __func__);
                return 0;
        }
        else
                return 1;
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

	edp_write(EDPLINK_CFG_RCGR, (4 << 8) | 0x01); /* CFG RCGR */
	edp_write(EDPLINK_CMD_RCGR, 3); /* CMD RCGR */

	edp_write(MDSS_EDPLINK_CBCR, 1); /* CBCR */
}

void edp_enable_aux_clk(int enable)
{
	if (!enable) {
		edp_write(MDSS_EDPAUX_CBCR, 0); /* CBCR */
		return;
	}

	edp_write(EDPAUX_CFG_RCGR, 0x01); /* CFG RCGR */

	edp_write(MDSS_EDPAUX_CBCR, 1); /* CBCR */
}

void edp_config_clk(void)
{
	edp_enable_link_clk(1);
	edp_enable_pixel_clk(1);
	edp_enable_aux_clk(1);
}

void edp_unconfig_clk(void)
{
	edp_enable_link_clk(0);
	edp_enable_pixel_clk(0);
	edp_enable_aux_clk(0);
}

void edp_phy_vm_pe_init(void)
{
	/* EDP_PHY_EDPPHY_GLB_VM_CFG0 */
	edp_write(EDP_BASE + 0x510, 0x3);
	/* EDP_PHY_EDPPHY_GLB_VM_CFG1 */
	edp_write(EDP_BASE + 0x514, 0x64);
	/* EDP_PHY_EDPPHY_GLB_MISC9 */
	edp_write(EDP_BASE + 0x518, 0x6c);
}
