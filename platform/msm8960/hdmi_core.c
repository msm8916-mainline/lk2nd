/* Copyright (c) 2009-2013, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
#include <dev/pm8921.h>
#include <platform/timer.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/iomap.h>

extern void hdmi_app_clk_init(int);
extern int hdmi_msm_turn_on();

/* HDMI PLL macros */
#define HDMI_PHY_PLL_REFCLK_CFG          (MSM_HDMI_BASE + 0x00000500)
#define HDMI_PHY_PLL_CHRG_PUMP_CFG       (MSM_HDMI_BASE + 0x00000504)
#define HDMI_PHY_PLL_LOOP_FLT_CFG0       (MSM_HDMI_BASE + 0x00000508)
#define HDMI_PHY_PLL_LOOP_FLT_CFG1       (MSM_HDMI_BASE + 0x0000050c)
#define HDMI_PHY_PLL_IDAC_ADJ_CFG        (MSM_HDMI_BASE + 0x00000510)
#define HDMI_PHY_PLL_I_VI_KVCO_CFG       (MSM_HDMI_BASE + 0x00000514)
#define HDMI_PHY_PLL_PWRDN_B             (MSM_HDMI_BASE + 0x00000518)
#define HDMI_PHY_PLL_SDM_CFG0            (MSM_HDMI_BASE + 0x0000051c)
#define HDMI_PHY_PLL_SDM_CFG1            (MSM_HDMI_BASE + 0x00000520)
#define HDMI_PHY_PLL_SDM_CFG2            (MSM_HDMI_BASE + 0x00000524)
#define HDMI_PHY_PLL_SDM_CFG3            (MSM_HDMI_BASE + 0x00000528)
#define HDMI_PHY_PLL_SDM_CFG4            (MSM_HDMI_BASE + 0x0000052c)
#define HDMI_PHY_PLL_SSC_CFG0            (MSM_HDMI_BASE + 0x00000530)
#define HDMI_PHY_PLL_SSC_CFG1            (MSM_HDMI_BASE + 0x00000534)
#define HDMI_PHY_PLL_SSC_CFG2            (MSM_HDMI_BASE + 0x00000538)
#define HDMI_PHY_PLL_SSC_CFG3            (MSM_HDMI_BASE + 0x0000053c)
#define HDMI_PHY_PLL_LOCKDET_CFG0        (MSM_HDMI_BASE + 0x00000540)
#define HDMI_PHY_PLL_LOCKDET_CFG1        (MSM_HDMI_BASE + 0x00000544)
#define HDMI_PHY_PLL_LOCKDET_CFG2        (MSM_HDMI_BASE + 0x00000548)
#define HDMI_PHY_PLL_VCOCAL_CFG0         (MSM_HDMI_BASE + 0x0000054c)
#define HDMI_PHY_PLL_VCOCAL_CFG1         (MSM_HDMI_BASE + 0x00000550)
#define HDMI_PHY_PLL_VCOCAL_CFG2         (MSM_HDMI_BASE + 0x00000554)
#define HDMI_PHY_PLL_VCOCAL_CFG3         (MSM_HDMI_BASE + 0x00000558)
#define HDMI_PHY_PLL_VCOCAL_CFG4         (MSM_HDMI_BASE + 0x0000055c)
#define HDMI_PHY_PLL_VCOCAL_CFG5         (MSM_HDMI_BASE + 0x00000560)
#define HDMI_PHY_PLL_VCOCAL_CFG6         (MSM_HDMI_BASE + 0x00000564)
#define HDMI_PHY_PLL_VCOCAL_CFG7         (MSM_HDMI_BASE + 0x00000568)
#define HDMI_PHY_PLL_DEBUG_SEL           (MSM_HDMI_BASE + 0x0000056c)
#define HDMI_PHY_PLL_PWRDN_B             (MSM_HDMI_BASE + 0x00000518)
#define HDMI_PHY_PLL_STATUS0             (MSM_HDMI_BASE + 0x00000598)

/* HDMI PHY/PLL bit field macros */
#define SW_RESET BIT(2)
#define SW_RESET_PLL BIT(0)
#define PWRDN_B BIT(7)

#define PLL_PWRDN_B BIT(3)
#define PD_PLL BIT(1)

static unsigned hdmi_pll_on;

void hdmi_msm_init_phy()
{
	dprintf(INFO, "phy init\n");
	uint32_t offset;

	writel(0x1B, HDMI_PHY_REG_0);
	writel(0xf2, HDMI_PHY_REG_1);

	offset = HDMI_PHY_REG_4;
	while (offset <= HDMI_PHY_REG_11) {
		writel(0x0, offset);
		offset += 0x4;
	}

	writel(0x20, HDMI_PHY_REG_3);
}

static void hdmi_gpio_config()
{
	writel(0x07, GPIO_CONFIG_ADDR(70));
	writel(0x07, GPIO_CONFIG_ADDR(71));
	writel(0x05, GPIO_CONFIG_ADDR(72));
}

void hdmi_msm_reset_core()
{
	uint32_t reg_val = 0;

	// Disable clocks
	hdmi_app_clk_init(0);
	udelay(5);
	// Enable clocks
	hdmi_app_clk_init(1);

	reg_val = readl(SW_RESET_CORE_REG);
	reg_val |= BIT(11);
	writel(reg_val, SW_RESET_CORE_REG);
	udelay(5);
	reg_val = readl(SW_RESET_CORE_REG);
	reg_val &= ~(BIT(11));
	writel(reg_val, SW_RESET_CORE_REG);
	udelay(5);
}

void hdmi_phy_reset(void)
{
	uint32_t phy_reset_polarity = 0x0;
	uint32_t pll_reset_polarity = 0x0;

	uint32_t val = readl(HDMI_PHY_CTRL);

	phy_reset_polarity = val >> 3 & 0x1;
	pll_reset_polarity = val >> 1 & 0x1;

	if (phy_reset_polarity == 0)
		writel(val | SW_RESET, HDMI_PHY_CTRL);
	else
		writel(val & (~SW_RESET), HDMI_PHY_CTRL);

	if (pll_reset_polarity == 0)
		writel(val | SW_RESET_PLL, HDMI_PHY_CTRL);
	else
		writel(val & (~SW_RESET_PLL), HDMI_PHY_CTRL);

	udelay(10);

	if (phy_reset_polarity == 0)
		writel(val & (~SW_RESET), HDMI_PHY_CTRL);
	else
		writel(val | SW_RESET, HDMI_PHY_CTRL);

	if (pll_reset_polarity == 0)
		writel(val & (~SW_RESET_PLL), HDMI_PHY_CTRL);
	else
		writel(val | SW_RESET_PLL, HDMI_PHY_CTRL);
}

/*
 * This is the start function which initializes clocks , gpios for hdmi
 * & powers on the HDMI core
 */
void hdmi_power_init()
{
	pm8921_low_voltage_switch_enable(lvs_7);
	apq8064_ext_3p3V_enable();
	pm8921_HDMI_Switch();
	hdmi_gpio_config();
	hdmi_phy_reset();
	hdmi_msm_set_mode(1);
}

void hdmi_pll_disable(void)
{
	uint32_t val;
	uint32_t ahb_en_reg, ahb_enabled;

	ahb_en_reg = readl(AHB_EN_REG);
	ahb_enabled = ahb_en_reg & BIT(4);
	if (!ahb_enabled) {
		writel(ahb_en_reg | BIT(4), AHB_EN_REG);
		udelay(10);
	}

	val = readl(HDMI_PHY_REG_12);
	val &= (~PWRDN_B);
	writel(val, HDMI_PHY_REG_12);

	val = readl(HDMI_PHY_PLL_PWRDN_B);
	val |= PD_PLL;
	val &= (~PLL_PWRDN_B);
	writel(val, HDMI_PHY_PLL_PWRDN_B);
	/* Make sure HDMI PHY/PLL are powered down */
	udelay(10);

	if (!ahb_enabled)
		writel(ahb_en_reg & ~BIT(4), AHB_EN_REG);
	hdmi_pll_on = 0;
}

void hdmi_pll_enable(void)
{
	uint32_t val;
	uint32_t ahb_en_reg, ahb_enabled;
	uint32_t timeout_count;
	int pll_lock_retry = 10;

	ahb_en_reg = readl(AHB_EN_REG);
	ahb_enabled = ahb_en_reg & BIT(4);
	if (!ahb_enabled) {
		dprintf(INFO, "ahb not enabled\n");
		writel(ahb_en_reg | BIT(4), AHB_EN_REG);
		/* Make sure iface clock is enabled before register access */
		udelay(10);
	}

	/* Assert PLL S/W reset */
	writel(0x8D, HDMI_PHY_PLL_LOCKDET_CFG2);
	writel(0x10, HDMI_PHY_PLL_LOCKDET_CFG0);
	writel(0x1A, HDMI_PHY_PLL_LOCKDET_CFG1);
	/* Wait for a short time before de-asserting
	 * to allow the hardware to complete its job.
	 * This much of delay should be fine for hardware
	 * to assert and de-assert.
	 */
	udelay(10);
	/* De-assert PLL S/W reset */
	writel(0x0D, HDMI_PHY_PLL_LOCKDET_CFG2);

	val = readl(HDMI_PHY_REG_12);
	val |= BIT(5);
	/* Assert PHY S/W reset */
	writel(val, HDMI_PHY_REG_12);
	val &= ~BIT(5);
	/* Wait for a short time before de-asserting
	   to allow the hardware to complete its job.
	   This much of delay should be fine for hardware
	   to assert and de-assert. */
	udelay(10);
	/* De-assert PHY S/W reset */
	writel(val, HDMI_PHY_REG_12);
	writel(0x3f, HDMI_PHY_REG_2);

	val = readl(HDMI_PHY_REG_12);
	val |= PWRDN_B;
	writel(val, HDMI_PHY_REG_12);
	/* Wait 10 us for enabling global power for PHY */
	udelay(10);

	val = readl(HDMI_PHY_PLL_PWRDN_B);
	val |= PLL_PWRDN_B;
	val &= ~PD_PLL;
	writel(val, HDMI_PHY_PLL_PWRDN_B);
	writel(0x80, HDMI_PHY_REG_2);

	timeout_count = 1000;
	while (!(readl(HDMI_PHY_PLL_STATUS0) & BIT(0)) &&
			timeout_count && pll_lock_retry) {
		if (--timeout_count == 0) {
			dprintf(INFO, "PLL not locked, retry\n");
			/*
			 * PLL has still not locked.
			 * Do a software reset and try again
			 * Assert PLL S/W reset first
			 */
			writel(0x8D, HDMI_PHY_PLL_LOCKDET_CFG2);

			/* Wait for a short time before de-asserting
			 * to allow the hardware to complete its job.
			 * This much of delay should be fine for hardware
			 * to assert and de-assert.
			 */
			udelay(10);
			writel(0x0D, HDMI_PHY_PLL_LOCKDET_CFG2);

			/*
			 * Wait for a short duration for the PLL calibration
			 * before checking if the PLL gets locked
			 */
			udelay(350);

			timeout_count = 1000;
			pll_lock_retry--;
		}
	}

	if (!ahb_enabled) {
		writel(ahb_en_reg & ~BIT(4), AHB_EN_REG);
		udelay(10);
	}

	if (!pll_lock_retry) {
		dprintf(INFO, "%s: HDMI PLL not locked\n", __func__);
		hdmi_pll_disable();
	}

	hdmi_pll_on = 1;
}


int hdmi_dtv_on()
{
	uint32_t ahb_en_reg = readl(AHB_EN_REG);
	uint32_t ahb_enabled = ahb_en_reg & BIT(4);
	uint32_t val, pll_mode, ns_val, pll_config;

	if (!ahb_enabled) {
		dprintf(INFO, "ahb not enabled, turning on\n");
		writel(ahb_en_reg | BIT(4), AHB_EN_REG);
		/* Make sure iface clock is enabled before register access */
		udelay(10);
	}

	if (hdmi_pll_on)
		hdmi_pll_disable();

	/* 1080p60/1080p50 case */
	writel(0x2, HDMI_PHY_PLL_REFCLK_CFG);
	writel(0x2, HDMI_PHY_PLL_CHRG_PUMP_CFG);
	writel(0x01, HDMI_PHY_PLL_LOOP_FLT_CFG0);
	writel(0x33, HDMI_PHY_PLL_LOOP_FLT_CFG1);
	writel(0x2C, HDMI_PHY_PLL_IDAC_ADJ_CFG);
	writel(0x6, HDMI_PHY_PLL_I_VI_KVCO_CFG);
	writel(0xA, HDMI_PHY_PLL_PWRDN_B);
	writel(0x76, HDMI_PHY_PLL_SDM_CFG0);
	writel(0x01, HDMI_PHY_PLL_SDM_CFG1);
	writel(0x4C, HDMI_PHY_PLL_SDM_CFG2);
	writel(0xC0, HDMI_PHY_PLL_SDM_CFG3);
	writel(0x00, HDMI_PHY_PLL_SDM_CFG4);
	writel(0x9A, HDMI_PHY_PLL_SSC_CFG0);
	writel(0x00, HDMI_PHY_PLL_SSC_CFG1);
	writel(0x00, HDMI_PHY_PLL_SSC_CFG2);
	writel(0x00, HDMI_PHY_PLL_SSC_CFG3);
	writel(0x10, HDMI_PHY_PLL_LOCKDET_CFG0);
	writel(0x1A, HDMI_PHY_PLL_LOCKDET_CFG1);
	writel(0x0D, HDMI_PHY_PLL_LOCKDET_CFG2);
	writel(0xe6, HDMI_PHY_PLL_VCOCAL_CFG0);
	writel(0x02, HDMI_PHY_PLL_VCOCAL_CFG1);
	writel(0x3B, HDMI_PHY_PLL_VCOCAL_CFG2);
	writel(0x00, HDMI_PHY_PLL_VCOCAL_CFG3);
	writel(0x86, HDMI_PHY_PLL_VCOCAL_CFG4);
	writel(0x00, HDMI_PHY_PLL_VCOCAL_CFG5);
	writel(0x33, HDMI_PHY_PLL_VCOCAL_CFG6);
	writel(0x00, HDMI_PHY_PLL_VCOCAL_CFG7);

	udelay(10);

	hdmi_pll_enable();

	if (!ahb_enabled)
		writel(ahb_en_reg & ~BIT(4), AHB_EN_REG);

	// set M N D
	ns_val = readl(TV_NS_REG);
	ns_val |= BIT(7);
	writel(ns_val, TV_NS_REG);

	writel(0x00, TV_MD_REG);

	val = readl(TV_CC_REG);
	val &= ~(BM(7, 6));
	val |= CC(6, 0);
	writel(val, TV_CC_REG);

	ns_val &= ~BIT(7);
	writel(ns_val, TV_NS_REG);

	// confiure hdmi_ref clk to run @ 148.5 MHz
	val = readl(MISC_CC2_REG);
	val |= BIT(11);
	writel(val, MISC_CC2_REG);

	// Enable TV src clk
	writel(0x03, TV_NS_REG);

	// Enable hdmi clk
	val = readl(TV_CC_REG);
	val |= BIT(12);
	writel(val, TV_CC_REG);

	// De-Assert hdmi clk
	val = readl(SW_RESET_CORE_REG);
	val |= BIT(1);
	writel(val, SW_RESET_CORE_REG);
	udelay(10);
	val = readl(SW_RESET_CORE_REG);
	val &= ~(BIT(1));
	writel(val, SW_RESET_CORE_REG);
	udelay(10);

	// Root en of tv src clk
	val = readl(TV_CC_REG);
	val |= BIT(2);
	writel(val, TV_CC_REG);

	// enable mdp dtv clk
	val = readl(TV_CC_REG);
	val |= BIT(0);
	writel(val, TV_CC_REG);
	udelay(10);

	return 0;
}

