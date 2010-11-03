/*
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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
#include <kernel/thread.h>
#include <platform/iomap.h>
#include <reg.h>

#define REG_BASE(off)           (MSM_CLK_CTL_BASE + (off))
#define REG(off)                (MSM_CLK_CTL_SH2_BASE + (off))

#define SH2_OWN_ROW2_BASE_REG	REG_BASE(0x0424)

void acpu_clock_init(void)
{
    /* Bump clock speed to 768 MHz */
    writel(0x0, SCSS_CLK_SEL);
    writel(0x1020, SCSS_CLK_CTL);
    writel(0x1, SCSS_CLK_SEL);
}

void hsusb_clock_init(void)
{
	int val = 0;
	unsigned sh2_own_row2;
	unsigned sh2_own_row2_hsusb_mask = (1 << 11);

	sh2_own_row2 = readl(SH2_OWN_ROW2_BASE_REG);
	if(sh2_own_row2 & sh2_own_row2_hsusb_mask)
	{
		/* USB local clock control enabled */
	    /* Set value in MD register */
	    val = 0x5DF;
	    writel(val, SH2_USBH_MD_REG);

	    /* Set value in NS register */
	    val = 1 << 8;
	    val = val | readl(SH2_USBH_NS_REG);
	    writel(val, SH2_USBH_NS_REG);

	    val = 1 << 11;
	    val = val | readl(SH2_USBH_NS_REG);
	    writel(val, SH2_USBH_NS_REG);

	    val = 1 << 9;
	    val = val | readl(SH2_USBH_NS_REG);
	    writel(val, SH2_USBH_NS_REG);

	    val = 1 << 13;
	    val = val | readl(SH2_USBH_NS_REG);
	    writel(val, SH2_USBH_NS_REG);

	    /* Enable USBH_P_CLK */
	    val = 1 << 25;
	    val = val | readl(SH2_GLBL_CLK_ENA_SC);
	    writel(val, SH2_GLBL_CLK_ENA_SC);
	}
	else
	{
		/* USB local clock control not enabled; use proc comm */
		usb_clock_init();
	}
}

void adm_enable_clock(void)
{
	unsigned int val=0;

	/* Enable ADM_CLK */
	val = 1 << 5;
	val = val | readl(SH2_GLBL_CLK_ENA_SC);
	writel(val, SH2_GLBL_CLK_ENA_SC);
}
