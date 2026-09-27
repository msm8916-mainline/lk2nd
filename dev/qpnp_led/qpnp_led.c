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

#include <stdio.h>
#include <qpnp_led.h>
#include <platform/iomap.h>
#include <pm8x41_wled.h>

static void qpnp_led_config(struct qpnp_led_data *led)
{
	pm8x41_wled_reg_write(RGB_LED_SRC_SEL(led->base),
		RGB_LED_SOURCE_VPH_PWR);
	pm8x41_wled_reg_write(LPG_PATTERN_CONFIG(led->lpg_base), 0x00);
	pm8x41_wled_reg_write(LPG_PWM_SIZE_CLK(led->lpg_base),
		PWM_6BIT_1KHZ_CLK);
	pm8x41_wled_reg_write(LPG_PWM_FREQ_PREDIV_CLK(led->lpg_base),
		PWM_FREQ);
	pm8x41_wled_reg_write(LPG_PWM_TYPE_CONFIG(led->lpg_base), 0x00);
	pm8x41_wled_reg_write(PWM_VALUE_LSB(led->lpg_base), 0x20);
	pm8x41_wled_reg_write(PWM_VALUE_MSB(led->lpg_base), 0x00);
	pm8x41_wled_reg_write(PWM_SYNC(led->lpg_base), 0x01);
	pm8x41_wled_reg_write(LPG_ENABLE_CONTROL(led->lpg_base),
		RGB_LED_ENABLE_PWM);
}

static int qpnp_led_setup(struct qpnp_led_data *led)
{
	uint8_t reg = 0;

	reg = pm8x41_wled_reg_read(RGB_LED_EN_CTL(led->base));

	switch (led->color_sel) {
		case QPNP_LED_RED:
			reg |= RGB_LED_ENABLE_RED;
			break;
		case QPNP_LED_GREEN:
			reg |= RGB_LED_ENABLE_GREEN;
			break;
		case QPNP_LED_BLUE:
			reg |= RGB_LED_ENABLE_BLUE;
			break;
		default:
			return -1;
	};

	pm8x41_wled_reg_write(RGB_LED_EN_CTL(led->base), reg);

	return 0;
}

void qpnp_led_init(enum qpnp_led_op color, uint16_t led_base_addr,
	uint16_t lpg_base_addr)
{
	int rc;
	struct qpnp_led_data led;

	led.base = led_base_addr;
	led.lpg_base = lpg_base_addr;
	led.color_sel = color;

	rc = qpnp_led_setup(&led);
	if (rc) {
		dprintf(INFO, "%s : failed\n", __func__);
		return;
	}

	 qpnp_led_config(&led);
}

