/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __PLATFORM_MSM7X30_GPIO_HW_H
#define __PLATFORM_MSM7X30_GPIO_HW_H

#define MSM_GPIO1_BASE 0xAC001000
#define MSM_GPIO2_BASE 0xAC101000

#define GPIO1_REG(off) (MSM_GPIO1_BASE + (off))
#define GPIO2_REG(off) (MSM_GPIO2_BASE + 0x400 + (off))

/* output value */
#define GPIO_OUT_0         GPIO1_REG(0x00)	/* gpio  15-0   */
#define GPIO_OUT_1         GPIO2_REG(0x00)	/* gpio  43-16  */
#define GPIO_OUT_2         GPIO1_REG(0x04)	/* gpio  67-44  */
#define GPIO_OUT_3         GPIO1_REG(0x08)	/* gpio  94-68  */
#define GPIO_OUT_4         GPIO1_REG(0x0C)	/* gpio 106-95  */
#define GPIO_OUT_5         GPIO1_REG(0x50)	/* gpio 133-107 */
#define GPIO_OUT_6         GPIO1_REG(0xC4)	/* gpio 150-134 */
#define GPIO_OUT_7         GPIO1_REG(0x214)	/* gpio 181-151 */

/* same pin map as above, output enable */
#define GPIO_OE_0          GPIO1_REG(0x10)
#define GPIO_OE_1          GPIO2_REG(0x08)
#define GPIO_OE_2          GPIO1_REG(0x14)
#define GPIO_OE_3          GPIO1_REG(0x18)
#define GPIO_OE_4          GPIO1_REG(0x1C)
#define GPIO_OE_5          GPIO1_REG(0x54)
#define GPIO_OE_6          GPIO1_REG(0xC8)
#define GPIO_OE_7          GPIO1_REG(0x218)

/* same pin map as above, input read */
#define GPIO_IN_0          GPIO1_REG(0x34)
#define GPIO_IN_1          GPIO2_REG(0x20)
#define GPIO_IN_2          GPIO1_REG(0x38)
#define GPIO_IN_3          GPIO1_REG(0x3C)
#define GPIO_IN_4          GPIO1_REG(0x40)
#define GPIO_IN_5          GPIO1_REG(0x44)
#define GPIO_IN_6          GPIO1_REG(0xCC)
#define GPIO_IN_7          GPIO1_REG(0x21C)

/* same pin map as above, 1=edge 0=level interrup */
#define GPIO_INT_EDGE_0    GPIO1_REG(0x60)
#define GPIO_INT_EDGE_1    GPIO2_REG(0x50)
#define GPIO_INT_EDGE_2    GPIO1_REG(0x64)
#define GPIO_INT_EDGE_3    GPIO1_REG(0x68)
#define GPIO_INT_EDGE_4    GPIO1_REG(0x6C)
#define GPIO_INT_EDGE_5    GPIO1_REG(0xC0)
#define GPIO_INT_EDGE_6    GPIO1_REG(0xD0)
#define GPIO_INT_EDGE_7    GPIO1_REG(0x240)

/* same pin map as above, 1=positive 0=negative */
#define GPIO_INT_POS_0     GPIO1_REG(0x70)
#define GPIO_INT_POS_1     GPIO2_REG(0x58)
#define GPIO_INT_POS_2     GPIO1_REG(0x74)
#define GPIO_INT_POS_3     GPIO1_REG(0x78)
#define GPIO_INT_POS_4     GPIO1_REG(0x7C)
#define GPIO_INT_POS_5     GPIO1_REG(0xBC)
#define GPIO_INT_POS_6     GPIO1_REG(0xD4)
#define GPIO_INT_POS_7     GPIO1_REG(0x228)

/* same pin map as above, interrupt enable */
#define GPIO_INT_EN_0      GPIO1_REG(0x80)
#define GPIO_INT_EN_1      GPIO2_REG(0x60)
#define GPIO_INT_EN_2      GPIO1_REG(0x84)
#define GPIO_INT_EN_3      GPIO1_REG(0x88)
#define GPIO_INT_EN_4      GPIO1_REG(0x8C)
#define GPIO_INT_EN_5      GPIO1_REG(0xB8)
#define GPIO_INT_EN_6      GPIO1_REG(0xD8)
#define GPIO_INT_EN_7      GPIO1_REG(0x22C)

/* same pin map as above, write 1 to clear interrupt */
#define GPIO_INT_CLEAR_0   GPIO1_REG(0x90)
#define GPIO_INT_CLEAR_1   GPIO2_REG(0x68)
#define GPIO_INT_CLEAR_2   GPIO1_REG(0x94)
#define GPIO_INT_CLEAR_3   GPIO1_REG(0x98)
#define GPIO_INT_CLEAR_4   GPIO1_REG(0x9C)
#define GPIO_INT_CLEAR_5   GPIO1_REG(0xB4)
#define GPIO_INT_CLEAR_6   GPIO1_REG(0xDC)
#define GPIO_INT_CLEAR_7   GPIO1_REG(0x230)

/* same pin map as above, 1=interrupt pending */
#define GPIO_INT_STATUS_0  GPIO1_REG(0xA0)
#define GPIO_INT_STATUS_1  GPIO2_REG(0x70)
#define GPIO_INT_STATUS_2  GPIO1_REG(0xA4)
#define GPIO_INT_STATUS_3  GPIO1_REG(0xA8)
#define GPIO_INT_STATUS_4  GPIO1_REG(0xAC)
#define GPIO_INT_STATUS_5  GPIO1_REG(0xB0)
#define GPIO_INT_STATUS_6  GPIO1_REG(0xE0)
#define GPIO_INT_STATUS_7  GPIO1_REG(0x234)

#define GPIO_OUT_VAL_REG_BASE     0xABC00000
#define GPIO_ALT_FUNC_PAGE_REG    (GPIO_OUT_VAL_REG_BASE + 0x20)
#define GPIO_ALT_FUNC_CFG_REG     (GPIO_OUT_VAL_REG_BASE + 0x24)

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

#define GPIO38_GPIO_CNTRL      0x175

/* GPIO TLMM: Status */
#define GPIO_ENABLE     0
#define GPIO_DISABLE    1

#define GPIO_CFG(gpio, func, dir, pull, drvstr) \
    ((((gpio) & 0x3FF) << 4)        |   \
    ((func) & 0xf)                  |   \
    (((dir) & 0x1) << 14)           |   \
    (((pull) & 0x3) << 15)          |   \
    (((drvstr) & 0xF) << 17))

/**
 * struct msm_gpio - GPIO pin description
 * @gpio_cfg - configuration bitmap, as per gpio_tlmm_config()
 * @label - textual label
 *
 * Usually, GPIO's are operated by sets.
 * This struct accumulate all GPIO information in single source
 * and facilitete group operations provided by msm_gpios_xxx()
 */
struct msm_gpio {
	unsigned gpio_cfg;
	const char *label;
};

/**
 * extract GPIO pin from bit-field used for gpio_tlmm_config
 */
#define GPIO_PIN(gpio_cfg)    (((gpio_cfg) >>  4) & 0x3ff)
#define GPIO_FUNC(gpio_cfg)   (((gpio_cfg) >>  0) & 0xf)
#define GPIO_DIR(gpio_cfg)    (((gpio_cfg) >> 14) & 0x1)
#define GPIO_PULL(gpio_cfg)   (((gpio_cfg) >> 15) & 0x3)
#define GPIO_DRVSTR(gpio_cfg) (((gpio_cfg) >> 17) & 0xf)

#endif
