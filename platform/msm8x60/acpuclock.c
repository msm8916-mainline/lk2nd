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
#include <reg.h>

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

