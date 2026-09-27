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

#ifndef PM_SMBCHG_DRIVER__H
#define PM_SMBCHG_DRIVER__H

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_resources_and_types.h"
#include "pm_comm.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/


/************************************************************************/
/* register definitions                                                                     */
/************************************************************************/
typedef struct
{
    uint32                   base_address;          //0x1200

    pm_register_address_type perph_subtype;         // 0x1205
    pm_register_address_type bat_pres_status;       // 0x1208
    pm_register_address_type int_rt_sts;            // 0x1210
    pm_register_address_type int_set_type;          // 0x1211
    pm_register_address_type int_polarity_high;     // 0x1212
    pm_register_address_type int_polarity_low;      // 0x1213
    pm_register_address_type int_latched_clr;       // 0x1214
    pm_register_address_type int_en_set;            // 0x1215
    pm_register_address_type int_en_clr;            // 0x1216
    pm_register_address_type int_latched_sts;       // 0x1218
    pm_register_address_type int_pending_sts;       // 0x1219
    pm_register_address_type int_mid_sel;           // 0x121a
    pm_register_address_type int_priority;          // 0x121b
    pm_register_address_type ship_mode;             // 0x1240
    pm_register_address_type clr_dead_bat_timer;    // 0x1241
    pm_register_address_type cmd_chg;               // 0x1242
    pm_register_address_type sec_access;            // 0x12d0
    pm_register_address_type spare;                 // 0x12df
    pm_register_address_type int_test1;             // 0x12e0
    pm_register_address_type int_test_val;          // 0x12e1
    pm_register_address_type trim_num;              // 0x12f0
    pm_register_address_type vbl_cfg;               // 0x12f1
    pm_register_address_type vbl_sel_cfg;           // 0x12f2
    pm_register_address_type bm_cfg;                // 0x12f3
    pm_register_address_type cfg_sysmin;            // 0x12f4
    pm_register_address_type cfg_systh;             // 0x12f5
    pm_register_address_type ss_cfg;                // 0x12f6
    pm_register_address_type cfg_bb_clmp_sel;       // 0x12f8
    pm_register_address_type trim9;                 // 0x12f9
    pm_register_address_type isense_cfg1;           // 0x12fa
    pm_register_address_type isense_cfg2;           // 0x12fb
    pm_register_address_type zin_icl_pt;            // 0x12fc
    pm_register_address_type zin_icl_lv;            // 0x12fd
    pm_register_address_type zin_icl_hv;            // 0x12fe
    pm_register_address_type wi_pwr_tmr;            // 0x12ff
}smbchg_bat_if_register_ds;

typedef struct smbchg_otg_register_ds
{
    pm_register_address_type   base_address;       //0x1100

    pm_register_address_type perph_subtype;         // 0x1105
    pm_register_address_type sec_access;            // 0x11d0
    pm_register_address_type spare;                 // 0x11df
    pm_register_address_type otg_cfg;               // 0x11f1
    pm_register_address_type cfg_battuv;            // 0x11f2
    pm_register_address_type otg_icfg;              // 0x11f3
    pm_register_address_type tr_isotg;              // 0x11f4
    pm_register_address_type tr_otg_sns;            // 0x11f5
    pm_register_address_type trim6;                 // 0x11f6
    pm_register_address_type tr_istr;               // 0x11f7
    pm_register_address_type cfg_tloop;             // 0x11f8
    pm_register_address_type chg_pbs_trig_control;  // 0x11f9
    pm_register_address_type tr_pwr_tr;             // 0x11fa
    pm_register_address_type tr_pwr_trb_a;          // 0x11fb
    pm_register_address_type tr_pwr_trb_b;          // 0x11fc
    pm_register_address_type tr_pwr_trs;            // 0x11fd
    pm_register_address_type tr_pwr_tr_bgtr;        // 0x11fe
    pm_register_address_type low_pwr_options;       // 0x11ff
}smbchg_otg_register_ds;

typedef struct
{
    uint32                   base_address;      //0x1000

    pm_register_address_type perph_subtype;     //0x1005
    pm_register_address_type chg_option;        //0x1008
    pm_register_address_type vbat_status;       //0x100B
    pm_register_address_type fv_sts;            //0x100C
    pm_register_address_type ichg_sts;          //0x100D
    pm_register_address_type chgr_sts;          //0x100E
    pm_register_address_type int_rt_sts;        //0x1010
    pm_register_address_type int_set_type;      //0x1011
    pm_register_address_type int_polarity_high; //0x1012
    pm_register_address_type int_polarity_low;  //0x1013
    pm_register_address_type int_latched_clr;   //0x1014
    pm_register_address_type int_en_set;        //0x1015
    pm_register_address_type int_en_clr;        //0x1016
    pm_register_address_type int_latched_sts;   //0x1018
    pm_register_address_type int_pending_sts;   //0x1019
    pm_register_address_type int_mid_sel;       //0x101A
    pm_register_address_type int_priority;      //0x101B
    pm_register_address_type sec_access;        //0x10D0
    pm_register_address_type spare;             //0x10DF
    pm_register_address_type pcc_cfg;           //0x10F1
    pm_register_address_type fcc_cfg;           //0x10F2
    pm_register_address_type fcc_cmp_cfg;       //0x10F3
    pm_register_address_type fv_cfg;            //0x10F4
    pm_register_address_type fv_cmp_cfg;        //0x10F5
    pm_register_address_type cfg_afvc;          //0x10F6
    pm_register_address_type cfg_chg_inhib;     //0x10F7
    pm_register_address_type cfg_p2f;           //0x10F8
    pm_register_address_type cfg_tcc;           //0x10F9
    pm_register_address_type ccmp_cfg;          //0x10FA
    pm_register_address_type chgr_cfg1;         //0x10FB
    pm_register_address_type chgr_cfg2;         //0x10FC
    pm_register_address_type sft_cfg;           //0x10FD
    pm_register_address_type stat_cfg;          //0x10FE
    pm_register_address_type cfg;               //0x10FF
}smbchg_chgr_register_ds;

typedef struct
{
    uint32                   base_address;      //0x1400

    pm_register_address_type perph_subtype;     //0x1405
    pm_register_address_type int_rt_sts;        //0x1410
    pm_register_address_type int_set_type;      //0x1411
    pm_register_address_type int_polarity_high; //0x1412
    pm_register_address_type int_polarity_low;  //0x1413
    pm_register_address_type int_latched_clr;   //0x1414
    pm_register_address_type int_en_set;        //0x1415
    pm_register_address_type int_en_clr;        //0x1416
    pm_register_address_type int_latched_sts;   //0x1418
    pm_register_address_type int_pending_sts;   //0x1419
    pm_register_address_type int_mid_sel;       //0x141a
    pm_register_address_type int_priority;      //0x141b
    pm_register_address_type sec_access;        //0x14d0
    pm_register_address_type spare;             //0x14df
    pm_register_address_type int_test1;         //0x14e0
    pm_register_address_type int_test_val;      //0x14e1
    pm_register_address_type tm_en_atest_dcin;  //0x14e2
    pm_register_address_type trim_num;          //0x14f0
    pm_register_address_type dcin_chgr_cfg;     //0x14f1
    pm_register_address_type dcin_il_cfg;       //0x14f2
    pm_register_address_type dc_aicl_cfg1;      //0x14f3
    pm_register_address_type dc_aicl_cfg2;      //0x14f4
    pm_register_address_type aicl_wl_sel_cfg;   //0x14f5
    pm_register_address_type temp_comp_cfg;     //0x14f6
    pm_register_address_type tr_temp;           //0x14fa
}smbchg_dc_chgpth_register_ds;


typedef struct
{
    uint32                   base_address;          // 0x1600

    pm_register_address_type revision1;             // 0x1600
    pm_register_address_type revision2;             // 0x1601
    pm_register_address_type revision3;             // 0x1602
    pm_register_address_type revision4;             // 0x1603
    pm_register_address_type perph_type;            // 0x1604
    pm_register_address_type perph_subtype;         // 0x1605
    pm_register_address_type idev_sts;              // 0x1608
    pm_register_address_type int_rt_sts;            // 0x1610
    pm_register_address_type int_set_type;          // 0x1611
    pm_register_address_type int_polarity_high;     // 0x1612
    pm_register_address_type int_polarity_low;      // 0x1613
    pm_register_address_type int_latched_clr;       // 0x1614
    pm_register_address_type int_en_set;            // 0x1615
    pm_register_address_type int_en_clr;            // 0x1616
    pm_register_address_type int_latched_sts;       // 0x1618
    pm_register_address_type int_pending_sts;       // 0x1619
    pm_register_address_type int_mid_sel;           // 0x161a
    pm_register_address_type int_priority;          // 0x161b
    pm_register_address_type wdog_rst;              // 0x1640
    pm_register_address_type afp_mode;              // 0x1641
    pm_register_address_type gsm_pa_on_adj_en;      // 0x1642
    pm_register_address_type sec_access;            // 0x16d0
    pm_register_address_type spare;                 // 0x16df
    pm_register_address_type wd_cfg;                // 0x16f1
    pm_register_address_type misc_cfg;              // 0x16f2
    pm_register_address_type cfg_ntc_vout;          // 0x16f3
    pm_register_address_type chgr_trim_options_16;  // 0x16f4
    pm_register_address_type chgr_trim_options_15_8;// 0x16f5
    pm_register_address_type chgr_trim_options_7_0; // 0x16f6
    pm_register_address_type tr_ipwr1;              // 0x16f7
    pm_register_address_type tr_ipwr2;              // 0x16f8
    pm_register_address_type cfg_temp_sel;          // 0x16ff
}smbchg_misc_register_ds;


typedef struct
{
    uint32                   base_address;           //0x1300

    pm_register_address_type perph_subtype;         //0x1305
    pm_register_address_type icl_sts_1;             //0x1307
    pm_register_address_type pwr_pth_sts;           //0x1308
    pm_register_address_type icl_sts_2;             //0x1309
    pm_register_address_type apsd_dg_sts;           //0x130A
    pm_register_address_type rid_sts;               //0x130B
    pm_register_address_type hvdcp_sts;             //0x130C
    pm_register_address_type input_sts;             //0x130D
    pm_register_address_type usbid_valid_id_11_8;   //0x130E
    pm_register_address_type usbid_valid_id_7_0;    //0x130F
    pm_register_address_type int_rt_sts;            //0x1310
    pm_register_address_type int_set_type;          //0x1311
    pm_register_address_type int_polarity_high;     //0x1312
    pm_register_address_type int_polarity_low;      //0x1313
    pm_register_address_type int_latched_clr;       //0x1314
    pm_register_address_type int_en_set;            //0x1315
    pm_register_address_type int_en_clr;            //0x1316
    pm_register_address_type int_latched_sts;       //0x1318
    pm_register_address_type int_pending_sts;       //0x1319
    pm_register_address_type int_mid_sel;           //0x131A
    pm_register_address_type int_priority;          //0x131B
    pm_register_address_type cmd_il;                //0x1340
    pm_register_address_type iusb_max;              //0x1344
    pm_register_address_type enum_timer_stop;       //0x134E
    pm_register_address_type sec_access;            //0x13D0
    pm_register_address_type usbin_chgr_cfg;        //0x13F1
    pm_register_address_type usbin_il_cfg;          //0x13F2
    pm_register_address_type usb_aicl_cfg;          //0x13F3
    pm_register_address_type cfg;                   //0x13F4
    pm_register_address_type apsd_cfg;              //0x13F5
    pm_register_address_type wi_pwr_options;        //0x13FF
}smbchg_usb_chgpth_register_ds;

typedef struct
{
    uint32                   base_address;           //0x1b00

    pm_register_address_type perph_subtype;         //0x1b05
    pm_register_address_type bsi_sts;               //0x1b08
    pm_register_address_type mipi_bif_fsm_sts;      //0x1b09
    pm_register_address_type rx_fifo_sts;           //0x1b0a
    pm_register_address_type bsi_bat_sts;           //0x1b0d
    pm_register_address_type int_rt_sts;            //0x1b10
    pm_register_address_type int_set_type;          //0x1b11
    pm_register_address_type int_polarity_high;     //0x1b12
    pm_register_address_type int_polarity_low;      //0x1b13
    pm_register_address_type int_latched_clr;       //0x1b14
    pm_register_address_type int_en_set;            //0x1b15
    pm_register_address_type int_en_clr;            //0x1b16
    pm_register_address_type int_latched_sts;       //0x1b18
    pm_register_address_type int_pending_sts;       //0x1b19
    pm_register_address_type int_mid_sel;           //0x1b1a
    pm_register_address_type int_priority;          //0x1b1b
    pm_register_address_type bsi_en;                //0x1b46
    pm_register_address_type mipi_bif_err_clear;    //0x1b4f
    pm_register_address_type mipi_bif_fsm_reset;    //0x1b50
    pm_register_address_type mipi_bif_force_bcl_low; //0x1b51
    pm_register_address_type mipi_bif_tau_cfg;      //0x1b52
    pm_register_address_type mipi_bif_mode;         //0x1b53
    pm_register_address_type mipi_bif_en;           //0x1b54
    pm_register_address_type mipi_bif_cfg;          //0x1b55
    pm_register_address_type mipi_bif_rx_cfg;       //0x1b56
    pm_register_address_type mipi_bif_tx_dly;       //0x1b59
    pm_register_address_type mipi_bif_data_tx_0;    //0x1b5a
    pm_register_address_type mipi_bif_data_tx_1;    //0x1b5b
    pm_register_address_type mipi_bif_data_tx_2;    //0x1b5c
    pm_register_address_type mipi_bif_tx_ctl;       //0x1b5d
    pm_register_address_type mipi_bif_data_rx_0;    //0x1b60
    pm_register_address_type mipi_bif_data_rx_1;    //0x1b61
    pm_register_address_type mipi_bif_data_rx_2;    //0x1b62
    pm_register_address_type rx_fifo_word1_0;       //0x1b63
    pm_register_address_type rx_fifo_word1_1;       //0x1b64
    pm_register_address_type rx_fifo_word2_0;       //0x1b65
    pm_register_address_type rx_fifo_word2_1;       //0x1b66
    pm_register_address_type rx_fifo_word3_0;       //0x1b67
    pm_register_address_type rx_fifo_word3_1;       //0x1b68
    pm_register_address_type rx_fifo_word4_0;       //0x1b69
    pm_register_address_type rx_fifo_word4_1;       //0x1b6a
    pm_register_address_type mipi_bif_bcl_raw;      //0x1b6d
    pm_register_address_type mipi_bif_error;        //0x1b70
    pm_register_address_type bat_gone_cfg;          //0x1ba7
    pm_register_address_type bat_rmv_deb_timer;     //0x1ba8
    pm_register_address_type bat_pres_deb_timer;    //0x1ba9
}smbchg_bsi_register_ds;

typedef struct
{
    smbchg_bsi_register_ds          *bsi_register;
    smbchg_chgr_register_ds         *chgr_register;
    smbchg_otg_register_ds          *otg_register;
    smbchg_bat_if_register_ds       *bat_if_register;
    smbchg_usb_chgpth_register_ds   *usb_chgpth_register;
    smbchg_dc_chgpth_register_ds    *dc_chgpth_register;
    smbchg_misc_register_ds         *misc_register;
}smbchg_register_ds;

#define BATT_MISSING_SIZE  4
#define DCIN_SIZE         26
#define USBIN_SIZE        32

typedef struct
{
    uint32 batt_missing_detect_time[BATT_MISSING_SIZE];
    uint32 dcin_current_limits[DCIN_SIZE];
    uint32 usbin_current_limits[USBIN_SIZE];
}chg_range_data_type; 

typedef struct
{
    pm_comm_info_type            *comm_ptr;
    smbchg_register_ds           *smbchg_register;
    uint8                        num_of_peripherals;
    chg_range_data_type          *chg_range_data;
}pm_smbchg_data_type;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_smbchg_driver_init(uint32 pmic_index);

pm_smbchg_data_type* pm_smbchg_get_data(uint32 pmic_index);

#endif // PM_SMBCHG_DRIVER__H
