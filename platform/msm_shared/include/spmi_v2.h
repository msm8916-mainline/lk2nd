/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
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

#ifndef __SPMI_V2_H
#define __SPMI_V2_H

#define PMIC_ARB_CORE_REG_BASE               (SPMI_BASE + 0x00400000)
#define PMIC_ARB_OBS_CORE_REG_BASE           (SPMI_BASE + 0x00C00000)
#define PMIC_ARB_CHNLn_CONFIG(x)             (PMIC_ARB_CORE_REG_BASE + 0x00000004 + (x) * 0x8000)
#define PMIC_ARB_OBS_CHNLn_CONFIG(x)         (PMIC_ARB_OBS_CORE_REG_BASE + 0x00000004 + (x) * 0x8000)
#define PMIC_ARB_CHNLn_STATUS(x)             (PMIC_ARB_CORE_REG_BASE + 0x00000008 + (x) * 0x8000)
#define PMIC_ARB_OBS_CHNLn_STATUS(x)         (PMIC_ARB_OBS_CORE_REG_BASE + 0x00000008 + (x) * 0x8000)
#define PMIC_ARB_CHNLn_WDATA(x, n)           (PMIC_ARB_CORE_REG_BASE + 0x00000010 + (x) * 0x8000 + (n) * 4)
#define PMIC_ARB_CHNLn_RDATA(x,n)            (PMIC_ARB_CORE_REG_BASE + 0x00000018 + (x) * 0x8000 + (n) * 4)
#define PMIC_ARB_OBS_CHNLn_RDATA(x,n)        (PMIC_ARB_OBS_CORE_REG_BASE + 0x00000018 + (x) * 0x8000 + (n) * 4)

#define PMIC_ARB_REG_CHLN(n)                 (PMIC_ARB_CORE + 0x00000800 + 0x4 * (n))
#define PMIC_ARB_CHNLn_CMD0(x)               (PMIC_ARB_CORE_REG_BASE + (x) * 0x8000)
#define PMIC_ARB_OBS_CHNLn_CMD0(x)           (PMIC_ARB_OBS_CORE_REG_BASE + (x) * 0x8000)

/* PIC Registers */
#define SPMI_PIC_OWNERm_ACC_STATUSn(m, n)    (SPMI_PIC_BASE + 0x00100000 + 0x1000 * (m) + 0x4 * (n))
#define SPMI_PIC_ACC_ENABLEn(n)              (SPMI_PIC_BASE + 0x1000 * n )
#define SPMI_PIC_IRQ_STATUSn(n)              (SPMI_PIC_BASE + 0x00000004 + 0x1000 * (n))
#define SPMI_PIC_IRQ_CLEARn(n)               (SPMI_PIC_BASE + 0x00000008 + 0x1000 * (n))

#endif
