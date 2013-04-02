/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation, Inc. nor the names of its
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

#ifndef __PLATFORM_COPPER_GPIO_H
#define __PLATFORM_COPPER_GPIO_H

/* GPIO TLMM: Direction */
#define GPIO_INPUT      0
#define GPIO_OUTPUT     1

/* GPIO TLMM: Pullup/Pulldown */
#define GPIO_NO_PULL    0
#define GPIO_PULL_DOWN  1
#define GPIO_KEEPER     2
#define GPIO_PULL_UP    3

/* GPIO TLMM: Drive Strength */
#define GPIO_2MA        0
#define GPIO_4MA        1
#define GPIO_6MA        2
#define GPIO_8MA        3
#define GPIO_10MA       4
#define GPIO_12MA       5
#define GPIO_14MA       6
#define GPIO_16MA       7

/* GPIO TLMM: Status */
#define GPIO_ENABLE     0
#define GPIO_DISABLE    1

#define TLMM_PULL_MASK  0x3
#define TLMM_HDRV_MASK  0x7

enum {
	TLMM_CUR_VAL_16MA = 0x7,
	TLMM_CUR_VAL_10MA = 0x4,
} tlmm_drive_config;

enum {
	TLMM_PULL_UP = 0x3,
	TLMM_NO_PULL = 0x0,
} tlmm_pull_values;

enum {
	SDC1_DATA_HDRV_CTL_OFF = 0,
	SDC1_CMD_HDRV_CTL_OFF  = 3,
	SDC1_CLK_HDRV_CTL_OFF  = 6,
	SDC1_DATA_PULL_CTL_OFF = 9,
	SDC1_CMD_PULL_CTL_OFF  = 11,
	SDC1_CLK_PULL_CTL_OFF  = 13,
} tlmm_drv_ctrl;

struct tlmm_cfgs {
	uint32_t off;
	uint8_t val;
	uint8_t mask;
};

void gpio_config_uart_dm(uint8_t id);
void gpio_config_blsp_i2c(uint8_t, uint8_t);
void tlmm_set_hdrive_ctrl(struct tlmm_cfgs *, uint8_t);
void tlmm_set_pull_ctrl(struct tlmm_cfgs *, uint8_t);
#endif
