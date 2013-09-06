/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#include <bits.h>
#include <debug.h>
#include <reg.h>
#include <spmi.h>
#include <platform/timer.h>
#include <pm8x41_adc.h>
#include <pm8x41_hw.h>

/*
 * This is the predefined adc configuration values for the supported
 * channels
 */
static struct adc_conf adc_data[] = {
	CHAN_INIT(VADC_USR1_BASE, VADC_BAT_CHAN_ID,     VADC_MODE_NORMAL, VADC_DECIM_RATIO_VAL, HW_SET_DELAY_100US, FAST_AVG_SAMP_1, CALIB_RATIO),
	CHAN_INIT(VADC_USR1_BASE, VADC_BAT_VOL_CHAN_ID, VADC_MODE_NORMAL, VADC_DECIM_RATIO_VAL, HW_SET_DELAY_100US, FAST_AVG_SAMP_1, CALIB_ABS),
	CHAN_INIT(VADC_USR1_BASE, MPP_8_CHAN_ID, VADC_MODE_NORMAL, VADC_DECIM_RATIO_VAL, HW_SET_DELAY_100US, FAST_AVG_SAMP_1, CALIB_ABS),
};


static struct adc_conf* get_channel_prop(uint16_t ch_num)
{
	struct adc_conf *chan_data = NULL;
	uint8_t i;

	for(i = 0; i < ARRAY_SIZE(adc_data) ; i++) {
		chan_data = &adc_data[i];
		if (chan_data->chan == ch_num)
			break;
	}

	return chan_data;
}

static void adc_limit_result_range(uint16_t *result)
{
	if (*result < VADC_MIN_VAL)
        *result = VADC_MIN_VAL;
	else if(*result > VADC_MAX_VAL)
        *result = VADC_MAX_VAL;
}

static void adc_read_conv_result(struct adc_conf *adc, uint16_t *result)
{
	uint8_t val = 0;

	/* Read the MSB part */
	val = REG_READ(adc->base + VADC_REG_DATA_MSB);
	*result = val;

	/* Read the LSB part */
	val = REG_READ(adc->base + VADC_REG_DATA_LSB);
	*result = ((*result) << 8) | val;

	adc_limit_result_range(result);
}

static void adc_enable(struct adc_conf *adc, uint8_t enable)
{
	if (enable)
		REG_WRITE((adc->base + VADC_EN_CTL), VADC_CTL_EN_BIT);
	else
		REG_WRITE((adc->base + VADC_EN_CTL), (uint8_t) (~VADC_CTL_EN_BIT));
}

static void adc_measure(struct adc_conf *adc, uint16_t *result)
{
	uint8_t status;

	/* Request conversion */
	REG_WRITE((adc->base + VADC_CONV_REQ), VADC_CON_REQ_BIT);

	/* Poll for the conversion to complete */
	do {
		status = REG_READ(adc->base + VADC_STATUS);
		status &= VADC_STATUS_MASK;
		if (status == VADC_STATUS_EOC) {
			dprintf(SPEW, "ADC conversion is complete\n");
			break;
		}
		/* Wait for sometime before polling for the status again */
		udelay(10);
	} while(1);

	/* Now read the conversion result */
	adc_read_conv_result(adc, result);
}

/*
 * This function configures adc & requests for conversion
 */
static uint16_t adc_configure(struct adc_conf *adc)
{
	uint8_t mode;
	uint8_t adc_p;
	uint16_t result;

	/* Mode Selection */
	mode = (adc->mode << VADC_MODE_BIT_NORMAL);
	REG_WRITE((adc->base + VADC_MODE_CTRL), mode);

	/* Select Channel */
	REG_WRITE((adc->base + VADC_CHAN_SEL), adc->chan);

	/* ADC digital param setup */
	adc_p = (adc->adc_param << VADC_DECIM_RATIO_SEL);
	REG_WRITE((adc->base + VADC_DIG_ADC_PARAM), adc_p);

	/* hardware settling time */
	REG_WRITE((adc->base + VADC_HW_SETTLE_TIME), adc->hw_set_time);

	/* For normal mode set the fast avg */
	REG_WRITE((adc->base + VADC_FAST_AVG), adc->fast_avg);

	/* Enable Vadc */
	adc_enable(adc, true);

	/* Measure the result */
	adc_measure(adc, &result);

	/* Disable vadc */
	adc_enable(adc, false);

	return result;
}

static uint32_t vadc_calibrate(uint16_t result, uint8_t calib_type)
{
	struct adc_conf calib;
	uint16_t calib1;
	uint16_t calib2;
	uint32_t calib_result = 0;
	uint32_t mul;

	if(calib_type == CALIB_ABS) {
		/*
		 * Measure the calib data for 1.25 V ref
		 */
		calib.base = VADC_USR1_BASE;
		calib.chan = VREF_125_CHAN_ID;
		calib.mode = VADC_MODE_NORMAL;
		calib.adc_param = VADC_DECIM_RATIO_VAL;
		calib.hw_set_time = 0x0;
		calib.fast_avg = 0x0;

		calib1 = adc_configure(&calib);

		/*
		 * Measure the calib data for 0.625 V ref
		 */
		calib.base = VADC_USR1_BASE;
		calib.chan = VREF_625_CHAN_ID;
		calib.mode = VADC_MODE_NORMAL;
		calib.adc_param = VADC_DECIM_RATIO_VAL;
		calib.hw_set_time = 0x0;
		calib.fast_avg = 0x0;

		calib2 = adc_configure(&calib);

		mul = VREF_625_MV / (calib1 - calib2);
		calib_result = (result - calib2) * mul;
		calib_result += VREF_625_MV;
		calib_result *= OFFSET_GAIN_DNOM;
		calib_result /= OFFSET_GAIN_NUME;
	} else if(calib_type == CALIB_RATIO) {
		/*
		 * Measure the calib data for VDD_ADC ref
		 */
		calib.base = VADC_USR1_BASE;
		calib.chan = VDD_VADC_CHAN_ID;
		calib.mode = VADC_MODE_NORMAL;
		calib.adc_param = VADC_DECIM_RATIO_VAL;
		calib.hw_set_time = 0;
		calib.fast_avg = 0;

		calib1 = adc_configure(&calib);

		/*
		 * Measure the calib data for ADC_GND
		 */
		calib.base = VADC_USR1_BASE;
		calib.chan = GND_REF_CHAN_ID;
		calib.mode = VADC_MODE_NORMAL;
		calib.adc_param = VADC_DECIM_RATIO_VAL;
		calib.hw_set_time = 0;
		calib.fast_avg = 0;

		calib2 = adc_configure(&calib);

		mul = VREF_18_V / (calib1 - calib2);
		calib_result = (result - calib2) * mul;
	}

	return calib_result;
}

/*
 * This API takes channel number as input
 * & returns the calibrated voltage as output
 * The calibrated result is the voltage in uVs
 */
uint32_t pm8x41_adc_channel_read(uint16_t ch_num)
{
	struct adc_conf *adc;
	uint16_t result;
	uint32_t calib_result;

	adc = get_channel_prop(ch_num);
	if (!adc) {
		dprintf(CRITICAL, "Error: requested channel is not supported: %u\n", ch_num);
		return 0;
	}

	result = adc_configure(adc);

	calib_result = vadc_calibrate(result, adc->calib_type);

	dprintf(SPEW, "Result: Raw %u\tCalibrated:%u\n", result, calib_result);

	return calib_result;
}

/*
 * This function configures the maximum
 * current for USB in uA
 */
int pm8x41_iusb_max_config(uint32_t current)
{
	uint32_t mul;

	if(current < IUSB_MIN_UA || current > IUSB_MAX_UA) {
		dprintf(CRITICAL, "Error: Current value for USB are not in permissible range\n");
		return -1;
	}

	if (current == USB_CUR_100UA)
		mul = 0x0;
	else if (current == USB_CUR_150UA)
		mul = 0x1;
	else
		mul = current / USB_CUR_100UA;

	REG_WRITE(IUSB_MAX_REG, mul);

	return 0;
}

/*
 * This function configures the maximum
 * current for battery in uA
 */
int pm8x41_ibat_max_config(uint32_t current)
{
	uint32_t mul;

	if(current < IBAT_MIN_UA || current > IBAT_MAX_UA) {
		dprintf(CRITICAL, "Error: Current value for BAT are not in permissible range\n");
		return -1;
	}

	mul = (current - BAT_CUR_100UA) / BAT_CUR_STEP;

	REG_WRITE(IBAT_MAX_REG, mul);

	return 0;
}

/*
 * API: pm8x41_chgr_vdd_max_config
 * Configure the VDD max to i/p value
 */
int pm8x41_chgr_vdd_max_config(uint32_t vol)
{
	uint8_t mul;

	/* Check for permissible range of i/p */
	if (vol < VDD_MIN_UA || vol > VDD_MAX_UA)
	{
		dprintf(CRITICAL, "Error: Voltage values are not in permissible range\n");
		return -1;
	}

	/* Calculate the multiplier */
	mul = (vol - VDD_MIN_UA) / VDD_VOL_STEP;

	/* Write to VDD_MAX register */
	REG_WRITE(CHGR_VDD_MAX, mul);

	return 0;
}

/*
 * API: pm8x41_chgr_ctl_enable
 * Enable FSM-controlled autonomous charging
 */
int pm8x41_chgr_ctl_enable(uint8_t enable)
{
	/* If charging has to be enabled?
	 * 1. Enable charging in charge control
	 * 2. Enable boot done to enable charging
	 */
	if (enable)
	{
		REG_WRITE(CHGR_CHG_CTRL, (CHGR_ENABLE << CHGR_EN_BIT));
		REG_WRITE(MISC_BOOT_DONE, (BOOT_DONE << BOOT_DONE_BIT));
	}
	else
		REG_WRITE(CHGR_CHG_CTRL, CHGR_DISABLE);

	return 0;
}

/*
 * API: pm8x41_get_batt_voltage
 * Get calibrated battery voltage from VADC, in UV
 */
uint32_t pm8x41_get_batt_voltage()
{
	uint32_t voltage;

	voltage = pm8x41_adc_channel_read(VADC_BAT_VOL_CHAN_ID);

	if(!voltage)
	{
		dprintf(CRITICAL, "Error getting battery Voltage\n");
		return 0;
	}

	return voltage;
}

/*
 * API: pm8x41_get_voltage_based_soc
 * Get voltage based State Of Charge, this takes vdd max & battery cutoff
 * voltage as i/p in uV
 */
uint32_t pm8x41_get_voltage_based_soc(uint32_t cutoff_vol, uint32_t vdd_max)
{
	uint32_t vol_soc;
	uint32_t batt_vol;

	batt_vol = pm8x41_get_batt_voltage();

	if(!batt_vol)
	{
		dprintf(CRITICAL, "Error: Getting battery voltage based Soc\n");
		return 0;
	}

	if (cutoff_vol >= vdd_max)
	{
		dprintf(CRITICAL, "Cutoff is greater than VDD max, Voltage based soc can't be calculated\n");
		return 0;
	}

	vol_soc = ((batt_vol - cutoff_vol) * 100) / (vdd_max - cutoff_vol);

	return vol_soc;
}

/*
 * API: pm8x41_enable_mpp_as_adc
 * Configurate the MPP pin as the ADC feature.
 */
void pm8x41_enable_mpp_as_adc(uint16_t mpp_num)
{
	uint32_t val;
	if(mpp_num > MPP_MAX_NUM)
	{
		dprintf(CRITICAL, "Error: The MPP pin number is unavailable\n");
		return;
	}
	/* set the MPP mode as AIN */
	val = (MPP_MODE_AIN << Q_REG_MODE_SEL_SHIFT) \
			| (0x1 << Q_REG_OUT_INVERT_SHIFT) \
			| (0x0 << Q_REG_SRC_SEL_SHIFT);
	REG_WRITE((MPP_REG_BASE + mpp_num * MPP_REG_RANGE + Q_REG_MODE_CTL), val);

	/* Enable the MPP */
	val = (MPP_MASTER_ENABLE << Q_REG_MASTER_EN_SHIFT);
	REG_WRITE((MPP_REG_BASE + mpp_num * MPP_REG_RANGE + Q_REG_EN_CTL), val);

	/* AIN route to AMUX8 */
	val = (MPP_AIN_ROUTE_AMUX3 << Q_REG_AIN_ROUTE_SHIFT);
	REG_WRITE((MPP_REG_BASE + mpp_num * MPP_REG_RANGE + Q_REG_AIN_CTL), val);

}
