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

#include "glink_os_utils.h"
#include "glink.h"
#include "glink_internal.h"

#define FEATURE_CH_MIGRATION_FREE

/*===========================================================================
                        GLOBAL FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
                     LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glink_process_negotiation_complete
===========================================================================*/
/**

  This is called when negotiation is complete.
  It will set the version and call link up callback to notify


  @param[in]    xport_ctx    transport context

  @param[in]    if_ptr       Pointer to interface instance; must be unique
                             for each edge

  @param[in]    version      negotiated version

  @param[in]    features     negotiated with local side

  @return        None
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static void glink_process_negotiation_complete
(
  glink_transport_if_type   *if_ptr,
  uint32                    version,
  uint32                    features
)
{
  glink_core_xport_ctx_type *xport_ctx = if_ptr->glink_core_priv;
  /* Version/Feautre can be negotiated both in ver_req and ver_ack
   * Only go through process once in case they are negotiated
   * in ver_req before receiving ver_ack */

  if (glinki_xport_linkup(if_ptr))
  {
    return;
  }

  /* setup core based on transport capabilities*/
  xport_ctx->xport_capabilities = if_ptr->set_version( if_ptr,
                                                       version,
                                                       features );
  glink_core_setup(if_ptr);

  /* transport is ready to open channels */
  glink_os_cs_acquire( &xport_ctx->status_cs );
  if_ptr->glink_core_priv->status = GLINK_XPORT_LINK_UP;
  glink_os_cs_release(&xport_ctx->status_cs);

  /* Scan the notification list to check is we have to let any registered
   * clients know that link came online */
  glinki_scan_notif_list_and_notify(if_ptr, GLINK_LINK_STATE_UP);
}

/*===========================================================================
  FUNCTION      glink_clean_channel_ctx
===========================================================================*/
/**

  This is called when channel is fully closed.
  Clean up the context and redeem channel id if possible.

  @param[in]    xport_ctx    transport context
  @param[in]    channel_ctx  channel context


  @return        None.
  @sideeffects   None.
  @dependencies  This function needs to be called in safe context
                 which is critical sections locked - channel_q_cs
*/
/*=========================================================================*/
static void glink_clean_channel_ctx
(
  glink_core_xport_ctx_type *xport_ctx,
  glink_channel_ctx_type    *channel_ctx
)
{
  /* If logging was enabled for this channel reset the logging filter */
  glinki_update_logging_filter(channel_ctx, TRUE);
  
  smem_list_delete(&xport_ctx->open_list, channel_ctx);

  if( channel_ctx->lcid == ( xport_ctx->free_lcid - 1 ) )
  {
    /* If channel being closed is the last opened channel
       re-use the lcid of this channel for any new channels */
    xport_ctx->free_lcid--;
  }

  xport_ctx->channel_cleanup(channel_ctx);
}

/*===========================================================================
  FUNCTION      glink_get_current_version
===========================================================================*/
/**

  Get current version/feature. This is necessarily highest version.

  @param[in]     xport_ctx    transport context
  
  @return        pointer to version/feature if available.
                 NULL otherwise.
  
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static const glink_core_version_type *glink_get_current_version
(
  glink_core_xport_ctx_type *xport_ctx
)
{
  const glink_core_version_type *ver;
  
  ver = &xport_ctx->version_array[xport_ctx->version_indx];
  ASSERT(ver);
  
  return ver;
}

/*===========================================================================
  FUNCTION      glink_is_local_ch_closed
===========================================================================*/
/**

  Check if local channel is fully closed
  
  @param[in]     local_state local channel state
  
  @return        TRUE, if local channel is closed
  
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static boolean glink_is_local_ch_closed
(
  glink_local_state_type local_state
)
{
  return local_state == GLINK_LOCAL_CH_INIT ||
         local_state == GLINK_LOCAL_CH_CLOSED;
}

/*===========================================================================
  FUNCTION      glink_is_remote_ch_closed
===========================================================================*/
/**

  Check if remote channel is fully closed.
  This doesn't chek GLINK_REMOTE_CH_CLEANUP state as channel ctx shouldn't be
  destroyed yet
  
  @param[in]     ch_ctx  channel ctx
  
  @return        TRUE, if remote channel is closed
  
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
static boolean glink_is_remote_ch_closed
(
    glink_remote_state_type remote_state
)
{
  return remote_state == GLINK_REMOTE_CH_INIT ||
         remote_state == GLINK_REMOTE_CH_CLOSED ||
         remote_state == GLINK_REMOTE_CH_SSR_RESET;
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glink_link_up
===========================================================================*/
/**
  Indicates that transport is now ready to start negotiation
  using the v0 configuration

  @param[in]     if_ptr    Pointer to interface instance; must be unique
                           for each edge

  @return        None
  @sideeffects   None.
  @dependencies  None.
*/
/*=========================================================================*/
void glink_link_up
(
  glink_transport_if_type *if_ptr
)
{
  glink_core_xport_ctx_type *xport_ctx;
  const glink_core_version_type *version_array;

  xport_ctx = if_ptr->glink_core_priv;

  version_array = glink_get_current_version(xport_ctx);

  /* Update the transport state */
  glink_os_cs_acquire(&if_ptr->glink_core_priv->status_cs);
  if_ptr->glink_core_priv->status = GLINK_XPORT_NEGOTIATING;
  glink_os_cs_release(&if_ptr->glink_core_priv->status_cs);

  /* Start the negtiation */
  if_ptr->tx_cmd_version(if_ptr,
                         version_array->version,
      version_array->features);

  GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_LINK_UP, "", 
                             xport_ctx->xport, 
                             xport_ctx->remote_ss, 
                             GLINK_STATUS_SUCCESS);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_version

DESCRIPTION   Receive transport version for remote-initiated version
              negotiation

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            version  Remote Version

            features Remote Features

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_version
(
  glink_transport_if_type *if_ptr,
  uint32                  version,
  uint32                  features
)
{
  const glink_core_version_type *ver;
  uint32 negotiated_features;
  glink_core_xport_ctx_type *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  ver = glink_get_current_version(xport_ctx);
  
  /* The version to use must be a subset of supported version and features
   * on this host and remote host */
  if (version == ver->version)
  {
    /* Call the transport's negotiation function */
    negotiated_features = ver->negotiate_features(if_ptr, ver, features);

    if_ptr->tx_cmd_version_ack(if_ptr, version, negotiated_features);
    
    /* If negotiated features match the provided features, version nogetiation
     * is complete */
    if(negotiated_features == features)
    {
      glink_process_negotiation_complete( if_ptr, version, features );
    }
  }
  else
  {
    /* Versions don't match, return ACK with the feature set that we support */
    if_ptr->tx_cmd_version_ack(if_ptr, ver->version, ver->features);
  }
}

/*===========================================================================
FUNCTION      glink_rx_cmd_version_ack

DESCRIPTION   Receive ACK to previous glink_transport_if::tx_cmd_version
              command

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            version  Remote Version

            features Remote Features

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_version_ack
(
  glink_transport_if_type *if_ptr,
  uint32                  version,
  uint32                  features
)
{
  const glink_core_version_type* ver;
  uint32 negotiated_features;
  glink_core_xport_ctx_type *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  /* Check to see if version returned by remote end is supported by
   * this host. Remote side would have acked only when the version/features
   * sent by this host did not match the remote */
  ver = glink_get_current_version(xport_ctx);

  if (ver->version == version)
  {
    /* Call the transport's negotiation function */
    negotiated_features = ver->negotiate_features(if_ptr, ver, features);

    if(negotiated_features == features)
    {
      glink_process_negotiation_complete( if_ptr, version, features );
    }
    else
    {
      /* Continue negotiating */
      if_ptr->tx_cmd_version(if_ptr, version, negotiated_features);
    }
  }
  else
  {
    while (ver->version > version && xport_ctx->version_indx > 0)
    {
      /* Next time use older version */
      xport_ctx->version_indx--;
      ver = &xport_ctx->version_array[xport_ctx->version_indx];
    }

    /* Versions don't match, return ACK with the feature set that we support */
    if_ptr->tx_cmd_version(if_ptr, ver->version, ver->features);
  }
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_remote_open
===========================================================================*/
/** 
 * Receive remote channel open request; 
 * Calls glink_transport_if:: tx_cmd_ch_remote_open_ack as a result
 *
 * @param[in]    if_ptr    Pointer to interface instance; must be unique
                           for each edge
 * @param[in]    rcid      Remote Channel ID
 * @param[in]    name      String name for logical channel
 * @param[in]    priority  xport priority requested by remote side
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_remote_open
(
  glink_transport_if_type *if_ptr,
  uint32                  rcid,
  const char              *name,
  glink_xport_priority    priority
)
{
  glink_channel_ctx_type     *remote_ch_ctx  = NULL;
  glink_channel_ctx_type     *allocated_ch_ctx;
  glink_err_type              status;

  GLINK_OS_UNREFERENCED_PARAM( priority );
  
  /* Allocate and initialize channel info structure */
  remote_ch_ctx = glink_os_calloc( sizeof( glink_channel_ctx_type ) );
  if(remote_ch_ctx == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_RM_CH_OPEN,
                     name,
                     if_ptr->glink_core_priv->xport,
                     if_ptr->glink_core_priv->remote_ss,
                     GLINK_STATUS_OUT_OF_RESOURCES );
    ASSERT(0);
  }

  /* Fill in the channel info structure */
  glink_os_string_copy(remote_ch_ctx->name, name, sizeof(remote_ch_ctx->name));
  remote_ch_ctx->rcid = rcid;

	status = glinki_add_ch_to_xport( if_ptr,
																	 remote_ch_ctx,
																	 &allocated_ch_ctx,
																	 FALSE,
																	 if_ptr->glink_priority );

  GLINK_LOG_EVENT( allocated_ch_ctx,
                   GLINK_EVENT_RM_CH_OPEN,
                     name,
                   if_ptr->glink_core_priv->xport,
                   if_ptr->glink_core_priv->remote_ss,
                   status );

  ASSERT(GLINK_STATUS_SUCCESS == status);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_open_ack
===========================================================================*/
/** 
 * This function is invoked by the transport
 * in response to glink_transport_if:: tx_cmd_ch_open
 *
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    lcid              Local Channel ID
 * @param[in]    migrated_ch_prio  Negotiated xport priority from remote side
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_open_ack
(
  glink_transport_if_type *if_ptr,
  uint32                   lcid,
  glink_xport_priority     migrated_ch_prio
)
{
  glink_channel_ctx_type     *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_remote_state_type    remote_state;

  GLINK_OS_UNREFERENCED_PARAM( migrated_ch_prio );
  
  xport_ctx = if_ptr->glink_core_priv;

  /* Move to closed state. Implies we clean up the channel from the
   * open list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  
  open_ch_ctx = glinki_find_ch_ctx_by_lcid(xport_ctx, lcid);
  
			if( open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSING )
			{
			/* Client called glink_close before gettng open ack. 
			 * Ignore open ack and go for closing sequence */
				glink_os_cs_release( &xport_ctx->channel_q_cs );
				return;
			}

  ASSERT(open_ch_ctx->local_state == GLINK_LOCAL_CH_OPENING);
  
			open_ch_ctx->local_state = GLINK_LOCAL_CH_OPENED;

  remote_state = open_ch_ctx->remote_state;
  glink_os_cs_release(&xport_ctx->channel_q_cs);
  
  if (remote_state == GLINK_REMOTE_CH_OPENED)
			{
			/* remote channel is open on same xport as current xport.
			 * change channel state to GLINK_CH_STATE_OPEN and notify client */
				open_ch_ctx->notify_state( open_ch_ctx,
																	 open_ch_ctx->priv,
																	 GLINK_CONNECTED );
			}

  GLINK_LOG_EVENT( open_ch_ctx, 
                   GLINK_EVENT_CH_OPEN_ACK, 
                   open_ch_ctx->name, 
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   lcid );
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_close_ack
===========================================================================*/
/** 
 * This function is invoked by the transport in response to 
 * glink_transport_if_type:: tx_cmd_ch_close
 *
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    lcid              Local Channel ID
 *
 * @return       None
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_rx_cmd_ch_close_ack
(
  glink_transport_if_type *if_ptr,
  uint32                   lcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  
  open_ch_ctx = glinki_find_ch_ctx_by_lcid(xport_ctx, lcid);
  
  GLINK_LOG_EVENT( open_ch_ctx, 
                   GLINK_EVENT_CH_CLOSE_ACK, 
                   open_ch_ctx->name,
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   lcid);

      ASSERT( open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSING );

      open_ch_ctx->local_state = GLINK_LOCAL_CH_CLOSED;
  open_ch_ctx->lcid        = GLINK_INVALID_CID;

  if ( glink_is_remote_ch_closed( open_ch_ctx->remote_state ) )
      {
        glink_clean_channel_ctx( xport_ctx, open_ch_ctx );
      }

      glink_os_cs_release(&xport_ctx->channel_q_cs);

      open_ch_ctx->notify_state( open_ch_ctx,
                                 open_ch_ctx->priv,
                                 GLINK_LOCAL_DISCONNECTED );

  if ( glink_is_remote_ch_closed( open_ch_ctx->remote_state ) )
      {
        glink_os_free( open_ch_ctx );
      }
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_remote_close
===========================================================================*/
/** 
 * Remote channel close request; will result in sending 
 * glink_transport_if_type:: tx_cmd_ch_remote_close_ack
 *
 * @param[in]    if_ptr            Pointer to interface instance; must be unique
 *                                 for each edge
 * @param[in]    rcid              Remote Channel ID
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glink_rx_cmd_ch_remote_close
(
  glink_transport_if_type *if_ptr,
  uint32                   rcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_remote_state_type    remote_state;

  xport_ctx = if_ptr->glink_core_priv;

  glink_os_cs_acquire( &xport_ctx->channel_q_cs );

  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);

  GLINK_LOG_EVENT( open_ch_ctx, 
                   GLINK_EVENT_CH_REMOTE_CLOSE, 
                   open_ch_ctx->name, 
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   rcid );

  /* It is possible that the remote subsystem sending close might crash 
     before we handle the close request from it */
  if (open_ch_ctx->remote_state == GLINK_REMOTE_CH_SSR_RESET)
  {
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return;
  }
                   
      ASSERT( open_ch_ctx->remote_state == GLINK_REMOTE_CH_OPENED );

      open_ch_ctx->remote_state = GLINK_REMOTE_CH_CLOSED;
  open_ch_ctx->rcid         = GLINK_INVALID_CID;

  if ( glink_is_local_ch_closed( open_ch_ctx->local_state ) )
      {
        /* Local side never opened the channel OR it opened it but closed it */
        /* Free channel resources */
        glink_clean_channel_ctx( xport_ctx, open_ch_ctx );
      }

      remote_state = open_ch_ctx->remote_state;

      glink_os_cs_release(&xport_ctx->channel_q_cs);

      /* Send the remote close ACK back to the other side */
  if_ptr->tx_cmd_ch_remote_close_ack(if_ptr, rcid);

  if ( glink_is_local_ch_closed( open_ch_ctx->local_state ) )
      {
    /* Destroy channel context only if there isn't any pending intents */
        if (remote_state != GLINK_REMOTE_CH_CLEANUP)
        {
        glink_os_free(open_ch_ctx);
      }
      }
      else
      {
        /* Inform the client */
    open_ch_ctx->notify_state(open_ch_ctx,
                              open_ch_ctx->priv,
                                   GLINK_REMOTE_DISCONNECTED);
      }
}

/*===========================================================================
FUNCTION      glink_rx_put_pkt_ctx
===========================================================================*/
/** 
 * Transport invokes this call to receive a packet fragment (must 
 * have previously received an rx_cmd_rx_data packet)
 *
 * @param[in]    if_ptr       Pointer to interface instance; must be unique
 *                            for each edge
 * @param[in]    rcid         Remote Channel ID
 * @param[in]    intent_ptr   Pointer to the intent fragment
 * @param[in]    complete     True if pkt is complete
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glink_rx_put_pkt_ctx
(
  glink_transport_if_type *if_ptr,
  uint32                   rcid,
  glink_rx_intent_type    *intent_ptr,
  boolean                  complete
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  GLINK_OS_UNREFERENCED_PARAM( complete );
  
  ASSERT(intent_ptr);

  xport_ctx = if_ptr->glink_core_priv;

  /* Find channel in the open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  glink_os_cs_release(&xport_ctx->channel_q_cs);
  
  GLINK_LOG_EVENT( open_ch_ctx,
                   GLINK_EVENT_CH_PUT_PKT_CTX, 
                   open_ch_ctx->name,
                   xport_ctx->xport, 
                   xport_ctx->remote_ss, 
                   intent_ptr->iid);

      xport_ctx->channel_receive_pkt(open_ch_ctx, intent_ptr);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_remote_sigs

DESCRIPTION   Transport invokes this call to inform GLink of remote side
              changing its control signals

ARGUMENTS   *if_ptr      Pointer to interface instance; must be unique
                         for each edge

            rcid         Remote Channel ID

            remote_sigs  Remote signal state.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_remote_sigs
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  uint32                  rcid,       /* Remote channel ID */
  uint32                  remote_sigs /* Remote control signals */
)
{
  glink_core_xport_ctx_type *xport_ctx;
  glink_channel_ctx_type *open_ch_ctx;
  uint32 prev_sigs;

  xport_ctx = if_ptr->glink_core_priv;
      
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = glinki_find_ch_ctx_by_rcid(xport_ctx, rcid);
  glink_os_cs_release( &xport_ctx->channel_q_cs );
      
  if (!glinki_channel_fully_opened(open_ch_ctx))
      {
        ASSERT(0);
      }
  
      /* Found channel, let client know of new remote signal state */
      prev_sigs = open_ch_ctx->remote_sigs;
      open_ch_ctx->remote_sigs = remote_sigs;
  open_ch_ctx->notify_rx_sigs(open_ch_ctx,
                              open_ch_ctx->priv,
                              prev_sigs,
                              remote_sigs);
  }

