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
#include "glink_core_if.h"
#include "smem_list.h"
#include "glink_channel_migration.h"


#define FEATURE_CH_MIGRATION_FREE

void glinki_scan_notif_list_and_notify
(
  glink_transport_if_type *if_ptr,
  glink_link_state_type state
);

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
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
  glink_core_xport_ctx_type *xport_ctx,
  glink_transport_if_type   *if_ptr,
  uint32                    version,
  uint32                    features
)
{
  /* Version/Feautre can be negotiated both in ver_req and ver_ack
   * Only go through process once in case they are negotiated
   * in ver_req before receiving ver_ack */
  if( xport_ctx->status == GLINK_XPORT_LINK_UP )
  {
    return;
  }

  /* setup core based on transport capabilities*/
  xport_ctx->xport_capabilities = if_ptr->set_version( if_ptr,
                                                       version,
                                                       features );
  glink_core_setup(if_ptr);

  /* transport is ready to open channels */
  if_ptr->glink_core_priv->status = GLINK_XPORT_LINK_UP;

  /* Scan the notification list to check is we have to let any registered
  * clients know that link came online */
  glinki_scan_notif_list_and_notify(if_ptr, GLINK_LINK_STATE_UP);
}

static void glink_dummy_ch_migration_notification_cb
(
  glink_handle_type         handle,
  const void               *priv,
  glink_channel_event_type event
)
{
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_link_up

DESCRIPTION   Indicates that transport is now ready to start negotiation
              using the v0 configuration

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
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
  if_ptr->glink_core_priv->status = GLINK_XPORT_NEGOTIATING;

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

      glink_process_negotiation_complete( xport_ctx, if_ptr,
                                          version, features );
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
      glink_process_negotiation_complete( xport_ctx, if_ptr,
                                          version, features );
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
  glink_channel_ctx_type *remote_ch_ctx;
  glink_channel_ctx_type  *allocated_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_err_type status;
  glink_xport_priority negotiated_prio;
  boolean migration_state = TRUE;

  ASSERT(if_ptr != NULL);
  ASSERT(name != NULL);

#ifndef FEATURE_CH_MIGRATION_FREE
  glink_transport_if_type *present_if_ptr = NULL;
  boolean channel_exist = FALSE;
  glink_channel_ctx_type *present_ch_ctx = NULL;
  /* search if channel with given name exists locally */
  channel_exist = glinki_local_channel_exists(if_ptr, &present_if_ptr, name, &present_ch_ctx, TRUE);

  /* start channel migration negotiation only if channel exists on local side
   * for negotiation AND channel is willing to migrate */
  if(channel_exist &&
     (present_ch_ctx->ch_open_options & GLINK_OPT_INITIAL_XPORT))
  {
    /* channel exists on one of the xports for given remote ss */
    current_prio =  present_ch_ctx->req_if_ptr->glink_priority;
    negotiated_prio = glinki_negotiate_ch_migration(priority, current_prio);

    /* if current channel is open in same xport as negotiated xport
     * local side wont migrate. Set migration flag to FALSE */
    if(negotiated_prio == present_if_ptr->glink_priority)
    {
      migration_state = FALSE;
    }
  }
  else if(channel_exist)
  {
    /* channel exists but channel does not want to be moved to another xport.
       channel is set in stone */
    negotiated_prio = present_if_ptr->glink_priority;
    migration_state = FALSE;
  }
  else
  {
    /* channel does not exist on local side as yet
     * return negotiated prio as current xport prio on which
     * remote open request is received */
    negotiated_prio = if_ptr->glink_priority;
    migration_state = FALSE;
  }
#else
  negotiated_prio = if_ptr->glink_priority;
    migration_state = FALSE;
#endif

  xport_ctx = if_ptr->glink_core_priv;

  /* Allocate and initialize channel info structure */
  remote_ch_ctx = glink_os_calloc(sizeof(glink_channel_ctx_type));
  if(remote_ch_ctx == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_RM_CH_OPEN, name, xport_ctx->xport,
        xport_ctx->remote_ss, GLINK_STATUS_OUT_OF_RESOURCES);
    ASSERT(0);
  }

  /* Fill in the channel info structure */
  glink_os_string_copy(remote_ch_ctx->name, name, sizeof(remote_ch_ctx->name));
  remote_ch_ctx->rcid = rcid;

  status = glinki_add_ch_to_xport(if_ptr, NULL, remote_ch_ctx,
                                  &allocated_ch_ctx, FALSE,
                                  migration_state, negotiated_prio);
  ASSERT(status == GLINK_STATUS_SUCCESS);

  GLINK_LOG_EVENT(GLINK_EVENT_RM_CH_OPEN, name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);

#ifndef FEATURE_CH_MIGRATION_FREE

  /* We are done with channel migration negotiation at this stage
   * Tag all channels with given name on xports other than negotiated
   * xport for deletion */
  glinki_tag_ch_for_deletion(if_ptr, name, negotiated_prio);

    if(migration_state == TRUE)
    {
      glink_channel_ctx_type *new_ch_ctx;

    /* create a new channel context as current channel will be migrated */
    new_ch_ctx = (glink_channel_ctx_type *)
                   glink_os_calloc(sizeof(glink_channel_ctx_type));

    /* Fill in the channel info structure */
    new_ch_ctx->req_if_ptr =
             glinki_get_xport_from_prio(negotiated_prio,
                                          if_ptr->glink_core_priv->remote_ss);
    glink_os_string_copy(new_ch_ctx->name, present_ch_ctx->name,
                         sizeof(present_ch_ctx->name));
    new_ch_ctx->priv = present_ch_ctx->priv;
    new_ch_ctx->notify_rx = present_ch_ctx->notify_rx;
    new_ch_ctx->notify_rxv = present_ch_ctx->notify_rxv;
    new_ch_ctx->notify_tx_done = present_ch_ctx->notify_tx_done;
    new_ch_ctx->notify_state = present_ch_ctx->notify_state;
    new_ch_ctx->notify_rx_intent_req = present_ch_ctx->notify_rx_intent_req;
    new_ch_ctx->notify_rx_intent = present_ch_ctx->notify_rx_intent;
    new_ch_ctx->notify_rx_sigs = present_ch_ctx->notify_rx_sigs;
    new_ch_ctx->ch_open_options = present_ch_ctx->ch_open_options;

//      glink_os_cs_init(&new_ch_ctx->intent_q_cs);

    /* close current channel */
    present_ch_ctx->notify_state = glink_dummy_ch_migration_notification_cb;
    if_ptr->tx_cmd_ch_close( if_ptr, present_ch_ctx->lcid );

    /* add new channel context on negotiated xport */
    glinki_add_ch_to_xport(new_ch_ctx->req_if_ptr,
                           new_ch_ctx->req_if_ptr,
                           new_ch_ctx,
                           &allocated_ch_ctx,
                           TRUE,
                           TRUE,
                           new_ch_ctx->req_if_ptr->glink_priority);
  }
#endif
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
  uint32                  lcid,
  glink_xport_priority    migrated_ch_prio
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Move to closed state. Implies we clean up the channel from the
   * open list */

  /* Find channel in the open_list */
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->lcid == lcid) {
#ifndef FEATURE_CH_MIGRATION_FREE
      glink_transport_if_type *present_if_ptr = NULL;
      glink_channel_ctx_type  *present_ch_ctx = NULL;
      boolean channel_exist;
      /* find the channel in all xport list */
      channel_exist = glinki_local_channel_exists(if_ptr,
                                                  &present_if_ptr,
                                                  open_ch_ctx->name,
                                                  &present_ch_ctx,
                                                  FALSE);

      if(if_ptr->glink_priority == migrated_ch_prio && !channel_exist)
      {
        /* only local side has opened the channel. Remote side has not come up yet
         * which implies negotiation did not take place on remote side */
        /* DO NOTHING */
      }
      else if(if_ptr->glink_priority == migrated_ch_prio)
      {
        /* remote channel exists. channel migration negotiation happened
         * on remote side and negotitated xport is same as current xport */
        if(present_ch_ctx->ref_count == 1)
        {
          /* remote channel is present on different xport than negotiated one.
           * remote side will migrate its channel to negotiated xport */
          /* DO NOTHING */
        }
        else if(present_ch_ctx->ref_count == 2)
        {
          /* remote channel is open on same xport as current xport.
           * change channel state to GLINK_CH_STATE_OPEN and notify client */
          open_ch_ctx->state = GLINK_CH_STATE_OPEN;
          open_ch_ctx->notify_state(open_ch_ctx, open_ch_ctx->priv,
                                    GLINK_CONNECTED);
        }
        else
        {
          /* something went wrong in updating ref_count of channel */
          ASSERT(0);
        }
      }
      else
      {

        /* migrated xport priority <> current xport priority */
        /* check if remote channel is opened on negotiated xport already */
          if(migrated_ch_prio == present_if_ptr->glink_priority &&
             (open_ch_ctx->ch_open_options & GLINK_OPT_INITIAL_XPORT))
          {
            /* remote channel is already on negotiated xport. remote channel
             * will not migrate. Local side should migrate */

            glink_channel_ctx_type *new_ch_ctx =
              (glink_channel_ctx_type *)glink_os_calloc(
                                       sizeof(glink_channel_ctx_type));

            /* Fill in the channel info structure */
            glink_os_string_copy(new_ch_ctx->name,
                                 open_ch_ctx->name,
                                 sizeof(open_ch_ctx->name));
            new_ch_ctx->priv = open_ch_ctx->priv;
            new_ch_ctx->notify_rx = open_ch_ctx->notify_rx;
            new_ch_ctx->notify_rxv = open_ch_ctx->notify_rxv;
            new_ch_ctx->notify_tx_done = open_ch_ctx->notify_tx_done;
            new_ch_ctx->notify_state = open_ch_ctx->notify_state;
            new_ch_ctx->notify_rx_intent_req =
                                            open_ch_ctx->notify_rx_intent_req;
            new_ch_ctx->notify_rx_intent = open_ch_ctx->notify_rx_intent;
            new_ch_ctx->notify_rx_sigs = open_ch_ctx->notify_rx_sigs;
            new_ch_ctx->ch_open_options = open_ch_ctx->ch_open_options;

            present_ch_ctx->notify_state = glink_dummy_ch_migration_notification_cb;
            if_ptr->tx_cmd_ch_close( if_ptr, present_ch_ctx->lcid );

            /* migrate to negotiated xport */
            glinki_add_ch_to_xport(present_if_ptr,
                                   present_if_ptr,
                                   new_ch_ctx,
                                   NULL,
                                   TRUE,
                                   TRUE,
                                   present_if_ptr->glink_priority);
          }
          else
          {
            /* Either our transport is "set in stone"  OR */
            /* remote side will migrate to negotiated xport and will call
             * remote open on this side which will cause channel migration
             * negotiation and this side will ultimately migrate */
            /* DO NOTHING */
          }
        }
#else
        if(open_ch_ctx->ref_count == 2)
        {
          /* remote channel is open on same xport as current xport.
           * change channel state to GLINK_CH_STATE_OPEN and notify client */
          open_ch_ctx->state = GLINK_CH_STATE_OPEN;
          open_ch_ctx->notify_state(open_ch_ctx, open_ch_ctx->priv,
                                    GLINK_CONNECTED);
        }
#endif

      GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN_ACK, open_ch_ctx->name,
          xport_ctx->xport, xport_ctx->remote_ss, lcid);
      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }
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
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->lcid == lcid) {
      /* Found channel */
      open_ch_ctx->ref_count--;
      GLINK_LOG_EVENT(GLINK_EVENT_CH_CLOSE_ACK, open_ch_ctx->name,
          xport_ctx->xport, xport_ctx->remote_ss, lcid);

      /* Transition state */
      if(open_ch_ctx->state == GLINK_CH_STATE_OPEN) {
        /* this side initiated close procedure */
        open_ch_ctx->state = GLINK_CH_STATE_REMOTE_OPEN_LOCAL_CLOSE;
      } else if(open_ch_ctx->state == GLINK_CH_STATE_LOCAL_OPEN ||
                open_ch_ctx->state == GLINK_CH_STATE_LOCAL_OPEN_REMOTE_CLOSE ||
                open_ch_ctx->state == GLINK_CH_STATE_REMOTE_OPEN) {
        /* Other side never opened the port, or closed from its end */
        /* Clear everything */
        if(open_ch_ctx->ref_count == 0)
        {
          xport_ctx->channel_cleanup(open_ch_ctx);
          /* re-use channel id if it can be done */
          if(lcid == (xport_ctx->free_lcid-1)) {
            /* If channel being closed is the last opened channel
               re-use the lcid of this channel for any new channels */
            xport_ctx->free_lcid--;
          }
          smem_list_delete(&if_ptr->glink_core_priv->open_list, open_ch_ctx);

          /* Notify the client */
          open_ch_ctx->notify_state( open_ch_ctx, open_ch_ctx->priv,
                                     GLINK_LOCAL_DISCONNECTED);
          glink_os_free(open_ch_ctx);
        }
      } else {
        /* Unsupported state */
        ASSERT(0);
      }

      glink_os_cs_release(&xport_ctx->channel_q_cs);
      return;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */

  /* We are here in case we could not find the channel in the open list. */
  ASSERT(0);

  glink_os_cs_release(&xport_ctx->channel_q_cs);
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
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->rcid == rcid) {
        GLINK_LOG_EVENT(GLINK_EVENT_CH_REMOTE_CLOSE, open_ch_ctx->name,
            xport_ctx->xport, xport_ctx->remote_ss, rcid);
      /* Found channel, transition it to appropriate state based
       * on current state */
      open_ch_ctx->ref_count--;

      if(open_ch_ctx->state == GLINK_CH_STATE_OPEN) {
         open_ch_ctx->state = GLINK_CH_STATE_LOCAL_OPEN_REMOTE_CLOSE;

        /* Inform the client */
        open_ch_ctx->notify_state( open_ch_ctx, open_ch_ctx->priv,
                                   GLINK_REMOTE_DISCONNECTED);

        /* Send the remote close ACK back to the other side */
        if_ptr->tx_cmd_ch_remote_close_ack(if_ptr, open_ch_ctx->rcid);

        /* Free channel resources */
        xport_ctx->channel_cleanup(open_ch_ctx);

        GLINK_LOG_EVENT(GLINK_EVENT_CH_REMOTE_CLOSE, open_ch_ctx->name,
            xport_ctx->xport, xport_ctx->remote_ss, rcid);
        glink_os_cs_release(&xport_ctx->channel_q_cs);

        return;
      } else if (open_ch_ctx->state == GLINK_CH_STATE_REMOTE_OPEN ||
                 open_ch_ctx->state == GLINK_CH_STATE_REMOTE_OPEN_LOCAL_CLOSE) {
        /* Local side never opened the channel OR it opened it but closed it */
        /* Free channel resources */
        xport_ctx->channel_cleanup(open_ch_ctx);

        /* Send the remote close ACK back to the other side */
        if_ptr->tx_cmd_ch_remote_close_ack(if_ptr, open_ch_ctx->rcid);

        if(open_ch_ctx->ref_count == 0)
        {
          /* re-use channel id if it can be done */
          if (open_ch_ctx->lcid == (xport_ctx->free_lcid - 1)) {
            /* If channel being closed is the last opened channel
            re-use the lcid of this channel for any new channels */
            xport_ctx->free_lcid--;
          }
          smem_list_delete(&if_ptr->glink_core_priv->open_list, open_ch_ctx);
          glink_os_free(open_ch_ctx);
        }

        glink_os_cs_release(&xport_ctx->channel_q_cs);

        return;
      } else {
        /* SHould not get this callback for a channel in any other state */

        //OS_LOG_ERROR(3, "GLINK_RX_CMD_CH_REMOTE_CLOSE: received close cmd for invalid state"
        //                "[channel ctx: 0x%x][current channel state: %d]",
        //                open_ch_ctx, open_ch_ctx->state );
        glink_os_cs_release(&xport_ctx->channel_q_cs);
        if_ptr->tx_cmd_ch_remote_close_ack(if_ptr, open_ch_ctx->rcid);
        return;
      }
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */

  /* We are here in case we could not find the channel in the open list
   * or OPEN state. */
  ASSERT(0);

  glink_os_cs_release(&xport_ctx->channel_q_cs);
}

/*===========================================================================
FUNCTION      glink_ch_state_local_trans

DESCRIPTION   Process local state transition

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            rcid      Remote Channel ID

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_ch_state_local_trans
(
  glink_transport_if_type  *if_ptr,  /* Pointer to the interface instance */
  uint32                   lcid,     /* Local channel ID */
  glink_ch_state_type      new_state /* New local channel state */
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(if_ptr != NULL);

  xport_ctx = if_ptr->glink_core_priv;

  /* Find channel in the open_list */
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if(open_ch_ctx->lcid == lcid) {
      /* Found channel, transition it to appropriate state */
      open_ch_ctx->state = new_state;
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
    GLINK_LOG_EVENT(GLINK_EVENT_CH_STATE_TRANS, open_ch_ctx->name,
          xport_ctx->xport, xport_ctx->remote_ss, new_state);
    return;
  }/* end while */

  /* We are here in case we could not find the channel in the open list
   * or OPEN state. */
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
    if(open_ch_ctx->rcid == rcid) {
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
FUNCTION      glink_tx_resume

DESCRIPTION   If transport was full and could not continue a transmit
              operation, then it will call this function to notify the core
              that it is ready to resume transmission.

ARGUMENTS   *if_ptr    Pointer to interface instance; must be unique
                       for each edge

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_tx_resume
(
  glink_transport_if_type *if_ptr /* Pointer to the interface instance */
)
{
  /* Not sure what to do here */
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
