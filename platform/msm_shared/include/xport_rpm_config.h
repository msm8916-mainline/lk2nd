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

#ifndef XPORT_RPM_CONFIG_H
#define XPORT_RPM_CONFIG_H

/**
 * @file xport_rpm_config.h
 *
 * Internal definitions for RPM Glink transport configurations
 */

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "glink_os_utils.h"
#include "smem_type.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
#define XPORT_RPM_TOC_MAGIC 0x67727430

#define XPORT_RPM_AP2R_ID   0x61703272
#define XPORT_RPM_R2AP_ID   0x72326170
#define XPORT_RPM_MP2R_ID   0x6D703272
#define XPORT_RPM_R2MP_ID   0x72326D70
#define XPORT_RPM_AD2R_ID   0x61643272
#define XPORT_RPM_R2AD_ID   0x72326164
#define XPORT_RPM_SC2R_ID   0x73633272
#define XPORT_RPM_R2SC_ID   0x72327363
#define XPORT_RPM_WC2R_ID   0x77633272
#define XPORT_RPM_R2WC_ID   0x72327763
#define XPORT_RPM_TZ2R_ID   0x747A3272
#define XPORT_RPM_R2TZ_ID   0x7232747A

#define XPORT_RPM_TOC_MAGIC_IDX    0
#define XPORT_RPM_TOC_LENGTH_IDX   1
#define XPORT_RPM_TOC_ENTRIES_IDX  2

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef struct _xport_rpm_toc_entry_type
{
  uint32 fifo_id;                   /* FIFO ID                */
  uint32 fifo_offset;               /* FIFO offset in MSG RAM */
  uint32 fifo_size;                 /* FIFO size              */
} xport_rpm_toc_entry_type;

typedef struct _xport_rpm_toc_config_type
{
  uint32 fifo_id;                    /* FIFO ID                        */
  smem_host_type host;               /* Host ID for MSG RAM allocation */
  uint32 fifo_size;                  /* FIFO size                      */
} xport_rpm_toc_config_type;


typedef struct _xport_rpm_config_type
{
  const char* remote_ss;            /* Remote host name        */
  uint32 tx_fifo_id;                /* Tx FIFO ID in RPM ToC   */
  uint32 rx_fifo_id;                /* Rx FIFO ID in RPM ToC   */
  os_ipc_intr_type irq_out;         /* Outgoing interrupt      */
  uint32 irq_in;                    /* Incoming interrupt      */
} xport_rpm_config_type;

#endif //XPORT_RPM_CONFIG_H

