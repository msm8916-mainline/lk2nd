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

#include "pm_fg_driver.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the FG driver data */

pm_fg_data_type        fg_data[PM_MAX_NUM_PMICS];
fg_adc_usr_register_ds fg_adc_usr_reg[1] =
	{   /* FG base address and offsets */
		{0x4200, 0x0 ,0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,\
		0x18, 0x19, 0x1A, 0x1B, 0x46, 0x50, 0x51, 0x53, \
		0xD0, 0x54, 0x55, 0x56,	0x57, 0x58, 0x59, 0x5A, 0x5B, \
		0x5C, 0x5D, 0x5E, 0x60,	0x61, 0x62, 0x63, 0x64, 0x65, \
		0x66, 0x67, 0x68, 0x69},
	};
/* Fill in slave id, pmic index and comm type (comm type is unused) */
pm_comm_info_type      comm[1] = {{2, 1, 0},};
/* We use only adc user register others are not needed */
fg_register_ds         fg_reg[1] = { {NULL, NULL, NULL, &fg_adc_usr_reg[0]}, };

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
/* Initialize FG driver and data structure */
void pm_fg_driver_init(uint32_t device_index)
{
	fg_data[device_index].comm_ptr = &comm[0];
	fg_data[device_index].fg_register = &fg_reg[0];
	fg_data[device_index].num_of_peripherals = 0x0;
}

pm_fg_data_type* pm_fg_get_data(uint32 pmic_index)
{
  if(pmic_index <PM_MAX_NUM_PMICS) 
  {
      return &fg_data[pmic_index];
  }

  return NULL;
}
