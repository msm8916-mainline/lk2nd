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

#ifndef PM_SMBCHG_USB_CHGPTH_H
#define PM_SMBCHG_USB_CHGPTH_H

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/


typedef enum input_current_limit_mode
{
  PM_SMBCHG_USB_CHGPTH_ICL_MODE_HIGH_CURRENT,
  PM_SMBCHG_USB_CHGPTH_ICL_MODE_100mA,
  PM_SMBCHG_USB_CHGPTH_ICL_MODE_500mA,
  PM_SMBCHG_USB_CHGPTH_ICL_MODE_INVALID
}input_current_limit_mode;

typedef struct input_current_limit_sts
{
  boolean                  is_aicl_complete;
  uint32                   input_current_limit;
  input_current_limit_mode icl_mode;
  boolean                  is_usbin_suspended;
  boolean                  is_dcin_suspended;
  boolean                  is_usbin_active_pwr_src;
  boolean                  is_dcin_active_pwr_src;
}input_current_limit_sts;

typedef enum pm_smbchg_usb_chgpth_pwr_pth_type
{
   PM_SMBCHG_USB_CHGPTH_PWR_PATH__NONE,
   PM_SMBCHG_USB_CHGPTH_PWR_PATH__BATTERY,
   PM_SMBCHG_USB_CHGPTH_PWR_PATH__USB_CHARGER,
   PM_SMBCHG_USB_CHGPTH_PWR_PATH__DC_CHARGER,
   PM_SMBCHG_USB_CHGPTH_PWR_PATH__INVALID
}pm_smbchg_usb_chgpth_pwr_pth_type;


typedef enum
{
   PM_SMBCHG_USB_CHGPTH_DG_STS__RID_ABC_DG,  /**< RID-A/B/C Data Connect Detect Deglitch status >*/
   PM_SMBCHG_USB_CHGPTH_DG_STS__OCD_GOOD_DG, /**< Other Charger Device Data Connect Deglitch status >*/
   PM_SMBCHG_USB_CHGPTH_DG_STS__DCD_GOOD_DG, /**< Data Connect Device Deglitch status >*/
   PM_SMBCHG_USB_CHGPTH_DG_STS__DCD_TIMEOUT, /**< DCD Timeout status >*/
   PM_SMBCHG_USB_CHGPTH_DG_STS__INVALID      /**<  INVALID>*/
}pm_apsd_dg_sts_type;


typedef enum
{
   PM_SMBCHG_USB_CHGPTH_RID__GND,
   PM_SMBCHG_USB_CHGPTH_RID__FLOAT,
   PM_SMBCHG_USB_CHGPTH_RID__A,
   PM_SMBCHG_USB_CHGPTH_RID__B,
   PM_SMBCHG_USB_CHGPTH_RID__C,
   PM_SMBCHG_USB_CHGPTH_RID__INVALID
}pm_smbchg_usb_chgpth_rid_sts_type;


typedef enum
{
   PM_SMBCHG_USB_CHGPTH__HVDCP_SEL_5V,     /**< 5V High Voltage DCP Enabled >*/
   PM_SMBCHG_USB_CHGPTH__HVDCP_SEL_9V,     /**< 9V High Voltage DCP Enabled >*/
   PM_SMBCHG_USB_CHGPTH__HVDCP_SEL_12V,    /**< 12V High Voltage DCP Enabled>*/
   PM_SMBCHG_USB_CHGPTH__HVDCP_SEL_20V,    /**< 20V High Voltage DCP Enabled>*/
   PM_SMBCHG_USB_CHGPTH__IDEV_HVDCP_SEL_A, /**< High Voltage DCP detected   >*/
   PM_SMBCHG_USB_CHGPTH__HVDCP_STS_INVALID /**< INVALID >*/
}pm_smbchg_usb_chgpth_hvdcp_sts_type;

typedef enum
{
  PM_SMBCHG_CHAR_TYPE_USB,
  PM_SMBCHG_CHAR_TYPE_DC,
  PM_SMBCHG_CHAR_TYPE_INVALID
}pm_smbchg_usb_chgpth_chgr_type;

typedef enum
{
  PM_SMBCHG_NO_CHGR_DETECTED,
  PM_SMBCHG_5V_9V_CHGR_DETECTED,
  PM_SMBCHG_UNREGULATED_CHGR_DETECTED,
  PM_SMBCHG_9V_CHGR_DETECTED,
  PM_SMBCHG_INVALID_DETECTED
}pm_smbchg_usb_chgpth_input_sts_type;


typedef enum
{
   PM_SMBCHG_USBCHGPTH_CMD_IL__USBIN_MODE_CHG,    /**< USBIN Mode Charge             >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__USB51_MODE,        /**< USB5/1 Mode                   >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__ICL_OVERRIDE,      /**< ICL Override                  >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__DCIN_SUSPEND,      /**< DCIN Suspend                  >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__USBIN_SUSPEND,     /**< USBIN Suspend                 >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__BAT_2_SYS_FET_DIS, /**< Battery-to-System FET Disable >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__SHDN_N_CLEAR_CMD,  /**< shdn_n_clear command           >*/
   PM_SMBCHG_USBCHGPTH_CMD_IL__INVALID            /**< INVALID                       >*/
}pm_smbchg_usb_chgpth_cmd_il_type;


typedef enum
{
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__DEB_LV_ADAPTER,     /**< USBIN Input Collapse Option for LV adapter>*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__DEB_HV_ADAPTER,     /**< USBIN Input Collapse Option for HV adapter>*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__AICL_EN,            /**< TRUE:enabled, FALSE: disabled>*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__DEB_5V_ADAPTER_SEL, /**< USBIN Input Collapse Glitch Filter for 5V Select, Unregulated or 5V - 9V Adapter >*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__DEB_9V_ADAPTER_SEL, /**< USBIN Input Collapse Glitch Filter for 9V Adapter Select. >*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__DEB_9V_ADAPTER_EN,  /**< USBIN Input Collapse Glitch Filter for 9V Adapter Enable  >*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__OV_OPTION,          /**< FALSE = OV on USBIN will block DCIN current path; TRUE = OV on USBIN will not block DCIN current path>*/
   PM_SMBCHG_USB_CHGPTH_AICL_CFG__INVALID             /**< INVALID >*/
}pm_smbchg_usb_chgpth_aicl_cfg_type;

typedef enum pm_smbchg_usbin_adptr_allowance
{
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_5V,
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_5V_OR_9V,
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_5V_TO_9V,
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_9V,
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_5V_UNREG,
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_5V_9V_UNREG,
 PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_INVALID
}pm_smbchg_usbin_adptr_allowance;

/*! \enum pm_smbchg_usb_chgpth_irq_bit_field_type
   \brief different types of irq bit fields of by smbb irq module
 */
typedef enum
{
  PM_SMBCHG_USB_CHGPTH_USBIN_UV,
  PM_SMBCHG_USB_CHGPTH_USBIN_OV,
  PM_SMBCHG_USB_CHGPTH_USBIN_SRC_DET,
  PM_SMBCHG_USB_CHGPTH_OTG_FAIL,
  PM_SMBCHG_USB_CHGPTH_OTG_OVERCURRENT,
  PM_SMBCHG_USB_CHGPTH_AICL_DONE,
  PM_SMBCHG_USB_CHGPTH_USBID_CHANGE_INTR,
  PM_SMBCHG_USB_CHGPTH_IRQ_INVALID
}pm_smbchg_usb_chgpth_irq_type; 


typedef enum 
{
   PM_SMBCHG_USB_CHGPTH_USB51AC_CTRL__CMD,    /**< USB51AC Command Register Controlled. >*/
   PM_SMBCHG_USB_CHGPTH_USB51AC_CTRL__PIN,    /**< USB51AC Pin Controlled. >*/
   PM_SMBCHG_USB_CHGPTH_USB51AC_CTRL__INVALID /**< INVALID >*/
}pm_smbchg_usb_chgpth_usb51ac_ctrl_type;

typedef enum 
{
   PM_SMBCHG_USB_CHGPTH_USB51_CMD_POL__CMD1_500,    /**< USB51AC Tri-state input. >*/
   PM_SMBCHG_USB_CHGPTH_USB51_CMD_POL__CMD1_100,   /**< USB51AC Dual-state input. >*/
   PM_SMBCHG_USB_CHGPTH_USB51_CMD_POL__INVALID       /**< INVALID >*/
}pm_smbchg_usb_chgpth_usb51_cmd_pol_type;  

typedef struct 
{
   pm_smbchg_usb_chgpth_usb51ac_ctrl_type   usb51ac_ctrl_type;  /**< See pm_smbchg_usb_chgpth_usb51ac_ctrl_type for details */
   pm_smbchg_usb_chgpth_usb51_cmd_pol_type  usb51_cmd_pol_type;  /**< See pm_smbchg_usb_chgpth_usb51ac_ctrl_type for details */
}pm_usb_chgpth_usb_cfg_type;


typedef struct pm_smbchg_usb_chgpth_apsd_cfg_type
{
   boolean en_auto_src_detect; /*autopower source detect enable/disable*/
   boolean en_dcd_tmout_only; /*0 = DCD Option - Deglitchers and Timeout 1 = DCD Option - Timeout only*/
   boolean set_rid_clk_2khz; /*0 = 1kHz; 1 = 2kHz*/
   boolean force_icl_500ma_vbat_low_sdp; /*0X = VBATT_LOW during SDP has no effect; 1X = VBATT_LOW during SDP forces 500ma ICL*/
   boolean sdp_suspend; /*0 = Normal SDP operation 1 = SDP enters suspend*/
   boolean is_ocd_isel_hc; /*0 = ICL 500mA, 1 = ICL HC*/
   boolean usb_fail_pok_hv; /*0 = Normal 1 = Use Power-OK thresholds for HV adapter*/
   boolean is_input_prority_usbin; /*0 = DCIN has priority, 1 = USBIN has priority*/
}pm_smbchg_usb_chgpth_apsd_cfg_type;


typedef enum
{
   PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_DCIN,
   PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_USBIN,
   PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_INVALID
}pm_smbchg_chgpth_input_priority_type;

/*===========================================================================

                 SMBCHG DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/**
* @brief This API returns input current limit status for usb chgpth
* 
* @details
*  This API returns input current limit status for usb chgpth
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] icl_sts             Refer input_current_limit_sts for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_icl_sts(uint32 device_index, input_current_limit_sts* icl_sts);

/**
* @brief This function returns power path for device
* 
* @details
*  This function returns power path for device
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] pwr_path           Refer enum pm_pwr_pth_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_pwr_pth(uint32 device_index, pm_smbchg_usb_chgpth_pwr_pth_type *pwr_path);


/**
* @brief This function returns auto power source detection degitched status
* 
* @details
*  his function returns auto power source detection degitched status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] apsd_dg_sts         Refer enum pm_apsd_dg_sts_type for more info
* @param[in] status              Returns set or clear           
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_apsd_dg_sts(uint32 device_index, pm_apsd_dg_sts_type apsd_dg_sts_type, boolean *status);

/**
* @brief This function returns RID state machine detected status
* 
* @details
*  This function returns RID state machine detected status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] rid_sts             For more info please refer pm_rid_sts_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_rid_sts(uint32 device_index, pm_smbchg_usb_chgpth_rid_sts_type *rid_sts);


/**
* @brief This function returns high voltage DCP detection and enable status
* 
* @details
*  This function returns high voltage DCP detection and enable status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] hvdcp_sts           Refer pm_hvdcp_sts for more info
* @param[out]status              TRUE set, FALSE clear
*
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_hvdcp_sts(uint32 device_index, pm_smbchg_usb_chgpth_hvdcp_sts_type hvdcp_sts, boolean *status);


/**
* @brief This function returns USBIN and DCIN input status
* 
* @details
*  This function returns USBIN and DCIN input status
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chgr                Refer pm_smbchg_usb_chgpth_chgr_type for more info
* @param[in] input_sts_type      Refer pm_smbchg_usb_chgpth_input_sts_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_input_sts(uint32 device_index, pm_smbchg_usb_chgpth_chgr_type chgr, pm_smbchg_usb_chgpth_input_sts_type *input_sts_type);


/**
* @brief This function returns the valid usbid
* 
* @details
*  This function returns the valid usbid
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] valid_id           Valid USBID read
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_valid_usbid(uint32 device_index, uint16 *valid_id);

/**
* @brief This function sets the command for current limit
* 
* @details
*  This function sets the command for current limit
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chgpth_cmd_il       Please refer pm_chgpth_cmd_il for more info
* @param[in] enable              TRUE to set and FALSE to clear
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_set_cmd_il(uint32 device_index, pm_smbchg_usb_chgpth_cmd_il_type chgpth_cmd_il, boolean enable);

/**
* @brief This function reads the command for current limit
* 
* @details
*  This function reads the command for current limit
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chgpth_cmd_il       Please refer pm_chgpth_cmd_il for more info
* @param[out] enable             TRUE to set and FALSE to clear
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_cmd_il(uint32 device_index, pm_smbchg_usb_chgpth_cmd_il_type chgpth_cmd_il, boolean *enable);

/**
* @brief This function sets the maximum USB current
* 
* @details
*  This function sets the maximum USB current
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] i_milli_amp         Current from 100 mA to 2500 mA
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_set_iusb_max(uint32 device_index, uint32 i_milli_amp);

/**
* @brief This function reads the set maximum USB current
* 
* @details
*  This function reads the set maximum USB current
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] i_milli_amp         Current from 100 mA to 2500 mA
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_iusb_max(uint32 device_index, uint32 *i_milli_amp);

/**
* @brief This function sets the STOP bit of enum timer 
* 
* @details
*  This function sets the STOP bit of enum timer stop. To stop the ENUM_TIMER, 
*  SW has to set this STOP bit and increase IUSB_MAX above 100mA
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
pm_err_flag_type pm_smbchg_usb_chgpth_set_enum_timer_stop(uint32 device_index);

/**
* @brief This function sets the USBIN adapter allowance
* 
* @details
*  This function sets the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] adptr_allowance     Refer  pm_smbchg_usbin_adptr_allowance for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_set_usbin_adptr_allowance(uint32 device_index, pm_smbchg_usbin_adptr_allowance adptr_allowance);

/**
* @brief This function reads the USBIN adapter allowance
* 
* @details
*  This function reads the USBIN adapter allowance
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] adptr_allowance     Refer  pm_smbchg_usbin_adptr_allowance for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_usbin_adptr_allowance(uint32 device_index, pm_smbchg_usbin_adptr_allowance *adptr_allowance);

/**
* @brief This function sets the USBIN input current limit
* 
* @details
*  This function sets the USBIN input current limit
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] current_ma          Valid value is 300 to 3000mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_set_usbin_current_limit(uint32 device_index, uint32 current_ma);

/**
* @brief This function reads the USBIN input current limit
* 
* @details
*  This function reads the USBIN input current limit
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out]current_ma          Valid value is 300 to 3000mAmp
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_usbin_current_limit(uint32 device_index, uint32 *current_ma);


/**
* @brief This function configures the AICL for USB CHGPTH
* 
* @details
*  This function configures the AICL for USB CHGPTH
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] usb_aicl_cfg        Refer pm_smbchg_usb_aicl_cfg for more info
* @param[in] enable              TRUE: enabled, FALSE: disabled
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_config_aicl(uint32 device_index, pm_smbchg_usb_chgpth_aicl_cfg_type usb_aicl_cfg, boolean enable);


/**
* @brief This function reads the configured the AICL for USB CHGPTH
* 
* @details
*  This function reads the configured the AICL for USB CHGPTH
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] usb_aicl_cfg        Refer pm_smbchg_usb_aicl_cfg for more info
* @param[in] enable              TRUE: enabled, FALSE: disabled                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_aicl_config(uint32 device_index, pm_smbchg_usb_chgpth_aicl_cfg_type usb_aicl_cfg, boolean *enable);


/**
* @brief This function configures the USB charge path
* 
* @details
*   This function configures the USB charge path
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] usb_cfg_type        Refer usb_cfg_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_config_usb_chgpth(uint32 device_index, pm_usb_chgpth_usb_cfg_type *usb_cfg_type);


/**
* @brief This function gets configuration for the USB charge path
* 
* @details
*   This function gets configuration for the USB charge path
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[out] usb_cfg_type        Refer usb_cfg_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_usb_chgpth_config(uint32 device_index, pm_usb_chgpth_usb_cfg_type *usb_cfg_type);


/**
* @brief This function configures USB charge path for auto power source detection
* 
* @details
*  This function configures USB charge path for auto power source detection
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] apsd_cfg            Refer pm_smbchg_usb_chgpth_apsd_cfg_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_config_apsd(uint32 device_index, pm_smbchg_usb_chgpth_apsd_cfg_type *apsd_cfg);


/**
* @brief This function reads configuration for  USB charge path auto power source detection
* 
* @details
*  This function reads configuration for  USB charge path auto power source detection
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] apsd_cfg            Refer pm_smbchg_usb_chgpth_apsd_cfg_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_get_apsd_config(uint32 device_index, pm_smbchg_usb_chgpth_apsd_cfg_type *apsd_cfg);


/**
* @brief This function sets chgpath input priority
* 
* @details
*  This function sets chgpath input priority
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] chgpth_priority:    Refer pm_smbchg_chgpth_input_priority_type for more info
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_chgpth_set_input_priority(uint32 device_index, pm_smbchg_chgpth_input_priority_type chgpth_priority);

/**
* @brief This function enables irq
* 
* @details
*  This function enables irq
* 
* @param[in] pmic_device_index.  Primary: 0 Secondary: 1
* @param[in] irq                 pm_smbchg_usb_chgpth_irq_type
*                                
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_smbchg_usb_chgpth_irq_enable(uint32 device_index, pm_smbchg_usb_chgpth_irq_type irq, boolean enable);


/**
 * @brief This function clears the SMBCHG irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBCHG irq type. Refer enum pm_smbchg_usb_chgpth_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_smbchg_usb_chgpth_irq_clear(uint32  device_index, pm_smbchg_usb_chgpth_irq_type irq);



/**
 * @brief This function configures the SMBCHG irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBCHG irq type. Refer enum pm_smbchg_usb_chgpth_irq_type.  
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
pm_err_flag_type pm_smbchg_usb_chgpth_irq_set_trigger(uint32 device_index, pm_smbchg_usb_chgpth_irq_type irq, pm_irq_trigger_type trigger);


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
pm_err_flag_type pm_smbchg_usb_chgpth_irq_status(uint32 device_index, pm_smbchg_usb_chgpth_irq_type irq, pm_irq_status_type type, boolean *status);


#endif /* PM_SMBCHG_USB_CHGPTH_H */

