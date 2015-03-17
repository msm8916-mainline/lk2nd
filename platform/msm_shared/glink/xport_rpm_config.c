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
#include "xport_rpm_config.h"
#include "smem_type.h"
#include "glink_os_utils.h"
#include <platform/iomap.h>

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

#define XPORT_RPM_MSG_RAM_BASE        RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS
#define XPORT_RPM_MSG_RAM_SIZE        RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE

/** Size of Glink RPM transport ToC in MSG RAM */
#define XPORT_RPM_MSG_TOC_SIZE        0x100

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
const xport_rpm_config_type xport_rpm_config[] =
{
  /* LK->Rpm */
  {
    "rpm",                /* Remote host name        */
    XPORT_RPM_AP2R_ID,    /* Tx FIFO ID in RPM ToC   */
    XPORT_RPM_R2AP_ID,    /* Rx FIFO ID in RPM ToC   */
    {                     /* Outgoing interrupt      */
      8, //DALIPCINT_PROC_RPM,
      0, //DALIPCINT_GP_0
    },
    200                   /* Incoming interrupt      */
  }
};

const uint32 xport_rpm_config_num = ARRAY_LENGTH(xport_rpm_config);
const char* xport_rpm_msg_ram = (char*)XPORT_RPM_MSG_RAM_BASE;
const uint32* xport_rpm_msg_ram_toc = (uint32*)(XPORT_RPM_MSG_RAM_BASE + XPORT_RPM_MSG_RAM_SIZE - XPORT_RPM_MSG_TOC_SIZE);

/*===========================================================================
FUNCTION      xport_rpm_get_config
===========================================================================*/
/**

  Provides a pointer to transport config strucutre.

  @param[in]  ind    Index of the config

  @return     Pointer to transport config strucutre.

  @sideeffects  None.
*/
/*=========================================================================*/
const xport_rpm_config_type* xport_rpm_get_config(uint32 ind)
{
  if (ind >= xport_rpm_config_num)
  {
    return NULL;
  }

  return &xport_rpm_config[ind];
}
