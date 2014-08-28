/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <debug.h>
#include <err.h>
#include <reg.h>
#include <smem.h>
#include <bits.h>
#include <msm_panel.h>
#include <platform/timer.h>
#include <platform/iomap.h>

#define HDMI_PHY_BASE  0xFD922500
#define HDMI_PLL_BASE  0xFD922700

/* hdmi phy registers */
#define HDMI_PHY_ANA_CFG0			(0x0000)
#define HDMI_PHY_ANA_CFG1			(0x0004)
#define HDMI_PHY_ANA_CFG2			(0x0008)
#define HDMI_PHY_ANA_CFG3			(0x000C)
#define HDMI_PHY_PD_CTRL0			(0x0010)
#define HDMI_PHY_PD_CTRL1			(0x0014)
#define HDMI_PHY_GLB_CFG			(0x0018)
#define HDMI_PHY_DCC_CFG0			(0x001C)
#define HDMI_PHY_DCC_CFG1			(0x0020)
#define HDMI_PHY_TXCAL_CFG0			(0x0024)
#define HDMI_PHY_TXCAL_CFG1			(0x0028)
#define HDMI_PHY_TXCAL_CFG2			(0x002C)
#define HDMI_PHY_TXCAL_CFG3			(0x0030)
#define HDMI_PHY_BIST_CFG0			(0x0034)
#define HDMI_PHY_BIST_CFG1			(0x0038)
#define HDMI_PHY_BIST_PATN0			(0x003C)
#define HDMI_PHY_BIST_PATN1			(0x0040)
#define HDMI_PHY_BIST_PATN2			(0x0044)
#define HDMI_PHY_BIST_PATN3			(0x0048)
#define HDMI_PHY_STATUS				(0x005C)

/* hdmi phy unified pll registers */
#define HDMI_UNI_PLL_REFCLK_CFG			(0x0000)
#define HDMI_UNI_PLL_POSTDIV1_CFG		(0x0004)
#define HDMI_UNI_PLL_CHFPUMP_CFG		(0x0008)
#define HDMI_UNI_PLL_VCOLPF_CFG			(0x000C)
#define HDMI_UNI_PLL_VREG_CFG			(0x0010)
#define HDMI_UNI_PLL_PWRGEN_CFG			(0x0014)
#define HDMI_UNI_PLL_GLB_CFG			(0x0020)
#define HDMI_UNI_PLL_POSTDIV2_CFG		(0x0024)
#define HDMI_UNI_PLL_POSTDIV3_CFG		(0x0028)
#define HDMI_UNI_PLL_LPFR_CFG			(0x002C)
#define HDMI_UNI_PLL_LPFC1_CFG			(0x0030)
#define HDMI_UNI_PLL_LPFC2_CFG			(0x0034)
#define HDMI_UNI_PLL_SDM_CFG0			(0x0038)
#define HDMI_UNI_PLL_SDM_CFG1			(0x003C)
#define HDMI_UNI_PLL_SDM_CFG2			(0x0040)
#define HDMI_UNI_PLL_SDM_CFG3			(0x0044)
#define HDMI_UNI_PLL_SDM_CFG4			(0x0048)
#define HDMI_UNI_PLL_SSC_CFG0			(0x004C)
#define HDMI_UNI_PLL_SSC_CFG1			(0x0050)
#define HDMI_UNI_PLL_SSC_CFG2			(0x0054)
#define HDMI_UNI_PLL_SSC_CFG3			(0x0058)
#define HDMI_UNI_PLL_LKDET_CFG0			(0x005C)
#define HDMI_UNI_PLL_LKDET_CFG1			(0x0060)
#define HDMI_UNI_PLL_LKDET_CFG2			(0x0064)
#define HDMI_UNI_PLL_CAL_CFG0			(0x006C)
#define HDMI_UNI_PLL_CAL_CFG1			(0x0070)
#define HDMI_UNI_PLL_CAL_CFG2			(0x0074)
#define HDMI_UNI_PLL_CAL_CFG3			(0x0078)
#define HDMI_UNI_PLL_CAL_CFG4			(0x007C)
#define HDMI_UNI_PLL_CAL_CFG5			(0x0080)
#define HDMI_UNI_PLL_CAL_CFG6			(0x0084)
#define HDMI_UNI_PLL_CAL_CFG7			(0x0088)
#define HDMI_UNI_PLL_CAL_CFG8			(0x008C)
#define HDMI_UNI_PLL_CAL_CFG9			(0x0090)
#define HDMI_UNI_PLL_CAL_CFG10			(0x0094)
#define HDMI_UNI_PLL_CAL_CFG11			(0x0098)
#define HDMI_UNI_PLL_STATUS			(0x00C0)

#define SW_RESET BIT(2)
#define SW_RESET_PLL BIT(0)

void hdmi_phy_reset(void)
{
	uint32_t phy_reset_polarity = 0x0;
	uint32_t pll_reset_polarity = 0x0;
	uint32_t val;

	val = readl(HDMI_PHY_CTRL);

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

	if (phy_reset_polarity == 0)
		writel(val & (~SW_RESET), HDMI_PHY_CTRL);
	else
		writel(val | SW_RESET, HDMI_PHY_CTRL);

	if (pll_reset_polarity == 0)
		writel(val & (~SW_RESET_PLL), HDMI_PHY_CTRL);
	else
		writel(val | SW_RESET_PLL, HDMI_PHY_CTRL);
}

void hdmi_phy_init(void)
{
	writel(0x1B, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG0);
	writel(0xF2, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG1);
	writel(0x0,  HDMI_PHY_BASE + HDMI_PHY_BIST_CFG0);
	writel(0x0,  HDMI_PHY_BASE + HDMI_PHY_BIST_PATN0);
	writel(0x0,  HDMI_PHY_BASE + HDMI_PHY_BIST_PATN1);
	writel(0x0,  HDMI_PHY_BASE + HDMI_PHY_BIST_PATN2);
	writel(0x0,  HDMI_PHY_BASE + HDMI_PHY_BIST_PATN3);

	writel(0x20, HDMI_PHY_BASE + HDMI_PHY_PD_CTRL1);
}

void hdmi_phy_powerdown(void)
{
	writel(0x7F, HDMI_PHY_BASE + HDMI_PHY_PD_CTRL0);
}

static uint32_t hdmi_poll_status(uint32_t addr)
{
	uint32_t count;

	for (count = 20; count > 0; count--) {
		if (readl(addr) & 0x1) {
			return NO_ERROR;
		}
		udelay(100);
	}
	return ERR_TIMED_OUT;
}

void hdmi_vco_disable(void)
{
        writel(0x0, HDMI_PLL_BASE + HDMI_UNI_PLL_GLB_CFG);
        udelay(5);
        writel(0x0, HDMI_PHY_BASE + HDMI_PHY_GLB_CFG);
}

int hdmi_vco_enable(void)
{
        /* Global Enable */
        writel(0x81, HDMI_PHY_BASE + HDMI_PHY_GLB_CFG);
        /* Power up power gen */
        writel(0x00, HDMI_PHY_BASE + HDMI_PHY_PD_CTRL0);
        udelay(350);

        /* PLL Power-Up */
        writel(0x01, HDMI_PLL_BASE + HDMI_UNI_PLL_GLB_CFG);
        udelay(5);

        /* Power up PLL LDO */
        writel(0x03, HDMI_PLL_BASE + HDMI_UNI_PLL_GLB_CFG);
        udelay(350);

        /* PLL Power-Up */
        writel(0x0F, HDMI_PLL_BASE + HDMI_UNI_PLL_GLB_CFG);
        udelay(350);

        /* poll for PLL ready status */
        if (hdmi_poll_status(HDMI_PLL_BASE + HDMI_UNI_PLL_STATUS)) {
                dprintf(CRITICAL, "%s: hdmi phy pll failed to Lock\n",
                       __func__);
                hdmi_vco_disable();
                return ERROR;
        }

        udelay(350);
        /* poll for PHY ready status */
        if (hdmi_poll_status(HDMI_PHY_BASE + HDMI_PHY_STATUS)) {
                dprintf(CRITICAL, "%s: hdmi phy failed to Lock\n",
                       __func__);
                hdmi_vco_disable();
                return ERROR;
        }

        return NO_ERROR;
}

uint32_t hdmi_pll_config(void)
{
	writel(0x81, HDMI_PHY_BASE + HDMI_PHY_GLB_CFG);
	writel(0x01, HDMI_PLL_BASE + HDMI_UNI_PLL_GLB_CFG);
	writel(0x01, HDMI_PLL_BASE + HDMI_UNI_PLL_REFCLK_CFG);
	writel(0x19, HDMI_PLL_BASE + HDMI_UNI_PLL_VCOLPF_CFG);
	writel(0x0E, HDMI_PLL_BASE + HDMI_UNI_PLL_LPFR_CFG);
	writel(0x20, HDMI_PLL_BASE + HDMI_UNI_PLL_LPFC1_CFG);
	writel(0x0D, HDMI_PLL_BASE + HDMI_UNI_PLL_LPFC2_CFG);
	writel(0x00, HDMI_PLL_BASE + HDMI_UNI_PLL_SDM_CFG0);
	writel(0x52, HDMI_PLL_BASE + HDMI_UNI_PLL_SDM_CFG1);
	writel(0x00, HDMI_PLL_BASE + HDMI_UNI_PLL_SDM_CFG2);
	writel(0x56, HDMI_PLL_BASE + HDMI_UNI_PLL_SDM_CFG3);
	writel(0x00, HDMI_PLL_BASE + HDMI_UNI_PLL_SDM_CFG4);
	writel(0x10, HDMI_PLL_BASE + HDMI_UNI_PLL_LKDET_CFG0);
	writel(0x1A, HDMI_PLL_BASE + HDMI_UNI_PLL_LKDET_CFG1);
	writel(0x05, HDMI_PLL_BASE + HDMI_UNI_PLL_LKDET_CFG2);
	writel(0x01, HDMI_PLL_BASE + HDMI_UNI_PLL_POSTDIV1_CFG);
	writel(0x00, HDMI_PLL_BASE + HDMI_UNI_PLL_POSTDIV2_CFG);
	writel(0x00, HDMI_PLL_BASE + HDMI_UNI_PLL_POSTDIV3_CFG);
	writel(0x01, HDMI_PLL_BASE + HDMI_UNI_PLL_CAL_CFG2);
	writel(0x60, HDMI_PLL_BASE + HDMI_UNI_PLL_CAL_CFG8);
	writel(0x00, HDMI_PLL_BASE + HDMI_UNI_PLL_CAL_CFG9);
	writel(0xE6, HDMI_PLL_BASE + HDMI_UNI_PLL_CAL_CFG10);
	writel(0x02, HDMI_PLL_BASE + HDMI_UNI_PLL_CAL_CFG11);
	writel(0x1F, HDMI_PHY_BASE + HDMI_PHY_PD_CTRL0);
	udelay(50);

	writel(0x0F, HDMI_PLL_BASE + HDMI_UNI_PLL_GLB_CFG);
	writel(0x00, HDMI_PHY_BASE + HDMI_PHY_PD_CTRL1);
	writel(0x10, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG2);
	writel(0xDB, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG0);
	writel(0x43, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG1);
	writel(0x02, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG2);
	writel(0x00, HDMI_PHY_BASE + HDMI_PHY_ANA_CFG3);
	writel(0x04, HDMI_PLL_BASE + HDMI_UNI_PLL_VREG_CFG);
	writel(0xD0, HDMI_PHY_BASE + HDMI_PHY_DCC_CFG0);
	writel(0x1A, HDMI_PHY_BASE + HDMI_PHY_DCC_CFG1);
	writel(0x00, HDMI_PHY_BASE + HDMI_PHY_TXCAL_CFG0);
	writel(0x00, HDMI_PHY_BASE + HDMI_PHY_TXCAL_CFG1);
	writel(0x02, HDMI_PHY_BASE + HDMI_PHY_TXCAL_CFG2);
	writel(0x05, HDMI_PHY_BASE + HDMI_PHY_TXCAL_CFG3);
	udelay(200);
}

