/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
#include <debug.h>
#include <reg.h>
#include <err.h>
#include <smem.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>

#define LPFR_LUT_SIZE 10

#define VCO_REF_CLOCK_RATE 19200000

#define FRAC_DIVIDER 10000

#define MMSS_DSI_PHY_PLL_SYS_CLK_CTRL                   0x0000
#define MMSS_DSI_PHY_PLL_PLL_VCOTAIL_EN                 0x0004
#define MMSS_DSI_PHY_PLL_CMN_MODE                       0x0008
#define MMSS_DSI_PHY_PLL_IE_TRIM                        0x000C
#define MMSS_DSI_PHY_PLL_IP_TRIM                        0x0010
#define MMSS_DSI_PHY_PLL_PLL_CNTRL              	0x0014
#define MMSS_DSI_PHY_PLL_PLL_PHSEL_CONTROL              0x0018
#define MMSS_DSI_PHY_PLL_IPTAT_TRIM_VCCA_TX_SEL         0x001C
#define MMSS_DSI_PHY_PLL_PLL_PHSEL_DC                   0x0020
#define MMSS_DSI_PHY_PLL_PLL_IP_SETI                    0x0024
#define MMSS_DSI_PHY_PLL_CORE_CLK_IN_SYNC_SEL           0x0028
#define MMSS_DSI_PHY_PLL_PLL_BKG_KVCO_CAL_EN    	0x002C

#define MMSS_DSI_PHY_PLL_BIAS_EN_CLKBUFLR_EN            0x0030
#define MMSS_DSI_PHY_PLL_PLL_CP_SETI                    0x0034
#define MMSS_DSI_PHY_PLL_PLL_IP_SETP                    0x0038
#define MMSS_DSI_PHY_PLL_PLL_CP_SETP                    0x003C
#define MMSS_DSI_PHY_PLL_ATB_SEL1                       0x0040
#define MMSS_DSI_PHY_PLL_ATB_SEL2                       0x0044
#define MMSS_DSI_PHY_PLL_SYSCLK_EN_SEL_TXBAND           0x0048
#define MMSS_DSI_PHY_PLL_RESETSM_CNTRL                  0x004C
#define MMSS_DSI_PHY_PLL_RESETSM_CNTRL2                 0x0050
#define MMSS_DSI_PHY_PLL_RESETSM_CNTRL3                 0x0054
#define MMSS_DSI_PHY_PLL_RESETSM_PLL_CAL_COUNT1         0x0058
#define MMSS_DSI_PHY_PLL_RESETSM_PLL_CAL_COUNT2         0x005C
#define MMSS_DSI_PHY_PLL_DIV_REF1                       0x0060
#define MMSS_DSI_PHY_PLL_DIV_REF2                       0x0064
#define MMSS_DSI_PHY_PLL_KVCO_COUNT1                    0x0068
#define MMSS_DSI_PHY_PLL_KVCO_COUNT2                    0x006C
#define MMSS_DSI_PHY_PLL_KVCO_CAL_CNTRL                 0x0070
#define MMSS_DSI_PHY_PLL_KVCO_CODE                      0x0074
#define MMSS_DSI_PHY_PLL_VREF_CFG1                      0x0078
#define MMSS_DSI_PHY_PLL_VREF_CFG2                      0x007C
#define MMSS_DSI_PHY_PLL_VREF_CFG3                      0x0080
#define MMSS_DSI_PHY_PLL_VREF_CFG4                      0x0084
#define MMSS_DSI_PHY_PLL_VREF_CFG5                      0x0088
#define MMSS_DSI_PHY_PLL_VREF_CFG6                      0x008C
#define MMSS_DSI_PHY_PLL_PLLLOCK_CMP1                   0x0090
#define MMSS_DSI_PHY_PLL_PLLLOCK_CMP2                   0x0094
#define MMSS_DSI_PHY_PLL_PLLLOCK_CMP3                   0x0098
#define MMSS_DSI_PHY_PLL_PLLLOCK_CMP_EN         	0x009C

#define MMSS_DSI_PHY_PLL_BGTC                           0x00A0
#define MMSS_DSI_PHY_PLL_PLL_TEST_UPDN                  0x00A4
#define MMSS_DSI_PHY_PLL_PLL_VCO_TUNE                   0x00A8
#define MMSS_DSI_PHY_PLL_DEC_START1                     0x00AC
#define MMSS_DSI_PHY_PLL_PLL_AMP_OS                     0x00B0
#define MMSS_DSI_PHY_PLL_SSC_EN_CENTER                  0x00B4
#define MMSS_DSI_PHY_PLL_SSC_ADJ_PER1                   0x00B8
#define MMSS_DSI_PHY_PLL_SSC_ADJ_PER2                   0x00BC
#define MMSS_DSI_PHY_PLL_SSC_PER1                       0x00C0
#define MMSS_DSI_PHY_PLL_SSC_PER2                       0x00C4
#define MMSS_DSI_PHY_PLL_SSC_STEP_SIZE1                 0x00C8
#define MMSS_DSI_PHY_PLL_SSC_STEP_SIZE2                 0x00CC
#define MMSS_DSI_PHY_PLL_RES_CODE_UP                    0x00D0
#define MMSS_DSI_PHY_PLL_RES_CODE_DN                    0x00D4
#define MMSS_DSI_PHY_PLL_RES_CODE_UP_OFFSET             0x00D8
#define MMSS_DSI_PHY_PLL_RES_CODE_DN_OFFSET             0x00DC
#define MMSS_DSI_PHY_PLL_RES_CODE_START_SEG1            0x00E0
#define MMSS_DSI_PHY_PLL_RES_CODE_START_SEG2            0x00E4
#define MMSS_DSI_PHY_PLL_RES_CODE_CAL_CSR               0x00E8
#define MMSS_DSI_PHY_PLL_RES_CODE                       0x00EC
#define MMSS_DSI_PHY_PLL_RES_TRIM_CONTROL               0x00F0
#define MMSS_DSI_PHY_PLL_RES_TRIM_CONTROL2              0x00F4
#define MMSS_DSI_PHY_PLL_RES_TRIM_EN_VCOCALDONE         0x00F8
#define MMSS_DSI_PHY_PLL_FAUX_EN                        0x00FC

#define MMSS_DSI_PHY_PLL_DIV_FRAC_START1                0x0100
#define MMSS_DSI_PHY_PLL_DIV_FRAC_START2                0x0104
#define MMSS_DSI_PHY_PLL_DIV_FRAC_START3                0x0108
#define MMSS_DSI_PHY_PLL_DEC_START2                     0x010C
#define MMSS_DSI_PHY_PLL_PLL_RXTXEPCLK_EN               0x0110
#define MMSS_DSI_PHY_PLL_PLL_CRCTRL                     0x0114
#define MMSS_DSI_PHY_PLL_LOW_POWER_RO_CONTROL           0x013C
#define MMSS_DSI_PHY_PLL_POST_DIVIDER_CONTROL           0x0140
#define MMSS_DSI_PHY_PLL_HR_OCLK2_DIVIDER               0x0144
#define MMSS_DSI_PHY_PLL_HR_OCLK3_DIVIDER               0x0148
#define MMSS_DSI_PHY_PLL_PLL_VCO_HIGH                   0x014C
#define MMSS_DSI_PHY_PLL_RESET_SM                       0x0150

uint32_t mdss_dsi_pll_20nm_lock_status(uint32_t pll_base)
{
	uint32_t cnt, status;

	udelay(1000);

	/* check pll lock first */
	for (cnt = 0; cnt < 5; cnt++) {
		status = readl(pll_base + MMSS_DSI_PHY_PLL_RESET_SM);
		dprintf(SPEW, "%s: pll_base=%x cnt=%d status=%x\n",
				__func__, pll_base, cnt, status);
		status &= 0x20; /* bit 5 */
		if (status)
			break;
		udelay(5000);
	}

	if (!status)
		goto pll_done;

	/* check pll ready */
	for (cnt = 0; cnt < 5; cnt++) {
		status = readl(pll_base + MMSS_DSI_PHY_PLL_RESET_SM);
		dprintf(SPEW, "%s: pll_base=%x cnt=%d status=%x\n",
				__func__, pll_base, cnt, status);
		status &= 0x40; /* bit 6 */
		if (status)
			break;
		udelay(5000);
	}

pll_done:
	return status;
}

uint32_t mdss_dsi_pll_20nm_sw_reset_st_machine(uint32_t pll_base)
{
	writel(0x64, pll_base + MMSS_DSI_PHY_PLL_RES_CODE_START_SEG1);
	writel(0x64, pll_base + MMSS_DSI_PHY_PLL_RES_CODE_START_SEG2);
	writel(0x15, pll_base + MMSS_DSI_PHY_PLL_RES_TRIM_CONTROL);

	writel(0x20, pll_base + MMSS_DSI_PHY_PLL_RESETSM_CNTRL);
	writel(0x07, pll_base + MMSS_DSI_PHY_PLL_RESETSM_CNTRL2);
	writel(0x02, pll_base + MMSS_DSI_PHY_PLL_RESETSM_CNTRL3);
	writel(0x03, pll_base + MMSS_DSI_PHY_PLL_RESETSM_CNTRL3);
}

static void pll_20nm_phy_kvco_config(uint32_t pll_base)
{

	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_DIV_REF1);
	writel(0x01, pll_base + MMSS_DSI_PHY_PLL_DIV_REF2);
	writel(0x8a, pll_base + MMSS_DSI_PHY_PLL_KVCO_COUNT1);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_KVCO_CAL_CNTRL);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_KVCO_CODE);
}

static void pll_20nm_phy_loop_bw_config(uint32_t pll_base)
{
	writel(0x03, pll_base + MMSS_DSI_PHY_PLL_PLL_IP_SETI);
	writel(0x3f, pll_base + MMSS_DSI_PHY_PLL_PLL_CP_SETI);
	writel(0x03, pll_base + MMSS_DSI_PHY_PLL_PLL_IP_SETP);
	writel(0x1f, pll_base + MMSS_DSI_PHY_PLL_PLL_CP_SETP);
	writel(0x77, pll_base + MMSS_DSI_PHY_PLL_PLL_CRCTRL);
}

static void pll_20nm_phy_config(uint32_t pll_base)
{
	writel(0x40, pll_base + MMSS_DSI_PHY_PLL_SYS_CLK_CTRL);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_PLL_VCOTAIL_EN);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_CMN_MODE);
	writel(0x0f, pll_base + MMSS_DSI_PHY_PLL_IE_TRIM);
	writel(0x0f, pll_base + MMSS_DSI_PHY_PLL_IP_TRIM);
	writel(0x08, pll_base + MMSS_DSI_PHY_PLL_PLL_PHSEL_CONTROL);
	writel(0x0e, pll_base + MMSS_DSI_PHY_PLL_IPTAT_TRIM_VCCA_TX_SEL);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_PLL_PHSEL_DC);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_CORE_CLK_IN_SYNC_SEL);
	writel(0x08, pll_base + MMSS_DSI_PHY_PLL_PLL_BKG_KVCO_CAL_EN);
	writel(0x3f, pll_base + MMSS_DSI_PHY_PLL_BIAS_EN_CLKBUFLR_EN);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_ATB_SEL1);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_ATB_SEL2);
	writel(0x4b, pll_base + MMSS_DSI_PHY_PLL_SYSCLK_EN_SEL_TXBAND);
	udelay(1000);

	pll_20nm_phy_kvco_config(pll_base);

	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_VREF_CFG1);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_VREF_CFG2);
	writel(0x10, pll_base + MMSS_DSI_PHY_PLL_VREF_CFG3);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_VREF_CFG4);
	writel(0x0f, pll_base + MMSS_DSI_PHY_PLL_BGTC);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_PLL_TEST_UPDN);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_PLL_VCO_TUNE);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_PLL_AMP_OS);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_SSC_EN_CENTER);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_RES_CODE_UP);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_RES_CODE_DN);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_RES_CODE_CAL_CSR);
	writel(0x00, pll_base + MMSS_DSI_PHY_PLL_RES_TRIM_EN_VCOCALDONE);
	writel(0x0c, pll_base + MMSS_DSI_PHY_PLL_FAUX_EN);
	writel(0x0f, pll_base + MMSS_DSI_PHY_PLL_PLL_RXTXEPCLK_EN);

	writel(0x0f, pll_base + MMSS_DSI_PHY_PLL_LOW_POWER_RO_CONTROL);
	udelay(1000);

	pll_20nm_phy_loop_bw_config(pll_base);
}

static void mdss_dsi_pll_20nm_disable(uint32_t pll_base)
{
	dprintf(SPEW, "Disabling DSI PHY PLL \n");
	writel(0x042, pll_base + MMSS_DSI_PHY_PLL_PLL_VCOTAIL_EN);
	writel(0x002, pll_base + MMSS_DSI_PHY_PLL_BIAS_EN_CLKBUFLR_EN);
	writel(0x002, pll_base + MMSS_DSI_PHY_PLL_RESETSM_CNTRL3);
	dmb();
}

int32_t mdss_dsi_auto_pll_20nm_config(uint32_t pll_base, uint32_t ctl_base,
				struct mdss_dsi_pll_config *pd)
{
	uint32_t data;

	mdss_dsi_phy_sw_reset(ctl_base);
	pll_20nm_phy_config(pll_base);

	/*
	 * For 20nm PHY, DSI PLL 1 drains some current in its reset state.
	 * Need to turn off the DSI1 PLL explicitly.
	 */
	if (ctl_base == MIPI_DSI0_BASE) {
		dprintf(SPEW, "Calling disable function for PHY PLL 1 \n");
		mdss_dsi_pll_20nm_disable(DSI1_PLL_BASE);
	}

	/* set up divider */
	data = readl(pll_base + MMSS_DSI_PHY_PLL_POST_DIVIDER_CONTROL);
	data |= 0x080; /* bit 7 */
	data |= (pd->lp_div_mux << 5);
	data |= pd->ndiv;

	writel(data, pll_base + MMSS_DSI_PHY_PLL_POST_DIVIDER_CONTROL);

	writel(pd->hr_oclk2, pll_base + MMSS_DSI_PHY_PLL_HR_OCLK2_DIVIDER);
	writel(pd->hr_oclk3, pll_base + MMSS_DSI_PHY_PLL_HR_OCLK3_DIVIDER);

	writel(((pd->frac_start & 0x7f) | 0x80),
				pll_base + MMSS_DSI_PHY_PLL_DIV_FRAC_START1);
	writel((((pd->frac_start >> 7) & 0x7f) | 0x80),
				pll_base + MMSS_DSI_PHY_PLL_DIV_FRAC_START2);
	writel((((pd->frac_start >> 14) & 0x3f) | 0x40),
				pll_base + MMSS_DSI_PHY_PLL_DIV_FRAC_START3);

	writel(((pd->dec_start & 0x7f) | 0x80),
				pll_base + MMSS_DSI_PHY_PLL_DEC_START1);
	writel((((pd->dec_start & 0x80) >> 7) | 0x02),
				pll_base + MMSS_DSI_PHY_PLL_DEC_START2);

	writel((pd->lock_comp & 0xff),
				pll_base + MMSS_DSI_PHY_PLL_PLLLOCK_CMP1);

	writel(((pd->lock_comp >> 8) & 0xff),
				pll_base + MMSS_DSI_PHY_PLL_PLLLOCK_CMP2);

	writel(((pd->lock_comp >> 16) & 0xff),
				pll_base + MMSS_DSI_PHY_PLL_PLLLOCK_CMP3);

	/*
	 * Make sure that PLL vco configuration is complete
	 * before controlling the state machine.
	 */
	udelay(1000);
	dmb();
}
