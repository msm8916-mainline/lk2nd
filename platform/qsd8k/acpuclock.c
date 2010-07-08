/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
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

#include <stdint.h>
#include <kernel/thread.h>
#include <platform/iomap.h>
#include <reg.h>

#define A11S_CLK_CNTL 0xAC100100
#define A11S_CLK_SEL  0xAC100104

#define SCPLL_CTL  0xA8800004
#define SCPLL_CTLE 0xA8800010
#define SCPLL_STAT 0xA8800018

void acpu_clock_init(void)
{
	unsigned val;

    /* Init Scorpion PLL */
    writel(0x0, SCPLL_CTL);
    writel(0x00400002, SCPLL_CTL);
    writel(0x00600004, SCPLL_CTL);
    thread_sleep(1);
    while(readl(SCPLL_STAT) & 0x2);
    writel(0x0, SCPLL_CTL);

    /* Enable pll */
    while(readl(SCPLL_STAT) & 0x1);
    val = readl(SCPLL_CTL);
    val &= ~(0x7);
    val |= 0x2;
    writel(val, SCPLL_CTL);
    thread_sleep(1);
    val = readl(SCPLL_CTL);
    val |= 0x7;
    writel(val, SCPLL_CTL);
    thread_sleep(1);

    /* For Scorpion PLL, must first SHOT to 384MHz then HOP to 768MHz */

    /* Set pll to 384 MHz */
    while(readl(SCPLL_STAT) & 0x3);
    val = readl(SCPLL_CTLE);
    val &= ~(0x3F << 3);
    val |= (0xA << 3);
    val &= ~(0x3 << 0);
    val |= (4 << 0);  // SHOT method
    writel(val, SCPLL_CTLE);
    writel(0x00600007, SCPLL_CTL);
    thread_sleep(1);

    /* HOP to 768MHz */
    while(readl(SCPLL_STAT) & 0x3);
    val = readl(SCPLL_CTLE);
    val &= ~(0x3F << 3);
    val |= (0x14 << 3); // Use 0x1A instead of 0x14 for 998MHz
    val &= ~(0x3 << 0);
    val |= (5 << 0);  // HOP method
    writel(val, SCPLL_CTLE);
    writel(0x00600007, SCPLL_CTL);
    thread_sleep(1);

    val = readl(A11S_CLK_SEL);
    val &= ~(0x3 << 1);
    val |= (1 << 1);
    writel(val, A11S_CLK_SEL);
}

