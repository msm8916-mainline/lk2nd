/* Copyright (c) 2014-2016, The Linux Foundation. All rights reserved.

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
#include <arch/defines.h>
#include <platform/iomap.h>
#include <qusb2_phy.h>
#include <reg.h>
#include <bits.h>
#include <debug.h>
#include <qtimer.h>
#include <platform.h>

__WEAK int platform_is_msm8994()
{
	return 0;
}

__WEAK int platform_is_msm8996()
{
	return 0;
}

__WEAK int platform_is_mdmcalifornium()
{
	return 0;
}

__WEAK int platform_is_msm8953()
{
	return 0;
}

void qusb2_phy_reset(void)
{
	uint32_t val;
	/* Default tune value */
	uint8_t tune2 = 0xB3;
	int retry = 100;
	int se_clock = 1;

	/* Disable the ref clock before phy reset */
#if GCC_RX2_USB2_CLKREF_EN
	writel((readl(GCC_RX2_USB2_CLKREF_EN) & ~0x1), GCC_RX2_USB2_CLKREF_EN);
	dmb();
#endif
	/* Block Reset */
	val = readl(GCC_QUSB2_PHY_BCR) | BIT(0);
	writel(val, GCC_QUSB2_PHY_BCR);
	udelay(10);
	writel(val & ~BIT(0), GCC_QUSB2_PHY_BCR);

	/* configure the abh2 phy to wait state */
	writel(0x11, PERIPH_SS_AHB2PHY_TOP_CFG);
	dmb();

	/* set CLAMP_N_EN and stay with disabled USB PHY */
	writel(0x23, QUSB2PHY_PORT_POWERDOWN);

	if (platform_is_msm8996() || platform_is_mdmcalifornium() || platform_is_msm8953())
	{
		writel(0xF8, QUSB2PHY_PORT_TUNE1);
		/* Upper nibble of tune2 register should be updated based on the fuse value.
		 * Read the bits 21..24 from fuse and update the upper nibble with this value
		 */
#if QFPROM_CORR_CALIB_ROW12_MSB
		uint8_t fuse_val = (readl(QFPROM_CORR_CALIB_ROW12_MSB) & 0x1E00000) >> 21;
		/* If fuse value is non zero then update the upper nibble with the fuse value
		 * otherwise use the default value
		 */
		if (fuse_val)
			tune2 = (tune2 & 0x0f) | (fuse_val << 4);
#endif
		writel(tune2, QUSB2PHY_PORT_TUNE2);
		writel(0x83, QUSB2PHY_PORT_TUNE3);
		writel(0xC0, QUSB2PHY_PORT_TUNE4);
		writel(0x30, QUSB2PHY_PLL_TUNE);
		writel(0x79, QUSB2PHY_PLL_USER_CTL1);
		writel(0x21, QUSB2PHY_PLL_USER_CTL2);
		writel(0x14, QUSB2PHY_PORT_TEST2);
		writel(0x9F, QUSB2PHY_PLL_AUTOPGM_CTL1);
		writel(0x00, QUSB2PHY_PLL_PWR_CTL);
	}
	else
	{
		/* Set HS impedance to 42ohms */
		writel(0xA0, QUSB2PHY_PORT_TUNE1);

		/* Set TX current to 19mA, TX SR and TX bias current to 1, 1 */
		writel(0xA5, QUSB2PHY_PORT_TUNE2);

		/* Increase autocalibration bias circuit settling time
		 * and enable utocalibration  */
		writel(0x81, QUSB2PHY_PORT_TUNE3);

		writel(0x85, QUSB2PHY_PORT_TUNE4);
	}

	/* Enable ULPI mode */
	if (platform_is_msm8994())
		writel(0x0,  QUSB2PHY_PORT_UTMI_CTRL2);
	/* set CLAMP_N_EN and USB PHY is enabled*/
	writel(0x22, QUSB2PHY_PORT_POWERDOWN);
	udelay(150);

	/* TCSR register bit 0 indicates whether single ended clock
	 * or differential clock configuration is enabled. Based on the
	 * configuration set the PLL_TEST register.
	 */
#if TCSR_PHY_CLK_SCHEME_SEL
	se_clock = readl(TCSR_PHY_CLK_SCHEME_SEL) & 0x1;
#endif
	/* By default consider differential clock configuration and if TCSR
	 * register bit 0 is not set then use single ended setting
	 */
	if (se_clock)
	{
		writel(0x80, QUSB2PHY_PLL_TEST);
	}
	else
	{
	/* turn the ref clock on for differential clocks */
#if GCC_RX2_USB2_CLKREF_EN
		writel((readl(GCC_RX2_USB2_CLKREF_EN) | 0x1), GCC_RX2_USB2_CLKREF_EN);
		dmb();
#endif
	}
	udelay(100);

	/* Check PLL status */
	while (!(readl(QUSB2PHY_PLL_STATUS) & QUSB2PHY_PLL_LOCK))
	{
		retry--;
		if (!retry)
		{
			dprintf(CRITICAL, "QUSB2PHY failed to lock: %d", readl(QUSB2PHY_PLL_STATUS));
			break;
		}
		/* As per recommendation form hw team wait for 5 us before reading the status */
		udelay(5);
	}
}
