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
#include "pm_smbchg_misc.h"
#include "pm_smbchg_driver.h"
#include "pm_resources_and_types.h"

/*This API detects the type of chgarging port device is connected to*/
pm_err_flag_type pm_smbchg_misc_chgr_port_detected(uint32 pmic_device, pm_smbchg_misc_src_detect_type *src_detected)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);
   uint8 src_loc = 0x0;

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == src_detected)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type idev_sts = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->idev_sts;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, idev_sts, &data, 0);
      if (err_flag == PM_ERR_FLAG__SUCCESS)
      {
         while (src_loc < 0x08) //loop thru each bit
         {
            if (data & (0x80 >> src_loc))
            {
               break;
            }
            src_loc++;
         }

         *src_detected = (src_loc > 7) ? PM_SMBCHG_MISC_SRC_DETECT_INVALID : (pm_smbchg_misc_src_detect_type)src_loc;

      }
   }
   return err_flag;
}

#ifndef LK
/*This API restarts watchdog*/
pm_err_flag_type pm_smbchg_misc_wdog_rst(uint32 pmic_device)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type wdog_rst = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->wdog_rst;
      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, wdog_rst, 0x80, 0x80, 0);
   }

   return err_flag;
}

/*This API enables automatic fault protection */
pm_err_flag_type pm_smbchg_misc_en_afp(uint32 pmic_device)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);
   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type afp_mode = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->afp_mode;
      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, afp_mode, 0x80, 0x80, 0);
   }

   return err_flag;
}


/*This API configures the charger watchgod */
pm_err_flag_type pm_smbchg_misc_config_wdog(uint32 pmic_device, pm_smbchg_misc_wdog_cfg_type *wd_cfg)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == wd_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else if (wd_cfg->sfty_timer >= PM_SMBCHG_MISC_SFTY_TIMER_AFTER_WDOG_INVALID || wd_cfg->wdog_timeout >= PM_SMBCHG_MISC_WD_TMOUT_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type wd_cfg_reg = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->wd_cfg;
      data = (wd_cfg->afp_wdog_en << 0x7) | (wd_cfg->wdog_timeout << 0x5) | (wd_cfg->sfty_timer << 3) | (wd_cfg->wdog_irq_sfty_en << 2) | (wd_cfg->wdog_option_run_always << 1) | (wd_cfg->wdog_timer_en);
      err_flag = pm_smbchg_misc_unlock_perph_write(smbchg_ptr);
      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, wd_cfg_reg, data, 0);
   }

   return err_flag;
}


/*This API reads the configurations for charger watchgod */
pm_err_flag_type pm_smbchg_misc_get_wdog_config(uint32 pmic_device, pm_smbchg_misc_wdog_cfg_type *wd_cfg)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == wd_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type wd_cfg_reg = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->wd_cfg;
      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, wd_cfg_reg, &data, 0);
   }

   if (err_flag == PM_ERR_FLAG__SUCCESS)
   {
      wd_cfg->wdog_timer_en          = (data & 0x1) ? TRUE : FALSE;
      wd_cfg->wdog_option_run_always = (data & 0x2) ? TRUE : FALSE;
      wd_cfg->wdog_irq_sfty_en       = (data & 0x4) ? TRUE : FALSE;
      wd_cfg->sfty_timer             = (pm_smbchg_misc_sfty_after_wdog_irq_type)((data & 0x18) >> 3);
      wd_cfg->wdog_timeout           = (pm_smbchg_wdog_timeout_type)((data & 0x60) >> 5);
      wd_cfg->afp_wdog_en            = (data & 0x80) ? TRUE : FALSE;
   }

   return err_flag;
}



pm_err_flag_type pm_smbchg_misc_enable_wdog(uint32 pmic_device, boolean enable_smbchg_wdog)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_misc_wdog_cfg_type wd_cfg;

   err_flag = pm_smbchg_misc_get_wdog_config(pmic_device, &wd_cfg);

   //Change needed bits and write it back
   if (err_flag == PM_ERR_FLAG__SUCCESS)
   {
      wd_cfg.afp_wdog_en  = enable_smbchg_wdog;

      err_flag |= pm_smbchg_misc_config_wdog(pmic_device, &wd_cfg);
   }

   return err_flag;
}


pm_err_flag_type pm_smbchg_misc_set_wdog_timeout(uint32 pmic_device, pm_smbchg_wdog_timeout_type wdog_timeout)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_smbchg_misc_wdog_cfg_type wd_cfg;


   err_flag = pm_smbchg_misc_get_wdog_config(pmic_device, &wd_cfg);

   //change needed bits and write it back
   if (err_flag == PM_ERR_FLAG__SUCCESS)
   {
     wd_cfg.wdog_timeout = wdog_timeout;
     err_flag |= pm_smbchg_misc_config_wdog(pmic_device, &wd_cfg);
   }

   return err_flag;
}


/*This API configures VCHG output which is proportional to the input current or the charger/discharge current. This also configures SYSOK LDO function and PHY_ON */
pm_err_flag_type pm_smbchg_misc_config_misc_fn(uint32 pmic_device, pm_smbchg_misc_misc_fn_cfg_type misc_cfg, boolean enable)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;

   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_MISC_FN__INVALID == misc_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type misc_cfg_reg = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->misc_cfg;

      err_flag = pm_comm_write_byte_mask(smbchg_ptr->comm_ptr->slave_id, misc_cfg_reg, (0x1 << misc_cfg), (pm_register_data_type)(enable<<misc_cfg), 0);
   }

   return err_flag;
}

/*This API reads VCHG output configuration, SYSOK LDO function and PHY_ON */
pm_err_flag_type pm_smbchg_misc_get_misc_fn_config(uint32 pmic_device, pm_smbchg_misc_misc_fn_cfg_type misc_cfg, boolean *enable)
{
   pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 0;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (PM_SMBCHG_MISC_FN__INVALID == misc_cfg)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else if (NULL == enable) 
   {
      err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type misc_cfg_reg = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->misc_cfg;

      err_flag = pm_comm_read_byte(smbchg_ptr->comm_ptr->slave_id, misc_cfg_reg, &data, 0);

      *enable = (data & (0x1 << misc_cfg)) ? TRUE : FALSE;
   }

   return err_flag;
}

pm_err_flag_type pm_smbchg_misc_irq_enable(uint32 pmic_device, pm_smbchg_misc_irq_type irq, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type irq_reg;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      if (enable)
      {
         irq_reg = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_en_set;
      }
      else
      {
         irq_reg = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_en_clr;
      }

      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, irq_reg, data, 0);
   }
   return err_flag;
}

pm_err_flag_type pm_smbchg_misc_irq_clear(uint32  pmic_device, pm_smbchg_misc_irq_type irq)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data = 1 << irq;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      pm_register_address_type int_latched_clr = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_latched_clr;

      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
   }
   return err_flag;
}


pm_err_flag_type pm_smbchg_misc_irq_set_trigger(uint32 pmic_device, pm_smbchg_misc_irq_type irq, pm_irq_trigger_type trigger)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   uint8 mask = 1 << irq;
   pm_register_data_type set_type, polarity_high, polarity_low;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (irq >= PM_SMBCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type int_set_type = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_set_type;
      pm_register_address_type int_polarity_high = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_polarity_high;
      pm_register_address_type int_polarity_low = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_polarity_low;

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

pm_err_flag_type pm_smbchg_misc_irq_status(uint32 pmic_device, pm_smbchg_misc_irq_type irq, pm_irq_status_type type, boolean *status)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   uint8 mask = 1 << irq;
   pm_register_address_type int_sts;
   pm_smbchg_data_type *smbchg_ptr  = pm_smbchg_get_data(pmic_device);

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == status)
   {
      err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
   }
   else if (irq >= PM_SMBCHG_MISC_IRQ_INVALID)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      switch (type)
      {
      case PM_IRQ_STATUS_RT:
         int_sts = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_rt_sts;
         break;
      case PM_IRQ_STATUS_LATCHED:
         int_sts = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_latched_sts;
         break;
      case PM_IRQ_STATUS_PENDING:
         int_sts = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->int_pending_sts;
         break;
      default:
         return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
      }

      err_flag = pm_comm_read_byte_mask(smbchg_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
      *status = data ? TRUE : FALSE;
   }

   return err_flag;
}


static inline pm_err_flag_type pm_smbchg_misc_unlock_perph_write(pm_smbchg_data_type *smbchg_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

   if (NULL == smbchg_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type sec_access = smbchg_ptr->smbchg_register->misc_register->base_address + smbchg_ptr->smbchg_register->misc_register->sec_access;
      err_flag = pm_comm_write_byte(smbchg_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}
#endif