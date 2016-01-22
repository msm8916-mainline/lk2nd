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

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_app_smbchg.h"
#include "pm_smbchg_chgr.h"
#include "pm_smbchg_bat_if.h"
#include "pm_app_smbchg.h"
#include "pm_fg_adc_usr.h"
#include "pm_fg_driver.h"
#include "pm_smbchg_driver.h"
#include "pm_comm.h"
#include "pm_smbchg_dc_chgpth.h"
#include <kernel/thread.h>
#include <debug.h>
#include <platform/timer.h>
#include <sys/types.h>
#include <target.h>
#include <pm8x41.h>
#include <bits.h>
#include <board.h>
#include <smem.h>

/*===========================================================================

                     PROTOTYPES

===========================================================================*/


/*===========================================================================

                     GLOBAL TYPE DEFINITIONS

===========================================================================*/
#define  PM_REG_CONFIG_SETTLE_DELAY       175  * 1000 //175ms  ; Delay required for battery voltage de-glitch time
#define  PM_WEAK_BATTERY_CHARGING_DELAY   500 * 1000  //500ms
#define  PM_WIPOWER_START_CHARGING_DELAY  3500 * 1000 //3.5sec
#define  PM_MIN_ADC_READY_DELAY             1 * 1000  //1ms
#define  PM_MAX_ADC_READY_DELAY     2000              //2s
#define SBL_PACKED_SRAM_CONFIG_SIZE 3
#define  PM_CHARGE_DISPLAY_TIMEOUT       5 * 1000 //5 secs
#define boot_log_message(...) dprintf(CRITICAL, __VA_ARGS__)

static pm_smbchg_bat_if_low_bat_thresh_type pm_dbc_bootup_volt_threshold;
/* Need to maintain flags to track
 * 1. charge_in_progress: Charging progress and exit the loop once charging is completed.
 * 2. display_initialized: Track if the display is already initialized to make sure display
 *    thread does not reinitialize the display again.
 * 3. display_shutdown_in_prgs: To avoid race condition between regualr display initialization and
 *    display shutdown in display thread.
 */

static bool display_initialized;
static bool charge_in_progress;
static bool display_shutdown_in_prgs;
static bool pm_app_read_from_sram;

char panel_name[256];

pm_err_flag_type pm_appsbl_chg_config_vbat_low_threshold(uint32 device_index, pm_smbchg_specific_data_type *chg_param_ptr);
static void display_thread_initialize();
static void pm_app_ima_read_voltage(uint32_t *);
static void pm_app_pmi8994_read_voltage(uint32_t *voltage);
/*===========================================================================

                     FUNCTION IMPLEMENTATION

===========================================================================*/
pm_err_flag_type pm_appsbl_chg_check_weak_battery_status(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_specific_data_type *chg_param_ptr = NULL;
   pm_smbchg_chgr_chgr_status_type vbatt_chging_status;
   boolean hot_bat_hard_lim_rt_sts  = FALSE;
   boolean cold_bat_hard_lim_rt_sts = FALSE;
   boolean vbatt_weak_status = TRUE;
   boolean adc_reading_ready = FALSE;
   boolean bat_present       = TRUE;
   uint32 vbat_adc = 0;
   uint16  wait_index = 0;
   boolean vbatt_status = FALSE;
   pm_smbchg_misc_src_detect_type chgr_src_detected;
   boolean configure_icl_flag = FALSE;
   boolean chg_prog_message_flag = FALSE;
   pm_smbchg_usb_chgpth_pwr_pth_type charger_path = PM_SMBCHG_USB_CHGPTH_PWR_PATH__INVALID;;  uint32  bootup_threshold;

   pm_smbchg_driver_init(device_index);
   pm_fg_driver_init(device_index);

   chg_param_ptr = (pm_smbchg_specific_data_type*)pm_target_information_get_specific_info();
   ASSERT(chg_param_ptr);
   bootup_threshold = chg_param_ptr->bootup_battery_theshold_mv; 

   if(chg_param_ptr->dbc_bootup_volt_threshold.enable_config == PM_ENABLE_CONFIG)
   {
      //Configure Vlowbatt threshold: Used by PMI on next bootup
      err_flag  |= pm_appsbl_chg_config_vbat_low_threshold(device_index, chg_param_ptr); 
   }

   //Check Battery presence
   err_flag |= pm_smbchg_bat_if_get_bat_pres_status(device_index, &bat_present);
   if( bat_present == FALSE )
   {
      dprintf(CRITICAL, "Booting up to HLOS: Charger is Connected and NO battery\n");
      return err_flag;
   }


   //Detect the typpe of charger used
   //err_flag |= pm_smbchg_usb_chgpth_get_pwr_pth(device_index, &
   err_flag |= pm_smbchg_get_charger_path(device_index, &charger_path);
   if (charger_path == PM_SMBCHG_USB_CHGPTH_PWR_PATH__DC_CHARGER) 
   {

      bootup_threshold = chg_param_ptr->wipwr_bootup_battery_theshold_mv;
   }
   else if (charger_path == PM_SMBCHG_USB_CHGPTH_PWR_PATH__USB_CHARGER)
   {
      bootup_threshold = chg_param_ptr->bootup_battery_theshold_mv;
   }

   //Enable BMS FG Algorithm BCL
   err_flag |= pm_fg_adc_usr_enable_bcl_monitoring(device_index, TRUE);
   if ( err_flag != PM_ERR_FLAG__SUCCESS )  
   { 
       return err_flag;
   }


   while( vbatt_weak_status == TRUE )  //While battery is in weak state
   {
      //Check Vbatt ADC level  
      err_flag |= pm_fg_adc_usr_get_bcl_values(device_index, &adc_reading_ready); //Check if Vbatt ADC is ready

      //Check if Vbatt ADC is Ready
      for (wait_index = 0; wait_index < PM_MAX_ADC_READY_DELAY; wait_index++)
      {
        if(adc_reading_ready == FALSE)
        {
           udelay(PM_MIN_ADC_READY_DELAY);
           err_flag |= pm_fg_adc_usr_get_bcl_values(device_index,&adc_reading_ready);
        }
        else
        {
           break;
        }
      }

      if ( err_flag != PM_ERR_FLAG__SUCCESS )  { break;} 

      if ( adc_reading_ready)
      {
         err_flag |= pm_fg_adc_usr_get_calibrated_vbat(device_index, &vbat_adc); //Read calibrated vbatt ADC
         if ( err_flag != PM_ERR_FLAG__SUCCESS )  { break;}

		/* FG_ADC hardware reports values that are off by ~120 to 200 mV, this results in boot up failures
		 * on devices that boot up with battery close to threshold value. If the FG_ADC voltage is less than
		 * threshold then read the voltage from a more accurate source FG SRAM to ascertain the voltage is indeed low.
		 */
		if (!pm_app_read_from_sram && (vbat_adc <= bootup_threshold))
		{
			if (board_pmic_type(PMIC_IS_PMI8996))
				pm_app_ima_read_voltage(&vbat_adc);
			else
				pm_app_pmi8994_read_voltage(&vbat_adc);

			pm_app_read_from_sram = true;
		}

         //Check if ADC reading is within limit
         if ( vbat_adc >=  bootup_threshold)  //Compaire it with SW bootup threshold
         {
            vbatt_weak_status = FALSE;
            break; //bootup
         }
		dprintf(INFO, "Vbatt Level: %u\n", vbat_adc);
   }
   else
   {
         boot_log_message("ERROR:  ADC Reading is NOT Ready\n");
         err_flag |= PM_ERR_FLAG__ADC_NOT_READY;
         break; 
   }

      //Check if USB charger is SDP
      err_flag |= pm_smbchg_misc_chgr_port_detected(device_index, &chgr_src_detected);
      if (chgr_src_detected == PM_SMBCHG_MISC_SRC_DETECT_SDP) 
   {
         if (configure_icl_flag == FALSE)
         {
            //Check Vlow_batt status
            err_flag |= pm_smbchg_chgr_vbat_sts(device_index, &vbatt_status);
            if (vbatt_status)
            {
               //set ICL to 500mA
               err_flag |= pm_smbchg_usb_chgpth_set_cmd_il(device_index, PM_SMBCHG_USBCHGPTH_CMD_IL__USB51_MODE, TRUE);
               err_flag |= pm_smbchg_usb_chgpth_set_cmd_il(device_index, PM_SMBCHG_USBCHGPTH_CMD_IL__USBIN_MODE_CHG, FALSE);
               configure_icl_flag = TRUE;
            }
         }
   }

      if (chg_prog_message_flag == FALSE)
      {
          //Ensure that Charging is enabled
          err_flag |= pm_smbchg_chgr_enable_src(device_index, FALSE);
          err_flag |= pm_smbchg_chgr_set_chg_polarity_low(device_index, TRUE);
          err_flag |= pm_smbchg_bat_if_config_chg_cmd(device_index, PM_SMBCHG_BAT_IF_CMD__EN_BAT_CHG, FALSE);
          udelay(PM_WEAK_BATTERY_CHARGING_DELAY);
      }

      //Check if JEITA check is enabled
      if (chg_param_ptr->enable_jeita_hard_limit_check == TRUE)
      {
         //Read JEITA condition
         err_flag |= pm_smbchg_bat_if_irq_status(device_index, PM_SMBCHG_BAT_IF_HOT_BAT_HARD_LIM,  PM_IRQ_STATUS_RT, &hot_bat_hard_lim_rt_sts );
         err_flag |= pm_smbchg_bat_if_irq_status(device_index, PM_SMBCHG_BAT_IF_COLD_BAT_HARD_LIM, PM_IRQ_STATUS_RT, &cold_bat_hard_lim_rt_sts);
         if ( err_flag != PM_ERR_FLAG__SUCCESS )  { break;}  

         if ( ( hot_bat_hard_lim_rt_sts  == TRUE ) || (cold_bat_hard_lim_rt_sts == TRUE) )  
         {
            continue;  // Stay in this loop as long as JEITA Hard Hot/Cold limit is exceeded
         }
      }

	if (!charge_in_progress)
      dprintf(INFO,"APPSBL Weak Battery charging: Start\n");

      charge_in_progress = true;
#if DISPLAY_SPLASH_SCREEN
	display_thread_initialize();
#endif
      /* Wait for 500 msecs before looking for vbat */
      udelay(PM_WEAK_BATTERY_CHARGING_DELAY); //500ms

      //Check if Charging in progress
      err_flag |= pm_smbchg_chgr_get_chgr_sts(device_index, &vbatt_chging_status);
      if ( err_flag != PM_ERR_FLAG__SUCCESS )  { break;}

      if ( vbatt_chging_status.charging_type == PM_SMBCHG_CHGR_NO_CHARGING )
      {
         if (charger_path == PM_SMBCHG_USB_CHGPTH_PWR_PATH__DC_CHARGER) 
         {
            //Delay for 3.5sec for charging to begin, and check charging status again prior to shutting down.
            udelay(PM_WIPOWER_START_CHARGING_DELAY); //3500ms 

            err_flag |= pm_smbchg_chgr_get_chgr_sts(device_index, &vbatt_chging_status);
            if ( err_flag != PM_ERR_FLAG__SUCCESS )  { break;}

            if ( vbatt_chging_status.charging_type == PM_SMBCHG_CHGR_NO_CHARGING )
            {
               boot_log_message("ERROR: Charging is NOT in progress: Shutting Down\n");
               shutdown_device();
            }
         }
         else
         {
            boot_log_message("ERROR: Charging is NOT in progress: Shutting Down\n");
            shutdown_device();
         }
      }
      else
      {
#ifdef DEBUG_CHARGER
          dprintf(INFO, "APPSBL Charging in Progress....\n");
#endif
          chg_prog_message_flag = TRUE;
      }
   }//while


   if (charger_path == PM_SMBCHG_USB_CHGPTH_PWR_PATH__DC_CHARGER) 
   {
      //If battery is good, Toggle SHDN_N_CLEAR_CMD Reg:  Set 0x1340[6] to  1 and then  0
      err_flag = pm_smbchg_usb_chgpth_set_cmd_il(device_index, PM_SMBCHG_USBCHGPTH_CMD_IL__SHDN_N_CLEAR_CMD, TRUE);
      err_flag = pm_smbchg_usb_chgpth_set_cmd_il(device_index, PM_SMBCHG_USBCHGPTH_CMD_IL__SHDN_N_CLEAR_CMD, FALSE);
   }
   
   if (charge_in_progress)
     dprintf(INFO, "APPSBL Weak Battery Charging: Done \n");

   charge_in_progress = false;
   return err_flag; 
}



pm_err_flag_type pm_smbchg_get_charger_path(uint32 device_index, pm_smbchg_usb_chgpth_pwr_pth_type* charger_path)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   boolean usbin_uv_status = TRUE;
   boolean usbin_ov_status = TRUE;
   boolean dcbin_uv_status = TRUE;
   boolean dcbin_ov_status = TRUE;

   //DC charger present, if DCIN_UV_RT_STS and DCIN_UV_RT_STS status is 0 (INT_RT_STS : 0x1410[1] and [0] == 0)
   err_flag |= pm_smbchg_dc_chgpth_irq_status(device_index, PM_SMBCHG_DC_CHGPTH_DCBIN_UV, PM_IRQ_STATUS_RT, &dcbin_uv_status);
   err_flag |= pm_smbchg_dc_chgpth_irq_status(device_index, PM_SMBCHG_DC_CHGPTH_DCBIN_OV, PM_IRQ_STATUS_RT, &dcbin_ov_status);
   //USB charger present, if USBIN_UV_RT_STS and USBIN_OV_RT_STS status is 0 ( INT_RT_STS : 0x1310[1] and [0] == 0)
   err_flag |= pm_smbchg_usb_chgpth_irq_status(device_index, PM_SMBCHG_USB_CHGPTH_USBIN_UV, PM_IRQ_STATUS_RT, &usbin_uv_status);
   err_flag |= pm_smbchg_usb_chgpth_irq_status(device_index, PM_SMBCHG_USB_CHGPTH_USBIN_OV, PM_IRQ_STATUS_RT, &usbin_ov_status);

   if((dcbin_uv_status == FALSE) && (dcbin_ov_status == FALSE))
   {
      *charger_path = PM_SMBCHG_USB_CHGPTH_PWR_PATH__DC_CHARGER;
   }
   else if((usbin_uv_status == FALSE) && (usbin_ov_status == FALSE))
   {
      *charger_path = PM_SMBCHG_USB_CHGPTH_PWR_PATH__USB_CHARGER;
   }
   else
   {
      *charger_path = PM_SMBCHG_USB_CHGPTH_PWR_PATH__INVALID;
   }

   return err_flag;
}



pm_err_flag_type pm_appsbl_chg_config_vbat_low_threshold(uint32 device_index, pm_smbchg_specific_data_type *chg_param_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   pm_dbc_bootup_volt_threshold = chg_param_ptr->dbc_bootup_volt_threshold.vlowbatt_threshold;

   if (chg_param_ptr->dbc_bootup_volt_threshold.enable_config == PM_ENABLE_CONFIG)
   {
   if (pm_dbc_bootup_volt_threshold  >= PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_INVALID)
   {
      err_flag = PM_ERR_FLAG__INVALID_VBATT_INDEXED;
      return err_flag;
   }

      err_flag = pm_smbchg_bat_if_set_low_batt_volt_threshold(device_index, pm_dbc_bootup_volt_threshold);
#ifdef DEBUG_CHARGER
      dprintf(INFO,"Configure Vlowbatt threshold");
#endif
   }

   return err_flag;
}

#ifndef LK
pm_err_flag_type pm_sbl_config_fg_sram(uint32 device_index)
   {
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  FgSramAddrDataEx_type *sram_data_ptr = NULL;
  FgSramAddrDataEx_type pm_sbl_sram_data[SBL_PACKED_SRAM_CONFIG_SIZE];
  pm_model_type pmic_model = PMIC_IS_INVALID;
  boolean sram_enable_config_flag = FALSE;

  //Check if any SRAM configuration is needed
  sram_data_ptr = (FgSramAddrDataEx_type*)pm_target_information_get_specific_info(PM_PROP_FG_SPECIFIC_DATA);
  CORE_VERIFY_PTR(sram_data_ptr);
  for (int i=0; i< SBL_SRAM_CONFIG_SIZE; i++) 
  {
     sram_enable_config_flag |= sram_data_ptr[i].EnableConfig;
   }

   
  if (sram_enable_config_flag == TRUE )
  {
     pmic_model = pm_get_pmic_model(device_index);   //Check if PMIC exists
     if ( (pmic_model != PMIC_IS_INVALID) || (pmic_model != PMIC_IS_UNKNOWN) )
     {
        //boot_log_message("BEGIN: Configure FG SRAM");

        //Pre-process JEITA data
        pm_sbl_sram_data[0].SramAddr = sram_data_ptr[0].SramAddr;
        pm_sbl_sram_data[0].SramData = (sram_data_ptr[3].SramData  << 24)| 
                                       (sram_data_ptr[2].SramData  << 16)|   
                                       (sram_data_ptr[1].SramData  <<  8)|   
                                        sram_data_ptr[0].SramData;
        pm_sbl_sram_data[0].DataOffset = sram_data_ptr[0].DataOffset;  
        pm_sbl_sram_data[0].DataSize = 4;
        //Set JEITA threshould configuration flag
        pm_sbl_sram_data[0].EnableConfig = sram_data_ptr[0].EnableConfig | sram_data_ptr[1].EnableConfig | 
                                           sram_data_ptr[2].EnableConfig | sram_data_ptr[3].EnableConfig;   

        //Pre-process Thermistor Beta Data
        //thremistor_c1_coeff
        pm_sbl_sram_data[1]  = sram_data_ptr[4];

        //thremistor_c2_coeff and thremistor_c3_coeff
        pm_sbl_sram_data[2].SramAddr   = sram_data_ptr[5].SramAddr;
        pm_sbl_sram_data[2].SramData   = (sram_data_ptr[6].SramData << 16) | sram_data_ptr[5].SramData;
        pm_sbl_sram_data[2].DataOffset = sram_data_ptr[5].DataOffset;  
        pm_sbl_sram_data[2].DataSize = 4;
        pm_sbl_sram_data[2].EnableConfig   = sram_data_ptr[5].EnableConfig;

        //Configure SRAM Data
        err_flag |= PmicFgSram_ProgBurstAccessEx(device_index, pm_sbl_sram_data, SBL_PACKED_SRAM_CONFIG_SIZE);

        //Test: Read Back
        //err_flag |= PmicFgSram_Dump(device_index, 0x0454, 0x0454);
        //err_flag |= PmicFgSram_Dump(device_index, 0x0444, 0x0448);
        //err_flag |= PmicFgSram_Dump(device_index, 0x0448, 0x0452);
        
        //boot_log_message("END: Configure FG SRAM");
     }
  }

   return err_flag; 
}




pm_err_flag_type pm_sbl_config_chg_parameters(uint32 device_index)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   static pm_smbchg_specific_data_type *chg_param_ptr;
   
   if(chg_param_ptr == NULL)
   {
      chg_param_ptr = (pm_smbchg_specific_data_type*)pm_target_information_get_specific_info(PM_PROP_SMBCHG_SPECIFIC_DATA);
      CORE_VERIFY_PTR(chg_param_ptr);
   }

   //Vlowbatt Threshold  
   //  - Done on:  pm_sbl_chg_config_vbat_low_threshold()
   
   //Charger Path Input Priority 
   if (chg_param_ptr->chgpth_input_priority.enable_config == PM_ENABLE_CONFIG)
   {
      pm_smbchg_chgpth_input_priority_type chgpth_priority = chg_param_ptr->chgpth_input_priority.chgpth_input_priority;
      if (chgpth_priority < PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_INVALID) 
      {
          err_flag |= pm_smbchg_chgpth_set_input_priority(device_index, chgpth_priority);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }


   //Battery Missing Detection Source 
   if (chg_param_ptr->bat_miss_detect_src.enable_config == PM_ENABLE_CONFIG)
   {
      pm_smbchg_bat_miss_detect_src_type batt_missing_det_src = chg_param_ptr->bat_miss_detect_src.bat_missing_detection_src;
      if (batt_missing_det_src < PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_INVALID) 
      {
          err_flag |= pm_smbchg_bat_if_set_bat_missing_detection_src(device_index, batt_missing_det_src);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }

   //WDOG Timeout      
   if (chg_param_ptr->wdog_timeout.enable_config == PM_ENABLE_CONFIG)
   {
      pm_smbchg_wdog_timeout_type wdog_timeout = chg_param_ptr->wdog_timeout.wdog_timeout;
      if (wdog_timeout < PM_SMBCHG_MISC_WD_TMOUT_INVALID) 
      {
          err_flag |= pm_smbchg_misc_set_wdog_timeout(device_index, wdog_timeout);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }


   //Enable WDOG                      
   if (chg_param_ptr->enable_wdog.enable_config == PM_ENABLE_CONFIG)
   {
      pm_smbchg_wdog_timeout_type enable_smbchg_wdog = chg_param_ptr->enable_wdog.enable_wdog;
      err_flag |= pm_smbchg_misc_enable_wdog(device_index, enable_smbchg_wdog);
   }


   //FAST Charging Current            
   if (chg_param_ptr->fast_chg_i.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 fast_chg_i_ma = chg_param_ptr->fast_chg_i.fast_chg_i_ma;
      if ((fast_chg_i_ma >= 300) && (fast_chg_i_ma <= 3000) )
      {
          err_flag |= pm_smbchg_chgr_set_fast_chg_i(device_index, fast_chg_i_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }

   //Pre Charge Current               
   if (chg_param_ptr->pre_chg_i.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 pre_chg_i_ma = chg_param_ptr->pre_chg_i.pre_chg_i_ma;
      if ((pre_chg_i_ma >= 100) && (pre_chg_i_ma <= 550) )
      {
          err_flag |= pm_smbchg_chgr_set_pre_chg_i(device_index, pre_chg_i_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }

   //Pre to Fast Charge Current       
   if (chg_param_ptr->pre_to_fast_chg_theshold_mv.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 p2f_chg_mv = chg_param_ptr->pre_to_fast_chg_theshold_mv.pre_to_fast_chg_theshold_mv;
      if ((p2f_chg_mv >= 2400) && (p2f_chg_mv <= 3000)  )
      {
          err_flag |= pm_smbchg_chgr_set_p2f_threshold(device_index, p2f_chg_mv);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }

   //Float Voltage : 3600mV to 4500 mv                   
   if (chg_param_ptr->float_volt_theshold_mv.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 float_volt_mv = chg_param_ptr->float_volt_theshold_mv.float_volt_theshold_mv;
      if ((float_volt_mv >= 3600) && (float_volt_mv <= 4500))
      {
          err_flag |= pm_smbchg_chgr_set_float_volt(device_index, float_volt_mv);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }


   //USBIN Input Current Limit  :Valid value is 300 to 3000mAmp      
   if (chg_param_ptr->usbin_input_current_limit.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 usbin_i_limit_ma = chg_param_ptr->usbin_input_current_limit.usbin_input_current_limit;
      if ((usbin_i_limit_ma >= 300) && (usbin_i_limit_ma <= 3000))
      {
          err_flag |= pm_smbchg_usb_chgpth_set_usbin_current_limit(device_index, usbin_i_limit_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }


   //DCIN Input Current Limit : valid range is 300 to 2000 mAmp         
   if (chg_param_ptr->dcin_input_current_limit.enable_config == PM_ENABLE_CONFIG)
   {
      uint32 dcin_i_limit_ma = chg_param_ptr->dcin_input_current_limit.dcin_input_current_limit;
      if ((dcin_i_limit_ma >= 300) && (dcin_i_limit_ma <= 3200))
      {
          err_flag |= pm_smbchg_dc_chgpth_set_dcin_current_limit(device_index, dcin_i_limit_ma);
      }
      else
      {
         err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      }
   }


   return err_flag; 
}
#endif

bool pm_appsbl_charging_in_progress()
{
	return charge_in_progress;
}

bool pm_appsbl_display_init_done()
{
	return display_initialized;
}

pm_err_flag_type pm_appsbl_set_dcin_suspend(uint32_t device_index)
{
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

	err_flag = pm_smbchg_usb_chgpth_set_cmd_il(device_index, PM_SMBCHG_USBCHGPTH_CMD_IL__DCIN_SUSPEND, TRUE);

	return err_flag;
}

static bool is_power_key_pressed()
{
	int count = 0;

	if (pm8x41_get_pwrkey_is_pressed())
	{
		while(count++ < 10 && pm8x41_get_pwrkey_is_pressed())
			thread_sleep(100);

		dprintf(INFO, "Power Key Pressed\n");
		return true;
	}

	return false;
}

bool pm_app_display_shutdown_in_prgs()
{
	return display_shutdown_in_prgs;
}

static int display_charger_screen()
{
	static bool display_init_first_time;

	/* By default first time display the charger screen
	 * Wait for 5 seconds and turn off the display
	 * If user presses power key & charging is in progress display the charger screen
	 */
	do {
		if (!display_init_first_time || (is_power_key_pressed() && charge_in_progress))
		{
			/* Display charger screen */
			target_display_init(panel_name);
			/* wait for 5 seconds to show the charger screen */
			display_initialized = true;
			thread_sleep(PM_CHARGE_DISPLAY_TIMEOUT);
			/* Shutdown the display: If the charging is complete
			 * continue boot up with display on
			 */
			if (charge_in_progress)
			{
				display_shutdown_in_prgs = true;
				target_display_shutdown();
				display_shutdown_in_prgs = false;
				display_initialized = false;
			}
			display_init_first_time = true;
		}
		/* Wait for 100ms before reading the pmic interrupt status
		 * again, reading the pmic interrupt status in a loop without delays
		 * reports false key presses */
		thread_sleep(100);
	} while (charge_in_progress);

	return 0;
}

/* Create a thread to monitor power key press events
 * and turn on/off the display for battery
 */
static void display_thread_initialize()
{
	thread_t *thr = NULL;
	static bool is_thread_start;

	if (!is_thread_start)
	{
		thr = thread_create("display_charger_screen", &display_charger_screen, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
		if (!thr)
		{
			dprintf(CRITICAL, "Error: Could not create display charger screen thread\n");
			return;
		}
		thread_resume(thr);

		is_thread_start = true;
	}
}

static void pm_app_wait_for_iacs_ready(uint32_t sid)
{
	uint8_t iacs;
	int max_retry = 100;

	udelay(50);
	pm_comm_read_byte(sid, 0x4454, &iacs, 0);
	while ((iacs & 0x02) == 0)
	{
		max_retry--;
		pm_comm_read_byte(2, 0x4454, &iacs, 0);
		mdelay(5);
		if (!max_retry)
		{
			dprintf(CRITICAL, "Error: IACS not ready, shutting down\n");
			shutdown_device();
		}
	}
}

static int pm_app_check_for_ima_exception(uint32_t sid)
{
	uint8_t ima_err_sts;
	uint8_t ima_exception_sts;

	pm_comm_read_byte(sid, 0x445f, &ima_err_sts, 0);
	pm_comm_read_byte(sid, 0x4455, &ima_exception_sts, 0);

	if (ima_err_sts != 0 || (ima_exception_sts & 0x1) == 1)
	{
		uint8_t ima_hw_sts;
		pm_comm_read_byte(sid, 0x4456, &ima_hw_sts, 0);
		dprintf(CRITICAL, "ima_err_sts: %x\tima_exception_sts:%x\tima_hw_sts:%x\n", ima_err_sts, ima_exception_sts, ima_hw_sts);
		return 1;
	}

	return 0;
}

static void pm_app_ima_read_voltage(uint32_t *voltage)
{
	uint8_t start_beat_count;
	uint8_t end_beat_count;
	uint8_t vbat;
	uint64_t vbat_adc = 0;
	uint32_t sid = 2; //sid for pmi8996
	int max_retry = 5;

retry:
	//Request IMA access
	pm_comm_write_byte(sid, 0x4450, 0xA0, 0);
	// Single read configure
	pm_comm_write_byte(sid, 0x4451, 0x00, 0);

	pm_app_wait_for_iacs_ready(sid);

	//configure SRAM access
	pm_comm_write_byte(sid, 0x4461, 0xCC, 0);
	pm_comm_write_byte(sid, 0x4462, 0x05, 0);

	pm_app_wait_for_iacs_ready(sid);

	pm_comm_read_byte(sid, 0x4457, &start_beat_count, 0);

	//Read the voltage
	pm_comm_read_byte(sid, 0x4467, &vbat, 0);
	vbat_adc = vbat;
	pm_comm_read_byte(sid, 0x4468, &vbat, 0);
	vbat_adc |= (vbat << 8);
	pm_comm_read_byte(sid, 0x4469, &vbat, 0);
	vbat_adc |= (vbat << 16);
	pm_comm_read_byte(sid, 0x446A, &vbat, 0);
	vbat_adc |= (vbat << 24);

	pm_app_wait_for_iacs_ready(sid);

	//Look for any errors
	if(pm_app_check_for_ima_exception(sid))
		goto err;

	pm_comm_read_byte(sid, 0x4457, &end_beat_count, 0);

	if (start_beat_count != end_beat_count)
	{
		max_retry--;
		if (!max_retry)
			goto err;
		goto retry;
	}

	//Release the ima access
	pm_comm_write_byte(2, 0x4450, 0x00, 0);

	//extract the byte1 & byte2 and convert to mv
	vbat_adc = ((vbat_adc & 0x00ffff00) >> 8) * 152587;
	*voltage = vbat_adc / 1000000;
	return;

err:
	dprintf(CRITICAL, "Failed to Read the Voltage from IMA, shutting down\n");
	shutdown_device();
}

static void pm_app_pmi8994_read_voltage(uint32_t *voltage)
{
	uint8_t val = 0;
	uint8_t vbat;
	uint64_t vbat_adc = 0;
	uint32_t sid = 2; //sid for pmi8994
	int max_retry = 100;

	pm_comm_read_byte(sid, 0x4440, &val, 0);

	//Request for FG access
	if ((val & BIT(7)) != 1)
		pm_comm_write_byte(sid, 0x4440, 0x80, 0);

	pm_comm_read_byte(sid, 0x4410, &val, 0);
	while((val & 0x1) == 0)
	{
		//sleep and retry again, this takes up to 1.5 seconds
		max_retry--;
		mdelay(100);
		pm_comm_read_byte(sid, 0x4410, &val, 0);
		if (!max_retry)
		{
		  dprintf(CRITICAL, "Error: Failed to read from Fuel Guage, Shutting down\n");
		  shutdown_device();
		}
	}

	//configure single read access
	pm_comm_write_byte(sid, 0x4441, 0x00, 0);
	//configure SRAM for voltage shadow
	pm_comm_write_byte(sid, 0x4442, 0xCC, 0);
	pm_comm_write_byte(sid, 0x4443, 0x05, 0);

	//Read voltage from SRAM
	pm_comm_read_byte(sid, 0x444c, &vbat, 0);
	vbat_adc = vbat;
	pm_comm_read_byte(sid, 0x444d, &vbat, 0);
	vbat_adc |= (vbat << 8);
	pm_comm_read_byte(sid, 0x444e, &vbat, 0);
	vbat_adc |= (vbat << 16);
	pm_comm_read_byte(sid, 0x444f, &vbat, 0);
	vbat_adc |= (vbat << 24);

	//clean up to relase sram access
	pm_comm_write_byte(sid, 0x4440, 0x00, 0);
	//extract byte 1 & byte 2
	vbat_adc = ((vbat_adc & 0x00ffff00) >> 8) * 152587;

	//convert the voltage to mv
	*voltage = vbat_adc / 1000000;
}

