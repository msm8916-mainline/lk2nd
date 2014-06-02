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
#ifndef _QMP_PHY_H_
#define _QMP_PHY_H_

#include <platform/iomap.h>

/* QMP register offsets */
#define QSERDES_COM_DEC_START1               (PLATFORM_QMP_OFFSET + 0xA4)
#define QSERDES_COM_DEC_START2               (PLATFORM_QMP_OFFSET + 0x104)
#define QSERDES_COM_DIV_FRAC_START1          (PLATFORM_QMP_OFFSET + 0xF8)
#define QSERDES_COM_DIV_FRAC_START2          (PLATFORM_QMP_OFFSET + 0xFC)
#define QSERDES_COM_DIV_FRAC_START3          (PLATFORM_QMP_OFFSET + 0x100)
#define QSERDES_COM_PLLLOCK_CMP_EN           (PLATFORM_QMP_OFFSET + 0x94)
#define QSERDES_COM_PLLLOCK_CMP1             (PLATFORM_QMP_OFFSET + 0x88)
#define QSERDES_COM_PLLLOCK_CMP2             (PLATFORM_QMP_OFFSET + 0x8C)
#define QSERDES_COM_PLL_CRCTRL               (PLATFORM_QMP_OFFSET + 0x10C)
#define QSERDES_COM_RES_CODE_START_SEG1      (PLATFORM_QMP_OFFSET + 0xD8)
#define QSERDES_COM_RES_CODE_CAL_CSR         (PLATFORM_QMP_OFFSET + 0xE0)
#define QSERDES_COM_RES_TRIM_CONTROL         (PLATFORM_QMP_OFFSET + 0xE8)
#define QSERDES_COM_SSC_EN_CENTER            (PLATFORM_QMP_OFFSET + 0xAC)
#define QSERDES_COM_SSC_ADJ_PER1             (PLATFORM_QMP_OFFSET + 0xB0)
#define QSERDES_COM_SSC_PER1                 (PLATFORM_QMP_OFFSET + 0xB8)
#define QSERDES_COM_SSC_PER2                 (PLATFORM_QMP_OFFSET + 0xBC)
#define QSERDES_COM_SSC_STEP_SIZE1           (PLATFORM_QMP_OFFSET + 0xC0)
#define QSERDES_COM_SSC_STEP_SIZE2           (PLATFORM_QMP_OFFSET + 0xC4)

#define QSERDES_COM_PLL_VCOTAIL_EN           0x004
#define QSERDES_COM_IE_TRIM                  0x0C
#define QSERDES_COM_IP_TRIM                  0x10
#define QSERDES_COM_PLL_CNTRL                0x14
#define QSERDES_COM_PLL_IP_SETI              0x24
#define QSERDES_COM_PLL_CP_SETI              0x34
#define QSERDES_COM_PLL_IP_SETP              0x38
#define QSERDES_COM_PLL_CP_SETP              0x3C
#define QSERDES_COM_RESETSM_CNTRL            0x4C
#define QSERDES_COM_SYSCLK_EN_SEL_TXBAND     0x48
#define QSERDES_COM_RESETSM_CNTRL2           0x50
#define PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_L   0x5C
#define PCIE_USB3_PHY_RCVR_DTCT_DLY_P1U2_H   0x60
#define QSERDES_TX_RCV_DETECT_LVL            0x268
#define QSERDES_RX_CDR_CONTROL1              0x400
#define QSERDES_RX_CDR_CONTROL2              0x404
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2     0x4BC
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3     0x4C0
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4     0x4C4
#define QSERDES_RX_SIGDET_ENABLES            0x4F8
#define QSERDES_RX_SIGDET_CNTRL              0x500
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL     0x504
#define PCIE_USB3_PHY_SW_RESET               0x600
#define PCIE_USB3_PHY_POWER_DOWN_CONTROL     0x604
#define PCIE_USB3_PHY_START                  0x608
#define PCIE_USB3_PHY_RX_IDLE_DTCT_CNTRL     0x64C
#define PCIE_USB3_PHY_POWER_STATE_CONFIG2    0x654
#define PCIE_USB3_PHY_PCS_STATUS             0x728

void usb30_qmp_phy_reset(void);
void usb30_qmp_phy_init(void);

#endif
