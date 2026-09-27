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

#ifndef PM_SMBCHG_DC_CHGPTH_H
#define PM_SMBCHG_DC_CHGPTH_H

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef enum pm_smbchg_dcin_adptr_allowance
{
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_5V,
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_5V_OR_9V,
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_5V_TO_9V,
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_9V,
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_5V_UNREG,
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_5V_9V_UNREG,
 PM_SMBCHG_DCIN_ADPTR_ALLOWANCE_INVALID
}pm_smbchg_dcin_adptr_allowance;


typedef enum pm_smbchg_dc_aicl_cfg_type
{
   /*AICL_CFG1*/
   PM_SMBCHG_AICL__DEB_LV_ADAPTER = 0,      /*DCIN Input Collapse Option for LV adapter*/
   PM_SMBCHG_AICL__DEB_HV_ADAPTER = 1,      /*DCIN Input Collapse Option for HV adapter*/
   PM_SMBCHG_AICL__AICL_EN = 2,             /*AICL enable/disable for DC CHGPTH*/
   PM_SMBCHG_AICL__DEB_5V_ADAPTER_SEL = 3,  /*DCIN Input Collapse Glitch Filter for 5V Select, Unregulated or 5V - 9V Adapter*/
   PM_SMBCHG_AICL__DEB_9V_ADAPTER_SEL = 4,  /*DCIN Input Collapse Glitch Filter for 9V Adapter Select*/
   PM_SMBCHG_AICL__DEB_9V_ADAPTER_EN = 5,   /*DCIN Input Collapse Glitch Filter for 9V Adapter Enable*/
   PM_SMBCHG_AICL__OV_OPTION = 6,           /*FALSE = OV on USBIN will block DCIN current path; TRUE = OV on USBIN will not block DCIN current path*/
   /*AICL_CFG2*/
   PM_SMBCHG_AICL__AICL_THRESHOLD_5V = 8,       /*DCIN 5V AICL Threshold*/
   PM_SMBCHG_AICL__AICL_THRESHOLD_5V_TO_9V = 9, /*AICL 5v to 9v threshold. FLASE : DC_AICL_6P25_OR_4P25, TRUE : DC_AICL_6P75_OR_4P40*/
   PM_SMBCHG_AICL__DEB_UV = 10,                 /* FALSE = 20ms DCIN falling, 20ms DCIN rising, TRUE= 10us DCIN falling, 10ms DCIN rising */
   PM_SMBCHG_AICL__INVALID_CFG                  /*INVALID*/
}pm_smbchg_dc_aicl_cfg_type; 

/*AICL restart timer*/
typedef enum pm_smbchg_dc_chgpth_aicl_rstrt_tmr
{
 PM_SMBCHG_AICL_WL_RSTRT_TMR_45s,
 PM_SMBCHG_AICL_WL_RSTRT_TMR_1p5m,
 PM_SMBCHG_AICL_WL_RSTRT_TMR_3m,
 PM_SMBCHG_AICL_WL_RSTRT_TMR_6m,
 PM_SMBCHG_AICL_WL_RSTRT_TMR_INVALID
}pm_smbchg_dc_chgpth_aicl_rstrt_tmr;

/*! \enum pm_smbchg_usb_chgpth_irq_bit_field_type
   \brief different types of irq bit fields of by smbb irq module
 */
typedef enum {
  PM_SMBCHG_DC_CHGPTH_DCBIN_UV,
  PM_SMBCHG_DC_CHGPTH_DCBIN_OV,
  PM_SMBCHG_DC_CHGPTH_IRQ_INVALID
}pm_smbchg_dc_chgpth_irq_type; 




/*===========================================================================

                 SMBCHG DRIVER FUNCTION PROTOTYPES

===========================================================================*/


 /**
 * @brief This function configures ADCIN adapter allowance
 * 
 * @details
 *  This API configures ADCIN adapter allowance
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] adptr_allowance     Refer pm_smbchg_dcin_adptr_allowance for more information 
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_set_dcin_adptr_allowance(uint32 device_index, pm_smbchg_dcin_adptr_allowance adptr_allowance);


 /**
 * @brief This function returns configured ADCIN adapter allowance
 * 
 * @details
 *  This API returns configured ADCIN adapter allowance
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] adptr_allowance     Refer pm_smbchg_dcin_adptr_allowance for more information 
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_get_dcin_adptr_allowance(uint32 device_index, pm_smbchg_dcin_adptr_allowance *adptr_allowance);


 /**
 * @brief This function sets the DCIN numput current limit 
 * 
 * @details
 *   This function sets the DCIN numput current limit
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] current_ma          valid range is 300 to 2000 milliAmp
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_set_dcin_current_limit(uint32 device_index, uint32 current_ma);


 /**
 * @brief This function reads the DCIN numput current limit 
 * 
 * @details
 *   This function reads the DCIN numput current limit
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] current_ma         valid range is 300 to 2000 milliAmp
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_get_dcin_current_limit(uint32 device_index, uint32 *current_milli_amp);


 /**
 * @brief This function configures various AICL parameters. Refer struct pm_smbchg_dc_aicl_cfg for more info.
 * 
 * @details
 *  This function configures various AICL parameters. Refer struct pm_smbchg_dc_aicl_cfg for more info.
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] dc_aicl_cfg         Refer struct pm_smbchg_dc_aicl_cfg for more info.
 * @param[in] enable              TRUE sets, FALSE clears                               
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_config_aicl(uint32 device_index, pm_smbchg_dc_aicl_cfg_type dc_aicl_cfg, boolean enable);


 /**
 * @brief This function returns configured AICL parameters. Refer struct pm_smbchg_dc_aicl_cfg for more info.
 * 
 * @details
 *  This function returns configured AICL parameters. Refer struct pm_smbchg_dc_aicl_cfg for more info.
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] dc_aicl_cfg         Refer struct pm_smbchg_dc_aicl_cfg for more info.
 * @param[out]enable              TRUE sets, FALSE clears                               
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_get_aicl_config(uint32 device_index, pm_smbchg_dc_aicl_cfg_type dc_aicl_cfg, boolean *enable);


 /**
 * @brief This function sets the AICL restart timer
 * 
 * @details
 *  This function sets the AICL restart timer
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] timer_type          Refer enum pm_smbchg_dc_chgpth_aicl_rstrt_tmr
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_set_aicl_restart_tmr(uint32 device_index, pm_smbchg_dc_chgpth_aicl_rstrt_tmr timer_type);


 /**
 * @brief This function returns the AICL restart timer value
 * 
 * @details
 *  brief This function returns the AICL restart timer value
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] timer_type          Refer enum pm_smbchg_dc_chgpth_aicl_rstrt_tmr
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_get_aicl_restart_tmr(uint32 device_index, pm_smbchg_dc_chgpth_aicl_rstrt_tmr *timer_type);


/**
 * @brief This function enables irq
 * 
 * @details
 *  This function enables irq
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 pm_smbchg_dc_chgpth_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_irq_enable(uint32 device_index, pm_smbchg_dc_chgpth_irq_type irq, boolean enable);


/**
 * @brief This function clears the SMBCHG irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBCHG irq type. Refer enum pm_smbchg_dc_chgpth_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_irq_clear(uint32  device_index, pm_smbchg_dc_chgpth_irq_type irq);


/**
 * @brief This function configures the SMBCHG irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBCHG irq type. Refer enum pm_smbchg_dc_chgpth_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_uefi_irq.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_irq_set_trigger(uint32 device_index, pm_smbchg_dc_chgpth_irq_type irq, pm_irq_trigger_type trigger);


/**
 * @brief This function configures the SMBCHG for irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBCHG irq type. Refer enum pm_Smbb_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_uefi_irq.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_dc_chgpth_irq_status(uint32 device_index, pm_smbchg_dc_chgpth_irq_type irq, pm_irq_status_type type, boolean *status);


#endif /* PM_SMBCHG_DC_CHGPTH_H */

