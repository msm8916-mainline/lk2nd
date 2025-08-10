/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
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

#ifndef _PM_VADC_HC_H_
#define _PM_VADC_HC_H_

#include <sys/types.h>
#include <bits.h>

/* HC_ peripheral */
#define HC_STATUS1					0x8

#define HC_DATA_HOLD_CTL				0x3f
#define HC_DATA_HOLD_CTL_FIELD				BIT(1)

#define HC_ADC_DIG_PARAM				0x42
#define HC_CAL_VAL					BIT(6)

#define HC_CAL_VAL_SHIFT				6
#define HC_CAL_SEL_MASK					0x30
#define HC_CAL_SEL_SHIFT				4
#define HC_DEC_RATIO_SEL				0xc
#define HC_DEC_RATIO_SHIFT				2

#define HC_FAST_AVG_CTL					0x43
#define HC_FAST_AVG_SAMPLES_MASK			0x7

#define HC_ADC_CH_SEL_CTL				0x44

#define HC_DELAY_CTL					0x45
#define HC_DELAY_CTL_MASK				0xf

#define HC_EN_CTL1					0x46
#define HC_ADC_EN					BIT(7)

#define HC_CONV_REQ					0x47
#define HC_CONV_REQ_START				BIT(7)

#define HC_DATA0					0x50
#define HC_DATA1					0x51

#define HC_DATA_CHECK_USR				0x8000
#define HC_CONV_TIME					250
#define HC_ERR_COUNT					1600
#define HC_VREF_CODE					0x4000

enum adc_type {
	PM_VADC_HC = 0,
	PMI_VADC_HC = 1,
	VADC_INVALID
};

struct adc_dev {
	enum adc_type	adc_type;
};

enum adc_fast_avg_sample {
	AVG_1_SAMPLE = 0,
	AVG_2_SAMPLES,
	AVG_4_SAMPLES,
	AVG_8_SAMPLES,
	AVG_16_SAMPLES,
	AVG_SAMPLES_INVALID
};

enum adc_channel_prediv_type {
	SCALE_DIV1 = 0,
	SCALE_DIV3,
	SCALE_DIV4,
	SCALE_DIV6,
	SCALE_DIV20,
	SCALE_DIV8,
	SCALE10_DIV81,
	SCALE_DIV10,
};

struct adc_pre_scale_ratio {
	uint8_t		num;
	uint8_t		den;
};

enum adc_dig_cal_sel {
	HC_NO_CAL		= 0,
	HC_RATIO_CAL,
	HC_ABS_CAL,
	HC_CAL_INVALID
};

enum adc_hc_channel {
	HC_VREF_GND		= 0,
	HC_CALIB_VREF_1P25	= 1,
	HC_CALIB_VREF		= 2,
	HC_CALIB_VREF_1_DIV_3	= 0x82,
	HC_VPH_PWR		= 0x83,
	HC_VBAT_SNS		= 0x84,
	HC_VCOIN		= 0x85,
	HC_DIE_TEMP		= 6,
	HC_CHG_TEMP		= 7,
	HC_USB_IN		= 8,
	HC_IREG_FB		= 9,
	/* External input connection */
	HC_BAT_THERM		= 0xa,
	HC_BAT_ID		= 0xb,
	HC_XO_THERM		= 0xc,
	HC_AMUX_THM1		= 0xd,
	HC_AMUX_THM2		= 0xe,
	HC_AMUX_THM3		= 0xf,
	HC_AMUX_THM4		= 0x10,
	HC_AMUX_THM5		= 0x11,
	HC_AMUX1_GPIO		= 0x12,
	HC_AMUX2_GPIO		= 0x13,
	HC_AMUX3_GPIO		= 0x14,
	HC_AMUX4_GPIO		= 0x15,
	HC_AMUX5_GPIO		= 0x16,
	HC_AMUX6_GPIO		= 0x17,
	HC_AMUX7_GPIO		= 0x18,
	HC_AMUX8_GPIO		= 0x19,
	HC_ATEST1		= 0x1a,
	HC_ATEST2		= 0x1b,
	HC_ATEST3		= 0x1c,
	HC_ATEST4		= 0x1d,
	HC_OFF			= 0xff,
	/* PU1 is 30K pull up */
	HC_BAT_THERM_PU1	= 0x2a,
	HC_BAT_ID_PU1		= 0x2b,
	HC_XO_THERM_PU1		= 0x2c,
	HC_AMUX_THM1_PU1	= 0x2d,
	HC_AMUX_THM2_PU1	= 0x2e,
	HC_AMUX_THM3_PU1	= 0x2f,
	HC_AMUX_THM4_PU1	= 0x30,
	HC_AMUX_THM5_PU1	= 0x31,
	HC_AMUX1_GPIO_PU1	= 0x32,
	HC_AMUX2_GPIO_PU1	= 0x33,
	HC_AMUX3_GPIO_PU1	= 0x34,
	HC_AMUX4_GPIO_PU1	= 0x35,
	HC_AMUX5_GPIO_PU1	= 0x36,
	HC_AMUX6_GPIO_PU1	= 0x37,
	HC_AMUX7_GPIO_PU1	= 0x38,
	HC_AMUX8_GPIO_PU1	= 0x39,
	/* PU2 is 100K pull up */
	HC_BAT_THERM_PU2	= 0x4a,
	HC_BAT_ID_PU2		= 0x4b,
	HC_XO_THERM_PU2		= 0x4c,
	HC_AMUX_THM1_PU2	= 0x4d,
	HC_AMUX_THM2_PU2	= 0x4e,
	HC_AMUX_THM3_PU2	= 0x4f,
	HC_AMUX_THM4_PU2	= 0x50,
	HC_AMUX_THM5_PU2	= 0x51,
	HC_AMUX1_GPIO_PU2	= 0x52,
	HC_AMUX2_GPIO_PU2	= 0x53,
	HC_AMUX3_GPIO_PU2	= 0x54,
	HC_AMUX4_GPIO_PU2	= 0x55,
	HC_AMUX5_GPIO_PU2	= 0x56,
	HC_AMUX6_GPIO_PU2	= 0x57,
	HC_AMUX7_GPIO_PU2	= 0x58,
	HC_AMUX8_GPIO_PU2	= 0x59,
	/* PU3 is 400K pull up */
	HC_BAT_THERM_PU3	= 0x6a,
	HC_BAT_ID_PU3		= 0x6b,
	HC_XO_THERM_PU3		= 0x6c,
	HC_AMUX_THM1_PU3	= 0x6d,
	HC_AMUX_THM2_PU3	= 0x6e,
	HC_AMUX_THM3_PU3	= 0x6f,
	HC_AMUX_THM4_PU3	= 0x70,
	HC_AMUX_THM5_PU3	= 0x71,
	HC_AMUX1_GPIO_PU3	= 0x72,
	HC_AMUX2_GPIO_PU3	= 0x73,
	HC_AMUX3_GPIO_PU3	= 0x74,
	HC_AMUX4_GPIO_PU3	= 0x75,
	HC_AMUX5_GPIO_PU3	= 0x76,
	HC_AMUX6_GPIO_PU3	= 0x77,
	HC_AMUX7_GPIO_PU3	= 0x78,
	HC_AMUX8_GPIO_PU3	= 0x79,
	/* External input connection with 1/3 div */
	HC_AMUX1_GPIO_DIV_3	= 0x92,
	HC_AMUX2_GPIO_DIV_3	= 0x93,
	HC_AMUX3_GPIO_DIV_3	= 0x94,
	HC_AMUX4_GPIO_DIV_3	= 0x95,
	HC_AMUX5_GPIO_DIV_3	= 0x96,
	HC_AMUX6_GPIO_DIV_3	= 0x97,
	HC_AMUX7_GPIO_DIV_3	= 0x98,
	HC_AMUX8_GPIO_DIV_3	= 0x99,
	HC_ATEST1_DIV_3		= 0x9a,
	HC_ATEST2_DIV_3		= 0x9b,
	HC_ATEST3_DIV_3		= 0x9c,
	HC_ATEST4_DIV_3		= 0x9d,
	HC_INVALID		= 0xffff
};

enum adc_usr_delay_ctl {
	HC_HW_SETTLE_DELAY_0US = 0,
	HC_HW_SETTLE_DELAY_100US,
	HC_HW_SETTLE_DELAY_200US,
	HC_HW_SETTLE_DELAY_300US,
	HC_HW_SETTLE_DELAY_400US,
	HC_HW_SETTLE_DELAY_500US,
	HC_HW_SETTLE_DELAY_600US,
	HC_HW_SETTLE_DELAY_700US,
	HC_HW_SETTLE_DELAY_800US,
	HC_HW_SETTLE_DELAY_900US,
	HC_HW_SETTLE_DELAY_1MS,
	HC_HW_SETTLE_DELAY_2MS,
	HC_HW_SETTLE_DELAY_4MS,
	HC_HW_SETTLE_DELAY_6MS,
	HC_HW_SETTLE_DELAY_8MS,
	HC_HW_SETTLE_DELAY_10MS,
	HC_HW_SETTLE_DELAY_INVALID
};

struct adc_result {
	uint16_t	adc_code;
	int64_t		physical;
};

int vadc_hc_read(struct adc_dev *dev, enum adc_hc_channel channel,
					struct adc_result *result);

#endif /* _PM_VADC_HC_H_ */
