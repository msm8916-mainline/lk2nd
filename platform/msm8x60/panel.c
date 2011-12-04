/*
 * * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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

#include <debug.h>
#include <kernel/thread.h>
#include <i2c_qup.h>
#include <platform.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/pmic.h>
#include <platform/pmic_pwm.h>
#include <platform/machtype.h>
#include <platform/timer.h>
#include <gsbi.h>
#include <dev/lcdc.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static struct qup_i2c_dev *dev = NULL;
void gpio_tlmm_config(uint32_t gpio, uint8_t func,
		      uint8_t dir, uint8_t pull,
		      uint8_t drvstr, uint32_t enable);

uint8_t expander_read(uint8_t addr)
{
	uint8_t ret = 0;
	/* Create a i2c_msg buffer, that is used to put the controller into read
	   mode and then to read some data. */
	struct i2c_msg msg_buf[] = {
		{CORE_GPIO_EXPANDER_I2C_ADDRESS, I2C_M_WR, 1, &addr},
		{CORE_GPIO_EXPANDER_I2C_ADDRESS, I2C_M_RD, 1, &ret}
	};

	qup_i2c_xfer(dev, msg_buf, 2);

	return ret;
}

uint8_t expander_write(uint8_t addr, uint8_t val)
{
	uint8_t data_buf[] = { addr, val };

	/* Create a i2c_msg buffer, that is used to put the controller into write
	   mode and then to write some data. */
	struct i2c_msg msg_buf[] = { {CORE_GPIO_EXPANDER_I2C_ADDRESS,
				      I2C_M_WR, 2, data_buf}
	};

	qup_i2c_xfer(dev, msg_buf, 1);

	/* Double check that the write worked. */
	if (val != expander_read(addr)) {
		return -1;
	}

	return 0;
}

void panel_backlight(int on)
{
}

static int display_common_power(int on)
{
}

static int lcd_power_on()
{
	uint8_t buffer = 0x0, mask = 0x0, prev_val = 0x0;
	int ret = 0;

	/* Configure LDO L2 TEST Bank 2, to Range Select 0 */
	/* Not updating reference voltage */
	buffer = (0x80);	/* Write mode */
	buffer |= (PM8901_LDO_TEST_BANK(2));	/* Test Bank 2 */
	mask = buffer | LDO_TEST_RANGE_SELECT_MASK;

	if ((ret = pm8901_test_bank_read(&prev_val,
					 PM8901_LDO_TEST_BANK(2),
					 PM8901_LDO_L2_TEST_BANK))) {
		return ret;
	}
	if ((ret = pm8901_vreg_write(&buffer, mask, PM8901_LDO_L2_TEST_BANK,
				     prev_val))) {
		return ret;
	}

	/* Enable LDO L2 at Max Voltage (should be around 3.3v) */
	buffer = (0x0 << PM8901_LDO_CTL_ENABLE__S);
	/* Disable Pull Down */
	buffer |= (0x1 << PM8901_LDO_CTL_PULL_DOWN__S);
	/* Put LDO into normal mode instead of low power mode */
	buffer |= (0x0 << PM8901_LDO_CTL_MODE__S);

	/* Set voltage programming to 3.3V or 2.85V(8660 fluid) */
	if (board_machtype() == LINUX_MACHTYPE_8660_FLUID)
		buffer |= (0xB);
	else
		buffer |= (0xF);

	mask = buffer | LDO_CTL_ENABLE_MASK |
	    LDO_CTL_PULL_DOWN_MASK |
	    LDO_CTL_NORMAL_POWER_MODE_MASK | LDO_CTL_VOLTAGE_SET_MASK;

	/* Do a normal read here, as to not destroy the value in LDO control */
	if ((ret = pm8901_read(&prev_val, 1, PM8901_LDO_L2))) {
		return ret;
	}
	/* Configure the LDO2 for 3.3V or 2.85V(8660 fluid) */
	ret = pm8901_vreg_write(&buffer, mask, PM8901_LDO_L2, prev_val);

	/* Configure LDO L2 TEST Bank 4, for High Range Mode */
	buffer = (0x80);	/* Write mode */
	buffer |= (PM8901_LDO_TEST_BANK(4));	/* Test Bank 4 */
	buffer |= (0x01);	/* Put into High Range Mode */
	mask = buffer | LDO_TEST_OUTPUT_RANGE_MASK;

	if ((ret = pm8901_test_bank_read(&prev_val,
					 PM8901_LDO_TEST_BANK(4),
					 PM8901_LDO_L2_TEST_BANK))) {
		return ret;
	}
	if ((ret = pm8901_vreg_write(&buffer, mask, PM8901_LDO_L2_TEST_BANK,
				     prev_val))) {
		return ret;
	}

	/* Configure LDO L2 TEST Bank 2, to Range Select 0 */
	buffer = (0x80);	/* Write mode */
	buffer |= (PM8901_LDO_TEST_BANK(2));	/* Test Bank 2 */
	buffer |= (1 << 1);	/* For fine step 50 mV */
	buffer |= (1 << 3);	/* to update reference voltage */
	mask = buffer | LDO_TEST_RANGE_SELECT_MASK;
	mask |= (1 << 2);	/* Setting mask to make ref voltage as 1.25 V */

	if ((ret = pm8901_test_bank_read(&prev_val,
					 PM8901_LDO_TEST_BANK(2),
					 PM8901_LDO_L2_TEST_BANK))) {
		return ret;
	}
	if ((ret = pm8901_vreg_write(&buffer, mask, PM8901_LDO_L2_TEST_BANK,
				     prev_val))) {
		return ret;
	}

	/* Enable PMR for LDO L2 */
	buffer = 0x7F;
	mask = 0x7F;
	if ((ret = pm8901_read(&prev_val, 1, PM8901_PMR_7))) {
		return ret;
	}
	ret = pm8901_vreg_write(&buffer, mask, PM8901_PMR_7, prev_val);
	return ret;
}

/* Configures the GPIO that are needed to enable LCD.
 * This function also configures the PMIC for PWM control of the LCD backlight.
 */
static void lcd_gpio_cfg(uint8_t on)
{
	uint32_t func;
	uint32_t pull;
	uint32_t dir;
	uint32_t enable = 0;	/* not used in gpio_tlmm_config */
	uint32_t drv;
	if (on) {
		func = 1;	/* Configure GPIO for LCDC function */
		pull = GPIO_NO_PULL;
		dir = 1;	/* doesn't matter since it is not configured as
				   GPIO */
		drv = GPIO_16MA;
	} else {
		/* As discussed in the MSM8660 FFA HW SW Control Doc configure these
		   GPIO as input and pull down. */
		func = 0;	/* GPIO */
		pull = GPIO_PULL_DOWN;
		dir = 0;	/* Input */
		drv = 0;	/* does not matter configured as input */
	}

	gpio_tlmm_config(0, func, dir, pull, drv, enable);	/* lcdc_pclk */
	gpio_tlmm_config(1, func, dir, pull, drv, enable);	/* lcdc_hsync */
	gpio_tlmm_config(2, func, dir, pull, drv, enable);	/* lcdc_vsync */
	gpio_tlmm_config(3, func, dir, pull, drv, enable);	/* lcdc_den */
	gpio_tlmm_config(4, func, dir, pull, drv, enable);	/* lcdc_red7 */
	gpio_tlmm_config(5, func, dir, pull, drv, enable);	/* lcdc_red6 */
	gpio_tlmm_config(6, func, dir, pull, drv, enable);	/* lcdc_red5 */
	gpio_tlmm_config(7, func, dir, pull, drv, enable);	/* lcdc_red4 */
	gpio_tlmm_config(8, func, dir, pull, drv, enable);	/* lcdc_red3 */
	gpio_tlmm_config(9, func, dir, pull, drv, enable);	/* lcdc_red2 */
	gpio_tlmm_config(10, func, dir, pull, drv, enable);	/* lcdc_red1 */
	gpio_tlmm_config(11, func, dir, pull, drv, enable);	/* lcdc_red0 */
	gpio_tlmm_config(12, func, dir, pull, drv, enable);	/* lcdc_rgn7 */
	gpio_tlmm_config(13, func, dir, pull, drv, enable);	/* lcdc_rgn6 */
	gpio_tlmm_config(14, func, dir, pull, drv, enable);	/* lcdc_rgn5 */
	gpio_tlmm_config(15, func, dir, pull, drv, enable);	/* lcdc_rgn4 */
	gpio_tlmm_config(16, func, dir, pull, drv, enable);	/* lcdc_rgn3 */
	gpio_tlmm_config(17, func, dir, pull, drv, enable);	/* lcdc_rgn2 */
	gpio_tlmm_config(18, func, dir, pull, drv, enable);	/* lcdc_rgn1 */
	gpio_tlmm_config(19, func, dir, pull, drv, enable);	/* lcdc_rgn0 */
	gpio_tlmm_config(20, func, dir, pull, drv, enable);	/* lcdc_blu7 */
	gpio_tlmm_config(21, func, dir, pull, drv, enable);	/* lcdc_blu6 */
	gpio_tlmm_config(22, func, dir, pull, drv, enable);	/* lcdc_blu5 */
	gpio_tlmm_config(23, func, dir, pull, drv, enable);	/* lcdc_blu4 */
	gpio_tlmm_config(24, func, dir, pull, drv, enable);	/* lcdc_blu3 */
	gpio_tlmm_config(25, func, dir, pull, drv, enable);	/* lcdc_blu2 */
	gpio_tlmm_config(26, func, dir, pull, drv, enable);	/* lcdc_blu1 */
	gpio_tlmm_config(27, func, dir, pull, drv, enable);	/* lcdc_blu0 */
}

/* API to set backlight level configuring PWM in PM8058 */

int panel_set_backlight(uint8_t bt_level)
{
	int rc = -1;
	uint32_t duty_us, period_us;

	if ((bt_level <= 0) || (bt_level > 15)) {
		dprintf(CRITICAL, "Error in brightness level (1-15 allowed)\n");
		goto bail_out;
	}

	duty_us = bt_level * PWM_DUTY_LEVEL;
	period_us = PWM_PERIOD_USEC;
	rc = pm_pwm_config(0, duty_us, period_us);
	if (rc) {
		dprintf(CRITICAL, "Error in pwm_config0\n");
		goto bail_out;
	}

	duty_us = PWM_PERIOD_USEC - (bt_level * PWM_DUTY_LEVEL);
	period_us = PWM_PERIOD_USEC;
	rc = pm_pwm_config(1, duty_us, period_us);
	if (rc) {
		dprintf(CRITICAL, "Error in pwm_config1\n");
		goto bail_out;
	}

	rc = pm_pwm_enable(0);
	if (rc) {
		dprintf(CRITICAL, "Error in pwm_enable0\n");
		goto bail_out;
	}

	rc = pm_pwm_enable(1);
	if (rc)
		dprintf(CRITICAL, "Error in pwm_enable1\n");

 bail_out:
	return rc;
}

void bl_gpio_init(void)
{
	/* Configure PM8058 GPIO24 as a PWM driver (LPG ch0) for chain 1 of 6 LEDs */
	pm8058_write_one(0x81, GPIO24_GPIO_CNTRL);	/* Write, Bank0, VIN0, Mode
							   selection enabled */
	pm8058_write_one(0x98, GPIO24_GPIO_CNTRL);	/* Write, Bank1, OutOn/InOff,
							   CMOS, Don't Invert Output */
	pm8058_write_one(0xAA, GPIO24_GPIO_CNTRL);	/* Write, Bank2, GPIO no pull */
	pm8058_write_one(0xB4, GPIO24_GPIO_CNTRL);	/* Write, Bank3, high drv
							   strength */
	pm8058_write_one(0xC6, GPIO24_GPIO_CNTRL);	/* Write, Bank4, Src: LPG_DRV1
							   (Spec. Fnc 2) */
	pm8058_write_one(0xD8, GPIO24_GPIO_CNTRL);	/* Write, Bank5, Interrupt
							   polarity noninversion */

	/* Configure PM8058 GPIO25 as a PWM driver (LPG ch1) for chain 2 of 5 LEDs */
	pm8058_write_one(0x81, GPIO25_GPIO_CNTRL);	/* Write, Bank0, VIN0, Mode
							   selection enabled */
	pm8058_write_one(0x98, GPIO25_GPIO_CNTRL);	/* Write, Bank1, OutOn/InOff,
							   CMOS, Don't Invert Output */
	pm8058_write_one(0xAA, GPIO25_GPIO_CNTRL);	/* Write, Bank2, GPIO no pull */
	pm8058_write_one(0xB4, GPIO25_GPIO_CNTRL);	/* Write, Bank3, high drv
							   strength */
	pm8058_write_one(0xC6, GPIO25_GPIO_CNTRL);	/* Write, Bank4, Src: LPG_DRV2
							   (Spec. Fnc 2) */
	pm8058_write_one(0xD8, GPIO25_GPIO_CNTRL);	/* Write, Bank5, Interrupt
							   polarity noninversion */
}

void board_lcd_enable(void)
{
	int rc = -1;
	dev = qup_i2c_init(GSBI_ID_8, 100000, 24000000);

	/* Make sure dev is created and initialized properly */
	if (!dev) {
		while (1) ;
		return;
	}

	/* Store current value of these registers as to not destroy their previous
	   state. */
	uint8_t open_drain_a = expander_read(GPIO_EXPANDER_REG_OPEN_DRAIN_A);
	uint8_t dir_b = expander_read(GPIO_EXPANDER_REG_DIR_B);
	uint8_t dir_a = expander_read(GPIO_EXPANDER_REG_DIR_A);
	uint8_t data_b = expander_read(GPIO_EXPANDER_REG_DATA_B);
	uint8_t data_a = expander_read(GPIO_EXPANDER_REG_DATA_A);

	/* Set the LVDS_SHUTDOWN_N to open drain and output low. */
	dprintf(INFO, "Enable lvds_shutdown_n line for Open Drain.\n");
	expander_write(GPIO_EXPANDER_REG_OPEN_DRAIN_A, 0x04 | open_drain_a);

	dprintf(INFO, "Enable lvds_shutdown_n line for output.\n");
	expander_write(GPIO_EXPANDER_REG_DIR_A, ~0x04 & dir_a);

	dprintf(INFO, "Drive the LVDS_SHUTDOWN_N pin high here.\n");
	expander_write(GPIO_EXPANDER_REG_DATA_A, 0x04 | data_a);

	/* Turn on the VREG_L2B to 3.3V. */

	/* Power on the appropiate PMIC LDO power rails */
	if (lcd_power_on())
		return;

	/* Enable the GPIO as LCDC mode LCD. */
	lcd_gpio_cfg(1);

	/* Arbitrary delay */
	udelay(20000);

	/* Set the GPIOs needed for backlight */
	bl_gpio_init();
	/* Set backlight level with API (to 8 by default) */
	rc = panel_set_backlight(8);
	if (rc)
		dprintf(CRITICAL, "Error in setting panel backlight\n");

	dprintf(INFO, "Enable BACKLIGHT_EN line for output.\n");
	expander_write(GPIO_EXPANDER_REG_DIR_B, ~0x10 & dir_b);

	dprintf(INFO, "Drive BACKLIGHT_EN to high\n");
	expander_write(GPIO_EXPANDER_REG_DATA_B, 0x10 | data_b);

}

void lcdc_on(void)
{
	board_lcd_enable();
}

void auo_board_lcd_enable(void)
{
	/* Make sure dev is created and initialized properly */
	dev = qup_i2c_init(GSBI_ID_8, 100000, 24000000);
	if (!dev) {
		dprintf(CRITICAL, "Error in qup_i2c_init\n");
		while (1) ;
	}

	/* Setup RESX_N */
	uint8_t open_drain_a = expander_read(GPIO_EXPANDER_REG_OPEN_DRAIN_A);
	uint8_t dir_a = expander_read(GPIO_EXPANDER_REG_DIR_A);
	uint8_t data_a = expander_read(GPIO_EXPANDER_REG_DATA_A);

	expander_write(GPIO_EXPANDER_REG_DIR_A, ~0x04 & dir_a);
	expander_write(GPIO_EXPANDER_REG_DATA_A, ~0x04 & data_a);

	/* Power on the appropiate PMIC LDO power rails */
	if (lcd_power_on())
		return;

	/* Toggle RESX_N */
	mdelay(20);
	expander_write(GPIO_EXPANDER_REG_DATA_A, 0x04 | data_a);
	mdelay(1);
	expander_write(GPIO_EXPANDER_REG_DATA_A, ~0x04 & data_a);
	mdelay(1);
	expander_write(GPIO_EXPANDER_REG_DATA_A, 0x04 | data_a);
	mdelay(25);

	/* Enable the gpios for  LCD */
	lcd_gpio_cfg(1);

	auo_lcdc_init();
}

void panel_poweron(void)
{
	if (board_machtype() == LINUX_MACHTYPE_8660_FLUID) {
		auo_board_lcd_enable();
	} else {
		panel_backlight(1);
		lcdc_on();
	}
}

struct lcdc_timing_parameters *get_lcd_timing(void)
{
	if (board_machtype() == LINUX_MACHTYPE_8660_FLUID)
		return auo_timing_param();
	else
		return DEFAULT_LCD_TIMING;
}
