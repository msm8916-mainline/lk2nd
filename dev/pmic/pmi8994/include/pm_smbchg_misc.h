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

#ifndef PM_SMBCHG_MISC_H
#define PM_SMBCHG_MISC_H

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef enum pm_smbchg_misc_src_detect_type
{
  PM_SMBCHG_MISC_SRC_DETECT_CDP,
  PM_SMBCHG_MISC_SRC_DETECT_DCP,
  PM_SMBCHG_MISC_SRC_DETECT_OTHER_CHARGING_PORT,
  PM_SMBCHG_MISC_SRC_DETECT_SDP,
  PM_SMBCHG_MISC_SRC_DETECT_FMB_UART_ON,
  PM_SMBCHG_MISC_SRC_DETECT_FMB_UART_OFF,
  PM_SMBCHG_MISC_SRC_DETECT_FMB_USB_ON,
  PM_SMBCHG_MISC_SRC_DETECT_FMB_USB_OFF,
  PM_SMBCHG_MISC_SRC_DETECT_INVALID
}pm_smbchg_misc_src_detect_type;


typedef enum {
  PM_SMBCHG_MISC_POWER_OK,
  PM_SMBCHG_MISC_TEMP_SHUTDOWN,
  PM_SMBCHG_MISC_WDOG_TIMER,
  PM_SMBCHG_MISC_FLASH_FAIL,
  PM_SMBCHG_MISC_OTS2,
  PM_SMBCHG_MISC_OTS3,
  PM_SMBCHG_MISC_IRQ_INVALID
}pm_smbchg_misc_irq_type; 


typedef enum pm_smbchg_misc_sfty_after_wdog_irq_type
{
  PM_SMBCHG_MISC_SFTY_TIMER_AFTER_WDOG_12MIN,
  PM_SMBCHG_MISC_SFTY_TIMER_AFTER_WDOG_24MIN,
  PM_SMBCHG_MISC_SFTY_TIMER_AFTER_WDOG_48MIN,
  PM_SMBCHG_MISC_SFTY_TIMER_AFTER_WDOG_96MIN,
  PM_SMBCHG_MISC_SFTY_TIMER_AFTER_WDOG_INVALID
}pm_smbchg_misc_sfty_after_wdog_irq_type;

typedef enum pm_smbchg_wdog_timeout_type
{
  PM_SMBCHG_MISC_WD_TMOUT_18S,
  PM_SMBCHG_MISC_WD_TMOUT_36S,
  PM_SMBCHG_MISC_WD_TMOUT_72S,
  PM_SMBCHG_MISC_WD_TMOUT_INVALID
}pm_smbchg_wdog_timeout_type;

typedef struct pm_smbchg_misc_wdog_cfg_type
{
  boolean                                 wdog_timer_en;
  boolean                                 wdog_option_run_always;
  boolean                                 wdog_irq_sfty_en;
  pm_smbchg_misc_sfty_after_wdog_irq_type sfty_timer;
  pm_smbchg_wdog_timeout_type             wdog_timeout;
  boolean                                 afp_wdog_en;
}pm_smbchg_misc_wdog_cfg_type; 


typedef enum pm_smbchg_misc_misc_fn_cfg_type
{
   PM_SMBCHG_MISC_FN__VCHG_PA_ON_SEL,     /**< VCHG Function>*/
   PM_SMBCHG_MISC_FN__GSM_PA_ON_ADJ_SEL,  /**< PA_ON Function>*/
   PM_SMBCHG_MISC_FN__STANDBY_VCHG,       /**< Standby VCHG Function>*/
   PM_SMBCHG_MISC_FN__CHG_OK,             /**< Charge OK Function>*/
   PM_SMBCHG_MISC_FN__PHY_ON,             /**< PHY_ON Function>*/
   PM_SMBCHG_MISC_FN__CFG_NTCVOUT,        /**< Reserved>*/
   PM_SMBCHG_MISC_FN__FETDRV,             /**< Reserved>*/
   PM_SMBCHG_MISC_FN__SYSOK_LDO,          /**< SYSOK LDO Function>*/
   PM_SMBCHG_MISC_FN__INVALID             /**< INVALID>*/
}pm_smbchg_misc_misc_fn_cfg_type; 



/*===========================================================================

                 SMBCHG OTG DRIVER FUNCTION PROTOTYPES

===========================================================================*/


 /**
 * @brief This function detects the type of chgarging port device is connected to
 * 
 * @details
 *  This API detects the type of chgarging port device is connected to
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] src_detected       Refer pm_smbchg_misc_src_detect_type for more info
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_chgr_port_detected(uint32 pmic_device, pm_smbchg_misc_src_detect_type *src_detected);


 /**
 * @brief This function restarts the charger watchdog
 * 
 * @details
 *  This API restarts charger watchdog
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_wdog_rst(uint32 pmic_device);


 /**
 * @brief This function enables automatic fault protection
 * 
 * @details
 *  This API enables automatic fault protection
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_en_afp(uint32 pmic_device);


 /**
 * @brief This function configures the charger watchdog 
 * 
 * @details
 *  This API configures the charger watchdog 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] wd_cfg              Refer struct pm_smbchg_misc_wdog_cfg_type for more info
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_config_wdog(uint32 pmic_device, pm_smbchg_misc_wdog_cfg_type *wd_cfg);


 /**
 * @brief This function reads the charger watchdog configuration
 * 
 * @details
 *  This API reads the charger watchdog configuration 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[out] wd_cfg              Refer struct pm_smbchg_misc_wdog_cfg_type for more info
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_get_wdog_config(uint32 pmic_device, pm_smbchg_misc_wdog_cfg_type *wd_cfg);


 /**
 * @brief This function enable/disable charger watchdog
 * 
 * @details
 *  This API enable/disable charger watchdog
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] enable_smbchg_wdog: TRUE=Enable; FALSE=Disable charger watchdog
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_enable_wdog(uint32 pmic_device, boolean enable_smbchg_wdog);


 /**
 * @brief This function sets charger watchdog timeout
 * 
 * @details
 *  This API sets charger watchdog timeout
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] wdog_timeout :   Refer struct pm_smbchg_wdog_timeout_type for more info
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_set_wdog_timeout(uint32 pmic_device, pm_smbchg_wdog_timeout_type wdog_timeout);


 /**
 * @brief This function clear/sets MISC funtions like VCHG, PA_ON, standby VCHG, CHGR_OK, PHY_ON, SYSOK_LDO
 * 
 * @details
 *  This function clear/sets MISC funtions like VCHG, PA_ON, standby VCHG, CHGR_OK, PHY_ON, SYSOK_LDO
 *  VCHG output proportional to the charger/discharge current. 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] misc_cfg            Refer pm_smbchg_misc_misc_fn_cfg_type for more info
 * @param[in]enable               TRUE sets and FALSE clears                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_config_misc_fn(uint32 pmic_device, pm_smbchg_misc_misc_fn_cfg_type misc_cfg, boolean enable);


 /**
 * @brief This function reads MISC funtions status like VCHG, PA_ON, standby VCHG, CHGR_OK, PHY_ON, SYSOK_LDO
 * 
 * @details
 *  This function reads MISC funtions status like VCHG, PA_ON, standby VCHG, CHGR_OK, PHY_ON, SYSOK_LDO
 *  VCHG output proportional to the charger/discharge current. 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] misc_cfg            Refer pm_smbchg_misc_misc_fn_cfg_type for more info
 * @param[out]enable              TRUE sets and FALSE clears                 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_get_misc_fn_config(uint32 pmic_device, pm_smbchg_misc_misc_fn_cfg_type misc_cfg, boolean* enable);

 /**
 * @brief This function enables the SMBCHG irq 
 * 
 * @details
 *  This function enables the SMBCHG irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_smbchg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_irq_enable(uint32 pmic_device, pm_smbchg_misc_irq_type irq, boolean enable);

 /**
 * @brief This function clears the SMBCHG irq 
 * 
 * @details
 *  This function clears the SMBCHG irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_smbchg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_irq_clear(uint32  pmic_device, pm_smbchg_misc_irq_type irq);

 /**
 * @brief This function configures the SMBCHG irq trigger 
 * 
 * @details
 *  This function configures the SMBCHG irq trigger 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_smbchg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_irq_set_trigger(uint32 pmic_device, pm_smbchg_misc_irq_type irq, pm_irq_trigger_type trigger);

 /**
 * @brief This function configures the SMBCHG for irq 
 * 
 * @details
 *  This function configures the SMBCHG for irq 
 * 
 * @param[in] pmic_device_index.  Primary: 0 Secondary: 1
 * @param[in] irq                 refer pm_smbchg_misc_irq_type
 *                                
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_misc_irq_status(uint32 pmic_device, pm_smbchg_misc_irq_type irq, pm_irq_status_type type, boolean *status);





#endif /* PM_SMBCHG_MISC_H*/

