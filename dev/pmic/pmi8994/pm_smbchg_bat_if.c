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

#include "pm_smbchg_bat_if.h"
#include "pm_smbchg_driver.h"
#include <debug.h>
#include <sys/types.h>

/*Find the nearest register value corresponding to input_data*/
static void pm_smbchg_bat_if_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value);

/*writes to secured acess register to unlock pmic write operation*/
static inline pm_err_flag_type pm_smbchg_bat_if_unlock_perph_write(pm_smbchg_data_type *smbchg);

/*Returns battery presence status*/
pm_err_flag_type pm_smbchg_bat_if_get_bat_pres_status(uint32 device_index, boolean *bat_present)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == bat_present)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        pm_register_address_type bat_pres_status = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->bat_pres_status;
        errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, bat_pres_status, 0x80, &data, 0);
        *bat_present = (data) ? TRUE : FALSE;
    }

    return errFlag;
}

/*This API clears the dead battery timer*/
pm_err_flag_type pm_smbchg_bat_if_clear_dead_bat_timer(uint32 device_index)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
     
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type clr_dead_bat_timer = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->clr_dead_bat_timer;

        errFlag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, clr_dead_bat_timer, 0x1, 0x1, 0);
    }

    return errFlag;
}

/*writes to battery interface command control to enable/disable wireless charging, thermal current override, stat output, fast charge, battery charging, OTG */
pm_err_flag_type pm_smbchg_bat_if_config_chg_cmd(uint32 device_index, pm_smbchg_bat_if_cmd_chg_type cmd_chg_cfg, boolean enable)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
   
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (PM_SMBCHG_BAT_IF_CMD__INVALID == cmd_chg_cfg)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        pm_register_address_type cmd_chg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->cmd_chg;

        errFlag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cmd_chg, (0x1<<cmd_chg_cfg), (pm_register_data_type)(enable<<cmd_chg_cfg), 0);
    }

    return errFlag;
}


/*reads battery interface command control settings for enable/disable wireless charging, thermal current override, stat output, fast charge, battery charging, OTG */
pm_err_flag_type pm_smbchg_bat_if_get_chg_cmd(uint32 device_index, pm_smbchg_bat_if_cmd_chg_type cmd_chg_cfg, boolean *enable)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type cmd_chg;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (PM_SMBCHG_BAT_IF_CMD__INVALID == cmd_chg_cfg)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if (NULL == enable)
    {
       errFlag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    else
    {
        cmd_chg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->cmd_chg;
        errFlag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, cmd_chg, &data, 0);

        *enable = (data & (0x1<<cmd_chg_cfg)) ? TRUE : FALSE;
    }

    return errFlag;
}

/*Sets low battery voltag threshold*/
pm_err_flag_type pm_smbchg_bat_if_set_low_batt_volt_threshold(uint32 device_index, pm_smbchg_bat_if_low_bat_thresh_type low_bat_threshold)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type vbl_cfg;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (low_bat_threshold >= PM_SMBCHG_BAT_IF_LOW_BATTERY_THRESH_INVALID)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        vbl_cfg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->vbl_cfg;
        errFlag = pm_smbchg_bat_if_unlock_perph_write(smbchg_ptr);
        errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, vbl_cfg, 0xF, (pm_register_data_type)low_bat_threshold, 0);
    }
    return errFlag;
}

/*Gets low battery voltag threshold*/
pm_err_flag_type pm_smbchg_bat_if_get_low_bat_volt_threshold(uint32 device_index, pm_smbchg_bat_if_low_bat_thresh_type *low_bat_threshold)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_register_address_type vbl_cfg;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == low_bat_threshold)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        vbl_cfg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->vbl_cfg;
        errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, vbl_cfg, 0xF, &data, 0);
        *low_bat_threshold = (pm_smbchg_bat_if_low_bat_thresh_type)data;
    }

    return errFlag;
}

/* This API sets battery missing detection configuration like select if the battery missing monitoring should only happen at the beginning of a charge cycle or every 3 seconds */
pm_err_flag_type pm_smbchg_bat_if_set_bat_missing_cfg(uint32 device_index, pm_smbchg_bat_if_batt_missing_cfg_type *batt_missing_cfg)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_register_address_type bm_cfg;
    uint32 time_pos = 0;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == batt_missing_cfg)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        uint32 *batt_missing_detect_time = smbchg_ptr->chg_range_data->batt_missing_detect_time;
        bm_cfg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->bm_cfg;

        /*Find the nearest register value corresponding to time*/
        while (time_pos < BATT_MISSING_SIZE)
        {
            if (batt_missing_cfg->batt_removal_det_time_usec < batt_missing_detect_time[time_pos])
            {
                break;
            }
            else
            {
                time_pos++;
            }
        }
        batt_missing_cfg->batt_removal_det_time_usec = (time_pos) ? (time_pos - 1) : 0;

        data =  (batt_missing_cfg->batt_removal_det_time_usec << 6) | (batt_missing_cfg->batt_bat_get_override_en << 5) | (batt_missing_cfg->batt_missing_input_plugin_en << 4)
           | (batt_missing_cfg->batt_missing_2p6s_poller_en << 3) | (batt_missing_cfg->batt_missing_algo_en << 2) | (batt_missing_cfg->use_therm_pin_for_batt_missing_src << 1)
           | batt_missing_cfg->use_bmd_pin_for_batt_missing_src;

        errFlag = pm_smbchg_bat_if_unlock_perph_write(smbchg_ptr);
        errFlag |= pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, bm_cfg, data, 0);
    }

    return errFlag;
}


/*This API reads the battery missing configuration settings*/
pm_err_flag_type pm_smbchg_bat_if_get_bat_missing_cfg(uint32 device_index, pm_smbchg_bat_if_batt_missing_cfg_type *batt_missing_cfg)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type bm_cfg;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == batt_missing_cfg)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        uint32 *batt_missing_detect_time = smbchg_ptr->chg_range_data->batt_missing_detect_time;
        bm_cfg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->bm_cfg;

        errFlag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, bm_cfg, &data, 0);

        batt_missing_cfg->batt_removal_det_time_usec         = batt_missing_detect_time[(data >> 6)];
        batt_missing_cfg->batt_bat_get_override_en           = (data & 0x20) ? TRUE : FALSE;
        batt_missing_cfg->batt_missing_input_plugin_en       = (data & 0x10) ? TRUE : FALSE;
        batt_missing_cfg->batt_missing_2p6s_poller_en        = (data & 0x8) ? TRUE : FALSE;
        batt_missing_cfg->batt_missing_algo_en               = (data & 0x4) ? TRUE : FALSE;
        batt_missing_cfg->use_therm_pin_for_batt_missing_src = (data & 0x2) ? TRUE : FALSE;
        batt_missing_cfg->use_bmd_pin_for_batt_missing_src   = (data & 0x1) ? TRUE : FALSE;
    }
    return errFlag;
}




pm_err_flag_type pm_smbchg_bat_if_set_bat_missing_detection_src(uint32 device_index, pm_smbchg_bat_miss_detect_src_type batt_missing_det_src)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_bat_if_batt_missing_cfg_type batt_missing_cfg;

   if (batt_missing_det_src >= PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      //Get the current configuration
      err_flag = pm_smbchg_bat_if_get_bat_missing_cfg(device_index, &batt_missing_cfg);
 
      if (err_flag == PM_ERR_FLAG__SUCCESS)
      {
         //Change needed bits and write it back
         switch (batt_missing_det_src)
         {
         case PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_THERM_PIN:
            batt_missing_cfg.use_bmd_pin_for_batt_missing_src   = 0;
            batt_missing_cfg.use_therm_pin_for_batt_missing_src = 1;
            break;
         case PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_BMD_PIN:
            batt_missing_cfg.use_bmd_pin_for_batt_missing_src   = 1;
            batt_missing_cfg.use_therm_pin_for_batt_missing_src = 0;
            break;
         case PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_THERM_BMD_PIN:
            batt_missing_cfg.use_bmd_pin_for_batt_missing_src   = 1;
            batt_missing_cfg.use_therm_pin_for_batt_missing_src = 1;
            break;
         case PM_SMBCHG_BAT_IF_BAT_MISS_DETECT_SRC_NONE:
            batt_missing_cfg.use_bmd_pin_for_batt_missing_src   = 0;
            batt_missing_cfg.use_therm_pin_for_batt_missing_src = 0;
            break;
         default:
            err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
            break;
         }
         err_flag |= pm_smbchg_bat_if_set_bat_missing_cfg(device_index, &batt_missing_cfg);  
      }
   }
   return err_flag;
}


/*This API sets the minimum system voltage. And below this voltage system and battery are not connected together.Valid values are 315, 345 and 360 milli volt*/
pm_err_flag_type pm_smbchg_bat_if_set_min_sys_volt(uint32 device_index, uint32 min_sys_millivolt)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        if (min_sys_millivolt < 345)
        {
            data = 0;
        }
        else if (min_sys_millivolt < 360)
        {
            data = 1;
        }
        else
        {
            data = 0x2;
        }
        pm_register_address_type cfg_sysmin = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->cfg_sysmin;

        errFlag = pm_smbchg_bat_if_unlock_perph_write(smbchg_ptr);
        errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_sysmin, 0x3, data, 0);
    }

    return errFlag;
}

/*This API reads the configured minimum system voltage */
pm_err_flag_type pm_smbchg_bat_if_get_min_sys_volt(uint32 device_index, uint32 *min_sys_millivolt)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == min_sys_millivolt)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        pm_register_address_type cfg_sysmin = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->cfg_sysmin;
        errFlag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, cfg_sysmin, &data, 0);
        switch (data & 0x03)
        {
        case 0x00:
            *min_sys_millivolt = 315;
            break;
        case 0x01:
            *min_sys_millivolt = 354;
        default:
            *min_sys_millivolt = 360;
        }
    }

    return errFlag;
}

/*This API configures the current limit for pass through mode, low volt mode and high volt mode in milliamp*/
pm_err_flag_type pm_smbchg_bat_if_set_dcin_input_current_limit(uint32 device_index, pm_smbchg_bat_if_dcin_icl_type dcin_icl_type, uint32 current_ma)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type reg;
    
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (dcin_icl_type >= PM_SMBCHG_BAT_IF_DCIN_INVALID_MODE)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        uint32 *current_limits = smbchg_ptr->chg_range_data->dcin_current_limits;

        switch (dcin_icl_type)
        {
        case PM_SMBCHG_BAT_IF_DCIN_PASS_THROUGH_MODE:
            reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->zin_icl_pt;
            break;
        case PM_SMBCHG_BAT_IF_DCIN_LOW_VOLT_MODE:
            reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->zin_icl_lv;
            break;
        case PM_SMBCHG_BAT_IF_DCIN_HIGH_VOLT_MODE:
            reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->zin_icl_hv;
            break;
        default:
            return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        }

        pm_smbchg_bat_if_return_reg_value(current_ma, current_limits, DCIN_SIZE, &data);
        errFlag |= pm_smbchg_bat_if_unlock_perph_write(smbchg_ptr);
        errFlag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, reg, 0x1F, data, 0);
    }

    return errFlag;
}

/*This API reads the configured current limit for pass through mode, low volt mode and high volt mode in milliamp*/
pm_err_flag_type pm_smbchg_bat_if_get_dcin_input_current_limit(uint32 device_index, pm_smbchg_bat_if_dcin_icl_type dcin_icl_type, uint32 *current_ma)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type reg;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (dcin_icl_type >= PM_SMBCHG_BAT_IF_DCIN_INVALID_MODE)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if (NULL == current_ma)
    {
        errFlag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    else
    {
        uint32 *current_limits = smbchg_ptr->chg_range_data->dcin_current_limits;

        switch (dcin_icl_type)
        {
        case PM_SMBCHG_BAT_IF_DCIN_PASS_THROUGH_MODE:
            reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->zin_icl_pt;
            break;
        case PM_SMBCHG_BAT_IF_DCIN_LOW_VOLT_MODE:
            reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->zin_icl_lv;
            break;
        case PM_SMBCHG_BAT_IF_DCIN_HIGH_VOLT_MODE:
            reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->zin_icl_hv;
            break;
        default:
            return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        }

        errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, reg, 0x1F, &data, 0);
        if (DCIN_SIZE <= data)
        {
           errFlag |= PM_ERR_FLAG__INVALID;
        }
        else
        {
           *current_ma = current_limits[data];
        }
    }

    return errFlag;
}

///////////////////////////////
/*This API configures WI PWR timer. div2 falling edge values: 0, 150, 250, 500 usec. wipwr_irq_tmr_us values 1000, 1500,2000,2500,3000,4000,4500 usec*/
///////////////////////////////
pm_err_flag_type pm_smbchg_bat_if_set_wi_pwr_tmr(uint32 device_index, uint32 div2_falling_edge_time_us, uint32 wipwr_irq_tmr_us)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_data_type wi_pwr_tmr = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->wi_pwr_tmr;

        /*div2_falling_edge_time_us: 0 - 0, 1 - 150, 2 - 250, 3 - 500 us*/
        div2_falling_edge_time_us = (div2_falling_edge_time_us > 250) ? 0x3 : ((div2_falling_edge_time_us - 50) / 100);

        /*wipwr_irq_tmr_us: 0 - 1000, 1 - 1500, 2 - 2000, 3 - 2500, 4 - 3000, 5 - 3500, 6 -4000, 7 - 4500 us*/
        wipwr_irq_tmr_us = (wipwr_irq_tmr_us > 4500) ? 5 : (wipwr_irq_tmr_us - 1000) / 500;

        data = (div2_falling_edge_time_us << 4) | wipwr_irq_tmr_us;
        errFlag = pm_smbchg_bat_if_unlock_perph_write(smbchg_ptr);
        errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, wi_pwr_tmr, 0x3F, data, 0);
    }

    return errFlag;
}

/*This API reads the WIPWR timer settings*/
pm_err_flag_type pm_smbchg_bat_if_get_wi_pwr_tmr(uint32 device_index, uint32 *div2_falling_edge_time_us, uint32 *wipwr_irq_tmr_us)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == div2_falling_edge_time_us)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if (NULL == wipwr_irq_tmr_us)
    {
        errFlag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    else
    {
        pm_register_data_type wi_pwr_tmr = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->wi_pwr_tmr;

        errFlag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, wi_pwr_tmr, &data, 0);
        /*div2_falling_edge_time_us: 0 - 0, 1 - 150, 2 - 250, 3 - 500 us*/
        *div2_falling_edge_time_us = ((data & 0x30) == 0x30) ? 500 : (((data & 0x30) >> 4) * 100 + 2);

        /*wipwr_irq_tmr_us: 0 - 1000, 1 - 1500, 2 - 2000, 3 - 2500, 4 - 3000, 5 - 3500, 6 -4000, 7 - 4500 us*/
        *wipwr_irq_tmr_us = (data & 0x7) * 500 + 1000;
    }

    return errFlag;
}


pm_err_flag_type pm_smbchg_bat_if_irq_enable(uint32 device_index, pm_smbchg_bat_if_irq_type irq, boolean enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type irq_reg;
    pm_register_data_type data = 1 << irq;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SMBCHG_BAT_IF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        if (enable)
        {
            irq_reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_en_set;
        }
        else
        {
            irq_reg = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_en_clr;
        }

        errFlag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, irq_reg, data, 0);
    }
    return errFlag;
}

pm_err_flag_type pm_smbchg_bat_if_irq_clear(uint32  device_index, pm_smbchg_bat_if_irq_type irq)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 1 << irq;
    pm_register_address_type int_latched_clr;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SMBCHG_BAT_IF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        int_latched_clr = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_latched_clr;
        errFlag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_smbchg_bat_if_irq_set_trigger(uint32 device_index, pm_smbchg_bat_if_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 1 << irq;
    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SMBCHG_BAT_IF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        int_set_type = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_set_type;
        int_polarity_high = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_polarity_high;
        int_polarity_low = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_polarity_low;

        switch (trigger)
        {
        case PM_IRQ_TRIGGER_ACTIVE_LOW:
            set_type = 0x00;
            polarity_high = 0x00;
            polarity_low = 0xFF;
            break;
        case PM_IRQ_TRIGGER_ACTIVE_HIGH:
            set_type = 0x00;
            polarity_high = 0xFF;
            polarity_low = 0x00;
            break;
        case PM_IRQ_TRIGGER_RISING_EDGE:
            set_type = 0xFF;
            polarity_high = 0xFF;
            polarity_low = 0x00;
            break;
        case PM_IRQ_TRIGGER_FALLING_EDGE:
            set_type = 0xFF;
            polarity_high = 0x00;
            polarity_low = 0xFF;
            break;
        case PM_IRQ_TRIGGER_DUAL_EDGE:
            set_type = 0xFF;
            polarity_high = 0xFF;
            polarity_low = 0xFF;
            break;
        default:
            return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
        errFlag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_set_type, mask, set_type, 0);
        errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_polarity_high, mask, polarity_high, 0);
        errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_polarity_low, mask, polarity_low, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_smbchg_bat_if_irq_status(uint32 device_index, pm_smbchg_bat_if_irq_type irq, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    uint8 mask = 1 << irq;
    pm_register_address_type int_sts;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

    if (NULL == smbchg_ptr)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SMBCHG_BAT_IF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        switch (type)
        {
        case PM_IRQ_STATUS_RT:
            int_sts = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_rt_sts;
            break;
        case PM_IRQ_STATUS_LATCHED:
            int_sts = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_latched_sts;
            break;
        case PM_IRQ_STATUS_PENDING:
            int_sts = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->int_pending_sts;
            break;
        default:
            return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }

        errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
        *status = data ? TRUE : FALSE;
    }

    return errFlag;
}

/*Find the nearest register value corresponding to input_data*/
static void pm_smbchg_bat_if_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value)
{
    uint32 loc = 0;

    while (loc < array_size)
    {
        if (input_data < array[loc])
        {
            break;
        }
        else
        {
            loc++;
        }
    }

    *reg_value = (loc) ? (loc - 1) : 0;

    return;
}


static inline pm_err_flag_type pm_smbchg_bat_if_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if (NULL == smbchg_ptr)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type sec_access = smbchg_ptr->smbchg_register->bat_if_register->base_address + smbchg_ptr->smbchg_register->bat_if_register->sec_access;
        err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
    }

    return err_flag;
}

