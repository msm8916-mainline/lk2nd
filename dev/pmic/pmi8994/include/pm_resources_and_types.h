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

#ifndef PM_RESOURCES_AND_TYPES__H
#define PM_RESOURCES_AND_TYPES__H

/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include <stdint.h>
#include <sys/types.h>
#include <smem.h>

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef uint8_t pm_register_data_type;
typedef uint16_t pm_register_address_type;


#define  PM_MAX_NUM_PMICS  SMEM_MAX_PMIC_DEVICES

/*! \enum chargers
 *  \brief This enum contains definitions of all chargers in the target
 */

enum
{
  PM_CHG_1,
  PM_CHG_INVALID
};

/*!
 * PMIC power module voltage level
 */
typedef uint32 pm_volt_level_type;

typedef enum
{
   PM_OFF,
   PM_ON,
   PM_INVALID
}pm_on_off_type;


typedef enum
{
   PM_DISABLE_CONFIG = 0,
   PM_ENABLE_CONFIG  = 1,
   PM_CONFIG_INVALID
} pm_smbchg_param_config_enable_type;


typedef enum 
{
    PM_SW_MODE_LPM     = 0,	// Low power mode	
    PM_SW_MODE_BYPASS  = 1,	// Bypass mode       bit 5
    PM_SW_MODE_AUTO    = 2, // Auto mode         bit 6
    PM_SW_MODE_NPM     = 4,	// Normal power mode bit 7	
    PM_SW_MODE_INVALID
}pm_sw_mode_type;

/*!
 * The number of phases that the autonomous phase controller(APC) can use.
 */
typedef enum 
{
    PM_PHASE_CNT__1,		
    PM_PHASE_CNT__2,	
    PM_PHASE_CNT__3, 
    PM_PHASE_CNT__4,
    PM_PHASE_CNT__INVALID
}pm_phase_cnt_type;

/** 
  @struct peripheral_info_type
  @brief Contains Peripheral Information such as Type, Subtype, 
         Analog and Digital Major Versions. 
 */
typedef struct 
{
    uint16 base_address;
    uint8  peripheral_type;
    uint8  peripheral_subtype;
    uint8  analog_major_version;
    uint8  analog_minor_version;
    uint8  digital_major_version;
    uint8  digital_minor_version;
}peripheral_info_type;

// /*! 
// * Generic enable,
// */
//typedef enum
//{
//    PM_NPA_GENERIC_DISABLE, // default
//    PM_NPA_GENERIC_ENABLE,
//    PM_NPA_GENERIC_INVALID
//}pm_npa_generic_enable_type;
//
///*!
// * Bypass Allowed
// */
//typedef enum
//{
//    PM_NPA_BYPASS_ALLOWED, // default
//    PM_NPA_BYPASS_DISALLOWED,
//    PM_NPA_BYPASS_INVALID
//}pm_npa_bypass_allowed_type;
//
///*!
// * LDO Software Mode
// */
//typedef enum
//{
//    PM_NPA_SW_MODE_LDO__IPEAK, // default
//    PM_NPA_SW_MODE_LDO__NPM,
//    PM_NPA_SW_MODE_LDO__INVALID
//}pm_npa_sw_mode_ldo_type;
///*!
// * SMPS Software Mode
// */
//typedef enum
//{
//    PM_NPA_SW_MODE_SMPS__AUTO, // default
//    PM_NPA_SW_MODE_SMPS__IPEAK,
//    PM_NPA_SW_MODE_SMPS__NPM,
//    PM_NPA_SW_MODE_SMPS__INVALID
//}pm_npa_sw_mode_smps_type;
//
///*!
// * Pin Control Enable
// */
//typedef enum
//{
//    PM_NPA_PIN_CONTROL_ENABLE__NONE = 0, // default
//    PM_NPA_PIN_CONTROL_ENABLE__EN1 = 1,
//    PM_NPA_PIN_CONTROL_ENABLE__EN2 = 2,
//    PM_NPA_PIN_CONTROL_ENABLE__EN3 = 4,
//    PM_NPA_PIN_CONTROL_ENABLE__EN4 = 8,
//    PM_NPA_PIN_CONTROL_ENABLE__INVALID
//}pm_npa_pin_control_enable_type;
//
///*!
// * Pin Control Power Mode
// */
//typedef enum
//{
//    PM_NPA_PIN_CONTROL_POWER_MODE__NONE = 0, // default
//    PM_NPA_PIN_CONTROL_POWER_MODE__EN1 = 1,
//    PM_NPA_PIN_CONTROL_POWER_MODE__EN2 = 2,
//    PM_NPA_PIN_CONTROL_POWER_MODE__EN3 = 4,
//    PM_NPA_PIN_CONTROL_POWER_MODE__EN4 = 8,
//    PM_NPA_PIN_CONTROL_POWER_MODE__SLEEPB = 16,
//    PM_NPA_PIN_CONTROL_POWER_MODE__INVALID
//}pm_npa_pin_control_power_mode_type;
//
//
///*!
// * Pin Control Power Mode
// */
//typedef enum
//{
//    PM_NPA_CORNER_MODE__NONE = 0, // default
//    PM_NPA_CORNER_MODE__1 = 1,
//    PM_NPA_CORNER_MODE__2 = 2,
//    PM_NPA_CORNER_MODE__3 = 3,
//    PM_NPA_CORNER_MODE__4 = 4,
//    PM_NPA_CORNER_MODE__5 = 5,
//    PM_NPA_CORNER_MODE__6 = 6,
//    PM_NPA_CORNER_MODE__INVALID
//}pm_npa_corner_mode_type;
//
///*!
// * Pin Control Enable
// */
//typedef enum
//{
//    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE = 0, // default
//    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__EN1 = 1,
//    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__INVALID
//}pm_npa_clk_buff_pin_control_enable_type;
//
typedef enum
{
    PM_CLK_BUFF_OUT_DRV__1X,
    PM_CLK_BUFF_OUT_DRV__2X,
    PM_CLK_BUFF_OUT_DRV__3X,
    PM_CLK_BUFF_OUT_DRV__4X,
} pm_clk_buff_output_drive_strength_type;

/*! \enum pm_irq_trigger_type
 *  \brief Type definition for different IRQ triggers
 */
typedef enum 
{
  PM_IRQ_TRIGGER_ACTIVE_LOW,
  PM_IRQ_TRIGGER_ACTIVE_HIGH,
  PM_IRQ_TRIGGER_RISING_EDGE,
  PM_IRQ_TRIGGER_FALLING_EDGE,
  PM_IRQ_TRIGGER_DUAL_EDGE,
  PM_IRQ_TRIGGER_INVALID
} pm_irq_trigger_type;

/*! \enum pm_irq_status_type
 *  \brief Type definition for different IRQ STATUS
 */
typedef enum
{
  PM_IRQ_STATUS_RT,
  PM_IRQ_STATUS_LATCHED,
  PM_IRQ_STATUS_PENDING,
  PM_IRQ_STATUS_ENABLE,
  PM_IRQ_STATUS_INVALID
} pm_irq_status_type;

///*! \enum pm_npa_mode_id_core_rail_type
// *  \brief To do CPR voltage settling in the SBL.
// */
//typedef enum
//{
//   PMIC_NPA_MODE_ID_CORE_RAIL_OFF = 0,
//   PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION = 1,
//   PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS = 2,
//   PMIC_NPA_MODE_ID_CORE_RAIL_LOW = 3,
//   PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL = 4,
//   PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS = 5,
//   PMIC_NPA_MODE_ID_CORE_RAIL_TURBO = 6,
//   PMIC_NPA_MODE_ID_CORE_RAIL_MAX = 7,
//}pm_npa_mode_id_core_rail_type;
//
#endif /* PM_RESOURCES_AND_TYPES__H */

