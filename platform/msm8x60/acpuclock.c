/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
#include <kernel/thread.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <platform/scm-io.h>
#include <reg.h>

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
        [PLL_1] = { MM_PLL0_MODE_REG, MM_PLL0_STATUS_REG },
        [PLL_2] = { MM_PLL1_MODE_REG, MM_PLL1_STATUS_REG },
        [PLL_3] = { MM_PLL2_MODE_REG, MM_PLL2_STATUS_REG },
    };
    uint32_t pll_mode;

    pll_mode = readl(pll_reg[src].mode_reg);
    if (enable) {
        /* Disable PLL bypass mode. */
        pll_mode |= (1<<1);
        writel( pll_mode, pll_reg[src].mode_reg);

        /* H/W requires a 5us delay between disabling the bypass and
         * de-asserting the reset. Delay 10us just to be safe. */
        udelay(10);

        /* De-assert active-low PLL reset. */
        pll_mode |= (1<<2);
        writel( pll_mode, pll_reg[src].mode_reg);

        /* Enable PLL output. */
        pll_mode |= (1<<0);
        writel( pll_mode, pll_reg[src].mode_reg);

        /* Wait until PLL is enabled. */
        while (!readl(pll_reg[src].status_reg));
    } else {
        /* Disable the PLL output, disable test mode, enable
         * the bypass mode, and assert the reset. */
        pll_mode &= 0xFFFFFFF0;
        writel( pll_mode, pll_reg[src].mode_reg);
    }

    return 0;
}


/* Write the M,N,D values and enable the MDP Core Clock */
void config_mdp_clk(    uint32_t ns,
        uint32_t md,
        uint32_t cc,
        uint32_t ns_addr,
        uint32_t md_addr,
        uint32_t cc_addr)
{
    int val = 0;

    /* MN counter reset */
    val = 1 << 31;
    writel(val, ns_addr);

    /* Write the MD and CC register values */
    writel(md, md_addr);
    writel(cc, cc_addr);

    /* Reset the clk control, and Write ns val */
    val = 1 << 31;
    val |= ns;
    writel(val, ns_addr);

    /* Clear MN counter reset */
    val = 1 << 31;
    val = ~val;
    val = val & readl(ns_addr);
    writel(val, ns_addr);

    /* Enable MND counter */
    val = 1 << 8;
    val = val | readl(cc_addr);
    writel(val, cc_addr);

    /* Enable the root of the clock tree */
    val = 1 << 2;
    val = val | readl(cc_addr);
    writel(val, cc_addr);

    /* Enable the MDP Clock */
    val = 1 << 0;
    val = val | readl(cc_addr);
    writel(val, cc_addr);
}

/* Write the M,N,D values and enable the Pixel Core Clock */
void config_pixel_clk(  uint32_t ns,
        uint32_t md,
        uint32_t cc,
        uint32_t ns_addr,
        uint32_t md_addr,
        uint32_t cc_addr){
    unsigned int val = 0;

    /* Activate the reset for the M/N Counter */
    val = 1 << 7;
    writel(val, ns_addr);

    /* Write the MD and CC register values */
    writel(md, md_addr);
    writel(cc, cc_addr);

    /* Write the ns value, and active reset for M/N Counter, again */
    val = 1 << 7;
    val |= ns;
    writel(val, ns_addr);

    /* De-activate the reset for M/N Counter */
    val = 1 << 7;
    val = ~val;
    val = val & readl(ns_addr);
    writel(val, ns_addr);

    /* Enable MND counter */
    val = 1 << 5;
    val = val | readl(cc_addr);
    writel(val, cc_addr);

    /* Enable the root of the clock tree */
    val = 1 << 2;
    val = val | readl(cc_addr);
    writel(val, cc_addr);

    /* Enable the MDP Clock */
    val = 1 << 0;
    val = val | readl(cc_addr);
    writel(val, cc_addr);

    /* Enable the LCDC Clock */
    val = 1 << 8;
    val = val | readl(cc_addr);
    writel(val, cc_addr);
}

/* Set rate and enable the clock */
void clock_config(uint32_t ns,
        uint32_t md,
        uint32_t ns_addr,
        uint32_t md_addr)
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

void acpu_clock_init (void)
{
}

void hsusb_clock_init(void)
{
	int val;
	/* Vote for PLL8 */
	val = readl(0x009034C0);
	val |= (1<<8);
	writel(val, 0x009034C0);
	/* Wait until PLL is enabled. */
	while (!(readl(0x00903158) & (1<<16)));

	//Set 7th bit in NS Register
	val = 1 << 7;
	writel(val, USB_HS1_XVCR_FS_CLK_NS);

	//Set rate specific value in MD
	writel(0x000500DF, USB_HS1_XVCR_FS_CLK_MD);

	//Set value in NS register
	val = 1 << 7;
	val |= 0x00E400C3;
	writel(val, USB_HS1_XVCR_FS_CLK_NS);

	// Clear 7th bit
	val = 1 << 7;
	val = ~val;
	val = val & readl(USB_HS1_XVCR_FS_CLK_NS);
	writel(val, USB_HS1_XVCR_FS_CLK_NS);

	//set 11th bit
	val = 1 << 11;
	val |= readl(USB_HS1_XVCR_FS_CLK_NS);
	writel(val, USB_HS1_XVCR_FS_CLK_NS);

	//set 9th bit
	val = 1 << 9;
	val |= readl(USB_HS1_XVCR_FS_CLK_NS);
	writel(val, USB_HS1_XVCR_FS_CLK_NS);

	//set 8th bit
	val = 1 << 8;
	val |= readl(USB_HS1_XVCR_FS_CLK_NS);
	writel(val, USB_HS1_XVCR_FS_CLK_NS);
}

