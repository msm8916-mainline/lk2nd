/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <hdmi.h>
#include <platform/timer.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/scm-io.h>

extern void hdmi_app_clk_init(int);
extern void hdmi_msm_turn_on();

void hdmi_msm_init_phy()
{
	dprintf(SPEW, "PHY INIT\n");
	uint32_t offset = 0;
	uint32_t len = 0;

	writel(0x0C, HDMI_PHY_REG_0);
	writel(0x54, HDMI_PHY_REG_1);
	writel(0x7F, HDMI_PHY_REG_2);
	writel(0x3F, HDMI_PHY_REG_2);
	writel(0x1F, HDMI_PHY_REG_2);

	writel(0x01, HDMI_PHY_REG_3);
	writel(0x00, HDMI_PHY_REG_9);
	writel(0x03, HDMI_PHY_REG_12);
	writel(0x01, HDMI_PHY_REG_2);
	writel(0x81, HDMI_PHY_REG_2);

	offset = (HDMI_PHY_REG_4 - MSM_HDMI_BASE);
	len = (HDMI_PHY_REG_11 - MSM_HDMI_BASE);
	while (offset <= len) {
		writel(0x0, MSM_HDMI_BASE + offset);
		offset += 4;
	}
	writel(0x13, HDMI_PHY_REG_12);
}

void hdmi_frame_ctrl_reg()
{
	uint32_t hdmi_frame_ctrl;

	hdmi_frame_ctrl = ((0 << 31) & 0x80000000);
	hdmi_frame_ctrl |= ((0 << 29) & 0x20000000);
	hdmi_frame_ctrl |= ((0 << 28) & 0x10000000);
	hdmi_frame_ctrl |= (1 << 12);
	writel(hdmi_frame_ctrl, HDMI_FRAME_CTRL);
}

static void hdmi_gpio_config()
{
	uint32_t func;
	uint32_t pull;
	uint32_t drv;
	uint32_t enable = 0;
	uint32_t dir;

	func = 1;
	pull = GPIO_NO_PULL;
	drv = GPIO_16MA;
	dir = 1;
	gpio_tlmm_config(170, func, dir, pull, drv, enable);

	gpio_tlmm_config(171, func, dir, pull, drv, enable);

	func = 1;
	pull = GPIO_PULL_DOWN;
	drv = GPIO_16MA;
	gpio_tlmm_config(172, func, dir, pull, drv, enable);
}

/*
 * This is the start function which initializes clocks , gpios for hdmi
 * & powers on the HDMI core
 */
void hdmi_display_init()
{
	// Enable HDMI clocks
	hdmi_app_clk_init(1);
	// Enable pm8058
	pm8058_ldo_set_voltage();
	pm8058_vreg_enable();
	// configure HDMI Gpio
	hdmi_gpio_config();
	// Enable pm8091
	pm8901_mpp_enable();
	pm8901_vs_enable();
	// Power on HDMI
	hdmi_msm_turn_on();
}

void dtv_on()
{
	uint32_t val, pll_mode, ns_val, pll_config;

	// Configure PLL2 for tv src clk
	pll_mode |= BIT(1);
	secure_writel(pll_mode, MM_PLL2_MODE_REG);
	udelay(10);

	pll_mode = secure_readl(MM_PLL2_MODE_REG);
	pll_mode &= ~BIT(0);
	secure_writel(pll_mode, MM_PLL2_MODE_REG);
	pll_mode &= ~BIT(2);
	secure_writel(pll_mode, MM_PLL2_MODE_REG);

	secure_writel(0x2C, MM_PLL2_L_VAL_REG);
	secure_writel(0x0, MM_PLL2_M_VAL_REG);
	secure_writel(0x0, MM_PLL2_N_VAL_REG);
	udelay(10);

	val = 0xA6248F;
	secure_writel(val, MM_PLL2_CONFIG_REG);

	// set M N D
	ns_val = secure_readl(TV_NS_REG);
	ns_val |= BIT(7);
	secure_writel(ns_val, TV_NS_REG);

	secure_writel(0xff, TV_MD_REG);

	val = secure_readl(TV_CC_REG);
	val &= ~(BM(7, 6));
	val |= CC(6, 0);
	secure_writel(val, TV_CC_REG);

	ns_val &= ~BIT(7);
	secure_writel(ns_val, TV_NS_REG);

	// confiure hdmi_ref clk to run @ 148.5 MHz
	val = secure_readl(MISC_CC2_REG);
	val &= ~(BIT(28) | BM(21, 18));
	ns_val = NS_MM(23, 16, 0, 0, 15, 14, 2, 2, 0, 3);
	val |= (BIT(28) | BVAL(21, 18, (ns_val >> 14) & 0x3));
	secure_writel(val, MISC_CC2_REG);

	pll_mode |= BIT(2);
	secure_writel(pll_mode, MM_PLL2_MODE_REG);

	pll_mode |= BIT(0);
	secure_writel(pll_mode, MM_PLL2_MODE_REG);
	udelay(50);

	// Enable TV src clk
	val = secure_readl(TV_NS_REG);
	val &= ~(BM(23, 16) | BM(15, 14) | BM(2, 0));
	ns_val = NS_MM(23, 16, 0, 0, 15, 14, 2, 2, 0, 3);
	val |= (ns_val & (BM(23, 16) | BM(15, 14) | BM(2, 0)));
	secure_writel(val, TV_NS_REG);

	// Enable hdmi clk
	val = secure_readl(TV_CC_REG);
	val |= BIT(12);
	secure_writel(val, TV_CC_REG);

	// Root en of tv src clk
	val = secure_readl(TV_CC_REG);
	val |= BIT(2);
	secure_writel(val, TV_CC_REG);

	// De-Assert hdmi clk
	val = secure_readl(SW_RESET_CORE_REG);
	val |= BIT(1);
	secure_writel(val, SW_RESET_CORE_REG);
	udelay(10);
	val = secure_readl(SW_RESET_CORE_REG);
	val &= ~(BIT(1));
	secure_writel(val, SW_RESET_CORE_REG);
	udelay(10);

	// enable mdp dtv clk
	val = secure_readl(TV_CC_REG);
	val |= BIT(0);
	secure_writel(val, TV_CC_REG);
	udelay(10);
}
