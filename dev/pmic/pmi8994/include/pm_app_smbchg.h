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

#ifndef PM_APP_SMBCHG__H
#define PM_APP_SMBCHG__H

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_smbchg_bat_if.h"
#include "pm_smbchg_misc.h"
#include "pm_smbchg_usb_chgpth.h"
#include "pm_resources_and_types.h"

/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/



/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
typedef struct
{
   pm_smbchg_bat_if_low_bat_thresh_type  vlowbatt_threshold;
   pm_smbchg_param_config_enable_type    enable_config;
} pm_smbchg_vlowbatt_threshold_data_type;

typedef struct
{
   pm_smbchg_chgpth_input_priority_type  chgpth_input_priority; //0=DCIN; 1=USBIN
   pm_smbchg_param_config_enable_type    enable_config;
} pm_smbchg_chgpth_input_priority_data_type;


typedef struct
{
   pm_smbchg_bat_miss_detect_src_type  bat_missing_detection_src; 
   pm_smbchg_param_config_enable_type  enable_config;
} pm_smbchg_bat_miss_detect_src_data_type ;


typedef struct
{
   pm_smbchg_wdog_timeout_type         wdog_timeout;
   pm_smbchg_param_config_enable_type  enable_config;
} pm_smbchg_wdog_timeout_data_type;


typedef struct
{
   boolean enable_wdog;
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_enable_wdog_data_type;


typedef struct
{
   uint32 fast_chg_i_ma; //Valid values are 300 to 3000 mAmp
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_fast_chg_current_data_type;


typedef struct
{
   uint32 pre_chg_i_ma; //Valid values are 100 to 550 mAmp
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_pre_chg_current_data_type;


typedef struct
{
   uint32 pre_to_fast_chg_theshold_mv; //Valid range is 2400mV to 3000mV
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_pre_to_fast_chg_threshold_data_type;


typedef struct
{
   uint32 float_volt_theshold_mv; //Valid range is 3600mV to 4500 mv
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_float_voltage_threshold_data_type;

typedef struct
{
   uint32  usbin_input_current_limit; //Valid value is 300 to 3000mAmp
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_usbin_input_current_limit_data_type;


typedef struct
{
   uint32  dcin_input_current_limit; //valid range is 300 to 2000 mAmp
   pm_smbchg_param_config_enable_type enable_config;
} pm_smbchg_dcin_input_current_limit_data_type;




typedef struct
{
    pm_smbchg_vlowbatt_threshold_data_type         dbc_bootup_volt_threshold;
    pm_smbchg_chgpth_input_priority_data_type      chgpth_input_priority;
    pm_smbchg_bat_miss_detect_src_data_type        bat_miss_detect_src;
    pm_smbchg_wdog_timeout_data_type               wdog_timeout;
    pm_smbchg_enable_wdog_data_type                enable_wdog;
    pm_smbchg_fast_chg_current_data_type           fast_chg_i;
    pm_smbchg_pre_chg_current_data_type            pre_chg_i;
    pm_smbchg_pre_to_fast_chg_threshold_data_type  pre_to_fast_chg_theshold_mv;
    pm_smbchg_float_voltage_threshold_data_type    float_volt_theshold_mv;
    pm_smbchg_usbin_input_current_limit_data_type  usbin_input_current_limit;
    pm_smbchg_dcin_input_current_limit_data_type   dcin_input_current_limit;
    uint32  bootup_battery_theshold_mv;
    uint32  wipwr_bootup_battery_theshold_mv;
    boolean enable_jeita_hard_limit_check;
} pm_smbchg_specific_data_type;



/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
/**
 * @name 
 *     pm_sbl_chg_config_vbat_low_threshold
 *
 * @description
 *    This function configures battery charge status 
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_target_information_init()
 */
pm_err_flag_type pm_appsbl_chg_config_vbat_low_threshold(uint32 device_index, pm_smbchg_specific_data_type *chg_param_ptr);



/**
 * @name 
 *     pm_sbl_chg_check_weak_battery_status
 *
 * @description
 *    This function checks the battery charge status 
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_appsbl_chg_check_weak_battery_status(uint32 device_index);

void *pm_target_information_get_specific_info();
void *pm_target_chg_range_data();
bool pm_appsbl_display_init_done();
bool pm_appsbl_charging_in_progress();
pm_err_flag_type pm_appsbl_set_dcin_suspend();
bool pm_app_display_shutdown_in_prgs();
pm_err_flag_type pm_smbchg_get_charger_path(uint32 device_index, pm_smbchg_usb_chgpth_pwr_pth_type* charger_path);
#endif  //PM_APP_SMBCHG__H

