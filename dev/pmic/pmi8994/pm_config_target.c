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
#include "pm_smbchg_driver.h"
#include "pm_app_smbchg.h"
#include "pm_smbchg_bat_if.h"
#include <sys/types.h>

/* 2 slave IDs per PMIC device. */
#define PM_MAX_SLAVE_ID          2

 /************************************************ DRIVER DATA *********************************/
 #ifndef LK
//PM8994,PMi8994,PM8004
uint32 num_of_ldo[]        = {32, 1, 1};
uint32 num_of_smps[]       = {12, 3, 5};
uint32 num_of_gpio[]       = {22,10, 0};
uint32 num_of_mpp[]        = {8, 4, 4};
uint32 num_of_rtc[]        = {1, 0, 0};
uint32 num_of_megaxo[]     = {1, 0, 0};
uint32 num_of_boost[]      = {0, 1, 0}; 
uint32 num_of_vs[]         = {2, 0, 0};
uint32 num_of_lpg_chan[]   = {12, 4,0};
uint32 num_of_pwron[]      = {1, 1, 1};
uint32 num_of_misc[]       = {1, 0, 0};
uint32 num_of_clk_buffer[] = {13,13,0};
uint32 num_of_rgb[]        = {0,1,0};
uint32 num_of_wled[]       = {0,1,0};
uint32 num_of_smbchg[]     = {0,1,0};
uint32 num_of_fg[]         = {0,1,0};
uint32 num_of_ibb[]        = {0,1,0};
uint32 num_of_lab[]        = {0,1,0};
uint32 num_of_vib[]        = {0,1,0};
uint32 num_of_pbs_client[] = {16,12,8};
pm_mpp_specific_info_type
mpp_specific[1] =
{
   {0x9E0, 4}
};

pm_lpg_specific_info_type lpg_specific[1] =  
{
    {24}
};

pm_vib_specific_data_type
vib_specific[1] = 
{
  {1200, 2000}
};
#endif
pm_smbchg_specific_data_type
smbchg_specific_data[1] = {
{
      //Configuration Value,                             Enable config
      {PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_3P25,         PM_DISABLE_CONFIG },  //Vlowbatt Threshold
      {PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_USBIN,         PM_DISABLE_CONFIG },  //Charger Path Input Priority
      {PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_BMD_PIN,     PM_DISABLE_CONFIG },  //Battery Missing Detection Source
      {PM_SMBCHG_MISC_WD_TMOUT_18S,                      PM_DISABLE_CONFIG },  //WDOG Timeout
      {FALSE,                                            PM_DISABLE_CONFIG },  //Enable WDOG
      {1000,                                             PM_DISABLE_CONFIG },  //FAST Charging Current
      {250,                                              PM_DISABLE_CONFIG },  //PRE Charge Current
      {3000,                                             PM_DISABLE_CONFIG },  //PRE to Fast Charge Current
      {4200,                                             PM_DISABLE_CONFIG },  //Float Voltage
      {2100,                                             PM_DISABLE_CONFIG },  //USBIN Input Current Limit
      {1000,                                             PM_DISABLE_CONFIG },  //DCIN Input Current Limit
      3600,                                                                    //bootup_battery_theshold_mv
      3800,                                                                    //wipowr bootup battery thesholdmv
      FALSE,                                                                   //Enable/Disable JEITA Hard Temp Limit Check in SBL
   }
};


chg_range_data_type chg_range_data[1] = {
   {
      //batt_missing_detect_time_range
      { 80, 160, 320, 640 },
      //DCIN_range
      { 300, 400, 450, 475, 500, 550, 600, 650, 700, 900, 950, 1000, 1100, 1200, 1400, 1450, 1500, 1600, 1800, 1850, 1880, 1910, 1930, 1950, 1970, 2000 },
      //USBIN_range
      { 300, 400, 450, 475, 500, 550, 600, 650, 700, 900, 950, 1000, 1100, 1200, 1400, 1450, 1500, 1600, 1800, 1850, 1880, 1910, 1930, 1950, 1970, 2000, 2050, 2100, 2300, 2400, 2500, 3000 }
   }
}; 

#ifndef LK
//By default, configuration of the FG params is NOT enabled. since the last parameter(EnableConfig) = 0
//To enable configuration, set EnableConfig = 1
FgSramAddrDataEx_type
fg_sram_data[SBL_SRAM_CONFIG_SIZE] = 
{
     //JEITA Thresholds:
    //SramAddr,  SramData, DataOffset, DataSize, EnableConfig
    { 0x0454,     0x23,     0,          1,        PM_DISABLE_CONFIG }, //JEITA Soft Cold Threshold:  default = 0x23
    { 0x0454,     0x46,     1,          1,        PM_DISABLE_CONFIG }, //JEITA Soft Hot  Threshold:  default = 0x46
    { 0x0454,     0x1E,     2,          1,        PM_DISABLE_CONFIG }, //JEITA Hard Cold Threshold:  default = 0x1E
    { 0x0454,     0x48,     3,          1,        PM_DISABLE_CONFIG }, //JEITA hard Hot  Threshold:  default = 0x48
    
    //Thermistor Beta Coefficents:    
    { 0x0444,     0x86D8,   2,          2,        PM_DISABLE_CONFIG }, //thremistor_c1_coeff:  default = 0x86D8;
    { 0x0448,     0x50F1,   0,          2,        PM_DISABLE_CONFIG }, //thremistor_c2_coeff:  default = 0x50F1;
    { 0x0448,     0x3C11,   2,          2,        PM_DISABLE_CONFIG }  //thremistor_c3_coeff:  default = 0x3C11;  
};

pm_pbs_info_data_type pm_pbs_info_data_a =
{
    256,                /* PBS MEMORY Size */
    PM_PBS_INFO_IN_OTP, /* Place where PBS Info stored */
};

pm_pbs_info_data_type pm_pbs_info_data_b =
{
    128,                /* PBS MEMORY Size */
    PM_PBS_INFO_IN_OTP, /* Place where PBS Info stored */
};

pm_pbs_info_data_type pm_pbs_info_data_c =
{
    256,                /* PBS MEMORY Size */
    PM_PBS_INFO_IN_OTP, /* Place where PBS Info stored */
};

pm_pbs_info_data_type *pm_pbs_info_data[] = {&pm_pbs_info_data_a, &pm_pbs_info_data_b, &pm_pbs_info_data_c};

/* max_num_seqs, ram_start_addr, poff_trig_id, poff_otp_addr, warmreset_trig_id, warmreset_otp_addr */
pm_pbs_custom_seq_data_type pbs_custom_seq_data_a = {2, 0x07DC, 3, 0x000C, 2, 0x0008};

/* max_num_seqs, ram_start_addr, poff_trig_id, poff_otp_addr, warmreset_trig_id, warmreset_otp_addr */
pm_pbs_custom_seq_data_type pbs_custom_seq_data_b = {33, 0x0554, 2, 0x0008, 7, 0x001C};
 
pm_pbs_custom_seq_data_type* pbs_custom_seq_data[] = 
{
   &pbs_custom_seq_data_a,
   &pbs_custom_seq_data_b,
   NULL,
};

/* PPID, channel mapping and ownership.  */

const uint8 pm_periph_bitmap[][PM_MAX_SLAVE_ID][PM_MAX_BITMAP_ENTRIES] =
{
    /* PM8994*/
    {
        {
          0x72, 0x07, 0x00, 0x10,
          0x10, 0x11, 0x16, 0x00,
          0x00, 0x00, 0x37, 0x3f,
          0x07, 0x00, 0xff, 0xff,
          0x01, 0x00, 0x00, 0x00,
          0xff, 0x00, 0x00, 0x00,
          0xff, 0xff, 0x3f, 0x00,
          0x00, 0x00, 0x00, 0x40,
        },
        {
          0x00, 0x00, 0xf1, 0xff,
          0xff, 0xff, 0xff, 0x00,
          0xff, 0xff, 0xff, 0xff,
          0x00, 0x00, 0x00, 0x00,
          0x03, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x7f, 0x10,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
        },
    },
    /* PMI8994*/
    {
        {
          0x72, 0x03, 0x5f, 0x18,
          0x00, 0x10, 0x02, 0x00,
          0x1f, 0x00, 0x00, 0x02,
          0x00, 0x00, 0xff, 0x1f,
          0x00, 0x00, 0x00, 0x00,
          0x0f, 0x00, 0x00, 0x00,
          0xff, 0x03, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x40,
        },
        {
          0x00, 0x00, 0xf1, 0x1f,
          0x00, 0x00, 0x00, 0x00,
          0x01, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x07, 0x00, 0x1f, 0x00,
          0x01, 0x00, 0x09, 0x53,
          0x00, 0x00, 0x00, 0x00,
        }
    },
  /* PM8004 */
    {
        {
          0x00, 0x03, 0x00, 0x00,
          0x10, 0x10, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x02,
          0x00, 0x00, 0xff, 0x01,
          0x00, 0x00, 0x00, 0x00,
          0x0f, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x40,
        },
        {
          0x00, 0x00, 0xf1, 0xe3,
          0x07, 0x00, 0x00, 0x00,
          0x01, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00,

        }
    }
};
#endif

void *pm_target_information_get_specific_info()
{
	return (void *) smbchg_specific_data;
};

void *pm_target_chg_range_data()
{
	return (void *) chg_range_data;
}
