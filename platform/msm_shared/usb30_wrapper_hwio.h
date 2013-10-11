/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
#ifndef __DWC_USB30_QSCRATCH_HWIO_H__
#define __DWC_USB30_QSCRATCH_HWIO_H__


/* Macros to simplify wrapper reg read */
#define REG_READ(_dev, _reg)            readl(HWIO_##_reg##_ADDR(_dev->base))
#define REG_READI(_dev, _reg, _index)   readl(HWIO_##_reg##_ADDR(_dev->base, _index))

/* Macros to simplify wrapper reg write */
#define REG_WRITE(_dev, _reg, _value)            writel(_value, HWIO_##_reg##_ADDR(_dev->base))
#define REG_WRITEI(_dev, _reg, _index, _value)   writel(_value, HWIO_##_reg##_ADDR(_dev->base, _index))

#define REG_BMSK(_reg, _field)          HWIO_##_reg##_##_field##_BMSK
#define REG_SHFT(_reg, _field)          HWIO_##_reg##_##_field##_SHFT

/* Macros to simplify wrapper reg field read */
#define REG_READ_FIELD(_dev, _reg, _field)             ((REG_READ(_dev,_reg) & REG_BMSK(_reg, _field)) >> REG_SHFT(_reg, _field))
#define REG_READ_FIELDI(_dev, _reg, _index, _field)    ((REG_READI(_dev,_reg, _index) & REG_BMSK(_reg, _field)) >> REG_SHFT(_reg, _field))

/* Macros to simplify wrapper reg field write: implementes read/modify/write */
#define REG_WRITE_FIELD(_dev, _reg, _field, _value) REG_WRITE(_dev, _reg, ((REG_READ(_dev, _reg) & ~REG_BMSK(_reg, _field)) | (_value << REG_SHFT(_reg, _field))))
#define REG_WRITE_FIELDI(_dev, _reg, _index, _field, _value)  REG_WRITEI(_dev, _reg, _index, ((REG_READI(_dev, _reg, _index) & ~REG_BMSK(_reg, _field)) | (_value << REG_SHFT(_reg, _field))))


/* The following defines are auto generated. */

/**
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    USB30_QSCRATCH
*/
/*----------------------------------------------------------------------------
 * MODULE: USB30_QSCRATCH
 *--------------------------------------------------------------------------*/

#define USB30_QSCRATCH_REG_BASE                                           (USB30_WRAPPER_BASE      + 0x000f8800)

#define HWIO_IPCAT_REG_ADDR(x)                                            ((x) + 0x00000000)
#define HWIO_IPCAT_REG_RMSK                                               0xffffffff
#define HWIO_IPCAT_REG_POR                                                0x10010001
#define HWIO_IPCAT_REG_IN(x)      \
        in_dword_masked(HWIO_IPCAT_REG_ADDR(x), HWIO_IPCAT_REG_RMSK)
#define HWIO_IPCAT_REG_INM(x, m)      \
        in_dword_masked(HWIO_IPCAT_REG_ADDR(x), m)
#define HWIO_IPCAT_REG_IPCAT_BMSK                                         0xffffffff
#define HWIO_IPCAT_REG_IPCAT_SHFT                                                0x0

#define HWIO_CTRL_REG_ADDR(x)                                             ((x) + 0x00000004)
#define HWIO_CTRL_REG_RMSK                                                    0x33ff
#define HWIO_CTRL_REG_POR                                                 0x00000190
#define HWIO_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_CTRL_REG_ADDR(x), HWIO_CTRL_REG_RMSK)
#define HWIO_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_CTRL_REG_ADDR(x), m)
#define HWIO_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_CTRL_REG_ADDR(x),v)
#define HWIO_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CTRL_REG_ADDR(x),m,v,HWIO_CTRL_REG_IN(x))
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SUSPEND_BMSK                              0x2000
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SUSPEND_SHFT                                 0xd
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SLEEP_BMSK                                0x1000
#define HWIO_CTRL_REG_HSIC_PLL_CTRL_SLEEP_SHFT                                   0xc
#define HWIO_CTRL_REG_BC_XCVR_SELECT_BMSK                                      0x300
#define HWIO_CTRL_REG_BC_XCVR_SELECT_SHFT                                        0x8
#define HWIO_CTRL_REG_BC_TERM_SELECT_BMSK                                       0x80
#define HWIO_CTRL_REG_BC_TERM_SELECT_SHFT                                        0x7
#define HWIO_CTRL_REG_BC_TX_VALID_BMSK                                          0x40
#define HWIO_CTRL_REG_BC_TX_VALID_SHFT                                           0x6
#define HWIO_CTRL_REG_BC_OPMODE_BMSK                                            0x30
#define HWIO_CTRL_REG_BC_OPMODE_SHFT                                             0x4
#define HWIO_CTRL_REG_BC_DMPULLDOWN_BMSK                                         0x8
#define HWIO_CTRL_REG_BC_DMPULLDOWN_SHFT                                         0x3
#define HWIO_CTRL_REG_BC_DPPULLDOWN_BMSK                                         0x4
#define HWIO_CTRL_REG_BC_DPPULLDOWN_SHFT                                         0x2
#define HWIO_CTRL_REG_BC_IDPULLUP_BMSK                                           0x2
#define HWIO_CTRL_REG_BC_IDPULLUP_SHFT                                           0x1
#define HWIO_CTRL_REG_BC_SEL_BMSK                                                0x1
#define HWIO_CTRL_REG_BC_SEL_SHFT                                                0x0

#define HWIO_GENERAL_CFG_ADDR(x)                                          ((x) + 0x00000008)
#define HWIO_GENERAL_CFG_RMSK                                                    0x6
#define HWIO_GENERAL_CFG_POR                                              0x00000000
#define HWIO_GENERAL_CFG_IN(x)      \
        in_dword_masked(HWIO_GENERAL_CFG_ADDR(x), HWIO_GENERAL_CFG_RMSK)
#define HWIO_GENERAL_CFG_INM(x, m)      \
        in_dword_masked(HWIO_GENERAL_CFG_ADDR(x), m)
#define HWIO_GENERAL_CFG_OUT(x, v)      \
        out_dword(HWIO_GENERAL_CFG_ADDR(x),v)
#define HWIO_GENERAL_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_GENERAL_CFG_ADDR(x),m,v,HWIO_GENERAL_CFG_IN(x))
#define HWIO_GENERAL_CFG_XHCI_REV_BMSK                                           0x4
#define HWIO_GENERAL_CFG_XHCI_REV_SHFT                                           0x2
#define HWIO_GENERAL_CFG_DBM_EN_BMSK                                             0x2
#define HWIO_GENERAL_CFG_DBM_EN_SHFT                                             0x1

#define HWIO_RAM1_REG_ADDR(x)                                             ((x) + 0x0000000c)
#define HWIO_RAM1_REG_RMSK                                                       0x7
#define HWIO_RAM1_REG_POR                                                 0x00000000
#define HWIO_RAM1_REG_IN(x)      \
        in_dword_masked(HWIO_RAM1_REG_ADDR(x), HWIO_RAM1_REG_RMSK)
#define HWIO_RAM1_REG_INM(x, m)      \
        in_dword_masked(HWIO_RAM1_REG_ADDR(x), m)
#define HWIO_RAM1_REG_OUT(x, v)      \
        out_dword(HWIO_RAM1_REG_ADDR(x),v)
#define HWIO_RAM1_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RAM1_REG_ADDR(x),m,v,HWIO_RAM1_REG_IN(x))
#define HWIO_RAM1_REG_RAM13_EN_BMSK                                              0x4
#define HWIO_RAM1_REG_RAM13_EN_SHFT                                              0x2
#define HWIO_RAM1_REG_RAM12_EN_BMSK                                              0x2
#define HWIO_RAM1_REG_RAM12_EN_SHFT                                              0x1
#define HWIO_RAM1_REG_RAM11_EN_BMSK                                              0x1
#define HWIO_RAM1_REG_RAM11_EN_SHFT                                              0x0

/* Note: HS_PHYCTRL_COMMON register is added only from 8084 and onwards. */
#define HWIO_HS_PHY_CTRL_COMMON_ADDR(x)                                     ((x) + 0x000000ec)
#define HWIO_HS_PHY_CTRL_COMMON_RMSK                                            0x7fff
#define HWIO_HS_PHY_CTRL_COMMON_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_COMMON_ADDR(x), HWIO_HS_PHY_CTRL_COMMON_RMSK)
#define HWIO_HS_PHY_CTRL_COMMON_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_COMMON_ADDR(x), m)
#define HWIO_HS_PHY_CTRL_COMMON_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_CTRL_COMMON_ADDR(x),v)
#define HWIO_HS_PHY_CTRL_COMMON_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_CTRL_COMMON_ADDR(x),m,v,HWIO_HS_PHY_CTRL_COMMON_IN(x))
#define HWIO_HS_PHY_CTRL_COMMON_USE_CLKCORE_BMSK                                0x4000
#define HWIO_HS_PHY_CTRL_COMMON_USE_CLKCORE_SHFT                                   0xe
#define HWIO_HS_PHY_CTRL_COMMON_ACAENB0_BMSK                                    0x2000
#define HWIO_HS_PHY_CTRL_COMMON_ACAENB0_SHFT                                       0xd
#define HWIO_HS_PHY_CTRL_COMMON_VBUSVLDEXTSEL0_BMSK                             0x1000
#define HWIO_HS_PHY_CTRL_COMMON_VBUSVLDEXTSEL0_SHFT                                0xc
#define HWIO_HS_PHY_CTRL_COMMON_OTGDISABLE0_BMSK                                 0x800
#define HWIO_HS_PHY_CTRL_COMMON_OTGDISABLE0_SHFT                                   0xb
#define HWIO_HS_PHY_CTRL_COMMON_OTGTUNE0_BMSK                                    0x700
#define HWIO_HS_PHY_CTRL_COMMON_OTGTUNE0_SHFT                                      0x8
#define HWIO_HS_PHY_CTRL_COMMON_COMMONONN_BMSK                                    0x80
#define HWIO_HS_PHY_CTRL_COMMON_COMMONONN_SHFT                                     0x7
#define HWIO_HS_PHY_CTRL_COMMON_FSEL_BMSK                                         0x70
#define HWIO_HS_PHY_CTRL_COMMON_FSEL_SHFT                                          0x4
#define HWIO_HS_PHY_CTRL_COMMON_RETENABLEN_BMSK                                    0x8
#define HWIO_HS_PHY_CTRL_COMMON_RETENABLEN_SHFT                                    0x3
#define HWIO_HS_PHY_CTRL_COMMON_SIDDQ_BMSK                                         0x4
#define HWIO_HS_PHY_CTRL_COMMON_SIDDQ_SHFT                                         0x2
#define HWIO_HS_PHY_CTRL_COMMON_VATESTENB_BMSK                                     0x3
#define HWIO_HS_PHY_CTRL_COMMON_VATESTENB_SHFT                                     0x0


#define HWIO_HS_PHY_CTRL_ADDR(x)                                          ((x) + 0x00000010)
#define HWIO_HS_PHY_CTRL_RMSK                                              0x7ffffff
#define HWIO_HS_PHY_CTRL_POR                                              0x072203b2
#define HWIO_HS_PHY_CTRL_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_ADDR(x), HWIO_HS_PHY_CTRL_RMSK)
#define HWIO_HS_PHY_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_CTRL_ADDR(x), m)
#define HWIO_HS_PHY_CTRL_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_CTRL_ADDR(x),v)
#define HWIO_HS_PHY_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_CTRL_ADDR(x),m,v,HWIO_HS_PHY_CTRL_IN(x))

/* Note: This field is introduced only in 8084 and onwards. */
#define HWIO_HS_PHY_CTRL_SW_SESSVLD_SEL_BMSK                             0x10000000
#define HWIO_HS_PHY_CTRL_SW_SESSVLD_SEL_SHFT                                   0x1c

#define HWIO_HS_PHY_CTRL_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK                     0x4000000
#define HWIO_HS_PHY_CTRL_CLAMP_MPM_DPSE_DMSE_EN_N_SHFT                          0x1a
#define HWIO_HS_PHY_CTRL_FREECLK_SEL_BMSK                                  0x2000000
#define HWIO_HS_PHY_CTRL_FREECLK_SEL_SHFT                                       0x19
#define HWIO_HS_PHY_CTRL_DMSEHV_CLAMP_EN_N_BMSK                            0x1000000
#define HWIO_HS_PHY_CTRL_DMSEHV_CLAMP_EN_N_SHFT                                 0x18
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_SEL_BMSK                            0x800000
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_SEL_SHFT                                0x17
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_BMSK                                0x400000
#define HWIO_HS_PHY_CTRL_USB2_SUSPEND_N_SHFT                                    0x16
#define HWIO_HS_PHY_CTRL_USB2_UTMI_CLK_EN_BMSK                              0x200000
#define HWIO_HS_PHY_CTRL_USB2_UTMI_CLK_EN_SHFT                                  0x15
#define HWIO_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_BMSK                           0x100000
#define HWIO_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID_SHFT                               0x14
#define HWIO_HS_PHY_CTRL_AUTORESUME_BMSK                                     0x80000
#define HWIO_HS_PHY_CTRL_AUTORESUME_SHFT                                        0x13
#define HWIO_HS_PHY_CTRL_USE_CLKCORE_BMSK                                    0x40000
#define HWIO_HS_PHY_CTRL_USE_CLKCORE_SHFT                                       0x12
#define HWIO_HS_PHY_CTRL_DPSEHV_CLAMP_EN_N_BMSK                              0x20000
#define HWIO_HS_PHY_CTRL_DPSEHV_CLAMP_EN_N_SHFT                                 0x11
#define HWIO_HS_PHY_CTRL_IDHV_INTEN_BMSK                                     0x10000
#define HWIO_HS_PHY_CTRL_IDHV_INTEN_SHFT                                        0x10
#define HWIO_HS_PHY_CTRL_OTGSESSVLDHV_INTEN_BMSK                              0x8000
#define HWIO_HS_PHY_CTRL_OTGSESSVLDHV_INTEN_SHFT                                 0xf
#define HWIO_HS_PHY_CTRL_VBUSVLDEXTSEL0_BMSK                                  0x4000
#define HWIO_HS_PHY_CTRL_VBUSVLDEXTSEL0_SHFT                                     0xe
#define HWIO_HS_PHY_CTRL_VBUSVLDEXT0_BMSK                                     0x2000
#define HWIO_HS_PHY_CTRL_VBUSVLDEXT0_SHFT                                        0xd
#define HWIO_HS_PHY_CTRL_OTGDISABLE0_BMSK                                     0x1000
#define HWIO_HS_PHY_CTRL_OTGDISABLE0_SHFT                                        0xc
#define HWIO_HS_PHY_CTRL_COMMONONN_BMSK                                        0x800
#define HWIO_HS_PHY_CTRL_COMMONONN_SHFT                                          0xb
#define HWIO_HS_PHY_CTRL_ULPIPOR_BMSK                                          0x400
#define HWIO_HS_PHY_CTRL_ULPIPOR_SHFT                                            0xa
#define HWIO_HS_PHY_CTRL_ID_HV_CLAMP_EN_N_BMSK                                 0x200
#define HWIO_HS_PHY_CTRL_ID_HV_CLAMP_EN_N_SHFT                                   0x9
#define HWIO_HS_PHY_CTRL_OTGSESSVLD_HV_CLAMP_EN_N_BMSK                         0x100
#define HWIO_HS_PHY_CTRL_OTGSESSVLD_HV_CLAMP_EN_N_SHFT                           0x8
#define HWIO_HS_PHY_CTRL_CLAMP_EN_N_BMSK                                        0x80
#define HWIO_HS_PHY_CTRL_CLAMP_EN_N_SHFT                                         0x7
#define HWIO_HS_PHY_CTRL_FSEL_BMSK                                              0x70
#define HWIO_HS_PHY_CTRL_FSEL_SHFT                                               0x4
#define HWIO_HS_PHY_CTRL_REFCLKOUT_EN_BMSK                                       0x8
#define HWIO_HS_PHY_CTRL_REFCLKOUT_EN_SHFT                                       0x3
#define HWIO_HS_PHY_CTRL_SIDDQ_BMSK                                              0x4
#define HWIO_HS_PHY_CTRL_SIDDQ_SHFT                                              0x2
#define HWIO_HS_PHY_CTRL_RETENABLEN_BMSK                                         0x2
#define HWIO_HS_PHY_CTRL_RETENABLEN_SHFT                                         0x1
#define HWIO_HS_PHY_CTRL_POR_BMSK                                                0x1
#define HWIO_HS_PHY_CTRL_POR_SHFT                                                0x0

#define HWIO_PARAMETER_OVERRIDE_X_ADDR(x)                                 ((x) + 0x00000014)
#define HWIO_PARAMETER_OVERRIDE_X_RMSK                                     0x3ffffff
#define HWIO_PARAMETER_OVERRIDE_X_POR                                     0x00de06e4
#define HWIO_PARAMETER_OVERRIDE_X_IN(x)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_ADDR(x), HWIO_PARAMETER_OVERRIDE_X_RMSK)
#define HWIO_PARAMETER_OVERRIDE_X_INM(x, m)      \
        in_dword_masked(HWIO_PARAMETER_OVERRIDE_X_ADDR(x), m)
#define HWIO_PARAMETER_OVERRIDE_X_OUT(x, v)      \
        out_dword(HWIO_PARAMETER_OVERRIDE_X_ADDR(x),v)
#define HWIO_PARAMETER_OVERRIDE_X_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PARAMETER_OVERRIDE_X_ADDR(x),m,v,HWIO_PARAMETER_OVERRIDE_X_IN(x))
#define HWIO_PARAMETER_OVERRIDE_X_TXFSLSTUNE0_BMSK                         0x3c00000
#define HWIO_PARAMETER_OVERRIDE_X_TXFSLSTUNE0_SHFT                              0x16
#define HWIO_PARAMETER_OVERRIDE_X_TXRESTUNE0_BMSK                           0x300000
#define HWIO_PARAMETER_OVERRIDE_X_TXRESTUNE0_SHFT                               0x14
#define HWIO_PARAMETER_OVERRIDE_X_TXHSXVTUNE0_BMSK                           0xc0000
#define HWIO_PARAMETER_OVERRIDE_X_TXHSXVTUNE0_SHFT                              0x12
#define HWIO_PARAMETER_OVERRIDE_X_TXRISETUNE0_BMSK                           0x30000
#define HWIO_PARAMETER_OVERRIDE_X_TXRISETUNE0_SHFT                              0x10
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPAMPTUNE0_BMSK                       0xc000
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPAMPTUNE0_SHFT                          0xe
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPPULSETUNE0_BMSK                     0x2000
#define HWIO_PARAMETER_OVERRIDE_X_TXPREEMPPULSETUNE0_SHFT                        0xd
#define HWIO_PARAMETER_OVERRIDE_X_TXVREFTUNE0_BMSK                            0x1e00
#define HWIO_PARAMETER_OVERRIDE_X_TXVREFTUNE0_SHFT                               0x9
#define HWIO_PARAMETER_OVERRIDE_X_SQRXTUNE0_BMSK                               0x1c0
#define HWIO_PARAMETER_OVERRIDE_X_SQRXTUNE0_SHFT                                 0x6
#define HWIO_PARAMETER_OVERRIDE_X_OTGTUNE0_BMSK                                 0x38
#define HWIO_PARAMETER_OVERRIDE_X_OTGTUNE0_SHFT                                  0x3
#define HWIO_PARAMETER_OVERRIDE_X_COMPDISTUNE0_BMSK                              0x7
#define HWIO_PARAMETER_OVERRIDE_X_COMPDISTUNE0_SHFT                              0x0

#define HWIO_CHARGING_DET_CTRL_ADDR(x)                                    ((x) + 0x00000018)
#define HWIO_CHARGING_DET_CTRL_RMSK                                             0x3f
#define HWIO_CHARGING_DET_CTRL_POR                                        0x00000000
#define HWIO_CHARGING_DET_CTRL_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_ADDR(x), HWIO_CHARGING_DET_CTRL_RMSK)
#define HWIO_CHARGING_DET_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_CTRL_ADDR(x), m)
#define HWIO_CHARGING_DET_CTRL_OUT(x, v)      \
        out_dword(HWIO_CHARGING_DET_CTRL_ADDR(x),v)
#define HWIO_CHARGING_DET_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CHARGING_DET_CTRL_ADDR(x),m,v,HWIO_CHARGING_DET_CTRL_IN(x))
#define HWIO_CHARGING_DET_CTRL_VDATDETENB0_BMSK                                 0x20
#define HWIO_CHARGING_DET_CTRL_VDATDETENB0_SHFT                                  0x5
#define HWIO_CHARGING_DET_CTRL_VDATSRCENB0_BMSK                                 0x10
#define HWIO_CHARGING_DET_CTRL_VDATSRCENB0_SHFT                                  0x4
#define HWIO_CHARGING_DET_CTRL_VDMSRCAUTO_BMSK                                   0x8
#define HWIO_CHARGING_DET_CTRL_VDMSRCAUTO_SHFT                                   0x3
#define HWIO_CHARGING_DET_CTRL_CHRGSEL0_BMSK                                     0x4
#define HWIO_CHARGING_DET_CTRL_CHRGSEL0_SHFT                                     0x2
#define HWIO_CHARGING_DET_CTRL_DCDENB0_BMSK                                      0x2
#define HWIO_CHARGING_DET_CTRL_DCDENB0_SHFT                                      0x1
#define HWIO_CHARGING_DET_CTRL_ACAENB0_BMSK                                      0x1
#define HWIO_CHARGING_DET_CTRL_ACAENB0_SHFT                                      0x0

#define HWIO_CHARGING_DET_OUTPUT_ADDR(x)                                  ((x) + 0x0000001c)
#define HWIO_CHARGING_DET_OUTPUT_RMSK                                          0xfff
#define HWIO_CHARGING_DET_OUTPUT_POR                                      0x00000000
#define HWIO_CHARGING_DET_OUTPUT_IN(x)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_ADDR(x), HWIO_CHARGING_DET_OUTPUT_RMSK)
#define HWIO_CHARGING_DET_OUTPUT_INM(x, m)      \
        in_dword_masked(HWIO_CHARGING_DET_OUTPUT_ADDR(x), m)
#define HWIO_CHARGING_DET_OUTPUT_DMSEHV_BMSK                                   0x800
#define HWIO_CHARGING_DET_OUTPUT_DMSEHV_SHFT                                     0xb
#define HWIO_CHARGING_DET_OUTPUT_DPSEHV_BMSK                                   0x400
#define HWIO_CHARGING_DET_OUTPUT_DPSEHV_SHFT                                     0xa
#define HWIO_CHARGING_DET_OUTPUT_LINESTATE_BMSK                                0x300
#define HWIO_CHARGING_DET_OUTPUT_LINESTATE_SHFT                                  0x8
#define HWIO_CHARGING_DET_OUTPUT_RIDFLOAT_N_BMSK                                0x80
#define HWIO_CHARGING_DET_OUTPUT_RIDFLOAT_N_SHFT                                 0x7
#define HWIO_CHARGING_DET_OUTPUT_RIDFLOAT_BMSK                                  0x40
#define HWIO_CHARGING_DET_OUTPUT_RIDFLOAT_SHFT                                   0x6
#define HWIO_CHARGING_DET_OUTPUT_RIDGND_BMSK                                    0x20
#define HWIO_CHARGING_DET_OUTPUT_RIDGND_SHFT                                     0x5
#define HWIO_CHARGING_DET_OUTPUT_RIDC_BMSK                                      0x10
#define HWIO_CHARGING_DET_OUTPUT_RIDC_SHFT                                       0x4
#define HWIO_CHARGING_DET_OUTPUT_RIDB_BMSK                                       0x8
#define HWIO_CHARGING_DET_OUTPUT_RIDB_SHFT                                       0x3
#define HWIO_CHARGING_DET_OUTPUT_RIDA_BMSK                                       0x4
#define HWIO_CHARGING_DET_OUTPUT_RIDA_SHFT                                       0x2
#define HWIO_CHARGING_DET_OUTPUT_DCDOUT_BMSK                                     0x2
#define HWIO_CHARGING_DET_OUTPUT_DCDOUT_SHFT                                     0x1
#define HWIO_CHARGING_DET_OUTPUT_CHGDET_BMSK                                     0x1
#define HWIO_CHARGING_DET_OUTPUT_CHGDET_SHFT                                     0x0

#define HWIO_ALT_INTERRUPT_EN_ADDR(x)                                     ((x) + 0x00000020)
#define HWIO_ALT_INTERRUPT_EN_RMSK                                             0xfff
#define HWIO_ALT_INTERRUPT_EN_POR                                         0x00000000
#define HWIO_ALT_INTERRUPT_EN_IN(x)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_ADDR(x), HWIO_ALT_INTERRUPT_EN_RMSK)
#define HWIO_ALT_INTERRUPT_EN_INM(x, m)      \
        in_dword_masked(HWIO_ALT_INTERRUPT_EN_ADDR(x), m)
#define HWIO_ALT_INTERRUPT_EN_OUT(x, v)      \
        out_dword(HWIO_ALT_INTERRUPT_EN_ADDR(x),v)
#define HWIO_ALT_INTERRUPT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_ALT_INTERRUPT_EN_ADDR(x),m,v,HWIO_ALT_INTERRUPT_EN_IN(x))
#define HWIO_ALT_INTERRUPT_EN_DMSEHV_LO_INTEN_BMSK                             0x800
#define HWIO_ALT_INTERRUPT_EN_DMSEHV_LO_INTEN_SHFT                               0xb
#define HWIO_ALT_INTERRUPT_EN_DMSEHV_HI_INTEN_BMSK                             0x400
#define HWIO_ALT_INTERRUPT_EN_DMSEHV_HI_INTEN_SHFT                               0xa
#define HWIO_ALT_INTERRUPT_EN_DPSEHV_LO_INTEN_BMSK                             0x200
#define HWIO_ALT_INTERRUPT_EN_DPSEHV_LO_INTEN_SHFT                               0x9
#define HWIO_ALT_INTERRUPT_EN_DPSEHV_HI_INTEN_BMSK                             0x100
#define HWIO_ALT_INTERRUPT_EN_DPSEHV_HI_INTEN_SHFT                               0x8
#define HWIO_ALT_INTERRUPT_EN_DMSEHV_INTEN_BMSK                                 0x80
#define HWIO_ALT_INTERRUPT_EN_DMSEHV_INTEN_SHFT                                  0x7
#define HWIO_ALT_INTERRUPT_EN_DPSEHV_INTEN_BMSK                                 0x40
#define HWIO_ALT_INTERRUPT_EN_DPSEHV_INTEN_SHFT                                  0x6
#define HWIO_ALT_INTERRUPT_EN_RIDFLOATNINTEN_BMSK                               0x20
#define HWIO_ALT_INTERRUPT_EN_RIDFLOATNINTEN_SHFT                                0x5
#define HWIO_ALT_INTERRUPT_EN_CHGDETINTEN_BMSK                                  0x10
#define HWIO_ALT_INTERRUPT_EN_CHGDETINTEN_SHFT                                   0x4
#define HWIO_ALT_INTERRUPT_EN_DPINTEN_BMSK                                       0x8
#define HWIO_ALT_INTERRUPT_EN_DPINTEN_SHFT                                       0x3
#define HWIO_ALT_INTERRUPT_EN_DCDINTEN_BMSK                                      0x4
#define HWIO_ALT_INTERRUPT_EN_DCDINTEN_SHFT                                      0x2
#define HWIO_ALT_INTERRUPT_EN_DMINTEN_BMSK                                       0x2
#define HWIO_ALT_INTERRUPT_EN_DMINTEN_SHFT                                       0x1
#define HWIO_ALT_INTERRUPT_EN_ACAINTEN_BMSK                                      0x1
#define HWIO_ALT_INTERRUPT_EN_ACAINTEN_SHFT                                      0x0

#define HWIO_HS_PHY_IRQ_STAT_ADDR(x)                                      ((x) + 0x00000024)
#define HWIO_HS_PHY_IRQ_STAT_RMSK                                              0xfff
#define HWIO_HS_PHY_IRQ_STAT_POR                                          0x00000000
#define HWIO_HS_PHY_IRQ_STAT_IN(x)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_ADDR(x), HWIO_HS_PHY_IRQ_STAT_RMSK)
#define HWIO_HS_PHY_IRQ_STAT_INM(x, m)      \
        in_dword_masked(HWIO_HS_PHY_IRQ_STAT_ADDR(x), m)
#define HWIO_HS_PHY_IRQ_STAT_OUT(x, v)      \
        out_dword(HWIO_HS_PHY_IRQ_STAT_ADDR(x),v)
#define HWIO_HS_PHY_IRQ_STAT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HS_PHY_IRQ_STAT_ADDR(x),m,v,HWIO_HS_PHY_IRQ_STAT_IN(x))
#define HWIO_HS_PHY_IRQ_STAT_DMSEHV_LO_INTLCH_BMSK                             0x800
#define HWIO_HS_PHY_IRQ_STAT_DMSEHV_LO_INTLCH_SHFT                               0xb
#define HWIO_HS_PHY_IRQ_STAT_DMSEHV_HI_INTLCH_BMSK                             0x400
#define HWIO_HS_PHY_IRQ_STAT_DMSEHV_HI_INTLCH_SHFT                               0xa
#define HWIO_HS_PHY_IRQ_STAT_DPSEHV_LO_INTLCH_BMSK                             0x200
#define HWIO_HS_PHY_IRQ_STAT_DPSEHV_LO_INTLCH_SHFT                               0x9
#define HWIO_HS_PHY_IRQ_STAT_DPSEHV_HI_INTLCH_BMSK                             0x100
#define HWIO_HS_PHY_IRQ_STAT_DPSEHV_HI_INTLCH_SHFT                               0x8
#define HWIO_HS_PHY_IRQ_STAT_DMSEHV_INTLCH_BMSK                                 0x80
#define HWIO_HS_PHY_IRQ_STAT_DMSEHV_INTLCH_SHFT                                  0x7
#define HWIO_HS_PHY_IRQ_STAT_DPSEHV_INTLCH_BMSK                                 0x40
#define HWIO_HS_PHY_IRQ_STAT_DPSEHV_INTLCH_SHFT                                  0x6
#define HWIO_HS_PHY_IRQ_STAT_RIDFLOATNINTLCH_BMSK                               0x20
#define HWIO_HS_PHY_IRQ_STAT_RIDFLOATNINTLCH_SHFT                                0x5
#define HWIO_HS_PHY_IRQ_STAT_CHGDETINTLCH_BMSK                                  0x10
#define HWIO_HS_PHY_IRQ_STAT_CHGDETINTLCH_SHFT                                   0x4
#define HWIO_HS_PHY_IRQ_STAT_DPINTLCH_BMSK                                       0x8
#define HWIO_HS_PHY_IRQ_STAT_DPINTLCH_SHFT                                       0x3
#define HWIO_HS_PHY_IRQ_STAT_DCDINTLCH_BMSK                                      0x4
#define HWIO_HS_PHY_IRQ_STAT_DCDINTLCH_SHFT                                      0x2
#define HWIO_HS_PHY_IRQ_STAT_DMINTLCH_BMSK                                       0x2
#define HWIO_HS_PHY_IRQ_STAT_DMINTLCH_SHFT                                       0x1
#define HWIO_HS_PHY_IRQ_STAT_ACAINTLCH_BMSK                                      0x1
#define HWIO_HS_PHY_IRQ_STAT_ACAINTLCH_SHFT                                      0x0

#define HWIO_CGCTL_REG_ADDR(x)                                            ((x) + 0x00000028)
#define HWIO_CGCTL_REG_RMSK                                                     0x1f
#define HWIO_CGCTL_REG_POR                                                0x00000000
#define HWIO_CGCTL_REG_IN(x)      \
        in_dword_masked(HWIO_CGCTL_REG_ADDR(x), HWIO_CGCTL_REG_RMSK)
#define HWIO_CGCTL_REG_INM(x, m)      \
        in_dword_masked(HWIO_CGCTL_REG_ADDR(x), m)
#define HWIO_CGCTL_REG_OUT(x, v)      \
        out_dword(HWIO_CGCTL_REG_ADDR(x),v)
#define HWIO_CGCTL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CGCTL_REG_ADDR(x),m,v,HWIO_CGCTL_REG_IN(x))
#define HWIO_CGCTL_REG_RAM13_EN_BMSK                                            0x10
#define HWIO_CGCTL_REG_RAM13_EN_SHFT                                             0x4
#define HWIO_CGCTL_REG_RAM1112_EN_BMSK                                           0x8
#define HWIO_CGCTL_REG_RAM1112_EN_SHFT                                           0x3
#define HWIO_CGCTL_REG_BAM_NDP_EN_BMSK                                           0x4
#define HWIO_CGCTL_REG_BAM_NDP_EN_SHFT                                           0x2
#define HWIO_CGCTL_REG_DBM_FSM_EN_BMSK                                           0x2
#define HWIO_CGCTL_REG_DBM_FSM_EN_SHFT                                           0x1
#define HWIO_CGCTL_REG_DBM_REG_EN_BMSK                                           0x1
#define HWIO_CGCTL_REG_DBM_REG_EN_SHFT                                           0x0

#define HWIO_DBG_BUS_REG_ADDR(x)                                          ((x) + 0x0000002c)
#define HWIO_DBG_BUS_REG_RMSK                                              0xf1ff001
#define HWIO_DBG_BUS_REG_POR                                              0x00000000
#define HWIO_DBG_BUS_REG_IN(x)      \
        in_dword_masked(HWIO_DBG_BUS_REG_ADDR(x), HWIO_DBG_BUS_REG_RMSK)
#define HWIO_DBG_BUS_REG_INM(x, m)      \
        in_dword_masked(HWIO_DBG_BUS_REG_ADDR(x), m)
#define HWIO_DBG_BUS_REG_OUT(x, v)      \
        out_dword(HWIO_DBG_BUS_REG_ADDR(x),v)
#define HWIO_DBG_BUS_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_DBG_BUS_REG_ADDR(x),m,v,HWIO_DBG_BUS_REG_IN(x))
#define HWIO_DBG_BUS_REG_GENERAL_DBG_SEL_BMSK                              0xf000000
#define HWIO_DBG_BUS_REG_GENERAL_DBG_SEL_SHFT                                   0x18
#define HWIO_DBG_BUS_REG_DBM_DBG_EN_BMSK                                    0x100000
#define HWIO_DBG_BUS_REG_DBM_DBG_EN_SHFT                                        0x14
#define HWIO_DBG_BUS_REG_DBM_DBG_SEL_BMSK                                    0xff000
#define HWIO_DBG_BUS_REG_DBM_DBG_SEL_SHFT                                        0xc
#define HWIO_DBG_BUS_REG_CTRL_DBG_SEL_BMSK                                       0x1
#define HWIO_DBG_BUS_REG_CTRL_DBG_SEL_SHFT                                       0x0

#define HWIO_SS_PHY_CTRL_ADDR(x)                                          ((x) + 0x00000030)
#define HWIO_SS_PHY_CTRL_RMSK                                             0x1fffffff
#define HWIO_SS_PHY_CTRL_POR                                              0x10210002
#define HWIO_SS_PHY_CTRL_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_CTRL_ADDR(x), HWIO_SS_PHY_CTRL_RMSK)
#define HWIO_SS_PHY_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_CTRL_ADDR(x), m)
#define HWIO_SS_PHY_CTRL_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_CTRL_ADDR(x),v)
#define HWIO_SS_PHY_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_CTRL_ADDR(x),m,v,HWIO_SS_PHY_CTRL_IN(x))
#define HWIO_SS_PHY_CTRL_REF_USE_PAD_BMSK                                 0x10000000
#define HWIO_SS_PHY_CTRL_REF_USE_PAD_SHFT                                       0x1c
#define HWIO_SS_PHY_CTRL_TEST_BURNIN_BMSK                                  0x8000000
#define HWIO_SS_PHY_CTRL_TEST_BURNIN_SHFT                                       0x1b
#define HWIO_SS_PHY_CTRL_TEST_POWERDOWN_BMSK                               0x4000000
#define HWIO_SS_PHY_CTRL_TEST_POWERDOWN_SHFT                                    0x1a
#define HWIO_SS_PHY_CTRL_RTUNE_REQ_BMSK                                    0x2000000
#define HWIO_SS_PHY_CTRL_RTUNE_REQ_SHFT                                         0x19
#define HWIO_SS_PHY_CTRL_LANE0_PWR_PRESENT_BMSK                            0x1000000
#define HWIO_SS_PHY_CTRL_LANE0_PWR_PRESENT_SHFT                                 0x18
#define HWIO_SS_PHY_CTRL_USB2_REF_CLK_EN_BMSK                               0x800000
#define HWIO_SS_PHY_CTRL_USB2_REF_CLK_EN_SHFT                                   0x17
#define HWIO_SS_PHY_CTRL_USB2_REF_CLK_SEL_BMSK                              0x400000
#define HWIO_SS_PHY_CTRL_USB2_REF_CLK_SEL_SHFT                                  0x16
#define HWIO_SS_PHY_CTRL_SSC_REF_CLK_SEL_BMSK                               0x3fe000
#define HWIO_SS_PHY_CTRL_SSC_REF_CLK_SEL_SHFT                                    0xd
#define HWIO_SS_PHY_CTRL_SSC_RANGE_BMSK                                       0x1c00
#define HWIO_SS_PHY_CTRL_SSC_RANGE_SHFT                                          0xa
#define HWIO_SS_PHY_CTRL_REF_USB2_EN_BMSK                                      0x200
#define HWIO_SS_PHY_CTRL_REF_USB2_EN_SHFT                                        0x9
#define HWIO_SS_PHY_CTRL_REF_SS_PHY_EN_BMSK                                    0x100
#define HWIO_SS_PHY_CTRL_REF_SS_PHY_EN_SHFT                                      0x8
#define HWIO_SS_PHY_CTRL_SS_PHY_RESET_BMSK                                      0x80
#define HWIO_SS_PHY_CTRL_SS_PHY_RESET_SHFT                                       0x7
#define HWIO_SS_PHY_CTRL_MPLL_MULTI_BMSK                                        0x7f
#define HWIO_SS_PHY_CTRL_MPLL_MULTI_SHFT                                         0x0

#define HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x)                                  ((x) + 0x00000034)
#define HWIO_SS_PHY_PARAM_CTRL_1_RMSK                                     0xffffffff
#define HWIO_SS_PHY_PARAM_CTRL_1_POR                                      0x0718154a
#define HWIO_SS_PHY_PARAM_CTRL_1_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x), HWIO_SS_PHY_PARAM_CTRL_1_RMSK)
#define HWIO_SS_PHY_PARAM_CTRL_1_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x), m)
#define HWIO_SS_PHY_PARAM_CTRL_1_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x),v)
#define HWIO_SS_PHY_PARAM_CTRL_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_PARAM_CTRL_1_ADDR(x),m,v,HWIO_SS_PHY_PARAM_CTRL_1_IN(x))
#define HWIO_SS_PHY_PARAM_CTRL_1_LANE0_TX_TERM_OFFSET_BMSK                0xf8000000
#define HWIO_SS_PHY_PARAM_CTRL_1_LANE0_TX_TERM_OFFSET_SHFT                      0x1b
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_SWING_FULL_BMSK                        0x7f00000
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_SWING_FULL_SHFT                             0x14
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB_BMSK                          0xfc000
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB_SHFT                              0xe
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB_BMSK                         0x3f00
#define HWIO_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB_SHFT                            0x8
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_LEVEL_BMSK                                 0xf8
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_LEVEL_SHFT                                  0x3
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_BIAS_BMSK                                   0x7
#define HWIO_SS_PHY_PARAM_CTRL_1_LOS_BIAS_SHFT                                   0x0

#define HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x)                                  ((x) + 0x00000038)
#define HWIO_SS_PHY_PARAM_CTRL_2_RMSK                                           0x37
#define HWIO_SS_PHY_PARAM_CTRL_2_POR                                      0x00000004
#define HWIO_SS_PHY_PARAM_CTRL_2_IN(x)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x), HWIO_SS_PHY_PARAM_CTRL_2_RMSK)
#define HWIO_SS_PHY_PARAM_CTRL_2_INM(x, m)      \
        in_dword_masked(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x), m)
#define HWIO_SS_PHY_PARAM_CTRL_2_OUT(x, v)      \
        out_dword(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x),v)
#define HWIO_SS_PHY_PARAM_CTRL_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_PHY_PARAM_CTRL_2_ADDR(x),m,v,HWIO_SS_PHY_PARAM_CTRL_2_IN(x))
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_TX2RX_LOOPBACK_BMSK                      0x20
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_TX2RX_LOOPBACK_SHFT                       0x5
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_EXT_PCLK_REQ_BMSK                        0x10
#define HWIO_SS_PHY_PARAM_CTRL_2_LANE0_EXT_PCLK_REQ_SHFT                         0x4
#define HWIO_SS_PHY_PARAM_CTRL_2_TX_VBOOST_LEVEL_BMSK                            0x7
#define HWIO_SS_PHY_PARAM_CTRL_2_TX_VBOOST_LEVEL_SHFT                            0x0

#define HWIO_SS_CR_PROTOCOL_DATA_IN_ADDR(x)                               ((x) + 0x0000003c)
#define HWIO_SS_CR_PROTOCOL_DATA_IN_RMSK                                      0xffff
#define HWIO_SS_CR_PROTOCOL_DATA_IN_POR                                   0x00000000
#define HWIO_SS_CR_PROTOCOL_DATA_IN_IN(x)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_DATA_IN_ADDR(x), HWIO_SS_CR_PROTOCOL_DATA_IN_RMSK)
#define HWIO_SS_CR_PROTOCOL_DATA_IN_INM(x, m)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_DATA_IN_ADDR(x), m)
#define HWIO_SS_CR_PROTOCOL_DATA_IN_OUT(x, v)      \
        out_dword(HWIO_SS_CR_PROTOCOL_DATA_IN_ADDR(x),v)
#define HWIO_SS_CR_PROTOCOL_DATA_IN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_CR_PROTOCOL_DATA_IN_ADDR(x),m,v,HWIO_SS_CR_PROTOCOL_DATA_IN_IN(x))
#define HWIO_SS_CR_PROTOCOL_DATA_IN_SS_CR_DATA_IN_REG_BMSK                    0xffff
#define HWIO_SS_CR_PROTOCOL_DATA_IN_SS_CR_DATA_IN_REG_SHFT                       0x0

#define HWIO_SS_CR_PROTOCOL_DATA_OUT_ADDR(x)                              ((x) + 0x00000040)
#define HWIO_SS_CR_PROTOCOL_DATA_OUT_RMSK                                     0xffff
#define HWIO_SS_CR_PROTOCOL_DATA_OUT_POR                                  0x00000000
#define HWIO_SS_CR_PROTOCOL_DATA_OUT_IN(x)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_DATA_OUT_ADDR(x), HWIO_SS_CR_PROTOCOL_DATA_OUT_RMSK)
#define HWIO_SS_CR_PROTOCOL_DATA_OUT_INM(x, m)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_DATA_OUT_ADDR(x), m)
#define HWIO_SS_CR_PROTOCOL_DATA_OUT_SS_CR_DATA_OUT_REG_BMSK                  0xffff
#define HWIO_SS_CR_PROTOCOL_DATA_OUT_SS_CR_DATA_OUT_REG_SHFT                     0x0

#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_ADDR(x)                              ((x) + 0x00000044)
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_RMSK                                        0x1
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_POR                                  0x00000000
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_IN(x)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_CAP_ADDR_ADDR(x), HWIO_SS_CR_PROTOCOL_CAP_ADDR_RMSK)
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_INM(x, m)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_CAP_ADDR_ADDR(x), m)
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_OUT(x, v)      \
        out_dword(HWIO_SS_CR_PROTOCOL_CAP_ADDR_ADDR(x),v)
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_CR_PROTOCOL_CAP_ADDR_ADDR(x),m,v,HWIO_SS_CR_PROTOCOL_CAP_ADDR_IN(x))
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_SS_CR_CAP_ADDR_REG_BMSK                     0x1
#define HWIO_SS_CR_PROTOCOL_CAP_ADDR_SS_CR_CAP_ADDR_REG_SHFT                     0x0

#define HWIO_SS_CR_PROTOCOL_CAP_DATA_ADDR(x)                              ((x) + 0x00000048)
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_RMSK                                        0x1
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_POR                                  0x00000000
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_IN(x)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_CAP_DATA_ADDR(x), HWIO_SS_CR_PROTOCOL_CAP_DATA_RMSK)
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_INM(x, m)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_CAP_DATA_ADDR(x), m)
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_OUT(x, v)      \
        out_dword(HWIO_SS_CR_PROTOCOL_CAP_DATA_ADDR(x),v)
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_CR_PROTOCOL_CAP_DATA_ADDR(x),m,v,HWIO_SS_CR_PROTOCOL_CAP_DATA_IN(x))
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_SS_CR_CAP_DATA_REG_BMSK                     0x1
#define HWIO_SS_CR_PROTOCOL_CAP_DATA_SS_CR_CAP_DATA_REG_SHFT                     0x0

#define HWIO_SS_CR_PROTOCOL_READ_ADDR(x)                                  ((x) + 0x0000004c)
#define HWIO_SS_CR_PROTOCOL_READ_RMSK                                            0x1
#define HWIO_SS_CR_PROTOCOL_READ_POR                                      0x00000000
#define HWIO_SS_CR_PROTOCOL_READ_IN(x)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_READ_ADDR(x), HWIO_SS_CR_PROTOCOL_READ_RMSK)
#define HWIO_SS_CR_PROTOCOL_READ_INM(x, m)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_READ_ADDR(x), m)
#define HWIO_SS_CR_PROTOCOL_READ_OUT(x, v)      \
        out_dword(HWIO_SS_CR_PROTOCOL_READ_ADDR(x),v)
#define HWIO_SS_CR_PROTOCOL_READ_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_CR_PROTOCOL_READ_ADDR(x),m,v,HWIO_SS_CR_PROTOCOL_READ_IN(x))
#define HWIO_SS_CR_PROTOCOL_READ_SS_CR_READ_REG_BMSK                             0x1
#define HWIO_SS_CR_PROTOCOL_READ_SS_CR_READ_REG_SHFT                             0x0

#define HWIO_SS_CR_PROTOCOL_WRITE_ADDR(x)                                 ((x) + 0x00000050)
#define HWIO_SS_CR_PROTOCOL_WRITE_RMSK                                           0x1
#define HWIO_SS_CR_PROTOCOL_WRITE_POR                                     0x00000000
#define HWIO_SS_CR_PROTOCOL_WRITE_IN(x)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_WRITE_ADDR(x), HWIO_SS_CR_PROTOCOL_WRITE_RMSK)
#define HWIO_SS_CR_PROTOCOL_WRITE_INM(x, m)      \
        in_dword_masked(HWIO_SS_CR_PROTOCOL_WRITE_ADDR(x), m)
#define HWIO_SS_CR_PROTOCOL_WRITE_OUT(x, v)      \
        out_dword(HWIO_SS_CR_PROTOCOL_WRITE_ADDR(x),v)
#define HWIO_SS_CR_PROTOCOL_WRITE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_SS_CR_PROTOCOL_WRITE_ADDR(x),m,v,HWIO_SS_CR_PROTOCOL_WRITE_IN(x))
#define HWIO_SS_CR_PROTOCOL_WRITE_SS_CR_WRITE_REG_BMSK                           0x1
#define HWIO_SS_CR_PROTOCOL_WRITE_SS_CR_WRITE_REG_SHFT                           0x0

#define HWIO_SS_STATUS_READ_ONLY_ADDR(x)                                  ((x) + 0x00000054)
#define HWIO_SS_STATUS_READ_ONLY_RMSK                                            0x3
#define HWIO_SS_STATUS_READ_ONLY_POR                                      0x00000000
#define HWIO_SS_STATUS_READ_ONLY_IN(x)      \
        in_dword_masked(HWIO_SS_STATUS_READ_ONLY_ADDR(x), HWIO_SS_STATUS_READ_ONLY_RMSK)
#define HWIO_SS_STATUS_READ_ONLY_INM(x, m)      \
        in_dword_masked(HWIO_SS_STATUS_READ_ONLY_ADDR(x), m)
#define HWIO_SS_STATUS_READ_ONLY_REF_CLKREQ_N_BMSK                               0x2
#define HWIO_SS_STATUS_READ_ONLY_REF_CLKREQ_N_SHFT                               0x1
#define HWIO_SS_STATUS_READ_ONLY_RTUNE_ACK_BMSK                                  0x1
#define HWIO_SS_STATUS_READ_ONLY_RTUNE_ACK_SHFT                                  0x0

#define HWIO_PWR_EVNT_IRQ_STAT_ADDR(x)                                    ((x) + 0x00000058)
#define HWIO_PWR_EVNT_IRQ_STAT_RMSK                                             0x3f
#define HWIO_PWR_EVNT_IRQ_STAT_POR                                        0x00000000
#define HWIO_PWR_EVNT_IRQ_STAT_IN(x)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x), HWIO_PWR_EVNT_IRQ_STAT_RMSK)
#define HWIO_PWR_EVNT_IRQ_STAT_INM(x, m)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x), m)
#define HWIO_PWR_EVNT_IRQ_STAT_OUT(x, v)      \
        out_dword(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x),v)
#define HWIO_PWR_EVNT_IRQ_STAT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PWR_EVNT_IRQ_STAT_ADDR(x),m,v,HWIO_PWR_EVNT_IRQ_STAT_IN(x))
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_IRQ_STAT_BMSK                         0x20
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_OUT_L2_IRQ_STAT_SHFT                          0x5
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_IRQ_STAT_BMSK                          0x10
#define HWIO_PWR_EVNT_IRQ_STAT_LPM_IN_L2_IRQ_STAT_SHFT                           0x4
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_OUT_P3_IRQ_STAT_BMSK                    0x8
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_OUT_P3_IRQ_STAT_SHFT                    0x3
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_IN_P3_IRQ_STAT_BMSK                     0x4
#define HWIO_PWR_EVNT_IRQ_STAT_POWERDOWN_IN_P3_IRQ_STAT_SHFT                     0x2
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_REQ_IN_P3_IRQ_STAT_BMSK                       0x2
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_REQ_IN_P3_IRQ_STAT_SHFT                       0x1
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_GATE_IN_P3_IRQ_STAT_BMSK                      0x1
#define HWIO_PWR_EVNT_IRQ_STAT_CLK_GATE_IN_P3_IRQ_STAT_SHFT                      0x0

#define HWIO_PWR_EVNT_IRQ_MASK_ADDR(x)                                    ((x) + 0x0000005c)
#define HWIO_PWR_EVNT_IRQ_MASK_RMSK                                             0x3f
#define HWIO_PWR_EVNT_IRQ_MASK_POR                                        0x00000000
#define HWIO_PWR_EVNT_IRQ_MASK_IN(x)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x), HWIO_PWR_EVNT_IRQ_MASK_RMSK)
#define HWIO_PWR_EVNT_IRQ_MASK_INM(x, m)      \
        in_dword_masked(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x), m)
#define HWIO_PWR_EVNT_IRQ_MASK_OUT(x, v)      \
        out_dword(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x),v)
#define HWIO_PWR_EVNT_IRQ_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PWR_EVNT_IRQ_MASK_ADDR(x),m,v,HWIO_PWR_EVNT_IRQ_MASK_IN(x))
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_IRQ_MASK_BMSK                         0x20
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_IRQ_MASK_SHFT                          0x5
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_IRQ_MASK_BMSK                          0x10
#define HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_IRQ_MASK_SHFT                           0x4
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_OUT_P3_IRQ_MASK_BMSK                    0x8
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_OUT_P3_IRQ_MASK_SHFT                    0x3
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_IN_P3_IRQ_MASK_BMSK                     0x4
#define HWIO_PWR_EVNT_IRQ_MASK_POWERDOWN_IN_P3_IRQ_MASK_SHFT                     0x2
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_REQ_IN_P3_IRQ_MASK_BMSK                       0x2
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_REQ_IN_P3_IRQ_MASK_SHFT                       0x1
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_GATE_IN_P3_IRQ_MASK_BMSK                      0x1
#define HWIO_PWR_EVNT_IRQ_MASK_CLK_GATE_IN_P3_IRQ_MASK_SHFT                      0x0

#define HWIO_HW_SW_EVT_CTRL_REG_ADDR(x)                                   ((x) + 0x00000060)
#define HWIO_HW_SW_EVT_CTRL_REG_RMSK                                           0x131
#define HWIO_HW_SW_EVT_CTRL_REG_POR                                       0x00000001
#define HWIO_HW_SW_EVT_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x), HWIO_HW_SW_EVT_CTRL_REG_RMSK)
#define HWIO_HW_SW_EVT_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x), m)
#define HWIO_HW_SW_EVT_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x),v)
#define HWIO_HW_SW_EVT_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HW_SW_EVT_CTRL_REG_ADDR(x),m,v,HWIO_HW_SW_EVT_CTRL_REG_IN(x))
#define HWIO_HW_SW_EVT_CTRL_REG_SW_EVT_MUX_SEL_BMSK                            0x100
#define HWIO_HW_SW_EVT_CTRL_REG_SW_EVT_MUX_SEL_SHFT                              0x8
#define HWIO_HW_SW_EVT_CTRL_REG_HW_EVT_MUX_CTRL_BMSK                            0x30
#define HWIO_HW_SW_EVT_CTRL_REG_HW_EVT_MUX_CTRL_SHFT                             0x4
#define HWIO_HW_SW_EVT_CTRL_REG_EVENT_BUS_HALT_BMSK                              0x1
#define HWIO_HW_SW_EVT_CTRL_REG_EVENT_BUS_HALT_SHFT                              0x0

#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x)                             ((x) + 0x00000064)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_RMSK                                       0x7
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_POR                                 0x00000000
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_IN(x)      \
        in_dword_masked(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x), HWIO_VMIDMT_AMEMTYPE_CTRL_REG_RMSK)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_INM(x, m)      \
        in_dword_masked(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x), m)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_OUT(x, v)      \
        out_dword(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x),v)
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_VMIDMT_AMEMTYPE_CTRL_REG_ADDR(x),m,v,HWIO_VMIDMT_AMEMTYPE_CTRL_REG_IN(x))
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_VMIDMT_AMEMTYPE_VALUE_BMSK                 0x7
#define HWIO_VMIDMT_AMEMTYPE_CTRL_REG_VMIDMT_AMEMTYPE_VALUE_SHFT                 0x0

#define HWIO_FLADJ_30MHZ_REG_ADDR(x)                                      ((x) + 0x00000068)
#define HWIO_FLADJ_30MHZ_REG_RMSK                                               0x3f
#define HWIO_FLADJ_30MHZ_REG_POR                                          0x00000020
#define HWIO_FLADJ_30MHZ_REG_IN(x)      \
        in_dword_masked(HWIO_FLADJ_30MHZ_REG_ADDR(x), HWIO_FLADJ_30MHZ_REG_RMSK)
#define HWIO_FLADJ_30MHZ_REG_INM(x, m)      \
        in_dword_masked(HWIO_FLADJ_30MHZ_REG_ADDR(x), m)
#define HWIO_FLADJ_30MHZ_REG_OUT(x, v)      \
        out_dword(HWIO_FLADJ_30MHZ_REG_ADDR(x),v)
#define HWIO_FLADJ_30MHZ_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_FLADJ_30MHZ_REG_ADDR(x),m,v,HWIO_FLADJ_30MHZ_REG_IN(x))
#define HWIO_FLADJ_30MHZ_REG_FLADJ_30MHZ_VALUE_BMSK                             0x3f
#define HWIO_FLADJ_30MHZ_REG_FLADJ_30MHZ_VALUE_SHFT                              0x0

#define HWIO_M_AW_USER_REG_ADDR(x)                                        ((x) + 0x0000006c)
#define HWIO_M_AW_USER_REG_RMSK                                                0x97f
#define HWIO_M_AW_USER_REG_POR                                            0x00000122
#define HWIO_M_AW_USER_REG_IN(x)      \
        in_dword_masked(HWIO_M_AW_USER_REG_ADDR(x), HWIO_M_AW_USER_REG_RMSK)
#define HWIO_M_AW_USER_REG_INM(x, m)      \
        in_dword_masked(HWIO_M_AW_USER_REG_ADDR(x), m)
#define HWIO_M_AW_USER_REG_OUT(x, v)      \
        out_dword(HWIO_M_AW_USER_REG_ADDR(x),v)
#define HWIO_M_AW_USER_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_M_AW_USER_REG_ADDR(x),m,v,HWIO_M_AW_USER_REG_IN(x))
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_1_SEL_BMSK                               0x800
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_1_SEL_SHFT                                 0xb
#define HWIO_M_AW_USER_REG_AW_NOALLOACATE_BMSK                                 0x100
#define HWIO_M_AW_USER_REG_AW_NOALLOACATE_SHFT                                   0x8
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_BMSK                                      0x70
#define HWIO_M_AW_USER_REG_AW_MEMTYPE_SHFT                                       0x4
#define HWIO_M_AW_USER_REG_AW_CACHE_BMSK                                         0xf
#define HWIO_M_AW_USER_REG_AW_CACHE_SHFT                                         0x0

#define HWIO_M_AR_USER_REG_ADDR(x)                                        ((x) + 0x00000070)
#define HWIO_M_AR_USER_REG_RMSK                                                0x97f
#define HWIO_M_AR_USER_REG_POR                                            0x00000122
#define HWIO_M_AR_USER_REG_IN(x)      \
        in_dword_masked(HWIO_M_AR_USER_REG_ADDR(x), HWIO_M_AR_USER_REG_RMSK)
#define HWIO_M_AR_USER_REG_INM(x, m)      \
        in_dword_masked(HWIO_M_AR_USER_REG_ADDR(x), m)
#define HWIO_M_AR_USER_REG_OUT(x, v)      \
        out_dword(HWIO_M_AR_USER_REG_ADDR(x),v)
#define HWIO_M_AR_USER_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_M_AR_USER_REG_ADDR(x),m,v,HWIO_M_AR_USER_REG_IN(x))
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_1_SEL_BMSK                               0x800
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_1_SEL_SHFT                                 0xb
#define HWIO_M_AR_USER_REG_AR_NOALLOACATE_BMSK                                 0x100
#define HWIO_M_AR_USER_REG_AR_NOALLOACATE_SHFT                                   0x8
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_BMSK                                      0x70
#define HWIO_M_AR_USER_REG_AR_MEMTYPE_SHFT                                       0x4
#define HWIO_M_AR_USER_REG_AR_CACHE_BMSK                                         0xf
#define HWIO_M_AR_USER_REG_AR_CACHE_SHFT                                         0x0

#define HWIO_QSCRTCH_REG_n_ADDR(base,n)                                   ((base) + 0x00000074 + 0x4 * (n))
#define HWIO_QSCRTCH_REG_n_RMSK                                           0xffffffff
#define HWIO_QSCRTCH_REG_n_MAXn                                                    2
#define HWIO_QSCRTCH_REG_n_POR                                            0x00000000
#define HWIO_QSCRTCH_REG_n_INI(base,n)        \
        in_dword_masked(HWIO_QSCRTCH_REG_n_ADDR(base,n), HWIO_QSCRTCH_REG_n_RMSK)
#define HWIO_QSCRTCH_REG_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_QSCRTCH_REG_n_ADDR(base,n), mask)
#define HWIO_QSCRTCH_REG_n_OUTI(base,n,val)    \
        out_dword(HWIO_QSCRTCH_REG_n_ADDR(base,n),val)
#define HWIO_QSCRTCH_REG_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_QSCRTCH_REG_n_ADDR(base,n),mask,val,HWIO_QSCRTCH_REG_n_INI(base,n))
#define HWIO_QSCRTCH_REG_n_QSCRTCH_REG_BMSK                               0xffffffff
#define HWIO_QSCRTCH_REG_n_QSCRTCH_REG_SHFT                                      0x0


#endif /* __DWC_USB30_QSCRATCH_HWIO_H__ */
