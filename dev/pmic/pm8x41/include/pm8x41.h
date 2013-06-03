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
#define PON_PSHOLD_SHUTDOWN     0x4

#define PMIC_VERSION_V2         1

/*Target power on reasons*/
#define DC_CHG                  8
#define USB_CHG                 16
#define PON1                    32
#define CBLPWR_N                64
#define KPDPWR_N                128

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

struct pm8x41_ldo {
	uint8_t  type;
	uint32_t base;
};

/* LDO base addresses. */
#define PM8x41_LDO2                           0x14100
#define PM8x41_LDO4                           0x14300
#define PM8x41_LDO12                          0x14B00
#define PM8x41_LDO14                          0x14D00
#define PM8x41_LDO19                          0x15200
#define PM8x41_LDO22                          0x15500

/* LDO voltage ranges */
#define NLDO_UV_MIN                           375000
#define NLDO_UV_MAX                           1537500
#define NLDO_UV_STEP                          12500
#define NLDO_UV_VMIN_LOW                      750000

#define PLDO_UV_VMIN_LOW                      750000
#define PLDO_UV_VMIN_MID                      1500000
#define PLDO_UV_VMIN_HIGH                     1750000

#define PLDO_UV_MIN                           1537500
#define PDLO_UV_MID                           3075000
#define PLDO_UV_MAX                           4900000
#define PLDO_UV_STEP_LOW                      12500
#define PLDO_UV_STEP_MID                      25000
#define PLDO_UV_STEP_HIGH                     50000

#define LDO_RANGE_SEL_BIT                     0
#define LDO_VSET_SEL_BIT                      0
#define LDO_VREG_ENABLE_BIT                   7
#define LDO_NORMAL_PWR_BIT                    7

#define PLDO_TYPE                             0
#define NLDO_TYPE                             1

#define LDO(_base, _type) \
{ \
	.type = _type, \
	.base = _base, \
}

int pm8x41_gpio_get(uint8_t gpio, uint8_t *status);
int pm8x41_gpio_set(uint8_t gpio, uint8_t value);
int pm8x41_gpio_config(uint8_t gpio, struct pm8x41_gpio *config);
void pm8x41_set_boot_done();
uint32_t pm8x41_resin_status();
uint32_t pm8x41_resin_bark_workaround_status();
void pm8x41_reset_configure(uint8_t);
void pm8x41_v2_reset_configure(uint8_t);
int pm8x41_ldo_set_voltage(struct pm8x41_ldo *ldo, uint32_t voltage);
int pm8x41_ldo_control(struct pm8x41_ldo *ldo, uint8_t enable);
uint8_t pm8x41_get_pmic_rev();
uint8_t pm8x41_get_pon_reason();

#endif
