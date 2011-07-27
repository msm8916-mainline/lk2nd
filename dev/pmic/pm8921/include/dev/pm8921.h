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
#ifndef __DEV_PM8921_H
#define __DEV_PM8921_H

#include <sys/types.h>


#define PM_GPIO_DIR_OUT         0x01
#define PM_GPIO_DIR_IN          0x02
#define PM_GPIO_DIR_BOTH        (PM_GPIO_DIR_OUT | PM_GPIO_DIR_IN)

//TODO: Rename these variables
#define PM_GPIO_PULL_UP0        0
#define PM_GPIO_PULL_UP1        2
#define PM_GPIO_PULL_UP2        3
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

/* GPIO24 for backlight_pwm which is 23 (index start at 0) */
#define GPIO_24                 23
#define GPIO_43                 42

/* LDO define values */
#define LDO_P_MASK (1 << 7)

#define LDO_2      (2)
#define LDO_8      (8  | LDO_P_MASK)
#define LDO_23     (23 | LDO_P_MASK)

enum
{
	LDO_VOLTAGE_1_2V = 0,
	LDO_VOLTAGE_1_8V = 1,
	LDO_VOLTAGE_3_0V = 2,
	LDO_VOLTAGE_ENTRIES
};

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

#endif
