/* Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
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
 */

#include <stdint.h>
#include <kernel/thread.h>
#include <platform/iomap.h>
#include <reg.h>
#include <debug.h>
#include <mmc.h>

#define ACPU_806MHZ             42
#define ACPU_1024MHZ            53
#define ACPU_1200MHZ            125
#define ACPU_1400MHZ            73

/* Macros to select PLL2 with divide by 1 */
#define ACPU_SRC_SEL       3
#define ACPU_SRC_DIV       0

#define BIT(n)  (1 << (n))
#define VREG_CONFIG (BIT(7) | BIT(6))	/* Enable VREG, pull-down if disabled. */
#define VREG_DATA   (VREG_CONFIG | (VREF_SEL << 5))
#define VREF_SEL    1		/* 0: 0.625V (50mV step), 1: 0.3125V (25mV step). */
#define V_STEP      (25 * (2 - VREF_SEL))	/* Minimum voltage step size. */
#define MV(mv)          ((mv) / (!((mv) % V_STEP)))
/* mv = (750mV + (raw * 25mV)) * (2 - VREF_SEL) */
#define VDD_RAW(mv)     (((MV(mv) / V_STEP) - 30) | VREG_DATA)

/* enum for SDC CLK IDs */
enum {
	SDC1_CLK = 19,
	SDC1_PCLK = 20,
	SDC2_CLK = 21,
	SDC2_PCLK = 22,
	SDC3_CLK = 23,
	SDC3_PCLK = 24,
	SDC4_CLK = 25,
	SDC4_PCLK = 26
};

/* Zero'th entry is dummy */
static uint8_t sdc_clk[] = { 0, SDC1_CLK, SDC2_CLK, SDC3_CLK, SDC4_CLK };
static uint8_t sdc_pclk[] = { 0, SDC1_PCLK, SDC2_PCLK, SDC3_PCLK, SDC4_PCLK };

void spm_init(void)
{
	writel(0x05, MSM_SAW_BASE + 0x10);	/* MSM_SPM_REG_SAW_CFG */
	writel(0x18, MSM_SAW_BASE + 0x14);	/* MSM_SPM_REG_SAW_SPM_CTL */
	writel(0x00006666, MSM_SAW_BASE + 0x18);	/* MSM_SPM_REG_SAW_SPM_SLP_TMR_DLY */
	writel(0xFF000666, MSM_SAW_BASE + 0x1C);	/* MSM_SPM_REG_SAW_SPM_WAKE_TMR_DLY */

	writel(0x01, MSM_SAW_BASE + 0x24);	/* MSM_SPM_REG_SAW_SLP_CLK_EN */
	writel(0x03, MSM_SAW_BASE + 0x28);	/* MSM_SPM_REG_SAW_SLP_HSFS_PRECLMP_EN */
	writel(0x00, MSM_SAW_BASE + 0x2C);	/* MSM_SPM_REG_SAW_SLP_HSFS_POSTCLMP_EN */

	writel(0x01, MSM_SAW_BASE + 0x30);	/* MSM_SPM_REG_SAW_SLP_CLMP_EN */
	writel(0x00, MSM_SAW_BASE + 0x34);	/* MSM_SPM_REG_SAW_SLP_RST_EN */
	writel(0x00, MSM_SAW_BASE + 0x38);	/* MSM_SPM_REG_SAW_SPM_MPM_CFG */
}

/* Configures msmc2 voltage. vlevel is in mV */
void msmc2_config(unsigned vlevel)
{
	unsigned val;
	val = readl(MSM_SAW_BASE + 0x08);	/* MSM_SPM_REG_SAW_VCTL */
	val &= ~0xFF;
	val |= VDD_RAW(vlevel);
	writel(val, MSM_SAW_BASE + 0x08);	/* MSM_SPM_REG_SAW_VCTL */

	/* Wait for PMIC state to return to idle and for VDD to stabilize */
	while (((readl(MSM_SAW_BASE + 0x0C) >> 0x20) & 0x3) != 0) ;
	udelay(160);
}

void enable_pll(unsigned num)
{
	unsigned reg_val;
	reg_val = readl(PLL_ENA_REG);
	reg_val |= (1 << num);
	writel(reg_val, PLL_ENA_REG);
	/* Wait until PLL is enabled */
	while ((readl(PLL2_STATUS_BASE_REG) & (1 << 16)) == 0) ;
}

void acpu_clock_init(void)
{
	unsigned clk, reg_clksel, reg_clkctl, src_sel;
	/* Fixing msmc2 voltage */
	spm_init();

	clk = readl(PLL2_L_VAL_ADDR) & 0xFF;
	if (clk == ACPU_806MHZ)
		msmc2_config(1100);
	else if (clk == ACPU_1024MHZ || clk == ACPU_1200MHZ)
		msmc2_config(1200);
	else if (clk == ACPU_1400MHZ)
		msmc2_config(1250);

	/* Enable pll 2 */
	enable_pll(2);

	reg_clksel = readl(SCSS_CLK_SEL);

	/* CLK_SEL_SRC1NO */
	src_sel = reg_clksel & 1;

	/* Program clock source and divider. */
	reg_clkctl = readl(SCSS_CLK_CTL);
	reg_clkctl &= ~(0xFF << (8 * src_sel));
	reg_clkctl |= ACPU_SRC_SEL << (4 + 8 * src_sel);
	reg_clkctl |= ACPU_SRC_DIV << (0 + 8 * src_sel);
	writel(reg_clkctl, SCSS_CLK_CTL);

	/* Toggle clock source. */
	reg_clksel ^= 1;

	/* Program clock source selection. */
	writel(reg_clksel, SCSS_CLK_SEL);
}

void hsusb_clock_init(void)
{
	int val = 0;
	unsigned sh2_own_row2;
	unsigned sh2_own_row2_hsusb_mask = (1 << 11);

	sh2_own_row2 = readl(SH2_OWN_ROW2_BASE_REG);
	if (sh2_own_row2 & sh2_own_row2_hsusb_mask) {
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
	} else {
		/* USB local clock control not enabled; use proc comm */
		usb_clock_init();
	}
}

void adm_enable_clock(void)
{
	unsigned int val = 0;

	/* Enable ADM_CLK */
	val = 1 << 5;
	val = val | readl(SH2_GLBL_CLK_ENA_SC);
	writel(val, SH2_GLBL_CLK_ENA_SC);
}

void mdp_lcdc_clock_init(void)
{
	unsigned int val = 0;
	unsigned sh2_own_apps2;
	unsigned sh2_own_apps2_lcdc_mask = (1 << 3);

	sh2_own_apps2 = readl(SH2_OWN_APPS2_BASE_REG);
	if (sh2_own_apps2 & sh2_own_apps2_lcdc_mask) {
		/* MDP local clock control enabled */
		/* Select clock source and divider */
		val = 0x29;
		val = val | readl(SH2_MDP_NS_REG);
		writel(val, SH2_MDP_NS_REG);

		/* Enable MDP source clock(root) */
		val = 1 << 11;
		val = val | readl(SH2_MDP_NS_REG);
		writel(val, SH2_MDP_NS_REG);

		/* Enable graphics clock(branch) */
		val = 1 << 9;
		val = val | readl(SH2_MDP_NS_REG);
		writel(val, SH2_MDP_NS_REG);

		/* Enable MDP_P_CLK */
		val = 1 << 6;
		val = val | readl(SH2_GLBL_CLK_ENA_2_SC);
		writel(val, SH2_GLBL_CLK_ENA_2_SC);

		/* Enable AXI_MDP_CLK */
		val = 1 << 29;
		val = val | readl(SH2_GLBL_CLK_ENA_2_SC);
		writel(val, SH2_GLBL_CLK_ENA_2_SC);

		/* LCDC local clock control enabled */
		/* Set value in MD register */
		val = 0x1FFF9;
		writel(val, SH2_MDP_LCDC_MD_REG);

		/* Set MDP_LCDC_N_VAL in NS register */
		val = 0xFFFA << 16;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Set clock source */
		val = 1;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Set divider */
		val = 3 << 3;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Set MN counter mode */
		val = 2 << 5;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Enable MN counter */
		val = 1 << 8;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Enable mdp_lcdc_src(root) clock */
		val = 1 << 11;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Enable mdp_lcdc_pclk(branch) clock */
		val = 1 << 9;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);

		/* Enable mdp_lcdc_pad_pclk(branch) clock */
		val = 1 << 12;
		val = val | readl(SH2_MDP_LCDC_NS_REG);
		writel(val, SH2_MDP_LCDC_NS_REG);
	} else {
		/* MDP local clock control not enabled; use proc comm */
		mdp_clock_init(122880000);

		/* LCDC local clock control not enabled; use proc comm */
		lcdc_clock_init(27648000);
	}
}

void mddi_pmdh_clock_init(void)
{
	unsigned int val = 0;
	unsigned sh2_own_row1;
	unsigned sh2_own_row1_pmdh_mask = (1 << 19);

	sh2_own_row1 = readl(SH2_OWN_ROW1_BASE_REG);
	if (sh2_own_row1 & sh2_own_row1_pmdh_mask) {
		/* Select clock source and divider */
		val = 1;
		val |= (1 << 3);
		val = val | readl(SH2_PMDH_NS_REG);
		writel(val, SH2_PMDH_NS_REG);

		/* Enable PMDH_SRC (root) signal */
		val = 1 << 11;
		val = val | readl(SH2_PMDH_NS_REG);
		writel(val, SH2_PMDH_NS_REG);

		/* Enable PMDH_P_CLK */
		val = 1 << 4;
		val = val | readl(SH2_GLBL_CLK_ENA_2_SC);
		writel(val, SH2_GLBL_CLK_ENA_2_SC);
	} else {
		/* MDDI local clock control not enabled; use proc comm */
		mddi_clock_init(0, 480000000);
	}
}

void ce_clock_init(void)
{
	unsigned int val = 0;

	/* Enable CE_CLK */
	val = 1 << 6;
	val = val | readl(SH2_GLBL_CLK_ENA_SC);
	writel(val, SH2_GLBL_CLK_ENA_SC);
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	uint32_t reg = 0;

	if (mmc_clock_set_rate(sdc_clk[interface], freq) < 0) {
		dprintf(CRITICAL, "Failure setting clock rate for MCLK - "
			"clk_rate: %d\n!", freq);
		ASSERT(0);
	}

	/* enable clock */
	if (mmc_clock_enable_disable(sdc_clk[interface], MMC_CLK_ENABLE) < 0) {
		dprintf(CRITICAL, "Failure enabling MMC Clock!\n");
		ASSERT(0);
	}

	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Wait 1 ms to provide the free running SD CLK to the card. */
	mdelay(1);
}

/* Intialize MMC clock */
void clock_init_mmc(uint32_t interface)
{
	if (mmc_clock_enable_disable(sdc_pclk[interface], MMC_CLK_ENABLE) < 0) {
		dprintf(CRITICAL, "Failure enabling PCLK!\n");
		ASSERT(0);
	}
}
