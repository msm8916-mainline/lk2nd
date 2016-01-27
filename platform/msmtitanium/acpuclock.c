/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include <platform.h>

#define MAX_LOOPS	500

/*
 * Disable power collapse using GDSCR:
 * Globally Distributed Switch Controller Register
 */
void clock_usb30_gdsc_enable(void)
{
	uint32_t reg = readl(GCC_USB30_GDSCR);

	reg &= ~(0x1);

	writel(reg, GCC_USB30_GDSCR);
}

/* enables usb30 clocks */
void clock_usb30_init(void)
{
	int ret;

	ret = clk_get_set_enable("usb30_iface_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_iface_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	clock_usb30_gdsc_enable();

	ret = clk_get_set_enable("usb30_master_clk", 133330000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_master_clk. ret = %d\n", ret);
		ASSERT(0);
	}
	ret = clk_get_set_enable("usb30_pipe_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_pipe_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_aux_clk", 19200000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_aux_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_mock_utmi_clk", 60000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_mock_utmi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_sleep_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_sleep_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb_phy_cfg_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_phy_cfg_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void clock_init_mmc(uint32_t interface)
{
	char clk_name[64];
	int ret;

	snprintf(clk_name, sizeof(clk_name), "sdc%u_iface_clk", interface);

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
	int ret = 1;
	char clk_name[64];

	snprintf(clk_name, sizeof(clk_name), "sdc%u_core_clk", interface);

	if(freq == MMC_CLK_400KHZ)
	{
		ret = clk_get_set_enable(clk_name, 400000, 1);
	}
	else if(freq == MMC_CLK_50MHZ)
	{
		ret = clk_get_set_enable(clk_name, 50000000, 1);
	}
	else if(freq == MMC_CLK_177MHZ)
	{
		ret = clk_get_set_enable(clk_name, 177770000, 1);
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
		dprintf(CRITICAL, "failed to set %s ret = %d\n", clk_name, ret);
		ASSERT(0);
	}
}

void clock_bumpup_pipe3_clk()
{
	int ret =0;
	ret = clk_get_set_enable("usb30_pipe_clk", 0, true);

	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_pipe_clk. ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Configure UART clock based on the UART block id*/
void clock_config_uart_dm(uint8_t id)
{
	int ret;
	char iclk[64];
	char cclk[64];

	snprintf(iclk, sizeof(iclk), "uart%u_iface_clk", id);
	snprintf(cclk, sizeof(cclk), "uart%u_core_clk", id);

	ret = clk_get_set_enable(iclk, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", iclk, ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable(cclk, 7372800, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set %s ret = %d\n", cclk, ret);
		ASSERT(0);
	}
}

/* Control the MDSS GDSC */
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
			dprintf(SPEW, "MDP GDSC already enabled\n");
		}
	} else {
		reg |= BIT(0);
		writel(reg, MDP_GDSCR);
		while(readl(MDP_GDSCR) & (GDSC_POWER_ON_BIT));
	}
}

/* Enable all the MDP branch clocks */
void mdp_clock_enable(void)
{
	int ret;

	ret = clk_get_set_enable("mdp_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Set MDP clock to 320MHz */
	ret = clk_get_set_enable("mdss_mdp_clk_src", 320000000, 1);

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
}

/* Disable all the MDP branch clocks */
void mdp_clock_disable(void)
{
	clk_disable(clk_get("mdss_vsync_clk"));
	clk_disable(clk_get("mdss_mdp_clk"));
	clk_disable(clk_get("mdss_mdp_clk_src"));
	clk_disable(clk_get("mdp_ahb_clk"));
}

/* Disable all the bus clocks needed by MDSS */
void mdss_bus_clocks_disable(void)
{
	/* Disable MDSS AXI clock */
	clk_disable(clk_get("mdss_axi_clk"));
}

/* Enable all the bus clocks needed by MDSS */
void mdss_bus_clocks_enable(void)
{
	int ret;

	/* Configure AXI clock */
	ret = clk_get_set_enable("mdss_axi_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdss_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Function to asynchronously reset CE.
 * Function assumes that all the CE clocks are off.
 */
static void ce_async_reset(uint8_t instance)
{
	/* Start the block reset for CE */
	writel(1, GCC_CRYPTO_BCR);

	udelay(2);

	/* Take CE block out of reset */
	writel(0, GCC_CRYPTO_BCR);

	udelay(2);
}

void clock_ce_enable(uint8_t instance)
{
	int ret;
	char clk_name[64];

	snprintf(clk_name, sizeof(clk_name), "ce%u_src_clk", instance);
	ret = clk_get_set_enable(clk_name, 160000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_src_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_core_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_core_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_ahb_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_ahb_clk ret = %d\n", instance, ret);
		ASSERT(0);
	}

	snprintf(clk_name, sizeof(clk_name), "ce%u_axi_clk", instance);
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce%u_axi_clk ret = %d\n", instance, ret);
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


void clock_reset_usb_phy()
{
	int ret;

	struct clk *phy_reset_clk = NULL;
	struct clk *pipe_reset_clk = NULL;
	struct clk *master_clk = NULL;

	master_clk = clk_get("usb30_master_clk");
	ASSERT(master_clk);

	/* Look if phy com clock is present */
	phy_reset_clk = clk_get("usb30_phy_reset");
	ASSERT(phy_reset_clk);

	pipe_reset_clk = clk_get("usb30_pipe_clk");
	ASSERT(pipe_reset_clk);

	/* ASSERT */
	ret = clk_reset(master_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb30_master_reset clk\n");
		return;
	}
	ret = clk_reset(phy_reset_clk, CLK_RESET_ASSERT);

	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb30_phy_reset clk\n");
		goto deassert_master_clk;
	}

	ret = clk_reset(pipe_reset_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb30_pipe_clk\n");
		goto deassert_phy_clk;
	}

	udelay(100);

	/* DEASSERT */
	ret = clk_reset(pipe_reset_clk, CLK_RESET_DEASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to deassert usb_pipe_clk\n");
		return;
	}

deassert_phy_clk:

	ret = clk_reset(phy_reset_clk, CLK_RESET_DEASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to deassert usb30_phy_com_reset clk\n");
		return;
	}

deassert_master_clk:

	ret = clk_reset(master_clk, CLK_RESET_DEASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to deassert usb30_master clk\n");
		return;
	}
}

static void rcg_update_config(uint32_t reg)
{
	int i;

	for (i = 0; i < MAX_LOOPS; i++) {
		if (!(readl(reg) & BIT(0)))
			return;
		udelay(1);
	}

	dprintf(CRITICAL, "failed to update rcg config for reg = 0x%x\n", reg);
	ASSERT(0);
}

static void branch_clk_halt_check(uint32_t reg)
{
	int i;

	for (i = 0; i < MAX_LOOPS; i++) {
		if (!(readl(reg) & BIT(31)))
			return;
		udelay(1);
	}

	dprintf(CRITICAL, "failed to enable branch for reg = 0x%x\n", reg);
	ASSERT(0);
}

void mmss_dsi_clock_enable(uint32_t cfg_rcgr, uint32_t flags,
		uint8_t pclk0_m, uint8_t pclk0_n, uint8_t pclk0_d)
{
	int ret;

	if (flags & MMSS_DSI_CLKS_FLAG_DSI0) {
		/* Enable DSI0 branch clocks */

		writel(cfg_rcgr, DSI_BYTE0_CFG_RCGR);
		writel(0x1, DSI_BYTE0_CMD_RCGR);
		rcg_update_config(DSI_BYTE0_CMD_RCGR);
		writel(0x1, DSI_BYTE0_CBCR);
		branch_clk_halt_check(DSI_BYTE0_CBCR);

		writel(cfg_rcgr, DSI_PIXEL0_CFG_RCGR);
		writel(pclk0_m, DSI_PIXEL0_M);
		writel(pclk0_n, DSI_PIXEL0_N);
		writel(pclk0_d, DSI_PIXEL0_D);
		writel(0x1, DSI_PIXEL0_CMD_RCGR);
		rcg_update_config(DSI_PIXEL0_CMD_RCGR);
		writel(0x1, DSI_PIXEL0_CBCR);
		branch_clk_halt_check(DSI_PIXEL0_CBCR);

		ret = clk_get_set_enable("mdss_esc0_clk", 0, 1);
		if(ret)
		{
			dprintf(CRITICAL, "failed to set esc0_clk ret = %d\n", ret);
			ASSERT(0);
		}
	}

	if (flags & MMSS_DSI_CLKS_FLAG_DSI1) {
		/* Enable DSI1 branch clocks */
		writel(cfg_rcgr, DSI_BYTE1_CFG_RCGR);
		writel(0x1, DSI_BYTE1_CMD_RCGR);
		rcg_update_config(DSI_BYTE1_CMD_RCGR);
		writel(0x1, DSI_BYTE1_CBCR);
		branch_clk_halt_check(DSI_BYTE1_CBCR);

		writel(cfg_rcgr, DSI_PIXEL1_CFG_RCGR);
		writel(pclk0_m, DSI_PIXEL1_M);
		writel(pclk0_n, DSI_PIXEL1_N);
		writel(pclk0_d, DSI_PIXEL1_D);
		writel(0x1, DSI_PIXEL1_CMD_RCGR);
		rcg_update_config(DSI_PIXEL1_CMD_RCGR);
		writel(0x1, DSI_PIXEL1_CBCR);
		branch_clk_halt_check(DSI_PIXEL1_CBCR);

		ret = clk_get_set_enable("mdss_esc1_clk", 0, 1);
		if(ret)
		{
			dprintf(CRITICAL, "failed to set esc1_clk ret = %d\n", ret);
			ASSERT(0);
		}
	}
}

void mmss_dsi_clock_disable(uint32_t flags)
{
	if (flags & MMSS_DSI_CLKS_FLAG_DSI0) {
		clk_disable(clk_get("mdss_esc0_clk"));
		writel(0x0, DSI_BYTE0_CBCR);
		writel(0x0, DSI_PIXEL0_CBCR);
	}

	if (flags & MMSS_DSI_CLKS_FLAG_DSI1) {
		clk_disable(clk_get("mdss_esc1_clk"));
		writel(0x0, DSI_BYTE1_CBCR);
		writel(0x0, DSI_PIXEL1_CBCR);
	}
}
