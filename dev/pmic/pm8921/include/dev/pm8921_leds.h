/*
 * * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#ifndef __PMIC_LED_H
#define __PMIC_LED_H

/* LED CNTL register */
#define LED_CNTL_BASE                    0x131
#define PM8921_LED_CNTL_REG(n)           (LED_CNTL_BASE + n)

#define LED_DRV0_CNTL                    PM8921_LED_CNTL_REG(0)
#define LED_DRV1_CNTL                    PM8921_LED_CNTL_REG(1)
#define LED_DRV2_CNTL                    PM8921_LED_CNTL_REG(2)

/* LED CNTL setting */

/* Current settings:
 * [00000, 10100]: Iout = x * 2 mA
 * [10101, 11111]: invalid settings
 */

#define LED_CURRENT_SET(x)              ((x) << 3)
#define LED_SIGNAL_SELECT(x)            ((x) << 0)

#define PM8921_DRV_KEYPAD_CNTL_REG      0x48

/* Keypad DRV CNTL Settings */

#define DRV_FLASH_SEL(x)                ((x) << 4)
#define FLASH_LOGIC_SEL(x)              ((x) << 2)
#define FLASH_ENSEL(x)                  ((x) << 0)

#define MAX_LC_LED_BRIGHTNESS           20
#define MAX_FLASH_BRIGHTNESS            15
#define MAX_KB_LED_BRIGHTNESS           15

/*
 * led ids
 * @PM8921_ID_LED_0 - First low current led
 * @PM8921_ID_LED_1 - Second low current led
 * @PM8921_ID_LED_2 - Third low current led
 */

enum pm8921_leds {
	PM8921_ID_LED_0,
	PM8921_ID_LED_1,
	PM8921_ID_LED_2,
};

enum led_mode{
	MANUAL,
	PWM1,
	PWM2,
	PWM3,
	DBUS1,
	DBUS2,
	DBUS3,
	DBUS4,
};

enum kp_backlight_mode{
	MANUAL_MODE,
	DBUS1_MODE,
	DBUS2_MODE,
	LPG_MODE,
};

enum kp_backlight_flash_logic{
	FLASH_ON_WITH_DTEST_HIGH,
	FLASH_ON_WITH_DTEST_LOW,
};

#endif
