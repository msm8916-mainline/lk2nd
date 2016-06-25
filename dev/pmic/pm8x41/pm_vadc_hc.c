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

#include <debug.h>
#include <spmi.h>
#include <platform/timer.h>
#include <pm_vadc_hc.h>
#include <pm8x41_hw.h>

static const struct adc_pre_scale_ratio vadc_hc_scale[] = {
	[SCALE_DIV1]	= {1,	1},
	[SCALE_DIV3]	= {1,	3},
	[SCALE_DIV4]	= {1,	4},
	[SCALE_DIV6]	= {1,	6},
	[SCALE_DIV20]	= {1,	20},
	[SCALE_DIV8]	= {1,	8},
	[SCALE10_DIV81]	= {10,	81},
	[SCALE_DIV10]	= {1,	10}
};

struct vadc_hc_periph_cfg {
	/* Peripheral base address */
	uint32_t			base;
	/* vdd_vref in millivolts */
	uint32_t			vdd_vref;
	/* ADC reference code used in post scaling calculation */
	uint32_t			vref_code;
	/* PM VADC /PMI VADC */
	enum adc_type			adc_type;
};

struct vadc_hc_channel_cfg {
	/* Calibration type for the channel - absolute/ratiometric */
	enum adc_dig_cal_sel		cal_sel;
	/* VADC channel number */
	enum adc_hc_channel		channel;
	/* Hardware settling delay for the channel */
	enum adc_usr_delay_ctl		hw_settle;
	/* Fast average sample value for the channel */
	enum adc_fast_avg_sample	avg_sample;
	/*
	 * Pre scale ratio for the channel before ADC is measured.
	 * This is used while scaling the code to physical units by
	 * applying the respective pre-scale value.
	 */
	struct adc_pre_scale_ratio	pre_scale;
	/*
	 * Scaling function used for converting the adc code to
	 * physical units based on channel properties
	 */
	int (*scale)(struct vadc_hc_periph_cfg *adc_cfg,
			uint16_t adc_code, struct adc_pre_scale_ratio *ratio,
					int64_t *result);
};

static struct vadc_hc_periph_cfg vadc_pdata[] = {
	{
		.base =		0x3100,
		.vdd_vref =	1875,
		.vref_code =	0x4000,
		.adc_type =	PM_VADC_HC
	},
	{
		.base =		0x23100,
		.vdd_vref =	1875,
		.vref_code =	0x4000,
		.adc_type =	PMI_VADC_HC
	},
};

static int scale_default_voltage(struct vadc_hc_periph_cfg *adc_cfg,
			uint16_t adc_code,
			struct adc_pre_scale_ratio *ratio,
			int64_t *result);

/*
 * This is an example of a container channel properties thats expected
 * by the ADC driver. Clients can add channels based on the
 * VADC channels supported by the target.
 */
static struct vadc_hc_channel_cfg channel_pdata[] = {
	{
		/* 1.25V reference channel */
		HC_ABS_CAL,
		HC_CALIB_VREF_1P25,
		HC_HW_SETTLE_DELAY_0US,
		AVG_1_SAMPLE,
		{1, 1},
		scale_default_voltage
	},
	{
		/* vph_pwr channel */
		HC_ABS_CAL,
		HC_VPH_PWR,
		HC_HW_SETTLE_DELAY_0US,
		AVG_1_SAMPLE,
		{1, 3},
		scale_default_voltage
	},
};

static void vadc_hc_reg_write(struct vadc_hc_periph_cfg *adc_cfg,
					uint8_t offset, uint8_t val)
{
	REG_WRITE((adc_cfg->base + offset), val);
}

static void vadc_hc_reg_read(struct vadc_hc_periph_cfg *adc_cfg,
					uint8_t offset, uint8_t *val)
{
	*val = REG_READ((adc_cfg->base + offset));
}

static int scale_default_voltage(struct vadc_hc_periph_cfg *adc_cfg,
			uint16_t adc_code,
			struct adc_pre_scale_ratio *ratio,
			int64_t *result)
{
	int64_t voltage = 0, den = 0;

	if (!ratio)
		return -1;

	voltage = (int64_t) adc_code;
	voltage *= (int64_t) adc_cfg->vdd_vref * 1000;
	den = (int64_t) adc_cfg->vref_code;
	*result = voltage / den;
	*result *= ratio->den;
	*result = *result / ratio->num;

	return 0;
}

static int vadc_hc_read_result(struct vadc_hc_periph_cfg *adc_cfg,
							uint16_t *data)
{
	uint8_t code_lsb = 0, code_msb = 0, hold_bit = 0, en = 0;

	/* Set hold bit */
	vadc_hc_reg_read(adc_cfg, HC_DATA_HOLD_CTL, &hold_bit);
	hold_bit |= HC_DATA_HOLD_CTL_FIELD;

	/* Read LSB/MSB */
	vadc_hc_reg_read(adc_cfg, HC_DATA0, &code_lsb);
	vadc_hc_reg_read(adc_cfg, HC_DATA1, &code_msb);
	*data = (code_msb << 8) | code_lsb;
	if (*data == HC_DATA_CHECK_USR) {
		dprintf(SPEW, "Invalid data :0x%x\n", *data);
		return -1;
	}

	/* Disable peripheral */
	vadc_hc_reg_write(adc_cfg, HC_EN_CTL1, en);

	/* De-assert hold bit */
	hold_bit &= ~HC_DATA_HOLD_CTL_FIELD;
	vadc_hc_reg_read(adc_cfg, HC_DATA_HOLD_CTL, &hold_bit);

	return 0;
}

static int32_t vadc_hc_check_completion(struct vadc_hc_periph_cfg *adc_cfg)
{
	int32_t i = 0;
	uint8_t status_bit = 0;

	for (i = 0; i < HC_ERR_COUNT; i++) {
		vadc_hc_reg_read(adc_cfg, HC_STATUS1, &status_bit);
		if (status_bit == 0x1)
			return 0;

		udelay(HC_CONV_TIME);
	};

	dprintf(SPEW, "Status bit not set with 0x%x\n", status_bit);

	return -1;
}

static void vadc_hc_configure(struct vadc_hc_periph_cfg *adc_cfg,
				struct vadc_hc_channel_cfg *ch_cfg)
{
	uint8_t cal = 0, avg_samples = 0;

	/* Configure calibration select */
	vadc_hc_reg_read(adc_cfg, HC_ADC_DIG_PARAM, &cal);
	cal &= ~HC_CAL_SEL_MASK;
	cal = cal | (ch_cfg->cal_sel << HC_CAL_SEL_SHIFT);
	vadc_hc_reg_write(adc_cfg, HC_ADC_DIG_PARAM, cal);

	/* Configure channel */
	vadc_hc_reg_write(adc_cfg, HC_ADC_CH_SEL_CTL, ch_cfg->channel);

	/* HW settle delay */
	vadc_hc_reg_write(adc_cfg, HC_DELAY_CTL, ch_cfg->hw_settle);

	/* Fast avg sample value */
	vadc_hc_reg_read(adc_cfg, HC_FAST_AVG_CTL, &avg_samples);
	avg_samples &= ~HC_FAST_AVG_SAMPLES_MASK;
	avg_samples |= ch_cfg->avg_sample;
	vadc_hc_reg_write(adc_cfg, HC_FAST_AVG_CTL, avg_samples);

	/* Enable ADC */
	vadc_hc_reg_write(adc_cfg, HC_EN_CTL1, HC_ADC_EN);

	/* Request conversion */
	vadc_hc_reg_write(adc_cfg, HC_CONV_REQ, HC_CONV_REQ_START);
}

static int vadc_check_channel_type(enum adc_hc_channel channel, int32_t *idx)
{
	uint32_t i = 0;

	for (i = 0; i < ARRAY_SIZE(channel_pdata); i++) {
		if (channel_pdata[i].channel == channel) {
			*idx = i;
			break;
		}
	}

	if (i == ARRAY_SIZE(channel_pdata))
		return -1;

	return 0;
}

static int vadc_check_adc_type(struct adc_dev *dev)
{
	uint32_t i = 0;

	if (dev->adc_type >= VADC_INVALID) {
		dprintf(SPEW, "Invalid VADC_TYPE\n");
		return -1;
	}

	for (i = 0; i < ARRAY_SIZE(vadc_pdata); i++)
		if (vadc_pdata[i].adc_type == dev->adc_type)
			break;

	if (i == ARRAY_SIZE(vadc_pdata))
		return -1;

	return 0;
}

int vadc_hc_read(struct adc_dev *dev, enum adc_hc_channel channel,
					struct adc_result *result)
{
	struct vadc_hc_periph_cfg *adc_cfg;
	struct vadc_hc_channel_cfg *chan_cfg;
	int32_t rc = 0, idx = 0;
	uint16_t adc_code;
	int64_t converted_value;

	if (!dev || channel >= HC_INVALID || !result) {
		dprintf(SPEW, "Invalid params\n");
		return -1;
	}

	rc = vadc_check_adc_type(dev);
	if (rc) {
		dprintf(SPEW, "Invalid adc_type params %d\n", dev->adc_type);
		return -1;
	}
	adc_cfg = &vadc_pdata[dev->adc_type];

	rc = vadc_check_channel_type(channel, &idx);
	if (rc) {
		dprintf(SPEW, "Channel pdata not present 0x%x\n", channel);
		return -1;
	}
	chan_cfg = &channel_pdata[idx];

	/* Configure ADC for the channel */
	vadc_hc_configure(adc_cfg, chan_cfg);

	/* Check completion */
	rc = vadc_hc_check_completion(adc_cfg);
	if (rc)
		return -1;

	/* Read ADC code */
	rc = vadc_hc_read_result(adc_cfg, &adc_code);
	if (rc)
		return -1;
	result->adc_code = adc_code;

	dprintf(INFO, "adc_code:0x%x\n", adc_code);

	/* Scale to physical units */
	chan_cfg->scale(adc_cfg, adc_code, &chan_cfg->pre_scale,
						&converted_value);
	result->physical = converted_value;
	dprintf(INFO, "converted value:%lld\n", converted_value);

	return 0;
}
