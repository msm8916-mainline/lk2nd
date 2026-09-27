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

#ifndef PM_SMBCHG_CHGR_H
#define PM_SMBCHG_CHGR_H

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*! \enum pm_smbchg_chgr_irq_bit_field_type
   \brief different types of irq bit fields of by smbb irq module
 */
typedef enum {
  PM_SMBCHG_CHGR_CHGR_ERROR,
  PM_SMBCHG_CHGR_GR_INHIBIT,
  PM_SMBCHG_CHGR_CHGR_PRECHG_SFT,
  PM_SMBCHG_CHGR_CHGR_COMPLETE_CHG_SFT,
  PM_SMBCHG_CHGR_BAT_FT_P2F_CHG_THRESHOLD,
  PM_SMBCHG_CHGR_BAT_LT_RECHG_THRESHOLD,
  PM_SMBCHG_CHGR_BAT_TAPER_MODE_CHARGING,
  PM_SMBCHG_CHGR_BAT_TCC_REACHED,
  PM_SMBCHG_CHGR_IRQ_INVALID
}pm_smbchg_chgr_irq_type;


typedef enum
{
  PM_SMBCHG_CHGR_NO_CHARGING,
  PM_SMBCHG_CHGR_PRE_CHARGING,
  PM_SMBCHG_CHGR_FAST_CHARGING,
  PM_SMBCHG_CHGR_TAPER_CHARGING,
  PM_SMBCHG_CHGR_CHARGING_INVALID
}pm_smbchg_chgr_charging_type;

typedef struct
{
  boolean is_net_i_discharg;
  /* is_net_discharg_i: FALSE = Net charging current, TRUE = Net dis-charging current*/
  boolean is_auto_fv_comp_active;
  /*automatic float voltage compensation status. FALSE=Inactive, TRUE=Active*/
  boolean is_chgr_done_once;
  /*Done Status, FALSE = No charging cycles have terminated since Charging first enabled, 
    TRUE = At least 1 charging cycle has terminated since Charging first enabled */
  boolean is_batt_lt_2v;
  /*battery voltage less than 2v*/
  boolean is_chgr_in_hold_off;
  /*charging hold off status. FALSE: charger not in hold off; TRUE: charger in hold off*/
  pm_smbchg_chgr_charging_type charging_type;
  /*type of charging happening*/
  boolean charger_enable;
  /*FALSE= charging command/pin is not asserted. TRUE = charging command/pin asserted*/
}pm_smbchg_chgr_chgr_status_type;


typedef enum
{
  PM_SMBCHG_CHGR_CCMPN__COLD_SL_CHG_I_COMP,    /** <Cold SL Charge Current Compensation              >*/
  PM_SMBCHG_CHGR_CCMPN__HOT_SL_CHG_I_COMP,     /** <Hot SL Charge Current Compensation               >*/
  PM_SMBCHG_CHGR_CCMPN__COLD_SL_FV_COMP,       /** <Cold SL Float Voltage Compensation               >*/
  PM_SMBCHG_CHGR_CCMPN__HOT_SL_FV_COMP,        /** <Hot SL Float Voltage Compensation                >*/
  PM_SMBCHG_CHGR_CCMPN__LOAD_BAT,              /** <Load Battery during Float Voltage Compensation   >*/
  PM_SMBCHG_CHGR_CCMPN__JEITA_TEMP_HARD_LIMIT, /** <Jeita Temperature Hard Limit disable             >*/
  PM_SMBCHG_CHGR_CCMPN__TEMP_MONITOR_ENABLE,   /** <Temperature Monitor Enabled when input is present>*/
  PM_SMBCHG_CHGR_CCMPN__INVALID                /** < Invalid                                         >*/
}pm_smbchg_chgr_ccmpn_type;


//output that can be used as an input or a systerm O.k. indicator
typedef enum
{ //INOK output is asserted to indicate valid input adapter presence
  PM_SMBCHG_CHGR_SYSOK_OPT__INOK_OPT_1, //The INOK is asserted only per the input voltage level and as soon as the glitch filter timer has expired
  PM_SMBCHG_CHGR_SYSOK_OPT__INOK_OPT_2, //The INOK is asserted only when the input FET is on and the charger buck soft-start has completed and only after the APSD algorithm has completed (if enabled)
  PM_SMBCHG_CHGR_SYSOK_OPT__SYSOK_OPT_A_1, //the SYSOK goes active LOW when no valid input power is present and the battery voltage is lower than the programmable low-battery/SYSOK voltage threshold (VLOWBATT) or the battery is missing
  PM_SMBCHG_CHGR_SYSOK_OPT__SYSOK_OPT_A_2, //the SYSOK goes active LOW when the battery voltage is lower than the programmable low-battery / SYSOK voltage threshold (VLOWBATT) or the battery is missing, regardless of the input being present
  PM_SMBCHG_CHGR_SYSOK_OPT__SYSOK_OPT_BC_1,
  PM_SMBCHG_CHGR_SYSOK_OPT__SYSOK_OPT_BC_2,
  PM_SMBCHG_CHGR_SYSOK_OPT__CHG_DET_OPT_1,
  PM_SMBCHG_CHGR_SYSOK_OPT__CHG_DET_OPT_2,
  PM_SMBCHG_CHGR_SYSOK_OPT__INVALID
} pm_smbchg_chgr_sysok_opt_type;

typedef struct
{
  boolean cont_without_fg_ready; //do not holdoff charging and continue without FG Ready
  boolean is_rchg_threshold_src_fg; //Recharge Threshold Source: TRUE:Fuel GaugeADC; FALSE:Analog sensor
  boolean is_term_current_src_fg; //TRUE: Early current termination source fuel gauge ADC; FALSE: Analog sensor
  boolean en_early_current_termination; //Early Current Termination
  pm_smbchg_chgr_sysok_opt_type sysok_opt;
  boolean en_sysok_pol_table_n; //TRUE: Inverse of Table in DOS; FALSE:Table in DOS
  boolean en_charger_inhibit;
  boolean is_holdoff_tmr_350ms; // TRUE: 350ms and FALSE: 700us
  boolean auto_rchg_dis; //TRUE: disable automatic recharge; FALSE: enable automatic discharge
  boolean current_termination_dis; //TRUE: current termination disabled, FALSE: current termination enabled
  boolean batt_ov_ends_cycle_en; //battery over voltage ends charge cycle
  boolean p2f_chg_tran_require_cmd; //TRUE: pre to fast charge transition requires command; FALSE: automatic transation
}pm_chgr_chgr_cfg_type;


typedef enum pm_smbchg_chgr_sfty_timer_type
{
  PM_SMBCHG_CHGR_SFTY_TIMER__PC_EN_TC_EN, //Pre-Charge and Total Charge Safety Timers Enabled
  PM_SMBCHG_CHGR_SFTY_TIMER__PC_DIS_TC_EN, // Pre-Charge Safety Timer Disabled, Total Charge Safety Timer Enabled
  PM_SMBCHG_CHGR_SFTY_TIMER__PC_DIS_TC_DIS, //Pre-Charge and Total Charge Safety Timers Disabled
  PM_SMBCHG_CHGR_SFTY_TIMER__INVAID_TYPE
} pm_smbchg_chgr_sfty_timer_type;

/*===========================================================================

                 SMBCHG DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/**
* @brief This function returns if battery is above VBAt_WEAK or 
*        not
*  
* @details
*  This API returns if battery is above VBAT_WEAK threshold or
*  not
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] is_above_vbat_weak if above then TRUE else FLASE 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_vbat_sts(uint32 device_index, boolean *is_above_vbat_weak);


/**
* @brief This function returns the float vlotage status
* 
* @details
*  This API returns the float vlotage status
* 
* @param[in]  pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] aicl_in_hard_limit  TRUE if AICL is hard limit else FALSE
* @param[out] fv_mv               Float voltage value; 3600mv to 4500mv
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_float_volt_sts(uint32 device_index, boolean *aicl_in_hard_limit, uint32 *fv_mv);


/**
* @brief This function returns the pre charger and fast charger current status in milliam
* 
* @details
*  This API returns the pre charger and fast charger current status in milliamp
* 
* @param[in] pmic_device_index.      Primary: 0 Secondary: 1
* @param[in] pre_charge_current_ma   Pre-charge current value in milli amp.
* @param[in] fast_charge_current_ma  Fast charge current value in milli amp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_chg_i_sts(uint32 device_index, uint32 *pre_charge_current_ma, uint32 *fast_charge_current_ma);


/**
* @brief This function returns charger status
* 
* @details
*  This function returns status  for charging/discharging, active/inactive AFV compensation, charging
*  done at least once, battery voltage less than 2v, charge hold off, current charging state, charging 
*  enable/disable status
* 
* @param[in]  pmic_device_index.                  Primary: 0 Secondary: 1
* @param[out] pm_smbchg_chgr_chgr_status_type     Refer enum pm_smbchg_chgr_chgr_status_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_chgr_sts(uint32 device_index, pm_smbchg_chgr_chgr_status_type *chgr_sts);


/**
* @brief This function configures the pre-charger current value in milliamp. Valid values are 100 to 550 mAmp
* 
* @details
*  This API configures the pre-charger current value in milliamp. Valid values are 100 to 550 mAmp
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pre_chg_i_ma        Pre charge current in milliamp. Values are 100 to 550mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_pre_chg_i(uint32 device_index, uint32 pre_chg_i_ma);


/**
* @brief This function configures the fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp
* 
* @details
*  This API configures the fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] fast_chg_i_ma        fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_fast_chg_i(uint32 device_index, uint32 fast_chg_i_ma);


/**
* @brief This function configures the fast-charger current compensation value in milliamp
* 
* @details
*  This API configures the fast-charger current compensation value in milliamp.
*  Valid values are 300 to 1200 mAmp
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] fast_chg_i_cmp_ma   fast-charger current compensation value in milliamp. 
*                                Valid values are 300 to 1200 mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_fast_chg_i_cmpn(uint32 device_index, uint32 fast_chg_i_cmp_ma);


/**
* @brief This function This API configures the floating voltage
* 
* @details
* This API configures the floating voltage. Valid range is 3600mV to 4500 mv
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] float_volt_mv       Valid range is 3600mV to 4500 mv
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_float_volt(uint32 device_index, uint32 float_volt_mv);

/**
* @brief This function returns configured floating voltage
* 
* @details
*  This function returns set floating voltage
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] float_volt_mv      set float voltage 
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_float_volt_cfg(uint32 device_index, uint32 *float_volt_mv);


/**
* @brief This function sets floating voltage conpmensation code
* 
* @details
*  This api sets floating voltage conpmensation code. Valid
*  value is from 0 to 63
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] float_volt_cmpn     Valid value is from 0 to 63 
*                         
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_float_volt_cmpn(uint32 device_index, uint32 float_volt_cmpn);


/**
* @brief This function reads floating voltage conpmensation code
* 
* @details
*  This api reads floating voltage conpmensation code. Valid
*  value is from 0 to 6300mV
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] float_volt_cmpn     Valid value is from 0 to 6300 mV 
*                         
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_float_volt_cmpn(uint32 device_index, uint32 *float_volt_cmpn);


/**
* @brief This function enables/disables Charger Auto Float 
*        Voltage Compensation
* 
* @details
*  This api enables/disables Charger Auto Float Voltage Compensation
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] enable             TRUE enables and FALSE disables
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_en_afvc(uint32 device_index, boolean enable);


/**
* @brief This function sets charge inhibit level
* 
* @details
*  This API sets charge inhibit level
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] inhibit_lvl_mv      valid range is 50 to 300mv 
*       
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_inhibi_threshold(uint32 device_index, uint32 inhibit_lvl_mv);


/**
* @brief This function returns charge inhibit level
* 
* @details
*  This API returns charge inhibit level
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] inhibit_lvl_mv      valid range is 50 to 300mv 
*       
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_inhibi_threshold(uint32 device_index, uint32 *inhibit_lvl_mv);


/**
* @brief This function sets the pre-charge to full charger 
*        threshold
* 
* @details
*  This API sets the pre-charge to full charger threshold. Valid range is 2400mV to 3000mV
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] milli_volt          Valid range is 2400mV to 3000mV
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_p2f_threshold(uint32 device_index, uint32 milli_volt);


/**
 * @brief This function returns the pre-charge to full charger 
 *        threshold
 * 
 * @details
 *  This API returns the pre-charge to full charger threshold.
 *  Valid range is 2400mV to 3000mV
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] milli_volt          Valid range is 2400mV to 
 *       3000mV
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_chgr_get_p2f_threshold(uint32 device_index, uint32 *milli_volt);


/**
* @brief This function sets the charge termination current
* 
* @details
*  This API sets the charge termination current in milliamp 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] current_ma          Termination current. Value range is 50 to 600 milliAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_set_charge_termination_current(uint32 device_index, uint32 current_ma);


/**
* @brief This function returns the charge termination current
* 
* @details
*  This API returns the charge termination current in milliamp 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] current_ma          Termination current. Value range is 50 to 600 milliAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_charge_termination_current(uint32 device_index, uint32 *current_ma);


/**
* @brief This function  configures charger temperature compensation
* 
* @details
*  This function configures charger temperature compensation
* 
* @param[in]  pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] ccmpn_type          Refer pm_smbchg_ccmpn_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_config_chgr_temp_cmpn(uint32 device_index, pm_smbchg_chgr_ccmpn_type ccmpn_type, boolean enable);


/**
* @brief This function returns configured charge temperature compensation
* 
* @details
*  This function returns configured charge temperature compensation
* 
* @param[in]  pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] ccmpn_type           Refer pm_smbchg_ccmpn_type for more info
* @param[in] enable               TRUE : set, FALSE: clear                                        
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_chgr_temp_cmpn_config(uint32 device_index, pm_smbchg_chgr_ccmpn_type ccmpn_type, boolean *enable);


/**
* @brief This function configures charger related paramters. 
* 
* @details
*  This function configures charger related paramters.
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chgr_cfg_type       Refer struct pm_chgr_chgr_cfg_type for more info
* @param[out] enable             TRUE : set, FALSE: clear     
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_config_chgr(uint32 device_index, pm_chgr_chgr_cfg_type *chgr_cfg_type);


/**
* @brief This function returns configured charger related 
*        paramters.
* 
* @details
*  This function returns configured charger related paramters.
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] chgr_cfg_type       Refer struct pm_chgr_chgr_cfg_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_chgr_config(uint32 device_index, pm_chgr_chgr_cfg_type *chgr_cfg_type);

/**
* @brief This function configures safety timer
* 
* @details
*  This function configures safety timer
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] pre_chg_sfty_min              Total pre-charge safety timer timeout. Valid values is 24 to 192 minutes
* @param[in] total_chg_sfty_min            Total charge safety timer timeout. Valid values is 192 to  1536 minutes
* @param[in] sfty_timer_type               refer pm_smbchg_chgr_sfty_timer_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_config_sfty_timer(uint32 device_index, uint32 pre_chg_sfty_min, uint32 total_chg_sfty_min, pm_smbchg_chgr_sfty_timer_type sfty_timer_type);


/**
* @brief This function returns configured safety timer
* 
* @details
*  This function returns configured safety timer
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] pre_chg_sfty_min              Total pre-charge safety timer timeout. Valid values is 24 to 192 minutes
* @param[out] total_chg_sfty_min            Total charge safety timer timeout. Valid values is 192 to  1536 minutes
* @param[out] sfty_timer_type               refer pm_smbchg_chgr_sfty_timer_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_get_sfty_timer_config(uint32 device_index, uint32 *pre_chg_sfty_min, uint32 *total_chg_sfty_min, pm_smbchg_chgr_sfty_timer_type *sfty_timer_type);


/**
* @brief This function enables irq
* 
* @details
*  This function enables irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_chgr_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_irq_enable(uint32 device_index, pm_smbchg_chgr_irq_type irq, boolean enable);


/**
* @brief This function clears the SMBCHG irq 
* 
* @details
*  This function clears the SMBCHG irq 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_chgr_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_irq_clear(uint32  device_index, pm_smbchg_chgr_irq_type irq);


/**
* @brief This function configures the SMBCHG irq trigger 
* 
* @details
*  This function configures the SMBCHG irq trigger 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_chgr_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_irq_set_trigger(uint32 device_index, pm_smbchg_chgr_irq_type irq, pm_irq_trigger_type trigger);


/**
* @brief This function configures the SMBCHG for irq 
* 
* @details
*  This function configures the SMBCHG for irq 
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_chgr_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgr_irq_status(uint32 device_index, pm_smbchg_chgr_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function enables source as command or pin 
 *        controlled.
 * 
 * @details
 *  This function enables source as command or pin controlled.
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] chg_en_src          TRUE: Enables Pin 
 *                                FALSE: Command Register 
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_chgr_enable_src(uint32 device_index, boolean chg_en_src);

/**
 * @brief This function sets the command/pin polarity 
 * 
 * @details
 *  This function sets the command/pin polarity .
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] chg_pol_low         TRUE: sets polarity low
 *                                FASLE: sets polarity high
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_chgr_set_chg_polarity_low(uint32 device_index, boolean chg_pol_low);

#endif /* PM_SMBCHG_CHGR_H */

