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

#ifndef GLINK_RPM_H
#define GLINK_RPM_H

/**
 * @file glink_rpm.h
 *
 * Public API for the GLink-RPM
 */

/** \defgroup glink GLink
 * \ingroup SMD
 *
 * GLink reliable, in-order, datagram-based interprocessor communication
 * over a set of supported transport (Shared Memory, UART, BAM, HSIC)
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include "glink_os_utils.h"

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

/*================= RESTRICTED API ==========================*/
/** Note that these API are only supported for RPM client. No other
 * client may use these as a generic API as the underlying implementation
 * works only for RPM ports */


/**
 * This API allows the RPM client to poll the transport for any new data
 * that might have come in. It would *usually* be used when incoming interrupts
 * are disabled.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_rpm_rx_poll
(
  glink_handle_type handle
);

/**
 * This API allows the RPM client to mask/unmask rx interrupts
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[in]   mask        Whether to mask or un-mask the incoming interrupt
 *
 * @param[in]   platform_struct Any platform specific into that transport may
                                require
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_rpm_mask_rx_interrupt
(
  glink_handle_type handle,
  boolean           mask,
  void              *platform_struct
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_RPM_H

