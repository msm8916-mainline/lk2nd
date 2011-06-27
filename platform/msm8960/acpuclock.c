/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <uart_dm.h>
#include <gsbi.h>
#include <mmc.h>

/* Set rate and enable the clock */
void clock_config(uint32_t ns, uint32_t md, uint32_t ns_addr, uint32_t md_addr)
{
	unsigned int val = 0;

	/* Activate the reset for the M/N Counter */
	val = 1 << 7;
	writel(val, ns_addr);

	/* Write the MD value into the MD register */
	if (md_addr != 0x0)
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

	/* Enable the Clock Root */
	val = 1 << 11;
	val = val | readl(ns_addr);
	writel(val, ns_addr);

	/* Enable the Clock Branch */
	val = 1 << 9;
	val = val | readl(ns_addr);
	writel(val, ns_addr);

	/* Enable the M/N Counter */
	val = 1 << 8;
	val = val | readl(ns_addr);
	writel(val, ns_addr);
}

/* Write the M,N,D values and enable the MMSS Clocks */
void config_mmss_clk(  uint32_t ns,
		uint32_t md,
		uint32_t cc,
		uint32_t ns_addr,
		uint32_t md_addr,
		uint32_t cc_addr){
	unsigned int val = 0;

	clock_config(ns, md, ns_addr, md_addr);

	/* Enable MND counter */
	val = cc | (1 << 5);
	val = val | readl(cc_addr);
	writel(val, cc_addr);

	/* Enable the root of the clock tree */
	val = 1 << 2;
	val = val | readl(cc_addr);
	writel(val, cc_addr);

	/* Enable the Pixel Clock */
	val = 1 << 0;
	val = val | readl(cc_addr);
	writel(val, cc_addr);

	/* Force On */
	val = 1 << 31;
	val = val | readl(cc_addr);
	writel(val, cc_addr);
}

void pll8_enable(void)
{
	unsigned int curr_value = 0;

	/* Vote for PLL8 to be enabled */
	curr_value = readl(MSM_BOOT_PLL_ENABLE_SC0);
	curr_value |= (1 << 8);
	writel(curr_value, MSM_BOOT_PLL_ENABLE_SC0);

	/* Proceed only after PLL is enabled */
	while (!(readl(MSM_BOOT_PLL8_STATUS) & (1<<16)));
}

void hsusb_clock_init(void)
{
	/* TODO: Enable pll8 here */
	/* Setup USB AHB clock */

	/* Setup XCVR clock */
	clock_config(USB_XCVR_CLK_NS,
				 USB_XCVR_CLK_MD,
				 USB_HS1_XCVR_FS_CLK_NS,
				 USB_HS1_XCVR_FS_CLK_MD);
}

/* Configure UART clock - based on the gsbi id */
void clock_config_uart_dm(uint8_t id)
{
	/* Enable gsbi_uart_clk */
	clock_config(UART_DM_CLK_NS_115200,
				 UART_DM_CLK_MD_115200,
				 GSBIn_UART_APPS_NS(id),
				 GSBIn_UART_APPS_MD(id));


	/* Enable gsbi_pclk */
	writel(GSBI_HCLK_CTL_CLK_ENA << GSBI_HCLK_CTL_S, GSBIn_HCLK_CTL(id));
}

/* Configure i2c clock */
void clock_config_i2c(uint8_t id, uint32_t freq)
{
	uint32_t ns;
	uint32_t md;

	switch (freq)
	{
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

void pll1_enable(void){
	uint32_t val = 0;

	/* Reset MND divider */
	val |= (1<<2);
	writel(val, MM_PLL1_MODE_REG);

	/* Use PLL -- Disable Bypass */
	val |= (1<<1);
	writel(val, MM_PLL1_MODE_REG);

	/* Activate PLL out control */
	val |= 1;
	writel(val, MM_PLL1_MODE_REG);

	while (!readl(MM_PLL1_STATUS_REG));
}

void config_mdp_lut_clk(void)
{
	/* Force on*/
	writel(MDP_LUT_VAL, MDP_LUT_CC_REG);
}

/* Turn on MDP related clocks and pll's for MDP */
void mdp_clock_init(void)
{
	/* Turn on the PLL1, as source for  MDP clock */
	pll1_enable();

	/* Turn on MDP clk */
	config_mmss_clk(MDP_NS_VAL, MDP_MD_VAL,
				MDP_CC_VAL, MDP_NS_REG, MDP_MD_REG, MDP_CC_REG);

	/* Seems to lose pixels without this from status 0x051E0048 */
	config_mdp_lut_clk();
}

/* Initialize all clocks needed by Display */
void mmss_clock_init(void)
{
	/* Configure Pixel clock */
	config_mmss_clk(PIXEL_NS_VAL, PIXEL_MD_VAL, PIXEL_CC_VAL, PIXEL_NS_REG, PIXEL_MD_REG, PIXEL_CC_REG);

	/* Configure DSI clock */
	config_mmss_clk(DSI_NS_VAL, DSI_MD_VAL, DSI_CC_VAL, DSI_NS_REG, DSI_MD_REG, DSI_CC_REG);

	/* Configure Byte clock */
	config_mmss_clk(BYTE_NS_VAL, 0x0, BYTE_CC_VAL, BYTE_NS_REG, 0x0, BYTE_CC_REG);

	/* Configure ESC clock */
	config_mmss_clk(ESC_NS_VAL, 0x0, ESC_CC_VAL, ESC_NS_REG, 0x0, ESC_CC_REG);
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

	switch(freq)
	{
	case MMC_CLK_400KHZ:
		clock_config(SDC_CLK_NS_400KHZ,
					 SDC_CLK_MD_400KHZ,
					 SDC_NS(interface),
					 SDC_MD(interface));
		break;
	case MMC_CLK_48MHZ:
	case MMC_CLK_50MHZ: /* Max supported is 48MHZ */
		clock_config(SDC_CLK_NS_48MHZ,
					 SDC_CLK_MD_48MHZ,
					 SDC_NS(interface),
					 SDC_MD(interface));
		break;
	default:
		ASSERT(0);

	}

	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel( reg, MMC_BOOT_MCI_CLK );
}
