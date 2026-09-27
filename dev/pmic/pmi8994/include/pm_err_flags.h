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

#ifndef _PM_ERR_FLAGS_H
#define _PM_ERR_FLAGS_H

/**
 * These are all the error codes returned by PMIC APIs
 *
 * PMIC APIs most commonly return errors if the caller has tried
 * to use parameters that are out of range or if the feature is
 * not supported by the PMIC model detected at run-time.
 */
typedef enum
{
    /**
     * The API completed successfully
     */
    PM_ERR_FLAG__SUCCESS,
    /**
     * The SBI operation Failed
     * extra lines are to support += error codes
     */
    PM_ERR_FLAG__SBI_OPT_ERR,  
    PM_ERR_FLAG__SBI_OPT2_ERR,
    PM_ERR_FLAG__SBI_OPT3_ERR,
    PM_ERR_FLAG__SBI_OPT4_ERR,

    PM_ERR_FLAG__I2C_OPT_ERR,  
    PM_ERR_FLAG__I2C_OPT2_ERR,  
    PM_ERR_FLAG__I2C_OPT3_ERR,  
    PM_ERR_FLAG__I2C_OPT4_ERR,  

    PM_ERR_FLAG__SPMI_OPT_ERR,
    PM_ERR_FLAG__SPMI_OPT2_ERR,
    PM_ERR_FLAG__SPMI_OPT3_ERR,
    PM_ERR_FLAG__SPMI_OPT4_ERR,

    PM_ERR_FLAG__SPMI_TRANSCOMM_ERR,


    /**
     *  Input Parameter one is out of range
     */
    PM_ERR_FLAG__PAR1_OUT_OF_RANGE,
    /**
     *  Input Parameter two is out of range
     */
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE,
    /**
     *  Input Parameter three is out of range
     */
    PM_ERR_FLAG__PAR3_OUT_OF_RANGE,
    /**
     *  Input Parameter four is out of range
     */
    PM_ERR_FLAG__PAR4_OUT_OF_RANGE,
    /**
     *  Input Parameter five is out of range
     */
    PM_ERR_FLAG__PAR5_OUT_OF_RANGE,
    /**
     *  Input Parameter six is out of range
     */
    PM_ERR_FLAG__PAR6_OUT_OF_RANGE,
    PM_ERR_FLAG__INVALID_PARAMETER,
    PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE,
    PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE,
    /**
     *  This feature is not supported by this PMIC
     */
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED,
    PM_ERR_FLAG__INVALID_PMIC_MODEL,
    PM_ERR_FLAG__SECURITY_ERR,
    PM_ERR_FLAG__IRQ_LIST_ERR,
    PM_ERR_FLAG__DEV_MISMATCH,
    PM_ERR_FLAG__ADC_INVALID_RES,
    PM_ERR_FLAG__ADC_NOT_READY,
    PM_ERR_FLAG__ADC_SIG_NOT_SUPPORTED,
    PM_ERR_FLAG__ADC_VBATT_READ_ERR,
    /**
     *  The RTC displayed mode read from the PMIC is invalid
     */
    PM_ERR_FLAG__RTC_BAD_DIS_MODE,
    /**
     *  Failed to read the time from the PMIC RTC
     */
    PM_ERR_FLAG__RTC_READ_FAILED,
    /**
     *  Failed to write the time to the PMIC RTC
     */
    PM_ERR_FLAG__RTC_WRITE_FAILED,
    /**
     *  RTC not running
     */
    PM_ERR_FLAG__RTC_HALTED,
    /**
     *  The DBUS is already in use by another MPP.
     */
    PM_ERR_FLAG__DBUS_IS_BUSY_MODE,
    /**
     *  The ABUS is already in use by another MPP.
     */
    PM_ERR_FLAG__ABUS_IS_BUSY_MODE,
    /**
     *  The error occurs illegal value that isn't in the
     *  macro_type enum is used
     */
    PM_ERR_FLAG__MACRO_NOT_RECOGNIZED,
    /**
     *  This error occurs if the data read from a register does
     *  not match the setting data
     */
    PM_ERR_FLAG__DATA_VERIFY_FAILURE,
    /**
     *  The error occurs illegal value that isn't in the
     *  pm_register_type enum is used
     */
    PM_ERR_FLAG__SETTING_TYPE_NOT_RECOGNIZED,
    /**
     * The error occurs illegal value that isn't in the
     * pm_mode_group enum is used
     */
    PM_ERR_FLAG__MODE_NOT_DEFINED_FOR_MODE_GROUP,
    /**
     *  The error occurs illegal value that isn't in the pm_mode
     *  enum is used
     */
    PM_ERR_FLAG__MODE_GROUP_NOT_DEFINED,
    /**
     *  This error occurs if the PRESTUB function returns a false
     */
    PM_ERR_FLAG__PRESTUB_FAILURE,
    /**
     *  This error occurs if the POSTSTUB function returns a
     *  false
     */
    PM_ERR_FLAG__POSTSTUB_FAILURE,
    /**
     *  When modes are set for a modegroup, they are recorded and
     *  checked for success
     */
    PM_ERR_FLAG__MODE_NOT_RECORDED_CORRECTLY,
    /**
     *  Unable to find the mode group in the mode group recording
     *  structure. Fatal memory problem
     */
    PM_ERR_FLAG__MODE_GROUP_STATE_NOT_FOUND,
    /**
     *  This error occurs if the SUPERSTUB function return a
     *  false
     */
    PM_ERR_FLAG__SUPERSTUB_FAILURE,
   /* Processor does not have access to this resource */
   PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE,
   /* Resource is invalid. Resource index was pass 
    * into router that is not defined in the build 
    */
   PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED,
    
    /*! Non-Zero means unsuccessful call. Here it means registration failed  
         because driver has ran out of memory.MAX_CLIENTS_ALLOWED needs to be
         increased and the code needs to be recompiled */                            
    PM_ERR_FLAG__VBATT_CLIENT_TABLE_FULL,
    /*! One of the parameters to the function call was invalid. */                            
    PM_ERR_FLAG__VBATT_REG_PARAMS_WRONG,
    /*! Client could not be deregistered because perhaps it does not exist */                            
    PM_ERR_FLAG__VBATT_DEREGISTRATION_FAILED,
    /*! Client could not be modified because perhaps it does not exist */                            
    PM_ERR_FLAG__VBATT_MODIFICATION_FAILED,
    /*!< Client could not be queried because perhaps it does not exist */                            
    PM_ERR_FLAG__VBATT_INTERROGATION_FAILED,
    /*!< Client's filetr could not be set because perhaps it does not exist */                            
    PM_ERR_FLAG__VBATT_SET_FILTER_FAILED,
    /*! Keeps the count of all errors. Any error code equal to or greater
         than this one means an unknown error. VBATT_LAST_ERROR should be 
         the last error code always with the highest value */                            
    PM_ERR_FLAG__VBATT_LAST_ERROR,  
    PM_ERR_FLAG__PMIC_NOT_SUPPORTED,
    
    /* Non Vibrator Module is being indexed */
    PM_ERR_FLAG__INVALID_VIBRATOR_INDEXED ,

    /* Non PWM generator is being indexed */
    PM_ERR_FLAG__INVALID_PWM_GENERATOR_INDEXED ,

    /* Non Chg Module is being indexed */
    PM_ERR_FLAG__INVALID_CHG_INDEXED ,

    PM_ERR_FLAG__INVALID_CHG_PATH,

    PM_ERR_FLAG__INVALID_CLK_INDEXED,

    PM_ERR_FLAG__INVALID_XO_INDEXED,

    PM_ERR_FLAG__INVALID_XOADC_INDEXED,

    PM_ERR_FLAG__INVALID_TCXO_INDEXED,

    PM_ERR_FLAG__INVALID_RTC_INDEXED,

    PM_ERR_FLAG__INVALID_SMBC_INDEXED,

    PM_ERR_FLAG__INVALID_BMS_INDEXED,

    PM_ERR_FLAG__API_NOT_IMPLEMENTED,

    PM_ERR_FLAG__INVALID_PAONCNTRL_INDEXED,

    /* Non Coincell Module is being indexed */
    PM_ERR_FLAG__INVALID_COINCELL_INDEXED ,

    /* Non Flash Module is being indexed */
    PM_ERR_FLAG__INVALID_FLASH_INDEXED ,

    /* Non OVP Module is being indexed */
    PM_ERR_FLAG__INVALID_OVP_INDEXED ,

    /* Non KEYPAD Module is being indexed */
    PM_ERR_FLAG__INVALID_KEYPAD_INDEXED ,

    /* Non LVS Module is being indexed */
    PM_ERR_FLAG__INVALID_LVS_INDEXED ,

    /* Non HSED Module is being indexed */
    PM_ERR_FLAG__INVALID_HSED_INDEXED ,

    /* Non TALM Module is being indexed */
    PM_ERR_FLAG__INVALID_TALM_INDEXED ,

    /* Non NCP Module is being indexed */
    PM_ERR_FLAG__INVALID_NCP_INDEXED ,

    /* Non NFC Module is being indexed */
    PM_ERR_FLAG__INVALID_NFC_INDEXED ,

    /* Non MVS Module is being indexed */
    PM_ERR_FLAG__INVALID_MVS_INDEXED,

    /* Non HDMI Module is being indexed */
    PM_ERR_FLAG__INVALID_HDMI_INDEXED,

    /* Non VS Module is being indexed */
    PM_ERR_FLAG__INVALID_VS_INDEXED,

    /* Non UVLO Module is being indexed */
    PM_ERR_FLAG__INVALID_UVLO_INDEXED,

    /* Non AMUX Module is being indexed */
    PM_ERR_FLAG__INVALID_AMUX_INDEXED,

    PM_ERR_FLAG__INVALID_KEYPAD_EVENT_COUNTER,

    /* Non MPP Module is being indexed */ 
    PM_ERR_FLAG__INVALID_MPP_INDEXED,

    PM_ERR_FLAG__INVALID_BATTERY_CELL_NUMBER,

    PM_ERR_FLAG__INVALID_PWRON_INDEXED,

    PM_ERR_FLAG__INVALID_VBATT_INDEXED,

    PM_ERR_FLAG__INVALID_HSDKYPD_APP_INDEXED,

    PM_ERR_FLAG__INVALID_PWRKEY_APP_INDEXED,

    PM_ERR_FLAG__INVALID_EVENT_CALLBACK,

    PM_ERR_FLAG__SHADOW_REGISTER_INIT_FAILED ,

    /* PSDTE Error Functionality */
    PM_ERR_FLAG__PSDTE_ENV_FAILURE,

    PM_ERR_FLAG__PSDTE_PMIC_POWERUP_FAILED,

    PM_ERR_FLAG__PSDTE_PMIC_POWERDOWN_FAILED,

    PM_ERR_FLAG__FTS_CALCULATION_ERROR ,

    PM_ERR_FLAG__API_DEPRICATED_ERROR,

    PM_ERR_FLAG__RPC_PROCESSOR_NOT_RECOGNIZED_ERROR,

    /* VREG Errors */
    PM_ERR_FLAG__VREG_VOTE_DEREGISTER_ERROR,

    PM_ERR_FLAG__VREG_VOTE_REGISTER_ERROR,

    /* VS Errors */
    PM_ERR_FLAG__INVALID_VS_STATUS,

    PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED,

    PM_ERR_FLAG__DAL_SERVICE_FAILED,

    PM_ERR__MUTEX_CREATION_FAILED ,

    PM_ERR__MUTEX_RELEASE_FAILED ,

    PM_ERR__MUTEX_DELETION_FAILED ,
    
    PM_ERR__MUTEX_UNAVAILBLE ,
    // Returned if the Create Comm Channel fails to find the correct commType
    PM_ERR_FLAG__COMM_TYPE_NOT_RECOGNIZED,

    PM_ERR_FLAG__IRQ_GPIO_SET_FAILED,
    /*! Event information table has run out of client entries */
    PM_ERR_FLAG__PMAPP_EVENT_CLIENT_TABLE_FULL,
    /*! Client could not be deregistered because perhaps it does not exist */                            
    PM_ERR_FLAG__PMAPP_EVENT_DEREGISTRATION_FAILED,
    /*! Client is attempting to use a client index that is incorrect or has been previously deregisterd */
    PM_ERR_FLAG__PMAPP_EVENT_CLIENT_INDEX_FAILURE,
    
    PM_ERR_FLAG__INVALID_MISC_INDEXED,

    PM_ERR_FLAG__INVALID_MISC_TBD,
    
    PM_ERR_FLAG__INVALID_VREG_INDEXED,

    PM_ERR_FLAG__INVALID_POINTER,
    
    PM_ERR_FLAG__FUNCTION_NOT_SUPPORTED_AT_THIS_LEVEL,

    PM_ERR_FLAG__INVALID_LPG_INDEXED,

    PM_ERR_FLAG__INVALID_DISPBACKLIGHT_APP_INDEXED,
    PM_ERR_FLAG__INVALID_RGB_INDEXED,
    PM_ERR_FLAG__INVALID_SMBB_INDEXED,
    PM_ERR_FLAG__FG_SRAM_DEVICE_ACCESS,

    PM_ERR_FLAG__SBL_CONFIG_REG_OPERATION_NOT_SUPPORTED,

    PM_ERR_FLAG__INVALID

} pm_err_flag_type;

/**
 * This macro is used to return the greater of PM_ERR_FLAG__SUCCESS
 * or whatever error an API may have found.
 */
#define PM_ERR_MAX(a,b) ((a > b) ? (a) : (b))


#endif

