/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include <debug.h>
#include <dev/keys.h>
#include <dev/ssbi.h>
#include <dev/gpio_keypad.h>
#include <dev/pm8921.h>
#include <platform/gpio.h>
#include <sys/types.h>

#define BITS_IN_ELEMENT(x) (sizeof(x) * 8)
#define KEYMAP_INDEX(row, col) (row)* BITS_IN_ELEMENT(unsigned int) + (col)

unsigned int msm8960_qwerty_keymap[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP,	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN,	/* Volume key on the device/CDP */
};

unsigned int msm8960_keys_gpiomap[] = {
	[KEYMAP_INDEX(0, 0)] = PM_GPIO(1),	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = PM_GPIO(2),	/* Volume key on the device/CDP */
};

struct qwerty_keypad_info msm8960_qwerty_keypad = {
	.keymap = msm8960_qwerty_keymap,
	.gpiomap = msm8960_keys_gpiomap,
	.mapsize = ARRAY_SIZE(msm8960_qwerty_keymap),
	.key_gpio_get = &pm8921_gpio_get,
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
};

unsigned int msm8930_qwerty_keymap[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP,	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN,	/* Volume key on the device/CDP */
};

unsigned int msm8930_keys_gpiomap[] = {
	[KEYMAP_INDEX(0, 0)] = PM_GPIO(3),	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = PM_GPIO(8),	/* Volume key on the device/CDP */
};

struct qwerty_keypad_info msm8930_qwerty_keypad = {
	.keymap = msm8930_qwerty_keymap,
	.gpiomap = msm8930_keys_gpiomap,
	.mapsize = ARRAY_SIZE(msm8930_qwerty_keymap),
	.key_gpio_get = &pm8921_gpio_get,
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
};

unsigned int apq8064_qwerty_keymap[] = {
	[KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP,	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN,	/* Volume key on the device/CDP */
};

unsigned int apq8064_keys_gpiomap[] = {
	[KEYMAP_INDEX(0, 0)] = PM_GPIO(35),	/* Volume key on the device/CDP */
	[KEYMAP_INDEX(0, 1)] = PM_GPIO(38),	/* Volume key on the device/CDP */
};


struct qwerty_keypad_info apq8064_qwerty_keypad = {
	.keymap = apq8064_qwerty_keymap,
	.gpiomap = apq8064_keys_gpiomap,
	.mapsize = ARRAY_SIZE(apq8064_qwerty_keymap),
	.key_gpio_get = &pm8921_gpio_get,
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
};

void msm8960_keypad_init(void)
{
	msm8960_keypad_gpio_init();
	ssbi_gpio_keypad_init(&msm8960_qwerty_keypad);
}

void msm8930_keypad_init(void)
{
	msm8930_keypad_gpio_init();
	ssbi_gpio_keypad_init(&msm8930_qwerty_keypad);
}

void apq8064_keypad_init(void)
{
	apq8064_keypad_gpio_init();
	ssbi_gpio_keypad_init(&apq8064_qwerty_keypad);
}

/* Configure keypad_drv through pwm or DBUS inputs or manually */
int led_kp_set( int current,
	enum kp_backlight_mode mode,
	enum kp_backlight_flash_logic flash_logic)
{
	int rc = pm8921_config_drv_keypad(current, flash_logic, mode);

	if (rc)
	{
		dprintf(CRITICAL, "FAIL pm8921_config_drv_keypad(): rc=%d.\n", rc);
	}
}

/* Configure gpio 26 through lpg2 */
void keypad_led_drv_on_pwm(void)
{
	struct pm8921_gpio keypad_pwm = {
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = 0,
		.output_value = 0,
		.pull = PM_GPIO_PULL_NO,
		.vin_sel = 2,
		.out_strength = PM_GPIO_STRENGTH_HIGH,
		.function = PM_GPIO_FUNC_2,
		.inv_int_pol = 0,
	};

	int rc = pm8921_gpio_config(PM_GPIO(26), &keypad_pwm);
	if (rc)
	{
	dprintf(CRITICAL, "FAIL pm8921_gpio_config(): rc=%d.\n", rc);
	}
}
