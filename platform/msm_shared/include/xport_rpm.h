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

#ifndef XPORT_RPM_H
#define XPORT_RPM_H

/**
 * @file xport_rpm.h
 *
 * Internal definitions for RPM Glink transport
 */

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_transport_if.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/* Version request command */
#define XPORT_RPM_CMD_VERSION_REQ       0x0
/* Version ACK command */
#define XPORT_RPM_CMD_VERSION_ACK       0x1
/* Open channel command */
#define XPORT_RPM_CMD_OPEN_CHANNEL      0x2
/* Close channel command */
#define XPORT_RPM_CMD_CLOSE_CHANNEL     0x3
/* Open channel ACK command */
#define XPORT_RPM_CMD_OPEN_CHANNEL_ACK  0x4
/* Tx data command */
#define XPORT_RPM_CMD_TX_DATA           0x9
/* Close channel ACK command */
#define XPORT_RPM_CMD_CLOSE_CHANNEL_ACK 0xB
/* Tx signals command */
#define XPORT_RPM_CMD_TX_SIGNALS        0xF


#define XPORT_RPM_GET_CMD_ID(c)      ((c) & 0xFFFF)
#define XPORT_RPM_SET_CMD_ID(cmd_id) ((cmd_id) & 0xFFFF)

#define XPORT_RPM_GET_CHANNEL_ID(c)   (((c) >> 16) & 0xFFFF)
#define XPORT_RPM_SET_CHANNEL_ID(cid) (((cid) << 16) & 0xFFFF0000)

#define XPORT_RPM_GET_VERSION(c) (((c) >> 16) & 0xFFFF)
#define XPORT_RPM_SET_VERSION(v) (((v) << 16) & 0xFFFF0000)


/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

glink_err_type xport_rpm_init(void *arg);
#endif //XPORT_RPM_H

