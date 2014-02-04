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

#include <stdint.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <qmp_phy.h>
#include <reg.h>
#include <bits.h>

/* USB3.0 QMP phy reset */
void usb30_qmp_phy_reset(void)
{
	uint32_t val;

	/* phy com reset */
	val = readl(GCC_USB30_PHY_COM_BCR) | BIT(0);
	writel(val, GCC_USB30_PHY_COM_BCR);
	udelay(10);
	writel(val & ~BIT(0), GCC_USB30_PHY_COM_BCR);

	/* SS PHY reset */
	val = readl(GCC_USB3_PHY_BCR) | BIT(0);
	writel(val, GCC_USB3_PHY_BCR);
	udelay(10);
	writel(val & ~BIT(0), GCC_USB3_PHY_BCR);

	/* pipe clk reset */
	val = readl(GCC_USB30PHY_PHY_BCR) | BIT(0);
	writel(val, GCC_USB30PHY_PHY_BCR);
	udelay(10);
	writel(val & ~BIT(0), GCC_USB30PHY_PHY_BCR);
}

/* USB 3.0 phy init: HPG for QMP phy*/
void usb30_qmp_phy_init()
{
	/* Sequence as per HPG */
	writel(0x11, PERIPH_SS_AHB2PHY_TOP_CFG);

	writel(0x01, QMP_PHY_BASE + PCIE_USB3_PHY_POWER_DOWN_CONTROL);
	writel(0x08, QMP_PHY_BASE + QSERDES_COM_SYSCLK_EN_SEL_TXBAND);
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
	writel(0x05, QMP_PHY_BASE + QSERDES_COM_RESETSM_CNTRL2);

	writel(0x20, QMP_PHY_BASE + QSERDES_COM_RES_CODE_START_SEG1);
	writel(0x77, QMP_PHY_BASE + QSERDES_COM_RES_CODE_CAL_CSR);
	writel(0x15, QMP_PHY_BASE + QSERDES_COM_RES_TRIM_CONTROL);
	writel(0x03, QMP_PHY_BASE + QSERDES_TX_RCV_DETECT_LVL);
	writel(0x02, QMP_PHY_BASE + QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2);
	writel(0x6C, QMP_PHY_BASE + QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3);
	writel(0xC7, QMP_PHY_BASE + QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4);
	writel(0x40, QMP_PHY_BASE + QSERDES_RX_SIGDET_ENABLES);
	writel(0x70, QMP_PHY_BASE + QSERDES_RX_SIGDET_CNTRL);
	writel(0x06, QMP_PHY_BASE + QSERDES_RX_SIGDET_DEGLITCH_CNTRL);
	writel(0x48, QMP_PHY_BASE + PCIE_USB3_PHY_RX_IDLE_DTCT_CNTRL);
	writel(0x01, QMP_PHY_BASE + QSERDES_COM_SSC_EN_CENTER);
	writel(0x02, QMP_PHY_BASE + QSERDES_COM_SSC_ADJ_PER1);
	writel(0x31, QMP_PHY_BASE + QSERDES_COM_SSC_PER1);
	writel(0x01, QMP_PHY_BASE + QSERDES_COM_SSC_PER2);
	writel(0x19, QMP_PHY_BASE + QSERDES_COM_SSC_STEP_SIZE1);
	writel(0x19, QMP_PHY_BASE + QSERDES_COM_SSC_STEP_SIZE2);
	writel(0x08, QMP_PHY_BASE + PCIE_USB3_PHY_POWER_STATE_CONFIG2);

	writel(0xE5, QMP_PHY_BASE + PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_L);
	writel(0x03, QMP_PHY_BASE + PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_H);

	writel(0x00, QMP_PHY_BASE + PCIE_USB3_PHY_SW_RESET);
	writel(0x03, QMP_PHY_BASE + PCIE_USB3_PHY_START);

	clock_bumpup_pipe3_clk();
}
