/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2011, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
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
#include <reg.h>
#include <stdint.h>
#include <debug.h>
#include <gsbi.h>
#include <platform/iomap.h>
#include <platform/clock.h>

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

void acpu_clock_init(void)
{
	/* ADM3: enable cc_adm0_clk */
	writel( (1 << SC0_U_BRANCH_ENA_VOTE_ADM0) |
		(1 << SC0_U_BRANCH_ENA_VOTE_ADM0_PBUS), SC0_U_BRANCH_ENA_VOTE);
}

void hsusb_clock_init(void)
{
	uint32_t reg;

	pll8_enable();

	/* Setup USB HS1 System clock - 60 Mhz */
	//TODO: Remove this when verify that this is already configured
	if (!(readl(USB_HS1_SYSTEM_CLK_NS))){
		clock_config(USB_XCVR_CLK_NS,
					USB_XCVR_CLK_MD,
					USB_HS1_SYSTEM_CLK_NS,
					USB_HS1_SYSTEM_CLK_MD);
	}

	/* Setup USB HS1 XCVR clock - 60 Mhz */
	clock_config(USB_XCVR_CLK_NS,
				USB_XCVR_CLK_MD,
				USB_HS1_XCVR_FS_CLK_NS,
				USB_HS1_XCVR_FS_CLK_MD);

	/* HS-USB: enable cc_usb_hs1_hs_clk */
	reg = readl(USB_HS1_HCLK_CTL);
	reg |= 0x1 << 4;
	writel( reg, USB_HS1_HCLK_CTL);
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
