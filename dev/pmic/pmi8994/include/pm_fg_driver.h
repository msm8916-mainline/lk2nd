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

#ifndef __PM_FG_DRIVER_H__
#define __PM_FG_DRIVER_H__

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_comm.h"

/*===========================================================================
                        TYPE DEFINES AND ENUMS
===========================================================================*/

/*  FG SOC register */
typedef struct fg_soc_register_ds
{
  pm_register_address_type base_address;
  pm_register_address_type peripheral_offset;

  pm_register_address_type fg_soc_alg_sts;               //0x006
  pm_register_address_type fg_soc_alg_aux_sts0;          //0x007
  pm_register_address_type fg_soc_sleep_shutdown_sts;//0x008
  pm_register_address_type fg_soc_monotonic_soc;         //0x009
  pm_register_address_type fg_soc_monotonic_soc_cp;      //0x00A

  pm_register_address_type int_rt_sts;               //0x010
  pm_register_address_type int_set_type;             //0x011
  pm_register_address_type int_polarity_high;        //0x012
  pm_register_address_type int_polarity_low;         //0x013
  pm_register_address_type int_latched_clr;          //0x014
  pm_register_address_type int_en_set;               //0x015
  pm_register_address_type int_en_clr;               //0x016
  pm_register_address_type int_latched_sts;          //0x018
  pm_register_address_type int_pending_sts;          //0x019
  pm_register_address_type int_mid_sel;              //0x01A
  pm_register_address_type int_priority;             //0x01B

  pm_register_address_type fg_soc_boot_mode;         //0x050
  pm_register_address_type fg_soc_restart;           //0x051
  pm_register_address_type fg_soc_wdog_exp;              //0x052
  pm_register_address_type fg_soc_sts_clr;           //0x053

  pm_register_address_type fg_soc_sec_access;        //0x0D0

  pm_register_address_type fg_soc_vadc_data0;        //0x0E8
  pm_register_address_type fg_soc_bcl_tst0;          //0x0E9

  pm_register_address_type fg_soc_alg_sts_cmp;       //0x0EB

  pm_register_address_type fg_soc_trim_num;          //0x0F0

  pm_register_address_type fg_soc_fg_pbs_trig;       //0x0F1
  pm_register_address_type fg_soc_fg_pbs_trig_mux;   //0x0F2
  pm_register_address_type fg_soc_fg_reset;          //0x0F3
  pm_register_address_type fg_soc_fg_cya_cfg;        //0x0F4
  pm_register_address_type fg_soc_low_power_cfg;     //0x0F5
  pm_register_address_type fg_soc_bcl_v_gain_batt_trim;//0x0F6
  pm_register_address_type fg_soc_bcl_i_gain_rsense_trim;//0x0F7
  pm_register_address_type fg_soc_bcl_i_offset_rsense_trim;//0x0F8
  pm_register_address_type fg_soc_bcl_i_gain_batfet_trim;//0x0F9
  pm_register_address_type fg_soc_bcl_i_offset_batfet_trim;//0x0FA

}fg_soc_register_ds;

/*  FG Battery register */
typedef struct fg_batt_register_ds
{
  pm_register_address_type base_address;
  pm_register_address_type peripheral_offset;

  //Read Battery ID currently in use by the alogorithm
  pm_register_address_type fg_batt_battery;          //0x006
  //FG System Battery Status 
  pm_register_address_type fg_batt_sys_batt;         //0x007
  //Battery detection 
  pm_register_address_type fg_batt_det;              //0x008
  //Battery status information on JEITA
  pm_register_address_type fg_batt_info_sts;         //0x009

  pm_register_address_type fg_batt_recovery_sts;     //0x00A

  pm_register_address_type int_rt_sts;               //0x010
  pm_register_address_type int_set_type;             //0x011
  pm_register_address_type int_polarity_high;        //0x012
  pm_register_address_type int_polarity_low;         //0x013
  pm_register_address_type int_latched_clr;          //0x014
  pm_register_address_type int_en_set;               //0x015
  pm_register_address_type int_en_clr;               //0x016
  pm_register_address_type int_latched_sts;          //0x018
  pm_register_address_type int_pending_sts;          //0x019
  pm_register_address_type int_mid_sel;              //0x01A
  pm_register_address_type int_priority;             //0x01B

  //Force Battery ID during first SOC re detection
  pm_register_address_type fg_batt_sw_batt_id;       //0x050
  //Battery Profile ID
  pm_register_address_type fg_batt_profile_id;       //0x051
  pm_register_address_type fg_batt_removed_latched;  //0x052
  //Set by sw after FG provides recovery information
  pm_register_address_type fg_batt_batt_recovery;    //0x053
  // Battery secured access
  pm_register_address_type fg_batt_sec_access;       //0x0D0

  pm_register_address_type fg_batt_esr_change_bound; //0x0E2

  pm_register_address_type fg_batt_peek_mux1;        //0x0EB
  pm_register_address_type fg_batt_peek_mux2;        //0x0EC
  pm_register_address_type fg_batt_peek_mux3;        //0x0ED
  pm_register_address_type fg_batt_peek_mux4;        //0x0EE

  //Number of Trim register in pheripheral
  pm_register_address_type fg_batt_trim_num;         //0x0F0
  //Battery Misc configuration ..recovery and thermistor cfg
  pm_register_address_type fg_batt_misc_cfg;         //0x0F1

  //Battery ID trum values 
  pm_register_address_type fg_batt_batt_id_curr0;    //0x0F2
  pm_register_address_type fg_batt_batt_id_curr1;    //0x0F3
  pm_register_address_type fg_batt_esr_cur;          //0x0F4
  //Enable/Disbale ESR mesurement 
  pm_register_address_type fg_batt_esr_meas_en;      //0x0F5
  pm_register_address_type fg_batt_rbias_div_sel;    //0x0F6

}fg_batt_register_ds;


/*  FG Memory Intergace register */
typedef struct fg_memif_register_ds
{
  pm_register_address_type base_address;
  pm_register_address_type peripheral_offset;

  pm_register_address_type fg_memif_revision1;       //0x000
  pm_register_address_type fg_memif_revision2;       //0x001
  pm_register_address_type fg_memif_revision3;       //0x002
  pm_register_address_type fg_memif_revision4;       //0x003

  pm_register_address_type int_rt_sts;               //0x010
  pm_register_address_type int_set_type;             //0x011
  pm_register_address_type int_polarity_high;        //0x012
  pm_register_address_type int_polarity_low;         //0x013
  pm_register_address_type int_latched_clr;          //0x014
  pm_register_address_type int_en_set;               //0x015
  pm_register_address_type int_en_clr;               //0x016
  pm_register_address_type int_latched_sts;          //0x018
  pm_register_address_type int_pending_sts;          //0x019
  pm_register_address_type int_mid_sel;              //0x01A
  pm_register_address_type int_priority;             //0x01B

  //Memory Interface configuration
  pm_register_address_type fg_memif_mem_intf_cfg;    //0x040
  //Memory Interface Control
  pm_register_address_type fg_memif_mem_intf_ctl;    //0x041
  pm_register_address_type fg_memif_mem_intf_addr_lsb;//0x042
  pm_register_address_type fg_memif_mem_intf_addr_msb;//0x043

  pm_register_address_type fg_memif_mem_intf_wr_data0;//0x048
  pm_register_address_type fg_memif_mem_intf_wr_data1;//0x049
  pm_register_address_type fg_memif_mem_intf_wr_data2;//0x04A
  pm_register_address_type fg_memif_mem_intf_wr_data3;//0x04B

  pm_register_address_type fg_memif_mem_intf_rd_data0;//0x04C
  pm_register_address_type fg_memif_mem_intf_rd_data1;//0x04D
  pm_register_address_type fg_memif_mem_intf_rd_data2;//0x04E
  pm_register_address_type fg_memif_mem_intf_rd_data3;//0x04F

  //PMIC Secured Access 
  pm_register_address_type fg_memif_sec_access;      //0x0D0
  //OTP Configuration
  pm_register_address_type fg_memif_otp_cfg1;        //0x0E2
  pm_register_address_type fg_memif_otp_cfg2;        //0x0E3

}fg_memif_register_ds;


/*  FG ADC User register */
typedef struct fg_adc_usr_register_ds
{
  pm_register_address_type base_address;
  pm_register_address_type peripheral_offset;

  pm_register_address_type int_rt_sts;               //0x010
  pm_register_address_type int_set_type;             //0x011
  pm_register_address_type int_polarity_high;        //0x012
  pm_register_address_type int_polarity_low;         //0x013
  pm_register_address_type int_latched_clr;          //0x014
  pm_register_address_type int_en_set;               //0x015
  pm_register_address_type int_en_clr;               //0x016
  pm_register_address_type int_latched_sts;          //0x018
  pm_register_address_type int_pending_sts;          //0x019
  pm_register_address_type int_mid_sel;              //0x01A
  pm_register_address_type int_priority;             //0x01B

  //Enable FG ADC Modgule for BCL
  pm_register_address_type fg_adc_usr_en_ctl;        //0x046
  //Pause the updates ofr battery registers 
  pm_register_address_type fg_adc_usr_access_bat_req;//0x050
  pm_register_address_type fg_adc_usr_access_bat_grnt;//0x051
  // bcl values after first reading from ADC are obtained
  pm_register_address_type fg_adc_usr_bcl_values;    //0x053
  // ADC user secured access
  pm_register_address_type fg_adc_usr_sec_access;    //0x0D0
  // VBAT, IBAT
  pm_register_address_type fg_adc_usr_vbat;          //0x054
  pm_register_address_type fg_adc_usr_ibat;          //0x055
  pm_register_address_type fg_adc_usr_vbat_cp;       //0x056
  pm_register_address_type fg_adc_usr_ibat_cp;       //0x057
  //Min max values
  pm_register_address_type fg_adc_usr_vbat_min;      //0x058
  pm_register_address_type fg_adc_usr_ibat_max;      //0x059
  pm_register_address_type fg_adc_usr_vbat_min_cp;   //0x05A
  pm_register_address_type fg_adc_usr_ibat_max_cp;   //0x05B
  //HALF battery resistance value
  pm_register_address_type fg_adc_usr_bat_res_7_0;   //0x05C
  pm_register_address_type fg_adc_usr_bat_res_15_8;  //0x05D
  //BCL Modes
  pm_register_address_type fg_adc_usr_bcl_mode;      //0x05E
  //Gain Correction
  pm_register_address_type fg_adc_usr_bcl_v_gain_batt;//0x060
  pm_register_address_type fg_adc_usr_bcl_i_gain_rsense;//0x061
  pm_register_address_type fg_adc_usr_bcl_i_offset_rsense;//0x062
  pm_register_address_type fg_adc_usr_bcl_i_gain_batfet;//0x063
  pm_register_address_type fg_adc_usr_bcl_i_offset_batfet;//0x064
  pm_register_address_type fg_adc_usr_bcl_i_sense_source;//0x065
  //vbat ibatmax clear
  pm_register_address_type fg_adc_usr_vbat_min_clr;  //0x066
  pm_register_address_type fg_adc_usr_ibat_max_clr;  //0x067
  //threshold interrrupt 
  pm_register_address_type fg_adc_usr_vbat_int;  //0x068
  pm_register_address_type fg_adc_usr_ibat_int;  //0x069

}fg_adc_usr_register_ds;

typedef struct fg_register_ds
{
  fg_soc_register_ds      *soc_register;
  fg_batt_register_ds     *batt_register;
  fg_memif_register_ds    *memif_register;
  fg_adc_usr_register_ds  *adc_usr_register;

}fg_register_ds;

typedef struct
{
    pm_comm_info_type     *comm_ptr;
    fg_register_ds        *fg_register;
    uint8                 num_of_peripherals;
}pm_fg_data_type;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/

/* driver init */
void pm_fg_driver_init(uint32 pmic_index);

pm_fg_data_type* pm_fg_get_data(uint32 pmic_index);

#endif // __PM_FG_DRIVER_H__
