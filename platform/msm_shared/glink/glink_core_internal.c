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
#include <glink_internal.h>

#ifdef FEATURE_TRACER_PACKET
#include "glink_tracer.h"
#endif
/*===========================================================================
  MACRO DEFINITION
===========================================================================*/
/*===========================================================================
  GLOBAL DATA DECLARATIONS
===========================================================================*/
static os_cs_type glink_transport_q_cs[GLINK_NUM_HOSTS];

#if defined(GLINK_OS_DEFINED_LOGGING) || defined(GLINK_MEMORY_LOGGING)
/* glink_log_filter_status indicates if channel filtering is enabled or not */
glink_logging_filter_cfg_type log_filter_cfg = { 
    FALSE,      /* Filter Disabled */
    "",         /* Filter channel name */
    0x0,        /* Filter lcid - will be updated dynamically */
    0x0,        /* Filter rcid - will be updated dynamically */
    0xFFFFFFFF, /* remote host ID */
    0x0,        /* Channel Context */
    0xFFFFFFFF, /* All xports are enabled */
   };
#endif

/* Keep a list of registered transport for each edge allowed for this host 
 * ***IMPORTANT*** 
 * it should be safe to traverse this list without taking locks */
static smem_list_type glink_registered_transports[GLINK_NUM_HOSTS];

/* List of supported hosts */
const char* glink_hosts_supported[] = { "apss",
                                        "mpss",
                                        "lpass",
                                        "dsps",
                                        "wcnss",
                                        "tz",
                                        "rpm",
                                      };

#if defined(GLINK_MEMORY_LOGGING)
static glink_mem_log_entry_type glink_mem_log_arr[GLINK_MEM_LOG_SIZE];
static uint32 glink_mem_log_idx = 0;
static os_cs_type glink_mem_log_cs;
#endif

static smem_list_type  glink_link_notif_list;
static os_cs_type      glink_link_notif_list_cs;

/*===========================================================================
  GLOBAL FUNCTION DECLARATION
===========================================================================*/
/*===========================================================================
  LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      glinki_scan_channels_and_notify_discon
===========================================================================*/
/** 
 * Close all the channels belong to this transport
 * This is helper function for glink_ssr
 *
 * @param[in]    if_ptr  pointer to transport interface
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
static void glinki_scan_channels_and_notify_discon
(
  glink_transport_if_type *if_ptr
)
{
  glink_channel_ctx_type     *open_ch_ctx, *dummy_open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  glink_remote_state_type     remote_state;
  
  xport_ctx = if_ptr->glink_core_priv;

  /* Find channel in the open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
  glink_os_cs_release(&xport_ctx->channel_q_cs);
  
  while(open_ch_ctx)
  {
    glink_os_cs_acquire(&xport_ctx->channel_q_cs);
    open_ch_ctx->remote_state = GLINK_REMOTE_CH_SSR_RESET;
    
    dummy_open_ch_ctx = smem_list_next(open_ch_ctx);
    
    switch (open_ch_ctx->local_state)
    {
      case GLINK_LOCAL_CH_OPENED:
      case GLINK_LOCAL_CH_OPENING:
        glink_os_cs_release(&xport_ctx->channel_q_cs);
        /* local channel has called open at the moment. */
        open_ch_ctx->notify_state(open_ch_ctx, 
                                  open_ch_ctx->priv, 
                                  GLINK_REMOTE_DISCONNECTED);
        break;
        
      case GLINK_LOCAL_CH_CLOSING:
        /* Case when local client already closed channel 
         * but has not received ack yet */
        if_ptr->glink_core_if_ptr->rx_cmd_ch_close_ack(if_ptr, 
                                                       open_ch_ctx->lcid);
       glink_os_cs_release(&xport_ctx->channel_q_cs);
        break;
        
      case GLINK_LOCAL_CH_CLOSED:  /* Channel fully closed - local, remote */
      case GLINK_LOCAL_CH_INIT:    /* We had only remote open and waiting for local open */
        xport_ctx->channel_cleanup(open_ch_ctx);
        smem_list_delete(&if_ptr->glink_core_priv->open_list, open_ch_ctx);
        remote_state = open_ch_ctx->remote_state;

        if (remote_state != GLINK_REMOTE_CH_CLEANUP)
        {
          glink_os_free(open_ch_ctx);
        }
        glink_os_cs_release(&xport_ctx->channel_q_cs);
        break;
        
      default:
        glink_os_cs_release(&xport_ctx->channel_q_cs);
        /* invalid local channel state */
        ASSERT(0);
    }
    
    open_ch_ctx = dummy_open_ch_ctx;
  } /* end while */
}

/*===========================================================================
  FUNCTION      glinki_xport_priority_comp
===========================================================================*/
/** 
 * Helper function for glinki_get_xport_from_prio. This will be provided to
 * glinki_xports_find function.
 * Return true if given transport has same priority
 *
 * @param[in]    if_ptr     transport interface pointer
 * @param[in]    cond1      unused
 * @param[in]    priority   glink transport priority to find
 *
 * @return       TRUE   if if_ptr priority matches
 *               FALSE  otherwise
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static boolean glinki_xport_priority_comp
(
  glink_transport_if_type *if_ptr,
  void                    *cond1,
  uint32                   priority,
  void                   **out
)
{
  GLINK_OS_UNREFERENCED_PARAM( cond1 );
  GLINK_OS_UNREFERENCED_PARAM( out );

  return if_ptr->glink_priority == (glink_xport_priority)priority;
}

/*===========================================================================
  FUNCTION      glinki_client_requested_xport_check
===========================================================================*/
/**
 * check whether this is client requested xport or not
 *
 * @param[in]    if_ptr     transport interface pointer
 * @param[in]    xport_name transport name to find
 * @param[in]    cond2      unused
 * @param[in]    out        unused
 * 
 * @return       TRUE   if transport name
 *               FALSE  otherwise
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static boolean glinki_client_requested_xport_check
(
  glink_transport_if_type *if_ptr,
  void                    *xport_name,
  uint32                   cond2,
  void                   **out
)
{
  GLINK_OS_UNREFERENCED_PARAM( cond2 );
  GLINK_OS_UNREFERENCED_PARAM( out );
  
  if (!glinki_xport_linkup(if_ptr))
  {
    return FALSE;
  }
  
  return 0 == glink_os_string_compare(if_ptr->glink_core_priv->xport,
                                      (const char *)xport_name);
}

/*===========================================================================
  FUNCTION      glinki_update_best_xport
===========================================================================*/
/** 
 * Check whether given transport (if_ptr) has higher priority than
 * priority user wants to check and update if it is true.
 *
 * @param[in]    if_ptr          transport interface pointer
 * @param[in]    priority_param  xport priority to compare
 * @param[in]    param2          unused
 * @param[out]   best_xport      pointer to result xport
 * 
 * @return       FALSE  all the time since this needs to iterate all transports
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_update_best_xport
(
  glink_transport_if_type  *if_ptr,
  void                     *priority_param,
  uint32                    param2,
  void                    **best_xport
)
{
  glink_xport_priority *priority;
  
  GLINK_OS_UNREFERENCED_PARAM( param2 );

  ASSERT(best_xport && priority_param);
  
  priority = (glink_xport_priority*)priority_param;
  
  if (!glinki_xport_linkup(if_ptr))
  {
    return;
  }
  
  /* Given xport is best one if comparing xport doesn't exist */ 
  if (if_ptr->glink_priority < *priority)
  {
    *best_xport = (void*)if_ptr;
    *priority = if_ptr->glink_priority;
  }
}

/*===========================================================================
  FUNCTION      glinki_notify_xport_ssr
===========================================================================*/
/** 
 * Notify this xport of ssr event and do ssr clean up
 *
 * @param[in]    if_ptr         transport interface pointer
 * @param[in]    priority       priority to check
 * @param[in]    cond2          unused
 * @param[out]   updated_xport  pointer to result xport
 * 
 * @return       FALSE  all the time since this needs to iterate all transports
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_notify_xport_ssr
(
  glink_transport_if_type  *if_ptr,
  void                     *param1,
  uint32                    param2,
  void                    **out
)
{
  GLINK_OS_UNREFERENCED_PARAM( param1 );
  GLINK_OS_UNREFERENCED_PARAM( param2 );
  GLINK_OS_UNREFERENCED_PARAM( out );
  /* xport is down. change the xport state */
  glink_os_cs_acquire(&if_ptr->glink_core_priv->status_cs);
  if_ptr->glink_core_priv->status = GLINK_XPORT_REGISTERED;
  
  /* Let the xport know about ssr */
  if_ptr->ssr( if_ptr );
  
  /* Invoke LINK_DOWN notification for any registered notifiers */
  glinki_scan_notif_list_and_notify(if_ptr, GLINK_LINK_STATE_DOWN);
  
  /* Invoke REMOTE_DISCONNECT for all channels associated with if_ptr */
  glinki_scan_channels_and_notify_discon(if_ptr);
  
  glink_os_cs_release(&if_ptr->glink_core_priv->status_cs);
}

/*===========================================================================
  FUNCTION      glinki_check_xport_and_notify
===========================================================================*/
/** 
 * Notify this xport for link state change if applicable
 *
 * @param[in]  if_ptr                 transport interface pointer
 * @param[in]  link_notif_data_param  parameter for link notif data
 * @param[in]  state                  link state to notify
 * @param[out] out                    unused
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_check_xport_and_notify
(
  glink_transport_if_type  *if_ptr,
  void                     *link_notif_data_param,
  uint32                    state,
  void                    **out
)
{
  glink_core_xport_ctx_type  *xport_ctx;
  glink_link_notif_data_type *link_notif_data;
  glink_link_info_type        link_info;
  
  GLINK_OS_UNREFERENCED_PARAM( out );
  
  ASSERT(link_notif_data_param && if_ptr->glink_core_priv);
  
  link_notif_data = (glink_link_notif_data_type *)link_notif_data_param;
  xport_ctx = if_ptr->glink_core_priv;
  
  if (link_notif_data->xport == NULL ||
      0 == glink_os_string_compare(xport_ctx->xport, link_notif_data->xport))
  {
    /* xport not specified, or it is specified and matches the current xport */
    /* Invoke registered callback */
    link_info.xport = xport_ctx->xport;
    link_info.remote_ss = xport_ctx->remote_ss;
    link_info.link_state = (glink_link_state_type)state;
    
    link_notif_data->link_notifier(&link_info, link_notif_data->priv);
  }
}

/*===========================================================================
  FUNCTION      glinki_check_xport_link_up_and_notify
===========================================================================*/
/** 
 * Notify this xport for link state change if applicable
 *
 * @param[in]  if_ptr                 transport interface pointer
 * @param[in]  link_notif_data_param  parameter for link notif data
 * @param[in]  state                  link state to notify
 * @param[out] out                    unused. but just passed as parameter
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_check_xport_link_up_and_notify
(
  glink_transport_if_type  *if_ptr,
  void                     *link_notif_data_param,
  uint32                    state,
  void                    **out
)
{
  if (!glinki_xport_linkup(if_ptr))
  {
    return;
  }
  
  glinki_check_xport_and_notify(if_ptr, link_notif_data_param, state, out);
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
  boolean cs_created = FALSE;

#if defined(GLINK_MEMORY_LOGGING)
  cs_created = glink_os_cs_init(&glink_mem_log_cs);
  ASSERT(cs_created);
#elif defined(GLINK_OS_DEFINED_LOGGING)
  OS_LOG_INIT();
#endif
  
  smem_list_init(&glink_link_notif_list);
  cs_created = glink_os_cs_init(&glink_link_notif_list_cs);
  ASSERT(cs_created);
  
  /* Create/Initalize crtitical sections */
  for (i = 0; i < GLINK_NUM_HOSTS; ++i)
  {
    cs_created = glink_os_cs_init(&glink_transport_q_cs[i]);
    ASSERT(cs_created);
    smem_list_init(&glink_registered_transports[i]);
  }
}

/*===========================================================================
  FUNCTION      glinki_add_ch_to_xport
===========================================================================*/
/** 
 * Add remote/local channel context to xport open channel queue
 *
 * @param[in]    if_ptr            Pointer to xport if on which channel is to
 *                                 be opened
 * @param[in]    req_if_ptr        Pointer to xport if on which channel 
 *                                 actually wants to open
 * @param[in]    ch_ctx            channel context
 * @param[out]   allocated_ch_ctx  Pointer to channel context pointer
 * @param[in]    local_open        flag to determine if channel is opened 
 *                                 locally or remotely
 * @param[in]    migrated_ch_prio  negotiated xport priority
 *                                 (used to send priority via remote_open_ack to 
 *                                  remote side)
 *
 * @return       G-Link standard error type
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_err_type glinki_add_ch_to_xport
(
  glink_transport_if_type  *if_ptr,
  glink_channel_ctx_type   *ch_ctx,
  glink_channel_ctx_type  **allocated_ch_ctx,
  unsigned int              local_open,
  glink_xport_priority      migrated_ch_prio
)
{
  glink_err_type              status;
  glink_channel_ctx_type     *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  boolean                     valid_open_call;

  if ( if_ptr == NULL          ||
       ch_ctx->name[0] == '\0' ||
       allocated_ch_ctx == NULL )
  {
    return GLINK_STATUS_INVALID_PARAM;
  }
  
  xport_ctx = if_ptr->glink_core_priv;
  
  /* See if channel already exists in open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  
  open_ch_ctx = glinki_find_ch_ctx_by_name(xport_ctx,
                                           ch_ctx->name,
                                           (boolean)local_open,
                                           &valid_open_call);
  
  if ( !valid_open_call )
  {
    glink_os_free(ch_ctx);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return GLINK_STATUS_FAILURE;
  }
  
  if (!open_ch_ctx)
  {
    /* check if a new channel can be added */
    if ((uint32)smem_list_count(&xport_ctx->open_list) >= xport_ctx->max_lcid)
    {
      glink_os_cs_release(&xport_ctx->channel_q_cs);
      glink_os_free(ch_ctx);
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
    
    /* Channel not in the list - it was not previously opened */
    ch_ctx->if_ptr = if_ptr;
    *allocated_ch_ctx = ch_ctx;

    /* Set channel state */
    if (local_open)
    {
      /* This is a local open */
      ch_ctx->local_state = GLINK_LOCAL_CH_OPENING;
    }
    else
    {
      ch_ctx->remote_state = GLINK_REMOTE_CH_OPENED;
    }

    glink_os_cs_init(&ch_ctx->tx_cs);
    glink_os_cs_init(&ch_ctx->qos_cs);

    /* make sure next LCID is not used in currently open channels */
    open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);

    while (open_ch_ctx)
    {
      if (open_ch_ctx->lcid == xport_ctx->free_lcid)
      {
        xport_ctx->free_lcid++;
        
        if (xport_ctx->free_lcid >= xport_ctx->max_lcid)
        {
          xport_ctx->free_lcid = 1;
        }

        open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
        continue;
      }

      open_ch_ctx = smem_list_next(open_ch_ctx);
    }

    ch_ctx->lcid = xport_ctx->free_lcid;

    /* Append the channel to the transport interface's open_list */
    smem_list_append(&if_ptr->glink_core_priv->open_list, ch_ctx);

    /* release lock before context switch otherwise it is causing deadlock */
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    
    /* Send the OPEN command to transport */
    if (local_open)
    {
      status = if_ptr->tx_cmd_ch_open(if_ptr, ch_ctx->lcid,
                                      ch_ctx->name,
                                      migrated_ch_prio);
    }
    else
    {
      /* initialize channel resources */
      status = xport_ctx->channel_init(ch_ctx);

      if_ptr->tx_cmd_ch_remote_open_ack(if_ptr, ch_ctx->rcid, migrated_ch_prio);
    }

    if (status != GLINK_STATUS_SUCCESS)
    {
      /* Remove the channel from the transport interface's open_list */
      xport_ctx->free_lcid--;
      glinki_dequeue_item(&if_ptr->glink_core_priv->open_list,
                          ch_ctx,
                          &xport_ctx->channel_q_cs);
      
      /* free the ch_ctx structure and return */
      xport_ctx->channel_cleanup(ch_ctx);
      glink_os_free(ch_ctx);
    }
    else
    {
      //Update the Filter
      glinki_update_logging_filter(*allocated_ch_ctx, FALSE);
    }

    return status;
  }
  
  if (local_open)
  {
    /* LOCAL OPEN REQUEST */
    
    ch_ctx->rcid     = open_ch_ctx->rcid;
    ch_ctx->lcid     = open_ch_ctx->lcid;
    ch_ctx->pintents = open_ch_ctx->pintents;
    ch_ctx->if_ptr   = open_ch_ctx->if_ptr;

    if (ch_ctx->pintents != NULL)
    {
      ch_ctx->pintents->ch_ctx = ch_ctx;
    }

    ch_ctx->remote_state = open_ch_ctx->remote_state;
    ch_ctx->local_state  = GLINK_LOCAL_CH_OPENING;
    
    /* release lock before context switch otherwise it is causing
     * deadlock */
    smem_list_delete(&xport_ctx->open_list, open_ch_ctx);
    smem_list_append(&xport_ctx->open_list, ch_ctx);

    glink_os_cs_release(&xport_ctx->channel_q_cs);

    glink_os_free(open_ch_ctx);
    *allocated_ch_ctx = ch_ctx;
    
    /* Send open cmd to transport */
    status = if_ptr->tx_cmd_ch_open(if_ptr, 
                                    ch_ctx->lcid,
                                    ch_ctx->name,
                                    migrated_ch_prio);
  }
  else
  { 
    /* REMOTE OPEN REQUEST */
    open_ch_ctx->rcid = ch_ctx->rcid;
    *allocated_ch_ctx = open_ch_ctx;
    status = xport_ctx->channel_init(open_ch_ctx);

    if (status == GLINK_STATUS_SUCCESS)
    {
      open_ch_ctx->remote_state = GLINK_REMOTE_CH_OPENED;
    }
    
    /* release lock before context switch otherwise it is causing deadlock */
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    
    /* Send ACK to transport */
    if_ptr->tx_cmd_ch_remote_open_ack(if_ptr,
                                      open_ch_ctx->rcid,
                                      migrated_ch_prio);
    
    if (status == GLINK_STATUS_SUCCESS)
    {
      /* Inform the client */
      if (open_ch_ctx->local_state == GLINK_LOCAL_CH_OPENED)
      {
        open_ch_ctx->notify_state(open_ch_ctx, 
                                  open_ch_ctx->priv, 
                                  GLINK_CONNECTED);
      }
    }

    glink_os_free(ch_ctx);
  } /* end If - else (local_open) */
  
  /* If the channel was added to be closed later by channel migration
     do not update the filter */
  if ( status == GLINK_STATUS_SUCCESS )
  {
    //Update the Filter - Reset Boolean=FALSE
    glinki_update_logging_filter( *allocated_ch_ctx, FALSE );
  }
  
  return status;
}

/*===========================================================================
  FUNCTION      glinki_channel_fully_opened
===========================================================================*/
/** 
 * Check whether this channel is fully opened or not (local & remote)
 * This also checks transport status
 *
 * @param[in]  handle        glink channel handle
 *
 * @return     TRUE,  if channel is fully opened
 *             FASLE, otherwise
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
boolean glinki_channel_fully_opened
(
  glink_handle_type          handle
)
{
  boolean ch_fully_opened = TRUE;

  if (!glinki_xport_linkup(handle->if_ptr))
  {
    return FALSE;
  }
  
  glink_os_cs_acquire( &handle->if_ptr->glink_core_priv->channel_q_cs );
  
  if (handle->local_state != GLINK_LOCAL_CH_OPENED ||
      handle->remote_state != GLINK_REMOTE_CH_OPENED)
  {
    ch_fully_opened = FALSE;
  }

  glink_os_cs_release( &handle->if_ptr->glink_core_priv->channel_q_cs );
  
  return ch_fully_opened;
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
  if (if_ptr == NULL          ||
      cfg == NULL             ||
      cfg->name == NULL       ||
      cfg->remote_ss == NULL  ||
      cfg->version == NULL    ||
      cfg->version_count == 0 ||
      cfg->max_cid == 0)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", "", "", 
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
     if_ptr->ssr  == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", 
                           cfg->name, 
                           cfg->remote_ss, 
                           GLINK_STATUS_INVALID_PARAM );    
    return GLINK_STATUS_INVALID_PARAM;;
  }

  remote_host = glinki_find_remote_host(cfg->remote_ss);

  if(remote_host == GLINK_NUM_HOSTS )
  {
    /* Unknown transport name trying to register with GLink */
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", 
                           cfg->name, 
                           cfg->remote_ss, 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Set the glink_core_if_ptr to point to the default interface */
  if_ptr->glink_core_if_ptr = glink_core_get_default_interface();

  /* Allocate/fill out the GLink private context data */
  {
     xport_ctx = glink_os_calloc(sizeof(glink_core_xport_ctx_type));
    if(xport_ctx == NULL)
    {
      GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", 
                             cfg->name, 
                             cfg->remote_ss, 
                             GLINK_STATUS_OUT_OF_RESOURCES );
    
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }

    xport_ctx->xport = cfg->name;
    xport_ctx->remote_ss = cfg->remote_ss;
    xport_ctx->free_lcid = 1; /* lcid 0 is reserved for invalid channel */
    xport_ctx->max_lcid = cfg->max_cid; /* Max channel ID supported by transport */
    xport_ctx->version_array = cfg->version;
    xport_ctx->version_indx = (int32)cfg->version_count - 1;

    glink_os_cs_init(&xport_ctx->channel_q_cs);
    glink_os_cs_init(&xport_ctx->liid_cs);
    glink_os_cs_init(&xport_ctx->status_cs);

    glink_os_cs_acquire(&xport_ctx->channel_q_cs);
    smem_list_init(&xport_ctx->open_list);
    glink_os_cs_release(&xport_ctx->channel_q_cs);

    /* Set the glink_core_if_ptr to point to the allocated structure */
    if_ptr->glink_core_priv = xport_ctx;
    xport_ctx->status = GLINK_XPORT_REGISTERED;
  }
  
  /* Push the transport interface into appropriate queue */
  glinki_enqueue_item(&glink_registered_transports[remote_host],
                      if_ptr,
                      &glink_transport_q_cs[remote_host]);

  GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_REGISTER_XPORT, "", 
                  xport_ctx->xport,
                  xport_ctx->remote_ss,
                  GLINK_STATUS_SUCCESS);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION      glinki_register_link_notif_data
===========================================================================*/
/** 
 * Register link notification data
 *
 * @param[in]  link_notif_data  parameter for link notif data
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_register_link_notif_data
(
  glink_link_notif_data_type *link_notif_data
)
{
  glinki_enqueue_item(&glink_link_notif_list,
                      (void *)link_notif_data,
                      &glink_link_notif_list_cs);
}

/*===========================================================================
  FUNCTION      glinki_deregister_link_notif_data
===========================================================================*/
/** 
 * Deregister link notification data
 *
 * @param[in]  link_notif_data  parameter for link notif data
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_deregister_link_notif_data
(
  glink_link_notif_data_type *link_notif_data
)
{
  glinki_dequeue_item(&glink_link_notif_list,
                      (void *)link_notif_data,
                      &glink_link_notif_list_cs);
}

/*===========================================================================
  FUNCTION      glinki_scan_xports_and_notify
===========================================================================*/
/** 
 * Scan xports and notify link up state event
 *
 * @param[in]  link_notif_data  parameter for link notif data
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_scan_xports_and_notify
(
  glink_link_notif_data_type *link_notif_data
)
{
  unsigned int          remote_host;
  glink_link_state_type link_state = GLINK_LINK_STATE_UP;
  
  ASSERT(link_notif_data);
  
  /* Find matching subsystem */
  for (remote_host = 0; remote_host < GLINK_NUM_HOSTS; ++remote_host)
  {
    if (link_notif_data->remote_ss != NULL &&
        0 != glink_os_string_compare(glink_hosts_supported[remote_host],
                                     link_notif_data->remote_ss))
    {
      /* client is not interested in this remote SS */
      continue;
    }
    
    glinki_xports_for_each(glink_hosts_supported[remote_host],
                           glinki_check_xport_link_up_and_notify,
                           link_notif_data,
                           link_state,
                           NULL);
  }
}

/*===========================================================================
  FUNCTION      glinki_scan_notif_list_and_notify
===========================================================================*/
/** 
 * Scan registered link notification list and notify of xport link state change
 *
 * @param[in]  if_ptr  pointer to xport interface
 * @param[in]  state   link state to notify
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_scan_notif_list_and_notify
(
  glink_transport_if_type *if_ptr,
  glink_link_state_type    state
)
{
  glink_link_notif_data_type *link_notif_data;

  glink_os_cs_acquire(&glink_link_notif_list_cs);
  
  for (link_notif_data = smem_list_first(&glink_link_notif_list);
       link_notif_data != NULL;
       link_notif_data = smem_list_next(link_notif_data))
  {
    if( link_notif_data->remote_ss == NULL || 
        0 == glink_os_string_compare( if_ptr->glink_core_priv->remote_ss,
                                      link_notif_data->remote_ss ) ) 
    {
      glinki_check_xport_and_notify(if_ptr,
                                    link_notif_data,
                                    (uint32)state,
                                    NULL);
    }
  }
  
  glink_os_cs_release(&glink_link_notif_list_cs);
} /* glinki_scan_notif_list_and_notify */

/*===========================================================================
  FUNCTION      glinki_ssr
===========================================================================*/
/** 
 * Notify all the xports of ssr event in this edge
 *
 * @param[in]    remote_ss name of remote subsystem
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_ssr(const char* remote_ss)
{
  glinki_xports_for_each(remote_ss,
                         glinki_notify_xport_ssr,
                         NULL,
                         0,
                         NULL);
}

/*===========================================================================
  FUNCTION      glinki_xport_linkup
===========================================================================*/
/** 
 * Check whether this transport is in linkup state or not
 *
 * @param[in]    if_ptr     transport interface pointer
 * 
 * @return       TRUE   if this xport is in link up state
 *               FALSE  otherwise
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
boolean glinki_xport_linkup
(
  glink_transport_if_type *if_ptr
)
{
  return if_ptr->glink_core_priv->status == GLINK_XPORT_LINK_UP;
}

/*===========================================================================
  FUNCTION      glinki_xports_for_each
===========================================================================*/
/** 
 * Scan all the transports in given edge and perform client's function for each
 * transport
 *
 * @param[in]    remote_ss    name of remote subsystem, NULL string not accepted
 * @param[in]    client_ex_fn client function to perform on each xport
 * @param[in]    param1       first parameter to use in client_ex_fn
 * @param[in]    param2       second parameter to use in client_ex_fn
 * @param[out]   out          value to return in case client wants
 *
 * @return       None.
 *
 * @sideeffects  None.
 *
 * This routine ASSUMES it is safe to traverse the glink_registered_transports 
 * without a lock. This is only true if the following conditions are met
 * 1) Only append/traverse operations are allowed on this list (No insert, delete)
 * 2) Append should make sure at no instance of time we have an uninitialized 
 * pointer(next) along the list
 */
/*=========================================================================*/
void glinki_xports_for_each
(
  const char         *remote_ss,
  glink_client_ex_fn  client_ex_fn,
  void               *param1,
  uint32              param2,
  void              **out
)
{
  glink_transport_if_type *if_iter_ptr;
  
  uint32 remote_host = glinki_find_remote_host(remote_ss);
  
  ASSERT(remote_host < GLINK_NUM_HOSTS);
  
  
  for(if_iter_ptr = smem_list_first(&glink_registered_transports[remote_host]);
      if_iter_ptr != NULL;
      if_iter_ptr = smem_list_next(if_iter_ptr))
  {
    client_ex_fn(if_iter_ptr, param1, param2, out);
  }
}

/*===========================================================================
  FUNCTION      glinki_xports_find
===========================================================================*/
/** 
 * Scan all the transports in given edge and finds transport that satisfy
 * client's condition
 *
 * @param[in]    remote_ss      name of remote subsystem, NULL string not accepted
 * @param[in]    client_cond_fn client function to check if this transport is 
 *                              what client is looking for
 * @param[in]    cond1          first condition to use in client_ex_fn
 * @param[in]    cond2          second condition to use in client_ex_fn
 * @param[out]   out            value to return in case client wants
 *
 * @return       pointer to glink_transport_if_type struct
 *               NULL if there isn't any xport matches client's search condition
 *
 * @sideeffects  None.
 *
 * This routine ASSUMES it is safe to traverse the glink_registered_transports 
 * without a lock. This is only true if the following conditions are met
 * 1) Only append/traverse operations are allowed on this list (No insert/delete)
 * 2) Append should make sure at no instance of time we have an uninitialized 
 * pointer(next) along the list
 */
/*=========================================================================*/
glink_transport_if_type *glinki_xports_find
(
  const char           *remote_ss,
  glink_client_cond_fn  client_cond_fn,
  void                 *cond1,
  uint32                cond2,
  void                **out
)
{
  glink_transport_if_type *if_iter_ptr;
  
  uint32 remote_host = glinki_find_remote_host(remote_ss);
  
  ASSERT(remote_host < GLINK_NUM_HOSTS);
  
  for (if_iter_ptr = smem_list_first(&glink_registered_transports[remote_host]);
       if_iter_ptr != NULL;
       if_iter_ptr = smem_list_next(if_iter_ptr))
  {
    if(client_cond_fn(if_iter_ptr, cond1, cond2, out))
    {
      break;
    }
  }
  
  return if_iter_ptr;
}

/*===========================================================================
  FUNCTION      glinki_find_ch_ctx_by_lcid
===========================================================================*/
/** 
 * Find channel context by lcid
 *
 * @param[in]    xport_ctx  Pointer to transport private context
 * @param[in]    lcid       local channel ID
 *
 * @return       pointer to glink channel context
 *
 * @sideeffects  This function needs to be protected by channel_q_cs
 *               Caller is responsible grab/release mutex when calling this
 */
/*=========================================================================*/
glink_channel_ctx_type *glinki_find_ch_ctx_by_lcid
(
  glink_core_xport_ctx_type *xport_ctx,
  uint32                     lcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  
  for (open_ch_ctx = smem_list_first(&xport_ctx->open_list);
       open_ch_ctx;
       open_ch_ctx = smem_list_next(open_ch_ctx))
  {
    if (open_ch_ctx->lcid == lcid)
    {
      return open_ch_ctx;
    }
  }
  
  /* open_ch_ctx will be NULL here. Using this instead of ASSERT(0)
    to avoid unreachable code warning */
  ASSERT( open_ch_ctx != NULL );
  return NULL;
}

/*===========================================================================
  FUNCTION      glinki_find_ch_ctx_by_rcid
===========================================================================*/
/** 
 * Find channel context by rcid
 *
 * @param[in]    xport_ctx  Pointer to transport private context
 * @param[in]    rcid       remote channel ID
 *
 * @return       pointer to glink channel context
 *
 * @sideeffects  This function needs to be protected by channel_q_cs
 *               Caller is responsible grab/release mutex when calling this
 */
/*=========================================================================*/
glink_channel_ctx_type *glinki_find_ch_ctx_by_rcid
(
  glink_core_xport_ctx_type *xport_ctx,
  uint32                     rcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  
  for (open_ch_ctx = smem_list_first(&xport_ctx->open_list);
       open_ch_ctx;
       open_ch_ctx = smem_list_next(open_ch_ctx))
  {
    if (open_ch_ctx->rcid == rcid)
    {
      return open_ch_ctx;
    }
  }
  
  /* open_ch_ctx will be NULL here. Using this instead of ASSERT(0)
    to avoid unreachable code warning */
  ASSERT( open_ch_ctx != NULL ); 
  return NULL;
}

/*===========================================================================
  FUNCTION      glinki_find_ch_ctx_by_name
===========================================================================*/
/** 
 * Find channel context by channel name, called by local/remote open function
 * This function will also indicate (valid_open_call) if this open call would
 * be valid or not
 *
 * @param[in]    xport_ctx  Pointer to transport private context
 * @param[in]    ch_name    channel name
 * @param[in]    local_open       flag to indicate this is local open call
 * @param[out]   valid_open_call  tell whether this open call would be valid
 *                                or not
 *
 * @return       pointer to glink channel context
 *               NULL if channel doesn't exist
 *
 * @sideeffects  This function needs to be protected by channel_q_cs
 *               Caller is responsible grab/release mutex when calling this
 */
/*=========================================================================*/
glink_channel_ctx_type *glinki_find_ch_ctx_by_name
(
  glink_core_xport_ctx_type *xport_ctx,
  const char                *ch_name,
  boolean                    local_open,
  boolean                   *valid_open_call
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_channel_ctx_type *ch_ctx_found = NULL;
  glink_remote_state_type remote_state;
  glink_local_state_type  local_state;
  
  ASSERT( valid_open_call != NULL );
  *valid_open_call = TRUE;
  
  for(open_ch_ctx = smem_list_first(&xport_ctx->open_list);
        open_ch_ctx != NULL;
      open_ch_ctx = smem_list_next(open_ch_ctx))
  {
    remote_state = open_ch_ctx->remote_state;
    local_state = open_ch_ctx->local_state;
    
    if ( 0 != glink_os_string_compare(open_ch_ctx->name, ch_name) ||
         remote_state == GLINK_REMOTE_CH_CLEANUP ||
         remote_state == GLINK_REMOTE_CH_SSR_RESET )
    {
      continue;
    }
    
    if ( ( local_open == TRUE && local_state == GLINK_LOCAL_CH_INIT ) || 
         ( local_open == FALSE && remote_state == GLINK_REMOTE_CH_INIT ) )
    {
      /* Local/Remote side already opened channel */
      ch_ctx_found = open_ch_ctx;
    }
    else if ( ( local_open == TRUE && local_state != GLINK_LOCAL_CH_CLOSED ) ||
              ( local_open == FALSE && remote_state != GLINK_REMOTE_CH_CLOSED ) )
    {
      /* Local/Remote side is trying to open channel without closing old one */
      *valid_open_call = FALSE;
      break;
    }
  }
  
  return ch_ctx_found;
}

/*===========================================================================
  FUNCTION      glinki_find_remote_host
===========================================================================*/
/** 
 * return remote subsystem ID based on remote subsystem name
 *
 * @param[in]    remote_ss  remote subsystem name
 *
 * @return       remote subsystem ID
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
uint32 glinki_find_remote_host
(
  const char *remote_ss
)
{
  uint32 remote_host;

  for(remote_host = 0; remote_host < GLINK_NUM_HOSTS; remote_host++) 
  {
    if( 0 == glink_os_string_compare(glink_hosts_supported[remote_host],
                                     remote_ss))
    {
      /* Match found, break out of loop */
      break;
    }
  }
  
  return remote_host;
}

/*===========================================================================
  FUNCTION      glinki_find_best_xport
===========================================================================*/
/** 
 * This function gives best available transport for give edge
 *
 * @param[in]    remote_host  Index into glink_registered_transports array of
 *                            registered transports list per edge
 * 
 * @return       pointer to glink_transport_if_type
 *               Null, if transport not found
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_transport_if_type *glinki_find_best_xport
(
  const char *remote_ss
)
{
  glink_transport_if_type *best_if_ptr = NULL;
  glink_xport_priority priority = GLINK_MIN_PRIORITY;
  
  glinki_xports_for_each(remote_ss,
                         glinki_update_best_xport,
                         (void *)&priority,
                         0,
                         (void **)&best_if_ptr);
  
  return best_if_ptr;
}

/*===========================================================================
  FUNCTION      glinki_find_requested_xport
===========================================================================*/
/** 
 * Find requested or best transport depending on client's request
 *
 * @param[in]    xport_name         name of transport
 * @param[in]    remote_ss          remote subsystem name
 * @param[in]    open_ch_option     option client gave when called glink_open
 * @param[out]   suggested_priority best xport priority glink suggests
 *
 * @return       pointer to glink_transport_if_type struct
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_transport_if_type *glinki_find_requested_xport
(
  const char           *xport_name,
  const char           *remote_ss,
  uint32                open_ch_option,
  glink_xport_priority *suggested_priority
)
{
  glink_transport_if_type *best_xport = glinki_find_best_xport(remote_ss);
  glink_transport_if_type *xport_found = NULL;
  
  *suggested_priority = GLINK_INVALID_PRIORITY;

  if (!xport_name)
  {
    if (best_xport)
    {
      *suggested_priority = best_xport->glink_priority;
    }
    
    return best_xport;
  }
  
  xport_found = glinki_xports_find(remote_ss,
                                   glinki_client_requested_xport_check,
                                   (void *)xport_name,
                                   0,
                                   NULL);
  
  if (!xport_found)
  {
    return NULL;
  }
  
  if ((open_ch_option & GLINK_OPT_INITIAL_XPORT) != 0)
  {
    *suggested_priority = best_xport->glink_priority;
  }
  else
  {
    /* Client is not willing to migrate to better transport */
    *suggested_priority = xport_found->glink_priority;
  }
  
  return xport_found;
}

/*===========================================================================
  FUNCTION      glinki_find_xport_by_priority
===========================================================================*/
/** 
 * This function returns glink_transport_if pointer based on transport priority
 *
 * @param[in]    prio        glink xport prio
 * @param[in]    remote_ss   remote subsytem name
 *
 * @return       pointer to glink_transport_if_type struct
 *               NULL, if it's not registered or transport not found with
 *                     the priority
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_transport_if_type *glinki_find_xport_by_priority
(
  glink_xport_priority  prio,
  const char           *remote_ss
)
{
  return glinki_xports_find(remote_ss,
                            glinki_xport_priority_comp,
                            NULL,
                            (uint32)prio,
                            NULL);
}

/*===========================================================================
  FUNCTION      glinki_enqueue_item
===========================================================================*/
/** 
 * Enqueue item to smem list in protected context
 * 
 * @param[in]    smem_list  smem list to enqueue
 * @param[in]    item       item to queue
 * @param[in]    cs         mutex to protect the list
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_enqueue_item
(
  smem_list_type *smem_list_ptr,
  void           *item,
  os_cs_type     *cs
)
{
  glink_os_cs_acquire(cs);
  smem_list_append(smem_list_ptr, item);
  glink_os_cs_release(cs);
}

/*===========================================================================
  FUNCTION      glinki_dequeue_item
===========================================================================*/
/** 
 *  Dequeue item from smem list in protected context
 * 
 * @param[in]    smem_list  smem list to dequeue from 
 * @param[in]    item       item to dequeue
 * @param[in]    cs         mutex to protect the list
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_dequeue_item
(
  smem_list_type *smem_list_ptr,
  void           *item,
  os_cs_type     *cs
)
{
  glink_os_cs_acquire(cs);
  smem_list_delete(smem_list_ptr, item);
  glink_os_cs_release(cs);
}

/*===========================================================================
  FUNCTION      glinki_acquire_edge_lock
===========================================================================*/
/** 
 *  Acquires the transport_q_cs lock for the specified edge
 * 
 * @param[in]    remote_ss  Name of the remote sub system on the edge
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_acquire_edge_lock(const char *ss_name)
{
    uint32 remote_host;
    remote_host = glinki_find_remote_host(ss_name);
    glink_os_cs_acquire(&glink_transport_q_cs[remote_host]);
}

/*===========================================================================
  FUNCTION      glinki_release_edge_lock
===========================================================================*/
/** 
 *  Releases the transport_q_cs lock for the specified edge
 * 
 * @param[in]    remote_ss  Name of the remote sub system on the edge
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_release_edge_lock(const char *ss_name)
{
    uint32 remote_host;
    remote_host = glinki_find_remote_host(ss_name);
    glink_os_cs_release(&glink_transport_q_cs[remote_host]);
}

#if defined(GLINK_MEMORY_LOGGING)
/* ============ Internal Logging API ================ */
void glink_mem_log
(
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
#endif

#ifdef FEATURE_TRACER_PACKET
/*===========================================================================
FUNCTION      glink_tracer_packet_log_pctx_pkt
===========================================================================*/
/** 
 * Log tracer packet event. Tracer packet is included in glink_core_tx_pkt
 * and needs to use vprovider to extract it
 * 
 * @param[in]  pctx     pointer to glink_core_tx_pkt_type
 * @param[in]  event_id event_id
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void glink_tracer_packet_log_pctx_pkt
(
  glink_core_tx_pkt_type  *pctx,
  uint32                   event_id
)
{
  tracer_pkt_result_type  tracer_pkt_log_result;
  void                   *tracer_pkt_data;
  size_t                  tracer_pkt_size;
  
  tracer_pkt_data = pctx->vprovider(pctx->iovec, 0, &tracer_pkt_size);
  
  if (tracer_pkt_size != pctx->size)
  {
    GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_TXV_INVALID_BUFFER, "", "", "",
                    tracer_pkt_size);
  }

  tracer_pkt_log_result = tracer_packet_log_event(tracer_pkt_data, (uint16)event_id);
  if (tracer_pkt_log_result != TRACER_PKT_STATUS_SUCCESS)
  {
    GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_TRACER_PKT_FAILURE, "", "", "",
                    tracer_pkt_log_result);
  }
}
#endif

/*===========================================================================
  FUNCTION      glinki_update_logging_filter
===========================================================================*/
/** 
 *  Update/Reset the logging filter if the name and remote host of the  
 *  logging filter matches to that of the passed channel context
 * 
 * @param[in]    chnl_ctx   Channel content to match/compare
 * @param[in]    reset      Indicate Update(FALSE) or Reset(TRUE)
 *
 * @return       None.
 */
/*=========================================================================*/
#if defined(GLINK_OS_DEFINED_LOGGING) || defined(GLINK_MEMORY_LOGGING)
void glinki_update_logging_filter
(
  glink_channel_ctx_type  *chnl_ctx, 
  boolean                 reset
)
{
  if ( ( log_filter_cfg.ch_filter_status == TRUE )  && 
       ( (chnl_ctx) != NULL )   && 
       ( glink_os_string_compare( (chnl_ctx)->name,
                                 log_filter_cfg.ch_name) == 0 ) &&
       ( log_filter_cfg.remote_host ==
         glinki_find_remote_host((chnl_ctx)->if_ptr->glink_core_priv->remote_ss) ) 
     )
  {
    if (reset == FALSE) /* Update the Filter */
    {
      log_filter_cfg.ch_ctx = (chnl_ctx);
      log_filter_cfg.ch_lcid = (chnl_ctx)->lcid;
      log_filter_cfg.ch_rcid = (chnl_ctx)->rcid;
    }
    else  /* Reset the Filter */
    {
      log_filter_cfg.ch_ctx = NULL;
      log_filter_cfg.ch_lcid = 0;
      log_filter_cfg.ch_rcid = 0;
      log_filter_cfg.remote_host = (uint32)-1;        
    }
  }
}

#else
void glinki_update_logging_filter(glink_channel_ctx_type *chnl_ctx, boolean reset)
{
    GLINK_OS_UNREFERENCED_PARAM( chnl_ctx );
    GLINK_OS_UNREFERENCED_PARAM( reset );
}
#endif
