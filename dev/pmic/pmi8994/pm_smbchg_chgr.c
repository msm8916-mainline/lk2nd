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
#include "pm_smbchg_chgr.h"
#include "pm_smbchg_driver.h"
#include <sys/types.h>

static inline pm_err_flag_type pm_smbchg_chgr_unlock_perph_write(pm_smbchg_data_type *smbchg);


/*Find the nearest register value corresponding to input_data*/
//static void pm_smbchg_chgr_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value)
//{
//   uint32 position = 0;
//
//   while (position < array_size)
//   {
//      if (input_data < array[position])
//      {
//         break;
//      }
//      else
//      {
//         position++;
//      }
//   }
//
//   *reg_value = (position) ? (position - 1) : 0;
//
//   return;
//}

//@todo: implement pm_smbchg_chgr_chg_option

/*This API returns if battery is above VBAT_WEAK threshold or not*/
pm_err_flag_type pm_smbchg_chgr_vbat_sts(uint32 device_index, boolean *is_above_vbat_weak)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == is_above_vbat_weak)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type vbat_status  = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->vbat_status;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, vbat_status, &data, 0);

      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         *is_above_vbat_weak = (data & 0x2) ? TRUE : FALSE;
      }
   }

   return err_flag;
}

//This API returns the float vlotage status
pm_err_flag_type pm_smbchg_chgr_get_float_volt_sts(uint32 device_index, boolean *aicl_in_hard_limit, uint32 *fv_mv)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   uint32 temp_data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == fv_mv)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type fv_sts  = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fv_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, fv_sts, &data, 0);

      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         *aicl_in_hard_limit = (data & 0x40) ? TRUE : FALSE;
         temp_data = data & 0x3F;
         if (temp_data < 0x5)
         {
            temp_data = 3600;
         }
         if (temp_data >= 0x22)
         {
            temp_data = 4500;
         }
         else
         {
            temp_data = (temp_data > 0x2D) ? (3620 + ((temp_data - 0x5) * 20)) : (3600 + ((temp_data - 0x5) * 20));
         }

         *fv_mv = temp_data;
      }
   }

   return err_flag;
}

/*This API returns the pre charger and fast charger current status in milliamp*/
pm_err_flag_type pm_smbchg_chgr_get_chg_i_sts(uint32 device_index, uint32 *pre_charge_current_ma, uint32 *fast_charge_current_ma)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;


   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == fast_charge_current_ma)
   {
      err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
   }
   else
   {
     uint32 *charge_current = smbchg_ptr->chg_range_data->usbin_current_limits;
     if ( NULL == charge_current)
     {
        err_flag |= PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
     }
      pm_register_address_type ichg_sts = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->ichg_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, ichg_sts, &data, 0);

      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         *pre_charge_current_ma = ((data >> 5) & 0x04) ? 550 : (100 + ((data >> 5) * 50));

         *fast_charge_current_ma = charge_current[data & 0x1F];
      }
   }

   return err_flag;
}

/*This API reads the various charging status*/
pm_err_flag_type pm_smbchg_chgr_get_chgr_sts(uint32 device_index, pm_smbchg_chgr_chgr_status_type *chgr_sts)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type chgr_sts_reg;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == chgr_sts)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      chgr_sts_reg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, chgr_sts_reg, &data, 0);

      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         chgr_sts->is_net_i_discharg       = (data & 0x80) ? TRUE : FALSE;
         chgr_sts->is_auto_fv_comp_active  = (data & 0x40) ? TRUE : FALSE;
         chgr_sts->is_chgr_done_once       = (data & 0x20) ? TRUE : FALSE;
         chgr_sts->is_batt_lt_2v           = (data & 0x10) ? TRUE : FALSE;
         chgr_sts->is_chgr_in_hold_off     = (data & 0x8) ? TRUE : FALSE;
         chgr_sts->charging_type           = (pm_smbchg_chgr_charging_type)((data & 0x6) >> 1);
         chgr_sts->charger_enable          = (data & 0x1) ? TRUE : FALSE;
      }
   }

   return err_flag;
}

/*This API configures the pre-charger current value in milliamp. Valid values are 100 to 550 mAmp*/
pm_err_flag_type pm_smbchg_chgr_set_pre_chg_i(uint32 device_index, uint32 pre_chg_i_ma)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type pcc_cfg;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pcc_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->pcc_cfg;

      pre_chg_i_ma = (pre_chg_i_ma<100)? 100 : pre_chg_i_ma;

      data = (pre_chg_i_ma > 250) ? 0x4 : ((pre_chg_i_ma - 100) / 50);

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, pcc_cfg, 0x7, data, 0);
   }

   return err_flag;
}

/*This API configures the fast-charger current value in milliamp. Valid values are 300 to 3000 mAmp*/
pm_err_flag_type pm_smbchg_chgr_set_fast_chg_i(uint32 device_index, uint32 fast_chag_i_ma)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;


   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
     uint32 *charge_current = smbchg_ptr->chg_range_data->usbin_current_limits;
      pm_register_address_type fcc_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fcc_cfg;

      while (data < USBIN_SIZE)
      {
         if (charge_current[data] > fast_chag_i_ma)
         {
            break;
         }
         else
         {
            data++;
         }
      }

      data = (data < USBIN_SIZE) ? (data - 1) : 0;

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, fcc_cfg, 0x1F, data, 0);
   }

   return err_flag;
}

/*This API configures the fast-charger current compensation value in milliamp. Valid values are 300 to 1200 mAmp*/
pm_err_flag_type pm_smbchg_chgr_set_fast_chg_i_cmpn(uint32 device_index, uint32 fast_chg_i_cmp_ma)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type fcc_cmp_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fcc_cmp_cfg;

      if (fast_chg_i_cmp_ma < 700)
      {
         data = 0x00;
      }
      else if (fast_chg_i_cmp_ma < 900)
      {
         data = 0x01;
      }
      else if (fast_chg_i_cmp_ma < 1200)
      {
         data = 0x02;
      }
      else
      {
         data = 0x03;
      }

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, fcc_cmp_cfg, 0x3, data, 0);
   }

   return err_flag;
}

/*This API configures the floating voltage. Valid range is 3600mV to 4500 mv*/
pm_err_flag_type pm_smbchg_chgr_set_float_volt(uint32 device_index, uint32 float_volt_mv)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type fv_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fv_cfg;

      if (float_volt_mv < 3600)
      {
         data = 0x5;
      }
      else if (float_volt_mv > 4500)
      {
         data = 0x33;
      }
      else
      {
         data = ((float_volt_mv - 3600) / 20) + 5;
      }

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, fv_cfg, 0x3F, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_chgr_get_float_volt_cfg(uint32 device_index, uint32 *float_volt_mv)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == float_volt_mv)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type fv_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fv_cfg;
      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, fv_cfg, 0x3F, &data, 0);
      *float_volt_mv = (data > 0x5) ? 0x00 : (3600 + ((data - 0x5) * 20));
   }

   return err_flag;
}

/*This api sets floating voltage conpmensation code. Valid value is from 0 to 6300 mV*/
pm_err_flag_type pm_smbchg_chgr_set_float_volt_cmpn(uint32 device_index, uint32 float_volt_cmpn)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type fv_cmp_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fv_cmp_cfg;

      data = (float_volt_cmpn > 6300) ? 63 : float_volt_cmpn/100;
      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, fv_cmp_cfg, 0x3F, data, 0);
   }

   return err_flag;
}


/*This api reads floating voltage conpmensation code settings. Valid value is from 0 to 63*/
pm_err_flag_type pm_smbchg_chgr_get_float_volt_cmpn(uint32 device_index, uint32 *float_volt_cmpn)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == float_volt_cmpn)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type fv_cmp_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->fv_cmp_cfg;
      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, fv_cmp_cfg, 0x3F, &data, 0);
      *float_volt_cmpn = data * 10;
   }

   return err_flag;
}

/*This api enables/disables Charger Auto Float Voltage Compensation*/
pm_err_flag_type pm_smbchg_chgr_en_afvc(uint32 device_index, boolean enable)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type cfg_afvc = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_afvc;
      data = (enable) ? TRUE : FALSE;
      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_afvc, 0x7, data, 0);
   }

   return err_flag;
}

/*This API sets charge inhibit level*/
pm_err_flag_type pm_smbchg_chgr_set_inhibi_threshold(uint32 device_index, uint32 inhibit_lvl_mv)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      if (inhibit_lvl_mv < 100)
      {
         data = 0x00;
      }
      else if (inhibit_lvl_mv < 200)
      {
         data = 0x1;
      }
      else if (inhibit_lvl_mv < 300)
      {
         data = 0x2;
      }
      else
      {
         data = 0x3;
      }

      pm_register_address_type cfg_chg_inhib = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_chg_inhib;
      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_chg_inhib, 0x3, data, 0);
   }

   return err_flag;
}

/*This API reads charge inhibit level configured*/
pm_err_flag_type pm_smbchg_chgr_get_inhibi_threshold(uint32 device_index, uint32 *inhibit_lvl_mv)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == inhibit_lvl_mv)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type cfg_chg_inhib = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_chg_inhib;
      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_chg_inhib, 0x3, &data, 0);
      *inhibit_lvl_mv = (data == 0x00) ? 50 : (data * 100);
   }

   return err_flag;
}

/*This API sets the pre-charge to full charger threshold. Valid range is 2400mV to 3000mV*/
pm_err_flag_type pm_smbchg_chgr_set_p2f_threshold(uint32 device_index, uint32 milli_volt)

{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      if (milli_volt < 2600)
      {
         data = 0x00;
      }
      else if (milli_volt < 2800)
      {
         data = 0x1;
      }
      else if (milli_volt < 3000)
      {
         data = 0x2;
      }
      else
      {
         data = 0x3;
      }

      pm_register_address_type cfg_p2f = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_p2f;
      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_p2f, 0x3, data, 0);
   }

   return err_flag;
}


/*This API reads the pre-charge to full charger threshold. Valid range is 2400mV to 3000mV*/
pm_err_flag_type pm_smbchg_chgr_get_p2f_threshold(uint32 device_index, uint32 *milli_volt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == milli_volt)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type cfg_p2f = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_p2f;
      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_p2f, 0x3, &data, 0);
      *milli_volt = 2400 + (200 * data);
   }

   return err_flag;
}


/*This API sets the charge termination current in milliamp */
pm_err_flag_type pm_smbchg_chgr_set_charge_termination_current(uint32 device_index, uint32 current_ma)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      /* 000 = 300mA, 001 = 50mA, 010 = 100mA, 011 = 150mA, 100 = 200mA, 101 = 250mA, 110 =500mA, 111 = 600mA */
      if (current_ma < 300)
      {
         data = (current_ma / 50) ? (current_ma / 50) : 0x1;
      }
      else if (current_ma < 500)
      {
         data = 0x00;
      }
      else
      {
         data = (current_ma < 600) ? 0x6 : 0x7;
      }

      pm_register_address_type cfg_tcc = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_tcc;
      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_tcc, 0x7, data, 0);
   }

   return err_flag;
}


/*This API reads the charge termination current_ma*/
pm_err_flag_type pm_smbchg_chgr_get_charge_termination_current(uint32 device_index, uint32 *current_ma)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == current_ma)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type cfg_tcc = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->cfg_tcc;
      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg_tcc, 0x7, &data, 0);
      if (data == 0)
      {
         *current_ma = 300;
      }
      else if (data < 5)
      {
         *current_ma = 50 * data;
      }
      else
      {
         *current_ma = 100 * (data - 1);
      }
   }

   return err_flag;
}


/*This API configures charge temperature compensation type*/
pm_err_flag_type pm_smbchg_chgr_config_chgr_temp_cmpn(uint32 device_index, pm_smbchg_chgr_ccmpn_type ccmpn_type, boolean enable)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_CHGR_CCMPN__INVALID == ccmpn_type)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type ccmp_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->ccmp_cfg;
      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, ccmp_cfg, (0x1 << ccmpn_type), (pm_register_data_type)(enable << ccmpn_type), 0);
   }

   return err_flag;
}


/*This API reads configured charge temperature compensation type*/
pm_err_flag_type pm_smbchg_chgr_get_chgr_temp_cmpn_config(uint32 device_index, pm_smbchg_chgr_ccmpn_type ccmpn_type, boolean *enable)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_CHGR_CCMPN__INVALID == ccmpn_type)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type ccmp_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->ccmp_cfg;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, ccmp_cfg, &data, 0);
      *enable = (data & (0x1 << ccmpn_type)) ? TRUE : FALSE;
   }

   return err_flag;
}


/*This API sets charger configuration */
pm_err_flag_type pm_smbchg_chgr_config_chgr(uint32 device_index, pm_chgr_chgr_cfg_type *chgr_cfg_type)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data1, data2;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == chgr_cfg_type)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else if (chgr_cfg_type->sysok_opt >= PM_SMBCHG_CHGR_SYSOK_OPT__INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      data1 =  (chgr_cfg_type->cont_without_fg_ready) | (chgr_cfg_type->is_rchg_threshold_src_fg << 1) | (chgr_cfg_type->is_term_current_src_fg << 2)
         | (chgr_cfg_type->en_early_current_termination << 3) | (chgr_cfg_type->sysok_opt << 4) | (chgr_cfg_type->en_sysok_pol_table_n << 7);

      data2 =  (chgr_cfg_type->en_charger_inhibit) | (chgr_cfg_type->is_holdoff_tmr_350ms << 1) | (chgr_cfg_type->auto_rchg_dis << 2) | (chgr_cfg_type->current_termination_dis << 3)
         | (chgr_cfg_type->batt_ov_ends_cycle_en << 4) | (chgr_cfg_type->p2f_chg_tran_require_cmd << 5);

      pm_register_address_type chgr_cfg1 = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_cfg1;
      pm_register_address_type chgr_cfg2 = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_cfg2;

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, chgr_cfg1, data1, 0);

      err_flag |= pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, chgr_cfg2, data2, 0);
   }

   return err_flag;
}

/*This API reads charger configuration */
pm_err_flag_type pm_smbchg_chgr_get_chgr_config(uint32 device_index, pm_chgr_chgr_cfg_type *chgr_cfg_type)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data1, data2;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == chgr_cfg_type)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type chgr_cfg1 = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_cfg1;
      pm_register_address_type chgr_cfg2 = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_cfg2;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, chgr_cfg1, &data1, 0);
      err_flag |= pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, chgr_cfg2, &data2, 0);

      chgr_cfg_type->cont_without_fg_ready        = (data1 & 0x1) ? TRUE : FALSE;
      chgr_cfg_type->is_rchg_threshold_src_fg     = (data1 & 0x2) ? TRUE : FALSE;
      chgr_cfg_type->is_term_current_src_fg       = (data1 & 0x4) ? TRUE : FALSE;
      chgr_cfg_type->en_early_current_termination = (data1 & 0x8) ? TRUE : FALSE;
      chgr_cfg_type->sysok_opt                    = (pm_smbchg_chgr_sysok_opt_type)((data1 & 0x70) >> 4);
      chgr_cfg_type->en_sysok_pol_table_n         = (data1 & 0x80) ? TRUE : FALSE;;

      chgr_cfg_type->en_charger_inhibit       = (data2 & 0x1) ?  TRUE : FALSE;
      chgr_cfg_type->is_holdoff_tmr_350ms     = (data2 & 0x2) ?  TRUE : FALSE;
      chgr_cfg_type->auto_rchg_dis            = (data2 & 0x4) ?  TRUE : FALSE;
      chgr_cfg_type->current_termination_dis  = (data2 & 0x8) ?  TRUE : FALSE;
      chgr_cfg_type->batt_ov_ends_cycle_en    = (data2 & 0x10) ? TRUE : FALSE;
      chgr_cfg_type->p2f_chg_tran_require_cmd = (data2 & 0x20) ? TRUE : FALSE;
   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_chgr_config_sfty_timer(uint32 device_index, uint32 pre_chg_sfty_min, uint32 total_chg_sfty_min, pm_smbchg_chgr_sfty_timer_type sfty_timer_type)

{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data1, data2, data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (sfty_timer_type >= PM_SMBCHG_CHGR_SFTY_TIMER__INVAID_TYPE)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      if (pre_chg_sfty_min < 96)
      {
         data1 = (pre_chg_sfty_min < 48) ? 0x0 : 0x1;
      }
      else
      {
         data1 = (pre_chg_sfty_min < 192) ? 0x2 : 0x3;
      }

      if (total_chg_sfty_min < 768)
      {
         data2 = (total_chg_sfty_min < 384) ? 0x0 : 0x1;
      }
      else
      {
         data2 = (total_chg_sfty_min < 1536) ? 0x2 : 0x3;
      }

      data = (sfty_timer_type << 4) | (data2 << 2) | data1;

      pm_register_address_type sft_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->sft_cfg;

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, sft_cfg, 0x3F, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_chgr_get_sfty_timer_config(uint32 device_index, uint32 *pre_chg_sfty_min, uint32 *total_chg_sfty_min, pm_smbchg_chgr_sfty_timer_type *sfty_timer_type)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == pre_chg_sfty_min || NULL == total_chg_sfty_min || NULL == sfty_timer_type)
   {
      err_flag = PM_ERR_FLAG__INVALID_POINTER;
   }
   else
   {
      pm_register_address_type sft_cfg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->sft_cfg;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, sft_cfg, &data, 0);

      *sfty_timer_type = (pm_smbchg_chgr_sfty_timer_type)((data & 0x30) >> 0x4);

      switch (data & 0x3)
      {
      case 0:
         *pre_chg_sfty_min = 24;  break;
      case 1:
         *pre_chg_sfty_min = 48;  break;
      case 2:
         *pre_chg_sfty_min = 96;  break;
      default:
         *pre_chg_sfty_min = 192;
      }

      switch ((data & 0x6) >> 2)
      {
      case 0:
         *total_chg_sfty_min = 192;  break;
      case 1:
         *total_chg_sfty_min = 384;  break;
      case 2:
         *total_chg_sfty_min = 768;  break;
      default:
         *total_chg_sfty_min = 1536;
      }

   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_chgr_irq_enable(uint32 device_index, pm_smbchg_chgr_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      if (enable)
      {
         irq_reg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_en_set;
      }
      else
      {
         irq_reg = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_en_clr;
      }

      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, irq_reg, data, 0);
   }
   return err_flag;
}

pm_err_flag_type pm_smbchg_chgr_irq_clear(uint32  device_index, pm_smbchg_chgr_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 1 << irq;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag =  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type int_latched_clr = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_latched_clr;
      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_chgr_irq_set_trigger(uint32 device_index, pm_smbchg_chgr_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag =  PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
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

      pm_register_address_type int_set_type = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_set_type;
      pm_register_address_type int_polarity_high = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_polarity_high;
      pm_register_address_type int_polarity_low = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_polarity_low;

      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_set_type, mask, set_type, 0);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_polarity_high, mask, polarity_high, 0);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_polarity_low, mask, polarity_low, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_chgr_irq_status(uint32 device_index, pm_smbchg_chgr_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_CHGR_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else if (NULL == status)
   {
      err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
   }
   else
   {
      switch (type)
      {
      case PM_IRQ_STATUS_RT:
         int_sts = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_rt_sts;
         break;
      case PM_IRQ_STATUS_LATCHED:
         int_sts = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_latched_sts;
         break;
      case PM_IRQ_STATUS_PENDING:
         int_sts = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->int_pending_sts;
         break;
      default:
         return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
      }

      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
      *status = data ? TRUE : FALSE;
   }

   return err_flag;
}

static inline pm_err_flag_type pm_smbchg_chgr_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->sec_access;
      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}


/*This API sets charger source */
pm_err_flag_type pm_smbchg_chgr_enable_src(uint32 device_index, boolean chg_en_src)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type chgr_cfg2 = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_cfg2;

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, chgr_cfg2, 0x80, chg_en_src, 0);
   }

   return err_flag;
}

/*This API sets charger polarity */
pm_err_flag_type  pm_smbchg_chgr_set_chg_polarity_low (uint32 device_index, boolean chg_pol_low)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type chgr_cfg2 = smbchg_ptr->smbchg_register->chgr_register->base_address + smbchg_ptr->smbchg_register->chgr_register->chgr_cfg2;

      err_flag = pm_smbchg_chgr_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, chgr_cfg2, 0x40, (chg_pol_low ? 0x40 : 0), 0);
   }

   return err_flag;
}
