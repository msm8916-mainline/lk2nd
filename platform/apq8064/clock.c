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

#include <reg.h>
#include <debug.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <gsbi.h>
#include <mmc.h>
#include <uart_dm.h>

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

void hsusb_clock_init(void)
{
	/* Setup XCVR clock */
	clock_config(USB_XCVR_CLK_NS_VAL,
				 USB_XCVR_CLK_MD_VAL,
				 USB_HS1_XCVR_FS_CLK_NS,
				 USB_HS1_XCVR_FS_CLK_MD);
}

/* Configure UART clock - based on the gsbi id */
void clock_config_uart_dm(uint8_t id)
{
	/* Enable gsbi_uart_clk */
	clock_config(UART_DM_CLK_NS_115200,
				 UART_DM_CLK_MD_115200,
				 GSBIn_QUP_APPS_NS(id),
				 GSBIn_QUP_APPS_MD(id));

	/* Configure clock selection register for tx and rx rates.
	 * Selecting 115.2k for both RX and TX.
	 */
	writel(UART_DM_CLK_RX_TX_BIT_RATE, MSM_BOOT_UART_DM_CSR(id));

	/* Enable gsbi_pclk */
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

