/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#include <stdint.h>
#include <debug.h>
#include <reg.h>
#include <mmc.h>
#include <clock.h>
#include <pm8x41.h>
#include <platform/clock.h>
#include <platform/iomap.h>

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
		dprintf(CRITICAL, "failed to set sdc%u_iface_clk ret = %d\n", interface, ret);
		ASSERT(0);
	}
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	int ret;
	uint32_t reg;
	char clk_name[64];

	snprintf(clk_name, 64, "sdc%u_core_clk", interface);

	if(freq == MMC_CLK_400KHZ)
	{
		ret = clk_get_set_enable(clk_name, 400000, 1);
	}
	else if(freq == MMC_CLK_25MHZ)
	{
		ret = clk_get_set_enable(clk_name, 25000000, 1);
	}
	else if(freq == MMC_CLK_50MHZ)
	{
		ret = clk_get_set_enable(clk_name, 50000000, 1);
	}
	else if(freq == MMC_CLK_96MHZ)
	{
		ret = clk_get_set_enable(clk_name, 100000000, 1);
	}
	else if(freq == MMC_CLK_192MHZ)
	{
		ret = clk_get_set_enable(clk_name, 192000000, 1);
	}
	else if(freq == MMC_CLK_200MHZ)
	{
		ret = clk_get_set_enable(clk_name, 200000000, 1);
	}
	else if(freq == MMC_CLK_400MHZ)
	{
		ret = clk_get_set_enable(clk_name, 384000000, 1);
	}
	else
	{
		dprintf(CRITICAL, "sdc frequency (%u) is not supported\n", freq);
		ASSERT(0);
	}


	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc%u_core_clk ret = %d\n", interface, ret);
		ASSERT(0);
	}

}

/* Configure clocks for SDCC Calibration circuit */
void clock_config_cdc(uint32_t interface)
{
	int ret = 0;
	char clk_name[64];

	snprintf(clk_name, sizeof(clk_name), "gcc_sdcc%u_cdccal_sleep_clk", interface);

	ret = clk_get_set_enable(clk_name, 0 , 1);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to enable clock: %s\n", clk_name);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "gcc_sdcc%u_cdccal_ff_clk", interface);
	ret = clk_get_set_enable(clk_name, 0 , 1);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to enable clock: %s\n", clk_name);
		ASSERT(0);
	}
}

/* Configure UART clock based on the UART block id*/
void clock_config_uart_dm(uint8_t id)
{
	int ret;
	char iclk[64];
	char cclk[64];

	snprintf(iclk, 64, "uart%u_iface_clk", id);
	snprintf(cclk, 64, "uart%u_core_clk", id);

	ret = clk_get_set_enable(iclk, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set uart%u_iface_clk ret = %d\n", id, ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable(cclk, 7372800, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set uart%u_core_clk ret = %d\n", id, ret);
		ASSERT(0);
	}
}

/* Function to asynchronously reset CE (Crypto Engine).
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
	else if (instance == 2)
	{
		/* Start the block reset for CE */
		writel(1, GCC_CE2_BCR);

		udelay(2);

		/* Take CE block out of reset */
		writel(0, GCC_CE2_BCR);

		udelay(2);
	}
	else
	{
		dprintf(CRITICAL, "CE instance not supported instance = %d", instance);
		ASSERT(0);
	}
}

void clock_ce_enable(uint8_t instance)
{
	int ret;
	char clk_name[64];

	snprintf(clk_name, sizeof(clk_name), "ce%u_src_clk", instance);
	ret = clk_get_set_enable(clk_name, 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_core_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_ahb_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_axi_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
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

	snprintf(clk_name, sizeof(clk_name), "ce%u_src_clk", instance);
	src_clk = clk_get(clk_name);

	snprintf(clk_name, sizeof(clk_name), "ce%u_ahb_clk", instance);
	ahb_clk = clk_get(clk_name);

	snprintf(clk_name, sizeof(clk_name), "ce%u_axi_clk", instance);
	axi_clk = clk_get(clk_name);

	snprintf(clk_name, sizeof(clk_name), "ce%u_core_clk", instance);
	cclk    = clk_get(clk_name);

	clk_disable(ahb_clk);
	clk_disable(axi_clk);
	clk_disable(cclk);
	clk_disable(src_clk);

	/* Some delay for the clocks to stabalize. */
	udelay(1);
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

void clock_usb30_gdsc_enable(void)
{
	uint32_t reg = readl(GCC_USB30_GDSCR);

	reg &= ~(0x1);

	writel(reg, GCC_USB30_GDSCR);
}

/* enables usb30 interface and master clocks */
void clock_usb30_init(void)
{
	int ret;

	/* interface clock */
	ret = clk_get_set_enable("usb30_iface_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_iface_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	clock_usb30_gdsc_enable();

	/* master clock */
	ret = clk_get_set_enable("usb30_master_clk", 125000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_master_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	/* Enable pmic diff clock */
	pm8x41_diff_clock_ctrl(1);
}

void mdp_gdsc_ctrl(uint8_t enable)
{
	uint32_t reg = 0;
	reg = readl(MDP_GDSCR);
	if (enable) {
		if (!(reg & GDSC_POWER_ON_BIT)) {
			reg &=  ~(BIT(0) | GDSC_EN_FEW_WAIT_MASK);
			reg |= GDSC_EN_FEW_WAIT_256_MASK;
			writel(reg, MDP_GDSCR);
			while(!(readl(MDP_GDSCR) & (GDSC_POWER_ON_BIT)));
		} else {
			dprintf(INFO, "MDP GDSC already enabled\n");
		}
	} else {
		reg |= BIT(0);
		writel(reg, MDP_GDSCR);
		while(readl(MDP_GDSCR) & (GDSC_POWER_ON_BIT));
	}
}

/* Configure MDP clock */
void mdp_clock_enable(void)
{
	int ret;

	/* Set MDP clock to 240MHz */
	ret = clk_get_set_enable("mdp_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_clk_src", 240000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_clk_src ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_vsync_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdss vsync clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_lut_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set lut_mdp clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void mdp_clock_disable()
{
	clk_disable(clk_get("mdss_vsync_clk"));
	clk_disable(clk_get("mdss_mdp_clk"));
	clk_disable(clk_get("mdss_mdp_lut_clk"));
	clk_disable(clk_get("mdss_mdp_clk_src"));
	clk_disable(clk_get("mdp_ahb_clk"));

}

void mmss_bus_clock_enable(void)
{
	int ret;
	/* Configure MMSSNOC AXI clock */
	ret = clk_get_set_enable("mmss_mmssnoc_axi_clk", 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmssnoc_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure S0 AXI clock */
	ret = clk_get_set_enable("mmss_s0_axi_clk", 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_s0_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure AXI clock */
	ret = clk_get_set_enable("mdss_axi_clk", 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdss_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void mmss_bus_clock_disable(void)
{
	/* Disable MDSS AXI clock */
	clk_disable(clk_get("mdss_axi_clk"));

	/* Disable MMSSNOC S0AXI clock */
	clk_disable(clk_get("mmss_s0_axi_clk"));

	/* Disable MMSSNOC AXI clock */
	clk_disable(clk_get("mmss_mmssnoc_axi_clk"));
}

void mmss_dsi_clock_enable(uint32_t dsi_pixel0_cfg_rcgr, uint32_t dual_dsi,
			uint8_t pclk0_m, uint8_t pclk0_n, uint8_t pclk0_d)
{
	int ret;

	/* Configure Byte clock -autopll- This will not change because
	byte clock does not need any divider*/
	writel(0x100, DSI_BYTE0_CFG_RCGR);
	writel(0x1, DSI_BYTE0_CMD_RCGR);
	writel(0x1, DSI_BYTE0_CBCR);

	/* Configure Pixel clock */
	writel(dsi_pixel0_cfg_rcgr, DSI_PIXEL0_CFG_RCGR);
	writel(0x1, DSI_PIXEL0_CMD_RCGR);
	writel(0x1, DSI_PIXEL0_CBCR);

	writel(pclk0_m, DSI_PIXEL0_M);
	writel(pclk0_n, DSI_PIXEL0_N);
	writel(pclk0_d, DSI_PIXEL0_D);

	/* Configure ESC clock */
	ret = clk_get_set_enable("mdss_esc0_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set esc0_clk ret = %d\n", ret);
		ASSERT(0);
	}

	if (dual_dsi) {
		/* Configure Byte 1 clock */
		writel(0x100, DSI_BYTE1_CFG_RCGR);
		writel(0x1, DSI_BYTE1_CMD_RCGR);
		writel(0x1, DSI_BYTE1_CBCR);

		/* Configure Pixel clock */
		writel(dsi_pixel0_cfg_rcgr, DSI_PIXEL1_CFG_RCGR);
		writel(0x1, DSI_PIXEL1_CMD_RCGR);
		writel(0x1, DSI_PIXEL1_CBCR);

		writel(pclk0_m, DSI_PIXEL1_M);
		writel(pclk0_n, DSI_PIXEL1_N);
		writel(pclk0_d, DSI_PIXEL1_D);

		/* Configure ESC clock */
		ret = clk_get_set_enable("mdss_esc1_clk", 0, 1);
		if(ret)
		{
			dprintf(CRITICAL, "failed to set esc1_clk ret = %d\n", ret);
			ASSERT(0);
		}
	}
}

void mmss_dsi_clock_disable(uint32_t dual_dsi)
{
	/* Disable ESC clock */
	clk_disable(clk_get("mdss_esc0_clk"));
	writel(0x0, DSI_BYTE0_CBCR);
	writel(0x0, DSI_PIXEL0_CBCR);

	if (dual_dsi) {
		/* Disable ESC clock */
		clk_disable(clk_get("mdss_esc1_clk"));
		writel(0x0, DSI_BYTE1_CBCR);
		writel(0x0, DSI_PIXEL1_CBCR);
	}
}

void hdmi_clk_enable(void)
{
	int ret;

	/* Configure hdmi ahb clock */
	ret = clk_get_set_enable("hdmi_ahb_clk", 0, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set hdmi_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure hdmi core clock */
	ret = clk_get_set_enable("hdmi_core_clk", 19200000, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set hdmi_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure hdmi pixel clock */
	ret = clk_get_set_enable("hdmi_extp_clk", 148500000, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set hdmi_extp_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void hdmi_clk_disable(void)
{
	clk_disable(clk_get("hdmi_extp_clk"));
	clk_disable(clk_get("hdmi_core_clk"));
	clk_disable(clk_get("hdmi_ahb_clk"));
}

void edp_clk_enable(void)
{
	int ret;

	/* Configure MMSSNOC AXI clock */
	ret = clk_get_set_enable("mmss_mmssnoc_axi_clk", 100000000, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set mmssnoc_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure MMSSNOC AXI clock */
	ret = clk_get_set_enable("mmss_s0_axi_clk", 100000000, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set mmss_s0_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure AXI clock */
	ret = clk_get_set_enable("mdss_axi_clk", 100000000, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set mdss_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("edp_pixel_clk", 138500000, 1);
	if (ret) {
		dprintf(CRITICAL, "failed to set edp_pixel_clk ret = %d\n",
						ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("edp_link_clk", 270000000, 1);
	if (ret) {
		dprintf(CRITICAL, "failed to set edp_link_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("edp_aux_clk", 19200000, 1);
	if (ret) {
		dprintf(CRITICAL, "failed to set edp_aux_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void edp_clk_disable(void)
{

	writel(0x0, MDSS_EDPPIXEL_CBCR);
	writel(0x0, MDSS_EDPLINK_CBCR);
	clk_disable(clk_get("edp_pixel_clk"));
	clk_disable(clk_get("edp_link_clk"));
	clk_disable(clk_get("edp_aux_clk"));
}

