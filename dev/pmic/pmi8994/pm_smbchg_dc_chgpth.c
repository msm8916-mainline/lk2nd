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

#include "pm_smbchg_dc_chgpth.h"
#include "pm_smbchg_driver.h"
#include <sys/types.h>

/*Unlock peripheral for secured write access*/
static inline pm_err_flag_type pm_smbchg_dc_chgpth_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr);

/*Find the nearest register value corresponding to input_data*/
static void pm_smbchg_dc_chgpth_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value);

/*This API configures ADCIN adapter allowance*/
pm_err_flag_type pm_smbchg_dc_chgpth_set_dcin_adptr_allowance(uint32 device_index, pm_smbchg_dcin_adptr_allowance adptr_allowance)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type dcin_chgr_cfg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dcin_chgr_cfg;

      errFlag = pm_smbchg_dc_chgpth_unlock_perph_write(smbchg_ptr);
      errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, dcin_chgr_cfg, 0x7, (pm_register_data_type)adptr_allowance, 0);
   }
   return errFlag;
}

/*This API reads the adapter allowance configuration*/
pm_err_flag_type pm_smbchg_dc_chgpth_get_dcin_adptr_allowance(uint32 device_index, pm_smbchg_dcin_adptr_allowance *adptr_allowance)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == adptr_allowance)
   {
      errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type dcin_chgr_cfg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dcin_chgr_cfg;

      errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, dcin_chgr_cfg, 0x7, &data, 0);

      if (PM_ERR_FLAG__SUCCESS == errFlag)
      {
         *adptr_allowance = (pm_smbchg_dcin_adptr_allowance)data;
      }
   }

   return errFlag;
}

/*This API configures the DCIN input current limit*/
pm_err_flag_type pm_smbchg_dc_chgpth_set_dcin_current_limit(uint32 device_index, uint32 current_ma)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;
   
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      uint32 *current_limits = smbchg_ptr->chg_range_data->dcin_current_limits;

      pm_register_address_type dcin_il_cfg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dcin_il_cfg;

      pm_smbchg_dc_chgpth_return_reg_value(current_ma, current_limits, DCIN_SIZE, &data);

      errFlag = pm_smbchg_dc_chgpth_unlock_perph_write(smbchg_ptr);
      errFlag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, dcin_il_cfg, 0x1F, data, 0);
   }

   return errFlag;
}

/*This API reads the DCIN input current limit*/
pm_err_flag_type pm_smbchg_dc_chgpth_get_dcin_current_limit(uint32 device_index, uint32 *current_milli_amp)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr )
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == current_milli_amp)
   {
      errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      uint32 *current_limits = smbchg_ptr->chg_range_data->dcin_current_limits;

      pm_register_address_type dcin_il_cfg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dcin_il_cfg;

      errFlag |= pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, dcin_il_cfg, 0x1F, &data, 0);
      if ((data & 0x1F) >= DCIN_SIZE)
      {
         errFlag |= PM_ERR_FLAG__INVALID;
      }
      else
      {
         *current_milli_amp = current_limits[data & 0x1F];
      }
   }

   return errFlag;
}

/*This API configures various AICL parameters*/
pm_err_flag_type pm_smbchg_dc_chgpth_config_aicl(uint32 device_index, pm_smbchg_dc_aicl_cfg_type dc_aicl_cfg, boolean enable)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type dc_aicl_cfg_reg;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_AICL__INVALID_CFG == dc_aicl_cfg)
   {
      errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_data_type mask = (0x1 << (dc_aicl_cfg & 0x7));

      if (dc_aicl_cfg > PM_SMBCHG_AICL__OV_OPTION)
      {
         dc_aicl_cfg_reg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dc_aicl_cfg2;
      }
      else
      {
         dc_aicl_cfg_reg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dc_aicl_cfg1;
      }

      pm_register_data_type data = (enable)? 0xFF : 0x00;
      errFlag = pm_smbchg_dc_chgpth_unlock_perph_write(smbchg_ptr);
      errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, dc_aicl_cfg_reg, mask, data, 0);
   }

   return errFlag;
}

/*This API reads the various AICL configurations*/
pm_err_flag_type pm_smbchg_dc_chgpth_get_aicl_config(uint32 device_index, pm_smbchg_dc_aicl_cfg_type dc_aicl_cfg, boolean *enable)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type dc_aicl_cfg_reg;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_AICL__INVALID_CFG == dc_aicl_cfg)
   {
      errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else if (NULL == enable)
   {
      errFlag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
   }
   else
   {
      pm_register_data_type mask = (0x1 << (dc_aicl_cfg & 0x7));

      if (dc_aicl_cfg > PM_SMBCHG_AICL__OV_OPTION)
      {
         dc_aicl_cfg_reg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dc_aicl_cfg2;
      }
      else
      {
         dc_aicl_cfg_reg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->dc_aicl_cfg1;
      }

      errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, dc_aicl_cfg_reg, mask, &data, 0);
      *enable = (data) ? TRUE : FALSE;
   }

   return errFlag;
}

/* This API configures AICL restart timer */
pm_err_flag_type pm_smbchg_dc_chgpth_set_aicl_restart_tmr(uint32 device_index, pm_smbchg_dc_chgpth_aicl_rstrt_tmr timer_type)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (timer_type >= PM_SMBCHG_AICL_WL_RSTRT_TMR_INVALID)
   {
      errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type aicl_wl_sel_cfg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->aicl_wl_sel_cfg;

      errFlag = pm_smbchg_dc_chgpth_unlock_perph_write(smbchg_ptr);
      errFlag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, aicl_wl_sel_cfg, 0x3, (pm_smbchg_dc_chgpth_aicl_rstrt_tmr)timer_type, 0);
   }

   return errFlag;
}


/* This API reads configured AICL restart timer */
pm_err_flag_type pm_smbchg_dc_chgpth_get_aicl_restart_tmr(uint32 device_index, pm_smbchg_dc_chgpth_aicl_rstrt_tmr *timer_type)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == timer_type)
   {
      errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type aicl_wl_sel_cfg  = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->aicl_wl_sel_cfg;

      errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, aicl_wl_sel_cfg, 0x3, &data, 0);

      *timer_type = (pm_smbchg_dc_chgpth_aicl_rstrt_tmr)data;
   }

   return errFlag;
}


pm_err_flag_type pm_smbchg_dc_chgpth_irq_enable(uint32 device_index, pm_smbchg_dc_chgpth_irq_type irq, boolean enable)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_DC_CHGPTH_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      if (enable)
      {
         irq_reg = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_en_set;
      }
      else
      {
         irq_reg = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_en_clr;
      }

      errFlag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, irq_reg, data, 0);
   }
   return errFlag;
}

pm_err_flag_type pm_smbchg_dc_chgpth_irq_clear(uint32  device_index, pm_smbchg_dc_chgpth_irq_type irq)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_DC_CHGPTH_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type int_latched_clr = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_latched_clr;

      errFlag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
   }
   return errFlag;
}


pm_err_flag_type pm_smbchg_dc_chgpth_irq_set_trigger(uint32 device_index, pm_smbchg_dc_chgpth_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_DC_CHGPTH_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type int_set_type = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_set_type;
      pm_register_address_type int_polarity_high = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_polarity_high;
      pm_register_address_type int_polarity_low = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_polarity_low;

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

pm_err_flag_type pm_smbchg_dc_chgpth_irq_status(uint32 device_index, pm_smbchg_dc_chgpth_irq_type irq, pm_irq_status_type type, boolean *status)
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
   else if (irq >= PM_SMBCHG_DC_CHGPTH_IRQ_INVALID)
   {
      errFlag =  PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      switch (type)
      {
      case PM_IRQ_STATUS_RT:
         int_sts = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_rt_sts;
         break;
      case PM_IRQ_STATUS_LATCHED:
         int_sts = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_latched_sts;
         break;
      case PM_IRQ_STATUS_PENDING:
         int_sts = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->int_pending_sts;
         break;
      default:
         return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
      }

      errFlag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
      *status = data ? TRUE : FALSE;
   }

   return errFlag;
}

/*@todo: following API may require implementation. Need more info */
//pm_smbchg_dc_chgpth_temp_comp_cfg
//pm_smbchg_dc_chgpth_tr_dcin_sns
//pm_smbchg_dc_chgpth_tr_dcin_divider
//pm_smbchg_dc_chgpth_tr_iinos_dc


static inline pm_err_flag_type pm_smbchg_dc_chgpth_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr)
{
   pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

   if (NULL == smbchg_ptr)
   {
      errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = smbchg_ptr->smbchg_register->dc_chgpth_register->base_address + smbchg_ptr->smbchg_register->dc_chgpth_register->sec_access;
      errFlag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return errFlag;
}


/*Find the nearest register value corresponding to input_data*/
static void pm_smbchg_dc_chgpth_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value)
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
