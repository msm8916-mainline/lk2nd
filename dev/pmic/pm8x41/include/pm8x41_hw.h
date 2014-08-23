/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation, Inc. nor the names of its
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

#ifndef _PM8x41_HW_H_
#define _PM8x41_HW_H_

/* SMBB Registers */
#define SMBB_MISC_BOOT_DONE                   0x1642

/* SMBB bit values */
#define BOOT_DONE_BIT                         7

#define REVID_REVISION4                       0x103

/* LPG Registers */
#define LPG_SLAVE_ID                  	0x10000	/* slave_id == 1 */
#define LPG_PERIPHERAL_BASE		(0x0B100 | LPG_SLAVE_ID)
/* Peripheral base address for LPG channel */
#define LPG_N_PERIPHERAL_BASE(x)            (LPG_PERIPHERAL_BASE + ((x) - 1) * 0x100)

/* GPIO Registers */
#define GPIO_PERIPHERAL_BASE                  0xC000
/* Peripheral base address for GPIO_X */
#define GPIO_N_PERIPHERAL_BASE(x)            (GPIO_PERIPHERAL_BASE + ((x) - 1) * 0x100)

/* Register offsets within GPIO */
#define GPIO_STATUS                           0x08
#define GPIO_MODE_CTL                         0x40
#define GPIO_DIG_VIN_CTL                      0x41
#define GPIO_DIG_PULL_CTL                     0x42
#define GPIO_DIG_OUT_CTL                      0x45
#define GPIO_EN_CTL                           0x46

/* GPIO bit values */
#define PERPH_EN_BIT                          7
#define GPIO_STATUS_VAL_BIT                   0


/* PON Peripheral registers */
#define PON_PON_REASON1                       0x808
#define PON_WARMBOOT_STATUS1                  0x80A
#define PON_WARMBOOT_STATUS2                  0x80B
#define PON_POFF_REASON1                      0x80C
#define PON_POFF_REASON2                      0x80D
#define PON_INT_RT_STS                        0x810
#define PON_INT_SET_TYPE                      0x811
#define PON_INT_POLARITY_HIGH                 0x812
#define PON_INT_POLARITY_LOW                  0x813
#define PON_INT_LATCHED_CLR                   0x814
#define PON_INT_EN_SET                        0x815
#define PON_INT_LATCHED_STS                   0x818
#define PON_INT_PENDING_STS                   0x819
#define PON_RESIN_N_RESET_S1_TIMER            0x844  /* bits 0:3  : S1_TIMER */
#define PON_RESIN_N_RESET_S2_TIMER            0x845  /* bits 0:2  : S2_TIMER */
#define PON_RESIN_N_RESET_S2_CTL              0x846  /* bit 7: S2_RESET_EN, bit 0:3 : RESET_TYPE  */
#define PON_PS_HOLD_RESET_CTL                 0x85A  /* bit 7: S2_RESET_EN, bit 0:3 : RESET_TYPE  */
#define PON_PS_HOLD_RESET_CTL2                0x85B
#define PMIC_WD_RESET_S2_CTL2                 0x857

/* PON Peripheral register bit values */
#define RESIN_ON_INT_BIT                      1
#define KPDPWR_ON_INT_BIT                     0
#define RESIN_BARK_INT_BIT                    4
#define S2_RESET_EN_BIT                       7

#define S2_RESET_TYPE_WARM                    0x1
#define PON_RESIN_N_RESET_S2_TIMER_MAX_VALUE  0x7

/* MPP registers */
#define MPP_DIG_VIN_CTL                       0x41
#define MPP_MODE_CTL                          0x40
#define MPP_EN_CTL                            0x46

#define MPP_MODE_CTL_MODE_SHIFT               4
#define MPP_EN_CTL_ENABLE_SHIFT               7

/* MVS registers */
#define MVS_EN_CTL                            0x46
#define MVS_EN_CTL_ENABLE_SHIFT               7

void pm8x41_reg_write(uint32_t addr, uint8_t val);
uint8_t pm8x41_reg_read(uint32_t addr);

/* SPMI Macros */
#define REG_READ(_a)        pm8x41_reg_read(_a)
#define REG_WRITE(_a, _v)   pm8x41_reg_write(_a, _v)

#define REG_OFFSET(_addr)   ((_addr) & 0xFF)
#define PERIPH_ID(_addr)    (((_addr) & 0xFF00) >> 8)
#define SLAVE_ID(_addr)     ((_addr) >> 16)

#define LDO_RANGE_CTRL                        0x40
#define LDO_STEP_CTRL                         0x41
#define LDO_POWER_MODE                        0x45
#define LDO_EN_CTL_REG                        0x46

/* USB3 phy clock */
#define DIFF_CLK1_EN_CTL                      0x5746
#define DIFF_CLK1_EN_BIT                      7

#define LNBB_CLK_EN_CTL                      0x5246
#define LNBB_CLK_EN_BIT                      7

/* SMBB registers */
#define PM8XXX_IBAT_ATC_A                     0x1054
#define PM8XXX_VBAT_DET                       0x105D
#define PM8XXX_SEC_ACCESS                     0x10D0
#define PM8XXX_COMP_OVR0                      0x10ED
#define PM8XXX_VCP                            0x1247
#define PM8XXX_TRKL_CHG_TEST                  0x10E2
#define PM8XXX_VBAT_IN_TSTS                   0x1010

/* Macros for broken battery */
#define VBAT_DET_LO_4_30V                     0x35
#define SEC_ACCESS                            0xa5
#define OVR0_DIS_VTRKL_FAULT                  0x08
#define CHG_TRICKLE_FORCED_ON                 0x01
#define VBAT_DET_HI_RT_STS                    0x02
#define VCP_ENABLE                            0x01

int pm8xxx_is_battery_broken(void);

#endif
