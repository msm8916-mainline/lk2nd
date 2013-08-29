/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 */

#include <err.h>
#include <assert.h>
#include <debug.h>
#include <reg.h>
#include <platform/timer.h>
#include <platform/iomap.h>
#include <mmc.h>
#include <clock.h>
#include <platform/clock.h>

void hsusb_clock_init(void)
{
	int ret;
	struct clk *iclk, *cclk;

	ret = clk_get_set_enable("usb_iface_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb_core_clk", 75000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Wait for the clocks to be stable since we are disabling soon after. */
	mdelay(1);

	iclk = clk_get("usb_iface_clk");
	cclk = clk_get("usb_core_clk");

	clk_disable(iclk);
	clk_disable(cclk);

	/* Wait for the clock disable to complete. */
	mdelay(1);

	/* Start the block reset for usb */
	writel(1, USB_HS_BCR);

	/* Wait for reset to complete. */
	mdelay(1);

	/* Take usb block out of reset */
	writel(0, USB_HS_BCR);

	/* Wait for the block to be brought out of reset. */
	mdelay(1);

	ret = clk_enable(iclk);

	if(ret)
    {
		dprintf(CRITICAL, "failed to set usb_iface_clk after async ret = %d\n", ret);
		ASSERT(0);
    }

	ret = clk_enable(cclk);

	if(ret)
    {
		dprintf(CRITICAL, "failed to set usb_iface_clk after async ret = %d\n", ret);
		ASSERT(0);
    }

}

void clock_init_mmc(uint32_t interface)
{
	char clk_name[64];
	int ret;

	snprintf(clk_name, 64, "sdc%u_iface_clk", interface);

	/* enable interface clock */
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc1_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	int ret;
	char clk_name[64];

	snprintf(clk_name, 64, "sdc%u_core_clk", interface);

	if(freq == MMC_CLK_400KHZ)
	{
		ret = clk_get_set_enable(clk_name, 400000, 1);
	}
	else if(freq == MMC_CLK_50MHZ)
	{
		ret = clk_get_set_enable(clk_name, 50000000, 1);
	}
	else if(freq == MMC_CLK_200MHZ)
	{
		ret = clk_get_set_enable(clk_name, 200000000, 1);
	}
	else
	{
		dprintf(CRITICAL, "sdc frequency (%d) is not supported\n", freq);
		ASSERT(0);
	}

	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc1_core_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Configure UART clock based on the UART block id*/
void clock_config_uart_dm(uint8_t id)
{
	int ret;

	ret = clk_get_set_enable("uart2_iface_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set uart2_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("uart2_core_clk", 7372800, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set uart2_core_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Configure MDP clock */
void mdp_clock_enable(void)
{
	int ret;

	ret = clk_get_set_enable("axi_clk_src", 200000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set axi_clk_src ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mmss_mmssnoc_axi_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_mmssnoc_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mmss_s0_axi_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_s0_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdp_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdp_axi_clk" , 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdp_vsync_clk" , 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_vsync_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void mdp_clock_disable(void)
{
	clk_disable(clk_get("mdp_vsync_clk"));
	clk_disable(clk_get("mdp_axi_clk"));
	clk_disable(clk_get("mdp_ahb_clk"));
	clk_disable(clk_get("mmss_s0_axi_clk"));
	clk_disable(clk_get("mmss_mmssnoc_axi_clk"));
}

int dsi_vco_set_rate(uint32_t rate)
{
	uint32_t temp, val;
	unsigned long fb_divider;

	temp = rate / 10;
	val = VCO_PARENT_RATE / 10;
	fb_divider = (temp * VCO_PREF_DIV_RATIO) / val;
	fb_divider = fb_divider / 2 - 1;

	temp = readl(DSIPHY_PLL_CTRL(1));
	val = (temp & 0xFFFFFF00) | (fb_divider & 0xFF);
	writel(val, DSIPHY_PLL_CTRL(1));

	temp = readl(DSIPHY_PLL_CTRL(2));
	val = (temp & 0xFFFFFFF8) | ((fb_divider >> 8) & 0x07);
	writel(val, DSIPHY_PLL_CTRL(2));

	temp = readl(DSIPHY_PLL_CTRL(3));
	val = (temp & 0xFFFFFFC0) | (VCO_PREF_DIV_RATIO - 1);
	writel(val, DSIPHY_PLL_CTRL(3));
	return 0;
}

uint32_t dsi_vco_round_rate(uint32_t rate)
{
	uint32_t vco_rate = rate;

	if (rate < VCO_MIN_RATE)
		vco_rate = VCO_MIN_RATE;
	else if (rate > VCO_MAX_RATE)
		vco_rate = VCO_MAX_RATE;

	return vco_rate;
}

int dsi_byte_clk_set(uint32_t *vcoclk_rate, uint32_t rate)
{
	int div, ret;
	uint32_t vco_rate, bitclk_rate;
	uint32_t temp, val;

	bitclk_rate = 8 * rate;

	for (div = 1; div < VCO_MAX_DIVIDER; div++)
	{
		vco_rate = dsi_vco_round_rate(bitclk_rate * div);

		if (vco_rate == bitclk_rate * div)
			break;

		if (vco_rate < bitclk_rate * div)
			return -1;
	}

	if (vco_rate != bitclk_rate * div)
		return -1;

	ret = dsi_vco_set_rate(vco_rate);
	if (ret)
	{
		dprintf(CRITICAL, "fail to set vco rate, ret = %d\n", ret);
		return ret;
	}
	*vcoclk_rate = vco_rate;

	/* set the bit clk divider */
	temp =  readl(DSIPHY_PLL_CTRL(8));
	val = (temp & 0xFFFFFFF0) | (div - 1);
	writel(val, DSIPHY_PLL_CTRL(8));

	/* set the byte clk divider */
	temp = readl(DSIPHY_PLL_CTRL(9));
	val = (temp & 0xFFFFFF00) | (vco_rate / rate - 1);
	writel(val, DSIPHY_PLL_CTRL(9));

	return 0;
}

int dsi_dsi_clk_set(uint32_t vco_rate, uint32_t rate)
{
	uint32_t temp, val;

	if (vco_rate % rate != 0)
	{
		dprintf(CRITICAL, "dsiclk_set_rate invalid rate\n");
		return -1;
	}

	temp = readl(DSIPHY_PLL_CTRL(10));
	val = (temp & 0xFFFFFF00) | (vco_rate / rate - 1);
	writel(val, DSIPHY_PLL_CTRL(10));

	return 0;
}

void dsi_setup_dividers(uint32_t val, uint32_t cfg_rcgr,
	uint32_t cmd_rcgr)
{
	uint32_t i = 0;
	uint32_t term_cnt = 5000;
	int32_t reg;

	writel(val, cfg_rcgr);
	writel(0x1, cmd_rcgr);
	reg = readl(cmd_rcgr);
	while (reg & 0x1)
	{
		i++;
		if (i > term_cnt)
		{
			dprintf(CRITICAL, "some dsi clock not enabled"
					"exceeded polling TIMEOUT!\n");
			break;
		}
		udelay(1);
		reg = readl(cmd_rcgr);
	}
}

void vco_enable(int enable)
{
	if (enable)
	{
		writel(0x1, DSIPHY_PLL_CTRL(0));
		while (!(readl(DSIPHY_PLL_READY) & 0x01))
			udelay(1);
	} else {
		writel(0x0, DSIPHY_PLL_CTRL(0));
	}
}

void dsi_clock_enable(uint32_t dsiclk_rate, uint32_t byteclk_rate)
{
	uint32_t vcoclk_rate;
	int ret;

	ret = clk_get_set_enable("dsi_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set dsi_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = dsi_byte_clk_set(&vcoclk_rate, byteclk_rate);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set byteclk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = dsi_dsi_clk_set(vcoclk_rate, dsiclk_rate);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set dsiclk ret = %d\n", ret);
		ASSERT(0);
	}

	vco_enable(1);

	dsi_setup_dividers(0x105, DSI_PCLK_CFG_RCGR, DSI_PCLK_CMD_RCGR);
	dsi_setup_dividers(0x101, DSI_BYTE_CFG_RCGR, DSI_BYTE_CMD_RCGR);
	dsi_setup_dividers(0x101, DSI_CFG_RCGR, DSI_CMD_RCGR);

	ret = clk_get_set_enable("dsi_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set dsi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("dsi_byte_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set dsi_byte_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("dsi_esc_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set dsi_esc_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("dsi_pclk_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set dsi_pclk_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdp_lcdc_clk" , 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_lcdc_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdp_dsi_clk" , 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_dsi_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void dsi_clock_disable(void)
{
	clk_disable(clk_get("mdp_dsi_clk"));
	clk_disable(clk_get("mdp_lcdc_clk"));
	clk_disable(clk_get("dsi_pclk_clk"));
	clk_disable(clk_get("dsi_esc_clk"));
	clk_disable(clk_get("dsi_byte_clk"));
	clk_disable(clk_get("dsi_clk"));
	vco_enable(0);
	clk_disable(clk_get("dsi_ahb_clk"));
}


void clock_ce_enable(uint8_t instance)
{
	int ret;
	char clk_name[64];

	snprintf(clk_name, 64, "ce%u_src_clk", instance);
	ret = clk_get_set_enable(clk_name, 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_src_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "ce%u_core_clk", instance);
    ret = clk_get_set_enable(clk_name, 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "ce%u_ahb_clk", instance);
    ret = clk_get_set_enable(clk_name, 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "ce%u_axi_clk", instance);
    ret = clk_get_set_enable(clk_name, 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Wait for 48 * #pipes cycles.
	 * This is necessary as immediately after an access control reset (boot up)
	 * or a debug re-enable, the Crypto core sequentially clears its internal
	 * pipe key storage memory. If pipe key initialization writes are attempted
	 * during this time, they may be overwritten by the internal clearing logic.
	 */
	udelay(1);
}

void clock_ce_disable(uint8_t instance)
{
	struct clk *ahb_clk;
	struct clk *cclk;
	struct clk *axi_clk;
	struct clk *src_clk;
	char clk_name[64];

	snprintf(clk_name, 64, "ce%u_src_clk", instance);
	src_clk = clk_get(clk_name);

	snprintf(clk_name, 64, "ce%u_ahb_clk", instance);
	ahb_clk = clk_get(clk_name);

	snprintf(clk_name, 64, "ce%u_axi_clk", instance);
	axi_clk = clk_get(clk_name);

	snprintf(clk_name, 64, "ce%u_core_clk", instance);
	cclk    = clk_get(clk_name);

	clk_disable(ahb_clk);
	clk_disable(axi_clk);
	clk_disable(cclk);
	clk_disable(src_clk);

	/* Some delay for the clocks to stabalize. */
	udelay(1);
}

/* Function to asynchronously reset CE.
 * Function assumes that all the CE clocks are off.
 */
static void ce_async_reset(uint8_t instance)
{
	if (instance == 1)
	{
		/* Start the block reset for CE */
		writel(1, GCC_CE1_BCR);

		udelay(2);

		/* Take CE block out of reset */
		writel(0, GCC_CE1_BCR);

		udelay(2);
	}
	else
	{
		dprintf(CRITICAL, "CE instance not supported instance = %d", instance);
		ASSERT(0);
	}
}

void clock_config_ce(uint8_t instance)
{
	/* Need to enable the clock before disabling since the clk_disable()
	 * has a check to default to nop when the clk_enable() is not called
	 * on that particular clock.
	 */
	clock_ce_enable(instance);

	clock_ce_disable(instance);

	ce_async_reset(instance);

	clock_ce_enable(instance);

}
