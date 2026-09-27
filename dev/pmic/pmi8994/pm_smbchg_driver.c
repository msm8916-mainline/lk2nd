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
#include "pm_app_smbchg.h"
#include "pm_smbchg_driver.h"
#include "pm_comm.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the SMBCHG driver data */
pm_smbchg_data_type smb_data[PM_MAX_NUM_PMICS];

/* Initialize bat i/f registers */
smbchg_bat_if_register_ds bat_if_reg[1] =
	{   /* Battery interface register base and offsets */
		{
		0x1200, 0x5,  0x8,  0x10, 0x11, 0x12, 0x13,
		0x14,    0x15, 0x16, 0x18, 0x19, 0x1a, 0x1b,
		0x40, 0x41, 0x42, 0xD0, 0xDF, 0xE0, 0xE1, 0xF0,
		0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8, 0xF9,
		0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
		},
	};

/* Initialize usb charger path registers */
smbchg_usb_chgpth_register_ds usb_chgpath_reg[1] =
	{   /* Charger path register base & offset */
		{
		0x1300, 0x5,  0x7, 0x8, 0x9, 0xA, 0xB,
		0xC,    0xD,  0xE, 0xF, 0x10, 0x11, 0x12,
		0x13,   0x14, 0x15, 0x16, 0x18, 0x19,
		0x1A,   0x1B, 0x40, 0x44, 0x4E, 0xD0, 0xF1,
		0xF2,   0xF3, 0xF4, 0xF5, 0xFF
		},
	};

/* Initialize dc charger path registers */
smbchg_dc_chgpth_register_ds dc_chgpath_reg[1] =
	{   /* DC charge path base & offset */
		{
		0x1400, 0x5, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
		0x18, 0x19, 0x1A, 0x1B, 0xD0, 0xDF,	0xE0, 0xE1, 0xE2,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xFA
		},
	};

/* Initialize misc charger registers */
smbchg_misc_register_ds misc_reg[1] =
	{   /* Misc charge register base & offsets */
		{
		0x1600, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x8,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
		0x18, 0x19, 0x1A, 0x1B, 0x40, 0x41, 0x42,
		0xD0, 0xDF, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5,
		0xF6, 0xF7,	0xF8, 0xFF
		},
	};

smbchg_chgr_register_ds smb_chgr_reg[1] =
	{
		{
		0x1000, 0x5, 0x8, 0xB, 0xC, 0xD, 0xE, 0x10, 0x11,
		0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A,
		0x1B, 0xD0, 0xDF, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5,
		0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD,
		0xFE, 0xFF,
		},
	};

pm_comm_info_type   comm_ptr[1] =
	{ /* slave_id , pmic index, and comm type (comm type is unused) */
	{0x2, 0x1, 0x0},
	};

/* Initialize the SMB charger data structure */
smbchg_register_ds smb_charge[1] =
	{
		{
			NULL,
			&smb_chgr_reg[0],
			NULL,
			&bat_if_reg[0],
			&usb_chgpath_reg[0],
			&dc_chgpath_reg[0],
			&misc_reg[0],
		},
	};

/* Initialize the SMB data */
void pm_smbchg_driver_init(uint32_t device_index)
{

	smb_data[device_index].smbchg_register = &smb_charge[0];
	smb_data[device_index].comm_ptr        = &comm_ptr[0];
	smb_data[device_index].chg_range_data  = (chg_range_data_type *) pm_target_chg_range_data();
}

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

pm_smbchg_data_type* pm_smbchg_get_data(uint32 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return &smb_data[pmic_index];
  }

  return NULL;
}
