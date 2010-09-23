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

/* 7x30 clock control and source registers */
#define SCSS_CLK_CTL            0xC0101004
#define SCSS_CLK_SEL            0xC0101008
#define MSM_CLK_CTL_SH2_BASE    0xABA01000
#define MSM_CLK_CTL_BASE        0xAB800000

#define REG_BASE(off)           (MSM_CLK_CTL_BASE + (off))
#define REG(off)                (MSM_CLK_CTL_SH2_BASE + (off))

#define PLL_ENA_REG             REG(0x0264)
#define PLL2_STATUS_BASE_REG    REG_BASE(0x0350)

#define ACPU_SRC_SEL_PLL2       3
#define ACPU_SRC_DIV_PLL2       0

void enable_pll(unsigned num)
{
    unsigned reg_val;
    reg_val = readl(PLL_ENA_REG);
    reg_val |= (1 << num);
    writel(reg_val, PLL_ENA_REG);
    /* Wait until PLL is enabled */
    while ((readl(PLL2_STATUS_BASE_REG) & (1 << 16)) == 0);
}

void acpu_clock_init(void)
{
    unsigned reg_clksel, reg_clkctl, src_sel;
    enable_pll(2);

    reg_clksel = readl(SCSS_CLK_SEL);

    /* CLK_SEL_SRC1NO */
    src_sel = reg_clksel & 1;

    /* Program clock source and divider. */
    reg_clkctl = readl(SCSS_CLK_CTL);
    reg_clkctl &= ~(0xFF << (8 * src_sel));
    reg_clkctl |= ACPU_SRC_SEL_PLL2 << (4 + 8 * src_sel);
    reg_clkctl |= ACPU_SRC_DIV_PLL2 << (0 + 8 * src_sel);
    writel(reg_clkctl, SCSS_CLK_CTL);

    /* Toggle clock source. */
    reg_clksel ^= 1;

    /* Program clock source selection. */
    writel(reg_clksel, SCSS_CLK_SEL);
}
