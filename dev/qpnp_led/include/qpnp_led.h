/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
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

#include <reg.h>
#include <board.h>

#define PERPH_SUBTYPE(base)            (base + 0x05)
#define RGB_LED_PERPH_TYPE(base)       (base + 0x04)
#define RGB_LED_SRC_SEL(base)          (base + 0x45)
#define RGB_LED_EN_CTL(base)           (base + 0x46)
#define RGB_LED_ATC_CTL(base)          (base + 0x47)
#define LPG_PERPH_TYPE(base)           (base + 0x04)
#define LPG_PERPH_SUBTYPE(base)        (base + 0x05)
#define LPG_PATTERN_CONFIG(base)       (base + 0x40)
#define LPG_PWM_SIZE_CLK(base)         (base + 0x41)
#define LPG_PWM_FREQ_PREDIV_CLK(base)  (base + 0x42)
#define LPG_PWM_TYPE_CONFIG(base)      (base + 0x43)
#define PWM_VALUE_LSB(base)            (base + 0x44)
#define PWM_VALUE_MSB(base)            (base + 0x45)
#define LPG_ENABLE_CONTROL(base)       (base + 0x46)
#define PWM_SYNC(base)                 (base + 0x47)

#define RGB_LED_ENABLE_BLUE       0x20
#define RGB_LED_ENABLE_GREEN      0x40
#define RGB_LED_ENABLE_RED        0x80
#define RGB_LED_SOURCE_VPH_PWR    0x01
#define RGB_LED_ENABLE_MASK       0xE0
#define RGB_LED_SRC_MASK          0xfc
#define PWM_6BIT_1KHZ_CLK         0x01
#define PWM_FREQ                  0x05
#define RGB_LED_ENABLE_PWM        0xe4

enum qpnp_led_op {
       QPNP_LED_RED,
       QPNP_LED_GREEN,
       QPNP_LED_BLUE,
};

struct qpnp_led_data {
       uint16_t base;
       uint16_t lpg_base;
       enum qpnp_led_op color_sel;
};

/* LED Initial Setup */
void qpnp_led_init(enum qpnp_led_op color, uint16_t led_base_addr,
	uint16_t lpg_base_addr);
