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


/*=============================================================================

					INCLUDE FILES

===============================================================================*/

#include "pm_smbchg_driver.h"
#include "pm_smbchg_usb_chgpth.h"

/*unlock peripheral for secured access write*/
static inline pm_err_flag_type pm_smbchg_usb_chgpth_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr);

/*Find the nearest register value corresponding to input_data*/
static void pm_smbchg_usb_chgpth_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value);

pm_err_flag_type pm_smbchg_usb_chgpth_icl_sts(uint32 device_index, input_current_limit_sts *icl_sts)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data1, data2;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      uint32 *icl_current = smbchg_ptr->chg_range_data->usbin_current_limits;

      pm_register_address_type icl_sts1 = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->icl_sts_1;
      pm_register_address_type icl_sts2 = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->icl_sts_2;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, icl_sts1, &data1, 0);
      err_flag |= pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, icl_sts2, &data2, 0);

      icl_sts->is_aicl_complete = (data1 & 0x20) ? TRUE : FALSE;
      icl_sts->input_current_limit = icl_current[data1 & 0x1F];
      icl_sts->icl_mode = (input_current_limit_mode)((data2 & 0x30) >> 4);
      icl_sts->is_usbin_suspended = (data2 & 0x8) ? TRUE : FALSE;
      icl_sts->is_dcin_suspended = (data2 & 0x4) ? TRUE : FALSE;
      icl_sts->is_usbin_active_pwr_src = (data2 & 0x2) ? TRUE : FALSE;
      icl_sts->is_dcin_active_pwr_src = (data2 & 0x1) ? TRUE : FALSE;
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_get_pwr_pth(uint32 device_index, pm_smbchg_usb_chgpth_pwr_pth_type *pwr_path)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type pwr_pth_sts = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->pwr_pth_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, pwr_pth_sts, &data, 0);

      *pwr_path = (pm_smbchg_usb_chgpth_pwr_pth_type)(data & 0x03);
   }

   return err_flag;
}


/*auto power source detection degitched status*/
pm_err_flag_type pm_smbchg_usb_chgpth_apsd_dg_sts(uint32 device_index, pm_apsd_dg_sts_type apsd_dg_sts_type, boolean* status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if ( NULL == status || PM_SMBCHG_USB_CHGPTH_DG_STS__INVALID == apsd_dg_sts_type)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type apsd_dg_sts_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->apsd_dg_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, apsd_dg_sts_reg, &data, 0);

      *status = (data & (0x1 << apsd_dg_sts_type))? TRUE : FALSE;
   }
   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_rid_sts(uint32 device_index, pm_smbchg_usb_chgpth_rid_sts_type *rid_sts)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type rid_sts_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->rid_sts;

      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, rid_sts_reg, 0xF, &data, 0);

      switch (data)
      {
         case 0x8: *rid_sts = PM_SMBCHG_USB_CHGPTH_RID__FLOAT;   break;
         case 0x4: *rid_sts = PM_SMBCHG_USB_CHGPTH_RID__A;       break;
         case 0x2: *rid_sts = PM_SMBCHG_USB_CHGPTH_RID__B;       break;
         case 0x1: *rid_sts = PM_SMBCHG_USB_CHGPTH_RID__C;       break;
         case 0x0: *rid_sts = PM_SMBCHG_USB_CHGPTH_RID__GND;     break;
         default:  *rid_sts = PM_SMBCHG_USB_CHGPTH_RID__INVALID; 
      }
   }

   return err_flag;
};



pm_err_flag_type pm_smbchg_usb_chgpth_hvdcp_sts(uint32 device_index, pm_smbchg_usb_chgpth_hvdcp_sts_type hvdcp_sts, boolean *status)
{

   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == status || PM_SMBCHG_USB_CHGPTH__HVDCP_STS_INVALID == hvdcp_sts)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type hvdcp_sts_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->hvdcp_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, hvdcp_sts_reg, &data, 0);

      *status = (data & (0x1 << hvdcp_sts))? TRUE : FALSE;

   }
   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_input_sts(uint32 device_index, pm_smbchg_usb_chgpth_chgr_type chgr, pm_smbchg_usb_chgpth_input_sts_type *input_sts_type)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type input_sts = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->input_sts;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, input_sts, &data, 0);
      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         switch (chgr)
         {
         case PM_SMBCHG_CHAR_TYPE_USB:
            data = (data & 0x38) >> 3;     break;
         case PM_SMBCHG_CHAR_TYPE_DC:
            data = (data & 0x7);           break;
         default:
            return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
         }

         switch (data)
         {
         case 0x00:
            *input_sts_type = PM_SMBCHG_NO_CHGR_DETECTED;             break;
         case 0x01:
            *input_sts_type = PM_SMBCHG_5V_9V_CHGR_DETECTED;          break;
         case 0x02:
            *input_sts_type = PM_SMBCHG_UNREGULATED_CHGR_DETECTED;    break;
         case 0x04:
            *input_sts_type = PM_SMBCHG_9V_CHGR_DETECTED;             break;
         default:
            *input_sts_type = PM_SMBCHG_INVALID_DETECTED;             break;
         }
      }

   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_get_valid_usbid(uint32 device_index, uint16 *valid_id)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data1, data2;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type usbid_valid_id_7_0  = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usbid_valid_id_7_0;
      pm_register_address_type usbid_valid_id_11_8 = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usbid_valid_id_11_8;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, usbid_valid_id_7_0, &data1, 0);
      err_flag |= pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, usbid_valid_id_11_8, 0x0F, &data2, 0);
      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         *valid_id = (uint16)((data2 << 8) | data1);
      }

   }
   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_set_cmd_il(uint32 device_index, pm_smbchg_usb_chgpth_cmd_il_type chgpth_cmd_il, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   pm_register_data_type data;

   if(NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if(PM_SMBCHG_USBCHGPTH_CMD_IL__INVALID == chgpth_cmd_il)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type cmd_il  = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->cmd_il;

      data = (enable) ? 0xFF : 0x00;

      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cmd_il, (0x1 << chgpth_cmd_il), data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_get_cmd_il(uint32 device_index, pm_smbchg_usb_chgpth_cmd_il_type chgpth_cmd_il, boolean *enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   pm_register_data_type data;
   pm_register_address_type cmd_il;

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_USBCHGPTH_CMD_IL__INVALID == chgpth_cmd_il || NULL == enable)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      cmd_il  = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->cmd_il;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, cmd_il, &data, 0);
      *enable = (data & (0x1 << chgpth_cmd_il))? TRUE : FALSE;
   }
   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_set_iusb_max(uint32 device_index, uint32 i_milli_amp)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type iusb_max  = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->iusb_max;

      if (i_milli_amp > 2500)
      {
         data = 0x19;
      }
      else if (i_milli_amp < 100)
      {
         data = 0x0;
      }
      else if (i_milli_amp > 100 && i_milli_amp <= 150)
      {
         data = 0x1;
      }
      else
      {
         /*IUSB_MAX = data * 100*/
         data = (i_milli_amp + 50) / 100; //rounding
      }

      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, iusb_max, 0x1F, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_get_iusb_max(uint32 device_index, uint32 *i_milli_amp)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == i_milli_amp)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type iusb_max  = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->iusb_max;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, iusb_max, &data, 0);

      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
         data &= 0x1F;
         switch (data)
         {
         case 0:
            *i_milli_amp = 100; break;
         case 1:
            *i_milli_amp = 150; break;
         default:
            *i_milli_amp = data * 100;
         }
      }
   }
   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_set_enum_timer_stop(uint32 device_index)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type enum_timer_stop  = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->enum_timer_stop;
      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, enum_timer_stop, 0x1, 0x1, 0);
   }
   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_set_usbin_adptr_allowance(uint32 device_index, pm_smbchg_usbin_adptr_allowance adptr_allowance)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
  
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (adptr_allowance >= PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type usbin_chgr_cfg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usbin_chgr_cfg;
      err_flag = pm_smbchg_usb_chgpth_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, usbin_chgr_cfg, 0x7, (pm_register_data_type)adptr_allowance, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_get_usbin_adptr_allowance(uint32 device_index, pm_smbchg_usbin_adptr_allowance *adptr_allowance)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == adptr_allowance)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type usbin_chgr_cfg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usbin_chgr_cfg;
      
      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, usbin_chgr_cfg, 0x7, &data, 0);
       /*valid adptr_allowance is only 0 to 5 but data read is 3 bits*/
      *adptr_allowance = (data >=PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_INVALID)? PM_SMBCHG_USBIN_ADPTR_ALLOWANCE_INVALID :(pm_smbchg_usbin_adptr_allowance)data;
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_set_usbin_current_limit(uint32 device_index, uint32 current_ma)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data =0;
   
 
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      uint32 *usbin_icl = smbchg_ptr->chg_range_data->usbin_current_limits;
      pm_register_address_type usbin_il_cfg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usbin_il_cfg;

      pm_smbchg_usb_chgpth_return_reg_value(current_ma, usbin_icl, USBIN_SIZE, &data);

      err_flag = pm_smbchg_usb_chgpth_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, usbin_il_cfg, 0x1F, data, 0);
   }

   return err_flag;
}



pm_err_flag_type pm_smbchg_usb_chgpth_get_usbin_current_limit(uint32 device_index, uint32 *current_ma)
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
      uint32 *usbin_icl = smbchg_ptr->chg_range_data->usbin_current_limits;
      pm_register_address_type usbin_il_cfg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usbin_il_cfg;
      err_flag |= pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, usbin_il_cfg, 0x1F, &data, 0);
      *current_ma = usbin_icl[data & 0x1F];
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_config_aicl(uint32 device_index, pm_smbchg_usb_chgpth_aicl_cfg_type usb_aicl_cfg, boolean enable)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_USB_CHGPTH_AICL_CFG__INVALID == usb_aicl_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type usb_aicl_cfg_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usb_aicl_cfg;
      err_flag = pm_smbchg_usb_chgpth_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, usb_aicl_cfg_reg, (0x1 << usb_aicl_cfg), (pm_register_data_type)(enable<<usb_aicl_cfg), 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_get_aicl_config(uint32 device_index, pm_smbchg_usb_chgpth_aicl_cfg_type usb_aicl_cfg, boolean* enable)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_USB_CHGPTH_AICL_CFG__INVALID == usb_aicl_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else if (NULL == enable)
   {
      err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
   } 
   else
   {
      pm_register_address_type usb_aicl_cfg_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->usb_aicl_cfg;

      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, usb_aicl_cfg_reg, (0x1<<usb_aicl_cfg), &data, 0);
      
      *enable = (data)? TRUE: FALSE;
   }

   return err_flag;
}



pm_err_flag_type pm_smbchg_usb_chgpth_config_usb_chgpth(uint32 device_index, pm_usb_chgpth_usb_cfg_type *usb_cfg_type)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   uint8 mask = 0x06;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == usb_cfg_type)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      data = ((usb_cfg_type->usb51ac_ctrl_type << 1) |(usb_cfg_type->usb51_cmd_pol_type << 2));

      pm_register_address_type cfg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->cfg;
      err_flag = pm_smbchg_usb_chgpth_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, cfg, mask, data, 0);
   }

   return err_flag;
}



pm_err_flag_type pm_smbchg_usb_chgpth_get_usb_chgpth_config(uint32 device_index, pm_usb_chgpth_usb_cfg_type *usb_cfg_type)
{
    pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);
    
    if (NULL == smbchg_ptr)
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (NULL == usb_cfg_type)
    {
       err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
       pm_register_address_type cfg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->cfg;
       err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, cfg, &data, 0);

       usb_cfg_type->usb51ac_ctrl_type   = (data & 0x1)?  PM_SMBCHG_USB_CHGPTH_USB51AC_CTRL__PIN: PM_SMBCHG_USB_CHGPTH_USB51AC_CTRL__CMD;
       usb_cfg_type->usb51_cmd_pol_type  = (data & 0x2)?  PM_SMBCHG_USB_CHGPTH_USB51_CMD_POL__CMD1_100: PM_SMBCHG_USB_CHGPTH_USB51_CMD_POL__CMD1_500;                 
    }
    
    return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_config_apsd(uint32 device_index, pm_smbchg_usb_chgpth_apsd_cfg_type *apsd_cfg)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == apsd_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
       data = (apsd_cfg->en_auto_src_detect) |(apsd_cfg->en_dcd_tmout_only << 1)|(apsd_cfg->set_rid_clk_2khz << 2)|(apsd_cfg->force_icl_500ma_vbat_low_sdp << 3)
       |(apsd_cfg->sdp_suspend << 4)|(apsd_cfg->is_ocd_isel_hc << 5)|(apsd_cfg->usb_fail_pok_hv << 6)|(apsd_cfg->is_input_prority_usbin << 7 );

      pm_register_address_type apsd_cfg_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->apsd_cfg;
      err_flag = pm_smbchg_usb_chgpth_unlock_perph_write(smbchg_ptr);
      err_flag |= pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, apsd_cfg_reg, data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_get_apsd_config(uint32 device_index, pm_smbchg_usb_chgpth_apsd_cfg_type *apsd_cfg)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == apsd_cfg )
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type apsd_cfg_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->apsd_cfg;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, apsd_cfg_reg, &data, 0);

      apsd_cfg->en_auto_src_detect           = (data & 0x1)  ? TRUE : FALSE;     
      apsd_cfg->en_dcd_tmout_only            = (data & 0x2)  ? TRUE : FALSE;            
      apsd_cfg->set_rid_clk_2khz             = (data & 0x4)  ? TRUE : FALSE;             
      apsd_cfg->force_icl_500ma_vbat_low_sdp = (data & 0x8)  ? TRUE : FALSE; 
      apsd_cfg->sdp_suspend                  = (data & 0x10) ? TRUE : FALSE;                  
      apsd_cfg->is_ocd_isel_hc               = (data & 0x20) ? TRUE : FALSE;               
      apsd_cfg->usb_fail_pok_hv              = (data & 0x40) ? TRUE : FALSE;              
      apsd_cfg->is_input_prority_usbin       = (data & 0x80) ? TRUE : FALSE; 
   }
   return err_flag;
}



pm_err_flag_type pm_smbchg_chgpth_set_input_priority(uint32 device_index, pm_smbchg_chgpth_input_priority_type chgpth_priority)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_usb_chgpth_apsd_cfg_type apsd_cfg;

   if (chgpth_priority >= PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      //Get the current configuration
      err_flag = pm_smbchg_usb_chgpth_get_apsd_config(device_index,  &apsd_cfg);
      
      if (err_flag == PM_ERR_FLAG__SUCCESS)
      {
         switch (chgpth_priority) 
         {
         case PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_DCIN:
            {
               apsd_cfg.is_input_prority_usbin = 0;
            }
            break;
         case PM_SMBCHG_USBCHGPTH_INPUT_PRIORITY_USBIN:
            {
               apsd_cfg.is_input_prority_usbin = 1;
            }
            break;
         default:
            err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
            break;
         }

         err_flag |= pm_smbchg_usb_chgpth_config_apsd(device_index, &apsd_cfg);
      }
   }
   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_irq_enable(uint32 device_index, pm_smbchg_usb_chgpth_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_USB_CHGPTH_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      if (enable)
      {
         irq_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_en_set;
      }
      else
      {
         irq_reg = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_en_clr;
      }

      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, irq_reg, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_irq_clear(uint32  device_index, pm_smbchg_usb_chgpth_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_USB_CHGPTH_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type int_latched_clr = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_latched_clr;
      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
   }
   return err_flag;
}


pm_err_flag_type pm_smbchg_usb_chgpth_irq_set_trigger(uint32 device_index, pm_smbchg_usb_chgpth_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(device_index);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_USB_CHGPTH_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type int_set_type = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_set_type;
      pm_register_address_type int_polarity_high = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_polarity_high;
      pm_register_address_type int_polarity_low = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_polarity_low;

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
      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_set_type, mask, set_type, 0);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_polarity_high, mask, polarity_high, 0);
      err_flag |= pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_polarity_low, mask, polarity_low, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_usb_chgpth_irq_status(uint32 device_index, pm_smbchg_usb_chgpth_irq_type irq, pm_irq_status_type type, boolean *status)
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
   else if (irq >= PM_SMBCHG_USB_CHGPTH_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (type)
      {
      case PM_IRQ_STATUS_RT:
         int_sts = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_rt_sts;
         break;
      case PM_IRQ_STATUS_LATCHED:
         int_sts = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_latched_sts;
         break;
      case PM_IRQ_STATUS_PENDING:
         int_sts = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->int_pending_sts;
         break;
      default:
         return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
      }

      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
      *status = data ? TRUE : FALSE;
   }
   return err_flag;
}


static inline pm_err_flag_type pm_smbchg_usb_chgpth_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = smbchg_ptr->smbchg_register->usb_chgpth_register->base_address + smbchg_ptr->smbchg_register->usb_chgpth_register->sec_access;
      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}


/*Find the nearest register value corresponding to input_data*/
static void pm_smbchg_usb_chgpth_return_reg_value(uint32 input_data, uint32 *array, uint32 array_size, pm_register_data_type *reg_value)
{
    uint32 loc = 0;
    
    /*checking lower bound*/
    input_data = (array[0]>input_data)? array[0]: input_data;

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



/*@todo: need more info to implement below 
pm_smbchg_usb_chgpth_set_wi_pwr_options(uint32 device_index)
pm_smbchg_usb_chgpth_get_wi_pwr_options(uint32 device_index)
*/
