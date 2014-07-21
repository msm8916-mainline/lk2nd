/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.

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

#include <bits.h>
#include <debug.h>
#include <reg.h>

#define PM_WLED_BASE                 0x0D800
#define PM_WLED_CTNL_REG(n)          (PM_WLED_BASE + n)
#define PM_WLED_LED_CTNL_REG(n)      (PM_WLED_BASE + 0x60 + (n-1)*0x10)

#define PM_WLED_LED1_BRIGHTNESS_LSB  PM_WLED_CTNL_REG(0x40)
#define PM_WLED_LED1_BRIGHTNESS_MSB  PM_WLED_CTNL_REG(0x41)
#define PM_WLED_LED2_BRIGHTNESS_LSB  PM_WLED_CTNL_REG(0x42)
#define PM_WLED_LED2_BRIGHTNESS_MSB  PM_WLED_CTNL_REG(0x43)
#define PM_WLED_LED3_BRIGHTNESS_LSB  PM_WLED_CTNL_REG(0x44)
#define PM_WLED_LED3_BRIGHTNESS_MSB  PM_WLED_CTNL_REG(0x45)
#define PM_WLED_ENABLE               PM_WLED_CTNL_REG(0x46)
#define PM_WLED_ILED_SYNC_BIT        PM_WLED_CTNL_REG(0x47)
#define PM_WLED_FDBCK_CONTROL        PM_WLED_CTNL_REG(0x48)
#define PM_WLED_MODULATION_SCHEME    PM_WLED_CTNL_REG(0x4A)
#define PM_WLED_MAX_DUTY_CYCLE       PM_WLED_CTNL_REG(0x4B)
#define PM_WLED_OVP                  PM_WLED_CTNL_REG(0x4D)
#define PM_WLED_CURRENT_SINK         PM_WLED_CTNL_REG(0x4F)
#define LEDn_FULL_SCALE_CURRENT(n)   (PM_WLED_LED_CTNL_REG(n) + 0x2)

#define PM_WLED_LED1_SINK_MASK       BIT(5)
#define PM_WLED_LED2_SINK_MASK       BIT(6)
#define PM_WLED_LED3_SINK_MASK       BIT(7)

#define PM_WLED_LED_MODULATOR_EN     BIT(7)
#define PM_WLED_LED1_ILED_SYNC_MASK  BIT(0)
#define PM_WLED_LED2_ILED_SYNC_MASK  BIT(1)
#define PM_WLED_LED3_ILED_SYNC_MASK  BIT(2)

#define PM_WLED_ENABLE_MODULE_MASK   BIT(7)
#define DEFAULT_SLAVE_ID             0x1
struct pm8x41_wled_data{
	uint8_t mod_scheme;
	uint16_t led1_brightness;
	uint16_t led2_brightness;
	uint16_t led3_brightness;
	uint8_t max_duty_cycle;
	uint8_t ovp;
	uint8_t full_current_scale;
	uint8_t fdbck;
};

void pm8x41_wled_config(struct pm8x41_wled_data *wled_ctrl);
void pm8x41_wled_iled_sync_control(uint8_t enable);
void pm8x41_wled_sink_control(uint8_t enable);
void pm8x41_wled_led_mod_enable(uint8_t enable);
void pm8x41_wled_enable(uint8_t enable);
void pm8x41_wled_config_slave_id(uint8_t slave_id);
uint8_t pm8x41_wled_reg_read(uint32_t addr);
void pm8x41_wled_reg_write(uint32_t addr, uint8_t val);
