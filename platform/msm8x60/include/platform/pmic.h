/*
 * * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#ifndef __PLATFORM_MSM8X60_PMIC_H
#define __PLATFORM_MSM8X60_PMIC_H

/* PMIC 8901 LDO Module defines */
#define PM8901_LDO_BASE (0x2F)

#define PM8901_LDO_L0           (PM8901_LDO_BASE + 0x00)
#define PM8901_LDO_L0_TEST_BANK     (PM8901_LDO_BASE + 0x01)
#define PM8901_LDO_L1           (PM8901_LDO_BASE + 0x02)
#define PM8901_LDO_L1_TEST_BANK     (PM8901_LDO_BASE + 0x03)
#define PM8901_LDO_L2           (PM8901_LDO_BASE + 0x04)
#define PM8901_LDO_L2_TEST_BANK     (PM8901_LDO_BASE + 0x05)
#define PM8901_LDO_L3           (PM8901_LDO_BASE + 0x06)
#define PM8901_LDO_L3_TEST_BANK     (PM8901_LDO_BASE + 0x07)
#define PM8901_LDO_L4           (PM8901_LDO_BASE + 0x08)
#define PM8901_LDO_L4_TEST_BANK     (PM8901_LDO_BASE + 0x09)
#define PM8901_LDO_L5           (PM8901_LDO_BASE + 0x0A)
#define PM8901_LDO_L5_TEST_BANK     (PM8901_LDO_BASE + 0x0B)
#define PM8901_LDO_L6           (PM8901_LDO_BASE + 0x0C)
#define PM8901_LDO_L6_TEST_BANK     (PM8901_LDO_BASE + 0x0D)
#define PM8901_LDO_L7           (PM8901_LDO_BASE + 0x0E)
#define PM8901_LDO_L7_TEST_BANK     (PM8901_LDO_BASE + 0x0F)
#define PM8901_PMR_7            (0xAD)

#define PM8901_LDO_TEST_BANK(n) ((n)<<4)

#define PM8901_LDO_CTL_ENABLE__S    (7)
#define PM8901_LDO_CTL_PULL_DOWN__S (6)
#define PM8901_LDO_CTL_MODE__S      (5)
/* LDO CTL */
#define LDO_CTL_ENABLE_MASK     (0x80)
#define LDO_CTL_PULL_DOWN_MASK      (0x40)
#define LDO_CTL_NORMAL_POWER_MODE_MASK  (0x20)
#define LDO_CTL_VOLTAGE_SET_MASK    (0x1F)

/* LDO TEST BANK 2 */
#define LDO_TEST_RANGE_SELECT_MASK (0x01)

/* LDO TEST BANK 4 */
#define LDO_TEST_OUTPUT_RANGE_MASK (0x01)

/* LDO TEST BANK 5 */
#define LDO_TEST_XO_EN_ALL_MASK (0x1F)

/* PMIC 8058 defines */
#define LPG_CTL_0         (0x13C)
#define LPG_CTL_1         (0x13D)
#define LPG_CTL_2         (0x13E)
#define LPG_CTL_3         (0x13F)
#define LPG_CTL_4         (0x140)
#define LPG_CTL_5         (0x141)
#define LPG_CTL_6         (0x142)
#define LPG_BANK_SEL      (0x143)
#define LPG_BANK_ENABLE   (0x144)
#define GPIO24_GPIO_CNTRL (0x167)
#define GPIO25_GPIO_CNTRL (0x168)

#endif
