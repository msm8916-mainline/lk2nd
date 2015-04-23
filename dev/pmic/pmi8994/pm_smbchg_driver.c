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
#include "CoreVerify.h"
#include "pm_utils.h"
#include "hw_module_type.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the SMBCHG driver data */
static pm_smbchg_data_type *pm_smbchg_data_arr[PM_MAX_NUM_PMICS];
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_smbchg_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_smbchg_data_type *smbchg_ptr = NULL;

    /*return if coincell since it is not supported at the moment*/
    if(PM_HW_MODULE_CHARGER_COINCELL == peripheral_info->peripheral_subtype)
    {
      return;
    }

    smbchg_ptr = pm_smbchg_data_arr[pmic_index];

    if (NULL == smbchg_ptr)
    {
        pm_malloc( sizeof(pm_smbchg_data_type), (void**)&smbchg_ptr);   
                                                    
        /* Assign Comm ptr */
        smbchg_ptr->comm_ptr = comm_ptr;

        /* smbchg Register Info - Obtaining Data through dal config */
        smbchg_ptr->smbchg_register = (smbchg_register_ds*)pm_target_information_get_common_info(PM_PROP_SMBCHG_REG);
        CORE_VERIFY_PTR(smbchg_ptr->smbchg_register);

	    smbchg_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_SMBCHG_NUM, pmic_index);
        CORE_VERIFY(smbchg_ptr->num_of_peripherals != 0);

        smbchg_ptr->chg_range_data = (chg_range_data_type*)pm_target_information_get_specific_info(PM_PROP_SMBCHG_DATA);
        CORE_VERIFY_PTR(smbchg_ptr->chg_range_data);

        pm_smbchg_data_arr[pmic_index] = smbchg_ptr;

    }
}

pm_smbchg_data_type* pm_smbchg_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_smbchg_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_smbchg_get_num_peripherals(uint8 pmic_index)
{
  if((pm_smbchg_data_arr[pmic_index] !=NULL)&& 
  	  (pmic_index < PM_MAX_NUM_PMICS))
  {
      return pm_smbchg_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}




