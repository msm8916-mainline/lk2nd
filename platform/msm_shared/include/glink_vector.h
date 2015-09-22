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

#ifndef GLINK_VECTOR_H
#define GLINK_VECTOR_H

/*===========================================================================

                    GLink Vector Header File

===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        FEATURE DEFINITIONS
===========================================================================*/

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
typedef struct _glink_iovector_element_type {
  struct _glink_iovector_element_type *next;
  void* data;
  size_t start_offset;
  size_t size;
} glink_iovector_element_type;

typedef struct _glink_iovector_type {
  glink_iovector_element_type *vlist; /* virtual buffers list */
  glink_iovector_element_type *vlast; /* last accessed element in vlist */
  glink_iovector_element_type *plist; /* physical buffers list */
  glink_iovector_element_type *plast; /* last accessed element in plist */
} glink_iovector_type;

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_dummy_tx_vprovider
===========================================================================*/
/**

  Buffer provider for virtual space that operates on a non-vectored buffer.

  @param[in]  iovec   Pointer to the dummy vector.
  @param[in]  offset  Offset within the dummy vector.
  @param[out] size    Size of the provied buffer.

  @return     virtual address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
void* glink_dummy_tx_vprovider
(
  void*  iovec,
  size_t offset,
  size_t *size
);

/*===========================================================================
FUNCTION      glink_dummy_rx_vprovider
===========================================================================*/
/**

  Buffer provider for virtual space that operates on a Glink iovec.

  @param[in]  iovec   Pointer to the dummy vector.
  @param[in]  offset  Offset within the dummy vector.
  @param[out] size    Size of the provied buffer.

  @return     virtual address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
void* glink_iovec_vprovider
(
  void*  iovec,
  size_t offset,
  size_t *size
);

/*===========================================================================
FUNCTION      glink_dummy_rx_pprovider
===========================================================================*/
/**

  Buffer provider for physical space that operates on a Glink iovec.

  @param[in]  iovec   Pointer to the dummy vector.
  @param[in]  offset  Offset within the dummy vector.
  @param[out] size    Size of the provied buffer.

  @return     physical address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
void* glink_iovec_pprovider
(
  void*  iovec,
  size_t offset,
  size_t *size
);

#endif /* GLINK_VECTOR_H */
