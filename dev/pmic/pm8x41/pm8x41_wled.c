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
#include <reg.h>
#include <pm8x41_hw.h>
#include <pm8x41_wled.h>

static uint8_t wled_slave_id;

void pm8x41_wled_reg_write(uint32_t addr, uint8_t val)
{
	uint32_t new_addr;
	if (wled_slave_id) {
		new_addr = addr + (wled_slave_id << 16);
		REG_WRITE(new_addr, val);
	} else {
		new_addr = addr + (DEFAULT_SLAVE_ID << 16);
		REG_WRITE(new_addr, val);
	}
}

uint8_t pm8x41_wled_reg_read(uint32_t addr)
{
	uint32_t new_addr;
	uint8_t val;

	if (wled_slave_id) {
		new_addr = addr + (wled_slave_id << 16);
		val = REG_READ(new_addr);
	} else {
		new_addr = addr + (DEFAULT_SLAVE_ID << 16);
		val = REG_READ(new_addr);
	}
	return val;
}

void pm8x41_wled_config_slave_id(uint8_t slave_id)
{
	wled_slave_id = slave_id;
}

void pm8x41_wled_config(struct pm8x41_wled_data *wled_ctrl) {

	if (!wled_ctrl) {
		dprintf(CRITICAL, "Error: Invalid WLED data.\n");
		return;
	}

	pm8x41_wled_reg_write(PM_WLED_MODULATION_SCHEME, wled_ctrl->mod_scheme);

	pm8x41_wled_reg_write(PM_WLED_LED1_BRIGHTNESS_LSB, (wled_ctrl->led1_brightness & 0xFF));
	pm8x41_wled_reg_write(PM_WLED_LED1_BRIGHTNESS_MSB, ((wled_ctrl->led1_brightness >> 8) & 0xFF));
	pm8x41_wled_reg_write(PM_WLED_LED2_BRIGHTNESS_LSB, (wled_ctrl->led2_brightness & 0xFF));
	pm8x41_wled_reg_write(PM_WLED_LED2_BRIGHTNESS_MSB, ((wled_ctrl->led2_brightness >> 8) & 0xFF));
	pm8x41_wled_reg_write(PM_WLED_LED3_BRIGHTNESS_LSB, (wled_ctrl->led3_brightness & 0xFF));
	pm8x41_wled_reg_write(PM_WLED_LED3_BRIGHTNESS_MSB, ((wled_ctrl->led3_brightness >> 8) & 0xFF));

	pm8x41_wled_reg_write(PM_WLED_MAX_DUTY_CYCLE, wled_ctrl->max_duty_cycle);
	pm8x41_wled_reg_write(PM_WLED_OVP, wled_ctrl->ovp);
	pm8x41_wled_reg_write(LEDn_FULL_SCALE_CURRENT(1), wled_ctrl->full_current_scale);
	pm8x41_wled_reg_write(LEDn_FULL_SCALE_CURRENT(2), wled_ctrl->full_current_scale);
	pm8x41_wled_reg_write(LEDn_FULL_SCALE_CURRENT(3), wled_ctrl->full_current_scale);

	pm8x41_wled_reg_write(PM_WLED_FDBCK_CONTROL, wled_ctrl->fdbck);
	dprintf(SPEW, "WLED Configuration Success.\n");

}

void pm8x41_wled_sink_control(uint8_t enable) {

	uint8_t value = 0x0;

	if (enable) {
		value = PM_WLED_LED1_SINK_MASK |
			PM_WLED_LED2_SINK_MASK |
			PM_WLED_LED3_SINK_MASK;
	}

	pm8x41_wled_reg_write(PM_WLED_CURRENT_SINK, value);

	dprintf(SPEW, "WLED Sink Success\n");

}

void pm8x41_wled_iled_sync_control(uint8_t enable) {

	uint8_t value = 0x0;

	if (enable) {
		value = PM_WLED_LED1_ILED_SYNC_MASK |
			PM_WLED_LED2_ILED_SYNC_MASK |
			PM_WLED_LED3_ILED_SYNC_MASK;
	}

	pm8x41_wled_reg_write(PM_WLED_ILED_SYNC_BIT, value);

	dprintf(SPEW, "WLED ILED Sync Success\n");

}

void pm8x41_wled_led_mod_enable(uint8_t enable) {

	uint8_t value = 0x0;

	if (enable)
		value = PM_WLED_LED_MODULATOR_EN;

	pm8x41_wled_reg_write(PM_WLED_LED_CTNL_REG(1), value);
	pm8x41_wled_reg_write(PM_WLED_LED_CTNL_REG(2), value);
	pm8x41_wled_reg_write(PM_WLED_LED_CTNL_REG(3), value);

	dprintf(SPEW, "WLED LED Module Enable Success\n");

}

void pm8x41_wled_enable(uint8_t enable) {

	uint8_t value = 0x0;

	if (enable)
		value = PM_WLED_ENABLE_MODULE_MASK;

	pm8x41_wled_reg_write(PM_WLED_ENABLE, value);

	dprintf(SPEW, "WLED Enable Success\n");

}
