/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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
#ifndef __DEV_PM8921_H
#define __DEV_PM8921_H

#include <sys/types.h>
#include <dev/pm8921_leds.h>

enum {
	lvs_start = 1,
	lvs_1 = lvs_start,
	lvs_2,
	lvs_3,
	lvs_4,
	lvs_5,
	lvs_6,
	lvs_7,
	lvs_end = lvs_7,
};

enum {
	mpp_start = 1,
	mpp_1 = mpp_start,
	mpp_2,
	mpp_3,
	mpp_4,
	mpp_5,
	mpp_6,
	mpp_7,
	mpp_8,
	mpp_9,
	mpp_10,
	mpp_11,
	mpp_12,
	mpp_end = mpp_12,
};

#define PM_GPIO_DIR_OUT         0x01
#define PM_GPIO_DIR_IN          0x02
#define PM_GPIO_DIR_BOTH        (PM_GPIO_DIR_OUT | PM_GPIO_DIR_IN)

/* output_buffer */
#define PM_GPIO_OUT_BUF_OPEN_DRAIN  1
#define PM_GPIO_OUT_BUF_CMOS        0

#define PM_GPIO_PULL_UP_30      0
#define PM_GPIO_PULL_UP_1_5     1
#define PM_GPIO_PULL_UP_31_5    2
/* 1.5uA + 30uA boost */
#define PM_GPIO_PULL_UP_1_5_30  3
#define PM_GPIO_PULL_DN         4
#define PM_GPIO_PULL_NO         5

#define PM_GPIO_STRENGTH_NO     0
#define PM_GPIO_STRENGTH_HIGH   1
#define PM_GPIO_STRENGTH_MED    2
#define PM_GPIO_STRENGTH_LOW    3

#define PM_GPIO_FUNC_NORMAL     0
#define PM_GPIO_FUNC_PAIRED     1
#define PM_GPIO_FUNC_1          2
#define PM_GPIO_FUNC_2          3

/* LDO define values */
#define LDO_P_MASK (1 << 7)

#define LDO_2      (2)
#define LDO_8      (8  | LDO_P_MASK)
#define LDO_11     (11 | LDO_P_MASK)
#define LDO_23     (23 | LDO_P_MASK)

enum
{
	LDO_VOLTAGE_1_2V = 0,
	LDO_VOLTAGE_1_8V = 1,
	LDO_VOLTAGE_3_0V = 2,
	LDO_VOLTAGE_ENTRIES
};

#define PM_GPIO(_x)                    ((_x) - 1)
#define PM_IRQ_BLOCK(_x)               (_x)


#define PM_IRQ_BLOCK_GPIO_START        PM_IRQ_BLOCK(24)

#define PM_GPIO_BLOCK_ID(gpio)         (PM_IRQ_BLOCK_GPIO_START + (gpio)/8)
#define PM_GPIO_ID_TO_BIT_MASK(gpio)   (1 << ((gpio)%8))
#define PM_PWRKEY_BLOCK_ID		6
#define PM_PWRKEY_PRESS_BIT		(1 << 3)

typedef int (*pm8921_read_func)(uint8_t *data, uint32_t length, uint32_t addr);
typedef int (*pm8921_write_func)(uint8_t *data, uint32_t length, uint32_t addr);

typedef struct
{
	uint32_t initialized;

	pm8921_read_func	read;
	pm8921_write_func	write;

} pm8921_dev_t;


struct pm8921_gpio {
	int direction;
	int output_buffer;
	int output_value;
	int pull;
	int vin_sel;
	int out_strength;
	int function;
	int inv_int_pol;
	int disable_pin;
};

void pm8921_init(pm8921_dev_t *);
int  pm8921_gpio_config(int gpio, struct pm8921_gpio *param);
void pm8921_boot_done(void);
int  pm8921_ldo_set_voltage(uint32_t ldo_id, uint32_t voltage);
int  pm8921_config_reset_pwr_off(unsigned reset);
int  pm8921_gpio_get(uint8_t gpio, uint8_t *status);
int  pm8921_pwrkey_status(uint8_t *status);
int pm8921_config_led_current(enum pm8921_leds led_num,
	uint8_t current,
	enum led_mode sink,
	int enable);
int pm8921_config_drv_keypad(unsigned int drv_flash_sel,
	unsigned int flash_logic,
	unsigned int flash_ensel);
int pm8921_low_voltage_switch_enable(uint8_t lvs_id);
int pm8921_mpp_set_digital_output(uint8_t mpp_id);
#endif
