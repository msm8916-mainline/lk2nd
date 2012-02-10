/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
 */

#include<dev/pm8921_leds.h>
#include<dev/pm8921.h>

void led_tests()
{
uint32_t duty_us, period_us;

    /* 50% Duty cycle */
    duty_us = 500000;
    period_us = 1000000;

    /* Configure PM8921_ID_LED_0 from PWM2*/
    pm8921_config_led_current(PM8921_ID_LED_0, 2, PWM2, 1);

    /* PWM2 for PM8921_ID_LED_0 is LPG 5
     * Configure and enable lpg5
     */
    pm_set_pwm_config(5, duty_us, period_us, &pmic);
    pm_pwm_channel_enable(5, &pmic);

    /* Configure and enable lpg0 for  panel backlight*/
    pm_set_pwm_config(0, duty_us, period_us, &pmic);
    pm_pwm_channel_enable(0, &pmic);

    mdelay(10000);

    /* Configure PM8921_ID_LED_1 also from PWM2*/
    pm8921_config_led_current(PM8921_ID_LED_1, 2, PWM2, 1);
    mdelay(10000);

    /* Disable PM8921_ID_LED_0 */
    pm8921_config_led_current(PM8921_ID_LED_0, 2, 2, 0);

    /* Turn on GPIO 24 through LPG 0
     * Will be reconfigured during display_init
     */
    panel_backlight_on_pwm();

    mdelay(10000);

    /* Disable PM8921_ID_LED_1 */
    pm8921_config_led_current(PM8921_ID_LED_1, 2, 2, 0);
}
