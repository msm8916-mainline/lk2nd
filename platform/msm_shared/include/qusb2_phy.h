/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.

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
#ifndef _QUSB2_PHY_H_
#define _QUSB2_PHY_H_

#include <platform/iomap.h>

void qusb2_phy_reset(void);

#define QUSB2PHY_PLL_LOCK        0x20

#define QUSB2PHY_PORT_POWERDOWN     (QUSB2_PHY_BASE + 0x000000B4)
#define QUSB2PHY_PORT_UTMI_CTRL2    (QUSB2_PHY_BASE + 0x000000C4)
#define QUSB2PHY_PLL_TEST           (QUSB2_PHY_BASE + 0x00000004)
#define QUSB2PHY_PLL_TUNE           (QUSB2_PHY_BASE + 0x00000008)
#define QUSB2PHY_PLL_USER_CTL1      (QUSB2_PHY_BASE + 0x0000000C)
#define QUSB2PHY_PLL_USER_CTL2      (QUSB2_PHY_BASE + 0x00000010)
#define QUSB2PHY_PORT_TUNE1         (QUSB2_PHY_BASE + 0x00000080)
#define QUSB2PHY_PORT_TUNE2         (QUSB2_PHY_BASE + 0x00000084)
#define QUSB2PHY_PORT_TUNE3         (QUSB2_PHY_BASE + 0x00000088)
#define QUSB2PHY_PORT_TUNE4         (QUSB2_PHY_BASE + 0x0000008C)
#define QUSB2PHY_PORT_TUNE5         (QUSB2_PHY_BASE + 0x00000090)
#define QUSB2PHY_PORT_TEST2         (QUSB2_PHY_BASE + 0x0000009C)
#define QUSB2PHY_PLL_PWR_CTL        (QUSB2_PHY_BASE + 0x00000018)
#define QUSB2PHY_PLL_AUTOPGM_CTL1   (QUSB2_PHY_BASE + 0x0000001C)
#define QUSB2PHY_PLL_STATUS         (QUSB2_PHY_BASE + 0x00000038)


#endif
