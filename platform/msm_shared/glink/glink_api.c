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
#include <glink.h>
#include <glink_rpm.h>
#include <glink_os_utils.h>
#include <glink_internal.h>
#include <glink_vector.h>

#ifdef FEATURE_TRACER_PACKET
#include "glink_tracer.h"
#endif

#define GLINK_NOT_INITIALIZED 0
#define GLINK_INITIALIZED     1

/*===========================================================================
                        GLOBAL DATA DECLARATIONS
===========================================================================*/
/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/** Default implementation of optional callbacks */
static void glink_default_notify_rx_sigs
(
  glink_handle_type  handle,
  const void         *priv,
  uint32             prev,
  uint32             curr
)
{
  GLINK_OS_UNREFERENCED_PARAM( handle );
  GLINK_OS_UNREFERENCED_PARAM( priv );
  GLINK_OS_UNREFERENCED_PARAM( prev );
  GLINK_OS_UNREFERENCED_PARAM( curr );
  return;
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Opens a logical GLink based on the specified config params
 *
 * @param[in]    cfg_ptr  Pointer to the configuration structure for the
 *                        GLink. See glink.h
 * @param[out]   handle   GLink handle associated with the logical channel
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Allocates channel resources and informs remote host about
 *               channel open.
 */
glink_err_type glink_open
(
  glink_open_config_type *cfg_ptr,
  glink_handle_type      *handle
)
{
  glink_transport_if_type *if_ptr;
  glink_channel_ctx_type  *ch_ctx;
  unsigned int            remote_host;
  glink_xport_priority     suggested_priority;
  glink_channel_ctx_type  *allocated_ch_ctx;
  glink_err_type           status;

  /* Param validation */
  if(cfg_ptr == NULL            ||
     cfg_ptr->remote_ss == NULL ||
     cfg_ptr->name == NULL                  ||
     cfg_ptr->notify_state == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN, "", "", "", 
        GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Evaluate the equivalent edge name->enum for future use */
  remote_host = glinki_find_remote_host(cfg_ptr->remote_ss);

  if (remote_host == GLINK_NUM_HOSTS)
  {
    /* Unknown transport name trying to register with GLink */
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, 
                           cfg_ptr->name, "", 
                           cfg_ptr->remote_ss, 
                           GLINK_STATUS_INVALID_PARAM );    

    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Allocate and initialize channel info structure */
  ch_ctx = glink_os_calloc(sizeof(glink_channel_ctx_type));
  if(ch_ctx == NULL) 
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN, 
                           cfg_ptr->name, "", "",
                           GLINK_STATUS_OUT_OF_RESOURCES );
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  /* Fill in the channel info structure */
  glink_os_string_copy(ch_ctx->name, cfg_ptr->name, sizeof(ch_ctx->name));
  ch_ctx->priv = cfg_ptr->priv;
  ch_ctx->notify_rx = cfg_ptr->notify_rx;
  ch_ctx->notify_rxv = cfg_ptr->notify_rxv;
  ch_ctx->notify_tx_done = cfg_ptr->notify_tx_done;
  ch_ctx->notify_state = cfg_ptr->notify_state;
  ch_ctx->notify_rx_intent_req = cfg_ptr->notify_rx_intent_req;
  ch_ctx->notify_rx_intent = cfg_ptr->notify_rx_intent;
  ch_ctx->notify_rx_sigs = cfg_ptr->notify_rx_sigs;
  ch_ctx->ch_open_options = cfg_ptr->options;
  ch_ctx->notify_rx_abort = cfg_ptr->notify_rx_abort;
  ch_ctx->notify_tx_abort = cfg_ptr->notify_tx_abort;

  if (ch_ctx->notify_rx_sigs == NULL)
  {
    /* set default callback */
    ch_ctx->notify_rx_sigs = glink_default_notify_rx_sigs;
  }

  if_ptr = glinki_find_requested_xport( cfg_ptr->transport, 
                                       cfg_ptr->remote_ss,
                                        cfg_ptr->options,
                                        &suggested_priority );

  if( !if_ptr )
    {
    /* Code gets here if we are not able to find reqeusted transport */
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN,
                     cfg_ptr->name,
                     cfg_ptr->transport,
                     cfg_ptr->remote_ss,
                     GLINK_STATUS_NO_TRANSPORT );
    glink_os_free(ch_ctx);
    return GLINK_STATUS_NO_TRANSPORT;
      }
  
      status = glinki_add_ch_to_xport( if_ptr,
                                       ch_ctx,
                                       &allocated_ch_ctx,
                                       TRUE,
                                   suggested_priority );

      if( status == GLINK_STATUS_SUCCESS )
      {
        /* Set the handle and return */
        *handle = allocated_ch_ctx;
    GLINK_LOG_EVENT( *handle,
                     GLINK_EVENT_CH_OPEN,
                     cfg_ptr->name,
                     cfg_ptr->transport,
                     cfg_ptr->remote_ss,
                     status );
      }
      else
      {
        *handle = NULL;
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_OPEN,
                   cfg_ptr->name,
                   cfg_ptr->transport,
                   cfg_ptr->remote_ss,
                   status );
  }

  

      return status;
    }

/**
 * Closes the GLink logical channel specified by the handle.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Closes local end of the channel and informs remote host
 */
glink_err_type glink_close
(
  glink_handle_type handle
)
{
  glink_err_type status;
  glink_core_xport_ctx_type *xport_ctx; 

  if(handle == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_CLOSE, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* get xport context after NULL check */
  xport_ctx = handle->if_ptr->glink_core_priv;  
  
  ASSERT( xport_ctx != NULL );
  
  /* grab lock to change/check channel state atomically */
  glink_os_cs_acquire( &xport_ctx->channel_q_cs );

  /* Check to see if closed called again for same channel */
  if (handle->local_state != GLINK_LOCAL_CH_OPENED &&
      handle->local_state != GLINK_LOCAL_CH_OPENING)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_CLOSE, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           handle->local_state );

    glink_os_cs_release(&xport_ctx->channel_q_cs);

    return GLINK_STATUS_CH_ALREADY_CLOSED;
  }

  handle->local_state = GLINK_LOCAL_CH_CLOSING;

  if (handle->remote_state == GLINK_REMOTE_CH_SSR_RESET ||
      glinki_xport_linkup(handle->if_ptr) == FALSE)
  {
    /* SSR happened on remote-SS OR XPORT link is down. Fake close_ack from here */
    glink_os_cs_release( &xport_ctx->channel_q_cs);

    handle->if_ptr->glink_core_if_ptr->rx_cmd_ch_close_ack( handle->if_ptr,
                                                            handle->lcid );

    status = GLINK_STATUS_SUCCESS;
  }
  else
  {
    glink_os_cs_release(&xport_ctx->channel_q_cs);

    status = handle->if_ptr->tx_cmd_ch_close(handle->if_ptr, handle->lcid);

    GLINK_LOG_EVENT( handle,
                     GLINK_EVENT_CH_CLOSE, 
                     handle->name, 
                     xport_ctx->xport,
                     xport_ctx->remote_ss, 
                     status );
  }

  return status;
}

/**
 * Transmit the provided buffer over GLink.
 *
 * @param[in]    handle    GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv  Per packet private data
 *
 * @param[in]   *data      Pointer to the data buffer to be transmitted
 *
 * @param[in]   size       Size of buffer
 *
 * @param[in]   req_intent Whether to block and request for remote rx intent in
 *                         case it is not available for this pkt tx
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
glink_err_type glink_tx
(
  glink_handle_type handle,
  const void        *pkt_priv,
  const void        *data,
  size_t            size,
  uint32            options
)
{
  return glink_txv(handle, pkt_priv, (void*)data, size,
                   &glink_dummy_tx_vprovider, NULL, options);

}

/**
 * Transmit the provided vector buffer over GLink.
 *
 * @param[in]    handle    GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv  Per packet private data
 *
 * @param[in]   *iovec     Pointer to the vector buffer to be transmitted
 *
 * @param[in]   size       Size of buffer
 *
 * @param[in]   vprovider  Buffer provider for virtual space
 *
 * @param[in]   pprovider  Buffer provider for physical space
 *
 * @param[in]   req_intent Whether to block and request for remote rx intent in
 *                         case it is not available for this pkt tx
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
glink_err_type glink_txv
(
  glink_handle_type        handle,
  const void               *pkt_priv,
  void                     *iovec,
  size_t                   size,
  glink_buffer_provider_fn vprovider,
  glink_buffer_provider_fn pprovider,
  uint32                   options
)
{
  glink_err_type         status;
  glink_core_tx_pkt_type *pctx;
  boolean                req_intent = options & GLINK_TX_REQ_INTENT;

  glink_core_xport_ctx_type *xport_ctx;

  /* Input validation */
  if(handle == NULL || iovec == NULL || size == 0 ||
     (vprovider == NULL && pprovider == NULL))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, "", "", "",
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  xport_ctx = handle->if_ptr->glink_core_priv;

  if (!glinki_channel_fully_opened(handle))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, 
                     handle->name,
                     xport_ctx->xport,
                     xport_ctx->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  pctx = glink_os_calloc( sizeof( glink_core_tx_pkt_type ) );

  if (pctx == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_TX, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           GLINK_STATUS_OUT_OF_RESOURCES );
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  /* Protect the entire tx operation under a lock as a client may call
     tx in different thread context */
  glink_os_cs_acquire(&handle->tx_cs);

  pctx->pkt_priv = pkt_priv;
  pctx->size = size;
  pctx->size_remaining = size;
  pctx->vprovider = vprovider;
  pctx->pprovider = pprovider;

  if (vprovider == &glink_dummy_tx_vprovider)
  {
    pctx->data = (void*)iovec;
    pctx->iovec = pctx;
  }
  else
  {
    pctx->data = (void*)iovec;
    pctx->iovec = iovec;
  }

#ifdef FEATURE_TRACER_PACKET
  pctx->tracer_pkt = options & GLINK_TX_TRACER_PKT ? TRUE : FALSE;
  if( pctx->tracer_pkt )
  {
    glink_tracer_packet_log_pctx_pkt( pctx, GLINK_CORE_TX );
  }
#endif

  status = xport_ctx->channel_submit_pkt(handle, pctx, req_intent);

  /*Update the channel stats*/
  GLINK_UPDATE_CHANNEL_STATS( handle->ch_stats, tx_request, size);
  
  GLINK_LOG_EVENT( handle,
                   GLINK_EVENT_CH_TX, 
                   handle->name, 
                   xport_ctx->xport,
                   xport_ctx->remote_ss, 
                   status);

  glink_os_cs_release(&handle->tx_cs);
  return status;
}

/**
 * Queue one or more Rx intent for the logical GPIC Link channel.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv Per packet private data
 *
 * @param[in]   size      Size of buffer
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  GLink XAL allocates rx buffers for receiving packets
 */
glink_err_type glink_queue_rx_intent
(
  glink_handle_type handle,
  const void        *pkt_priv,
  size_t            size
)
{
  glink_err_type         status;
  glink_rx_intent_type   *lc_intent;
  glink_core_xport_ctx_type *xport_ctx; 
  size_t tmp;

  /* Input validation */
  if(handle == NULL || size == 0)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  xport_ctx = handle->if_ptr->glink_core_priv;
  
  /* short circuit for intentless mode */
  if(xport_ctx->xport_capabilities & GLINK_CAPABILITY_INTENTLESS)
  {
    return GLINK_STATUS_API_NOT_SUPPORTED;
  }

  if (!glinki_channel_fully_opened(handle))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                     handle->name,
                     xport_ctx->xport,
                     xport_ctx->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  /* Allocate an intent structure */
  lc_intent = glink_os_calloc(sizeof(glink_rx_intent_type));
  if(lc_intent == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           GLINK_STATUS_OUT_OF_RESOURCES );
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  /* Call transport API to allocate rx intent buffer */
  status = handle->if_ptr->allocate_rx_intent(handle->if_ptr, size, lc_intent);
  if(status != GLINK_STATUS_SUCCESS)
  {
    glink_os_free(lc_intent);
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                           handle->name, 
                           xport_ctx->xport,
                           xport_ctx->remote_ss, 
                           status );
    return status;
  }

  if (handle->notify_rxv == NULL &&
     (lc_intent->vprovider(lc_intent->iovec, 0, &tmp) == NULL || tmp < size))
  {
    /* Allocate bounce buffer for non-vectored Rx */
    lc_intent->data = glink_os_malloc(size);

    if(lc_intent->data == NULL)
    {
      handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
      glink_os_free(lc_intent);
      GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_Q_RX_INTENT, 
                             handle->name, 
                             xport_ctx->xport, 
                             xport_ctx->remote_ss, 
                             GLINK_STATUS_OUT_OF_RESOURCES );
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
  }

  glink_os_cs_acquire(&handle->if_ptr->glink_core_priv->liid_cs);
  
  /* push the intent on local queue. Do this before calling tx cmd
     as transport may try to read data into the newly queued rx_buffer */
  lc_intent->iid = handle->if_ptr->glink_core_priv->liid;
  lc_intent->size = size;
  lc_intent->pkt_priv = pkt_priv;
  glinki_enqueue_item(&handle->pintents->local_intent_q,
                      lc_intent,
                      &handle->pintents->intent_q_cs);

  /* Call transport API to queue rx intent */
  /* Increment the local intent ID counter associated with this channel */
  handle->if_ptr->glink_core_priv->liid++;

  status = handle->if_ptr->tx_cmd_local_rx_intent(handle->if_ptr,
              handle->lcid, size, lc_intent->iid);
  if(status != GLINK_STATUS_SUCCESS)
  {
    /* Failure */
    glinki_dequeue_item(&handle->pintents->local_intent_q,
                        lc_intent,
                        &handle->pintents->intent_q_cs);

    handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
    glink_os_free(lc_intent->data);
    glink_os_free(lc_intent);
  }
  glink_os_cs_release(&handle->if_ptr->glink_core_priv->liid_cs);

  GLINK_LOG_EVENT( handle,
                   GLINK_EVENT_CH_Q_RX_INTENT, 
                   handle->name, 
                   xport_ctx->xport,
                   xport_ctx->remote_ss, 
                   status );
  return status;
}

/**
 * Client uses this to signal to GLink layer that it is done with the received
 * data buffer. This API should be called to free up the receive buffer, which,
 * in zero-copy mode is actually remote-side's transmit buffer.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @param[in]   *ptr      Pointer to the received buffer
 *
 * @param[in]   reuse    Reuse intent
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  GLink XAL frees the Rx buffer
 */
glink_err_type glink_rx_done
(
  glink_handle_type handle,
  const void        *ptr,
  boolean           reuse
)
{
  glink_rx_intent_type      *lc_intent;
  glink_core_xport_ctx_type *xport_ctx = handle->if_ptr->glink_core_priv;

  /* Input validation */
  if(handle == NULL || ptr == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_RX_DONE, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* short circuit for intentless mode */
  if (xport_ctx->xport_capabilities & GLINK_CAPABILITY_INTENTLESS)
  {
    return GLINK_STATUS_API_NOT_SUPPORTED;
  }

  if (!glinki_channel_fully_opened(handle))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_RX_DONE, 
                     handle->name,
                     xport_ctx->xport,
                     xport_ctx->remote_ss,
			   GLINK_STATUS_CH_NOT_FULLY_OPENED );
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  /* Free the intent */
  lc_intent = smem_list_first(&handle->pintents->local_intent_client_q);
  while(lc_intent != NULL)
  {
    size_t tmp;

    if(lc_intent->iovec == ptr || (handle->notify_rxv == NULL &&
       (lc_intent->data == ptr ||
       ptr == lc_intent->vprovider(lc_intent->iovec, 0, &tmp))))
    {

      uint32 iid;

      /* Found intent, delete it */
      glinki_dequeue_item(&handle->pintents->local_intent_client_q,
                          lc_intent,
                          &handle->pintents->intent_q_cs);

      iid = lc_intent->iid;

      GLINK_UPDATE_CHANNEL_STATS( handle->ch_stats, rx_done, lc_intent->pkt_sz);

      if (reuse)
      {
        lc_intent->used = 0;

        glinki_enqueue_item(&handle->pintents->local_intent_q,
                            lc_intent,
                            &handle->pintents->intent_q_cs);
        
      }
      else
      {
        /* Free the intent */
        handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
        if(lc_intent->data)
        {
          /* Free the bounce buffer if we had allocated one */
          glink_os_free(lc_intent->data);
        }
        glink_os_free(lc_intent);
      }

      /* Note that the actual buffer, lc_intent->data, was allocated by the
      * transport and should be freed by the xport. We should not touch it */
      /* Let the xport know we are done with the buffer */
      handle->if_ptr->tx_cmd_local_rx_done(handle->if_ptr,
                                           handle->lcid,
                                           iid,
                                           reuse);

      GLINK_LOG_EVENT( handle,
                       GLINK_EVENT_CH_RX_DONE, 
                       handle->name, 
                       xport_ctx->xport,
                       xport_ctx->remote_ss, 
                       GLINK_STATUS_SUCCESS );

      return GLINK_STATUS_SUCCESS;
    }
    lc_intent = smem_list_next(lc_intent);
  }

  GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_RX_DONE, 
                         handle->name, 
                         xport_ctx->xport,
                         xport_ctx->remote_ss, 
                         GLINK_STATUS_INVALID_PARAM );
  return GLINK_STATUS_INVALID_PARAM;
}

/**
 * Set the 32 bit control signal field. Depending on the transport, it may
 * take appropriate actions on the set bit-mask, or transmit the entire
 * 32-bit value to the remote host.
 *
 * @param[in]   handle     GLink handle associated with the logical channel
 *
 * @param[in]   sig_value  32 bit signal word
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_sigs_set
(
  glink_handle_type handle,
  uint32            sig_value
)
{
  glink_core_xport_ctx_type *xport_ctx; 
  glink_err_type status;

  /* Input validation */
  if(handle == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_SET, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

	xport_ctx = handle->if_ptr->glink_core_priv;

	ASSERT( xport_ctx != NULL );

  if (!glinki_channel_fully_opened(handle))
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_SET, 
                     handle->name,
                     xport_ctx->xport,
                     xport_ctx->remote_ss,
                           GLINK_STATUS_CH_NOT_FULLY_OPENED );

    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  status = handle->if_ptr->tx_cmd_set_sigs(handle->if_ptr,
                                           handle->lcid,
                                           sig_value);
  if(GLINK_STATUS_SUCCESS == status)
  {
    /* Update local copy of local control signal state */
    handle->local_sigs = sig_value;
  }

  return status;
}

/**
 * Get the local 32 bit control signal bit-field.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_sigs_local_get
(
  glink_handle_type handle,
  uint32            *sig_value
)
{
  /* Input validation */
  if(handle == NULL || sig_value == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_L_GET, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  *sig_value = handle->local_sigs;

  return GLINK_STATUS_SUCCESS;
}

/**
 * Get the remote 32 bit control signal bit-field.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @param[out]  *sig_value  Pointer to a 32 bit signal word to get sig value
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_sigs_remote_get
(
  glink_handle_type handle,
  uint32            *sig_value
)
{
  /* Input validation */
  if(handle == NULL || sig_value == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_CH_SIG_R_GET, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  *sig_value = handle->remote_sigs;

  return GLINK_STATUS_SUCCESS;
}

/** 
 * Regsiters a client specified callback to be invoked when the specified
 * transport (link) is up/down.
 *
 * @param[in]    link_id  Pointer to the configuration structure for the
 *                        xport(link) to be monitored. See glink.h
 * @param[in]    priv     Callback data returned to client when callback
 *                        is invoked.
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Puts the callback in a queue which gets scanned when a 
 *               transport(link) comes up OR an SSR happnes.
 */
glink_err_type glink_register_link_state_cb
(
  glink_link_id_type *link_id,
  void*              priv
)
{
  glink_link_notif_data_type* link_notif_data;
  unsigned int remote_host;

  /* Input validation */
  if (link_id == NULL ||
      link_id->version != GLINK_LINK_ID_VER ||
      link_id->link_notifier == NULL)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Check for remote_ss validity */
  if (link_id->remote_ss != NULL)
  {
    remote_host = glinki_find_remote_host(link_id->remote_ss);
    if (remote_host == GLINK_NUM_HOSTS)
    {
      return GLINK_STATUS_INVALID_PARAM;
    }
  }
  
  /* Save the callback on the notification list */
  if((link_notif_data = glink_os_calloc(sizeof(glink_link_notif_data_type)))
       == NULL)
  {
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  link_notif_data->xport = link_id->xport;
  link_notif_data->remote_ss = link_id->remote_ss;
  link_notif_data->link_notifier = link_id->link_notifier;
  link_notif_data->priv = priv; /* private client data */

  /* Append the request to the list for link UP/DOWN notifications */
  glinki_register_link_notif_data(link_notif_data);
  link_id->handle = (glink_link_handle_type)link_notif_data;
  
  /* Scan the list of available transport to see if this link is already up */
  glinki_scan_xports_and_notify(link_notif_data);

  return GLINK_STATUS_SUCCESS;
}

/** 
 * Degsiter the link UP/DOWN notification callback associated with the
 * provided handle.
 *
 * @param[in]    handle  Callback handler returned by 
 *                       glink_register_link_state_cb
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Removes the callback in a queue which gets scanned when a 
 *               transport(link) comes up OR an SSR happnes.
 */
glink_err_type glink_deregister_link_state_cb
(
  glink_link_handle_type handle
)
{
  /* check if glink handle is NULL and return appropriate
     return code  */
  if(handle == NULL) 
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  glinki_deregister_link_notif_data((glink_link_notif_data_type *)handle);
  
  glink_os_free(handle);

  return GLINK_STATUS_SUCCESS;
}

/*================= RESTRICTED API ==========================*/

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
)
{
  if (!handle)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  if(handle->if_ptr->poll) {
    return handle->if_ptr->poll(handle->if_ptr);
  }
  return GLINK_STATUS_API_NOT_SUPPORTED;
}

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
)
{
  if (!handle)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  if(handle->if_ptr->mask_rx_irq) {
    return handle->if_ptr->mask_rx_irq(handle->if_ptr, mask);
  }
  return GLINK_STATUS_API_NOT_SUPPORTED;
}

/**
 * This API waits until link is down.
 *
 * @param[in]   handle      GLink handle associated with the logical channel
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  None
 */
glink_err_type glink_wait_link_down
(
  glink_handle_type handle
)
{
  ASSERT(handle && handle->if_ptr && handle->if_ptr->wait_link_down);

  return handle->if_ptr->wait_link_down(handle->if_ptr) ?
         GLINK_STATUS_SUCCESS : GLINK_STATUS_FAILURE;
}
