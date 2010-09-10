/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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
#define SCPLL_CAL  0xA8800008
#define SCPLL_CTLE 0xA8800024
#define SCPLL_STAT 0xA8800010

void acpu_clock_init(void)
{
	unsigned val;

    /* Go to standby */
    writel(0x2, SCPLL_CTL);
    thread_sleep(100);

    /* Calibrate for 384-1497 MHz */
    writel(0x270A0000, SCPLL_CAL);
    writel(0x4, SCPLL_CTL);
    thread_sleep(10);
    while(readl(SCPLL_STAT) & 0x2);

    /* Shot-switch directly to 768MHz */
    writel(0x001400A4, SCPLL_CTLE);
    writel(0x7, SCPLL_CTL);
    thread_sleep(10);
    while(readl(SCPLL_STAT) & 0x3);

    val = readl(A11S_CLK_SEL);
    val &= ~(0x3 << 1);
    val |= (1 << 1);
    writel(val, A11S_CLK_SEL);
}

