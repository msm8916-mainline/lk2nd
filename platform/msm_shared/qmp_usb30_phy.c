/* Copyright (c) 2014, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <sys/types.h>
#include <stdint.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <qmp_phy.h>
#include <reg.h>
#include <bits.h>
#include <clock.h>
#include <debug.h>

#define HS_PHY_COMMON_CTRL             0xEC
#define USE_CORECLK                    BIT(14)
#define PLLBTUNE                       BIT(15)
#define FSEL                           (0x7 << 4)
#define DIS_RETENTION                  BIT(18)
#define QMP_PHY_MAX_TIMEOUT            1000
#define PHYSTATUS                      BIT(6)

__WEAK uint32_t target_override_pll()
{
	return 0;
}

/* USB3.0 QMP phy reset */
void usb30_qmp_phy_reset(void)
{
	int ret = 0;
	uint32_t val;
	bool phy_com_reset = false;

	struct clk *usb2b_clk = NULL;
	struct clk *usb_pipe_clk = NULL;
	struct clk *phy_com_clk = NULL;
	struct clk *phy_clk = NULL;

	/* Look if phy com clock is present */
	phy_com_clk = clk_get("usb30_phy_com_reset");
	if (phy_com_clk)
		phy_com_reset = true;

	usb2b_clk = clk_get("usb2b_phy_sleep_clk");
	ASSERT(usb2b_clk);

	phy_clk  = clk_get("usb30_phy_reset");
	ASSERT(phy_clk);

	usb_pipe_clk = clk_get("usb30_pipe_clk");
	ASSERT(usb_pipe_clk);

	/* ASSERT */
	ret = clk_reset(usb2b_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb2b_phy_clk\n");
		return;
	}

	if (phy_com_reset)
	{
		ret = clk_reset(phy_com_clk, CLK_RESET_ASSERT);
		if (ret)
		{
			dprintf(CRITICAL, "Failed to assert phy_com_clk\n");
			goto deassert_usb2b_clk;
		}
	}

	ret = clk_reset(phy_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert phy_clk\n");
		goto deassert_phy_com_clk;
	}

	ret = clk_reset(usb_pipe_clk, CLK_RESET_ASSERT);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to assert usb_pipe_clk\n");
		goto deassert_phy_clk;
	}

	udelay(100);

	/* DEASSERT */
	ret = clk_reset(usb_pipe_clk, CLK_RESET_DEASSERT);
	if (ret)
		dprintf(CRITICAL, "Failed to deassert usb_pipe_clk\n");

deassert_phy_clk:
	ret = clk_reset(phy_clk, CLK_RESET_DEASSERT);
	if (ret)
		dprintf(CRITICAL, "Failed to deassert phy_clk\n");

deassert_phy_com_clk:
	if (phy_com_reset)
	{
		ret = clk_reset(phy_com_clk, CLK_RESET_DEASSERT);
		if (ret)
			dprintf(CRITICAL, "Failed to deassert phy_com_clk\n");
	}

deassert_usb2b_clk:
	ret = clk_reset(usb2b_clk, CLK_RESET_DEASSERT);
	if (ret)
		dprintf(CRITICAL, "Failed to deassert usb2b_phy_clk\n");

	/* Override the phy common control values */
	val = readl(MSM_USB30_QSCRATCH_BASE + HS_PHY_COMMON_CTRL);
	val |= USE_CORECLK | PLLBTUNE;
	val &= ~FSEL;
	val &= ~DIS_RETENTION;
	writel(val, MSM_USB30_QSCRATCH_BASE + HS_PHY_COMMON_CTRL);
}

/* USB 3.0 phy init: HPG for QMP phy*/
void usb30_qmp_phy_init()
{
	int timeout = QMP_PHY_MAX_TIMEOUT;

	/* Sequence as per HPG */

	writel(0x01, QMP_PHY_BASE + PCIE_USB3_PHY_POWER_DOWN_CONTROL);
	writel(0x08, QMP_PHY_BASE + QSERDES_COM_SYSCLK_EN_SEL_TXBAND);

	if (target_override_pll())
		writel(0xE1, QMP_PHY_BASE + QSERDES_COM_PLL_VCOTAIL_EN);

	writel(0x82, QMP_PHY_BASE + QSERDES_COM_DEC_START1);
	writel(0x03, QMP_PHY_BASE + QSERDES_COM_DEC_START2);
	writel(0xD5, QMP_PHY_BASE + QSERDES_COM_DIV_FRAC_START1);
	writel(0xAA, QMP_PHY_BASE + QSERDES_COM_DIV_FRAC_START2);
	writel(0x4D, QMP_PHY_BASE + QSERDES_COM_DIV_FRAC_START3);
	writel(0x01, QMP_PHY_BASE + QSERDES_COM_PLLLOCK_CMP_EN);
	writel(0x2B, QMP_PHY_BASE + QSERDES_COM_PLLLOCK_CMP1);
	writel(0x68, QMP_PHY_BASE + QSERDES_COM_PLLLOCK_CMP2);
	writel(0x7C, QMP_PHY_BASE + QSERDES_COM_PLL_CRCTRL);
	writel(0x02, QMP_PHY_BASE + QSERDES_COM_PLL_CP_SETI);
	writel(0x1F, QMP_PHY_BASE + QSERDES_COM_PLL_IP_SETP);
	writel(0x0F, QMP_PHY_BASE + QSERDES_COM_PLL_CP_SETP);
	writel(0x01, QMP_PHY_BASE + QSERDES_COM_PLL_IP_SETI);
	writel(0x0F, QMP_PHY_BASE + QSERDES_COM_IE_TRIM);
	writel(0x0F, QMP_PHY_BASE + QSERDES_COM_IP_TRIM);
	writel(0x46, QMP_PHY_BASE + QSERDES_COM_PLL_CNTRL);

	/* CDR Settings */
	writel(0xDA, QMP_PHY_BASE + QSERDES_RX_CDR_CONTROL1);
	writel(0x42, QMP_PHY_BASE + QSERDES_RX_CDR_CONTROL2);

	/* Calibration Settings */
	writel(0x90, QMP_PHY_BASE + QSERDES_COM_RESETSM_CNTRL);
	if (target_override_pll())
		writel(0x07, QMP_PHY_BASE + QSERDES_COM_RESETSM_CNTRL2);
	else
		writel(0x05, QMP_PHY_BASE + QSERDES_COM_RESETSM_CNTRL2);

	writel(0x20, QMP_PHY_BASE + QSERDES_COM_RES_CODE_START_SEG1);
	writel(0x77, QMP_PHY_BASE + QSERDES_COM_RES_CODE_CAL_CSR);
	writel(0x15, QMP_PHY_BASE + QSERDES_COM_RES_TRIM_CONTROL);
	writel(0x03, QMP_PHY_BASE + QSERDES_TX_RCV_DETECT_LVL);
	writel(0x02, QMP_PHY_BASE + QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2);
	writel(0x6C, QMP_PHY_BASE + QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3);
	writel(0xC7, QMP_PHY_BASE + QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4);
	writel(0x40, QMP_PHY_BASE + QSERDES_RX_SIGDET_ENABLES);
	writel(0x73, QMP_PHY_BASE + QSERDES_RX_SIGDET_CNTRL);
	writel(0x06, QMP_PHY_BASE + QSERDES_RX_SIGDET_DEGLITCH_CNTRL);
	writel(0x48, QMP_PHY_BASE + PCIE_USB3_PHY_RX_IDLE_DTCT_CNTRL);
	writel(0x01, QMP_PHY_BASE + QSERDES_COM_SSC_EN_CENTER);
	writel(0x02, QMP_PHY_BASE + QSERDES_COM_SSC_ADJ_PER1);
	writel(0x31, QMP_PHY_BASE + QSERDES_COM_SSC_PER1);
	writel(0x01, QMP_PHY_BASE + QSERDES_COM_SSC_PER2);
	writel(0x19, QMP_PHY_BASE + QSERDES_COM_SSC_STEP_SIZE1);
	writel(0x19, QMP_PHY_BASE + QSERDES_COM_SSC_STEP_SIZE2);
	writel(0x08, QMP_PHY_BASE + PCIE_USB3_PHY_POWER_STATE_CONFIG2);

	writel(0x00, QMP_PHY_BASE + PCIE_USB3_PHY_SW_RESET);
	writel(0x03, QMP_PHY_BASE + PCIE_USB3_PHY_START);

	clock_bumpup_pipe3_clk();

	while ((readl(QMP_PHY_BASE + PCIE_USB3_PHY_PCS_STATUS) & PHYSTATUS))
	{
		udelay(1);
		timeout--;
		if (!timeout)
		{
			dprintf(CRITICAL, "QMP phy initialization failed\n");
			return;
		}
	}
}
