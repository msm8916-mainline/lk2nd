/* Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.
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
#include <platform/timer.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <pm8x41.h>
#include <rpm-smd.h>
#include <regulator.h>
#include <blsp_qup.h>
#include <err.h>

#define RPM_CE_CLK_TYPE    0x6563
#define CE1_CLK_ID         0x0
#define RPM_SMD_KEY_RATE   0x007A484B

uint32_t CE1_CLK[][8]=
{
	{
		RPM_CE_CLK_TYPE, CE1_CLK_ID,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		RPM_SMD_KEY_RATE, 4, 0,
	},
	{
		RPM_CE_CLK_TYPE, CE1_CLK_ID,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		RPM_SMD_KEY_RATE, 4, 176128, /* clk rate in KHZ */
	},
};

void clock_init_mmc(uint32_t interface)
{
	char clk_name[64];
	int ret;

	snprintf(clk_name, sizeof(clk_name), "sdc%u_iface_clk", interface);

	/* enable interface clock */
	ret = clk_get_set_enable(clk_name, 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc%u_iface_clk ret = %d\n", interface, ret);
		ASSERT(0);
	}
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	int ret = 0;
	char clk_name[64];

	snprintf(clk_name, sizeof(clk_name), "sdc%u_core_clk", interface);

	if(freq == MMC_CLK_400KHZ)
	{
		ret = clk_get_set_enable(clk_name, 400000, true);
	}
	else if(freq == MMC_CLK_50MHZ)
	{
		ret = clk_get_set_enable(clk_name, 50000000, true);
	}
	else if(freq == MMC_CLK_96MHZ)
	{
		ret = clk_get_set_enable(clk_name, 96000000, true);
	}
	else if(freq == MMC_CLK_192MHZ)
	{
		ret = clk_get_set_enable(clk_name, 192000000, true);
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

/* Configure UART clock based on the UART block id*/
void clock_config_uart_dm(uint8_t id)
{
	int ret;
	char iclk[64];
	char cclk[64];

	snprintf(iclk, sizeof(iclk), "uart%u_iface_clk", id);
	snprintf(cclk, sizeof(cclk), "uart%u_core_clk", id);

	ret = clk_get_set_enable(iclk, 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set uart%u_iface_clk ret = %d\n", id, ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable(cclk, 7372800, true);
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
	else
	{
		dprintf(CRITICAL, "Unsupported CE instance: %u\n", instance);
		ASSERT(0);
	}
}

void clock_ce_enable(uint8_t instance)
{
	if (instance == 1)
		rpm_send_data(&CE1_CLK[GENERIC_ENABLE][0], 24, RPM_REQUEST_TYPE);
	else
	{
		dprintf(CRITICAL, "Unsupported CE instance: %u\n", instance);
		ASSERT(0);
	}
}

void clock_ce_disable(uint8_t instance)
{
	if (instance == 1)
		rpm_send_data(&CE1_CLK[GENERIC_DISABLE][0], 24, RPM_REQUEST_TYPE);
	else
	{
		dprintf(CRITICAL, "Unsupported CE instance: %u\n", instance);
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

void clock_usb30_gdsc_enable(void)
{
	uint32_t reg = readl(GCC_USB30_GDSCR);

	reg &= ~(0x1);

	writel(reg, GCC_USB30_GDSCR);
}

/* enables usb20 clocks */
void clock_usb20_init(void)
{
	int ret;

	ret = clk_get_set_enable("usb20_noc_usb20_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb20_noc_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb20_master_clk", 120000000, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb20_master_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb20_mock_utmi_clk", 60000000, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb20_mock_utmi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb20_sleep_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb2_sleep_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* enables usb30 clocks */
void clock_usb30_init(void)
{
	int ret;

	ret = clk_get_set_enable("usb30_iface_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_iface_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	clock_usb30_gdsc_enable();

	ret = clk_get_set_enable("usb30_master_clk", 150000000, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_master_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("gcc_aggre2_usb3_axi_clk", 150000000, true);
	if (ret)
	{
		dprintf(CRITICAL, "failed to set aggre2_usb3_axi_clk, ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_phy_aux_clk", 1200000, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_phy_aux_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_mock_utmi_clk", 60000000, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_mock_utmi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb30_sleep_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_sleep_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb_phy_cfg_ahb2phy_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to enable usb_phy_cfg_ahb2phy_clk = %d\n", ret);
		ASSERT(0);
	}
}

void clock_bumpup_pipe3_clk()
{
	int ret = 0;

	ret = clk_get_set_enable("usb30_pipe_clk", 0, true);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb30_pipe_clk. ret = %d\n", ret);
		ASSERT(0);
	}

	return;
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

void mmss_gdsc_enable()
{
	uint32_t reg = 0;

	reg = readl(MMAGIC_BIMC_GDSCR);
	if (!(reg & GDSC_POWER_ON_BIT)) {
		reg &=  ~(BIT(0) | GDSC_EN_FEW_WAIT_MASK);
		reg |= GDSC_EN_FEW_WAIT_256_MASK;
		writel(reg, MMAGIC_BIMC_GDSCR);
		while(!(readl(MMAGIC_BIMC_GDSCR) & (GDSC_POWER_ON_BIT)));
	} else {
		dprintf(SPEW, "MMAGIC BIMC GDSC already enabled\n");
	}

	reg = readl(MMAGIC_MDSS_GDSCR);
	if (!(reg & GDSC_POWER_ON_BIT)) {
		reg &=  ~(BIT(0) | GDSC_EN_FEW_WAIT_MASK);
		reg |= GDSC_EN_FEW_WAIT_256_MASK;
		writel(reg, MMAGIC_MDSS_GDSCR);
		while(!(readl(MMAGIC_MDSS_GDSCR) & (GDSC_POWER_ON_BIT)));
	} else {
		dprintf(SPEW, "MMAGIC MDSS GDSC already enabled\n");
	}

	reg = readl(MDSS_GDSCR);
	if (!(reg & GDSC_POWER_ON_BIT)) {
		reg &=  ~(BIT(0) | GDSC_EN_FEW_WAIT_MASK);
		reg |= GDSC_EN_FEW_WAIT_256_MASK;
		writel(reg, MDSS_GDSCR);
		while(!(readl(MDSS_GDSCR) & (GDSC_POWER_ON_BIT)));
	} else {
		dprintf(SPEW, "MDSS GDSC already enabled\n");
	}
}

void mmss_gdsc_disable()
{
	uint32_t reg = 0;

	reg = readl(MDSS_GDSCR);
	reg |= BIT(0);
	writel(reg, MDSS_GDSCR);
	while(readl(MDSS_GDSCR) & (GDSC_POWER_ON_BIT));

	reg = readl(MMAGIC_MDSS_GDSCR);
	reg |= BIT(0);
	writel(reg, MMAGIC_MDSS_GDSCR);
	while(readl(MMAGIC_MDSS_GDSCR) & (GDSC_POWER_ON_BIT));

	reg = readl(MMAGIC_BIMC_GDSCR);
	reg |= BIT(0);
	writel(reg, MMAGIC_BIMC_GDSCR);
	while(readl(MMAGIC_BIMC_GDSCR) & (GDSC_POWER_ON_BIT));
}

void video_gdsc_enable()
{
	uint32_t reg = 0;

	reg = readl(MMAGIC_VIDEO_GDSCR);
	if (!(reg & GDSC_POWER_ON_BIT)) {
		reg &=  ~(BIT(0) | GDSC_EN_FEW_WAIT_MASK);
		reg |= GDSC_EN_FEW_WAIT_256_MASK;
		writel(reg, MMAGIC_VIDEO_GDSCR);
		while(!(readl(MMAGIC_VIDEO_GDSCR) & (GDSC_POWER_ON_BIT)));
	} else {
		dprintf(SPEW, "VIDEO BIMC GDSC already enabled\n");
	}

	reg = readl(VIDEO_GDSCR);
	if (!(reg & GDSC_POWER_ON_BIT)) {
		reg &=  ~(BIT(0) | GDSC_EN_FEW_WAIT_MASK);
		reg |= GDSC_EN_FEW_WAIT_256_MASK;
		writel(reg, VIDEO_GDSCR);
		while(!(readl(VIDEO_GDSCR) & (GDSC_POWER_ON_BIT)));
	} else {
		dprintf(SPEW, "VIDEO GDSC already enabled\n");
	}
}

void video_gdsc_disable()
{
	uint32_t reg = 0;

	reg = readl(VIDEO_GDSCR);
	reg |= BIT(0);
	writel(reg, VIDEO_GDSCR);
	while(readl(VIDEO_GDSCR) & (GDSC_POWER_ON_BIT));

	reg = readl(MMAGIC_VIDEO_GDSCR);
	reg |= BIT(0);
	writel(reg, MMAGIC_VIDEO_GDSCR);
	while(readl(MMAGIC_VIDEO_GDSCR) & (GDSC_POWER_ON_BIT));
}

/* Configure MDP clock */
void mdp_clock_enable(void)
{
	int ret;

	ret = clk_get_set_enable("mmss_mmagic_ahb_clk", 19200000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmagic_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("smmu_mdp_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set smmu_mdp_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdp_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_clk", 320000000, 1);
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

}

void mdp_clock_disable()
{
	clk_disable(clk_get("mdss_vsync_clk"));
	clk_disable(clk_get("mdss_mdp_clk"));
	clk_disable(clk_get("mdp_ahb_clk"));
	clk_disable(clk_get("smmu_mdp_ahb_clk"));
	clk_disable(clk_get("mmss_mmagic_ahb_clk"));
}

void mmss_bus_clock_enable(void)
{
	int ret;
	ret = clk_get_set_enable("mmss_mmagic_axi_clk", 320000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_mmagic_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mmagic_bimc_axi_clk", 320000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_s0_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mmss_s0_axi_clk", 320000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_s0_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mmagic_mdss_axi_clk", 320000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_mmagic_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("smmu_mdp_axi_clk", 320000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set smmu_mdp_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_axi_clk", 320000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdss_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void mmss_bus_clock_disable(void)
{
	clk_disable(clk_get("mdss_axi_clk"));
	clk_disable(clk_get("smmu_mdp_axi_clk"));
	clk_disable(clk_get("mmagic_mdss_axi_clk"));
	clk_disable(clk_get("mmss_s0_axi_clk"));
	clk_disable(clk_get("mmagic_bimc_axi_clk"));
	clk_disable(clk_get("mmss_mmagic_axi_clk"));
}

void mmss_dsi_clock_enable(uint32_t cfg_rcgr, uint32_t flags)
{
	int ret;

	if (flags & MMSS_DSI_CLKS_FLAG_DSI0) {
		/* Enable DSI0 branch clocks */

		writel(cfg_rcgr, DSI_BYTE0_CFG_RCGR);
		writel(0x1, DSI_BYTE0_CMD_RCGR);
		writel(0x1, DSI_BYTE0_CBCR);

		writel(cfg_rcgr, DSI_PIXEL0_CFG_RCGR);
		writel(0x1, DSI_PIXEL0_CMD_RCGR);
		writel(0x1, DSI_PIXEL0_CBCR);

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
		writel(0x1, DSI_BYTE1_CBCR);

		writel(cfg_rcgr, DSI_PIXEL1_CFG_RCGR);
		writel(0x1, DSI_PIXEL1_CMD_RCGR);
		writel(0x1, DSI_PIXEL1_CBCR);

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


void clock_config_blsp_i2c(uint8_t blsp_id, uint8_t qup_id)
{
	uint8_t ret = 0;
	char clk_name[64];

	struct clk *qup_clk;

	if((blsp_id != BLSP_ID_2) || ((qup_id != QUP_ID_1) &&
		(qup_id != QUP_ID_3))) {
		dprintf(CRITICAL, "Incorrect BLSP-%d or QUP-%d configuration\n",
			blsp_id, qup_id);
		ASSERT(0);
	}

	if (qup_id == QUP_ID_1) {
		snprintf(clk_name, sizeof(clk_name), "blsp2_qup2_ahb_iface_clk");
	}
	else if (qup_id == QUP_ID_3) {
		snprintf(clk_name, sizeof(clk_name), "blsp1_qup4_ahb_iface_clk");
	}

	ret = clk_get_set_enable(clk_name, 0 , 1);
	if (ret) {
		dprintf(CRITICAL, "Failed to enable %s clock\n", clk_name);
		return;
	}

	if (qup_id == QUP_ID_1) {
		snprintf(clk_name, sizeof(clk_name), "gcc_blsp2_qup2_i2c_apps_clk");
	}
	else if (qup_id == QUP_ID_3) {
		snprintf(clk_name, sizeof(clk_name), "gcc_blsp1_qup4_i2c_apps_clk");
	}

	qup_clk = clk_get(clk_name);
	if (!qup_clk) {
		dprintf(CRITICAL, "Failed to get %s\n", clk_name);
		return;
	}

	ret = clk_enable(qup_clk);
	if (ret) {
		dprintf(CRITICAL, "Failed to enable %s\n", clk_name);
		return;
	}
}

void hdmi_ahb_core_clk_enable(void)
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
}

void hdmi_pixel_clk_enable(uint32_t rate) {
	int ret;

	/* Configure hdmi pixel clock */
	ret = clk_get_set_enable("hdmi_extp_clk", rate, 1);
	if(ret) {
		dprintf(CRITICAL, "failed to set hdmi_extp_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void hdmi_pixel_clk_disable(void)
{
	clk_disable(clk_get("hdmi_extp_clk"));
}

void hdmi_core_ahb_clk_disable(void)
{
	clk_disable(clk_get("hdmi_core_clk"));
	clk_disable(clk_get("hdmi_ahb_clk"));
}
