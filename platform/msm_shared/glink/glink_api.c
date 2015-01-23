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
#include <glink_channel_migration.h>
#include <smem_list.h>
#include <smem_type.h>

#define GLINK_NOT_INITIALIZED 0
#define GLINK_INITIALIZED     1

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/
int glink_core_status = GLINK_NOT_INITIALIZED;

os_cs_type *glink_transport_q_cs;
os_cs_type *glink_mem_log_cs;

glink_mem_log_entry_type glink_mem_log_arr[GLINK_MEM_LOG_SIZE];
uint32 glink_mem_log_idx = 0;

/* Keep a list of registered transport for each edge allowed for this host */
smem_list_type glink_registered_transports[GLINK_NUM_HOSTS];

smem_list_type glink_link_notif_list;

/* List of supported hosts */
const char* glink_hosts_supported[]   = { "apss",
                                          "mpss",
                                          "lpass",
                                          "dsps",
                                          "wcnss",
                                          "tz",
                                          "rpm",
                                        };

/* Forward function declarations */
void glinki_free_intents(glink_channel_ctx_type *open_ch_ctx);

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

glink_err_type glinki_add_ch_to_xport
(
  glink_transport_if_type  *if_ptr,
  glink_transport_if_type  *req_if_ptr,
  glink_channel_ctx_type   *ch_ctx,
  glink_channel_ctx_type   **allocated_ch_ctx,
  unsigned int              local_open,
  boolean                      migration_state,
  glink_xport_priority      migrated_ch_prio
)
{
  glink_err_type             status = 0;
  glink_channel_ctx_type     *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx = if_ptr->glink_core_priv;

  /* See if channel already exists in open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  while(open_ch_ctx != NULL)
  {
    if (strcmp(open_ch_ctx->name, ch_ctx->name) == 0)
    {
      /* We've found a channel name is already in the list of open channel */
      /* increase reference open count for channel */
      *allocated_ch_ctx = open_ch_ctx;
      open_ch_ctx->ref_count++;

      /* Case A: Channel was opened before on the same host */
      if((open_ch_ctx->state == GLINK_CH_STATE_REMOTE_OPEN) && local_open) {

        open_ch_ctx->req_if_ptr = req_if_ptr;

        /* Copy local open ctx params */
        open_ch_ctx->notify_rx = ch_ctx->notify_rx;
        open_ch_ctx->notify_rxv = ch_ctx->notify_rxv;
        open_ch_ctx->notify_tx_done = ch_ctx->notify_tx_done;
        open_ch_ctx->notify_state = ch_ctx->notify_state;
        open_ch_ctx->notify_rx_intent_req = ch_ctx->notify_rx_intent_req;
        open_ch_ctx->notify_rx_intent = ch_ctx->notify_rx_intent;
        open_ch_ctx->notify_rx_sigs = ch_ctx->notify_rx_sigs;
        open_ch_ctx->priv = ch_ctx->priv;
        open_ch_ctx->ch_open_options = ch_ctx->ch_open_options;

        /* release lock before context switch otherwise it is causing
         * deadlock */
        glink_os_cs_release(&xport_ctx->channel_q_cs);

        /* Send open cmd to transport */
        status = if_ptr->tx_cmd_ch_open(if_ptr,
                      open_ch_ctx->lcid, open_ch_ctx->name,
                      open_ch_ctx->req_if_ptr->glink_priority);
      }
      else if ((open_ch_ctx->state == GLINK_CH_STATE_LOCAL_OPEN) &&
               (!local_open))
      {
        /* Case B: Channel was opened on this end and we got a remote open */
        open_ch_ctx->rcid = ch_ctx->rcid;

        status = xport_ctx->channel_init(open_ch_ctx);

        /* release lock before context switch otherwise it is causing deadlock */
        glink_os_cs_release(&xport_ctx->channel_q_cs);

        if (status == GLINK_STATUS_SUCCESS)
        {
          /* Send ACK to transport */
          if_ptr->tx_cmd_ch_remote_open_ack(if_ptr, open_ch_ctx->rcid, migrated_ch_prio);
        }
      } else if ((open_ch_ctx->state == GLINK_CH_STATE_REMOTE_OPEN_LOCAL_CLOSE)
                  && (local_open)) {
        /* Allocate new channel context */
        break; /* code would break out of loop and create new ch ctx */
      } else {
        /* Can't handle this state */
        ASSERT(0);
      }

      break;
    } /* end if match found */
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */

  if (open_ch_ctx != NULL)
  {
    glink_os_free(ch_ctx);

    /* connect channel here if state is local open and remote open request
     * comes up and channel migration is done; channel will be connected in
     * remote_open_ack if channel state is remote open and local open
     * request comes up */
    if(open_ch_ctx->state == GLINK_CH_STATE_LOCAL_OPEN &&
       migration_state == FALSE && status == GLINK_STATUS_SUCCESS)
    {
      /* Set the channel state to OPEN */
      open_ch_ctx->state = GLINK_CH_STATE_OPEN;
      /* Inform the client */
      open_ch_ctx->notify_state(open_ch_ctx, open_ch_ctx->priv,
                                GLINK_CONNECTED);
    }
  }
  else
  {
    /* Channel not in the list - it was not previously opened */
    ch_ctx->if_ptr = if_ptr;
    *allocated_ch_ctx = ch_ctx;

    /* Set channel state */
    if (local_open) {
      /* This is a local open */
      ch_ctx->state      = GLINK_CH_STATE_LOCAL_OPEN;
      ch_ctx->req_if_ptr = req_if_ptr;
    }
    else {
      ch_ctx->state = GLINK_CH_STATE_REMOTE_OPEN;
    }

    glink_os_cs_init(&ch_ctx->tx_cs);

    /* Append the channel to the transport interface's open_list */
    ch_ctx->ref_count++;
    ch_ctx->lcid = xport_ctx->free_lcid;
    xport_ctx->free_lcid++;
    smem_list_append(&if_ptr->glink_core_priv->open_list, ch_ctx);

    /* release lock before context switch otherwise it is causing deadlock */
    glink_os_cs_release(&xport_ctx->channel_q_cs);

    /* Send the OPEN command to transport */
    if (local_open)
    {
      status = if_ptr->tx_cmd_ch_open(if_ptr, ch_ctx->lcid,
                                      ch_ctx->name,
                                      ch_ctx->req_if_ptr->glink_priority);
    }
    else
    {
      /* initialize channel resources */
      status = xport_ctx->channel_init(ch_ctx);

      /* ACK the transport for remote open */
      if (status == GLINK_STATUS_SUCCESS)
      {
        if_ptr->tx_cmd_ch_remote_open_ack(if_ptr, ch_ctx->rcid,
                                          migrated_ch_prio);
      }
    }

    if (status != GLINK_STATUS_SUCCESS)
    {
      /* Remove the channel from the transport interface's open_list */
      xport_ctx->free_lcid--;

      glink_os_cs_acquire(&xport_ctx->channel_q_cs);
      smem_list_delete(&if_ptr->glink_core_priv->open_list, ch_ctx);
      glink_os_cs_release(&xport_ctx->channel_q_cs);

      /* free the ch_ctx structure and return */
      xport_ctx->channel_cleanup(ch_ctx);
      glink_os_free(ch_ctx);
    }
  }

  return status;
}

/** Default implementation of optional callbacks */

static void glink_default_notify_rx_sigs
(
  glink_handle_type  handle,
  const void         *priv,
  uint32             prev,
  uint32             curr
)
{
  return;
}

static void glinki_call_link_notifier
(
  glink_link_notif_data_type *link_notif_data,
  glink_core_xport_ctx_type  *xport_ctx,
  glink_link_state_type      state
)
{
  glink_link_info_type link_info;

  ASSERT(xport_ctx);
  ASSERT(link_notif_data);

  link_info.xport = xport_ctx->xport;
  link_info.remote_ss = xport_ctx->remote_ss;
  link_info.link_state = state;
  link_notif_data->link_notifier(&link_info, link_notif_data->priv);
}

static uint32 glinki_find_remote_host
(
  const char *remote_ss
)
{
  uint32 remote_host;
  ASSERT(remote_ss);

  for(remote_host = 0;
      remote_host < sizeof(glink_hosts_supported)/sizeof(char *);
      remote_host++) {
    if( 0 == strcmp(glink_hosts_supported[remote_host], remote_ss) ) {
      /* Match found, break out of loop */
      break;
    }
  }
  return remote_host;
}

static void glinki_check_xport_and_notify
(
  glink_link_notif_data_type *link_notif_data,
  glink_core_xport_ctx_type  *xport_ctx,
  glink_link_state_type      state
)
{
  glink_link_info_type link_info;

  ASSERT(xport_ctx);
  ASSERT(link_notif_data);

  link_info.xport = xport_ctx->xport;
  link_info.remote_ss = xport_ctx->remote_ss;
  link_info.link_state = state;

  if(link_notif_data->xport == NULL ||
     0 == strcmp(xport_ctx->xport, link_notif_data->xport)) {
    /* xport not specified, or it is specified and matches the current xport */
    /* Invoke registered callback */
    link_notif_data->link_notifier(&link_info, link_notif_data->priv);
  }
}


static void glinki_scan_xports_and_notify
(
  glink_link_notif_data_type *link_notif_data
)
{
  unsigned int remote_host;
  glink_transport_if_type *if_ptr;
  glink_core_xport_ctx_type  *xport_ctx;

  ASSERT(link_notif_data);

  /* Find matching subsystem */
  if(link_notif_data->remote_ss) {
    remote_host = glinki_find_remote_host(link_notif_data->remote_ss);

    /* Find the xport and give link UP notification */
    if_ptr = smem_list_first(&glink_registered_transports[remote_host]);
    if(if_ptr == NULL) {
      /* No registered xports at this time, return without doing anything */
      return;
    }

    if(link_notif_data->xport) {
      do {
        xport_ctx = if_ptr->glink_core_priv;
        if( 0 == strcmp(xport_ctx->xport, link_notif_data->xport) ) {
          /* Match found, break out of loop */
          break;
        }
      }while ((if_ptr = smem_list_next(if_ptr)) != NULL);

      if((if_ptr != NULL) && (xport_ctx->status == GLINK_XPORT_LINK_UP)) {
        /* Invoke registered callback */
        glinki_call_link_notifier(link_notif_data, xport_ctx,
                                  GLINK_LINK_STATE_UP);
      }
    } else {
      /* No xport has been specified, invoke notifier for all registered
       * xports */
      do {
        xport_ctx = if_ptr->glink_core_priv;
        if(xport_ctx->status == GLINK_XPORT_LINK_UP) {
          /* Invoke registered callback */
          glinki_call_link_notifier(link_notif_data, xport_ctx,
                                    GLINK_LINK_STATE_UP);
        }
      }while ((if_ptr = smem_list_next(if_ptr)) != NULL);
    }
  } else {
    /* No remote ss is specified, invoke notifier for all remote_ss */
    for(remote_host = 0;
        remote_host < sizeof(glink_hosts_supported)/sizeof(char *);
        remote_host++) {
      /* Find the xport and give link UP notification */
      if_ptr = smem_list_first(&glink_registered_transports[remote_host]);
      if(if_ptr == NULL) {
        /* No registered xports at this time, continue with next remote_ss */
        continue;
      }

      if(link_notif_data->xport) {
        do {
          xport_ctx = if_ptr->glink_core_priv;
          if( 0 == strcmp(xport_ctx->xport, link_notif_data->xport) ) {
            /* Match found, break out of loop */
            break;
          }
        }while ((if_ptr = smem_list_next(if_ptr)) != NULL);

        if((if_ptr != NULL) && (xport_ctx->status == GLINK_XPORT_LINK_UP)) {
          /* Invoke registered callback */
          glinki_call_link_notifier(link_notif_data, xport_ctx,
                                    GLINK_LINK_STATE_UP);
        }
      } else {
        /* No xport has been specified, invoke notifier for all registered
         * xports */
        do {
          xport_ctx = if_ptr->glink_core_priv;
          if(xport_ctx->status == GLINK_XPORT_LINK_UP) {
            /* Invoke registered callback */
            glinki_call_link_notifier(link_notif_data, xport_ctx,
                                      GLINK_LINK_STATE_UP);
          }
        }while ((if_ptr = smem_list_next(if_ptr)) != NULL);
      }
    } /* end for remote_host */
  }/* end if else (link_notif_data->remote_ss) */
} /* glinki_scan_xports_and_notify */

void glinki_scan_notif_list_and_notify
(
  glink_transport_if_type *if_ptr,
  glink_link_state_type state
)
{
  glink_link_notif_data_type *link_notif_data;
  glink_core_xport_ctx_type  *xport_ctx = if_ptr->glink_core_priv;

  link_notif_data = smem_list_first(&glink_link_notif_list);

  if(link_notif_data == NULL) {
    /* list empty */
    return;
  }

  do {
      if(link_notif_data->remote_ss &&
         0 == strcmp(xport_ctx->remote_ss, link_notif_data->remote_ss)) {
        /* remote_ss specified and matches */
        glinki_check_xport_and_notify(link_notif_data, xport_ctx, state);
      } else if(link_notif_data->remote_ss == NULL) {
        /* remote_ss not specified, invoke link notif for any remote_ss */
        if(link_notif_data->xport) {
          glinki_check_xport_and_notify(link_notif_data, xport_ctx, state);
        } /* if else link_notif_data->xport */
      } /* if else link_notif_data->remote_ss */
  } while ( (link_notif_data = smem_list_next(link_notif_data)) != NULL);

} /* glinki_scan_notif_list_and_notify */

void glinki_scan_channels_and_notify_discon
(
  glink_transport_if_type *if_ptr
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
    /* Found channel, transition it to appropriate state based
     * on current state */
    if(open_ch_ctx->state == GLINK_CH_STATE_OPEN) {
      open_ch_ctx->state = GLINK_CH_STATE_LOCAL_OPEN;

      /* Inform the client */
      open_ch_ctx->notify_state(open_ch_ctx, open_ch_ctx->priv,
          GLINK_REMOTE_DISCONNECTED);
    } else if (open_ch_ctx->state == GLINK_CH_STATE_REMOTE_OPEN) {
      /* Local side never opened the channel */
      /* Free channel resources */
      xport_ctx->channel_cleanup(open_ch_ctx);

      smem_list_delete(&if_ptr->glink_core_priv->open_list, open_ch_ctx);

      glink_os_free(open_ch_ctx);
    }
    open_ch_ctx = smem_list_next(open_ch_ctx);
  }/* end while */
  glink_os_cs_release(&xport_ctx->channel_q_cs);
}

void glink_ssr(const char* remote_ss)
{
  unsigned int remote_host;
  glink_transport_if_type *if_ptr;

  remote_host = glinki_find_remote_host(remote_ss);

  /* Scan through the registered interfaces with the crashing ss
     and let the clients know about the crash via LINK_DOWN
     notification followed by REMOTE_DISCONNECT */
  if_ptr = smem_list_first(&glink_registered_transports[remote_host]);

  while(if_ptr != NULL) {
    /* Invoke LINK_DOWN notification for any registered notifiers */
    glinki_scan_notif_list_and_notify(if_ptr, GLINK_LINK_STATE_DOWN);

    /* Invoke REMOTE_DISCONNECT for all channels associated with if_ptr */
    glinki_scan_channels_and_notify_discon(if_ptr);

    /* Let the xport know about ssr */
    if_ptr->ssr(if_ptr);

    if_ptr = smem_list_next(if_ptr);
  }
}


/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_init

DESCRIPTION   Initializes the GLink core library.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_init(void)
{
  uint32 i;

  glink_mem_log_cs = glink_os_cs_create();

  glink_core_status = GLINK_INITIALIZED;

  /* Create/Initalize crtitical sections */
  glink_transport_q_cs = glink_os_cs_create();
  if(glink_transport_q_cs == NULL) {
    return;
  }

  glink_os_cs_acquire(glink_transport_q_cs);
  for(i= 0; i < sizeof(glink_registered_transports)/sizeof(smem_list_type);
      i++)
  {
    smem_list_init(&glink_registered_transports[i]);
  }
  glink_os_cs_release(glink_transport_q_cs);
}

/*===========================================================================
FUNCTION      glink_core_register_transport

DESCRIPTION   Transport calls this API to register its interface with GLINK
              Core

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            *cfg      Pointer to transport configuration structure.

RETURN VALUE  Standard GLINK error codes.

SIDE EFFECTS  None
===========================================================================*/
glink_err_type glink_core_register_transport
(
  glink_transport_if_type       *if_ptr,
  glink_core_transport_cfg_type *cfg
)
{
  unsigned int remote_host = 0;
  glink_core_xport_ctx_type *xport_ctx;
  /* Param validation */
  if(if_ptr == NULL || cfg == NULL)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, "", "",
        GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  if(cfg->name == NULL       ||
     cfg->remote_ss == NULL  ||
     cfg->version == NULL    ||
     cfg->version_count == 0 ||
     cfg->max_cid == 0)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, "", "",
        GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }


  if(if_ptr->tx_cmd_version == NULL             ||
     if_ptr->tx_cmd_version_ack == NULL         ||
     if_ptr->set_version == NULL                ||
     if_ptr->tx_cmd_ch_open == NULL             ||
     if_ptr->tx_cmd_ch_close == NULL            ||
     if_ptr->tx_cmd_ch_remote_open_ack == NULL  ||
     if_ptr->tx_cmd_ch_remote_close_ack == NULL ||
     if_ptr->tx_cmd_set_sigs  == NULL           ||
     if_ptr->ssr  == NULL)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, cfg->name,
        cfg->remote_ss, GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;;
  }


  /* Allocate/fill out the GLink Core interface structure */
  {
    glink_core_if_type *core_if = glink_os_calloc(sizeof(glink_core_if_type));
    if(core_if == NULL) {
      GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, cfg->name,
          cfg->remote_ss, GLINK_STATUS_OUT_OF_RESOURCES);
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
    core_if->link_up = glink_link_up;
    core_if->rx_cmd_version = glink_rx_cmd_version;
    core_if->rx_cmd_version_ack = glink_rx_cmd_version_ack;
    core_if->rx_cmd_ch_remote_open = glink_rx_cmd_ch_remote_open;
    core_if->rx_cmd_ch_open_ack = glink_rx_cmd_ch_open_ack;
    core_if->rx_cmd_ch_close_ack = glink_rx_cmd_ch_close_ack;
    core_if->rx_cmd_ch_remote_close = glink_rx_cmd_ch_remote_close;
    core_if->ch_state_local_trans = glink_ch_state_local_trans;
    core_if->rx_put_pkt_ctx = glink_rx_put_pkt_ctx;
    core_if->rx_cmd_remote_sigs = glink_rx_cmd_remote_sigs;
    core_if->tx_resume = glink_tx_resume;
    core_if->set_core_version = glink_set_core_version;

    /* Set the glink_core_if_ptr to point to the allocated structure */
    if_ptr->glink_core_if_ptr = core_if;
  }

  /* Allocate/fill out the GLink private context data */
  {
     xport_ctx = glink_os_calloc(sizeof(glink_core_xport_ctx_type));
    if(xport_ctx == NULL) {
      /* Free previously allocated memory */
      glink_os_free(if_ptr->glink_core_if_ptr);

      GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, cfg->name,
          cfg->remote_ss, GLINK_STATUS_OUT_OF_RESOURCES);

      return GLINK_STATUS_OUT_OF_RESOURCES;
    }

    glink_os_string_copy(xport_ctx->xport, cfg->name,
        sizeof(xport_ctx->xport));
    glink_os_string_copy(xport_ctx->remote_ss, cfg->remote_ss,
        sizeof(xport_ctx->xport));
    xport_ctx->free_lcid = 1; /* lcid 0 is reserved for invalid channel */
    xport_ctx->version_array = cfg->version;
    xport_ctx->version_indx = cfg->version_count - 1;

    glink_os_cs_init(&xport_ctx->channel_q_cs);
    glink_os_cs_init(&xport_ctx->liid_cs);

    glink_os_cs_acquire(&xport_ctx->channel_q_cs);
    smem_list_init(&xport_ctx->open_list);
    glink_os_cs_release(&xport_ctx->channel_q_cs);

    /* Set the glink_core_if_ptr to point to the allocated structure */
    if_ptr->glink_core_priv = xport_ctx;
    xport_ctx->status = GLINK_XPORT_REGISTERED;
  }

  /* Push the transport interface into appropriate queue */
  remote_host = glinki_find_remote_host(cfg->remote_ss);

  if(remote_host == GLINK_NUM_HOSTS ) {
    /* Unknown transport name trying to register with GLink */
    GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, xport_ctx->xport,
           xport_ctx->remote_ss, GLINK_STATUS_INVALID_PARAM);

    return GLINK_STATUS_INVALID_PARAM;
  }
  glink_os_cs_acquire(glink_transport_q_cs);
  smem_list_append(&glink_registered_transports[remote_host], if_ptr);
  glink_os_cs_release(glink_transport_q_cs);

  GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, NULL, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);

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
  ASSERT(link_id != NULL);

  /* Make sure client provided us with the correct version of the input
   * structure */
  if(link_id->version != GLINK_LINK_ID_VER || link_id->link_notifier == NULL) {
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Save the callback on the notification list */
  if((link_notif_data = glink_os_malloc(sizeof(glink_link_notif_data_type)))
       == NULL) {
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  /* Check for remote_ss validity */
  if(link_id->remote_ss != NULL) {
    remote_host = glinki_find_remote_host(link_id->remote_ss);

    if(remote_host == sizeof(glink_hosts_supported)/sizeof(char *)) {
      glink_os_free(link_notif_data);
      return GLINK_STATUS_INVALID_PARAM;
    }
  }

  link_notif_data->xport = link_id->xport;
  link_notif_data->remote_ss = link_id->remote_ss;
  link_notif_data->link_notifier = link_id->link_notifier;
  link_notif_data->priv = priv; /* private client data */

  /* Append the request to the list for link UP/DOWN notifications */
  smem_list_append(&glink_link_notif_list, link_notif_data);

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
  ASSERT(handle);

  smem_list_delete(&glink_link_notif_list,
                   (glink_link_notif_data_type*)handle);

  glink_os_free(handle);

  return GLINK_STATUS_SUCCESS;
}

/**
 * This function gives best available transport for give edge
 *
 * @param[in]    remote_host  Index into glink_registered_transports array of
 *                            registered transports list per edge
 *
 * @return       pointer to glink_transport_if_type
 *
 * @sideeffects  NONE
 */
glink_transport_if_type* glink_get_best_xport
(
  unsigned int remote_host
)
{
  glink_transport_if_type   *if_ptr    = NULL, *best_if_ptr = NULL;
  glink_xport_priority       priority  = GLINK_MIN_PRIORITY;
  glink_core_xport_ctx_type *xport_ctx = NULL;

  best_if_ptr = if_ptr = smem_list_first(
                          &glink_registered_transports[remote_host]);

  while(if_ptr != NULL)
  {
    /* check if priority of current transport is higher than
     * current highest priority (0 = highest priority)
     */
    xport_ctx = if_ptr->glink_core_priv;
    if( xport_ctx->status == GLINK_XPORT_LINK_UP &&
        if_ptr->glink_priority < priority )
    {
      best_if_ptr = if_ptr;
      priority    = if_ptr->glink_priority;
    }

    if_ptr    = smem_list_next(if_ptr);
  } /* end while() */

  return best_if_ptr;
}

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
  glink_transport_if_type *if_ptr, *req_if_ptr;
  glink_channel_ctx_type  *ch_ctx;
  unsigned int            remote_host;

  /* Param validation */
  if(cfg_ptr == NULL)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN, NULL, "", "",
        GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  if(cfg_ptr->remote_ss == NULL             ||
     cfg_ptr->name == NULL                  ||
     cfg_ptr->notify_state == NULL)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN, NULL, "", "",
        GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Evaluate the equivalent edge name->enum for future use */
  remote_host = glinki_find_remote_host(cfg_ptr->remote_ss);

  if(remote_host == GLINK_NUM_HOSTS ) {
    /* Unknown transport name trying to register with GLink */
    GLINK_LOG_EVENT(GLINK_EVENT_REGISTER_XPORT, cfg_ptr->name, "",
        cfg_ptr->remote_ss, GLINK_STATUS_INVALID_PARAM);

    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Allocate and initialize channel info structure */
  ch_ctx = glink_os_calloc(sizeof(glink_channel_ctx_type));
  if(ch_ctx == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN, cfg_ptr->name, "",
        "", GLINK_STATUS_OUT_OF_RESOURCES);
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

  if (ch_ctx->notify_rx_sigs == NULL) {
    /* set default callback */
    ch_ctx->notify_rx_sigs = glink_default_notify_rx_sigs;
  }

  glink_os_cs_acquire(glink_transport_q_cs);

  /* Check to see if requested transport is available */
  for (if_ptr = smem_list_first(&glink_registered_transports[remote_host]);
       if_ptr != NULL;
       if_ptr = smem_list_next(if_ptr))
  {
    glink_core_xport_ctx_type *xport_ctx = if_ptr->glink_core_priv;
    glink_channel_ctx_type    *allocated_ch_ctx;

    if (xport_ctx->status == GLINK_XPORT_LINK_UP &&
        (cfg_ptr->transport == NULL ||
         0 == strcmp(cfg_ptr->transport, xport_ctx->xport)) &&
        xport_ctx->verify_open_cfg(ch_ctx))
    {
      glink_err_type status;

      if(cfg_ptr->transport == NULL)
      {
        /* get best available transport */
        if_ptr = req_if_ptr = glink_get_best_xport(remote_host);
      }
      else
      {
        if(cfg_ptr->options & GLINK_OPT_INITIAL_XPORT)
        {
          /* xport suggested by client is optional.
           * get best available xport */
          req_if_ptr = glink_get_best_xport(remote_host);
        }
        else
        {
          req_if_ptr = if_ptr;
        }
      }
      /* Xport match found */
      status = glinki_add_ch_to_xport( if_ptr,
                                       req_if_ptr,
                                       ch_ctx,
                                       &allocated_ch_ctx,
                                       TRUE,
                                       TRUE,
                                       if_ptr->glink_priority );

      if (status == GLINK_STATUS_SUCCESS) {
        /* Set the handle and return */
        *handle = allocated_ch_ctx;
      }
      else {
        *handle = NULL;
      }

      glink_os_cs_release(glink_transport_q_cs);

      GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN, ch_ctx->name, xport_ctx->xport,
        xport_ctx->remote_ss, status);
      return status;
    }
  } /* end for() */

  glink_os_cs_release(glink_transport_q_cs);

  /* Code gets here if we are not able to find reqeusted transport */
  GLINK_LOG_EVENT(GLINK_EVENT_CH_OPEN, cfg_ptr->name, cfg_ptr->transport,
      cfg_ptr->remote_ss, GLINK_STATUS_NO_TRANSPORT);
  glink_os_free(ch_ctx);
  return GLINK_STATUS_NO_TRANSPORT;
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
  glink_core_xport_ctx_type *xport_ctx = handle->if_ptr->glink_core_priv;
  glink_ch_state_type ch_state;

  if(handle == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_CLOSE, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  ch_state = handle->state;

  /* Check to see if channel is in open/opening state */
  if (ch_state != GLINK_CH_STATE_OPEN &&
      ch_state != GLINK_CH_STATE_LOCAL_OPEN &&
      ch_state != GLINK_CH_STATE_LOCAL_OPEN_REMOTE_CLOSE &&
      ch_state != GLINK_CH_STATE_REMOTE_OPEN)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_CLOSE, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, ch_state);
    return GLINK_STATUS_FAILURE;
  }

  /* Transition to closing */
  //handle->state = GLINK_CH_STATE_CLOSING;

  /* Send CLOSE cmd to the transport interface */
  status = handle->if_ptr->tx_cmd_ch_close(handle->if_ptr, handle->lcid);
  GLINK_LOG_EVENT(GLINK_EVENT_CH_CLOSE, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, status);
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
  glink_core_tx_pkt_type pctx;
  boolean                req_intent = options & GLINK_TX_REQ_INTENT;
  glink_core_xport_ctx_type *xport_ctx = handle->if_ptr->glink_core_priv;

  /* Input validation */
  if(handle == NULL || iovec == NULL || size == 0 ||
     (vprovider == NULL && pprovider == NULL)) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_TX, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Make sure channel is in OPEN state */
  if(handle->state != GLINK_CH_STATE_OPEN )
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_CLOSE, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_FAILURE);
    return GLINK_STATUS_FAILURE;
  }

  /* Protect the entire tx operation under a lock as a client may call
     tx in different thread context */
  glink_os_cs_acquire(&handle->tx_cs);

  pctx.pkt_priv = pkt_priv;
  pctx.size = size;
  pctx.size_remaining = size;
  pctx.vprovider = vprovider;
  pctx.pprovider = pprovider;

  if (vprovider == &glink_dummy_tx_vprovider)
  {
    pctx.data = (void*)iovec;
    pctx.iovec = &pctx;
  }
  else
  {
    pctx.data = (void*)iovec;
    pctx.iovec = iovec;
  }

  status = xport_ctx->use_rm_intent(handle, &pctx, req_intent);

  /* Call transport API to transmit data */
  while (pctx.size_remaining != 0 && status == GLINK_STATUS_SUCCESS)
  {
    status = handle->if_ptr->tx(handle->if_ptr, handle->lcid, &pctx);
  }

  GLINK_LOG_EVENT(GLINK_EVENT_CH_TX, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, status);

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
  glink_core_xport_ctx_type *xport_ctx = handle->if_ptr->glink_core_priv;
  size_t tmp;

  /* Input validation */
  if(handle == NULL || size == 0) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_Q_RX_INTENT, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* short circuit for intentless mode */
  if(xport_ctx->xport_capabilities & GLINK_CAPABILITY_INTENTLESS) {
    return GLINK_STATUS_FAILURE;
  }

  /* Make sure channel is in OPEN state */
  if(handle->state != GLINK_CH_STATE_OPEN)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_Q_RX_INTENT, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_FAILURE);
    return GLINK_STATUS_FAILURE;
  }

  /* Allocate an intent structure */
  lc_intent = glink_os_calloc(sizeof(glink_rx_intent_type));
  if(lc_intent == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_Q_RX_INTENT, handle->name, xport_ctx->xport,
        xport_ctx->remote_ss, GLINK_STATUS_OUT_OF_RESOURCES);
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  glink_os_cs_acquire(&handle->if_ptr->glink_core_priv->liid_cs);

  /* Call transport API to allocate rx intent buffer */
  status = handle->if_ptr->allocate_rx_intent(handle->if_ptr, size, lc_intent);
  if(status != GLINK_STATUS_SUCCESS) {
    glink_os_free(lc_intent);
    glink_os_cs_release(&handle->if_ptr->glink_core_priv->liid_cs);
    GLINK_LOG_EVENT(GLINK_EVENT_CH_Q_RX_INTENT, handle->name, xport_ctx->xport,
        xport_ctx->remote_ss, status);
    return status;
  }

  if (handle->notify_rxv == NULL &&
     (lc_intent->vprovider(lc_intent->iovec, 0, &tmp) == NULL || tmp < size)) {
    /* Allocate bounce buffer for non-vectored Rx */
    lc_intent->data = glink_os_malloc(size);

    if(lc_intent->data == NULL) {
      handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
      glink_os_free(lc_intent);
      glink_os_cs_release(&handle->if_ptr->glink_core_priv->liid_cs);
      GLINK_LOG_EVENT(GLINK_EVENT_CH_Q_RX_INTENT, handle->name,
        xport_ctx->xport, xport_ctx->remote_ss, GLINK_STATUS_OUT_OF_RESOURCES);
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
  }

  /* push the intent on local queue. Do this before calling tx cmd
     as transport may try to read data into the newly queued rx_buffer */
  lc_intent->iid = handle->if_ptr->glink_core_priv->liid;
  lc_intent->size = size;
  lc_intent->pkt_priv = pkt_priv;
  glink_os_cs_acquire(&handle->pintents->intent_q_cs);
  smem_list_append(&handle->pintents->local_intent_q, lc_intent);
  glink_os_cs_release(&handle->pintents->intent_q_cs);

  /* Call transport API to queue rx intent */
  /* Increment the local intent ID counter associated with this channel */
  handle->if_ptr->glink_core_priv->liid++;

  status = handle->if_ptr->tx_cmd_local_rx_intent(handle->if_ptr,
              handle->lcid, size, lc_intent->iid);
  if(status != GLINK_STATUS_SUCCESS) {
    /* Failure */
    glink_os_cs_acquire(&handle->pintents->intent_q_cs);
    smem_list_delete(&handle->pintents->local_intent_q, lc_intent);
    glink_os_cs_release(&handle->pintents->intent_q_cs);

    handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
    glink_os_free(lc_intent->data);
    glink_os_free(lc_intent);
  }
  glink_os_cs_release(&handle->if_ptr->glink_core_priv->liid_cs);

  GLINK_LOG_EVENT(GLINK_EVENT_CH_Q_RX_INTENT, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, status);
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
  if(handle == NULL || ptr == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_RX_DONE, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* short circuit for intentless mode */
  if(xport_ctx->xport_capabilities & GLINK_CAPABILITY_INTENTLESS) {
    return GLINK_STATUS_SUCCESS;
  }

  /* Make sure channel is in OPEN state */
  if(handle->state != GLINK_CH_STATE_OPEN)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_RX_DONE, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_FAILURE);
    return GLINK_STATUS_FAILURE;
  }

  /* Free the intent */
  lc_intent = smem_list_first(&handle->pintents->local_intent_client_q);
  while(lc_intent != NULL) {
    size_t tmp;

    if(lc_intent->iovec == ptr || (handle->notify_rxv == NULL &&
       (lc_intent->data == ptr ||
       ptr == lc_intent->vprovider(lc_intent->iovec, 0, &tmp)))) {

      uint32 iid;

      /* Found intent, delete it */
      glink_os_cs_acquire(&handle->pintents->intent_q_cs);
      smem_list_delete(&handle->pintents->local_intent_client_q, lc_intent);
      glink_os_cs_release(&handle->pintents->intent_q_cs);

      iid = lc_intent->iid;

      if (reuse)
      {
        lc_intent->used = 0;

        glink_os_cs_acquire(&handle->pintents->intent_q_cs);
        smem_list_append(&handle->pintents->local_intent_q, lc_intent);
        glink_os_cs_release(&handle->pintents->intent_q_cs);
      }
      else
      {
        /* Free the intent */
        handle->if_ptr->deallocate_rx_intent(handle->if_ptr, lc_intent);
        if(lc_intent->data) {
          /* Free the bounce buffer if we had allocated one */
          glink_os_free(lc_intent->data);
        }
        glink_os_free(lc_intent);
      }

      /* Note that the actual buffer, lc_intent->data, was allocated by the
      * transport and should be freed by the xport. We should not touch it */
      /* Let the xport know we are done with the buffer */
      handle->if_ptr->tx_cmd_local_rx_done(handle->if_ptr, handle->lcid,
                                           iid, reuse);

      GLINK_LOG_EVENT(GLINK_EVENT_CH_RX_DONE, handle->name, xport_ctx->xport,
          xport_ctx->remote_ss, GLINK_STATUS_SUCCESS);
      return GLINK_STATUS_SUCCESS;
    }
    lc_intent = smem_list_next(lc_intent);
  }

  GLINK_LOG_EVENT(GLINK_EVENT_CH_RX_DONE, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_INVALID_PARAM);
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
  glink_core_xport_ctx_type *xport_ctx = handle->if_ptr->glink_core_priv;
  glink_err_type status;

  /* Input validation */
  if(handle == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_SIG_SET, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Make sure channel is in OPEN state */
  if(handle->state != GLINK_CH_STATE_OPEN)
  {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_SIG_SET, handle->name, xport_ctx->xport,
      xport_ctx->remote_ss, GLINK_STATUS_FAILURE);
    return GLINK_STATUS_FAILURE;
  }

  status = handle->if_ptr->tx_cmd_set_sigs(handle->if_ptr, handle->lcid,
                                           sig_value);
  if(GLINK_STATUS_SUCCESS == status) {
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
  if(handle == NULL || sig_value == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_SIG_L_GET, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
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
  if(handle == NULL || sig_value == NULL) {
    GLINK_LOG_EVENT(GLINK_EVENT_CH_SIG_R_GET, NULL, "",
        "", GLINK_STATUS_INVALID_PARAM);
    return GLINK_STATUS_INVALID_PARAM;
  }

  *sig_value = handle->remote_sigs;

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
  ASSERT(handle);

  if(handle->if_ptr->poll) {
    return handle->if_ptr->poll(handle->if_ptr);
  }
  return GLINK_STATUS_FAILURE;
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
  ASSERT(handle);

  if(handle->if_ptr->mask_rx_irq) {
    return handle->if_ptr->mask_rx_irq(handle->if_ptr, mask);
  }
  return GLINK_STATUS_FAILURE;
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

/* ============ Internal Logging API ================ */
void glink_mem_log
(
  const char *func,
  uint32 line,
  glink_log_event_type type,
  const char *msg,
  const char *xport,
  const char *remote_ss,
  uint32 param
)
{
#ifdef DEBUG_GLINK
  dprintf(INFO, "%s:%u, event:%d, msg:%s, xport:%s, remote_ss:%s, param:%u\n", func, line, type, msg, xport, remote_ss, param);
#endif
}
