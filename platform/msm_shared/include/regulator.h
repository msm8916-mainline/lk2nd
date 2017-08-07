/* Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Fundation, Inc. nor the names of its
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

#ifndef __REGULATOR_H
#define __REGULATOR_H

#include <stdint.h>

#define GENERIC_DISABLE 0
#define GENERIC_ENABLE  1
#define SW_MODE_LDO_IPEAK 1
#define LDOA_RES_TYPE 0x616F646C //aodl
#define SMPS_RES_TYPE 0x61706D73 //apms

#define KEY_SOFTWARE_ENABLE                0x6E657773 // swen - software enable
#define KEY_LDO_SOFTWARE_MODE              0X646D736C // lsmd - LDO software mode
#define KEY_SMPS_SOFTWARE_MODE             0X646D7373 // ssmd - SMPS software mode
#define KEY_PIN_CTRL_ENABLE                0x6E656370 //pcen - pin control enable
#define KEY_PIN_CTRL_POWER_MODE            0x646d6370 // pcmd - pin control mode
#define KEY_CURRENT                        0x616D //ma
#define KEY_MICRO_VOLT                     0x7675 //uv
#define KEY_FREQUENCY                      0x71657266 //freq
#define KEY_FREQUENCY_REASON               0x6E736572 //resn
#define KEY_FOLLOW_QUIET_MODE              0x6D71 //qm
#define KEY_HEAD_ROOM                      0x7268 // hr
#define KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY 0x62636370 // pccb - clk buffer pin control
#define KEY_BYPASS_ALLOWED_KEY             0x61707962 //bypa - bypass allowed
#define KEY_CORNER_LEVEL_KEY               0x6E726F63 // corn - coner voltage
#define KEY_ACTIVE_FLOOR                   0x636676
#define GENERIC_DISABLE 0
#define GENERIC_ENABLE  1
#define SW_MODE_LDO_IPEAK 1
#define LDOA_RES_TYPE 0x616F646C //aodl
#define SMPS_RES_TYPE 0x61706D73 //apms

/* RPM clock helper macros */
#define RPM_CLK_BUFFER_A_REQ                   0x616B6C63 //aklc
#define RPM_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY 0x62636370 //pccb - clk buffer pin control
#define LNBB_CLK_ID                            0x8
#define RPM_CLK_BUFFER_PIN_CONTROL_ENABLE_NONE 0x0

#define REG_LDO1	BIT(0)
#define REG_LDO2	BIT(1)
#define REG_LDO5	BIT(4)
#define REG_LDO6	BIT(5)
#define REG_LDO11	BIT(10)
#define REG_LDO12	BIT(11)
#define REG_LDO14	BIT(13)
#define REG_LDO17	BIT(16)
#define REG_LDO18	BIT(17)
#define REG_LDO28	BIT(27)
#define REG_SMPS3	BIT(7)
#define REG_LDO3	BIT(2)
#define REG_LDO4	BIT(4)

void regulator_enable(uint32_t enable);
void regulator_disable(uint32_t enable);

#endif
