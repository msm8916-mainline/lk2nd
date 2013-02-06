/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.

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

#ifndef _PM8x41_H_
#define _PM8x41_H_

#include <sys/types.h>

#define PM_GPIO_DIR_OUT         0x01
#define PM_GPIO_DIR_IN          0x00
#define PM_GPIO_DIR_BOTH        0x02

#define PM_GPIO_PULL_UP_30      0
#define PM_GPIO_PULL_UP_1_5     1
#define PM_GPIO_PULL_UP_31_5    2
/* 1.5uA + 30uA boost */
#define PM_GPIO_PULL_UP_1_5_30  3
#define PM_GPIO_PULL_RESV_1     4
#define PM_GPIO_PULL_RESV_2     5


#define PM_GPIO_OUT_CMOS        0x00
#define PM_GPIO_OUT_DRAIN_NMOS  0x01
#define PM_GPIO_OUT_DRAIN_PMOS  0x02

#define PM_GPIO_OUT_DRIVE_LOW   0x01
#define PM_GPIO_OUT_DRIVE_MED   0x02
#define PM_GPIO_OUT_DRIVE_HIGH  0x03


#define PM_GPIO_FUNC_LOW        0x00
#define PM_GPIO_FUNC_HIGH       0x01

#define PM_GPIO_MODE_MASK       0x70
#define PM_GPIO_OUTPUT_MASK     0x0F

#define PON_PSHOLD_WARM_RESET   0x1

#define PMIC_VERSION_V2         1

struct pm8x41_gpio {
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

int pm8x41_gpio_get(uint8_t gpio, uint8_t *status);
int pm8x41_gpio_set(uint8_t gpio, uint8_t value);
int pm8x41_gpio_config(uint8_t gpio, struct pm8x41_gpio *config);
void pm8x41_set_boot_done();
uint32_t pm8x41_resin_status();
uint32_t pm8x41_resin_bark_workaround_status();
void pm8x41_reset_configure(uint8_t);
int pm8x41_ldo_set_voltage(const char *, uint32_t);
int pm8x41_ldo_control(const char *, uint8_t);
uint8_t pm8x41_get_pmic_rev();

struct pm8x41_ldo {
	const char *name;
	uint8_t type;
	uint32_t base;
	uint32_t range_reg;
	uint32_t step_reg;
	uint32_t enable_reg;
};
#endif
