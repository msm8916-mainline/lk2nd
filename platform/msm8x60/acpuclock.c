/*
 * Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Code Aurora nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <debug.h>
#include <reg.h>
#include <kernel/thread.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <platform/scm-io.h>
#include <uart_dm.h>
#include <gsbi.h>
#include <mmc.h>

/* Read, modify, then write-back a register. */
static void rmwreg(uint32_t val, uint32_t reg, uint32_t mask)
{
	uint32_t regval = readl(reg);
	regval &= ~mask;
	regval |= val;
	writel(regval, reg);
}

/* Enable/disable for non-shared NT PLLs. */
int nt_pll_enable(uint8_t src, uint8_t enable)
{
	static const struct {
		uint32_t const mode_reg;
		uint32_t const status_reg;
	} pll_reg[] = {
		[PLL_1] = {
		MM_PLL0_MODE_REG, MM_PLL0_STATUS_REG},[PLL_2] = {
		MM_PLL1_MODE_REG, MM_PLL1_STATUS_REG},[PLL_3] = {
	MM_PLL2_MODE_REG, MM_PLL2_STATUS_REG},};
	uint32_t pll_mode;

	pll_mode = secure_readl(pll_reg[src].mode_reg);
	if (enable) {
		/* Disable PLL bypass mode. */
		pll_mode |= (1 << 1);
		secure_writel(pll_mode, pll_reg[src].mode_reg);

		/* H/W requires a 5us delay between disabling the bypass and
		 * de-asserting the reset. Delay 10us just to be safe. */
		udelay(10);

		/* De-assert active-low PLL reset. */
		pll_mode |= (1 << 2);
		secure_writel(pll_mode, pll_reg[src].mode_reg);

		/* Enable PLL output. */
		pll_mode |= (1 << 0);
		secure_writel(pll_mode, pll_reg[src].mode_reg);

		/* Wait until PLL is enabled. */
		while (!secure_readl(pll_reg[src].status_reg)) ;
	} else {
		/* Disable the PLL output, disable test mode, enable
		 * the bypass mode, and assert the reset. */
		pll_mode &= 0xFFFFFFF0;
		secure_writel(pll_mode, pll_reg[src].mode_reg);
	}

	return 0;
}

/* Write the M,N,D values and enable the MDP Core Clock */
void config_mdp_clk(uint32_t ns,
		    uint32_t md,
		    uint32_t cc,
		    uint32_t ns_addr, uint32_t md_addr, uint32_t cc_addr)
{
	unsigned int val = 0;

	/* MN counter reset */
	val = 1 << 31;
	secure_writel(val, ns_addr);

	/* Write the MD and CC register values */
	secure_writel(md, md_addr);
	secure_writel(cc, cc_addr);

	/* Reset the clk control, and Write ns val */
	val = 1 << 31;
	val |= ns;
	secure_writel(val, ns_addr);

	/* Clear MN counter reset */
	val = 1 << 31;
	val = ~val;
	val = val & secure_readl(ns_addr);
	secure_writel(val, ns_addr);

	/* Enable MND counter */
	val = 1 << 8;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);

	/* Enable the root of the clock tree */
	val = 1 << 2;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);

	/* Enable the MDP Clock */
	val = 1 << 0;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);
}

/* Write the M,N,D values and enable the Pixel Core Clock */
void config_pixel_clk(uint32_t ns,
		      uint32_t md,
		      uint32_t cc,
		      uint32_t ns_addr, uint32_t md_addr, uint32_t cc_addr)
{
	unsigned int val = 0;

	/* Activate the reset for the M/N Counter */
	val = 1 << 7;
	secure_writel(val, ns_addr);

	/* Write the MD and CC register values */
	secure_writel(md, md_addr);
	secure_writel(cc, cc_addr);

	/* Write the ns value, and active reset for M/N Counter, again */
	val = 1 << 7;
	val |= ns;
	secure_writel(val, ns_addr);

	/* De-activate the reset for M/N Counter */
	val = 1 << 7;
	val = ~val;
	val = val & secure_readl(ns_addr);
	secure_writel(val, ns_addr);

	/* Enable MND counter */
	val = 1 << 5;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);

	/* Enable the root of the clock tree */
	val = 1 << 2;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);

	/* Enable the MDP Clock */
	val = 1 << 0;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);

	/* Enable the LCDC Clock */
	val = 1 << 8;
	val = val | secure_readl(cc_addr);
	secure_writel(val, cc_addr);
}

/* Set rate and enable the clock */
void clock_config(uint32_t ns, uint32_t md, uint32_t ns_addr, uint32_t md_addr)
{
	unsigned int val = 0;

	/* Activate the reset for the M/N Counter */
	val = 1 << 7;
	writel(val, ns_addr);

	/* Write the MD value into the MD register */
	writel(md, md_addr);

	/* Write the ns value, and active reset for M/N Counter, again */
	val = 1 << 7;
	val |= ns;
	writel(val, ns_addr);

	/* De-activate the reset for M/N Counter */
	val = 1 << 7;
	val = ~val;
	val = val & readl(ns_addr);
	writel(val, ns_addr);

	/* Enable the M/N Counter */
	val = 1 << 8;
	val = val | readl(ns_addr);
	writel(val, ns_addr);

	/* Enable the Clock Root */
	val = 1 << 11;
	val = val | readl(ns_addr);
	writel(val, ns_addr);

	/* Enable the Clock Branch */
	val = 1 << 9;
	val = val | readl(ns_addr);
	writel(val, ns_addr);
}

void pll8_enable(void)
{
	/* Currently both UART and USB depend on this PLL8 clock initialization. */
	unsigned int curr_value = 0;

	/* Vote for PLL8 to be enabled */
	curr_value = readl(MSM_BOOT_PLL_ENABLE_SC0);
	curr_value |= (1 << 8);
	writel(curr_value, MSM_BOOT_PLL_ENABLE_SC0);

	/* Proceed only after PLL is enabled */
	while (!(readl(MSM_BOOT_PLL8_STATUS) & (1 << 16))) ;
}

void uart_clock_init(void)
{
	/* Enable PLL8 */
	pll8_enable();
}

void hsusb_clock_init(void)
{
	int val;

	/* Enable PLL8 */
	pll8_enable();

	//Set 7th bit in NS Register
	val = 1 << 7;
	writel(val, USB_HS1_XCVR_FS_CLK_NS);

	//Set rate specific value in MD
	writel(0x000500DF, USB_HS1_XCVR_FS_CLK_MD);

	//Set value in NS register
	val = 1 << 7;
	val |= 0x00E400C3;
	writel(val, USB_HS1_XCVR_FS_CLK_NS);

	// Clear 7th bit
	val = 1 << 7;
	val = ~val;
	val = val & readl(USB_HS1_XCVR_FS_CLK_NS);
	writel(val, USB_HS1_XCVR_FS_CLK_NS);

	//set 11th bit
	val = 1 << 11;
	val |= readl(USB_HS1_XCVR_FS_CLK_NS);
	writel(val, USB_HS1_XCVR_FS_CLK_NS);

	//set 9th bit
	val = 1 << 9;
	val |= readl(USB_HS1_XCVR_FS_CLK_NS);
	writel(val, USB_HS1_XCVR_FS_CLK_NS);

	//set 8th bit
	val = 1 << 8;
	val |= readl(USB_HS1_XCVR_FS_CLK_NS);
	writel(val, USB_HS1_XCVR_FS_CLK_NS);
}

void ce_clock_init(void)
{
	/* Enable clock branch for CE2 */
	writel((1 << 4), CE2_HCLK_CTL);
	return;
}

/* Configure UART clock - based on the gsbi id */
void clock_config_uart_dm(uint8_t id)
{
	uint32_t ns = UART_DM_CLK_NS_115200;
	uint32_t md = UART_DM_CLK_MD_115200;

	/* Enable PLL8 */
	pll8_enable();

	/* Enable gsbi_uart_clk */
	clock_config(ns, md, GSBIn_UART_APPS_NS(id), GSBIn_UART_APPS_MD(id));

	/* Enable the GSBI HCLK */
	writel(GSBI_HCLK_CTL_CLK_ENA << GSBI_HCLK_CTL_S, GSBIn_HCLK_CTL(id));
}

/* Configure i2c clock */
void clock_config_i2c(uint8_t id, uint32_t freq)
{
	uint32_t ns;
	uint32_t md;

	switch (freq) {
	case 24000000:
		ns = I2C_CLK_NS_24MHz;
		md = I2C_CLK_MD_24MHz;
		break;
	default:
		ASSERT(0);
	}

	clock_config(ns, md, GSBIn_QUP_APPS_NS(id), GSBIn_QUP_APPS_MD(id));

	/* Enable the GSBI HCLK */
	writel(GSBI_HCLK_CTL_CLK_ENA << GSBI_HCLK_CTL_S, GSBIn_HCLK_CTL(id));
}

/* Intialize MMC clock */
void clock_init_mmc(uint32_t interface)
{
	/* Nothing to be done. */
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	uint32_t reg = 0;

	switch (freq) {
	case MMC_CLK_400KHZ:
		clock_config(SDC_CLK_NS_400KHZ,
			     SDC_CLK_MD_400KHZ,
			     SDC_NS(interface), SDC_MD(interface));
		break;
	case MMC_CLK_48MHZ:
	case MMC_CLK_50MHZ:	/* Max supported is 48MHZ */
		clock_config(SDC_CLK_NS_48MHZ,
			     SDC_CLK_MD_48MHZ,
			     SDC_NS(interface), SDC_MD(interface));
		break;
	default:
		ASSERT(0);

	}

	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);
}

void mdp_clock_init(void)
{
	/* Turn on the PLL2, to ramp up the MDP clock to max (200MHz) */
	nt_pll_enable(PLL_2, 1);

	config_mdp_clk(MDP_NS_VAL, MDP_MD_VAL,
		       MDP_CC_VAL, MDP_NS_REG, MDP_MD_REG, MDP_CC_REG);
}

void mmss_pixel_clock_configure(uint32_t pclk_id)
{
	if (pclk_id == PIXEL_CLK_INDEX_25M) {
		config_pixel_clk(PIXEL_NS_VAL_25M, PIXEL_MD_VAL_25M,
				 PIXEL_CC_VAL_25M, MMSS_PIXEL_NS_REG,
				 MMSS_PIXEL_MD_REG, MMSS_PIXEL_CC_REG);
	} else {
		config_pixel_clk(PIXEL_NS_VAL, PIXEL_MD_VAL,
				 PIXEL_CC_VAL, MMSS_PIXEL_NS_REG,
				 MMSS_PIXEL_MD_REG, MMSS_PIXEL_CC_REG);
	}
}

void configure_dsicore_dsiclk()
{
	unsigned char mnd_mode, root_en, clk_en;
	unsigned long src_sel = 0x3;	// dsi_phy_pll0_src
	unsigned long pre_div_func = 0x00;	// predivide by 1
	unsigned long pmxo_sel;

	secure_writel(pre_div_func << 14 | src_sel, DSI_NS_REG);
	mnd_mode = 0;		// Bypass MND
	root_en = 1;
	clk_en = 1;
	pmxo_sel = 0;

	secure_writel((pmxo_sel << 8) | (mnd_mode << 6), DSI_CC_REG);
	secure_writel(secure_readl(DSI_CC_REG) | root_en << 2, DSI_CC_REG);
	secure_writel(secure_readl(DSI_CC_REG) | clk_en, DSI_CC_REG);
}

void configure_dsicore_byteclk(void)
{
	secure_writel(0x00400401, MISC_CC2_REG);	// select pxo
}

void configure_dsicore_pclk(void)
{
	unsigned char mnd_mode, root_en, clk_en;
	unsigned long src_sel = 0x3;	// dsi_phy_pll0_src
	unsigned long pre_div_func = 0x01;	// predivide by 2

	secure_writel(pre_div_func << 12 | src_sel, DSI_PIXEL_NS_REG);

	mnd_mode = 0;		// Bypass MND
	root_en = 1;
	clk_en = 1;
	secure_writel(mnd_mode << 6, DSI_PIXEL_CC_REG);
	secure_writel(secure_readl(DSI_PIXEL_CC_REG) | root_en << 2, DSI_PIXEL_CC_REG);
	secure_writel(secure_readl(DSI_PIXEL_CC_REG) | clk_en, DSI_PIXEL_CC_REG);
}
/* Async Reset CE2 */
void ce_async_reset()
{
	/* Enable Async reset bit for HCLK CE2 */
	writel((1<<7) | (1 << 4), CE2_HCLK_CTL);

	/* Add a small delay between switching the
	 * async intput from high to low
	 */
	udelay(2);

	/* Disable Async reset bit for HCLK for CE2 */
	writel((1 << 4), CE2_HCLK_CTL);

	return;
}
