/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __PM_FG_ADC_USR_H__
#define __PM_FG_ADC_USR_H__

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

typedef enum{
  /*IRQ indicating that IBAT is greater than threshold defined in IBAT_INT__THR register
    0x0 : INT_RT_STATUS_LOW
    0x1 : INT_RT_STATUS_HIGH */
  PM_FG_ADC_USR_IBAT_GT_THR_RT_STS =0,
  /*IRQ indicating that VBAT is less than threshold defined in VBAT_INT__THR register
    0x0 : INT_RT_STATUS_LOW
    0x1 : INT_RT_STATUS_HIGH */
  PM_FG_ADC_USR_VBAT_LT_THR_INT_RT_STS,
  PM_FG_ADC_USR_IRQ_INVALID
} pm_fg_adc_usr_irq_type;


typedef enum{
  /*STS 
    00 = lpm - low power mode
    01 = hpm - high power mode
    10 = mpm - medium (normal) power mode
    11 = not used
    0x0 : BCL_IS_LPM
    0x1 : BCL_IS_HPM
    0x2 : BCL_IS_MPM
    0x3 : BCL_IS_INVALID_MODE */
  PM_FG_ADC_USR_BCL_MODE_STS_LPM =0,
  PM_FG_ADC_USR_BCL_MODE_STS_HPM,
  PM_FG_ADC_USR_BCL_MODE_STS_MPM,
  PM_FG_ADC_USR_BCL_MODE_STS_INVAID
} pm_fg_adc_usr_bcl_mode_sts;


/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
  /**
  * @brief This function enables irq
  * 
  * @details
  * This function enables irq
  * 
  * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
  * @param[in] pm_fg_adc_usr_irq_type               irq type
  * @param[in] boolean        enable/disable value
  *
  * @return  pm_err_flag_type 
  *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
  *          version of the PMIC.
  *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
  *
  */
  pm_err_flag_type pm_fg_adc_usr_irq_enable(uint32 device_index, pm_fg_adc_usr_irq_type irq, boolean enable);
  
  
  /**
  * @brief This function clears irq
  * 
  * @details
  *  This function clears irq
  * 
  * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
  * @param[in] pm_fg_adc_usr_irq_type               irq type
  *
  *
  * @return  pm_err_flag_type 
  *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
  *          version of the PMIC.
  *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
  *
  */
  pm_err_flag_type pm_fg_adc_usr_irq_clear(uint32  device_index, pm_fg_adc_usr_irq_type irq);
  
  /**
  * @brief This function sets interrupt triggers
  * 
  * @details
  *  This function sets interrupt triggers
  * 
  * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
  * @param[in] pm_fg_adc_usr_irq_type               irq type
  * @param[in] pm_irq_trigger_type               trigger
  *
  * @return  pm_err_flag_type 
  *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
  *          version of the PMIC.
  *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
  *
  */
  pm_err_flag_type pm_fg_adc_usr_irq_set_trigger(uint32 device_index, pm_fg_adc_usr_irq_type irq, pm_irq_trigger_type trigger);
  
  
  /**
  * @brief This function returns irq status
  * 
  * @details
  *  This function returns irq status
  * 
  * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
  * @param[in] pm_fg_adc_usr_irq_type        irq type
  * @param[in] pm_irq_status_type        irq status type
  * @param[in] boolean        irq TRUE/FALSE status
  *
  * @return  pm_err_flag_type 
  *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
  *          version of the PMIC.
  *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
  *
  */
  pm_err_flag_type pm_fg_adc_usr_irq_status(uint32 device_index, pm_fg_adc_usr_irq_type irq, pm_irq_status_type type, boolean *status);

/**
* @brief This function enable/disables BMS Fule Gauge Algorithm BCL (battery current limiting s/w use) *
* @details
*  This function enable/disables BMS Fule Gauge Algorithm BCL (battery current limiting s/w use) 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_enable_bcl_monitoring(uint32 device_index, boolean enable);

/**
* @brief This function returns BMS Fule Gauge BCL (battery current limiting s/w use) *
* @details
* This function returns BMS Fule Gauge BCL (battery current limiting s/w use)  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_monitoring_sts(uint32 device_index, boolean *enable);


/**
* @brief This function returns of status of battery parameter update request *
* @details
*  This function returns of status of battery parameter update request (VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP) 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_access_bat_req_sts(uint32 device_index, boolean *enable);


/**
* @brief This function enable/disables the updates of VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP *
* @details
*  This function enable/disables the updates of VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_set_access_bat_req(uint32 device_index, boolean enable);


/**
* @brief This function returns the grant status of battery parameter update request *
* @details
*  This function returns of status of battery parameter update request (VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP) 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_access_bat_grnt_sts(uint32 device_index, boolean *enable);



/**
* @brief This function returns status RDY bit after battery parameter update request is serviced *
* @details
*  After the first readings from ADC are obtained, this bit is set to 1; At reset and shutdown, this bit gets automatically cleared 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_values(uint32 device_index, boolean *enable);


/**
* @brief This function returns battery ADC Voltage *
* @details
*  8 bit signed partial ADC value, MSB = 0 is positive voltage (positive number), only positive voltages are captured, 1 LSB = 39 mV 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]vbat_adc  Battery Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_vbat(uint32 device_index, uint32 *vbat_adc);


/**
* @brief This function returns battery ADC Current *
* @details
*  8 bit signed partial ADC value, MSB = 0 is discharging current (positive number), only discharging currents are captured, 1 LSB = 39 mA 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]ibat_adc  Battery Current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_ibat(uint32 device_index, int32 *ibat_adc);


/**
* @brief This function returns minimum battery Voltage *
* @details
*  Running Vbat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]vbat_min  Battery Minimum Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_vbat_min(uint32 device_index, uint8 *vbat_min);

/**
* @brief This function returns max battery current *
* @details
*  Running Ibat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]ibat_max  Battery Maximum Current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_ibat_max(uint32 device_index, uint8 *ibat_max);


/**
* @brief This function returns copy of minimum battery Voltage *
* @details
*  Running Vbat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]vbat_min  Battery Minimum Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_vbat_min_cp(uint32 device_index, uint8 *vbat_min_cp);

/**
* @brief This function returns copy of max battery current *
* @details
*  Running Ibat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]ibat_max  Battery Maximum Current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_ibat_max_cp(uint32 device_index, uint8 *ibat_max_cp);


/**
* @brief This function returns Battery Resitance in HALF encoding *
* @details
*  HALF-FLOATING point encoding, 15:11 exp, bit 10 sign, 9:0 mantissa, 1=1 ohm, refer to MDOS for encoding info
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]batt_resistance  Battery Resistance 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_half_point_encoding(uint32 device_index, uint16 *batt_resistance);


/**
* @brief This function returns BCL mode status *
* @details
*  This function returns BCL mode status 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out] bcl_mode  BCL mode
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_mode(uint32 device_index, pm_fg_adc_usr_bcl_mode_sts *bcl_mode);


/**
* @brief This function returns votlage gain correction value for battery voltage *
* @details
*  This function returns gain correction value for battery 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] gainCorrection  for battery volatge
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_v_gain_batt(uint32 device_index, int32 *v_gain_correction);

/**
* @brief This function returns current Rsense gain correction value for Battery Current *
* @details
*  This function returns current Rsense gain correction value for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] gainCorrection  for battery volatge
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_gain_rsense(uint32 device_index, int32 *i_gain_rsense);


/**
* @brief This function returns current Rsense offset value for Battery Current *
* @details
*  This function returns current Rsense offset value for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] ioffset_rsense  Offset Gain Correction battery current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_offset_rsense(uint32 device_index, int32 *i_offset_rsense);


/**
* @brief This function returns current gain in BATFET for Battery Current *
* @details
*  This function returns current gain in BATFET for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] igain_offset_correction  Gain Correction for battery current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_gain_batfet(uint32 device_index, int32 *i_gain_batfet);


/**
* @brief This function returns current offset in BATFET for Battery Current *
* @details
*  This function returns current gain in BATFET for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] ioffset_batfet_correction  offset Gain Correction for BATFET
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_offset_batfet(uint32 device_index, int32 *i_offset_batfet);


/**
* @brief This function returns source used for current sense *
* @details
*  This function returns source used for current sense  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] isense_source  source used for current sense
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_sense_source(uint32 device_index, boolean *i_sense_source);


/**
* @brief This function cleras stored VBAT minimum  *
* @details
* This function cleras stored VBAT minimum  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable for clearing stored vbat minimum
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_clear_vbat_min(uint32 device_index, boolean enable);

/**
* @brief This function clears stored IBAT Max *
* @details
* This function cleras stored IBAT Max 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable for clearing stored vbat minimum
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_clear_ibat_max(uint32 device_index, boolean enable);

/**
* @brief This function returns battery charge current after gain and offset calibration*
* @details
*  calibrated_value = ((raw_data + offset) * (1 + gain))
* 
* @param[in] pmic_device_index  Primary: 0 Secondary: 1
* @param[out]calibrated_vbat    Calibrated Battery Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_calibrated_ibat(uint32 pmic_device, int32 *calibrated_ibat);

/**
* @brief This function returns battery ADC Voltage after gain calibration*
* @details
*  calibrated_value = raw_data * (1 + gain)
* 
* @param[in] pmic_device_index  Primary: 0 Secondary: 1
* @param[out]calibrated_vbat    Calibrated Battery Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_calibrated_vbat(uint32 pmic_device, uint32 *calibrated_vbat);

/**
* @brief This function returns battery ADC Voltage after gain calibration*
*  This function with fg driver's initialize and battery status checking
* @details
*  calibrated_value = raw_data * (1 + gain)
*
* @param[in] pmic_device_index  Primary: 0 Secondary: 1
* @param[out]calibrated_vbat    Calibrated Battery Voltage
*
* @return  pm_err_flag_type
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_usr_get_vbat(uint32 pmic_device, uint32 *calibrated_vbat);
#endif /* __PM_FG_ADC_USR_H__ */
