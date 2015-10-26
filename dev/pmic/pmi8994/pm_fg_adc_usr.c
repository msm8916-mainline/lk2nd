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

#include "pm_fg_adc_usr.h"
#include "pm_fg_driver.h"
#include "pm_smbchg_driver.h"
#include "pm_err_flags.h"
#include "pm_comm.h"
#include "pm_fg_adc_usr.h"
#include "pm_fg_driver.h"
#include <platform/timer.h>
#include <sys/types.h>

/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/
#define RAW_IBAT_LSB          39
#define RAW_VBAT_LSB          39
#define OFFSET_LSB_NUM        12   //Offset LSB Numerator
#define OFFSET_LSB_DENOM      10   //Offset LSB Denominator
#define GAIN_LSB_DENOM       400  // Gain LSB is 0.32/128 = 1/400
#define  PM_MAX_ADC_READY_DELAY     2000
#define  PM_MIN_ADC_READY_DELAY     1 * 1000  //1ms

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/
/**
* @brief This function grant access to FG ADC User access *
* @details
* This function grant access to FG ADC User access 
* 
* @param[in] pm_fg_data_type Self
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_grant_sec_access(pm_fg_data_type *fg_adc_usr_ptr)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if(NULL == fg_adc_usr_ptr)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type sec_access = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_sec_access;
    err_flag = pm_comm_write_byte(fg_adc_usr_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
  }

  return err_flag;
}

/* Interrupt */
pm_err_flag_type pm_fg_adc_usr_irq_enable(uint32 pmic_device, pm_fg_adc_usr_irq_type irq, boolean enable)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type irq_reg;
  pm_register_data_type data = 1 << irq;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_USR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
  }
  else
  {
    if (enable)
    {
      irq_reg = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_en_set;
    }
    else
    {
      irq_reg = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_en_clr;
    }

    err_flag = pm_comm_write_byte(fg_adc_usr_ptr->comm_ptr->slave_id, irq_reg, data, 0);
  }
  return err_flag;
}

pm_err_flag_type pm_fg_adc_usr_irq_clear(uint32 pmic_device, pm_fg_adc_usr_irq_type irq)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
  pm_register_data_type data = 1 << irq;
  pm_register_address_type int_latched_clr;
  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_USR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    int_latched_clr = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_latched_clr;
    err_flag = pm_comm_write_byte(fg_adc_usr_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_adc_usr_irq_set_trigger(uint32 pmic_device, pm_fg_adc_usr_irq_type irq, pm_irq_trigger_type trigger)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
  uint8 mask = 1 << irq;
  pm_register_data_type set_type, polarity_high, polarity_low;
  pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_USR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    int_set_type = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_set_type;
    int_polarity_high = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_polarity_high;
    int_polarity_low = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_polarity_low;

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
    err_flag = pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, int_set_type, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, int_polarity_high, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, int_polarity_low, mask, polarity_low, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_adc_usr_irq_status(uint32 pmic_device, pm_fg_adc_usr_irq_type irq, pm_irq_status_type type, boolean *status)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
  pm_register_data_type data;
  uint8 mask = 1 << irq;
  pm_register_address_type int_sts;
  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_USR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
  }
  else
  {
    switch (type)
    {
    case PM_IRQ_STATUS_RT:
      int_sts = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_rt_sts;
        break;
    case PM_IRQ_STATUS_LATCHED:
      int_sts = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_latched_sts;
        break;
    case PM_IRQ_STATUS_PENDING:
      int_sts = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->int_pending_sts;
        break;
    default:
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
    *status = data ? TRUE : FALSE;
  }

  return err_flag;
}



/**
* @brief This function enable/disables BMS Fule Gauge Algorithm BCL (battery current limiting s/w use) *
* @details
*  This function enable/disables BMS Fule Gauge Algorithm BCL (battery current limiting s/w use) 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_enable_bcl_monitoring(uint32 pmic_device, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bcl_monitoring = 0x00;
  uint8 mask = 0x80;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bcl_monitoring = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_en_ctl;
    err_flag = pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bcl_monitoring, mask, (pm_register_data_type)(enable << 7), 0);
  }

  return err_flag;

}


/**
* @brief This function returns BMS Fule Gauge BCL (battery current limiting s/w use) *
* @details
* This function returns BMS Fule Gauge BCL (battery current limiting s/w use)  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]enable  BCL monitoring sts
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_monitoring_sts(uint32 pmic_device, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bcl_monitoring = 0x00;
  pm_register_data_type data = 0x00;
  uint8 mask = 0x80;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bcl_monitoring = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_en_ctl;
    err_flag = pm_comm_read_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bcl_monitoring, mask, &data, 0);
    *enable = (boolean )data;
  }

  return err_flag;

}


/**
* @brief This function returns of status of adc_usrery parameter update request *
* @details
*  This function returns of status of battery parameter update request (VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP) 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_access_bat_req_sts(uint32 pmic_device, boolean *enable)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bat_req_sts = 0x00;
  pm_register_data_type data = 0x00;
  uint8 mask = 0x80;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bat_req_sts = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_access_bat_req;
    err_flag = pm_comm_read_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bat_req_sts, mask, &data, 0);
    *enable = (boolean )data;
  }

  return err_flag;

}



/**
* @brief This function enable/disables the updates of VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP *
* @details
*  This function enable/disables the updates of VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_set_access_bat_req(uint32 pmic_device, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bat_req_sts = 0x00;
  uint8 mask = 0x80;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bat_req_sts = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_access_bat_req;
    err_flag = pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bat_req_sts, mask, (pm_register_data_type)enable, 0);
  }

  return err_flag;
}


/**
* @brief This function returns the grant status of battery parameter update request *
* @details
*  This function returns of status of battery parameter update request (VBAT, IBAT, VBAT_CP, IBAT_CP, IBAT_MAX, IBAT_MAX_CP, VBAT_MIN, VBAT_MIN_CP) 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_access_bat_grnt_sts(uint32 pmic_device, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bat_grnt_sts = 0x00;
  pm_register_data_type data = 0x00;
  uint8 mask = 0x80;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bat_grnt_sts = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_access_bat_grnt;
    err_flag = pm_comm_read_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bat_grnt_sts, mask, &data, 0);
    *enable = (boolean )data;
  }

  return err_flag;

}



/**
* @brief This function returns status RDY bit after battery parameter update request is serviced *
* @details
*  After the first readings from ADC are obtained, this bit is set to 1; At reset and shutdown, this bit gets automatically cleared 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable BCL monitoring
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_values(uint32 pmic_device, boolean *enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bcl_values = 0x00;
  pm_register_data_type data = 0x00;
  uint8 mask = 0x80;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bcl_values = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_values;
    err_flag = pm_comm_read_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bcl_values, mask, &data, 0);
    *enable = (boolean )data;
  }

  return err_flag;

}


/**
* @brief This function returns battery ADC Voltage *
* @details
*  8 bit signed partial ADC value, MSB = 0 is positive voltage (positive number), only positive voltages are captured, 1 LSB = 39 mV 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]vbat_adc  Battery Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_vbat(uint32 pmic_device, uint32 *vbat_adc)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_vbat = 0x00;
  pm_register_address_type  fg_adc_usr_vbat_cp = 0x00;
  pm_register_data_type data = 0x00;
  pm_register_data_type data1 = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  //Compare data thrice in copy register and original register to make sure we have right data
  uint32  total_count = 3;
  uint32  count = 0;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  *vbat_adc = 0;

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_vbat = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_vbat;
    fg_adc_usr_vbat_cp = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_vbat_cp;

    do {
      err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_vbat, &data1, 0);
      err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_vbat_cp, &data, 0);

      if(data == data1)
      {
        break;
      }

      if((total_count - 1) == count)
      {
        data = 0;
        err_flag = PM_ERR_FLAG__INVALID;
      }

      count++;

    }while ( count < total_count);

    //Convert uint8 to int8
	temp_data = data;

	//Convert int8 to int32
    temp_data1 = temp_data;

    *vbat_adc = temp_data1 * RAW_VBAT_LSB;
  }

  return err_flag;
}


/**
* @brief This function returns battery ADC Current *
* @details
*  8 bit signed partial ADC value, MSB = 0 is discharging current (positive number), only discharging currents are captured, 1 LSB = 39 mA 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]ibat_adc  Battery Current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_ibat(uint32 pmic_device, int32 *ibat_adc)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_ibat = 0x00;
  pm_register_address_type  fg_adc_usr_ibat_cp = 0x00;
  pm_register_data_type     data = 0x00;
  pm_register_data_type     data1 = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  uint32   count = 0;
  //Compare data thrice in copy register and original register to make sure we have right data
  uint32   total_count = 3;

  pm_fg_data_type *fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == ibat_adc)
  {
    err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
  }
  else
  {
    fg_adc_usr_ibat = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_ibat;
    fg_adc_usr_ibat_cp = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_ibat_cp;

    do
    {
      /*Read original register and copy register to make sure available data is reliable*/
      err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_ibat, &data1, 0);
      err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_ibat_cp, &data, 0);

      if (data1 == data)
      {
        break;
      }
      if ((total_count - 1) == count)
      {
        err_flag = PM_ERR_FLAG__INVALID;
      }
      count++;

    }
    while (count < total_count);

    //Convert uint8 to int8
	temp_data = data;

	//Convert int8 to int32
	temp_data1 = temp_data;

    *ibat_adc = temp_data1 * RAW_IBAT_LSB;
  }

  return err_flag;
}


/**
* @brief This function returns minimum battery Voltage *
* @details
*  Running Vbat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]vbat_min  Battery Minimum Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_vbat_min(uint32 pmic_device, uint8 *vbat_min)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_vbat_min = 0x00;
  pm_register_data_type data = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_vbat_min = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_vbat_min;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_vbat_min, &data, 0);
    *vbat_min = data;
  }

  return err_flag;

}

/**
* @brief This function returns max battery current *
* @details
*  Running Ibat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]ibat_max  Battery Maximum Current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_ibat_max(uint32 pmic_device, uint8 *ibat_max)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_ibat_max = 0x00;
  pm_register_data_type data = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_ibat_max = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_ibat_max;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_ibat_max, &data, 0);
    *ibat_max = (uint8)data;
  }

  return err_flag;

}


/**
* @brief This function returns copy of minimum battery Voltage *
* @details
*  Running Vbat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]vbat_min  Battery Minimum Voltage
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_vbat_min_cp(uint32 pmic_device, uint8 *vbat_min_cp)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_vbat_min_cp = 0x00;
  pm_register_data_type data = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_vbat_min_cp = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_vbat_min_cp;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_vbat_min_cp, &data, 0);
    *vbat_min_cp = (uint8)data;
  }

  return err_flag;

}

/**
* @brief This function returns copy of max battery current *
* @details
*  Running Ibat Min stored and then cleared by SW
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]ibat_max  Battery Maximum Current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_ibat_max_cp(uint32 pmic_device, uint8 *ibat_max_cp)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_ibat_max_cp = 0x00;
  pm_register_data_type data = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_ibat_max_cp = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_ibat_max_cp;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_ibat_max_cp, &data, 0);
    *ibat_max_cp = (uint8)data;
  }

  return err_flag;

}


/**
* @brief This function returns Battery Resitance in HALF encoding *
* @details
*  HALF-FLOATING point encoding, 15:11 exp, bit 10 sign, 9:0 mantissa, 1=1 ohm, refer to MDOS for encoding info
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out]battResistance  Battery Resistance 
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_half_point_encoding(uint32 pmic_device, uint16 *battResistance)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type addr_msb;
  pm_register_address_type addr_lsb;
  pm_register_data_type    data_0 = 0x00;
  pm_register_data_type    data_1 = 0x00;
  uint16 battR = 0x0000;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    addr_lsb = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bat_res_7_0;
    addr_msb = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bat_res_15_8;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, addr_lsb, &data_0, 0);
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, addr_msb, &data_1, 0);

    battR = ((data_1 | battR) << 8) | (data_0 | battR);

    *battResistance = battR;
  }

  return err_flag;

}


/**
* @brief This function returns BCL mode status *
* @details
*  This function returns BCL mode status 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[out] bcl_mode  BCL mode
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_mode(uint32 pmic_device, pm_fg_adc_usr_bcl_mode_sts *bcl_mode)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bcl_values = 0x00;
  pm_register_data_type data = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bcl_values = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_values;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bcl_values, &data, 0);
    data = data >> 0x06;
    *bcl_mode = (pm_fg_adc_usr_bcl_mode_sts)data;
  }

  return err_flag;

}


/**
* @brief This function returns votlage gain correction value for battery voltage *
* @details
*  This function returns gain correction value for battery 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] gainCorrection  for battery volatge
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_v_gain_batt(uint32 pmic_device, int32 *v_gain_correction)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_v_gain_batt = 0x00;
  pm_register_data_type data = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_v_gain_batt = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_v_gain_batt;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_v_gain_batt, &data, 0);

    //Convert uint8 to int8
	temp_data = data;

	//Convert int8 to int32
	temp_data1 = temp_data;

    *v_gain_correction = temp_data1;
  }

  return err_flag;

}

/**
* @brief This function returns current Rsense gain correction value for Battery Current *
* @details
*  This function returns current Rsense gain correction value for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] gainCorrection  for battery volatge
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_gain_rsense(uint32 pmic_device, int32 *i_gain_rsense)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_i_gain_rsense = 0x00;
  pm_register_data_type data = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);;

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_i_gain_rsense = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_i_gain_rsense;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_i_gain_rsense, &data, 0);

    //Convert uint8 to int8
	temp_data = data;

    //Convert int8 to int32
	temp_data1 = temp_data;
	
    *i_gain_rsense = temp_data1;
  }

  return err_flag;
}



/**
* @brief This function returns current Rsense offset value for Battery Current *
* @details
*  This function returns current Rsense offset value for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] ioffset_rsense  Offset Gain Correction battery current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_offset_rsense(uint32 pmic_device, int32 *i_offset_rsense)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_i_offset_rsense = 0x00;
  pm_register_data_type data = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_i_offset_rsense = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_i_offset_rsense;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_i_offset_rsense, &data, 0);

    //Convert uint8 to int8
	temp_data = data;

	//Convert int8 to int32
    temp_data1 = temp_data;

    *i_offset_rsense = (temp_data1 * OFFSET_LSB_NUM) / OFFSET_LSB_DENOM;
  }

  return err_flag;

}


/**
* @brief This function returns current gain in BATFET for Battery Current *
* @details
*  This function returns current gain in BATFET for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] igain_offset_correction  Gain Correction for battery current
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_gain_batfet(uint32 pmic_device, int32 *i_gain_batfet)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_i_gain_batfet = 0x00;
  pm_register_data_type data = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_i_gain_batfet = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_i_gain_batfet;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_i_gain_batfet, &data, 0);

    //Convert uint8 to int8
	temp_data = data;

    //Convert int8 to int32
	temp_data1 = temp_data;

    *i_gain_batfet = temp_data1;
  }

  return err_flag;

}


/**
* @brief This function returns current offset in BATFET for Battery Current *
* @details
*  This function returns current gain in BATFET for Battery Current  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] ioffset_batfet_correction  offset Gain Correction for BATFET
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_offset_batfet(uint32 pmic_device, int32 *i_offset_batfet)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_i_offset_batfet = 0x00;
  pm_register_data_type data = 0x00;

  int8   temp_data = 0;
  int32  temp_data1 = 0;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);;

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_i_offset_batfet = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_i_offset_batfet;
    err_flag = pm_comm_read_byte(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_i_offset_batfet, &data, 0);

	//Convert uint8 to int8
	temp_data = data;

	//Convert int8 to int32
    temp_data1 = temp_data;

    *i_offset_batfet = ((temp_data1 * OFFSET_LSB_NUM) / OFFSET_LSB_DENOM);
  }

  return err_flag;
}



/**
* @brief This function returns source used for current sense *
* @details
*  This function returns source used for current sense  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in] isense_source  source used for current sense
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_get_bcl_i_sense_source(uint32 pmic_device, boolean *i_sense_source)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_bcl_isense_source = 0x00;
  pm_register_data_type data = 0x00;
  uint8 mask = 0x01;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_bcl_isense_source = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_bcl_i_sense_source;
    err_flag = pm_comm_read_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_bcl_isense_source, mask, &data, 0);
    *i_sense_source = (boolean )data;
  }

  return err_flag;
}


/**
* @brief This function cleras stored VBAT minimum  *
* @details
* This function cleras stored VBAT minimum  
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable for clearing stored vbat minimum
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_clear_vbat_min(uint32 pmic_device, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_clr_vbat_min = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_clr_vbat_min = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_vbat_min_clr;
    err_flag = pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_clr_vbat_min, 0x80, (pm_register_data_type)enable, 0);
  }

  return err_flag;

}

/**
* @brief This function clears stored IBAT Max *
* @details
* This function cleras stored IBAT Max 
* 
* @param[in] pmic_device_index. Primary: 0 Secondary: 1
* @param[in]enable  enable/disable for clearing stored vbat minimum
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG__SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_usr_clear_ibat_max(uint32 pmic_device, boolean enable)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_register_address_type  fg_adc_usr_clr_ibat_min = 0x00;

  pm_fg_data_type* fg_adc_usr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_usr_ptr)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    fg_adc_usr_clr_ibat_min = fg_adc_usr_ptr->fg_register->adc_usr_register->base_address + fg_adc_usr_ptr->fg_register->adc_usr_register->fg_adc_usr_ibat_max_clr;
    err_flag = pm_comm_write_byte_mask(fg_adc_usr_ptr->comm_ptr->slave_id, fg_adc_usr_clr_ibat_min, 0x80, (pm_register_data_type)enable, 0);
  }

  return err_flag;

}


/**
PmicFgCalibrateIbat()

@brief

*/
pm_err_flag_type pm_fg_adc_usr_get_calibrated_ibat(uint32 pmic_device, int32 *calibrated_ibat)
{
  pm_err_flag_type   errFlag = PM_ERR_FLAG__SUCCESS;

  int32 raw_ibat = 0;
  static int32 gain = 0;
  static int32 offset = 0;

  boolean sense_source = FALSE;
  
  *calibrated_ibat = 0;

  errFlag |= pm_fg_adc_usr_get_ibat(pmic_device, &raw_ibat);

  if((!gain) || (!offset))
  {//gain and offset are constant per part, so we should read them only once
    errFlag |= pm_fg_adc_usr_get_bcl_i_sense_source( pmic_device, &sense_source);
    
    if(sense_source)
    {// 1 -> pick rsense correction
      errFlag |= pm_fg_adc_usr_get_bcl_i_gain_rsense( pmic_device, &gain );
      errFlag |= pm_fg_adc_usr_get_bcl_i_offset_rsense( pmic_device, &offset );
    }
    else
    {// 0 -> pick batfet correction
      errFlag |= pm_fg_adc_usr_get_bcl_i_gain_batfet( pmic_device,  &gain );
      errFlag |= pm_fg_adc_usr_get_bcl_i_offset_batfet( pmic_device, &offset );
    }
  }
      
  *calibrated_ibat = (((raw_ibat + offset) * (GAIN_LSB_DENOM + gain ))/ GAIN_LSB_DENOM);

  return errFlag;
}

pm_err_flag_type pm_fg_adc_usr_get_calibrated_vbat(uint32 pmic_device, uint32 *calibrated_vbat)
{
  pm_err_flag_type   errFlag = PM_ERR_FLAG__SUCCESS;

  uint32 raw_vbat = 0;
  static int32 gain = 0;
  
  errFlag = pm_fg_adc_usr_get_vbat(pmic_device, &raw_vbat);
  
  if(!gain)
  {//Gain is constant w.r.t to every part, so we should read it only once.
    errFlag |= pm_fg_adc_usr_get_bcl_v_gain_batt( pmic_device,  &gain );
  }

  /* Applying gain calibration to the raw value*/
  // Twos_complement(VBAT_registerval) *39 * (1+ Twos_Complement(V_GAIN_registerval) * (.32/128))
  *calibrated_vbat = (uint32)((((raw_vbat + 2) * (GAIN_LSB_DENOM + gain)))/GAIN_LSB_DENOM);

  return errFlag;
}

pm_err_flag_type pm_fg_usr_get_vbat(uint32 pmic_device, uint32 *calibrated_vbat)
{
	uint16	wait_index = 0;
	boolean adc_reading_ready = FALSE;
	boolean enable = FALSE;

	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

	pm_fg_driver_init(pmic_device);

	err_flag |= pm_fg_adc_usr_get_bcl_monitoring_sts(pmic_device, &enable);
	if (err_flag != PM_ERR_FLAG__SUCCESS)  {
		return err_flag;
	} else {
		if (enable == FALSE) {
			err_flag |= pm_fg_adc_usr_enable_bcl_monitoring(pmic_device, TRUE);
			if (err_flag != PM_ERR_FLAG__SUCCESS)
				return err_flag;
		}
	}

	//Check Vbatt ADC level
	err_flag |= pm_fg_adc_usr_get_bcl_values(pmic_device, &adc_reading_ready); //Check if Vbatt ADC is ready

	//Check if Vbatt ADC is Ready
	for (wait_index = 0; wait_index < PM_MAX_ADC_READY_DELAY; wait_index++) {
		if(adc_reading_ready == FALSE) {
		udelay(PM_MIN_ADC_READY_DELAY);
		err_flag |= pm_fg_adc_usr_get_bcl_values(pmic_device, &adc_reading_ready);
		} else {
			break;
		}
	}

	if ( err_flag != PM_ERR_FLAG__SUCCESS )  {
		return err_flag;
	}

	if (adc_reading_ready) {
		err_flag |= pm_fg_adc_usr_get_calibrated_vbat(pmic_device, calibrated_vbat); //Read calibrated vbatt ADC
	}

	return err_flag;
}
