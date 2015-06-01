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
#include "smem_list.h"
#include "glink_channel_migration.h"

#define FEATURE_CH_MIGRATION_FREE

/*===========================================================================
                        GLOBAL FUNCTION DECLARATIONS
==========================================================================*/
extern void glinki_scan_notif_list_and_notify
(
  glink_transport_if_type *if_ptr,
  glink_link_state_type state
);


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

  glink_os_cs_acquire( &xport_ctx->status_cs );

  if( xport_ctx->status == GLINK_XPORT_LINK_UP )
  {
    glink_os_cs_release( &xport_ctx->status_cs );
    return;
  }

  glink_os_cs_release(&if_ptr->glink_core_priv->status_cs);

  /* setup core based on transport capabilities*/
  xport_ctx->xport_capabilities = if_ptr->set_version( if_ptr,
                                                       version,
                                                       features );
  glink_core_setup(if_ptr);

  /* transport is ready to open channels */
  glink_os_cs_acquire( &xport_ctx->status_cs );
  if_ptr->glink_core_priv->status = GLINK_XPORT_LINK_UP;
  glink_os_cs_release(&if_ptr->glink_core_priv->status_cs);

  /* Scan the notification list to check is we have to let any registered
   * clients know that link came online */
  glinki_scan_notif_list_and_notify(if_ptr, GLINK_LINK_STATE_UP);
}


/*===========================================================================
  FUNCTION      glink_clean_channel_ctx
===========================================================================*/
/**

  This is called when channel is fully closed
  Clean up the context and redeem channel id if possible

  @param[in]    xport_ctx    transport context
  @param[in]    channel_ctx  channel context


  @return        None
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
  xport_ctx->channel_cleanup( channel_ctx );

  if( channel_ctx->lcid == ( xport_ctx->free_lcid - 1 ) )
  {
    /* If channel being closed is the last opened channel
       re-use the lcid of this channel for any new channels */
    xport_ctx->free_lcid--;
  }

  smem_list_delete(&xport_ctx->open_list, channel_ctx);
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
  const glink_core_version_type *version_array;
  glink_core_xport_ctx_type *xport_ctx;

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  version_array = xport_ctx->version_array;

  /* Update the transport state */
  glink_os_cs_acquire(&if_ptr->glink_core_priv->status_cs);
  if_ptr->glink_core_priv->status = GLINK_XPORT_NEGOTIATING;
  glink_os_cs_release(&if_ptr->glink_core_priv->status_cs);

  /* Start the negtiation */
  if_ptr->tx_cmd_version(if_ptr, version_array->version,
      version_array->features);

  GLINK_LOG_EVENT(GLINK_EVENT_LINK_UP, NULL, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);

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

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* The version to use must be a subset of supported version and features
   * on this host and remote host */
  ver = &xport_ctx->version_array[xport_ctx->version_indx];
  ASSERT(ver);

  if (version == ver->version)
  {
    /* Call the transport's negotiation function */
    negotiated_features = ver->negotiate_features(if_ptr, ver, features);

    /* If negotiated features match the provided features, version nogetiation
     * is complete */
    if(negotiated_features == features)
    {
      /* send version ack before allowing to open channels */
      if_ptr->tx_cmd_version_ack(if_ptr, version, features);

      glink_process_negotiation_complete( if_ptr, version, features );
      return;
    }
    else
    {
      if_ptr->tx_cmd_version_ack(if_ptr, version, negotiated_features);
    }
  }
  else
  {
    /* Next time use older version */
    ver = &xport_ctx->version_array[xport_ctx->version_indx];

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

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Check to see if version returned by remote end is supported by
   * this host. Remote side would have acked only when the version/features
   * sent by this host did not match the remote */

  ver = &xport_ctx->version_array[xport_ctx->version_indx];
  ASSERT(ver);

  if (ver->version == version)
  {
    /* Call the transport's negotiation function */
    negotiated_features = ver->negotiate_features(if_ptr, ver, features);

    if(negotiated_features != features)
    {
      /* Continue negotiating */
      if_ptr->tx_cmd_version(if_ptr, version, negotiated_features);
    }
    else
    {
      glink_process_negotiation_complete( if_ptr, version, features );
    }
  }
  else
  {
    while (ver->version > version)
    {
      /* Next time use older version */
      ASSERT(xport_ctx->version_indx > 0);
      xport_ctx->version_indx--;
      ver = &xport_ctx->version_array[xport_ctx->version_indx];
    }

    /* Versions don't match, return ACK with the feature set that we support */
    if_ptr->tx_cmd_version(if_ptr, ver->version, ver->features);
  }
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_remote_open

DESCRIPTION   Receive remote channel open request; Calls
              glink_transport_if:: tx_cmd_ch_remote_open_ack as a result

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            rcid     Remote Channel ID

            *name    String name for logical channel

            prio     xport priority requested by remote side

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
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

  glink_xport_priority        negotiated_xport_priority;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_err_type              status;

  ASSERT( if_ptr != NULL );
  ASSERT( name != NULL );

  xport_ctx = if_ptr->glink_core_priv;

  /* Allocate and initialize channel info structure */
  remote_ch_ctx = glink_os_calloc( sizeof( glink_channel_ctx_type ) );
  if(remote_ch_ctx == NULL)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_RM_CH_OPEN, name, xport_ctx->xport,
        xport_ctx->remote_ss, GLINK_STATUS_OUT_OF_RESOURCES);
    ASSERT(0);
  }

  /* Fill in the channel info structure */
  glink_os_string_copy(remote_ch_ctx->name, name, sizeof(remote_ch_ctx->name));
  remote_ch_ctx->rcid = rcid;

#ifndef FEATURE_CH_MIGRATION_FREE
  glink_channel_ctx_type     *present_ch_ctx = NULL;
  glink_transport_if_type    *negotiated_xport;
  glink_channel_ctx_type     *dummy_ch_ctx;

  /* search if channel with given name exists locally */

  glink_os_cs_acquire( &xport_ctx->channel_q_cs );
  present_ch_ctx = glinki_local_channel_exists( if_ptr, name );

  if( !present_ch_ctx )
  {
    // channel doesn't exists yet and migration will not happen here
    status = glinki_add_ch_to_xport( if_ptr,
                                     if_ptr,
                                     remote_ch_ctx,
                                     &allocated_ch_ctx,
                                     FALSE,
                                     if_ptr->glink_priority );
    ASSERT(status == GLINK_STATUS_SUCCESS);

    GLINK_LOG_EVENT(GLINK_EVENT_RM_CH_OPEN, name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);

    glink_os_cs_release( &xport_ctx->channel_q_cs );
    return;
  }

  glink_os_cs_release( &xport_ctx->channel_q_cs );

  present_ch_ctx->rcid = rcid;

  if( present_ch_ctx->tag_ch_for_close )
  {
    // Migration already started.
    GLINK_LOG_EVENT(GLINK_EVENT_CH_MIGRATION_IN_PROGRESS, name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);

    return;
  }

  /* Negotiate the priority for migration if needed */
  if( priority < present_ch_ctx->req_if_ptr->glink_priority )
  {
    negotiated_xport_priority = present_ch_ctx->req_if_ptr->glink_priority;
  }
  else
  {
    negotiated_xport_priority = priority;
  }

  negotiated_xport =  glinki_get_xport_from_prio( negotiated_xport_priority,
                                                  xport_ctx->remote_ss );

  if( if_ptr == negotiated_xport )
  {
    /* Current transport is best one. No need to migrate */

    status = glinki_add_ch_to_xport( if_ptr,
                                     if_ptr,
                                     remote_ch_ctx,
                                     &allocated_ch_ctx,
                                     FALSE,
                                     negotiated_xport_priority );

    ASSERT(status == GLINK_STATUS_SUCCESS);
    GLINK_LOG_EVENT( GLINK_EVENT_CH_NO_MIGRATION, name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);

    return;
  }

  /* Need to migrate to new transport */
  if_ptr->tx_cmd_ch_remote_open_ack( if_ptr,
                                     rcid,
                                     negotiated_xport_priority );

  glink_create_dummy_ch_ctx( &dummy_ch_ctx, present_ch_ctx );
  glink_close_dummy_ch_for_migration( if_ptr, dummy_ch_ctx );
  glink_init_ch_migrate_candidate( present_ch_ctx );

  status = glinki_add_ch_to_xport( negotiated_xport,
                                   negotiated_xport,
                                   present_ch_ctx,
                                   &allocated_ch_ctx,
                                   TRUE,
                                   negotiated_xport_priority );

  ASSERT( status == GLINK_STATUS_SUCCESS );

  glink_os_free( remote_ch_ctx );

#else
  negotiated_xport_priority = if_ptr->glink_priority;

  status = glinki_add_ch_to_xport( if_ptr,
                                   NULL,
                                   remote_ch_ctx,
                                   &allocated_ch_ctx,
                                   FALSE,
                                   negotiated_xport_priority );

  ASSERT(status == GLINK_STATUS_SUCCESS);
#endif

  GLINK_LOG_EVENT(GLINK_EVENT_RM_CH_OPEN, name, xport_ctx->xport,
    xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_open_ack

DESCRIPTION   This function is invoked by the transport in response to
              glink_transport_if:: tx_cmd_ch_open

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            lcid     Local Channel ID

            prio     Negotiated xport priority from remote side

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_ch_open_ack
(
  glink_transport_if_type *if_ptr,
  uint32                   lcid,
  glink_xport_priority     migrated_ch_prio
)
{
  glink_channel_ctx_type     *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Move to closed state. Implies we clean up the channel from the
   * open list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  /* Find channel in the open_list */
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->lcid == lcid)
    {
#ifndef FEATURE_CH_MIGRATION_FREE
      glink_transport_if_type    *negotiated_xport = NULL;
      negotiated_xport = glinki_get_xport_from_prio( migrated_ch_prio,
                                                     xport_ctx->remote_ss );

      if( negotiated_xport != if_ptr &&
          !open_ch_ctx->tag_ch_for_close &&
          ( open_ch_ctx->ch_open_options & GLINK_OPT_INITIAL_XPORT ) )
      {
        glink_channel_ctx_type *dummy_ch_ctx;
        glink_err_type          status;

        glink_create_dummy_ch_ctx( &dummy_ch_ctx, open_ch_ctx );
        glink_close_dummy_ch_for_migration( if_ptr, dummy_ch_ctx );

        glink_init_ch_migrate_candidate( open_ch_ctx );
        dummy_ch_ctx = NULL;

        status = glinki_add_ch_to_xport( negotiated_xport,
                                         negotiated_xport,
                                         open_ch_ctx,
                                         &dummy_ch_ctx,
                                         TRUE,
                                         negotiated_xport->glink_priority );

        ASSERT( status == GLINK_STATUS_SUCCESS );
      }
      else
      {
        glink_os_cs_acquire( &open_ch_ctx->ch_state_cs );

        if( open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSING )
        {
          /* Local client called glink_close
           * before local channel is fully opened */
          glink_os_cs_release( &open_ch_ctx->ch_state_cs );
          glink_os_cs_release( &xport_ctx->channel_q_cs );
          return;
        }

        open_ch_ctx->local_state = GLINK_LOCAL_CH_OPENED;

        if( open_ch_ctx->remote_state == GLINK_REMOTE_CH_OPENED )
        {
          glink_os_cs_release( &open_ch_ctx->ch_state_cs );
          open_ch_ctx->notify_state( open_ch_ctx,
                                     open_ch_ctx->priv,
                                     GLINK_CONNECTED );
        }
        else
        {
          glink_os_cs_release( &open_ch_ctx->ch_state_cs );
        }
      }

#else
      /* This is same code as above. once core capability code is done copy
       * issue should be resolved */
      glink_os_cs_acquire( &open_ch_ctx->ch_state_cs );
      open_ch_ctx->local_state = GLINK_LOCAL_CH_OPENED;
      if( open_ch_ctx->remote_state == GLINK_REMOTE_CH_OPENED )
      {
        /* remote channel is open on same xport as current xport.
         * change channel state to GLINK_CH_STATE_OPEN and notify client */
        glink_os_cs_release( &open_ch_ctx->ch_state_cs );
        open_ch_ctx->notify_state( open_ch_ctx,
                                   open_ch_ctx->priv,
                                   GLINK_CONNECTED);
      }
#endif

      GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN_ACK, open_ch_ctx->name,
          xport_ctx->xport, xport_ctx->remote_ss, lcid);
      glink_os_cs_release(&xport_ctx->channel_q_cs);
      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }
  glink_os_cs_release(&xport_ctx->channel_q_cs);
  /* We are here in case we could not find the channel in the open list. */
  ASSERT(0);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_close_ack

DESCRIPTION   This function is invoked by the transport in response to
              glink_transport_if_type:: tx_cmd_ch_close

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            lcid      Local Channel ID

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_ch_close_ack
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  lcid     /* Local channel ID */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Move to closed state. Implies we clean up the channel from the
   * open list */

  /* Find channel in the open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = smem_list_first(&xport_ctx->open_list);
  while( open_ch_ctx )
  {
    if( open_ch_ctx->lcid == lcid )
    {
      /* Found channel */
      glink_os_cs_acquire( &open_ch_ctx->ch_state_cs );


      GLINK_LOG_EVENT(GLINK_EVENT_CH_CLOSE_ACK, open_ch_ctx->name,
          xport_ctx->xport, xport_ctx->remote_ss, lcid);

      ASSERT( open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSING );

      open_ch_ctx->local_state = GLINK_LOCAL_CH_CLOSED;

      if( open_ch_ctx->remote_state == GLINK_REMOTE_CH_CLOSED ||
          open_ch_ctx->remote_state == GLINK_REMOTE_CH_SSR_RESET )
      {
        glink_clean_channel_ctx( xport_ctx, open_ch_ctx );
      }

      glink_os_cs_release(&open_ch_ctx->ch_state_cs);
      glink_os_cs_release(&xport_ctx->channel_q_cs);

      open_ch_ctx->notify_state( open_ch_ctx,
                                 open_ch_ctx->priv,
                                 GLINK_LOCAL_DISCONNECTED );

      if( open_ch_ctx->remote_state == GLINK_REMOTE_CH_CLOSED ||
          open_ch_ctx->remote_state == GLINK_REMOTE_CH_SSR_RESET )
      {
        glink_os_free( open_ch_ctx );
      }

      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */

  glink_os_cs_release(&xport_ctx->channel_q_cs);
  /* We are here in case we could not find the channel in the open list. */
  ASSERT(0);
}

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_remote_close

DESCRIPTION   Remote channel close request; will result in sending
              glink_transport_if_type:: tx_cmd_ch_remote_close_ack

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            rcid      Remote Channel ID

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_ch_remote_close
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid     /* Remote channel ID */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Find channel in the open_list */
  glink_os_cs_acquire( &xport_ctx->channel_q_cs );

  open_ch_ctx = smem_list_first( &if_ptr->glink_core_priv->open_list );
  while(open_ch_ctx != NULL)
  {
    if( open_ch_ctx->rcid == rcid )
    {
      GLINK_LOG_EVENT( GLINK_EVENT_CH_REMOTE_CLOSE, open_ch_ctx->name,
                       xport_ctx->xport, xport_ctx->remote_ss, rcid);
      /* Found channel, transition to appropriate state based on current state
       * grab lock to perform channel state related operations */
      glink_os_cs_acquire(&open_ch_ctx->ch_state_cs);

      ASSERT( open_ch_ctx->remote_state == GLINK_REMOTE_CH_OPENED );

      open_ch_ctx->remote_state = GLINK_REMOTE_CH_CLOSED;

      if( open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSED )
      {
        /* Local side never opened the channel OR it opened it but closed it */
        /* Free channel resources */
        glink_clean_channel_ctx( xport_ctx, open_ch_ctx );
      }

      glink_os_cs_release(&open_ch_ctx->ch_state_cs);
      glink_os_cs_release(&xport_ctx->channel_q_cs);

      /* Send the remote close ACK back to the other side */
      if_ptr->tx_cmd_ch_remote_close_ack(if_ptr, open_ch_ctx->rcid);

      if( open_ch_ctx->local_state == GLINK_LOCAL_CH_CLOSED )
      {
        glink_os_free(open_ch_ctx);
      }
      else
      {
        /* Inform the client */
        open_ch_ctx->notify_state( open_ch_ctx, open_ch_ctx->priv,
                                   GLINK_REMOTE_DISCONNECTED);
      }

      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */

  glink_os_cs_release( &xport_ctx->channel_q_cs );
  ASSERT(0);
}

/*===========================================================================
FUNCTION      glink_rx_put_pkt_ctx

DESCRIPTION   Transport invokes this call to receive a packet fragment (must
              have previously received an rx_cmd_rx_data packet)

ARGUMENTS   *if_ptr      Pointer to interface instance; must be unique
                         for each edge

            rcid         Remote Channel ID

            *intent_ptr  Pointer to the intent fragment

            complete     True if pkt is complete

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_put_pkt_ctx
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  glink_rx_intent_type    *intent_ptr, /* Fragment ptr */
  boolean                 complete     /* True if pkt is complete */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(if_ptr != NULL && intent_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Find channel in the open_list */
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->rcid == rcid) {
      /* Found channel */
      GLINK_LOG_EVENT(GLINK_EVENT_CH_PUT_PKT_CTX, open_ch_ctx->name,
          xport_ctx->xport, xport_ctx->remote_ss, intent_ptr->iid);

      xport_ctx->channel_receive_pkt(open_ch_ctx, intent_ptr);

      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */

  /* We end up here if we don't find the channel */
  ASSERT(0);
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
  glink_channel_ctx_type *open_ch_ctx;
  uint32 prev_sigs;

  ASSERT(if_ptr != NULL);

  /* Find channel in the open_list */
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->rcid == rcid ) {
      glink_os_cs_acquire( &open_ch_ctx->ch_state_cs );
      if( open_ch_ctx->local_state != GLINK_LOCAL_CH_OPENED &&
          open_ch_ctx->remote_state != GLINK_REMOTE_CH_OPENED )
      {
        glink_os_cs_release( &open_ch_ctx->ch_state_cs );
        ASSERT(0);
        return;
      }
      glink_os_cs_release( &open_ch_ctx->ch_state_cs );
      /* Found channel, let client know of new remote signal state */
      prev_sigs = open_ch_ctx->remote_sigs;
      open_ch_ctx->remote_sigs = remote_sigs;
      open_ch_ctx->notify_rx_sigs(open_ch_ctx, open_ch_ctx->priv,
                                  prev_sigs, remote_sigs);
      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }

  /* We end up here if we don't find the channel */
  ASSERT(0);
}


/*===========================================================================
FUNCTION      glink_set_core_version

DESCRIPTION   Sets the core version used by the transport; called after
              completing negotiation.

ARGUMENTS   *if_ptr    Pointer to interface instance; must be unique
                       for each edge

             version   Negotiated transport version

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_set_core_version
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  version  /* Version */
)
{
}
