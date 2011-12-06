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
