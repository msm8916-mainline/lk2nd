/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Linux Foundation nor
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
#include <clock.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/timer.h>

void hsusb_clock_init(void)
{
	int ret;
	struct clk *iclk;
	struct clk *cclk;

	iclk = clk_get("usb_iface_clk");
	cclk = clk_get("usb_core_clk");

	ASSERT(iclk);
	ASSERT(cclk);

	/* Disable interface and core clk */
	clk_disable(iclk);
	clk_disable(cclk);

	/* Start the block reset for usb */
	writel(1, USB_HS_BCR);

	/* Take usb block out of reset */
	writel(0, USB_HS_BCR);

	ret = clk_enable(iclk);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_set_rate(cclk, 75000000);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set_rate of usb_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_enable(cclk);
	if(ret)
	{
		dprintf(CRITICAL, "failed to enable usb_core_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

void clock_config_uart_dm(uint8_t id)
{
	int ret;
	char clk_name[64];

    ret = clk_get_set_enable("uart_iface_clk", 0, 1);
    if (ret)
	{
		dprintf(CRITICAL, "failed to set uart_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "uart%u_core_clk", id);

    ret = clk_get_set_enable(clk_name, 7372800, 1);
	if (ret)
	{
		dprintf(CRITICAL, "failed to set uart%u_core_clk ret = %d\n", id, ret);
		ASSERT(0);
	}
}

