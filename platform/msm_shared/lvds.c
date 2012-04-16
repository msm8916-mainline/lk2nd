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

#include <stdint.h>
#include <reg.h>
#include <err.h>
#include <bits.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <dev/fbcon.h>
#include <dev/lcdc.h>

#include <msm_panel.h>
#define MDP_OUTP(addr, val)		writel(val, addr);

void lvds_init(struct msm_panel_info *pinfo)
{
	unsigned int lvds_intf, lvds_phy_cfg0;
	MDP_OUTP(MDP_BASE + 0xc2034, 0x33);
	udelay(1000);

	/*1. Configure LVDS PHY PLL through MDP_LVDSPHY_PLL_CTRL_* registers*/
	/* LVDS PHY PLL configuration */
	MDP_OUTP(MDP_BASE + 0xc3000, 0x08);
	MDP_OUTP(MDP_BASE + 0xc3004, 0x87);
	MDP_OUTP(MDP_BASE + 0xc3008, 0x30);
	MDP_OUTP(MDP_BASE + 0xc300c, 0x06);
	MDP_OUTP(MDP_BASE + 0xc3014, 0x20);
	MDP_OUTP(MDP_BASE + 0xc3018, 0x0F);
	MDP_OUTP(MDP_BASE + 0xc301c, 0x01);
	MDP_OUTP(MDP_BASE + 0xc3020, 0x41);
	MDP_OUTP(MDP_BASE + 0xc3024, 0x0d);
	MDP_OUTP(MDP_BASE + 0xc3028, 0x07);
	MDP_OUTP(MDP_BASE + 0xc302c, 0x00);
	MDP_OUTP(MDP_BASE + 0xc3030, 0x1c);
	MDP_OUTP(MDP_BASE + 0xc3034, 0x01);
	MDP_OUTP(MDP_BASE + 0xc3038, 0x00);
	MDP_OUTP(MDP_BASE + 0xc3040, 0xC0);
	MDP_OUTP(MDP_BASE + 0xc3044, 0x00);
	MDP_OUTP(MDP_BASE + 0xc3048, 0x30);
	MDP_OUTP(MDP_BASE + 0xc304c, 0x00);

	MDP_OUTP(MDP_BASE + 0xc3000, 0x11);
	MDP_OUTP(MDP_BASE + 0xc3064, 0x05);
	MDP_OUTP(MDP_BASE + 0xc3050, 0x20);

	/*2. Enable LVDS PHY PLL in MDP_LVDSPHY_PLL_CTRL_0*/
	MDP_OUTP(MDP_BASE + 0xc3000, 0x01);

	/*3. Poll the MDP_LVDSPHY_PLL_RDY register until it is 1*/
	/* Wait until LVDS PLL is locked and ready */
	while (!readl(MDP_BASE + 0xc3080))
		udelay(1);

	/*4. Enable dsi2_pixel domain clocks*/
	writel(0x00, REG_MM(0x0264));
	writel(0x00, REG_MM(0x0094));

	writel(0x02, REG_MM(0x00E4));

	writel((0x80 | readl(REG_MM(0x00E4))),
			REG_MM(0x00E4));
	udelay(1000);
	writel((~0x80 & readl(REG_MM(0x00E4))),
			REG_MM(0x00E4));

	writel(0x05, REG_MM(0x0094));
	writel(0x02, REG_MM(0x0264));
	/* Wait until LVDS pixel clock output is enabled */
	dsb();
	if (pinfo->bpp == 24) {
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D0_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc2014, 0x03040508);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D0_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2018, 0x00000102);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D1_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc201c, 0x0c0d1011);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D1_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2020, 0x00090a0b);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D2_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc2024, 0x151a191a);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D2_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2028, 0x00121314);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D3_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc202c, 0x1706071b);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D3_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2030, 0x000e0f16);

		if (pinfo->lvds.channel_mode ==
			LVDS_DUAL_CHANNEL_MODE) {
			lvds_intf = 0x0001ff80;
			lvds_phy_cfg0 = BIT(6) | BIT(7);
		if (pinfo->lvds.channel_swap)
			lvds_intf |= BIT(4);
		} else {
			lvds_intf = 0x00010f84;
			lvds_phy_cfg0 = BIT(6);
		}
	} else if (pinfo->bpp == 18) {
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D0_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc2014, 0x03040508);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D0_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2018, 0x00000102);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D1_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc201c, 0x0c0d1011);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D1_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2020, 0x00090a0b);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D2_3_TO_0 */
		MDP_OUTP(MDP_BASE +  0xc2024, 0x1518191a);
		/* MDP_LCDC_LVDS_MUX_CTL_FOR_D2_6_TO_4 */
		MDP_OUTP(MDP_BASE +  0xc2028, 0x00121314);

		if (pinfo->lvds.channel_mode ==
			LVDS_DUAL_CHANNEL_MODE) {
			lvds_intf = 0x00017788;
			lvds_phy_cfg0 = BIT(6) | BIT(7);
			if (pinfo->lvds.channel_swap)
				lvds_intf |= BIT(4);
		} else {
			lvds_intf = 0x0001078c;
			lvds_phy_cfg0 = BIT(6);
		}
	}

	/* MDP_LVDSPHY_CFG0 */
	MDP_OUTP(MDP_BASE +  0xc3100, lvds_phy_cfg0);
	/* MDP_LCDC_LVDS_INTF_CTL */
	MDP_OUTP(MDP_BASE +  0xc2000, lvds_intf);
	MDP_OUTP(MDP_BASE +  0xc3108, 0x30);
	lvds_phy_cfg0 |= BIT(4);

	/* Wait until LVDS PHY registers are configured */
	dsb();
	udelay(1);
	/* MDP_LVDSPHY_CFG0, enable serialization */
	MDP_OUTP(MDP_BASE +  0xc3100, lvds_phy_cfg0);
}

int lvds_on(struct msm_fb_panel_data *pdata)
{
	int ret = 0;
	if (pdata == NULL) {
		ret = ERR_INVALID_ARGS;
		goto out;
	}

	lvds_init(&(pdata->panel_info));
out:
	return ret;
}

