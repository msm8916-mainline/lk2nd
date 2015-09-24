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
#include "glink.h"
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "glink_vector.h"
#include "glink_os_utils.h"

/*===========================================================================
                       LOCAL DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                       PRIVATE FUNCTIONS
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
)
{
  glink_core_tx_pkt_type* pkt = (glink_core_tx_pkt_type*)iovec;

  if (pkt == NULL || size == NULL || pkt->size <= offset)
  {
    return NULL;
  }

  *size = pkt->size - offset;

  return (char*)(pkt->data) + offset;
}

/*===========================================================================
FUNCTION      glink_iovec_vprovider
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
)
{
  glink_iovector_type* iovec_l = (glink_iovector_type*)iovec;

  if (iovec_l == NULL || size == NULL)
  {
    return NULL;
  }

  if (!iovec_l->vlast || iovec_l->vlast->start_offset > offset)
  {
       iovec_l->vlast = iovec_l->vlist;
  }

  while (iovec_l->vlast &&
         iovec_l->vlast->start_offset + iovec_l->vlast->size <= offset)
  {
    iovec_l->vlast = iovec_l->vlast->next;
  }

  if (iovec_l->vlast == NULL)
  {
    return NULL;
  }

  offset -= iovec_l->vlast->start_offset;
  *size = iovec_l->vlast->size - offset;

  return (char*)(iovec_l->vlast->data) + offset;
}

/*===========================================================================
FUNCTION      glink_iovec_pprovider
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
)
{
  glink_iovector_type* iovec_l = (glink_iovector_type*)iovec;

  if (iovec_l == NULL || size == NULL)
  {
    return NULL;
  }

  if (!iovec_l->plast || iovec_l->plast->start_offset > offset)
  {
       iovec_l->plast = iovec_l->plist;
  }

  while (iovec_l->plast &&
         iovec_l->plast->start_offset + iovec_l->plast->size <= offset)
  {
    iovec_l->plast = iovec_l->plast->next;
  }

  if (iovec_l->plast == NULL)
  {
    return NULL;
  }

  offset -= iovec_l->plast->start_offset;
  *size = iovec_l->plast->size - offset;

  return (char*)(iovec_l->plast->data) + offset;
}
