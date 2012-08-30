/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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

#include <err.h>
#include <assert.h>
#include <debug.h>
#include <reg.h>
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

	mdelay(20);

	iclk = clk_get("usb_iface_clk");
	cclk = clk_get("usb_core_clk");

	/* Disable USB all clock init */
	writel(0, USB_BOOT_CLOCK_CTL);

	clk_disable(iclk);
	clk_disable(cclk);

	mdelay(20);

	/* Start the block reset for usb */
	writel(1, USB_HS_BCR);

	mdelay(20);

	/* Take usb block out of reset */
	writel(0, USB_HS_BCR); 

	mdelay(20);

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
	uint32_t reg;
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

	reg = 0;
	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);
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
		dprintf(CRITICAL, "failed to set uart1_core_clk ret = %d\n", ret);
		ASSERT(0);
	}
}
