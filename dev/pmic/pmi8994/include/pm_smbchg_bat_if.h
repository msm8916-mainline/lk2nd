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

#ifndef PM_SMBCHG_BAT_IF_H
#define PM_SMBCHG_BAT_IF_H

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/


/*! \struct pm_smbchg_bat_if_cmd_chg_type
   \brief Struct for writing to battery interface command control
 */
typedef enum pm_smbchg_bat_if_cmd_chg_type
{
   PM_SMBCHG_BAT_IF_CMD__OTG_EN,               /** <OTG Enable, 0 = Disable, 1 = Enable                                                             >*/
   PM_SMBCHG_BAT_IF_CMD__EN_BAT_CHG,           /** <Enable Battery Charging                                                                         >*/
   PM_SMBCHG_BAT_IF_CMD__FC_COM,               /** <Fast Charge Command, FALSE = Force pre-charge current, TRUE = Allow Fast Charge                 >*/
   PM_SMBCHG_BAT_IF_CMD__STAT_OUTPUT,          /** <0 = STAT output enabled (still allow IRQs) 1 = Turn off STAT pin (still allow IRQs)             >*/
   PM_SMBCHG_BAT_IF_CMD__THERM_NTC_I_OVERRIDE, /** <0 = Thermistor Monitor per config setting 1 = Force thermistor monitor disable (override config)>*/
   PM_SMBCHG_BAT_IF_CMD__WIRELESS_CHG_DIS,     /** <Wireless charging 0 = enabled, 1 = disabled                                                     >*/
   PM_SMBCHG_BAT_IF_CMD__INVALID               /** <INVALID                                                                                         >*/
}pm_smbchg_bat_if_cmd_chg_type;


/*! \enum pm_smbchg_bat_if_low_bat_thresh_type
   \brief enum for different level of low battery threshold 
 */
typedef enum pm_smbchg_bat_if_low_bat_thresh_type
{
   PM_SMBCHG_BAT_IF_LOW_BATTERY_DISABLED,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_2P5,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_2P6,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_2P7,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_2P8,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_2P9,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P0,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P1,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P7,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_2P88,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P00,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P10,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P25,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P35,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P46,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P58,
   PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_INVALID
}pm_smbchg_bat_if_low_bat_thresh_type;


/*! \struct pm_smbchg_bat_if_cmd_chg_type
   \brief Struct for configuring battery missing configuration
 */
typedef struct pm_smbchg_bat_if_batt_missing_cfg_type
{
   uint32  batt_removal_det_time_usec;       /** <Battery removal detection time: 80,160,320,640 usec > */
   boolean batt_bat_get_override_en;         /** <Battery FET Configuration 0 = Normal operation 1 = override (turn off FET) 0x0 : BATT_FET_NORMAL 0x1 : BATT_FET_OVERRIDE > */
   boolean batt_missing_input_plugin_en;     /** <Battery Missing on Input Plug-In 0 = Disabled 1 = Enabled 0x0 : BMA_PLUG_IN_DIS 0x1 : BMA_PLUG_IN_EN > */
   boolean batt_missing_2p6s_poller_en;      /** <Battery Missing 2.6s Poller 0 = Disabled 1 = Enabled 0x0 : BMA_POLLER_DIS 0x1 : BMA_POLLER_EN > */
   boolean batt_missing_algo_en;             /** <Battery Missing Algorithm 0 = Disabled 1 = Enabled 0x0 : BMA_DIS 0x1 : BMA_EN > */
   boolean use_bmd_pin_for_batt_missing_src;
   boolean use_therm_pin_for_batt_missing_src;
}pm_smbchg_bat_if_batt_missing_cfg_type;

/*! \enum pm_smbchg_bat_miss_detect_src_type
   \brief enum for battery missing detection sources
 */
typedef enum pm_smbchg_bat_miss_detect_src_type
{
   PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_THERM_PIN,
   PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_BMD_PIN,
   PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_THERM_BMD_PIN,
   PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_NONE,
   PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_INVALID,
}pm_smbchg_bat_miss_detect_src_type;


/*! \enum pm_smbchg_bat_if_dcin_icl_type
   \brief enum for configuring dcin input current limit mode
 */
typedef enum pm_smbchg_bat_if_dcin_icl_type
{
   PM_SMBCHG_BAT_IF_DCIN_PASS_THROUGH_MODE, /** < pass through mode and ~9.8v                         >*/
   PM_SMBCHG_BAT_IF_DCIN_LOW_VOLT_MODE,     /** <low volt mode, DCIN less than 6.5v, DIV2_EN =1       >*/
   PM_SMBCHG_BAT_IF_DCIN_HIGH_VOLT_MODE,    /** <high volt mode DCIN >= 6.5v and max 9v, , DIV2_EN =1 >*/
   PM_SMBCHG_BAT_IF_DCIN_INVALID_MODE,      /** <INVALID mode                                         >*/
}pm_smbchg_bat_if_dcin_icl_type;


/*! \enum pm_smbchg_irq_bit_field_type
   \brief different types of irq bit fields of by smbch_bat_if irq module
 */
typedef enum {
   PM_SMBCHG_BAT_IF_HOT_BAT_HARD_LIM,
   PM_SMBCHG_BAT_IF_HOT_BAT_SOFT_LIM,
   PM_SMBCHG_BAT_IF_COLD_BAT_HARD_LIM,
   PM_SMBCHG_BAT_IF_COLD_BAT_SOFT_LIM,
   PM_SMBCHG_BAT_IF_BAT_OV,
   PM_SMBCHG_BAT_IF_BAT_LOW,
   PM_SMBCHG_BAT_IF_BAT_MISSING,
   PM_SMBCHG_BAT_IF_BAT_TERM_MISSING,
   PM_SMBCHG_BAT_IF_IRQ_INVALID
}pm_smbchg_bat_if_irq_type;

/*===========================================================================

                 SMBCHG BAT_IF DRIVER FUNCTION PROTOTYPES

===========================================================================*/


/*Returns battery presence status*/
/**
 * @brief This function returns battery presence status.
 * 
 * @details
 *  This function returns battery presence status..
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]bat_present.  TRUE: Present, FALSE: not present 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_get_bat_pres_status(uint32 device_index, boolean *bat_present);


/*This API clears the dead battery timer*/
/**
 * @brief This function clears the dead battery timer
 * 
 * @details
 *  This function clears the dead battery timer.
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_clear_dead_bat_timer(uint32 device_index);



/**
 * @brief This function writes to battery interface command control to enable/disable wireless charging,
 *         thermal current override, stat output, fast charge, battery charging, OTG .
 * 
 * @details
 *  This function writes to battery interface command control to enable/disable wireless charging,
 *  thermal current override, stat output, fast charge, battery charging, OTG .
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]cmd_chg_cfg.  Refer pm_smbchg_bat_if_cmd_chg_type 
 *                          for more info
 * @param[in]boolean       TRUE: enable, FALSE: disable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_config_chg_cmd(uint32 device_index, pm_smbchg_bat_if_cmd_chg_type cmd_chg_cfg, boolean enable);



 /**
 * @brief This function reads to battery interface command control to enable/disable wireless charging,
 *         thermal current override, stat output, fast charge, battery charging, OTG .
 * 
 * @details
 *  This function reads to battery interface command control to
 *  enable/disable wireless charging, thermal current override,
 *  stat output, fast charge, battery charging, OTG .
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]cmd_chg_cfg.  Refer pm_smbchg_bat_if_cmd_chg_type 
 *                          for more info
 * @param[out]boolean       TRUE: enable, FALSE: disable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_get_chg_cmd(uint32 device_index, pm_smbchg_bat_if_cmd_chg_type cmd_chg_cfg, boolean *enable);

/*Sets low battery voltag threshold*/
/**
 * @brief This function sets low battery voltag threshold.
 * 
 * @details
 *  Sets low battery voltag threshold .
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[in]low_bat_threshold.  Refer pm_smbchg_bat_if_low_bat_thresh_type 
 *                                for more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_set_low_batt_volt_threshold(uint32 device_index, pm_smbchg_bat_if_low_bat_thresh_type low_bat_threshold);


/**
 * @brief This function reads low battery voltag threshold.
 * 
 * @details
 *  Gets low battery voltag threshold .
 * 
 * @param[in] pmic_device_index. Primary: 0 Secondary: 1
 * @param[out]low_bat_threshold.  Refer pm_smbchg_bat_if_low_bat_thresh_type 
 *                                for more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_get_low_bat_volt_threshold(uint32 device_index, pm_smbchg_bat_if_low_bat_thresh_type *low_bat_threshold);



/**
 * @brief This function configures battery missing parameters 
 * 
 * @details
 *  This API sets battery missing detection configuration like select if the
 *  battery missing monitoring should only happen at the beginning of a charge cycle
 *  or every 3 seconds
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] batt_missing_cfg.   Refer pm_smbchg_bat_if_batt_missing_cfg_type 
 *                                for more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_set_bat_missing_cfg(uint32 device_index, pm_smbchg_bat_if_batt_missing_cfg_type *batt_missing_cfg);


/**
 * @brief This function reads battery missing config
 * 
 * @details
 *  This API reads battery missing detection configuration like
 *  select if the battery missing monitoring should only happen
 *  at the beginning of a charge cycle or every 3 seconds
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] batt_missing_cfg.   Refer 
 *                                pm_smbchg_bat_if_batt_missing_cfg_type
 *                                for more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_get_bat_missing_cfg(uint32 device_index, pm_smbchg_bat_if_batt_missing_cfg_type *batt_missing_cfg);


/**
 * @brief This function sets battery missing detection source
 * 
 * @details
 *  This API sets battery missing detection source
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] batt_missing_cfg.   Refer 
 *                                pm_smbchg_bat_miss_detect_src_type
 *                                for more info
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_set_bat_missing_detection_src(uint32 device_index, pm_smbchg_bat_miss_detect_src_type batt_missing_det_src);

/**
 * @brief This function sets the minimum system voltage
 * 
 * @details
 *  This API sets the minimum system voltage. And below this
 *  voltage system and battery are not connected
 *  together.Valid values are 3150, 3450 and 3600 milli volt 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] min_sys_millivolt   Valid value is 3150 mv to 
 *                                3600mV
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_set_min_sys_volt(uint32 device_index, uint32 min_sys_millivolt);


/**
 * @brief This function reads the minimum system voltage
 * 
 * @details
 *  This API reads the minimum system voltage. And below this
 *  voltage system and battery are not connected together.Valid
 *  values are 3150, 3450 and 3600 milli volt
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] min_sys_millivolt   Valid value is 3150 mv to 
 *                                3600mV
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_get_min_sys_volt(uint32 device_index, uint32 *min_sys_millivolt);



/** 
 * @brief This function configures the current limit for pass 
 *        through mode, low volt mode and high volt mode
 * 
 * @details
 *  This API configures the DCIN input current limit for pass
 *  through mode, low volt mode and high volt mode in milliamp
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] dcin_icl_type        refer enum pm_smbchg_bat_if_dcin_icl_type 
 *                                 for more info
 *      
 * @param[in] current_ma           valid value 300 to 2000 mAmp 
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_set_dcin_input_current_limit(uint32 device_index, pm_smbchg_bat_if_dcin_icl_type dcin_icl_type, uint32 current_ma);

/*This API reads the configured current limit for pass through mode, low volt mode and high volt mode in milliamp*/
/** 
 * @brief This function returns the current limit for pass 
 *        through mode, low volt mode and high volt mode
 * 
 * @details
 *  This API returns the DCIN input current limit for pass
 *  through mode, low volt mode and high volt mode in milliamp
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] dcin_icl_type        refer enum pm_smbchg_bat_if_dcin_icl_type 
 *                                 for more info
 *      
 * @param[out]current_ma           valid value 300 to 2000 mAmp 
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_bat_if_get_dcin_input_current_limit(uint32 device_index, pm_smbchg_bat_if_dcin_icl_type dcin_icl_type, uint32 *current_ma);



/**
* @brief This function configures WI PWR timer.
* 
* @details
*  This API configures WI PWR timer. div2 falling edge values:
*  0, 150, 250, 500 usec. wipwr_irq_tmr_us values 1000,
*  1500,2000,2500,3000,4000,4500 usec*
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] div2_falling_edge_time_us   valid values are 0, 150, 250, 500usec 
* @param[in] wipwr_irq_tmr_us               values 1000, 1500,2000,2500,3000,4000,4500 usec 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_bat_if_set_wi_pwr_tmr(uint32 device_index, uint32 div2_falling_edge_time_us, uint32 wipwr_irq_tmr_us);

/**
* @brief This function configures WI PWR timer.
* 
* @details
*  This API configures WI PWR timer. div2 falling edge values:
*  0, 150, 250, 500 usec. wipwr_irq_tmr_us values 1000,
*  1500,2000,2500,3000,4000,4500 usec*
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] div2_falling_edge_time_us   valid values are 0, 150, 250, 500usec 
* @param[in] wipwr_irq_tmr_us               values 1000, 1500,2000,2500,3000,4000,4500 usec 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_bat_if_get_wi_pwr_tmr(uint32 device_index, uint32 *div2_falling_edge_time_us, uint32 *wipwr_irq_tmr_us);


/**
* @brief This function enables IRQ
* 
* @details
*  This function enables IRQ
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_bat_if_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_bat_if_irq_enable(uint32 device_index, pm_smbchg_bat_if_irq_type irq, boolean enable);


/**
* @brief This function clears the SMBCHG irq 
* 
* @details
*  This function clears the SMBCHG irq 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_bat_if_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_bat_if_irq_clear(uint32  device_index, pm_smbchg_bat_if_irq_type irq);

/**
* @brief This function configures the SMBCHG irq trigger 
* 
* @details
*  This function configures the SMBCHG irq trigger 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_bat_if_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_bat_if_irq_set_trigger(uint32 device_index, pm_smbchg_bat_if_irq_type irq, pm_irq_trigger_type trigger);


/**
* @brief This function configures the SMBCHG for irq 
* 
* @details
*  This function configures the SMBCHG for irq 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_bat_if_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_bat_if_irq_status(uint32 device_index, pm_smbchg_bat_if_irq_type irq, pm_irq_status_type type, boolean *status);



#endif /* PM_SMBCHG_BAT_IF_H*/

