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

#ifndef GLINK_INTERNAL_H
#define GLINK_INTERNAL_H

/*===========================================================================

                    GLink Core Internal Header File

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_list.h"
#include "glink.h"
#include "glink_os_utils.h"
#include "glink_core_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        FEATURE DEFINITIONS
===========================================================================*/
#define GLINK_VERSION 0
#define GLINK_FEATURE_FLAGS 0
#define GLINK_NUM_HOSTS     7

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#define GLINK_LOG_EVENT(type, ch_name, xport, remote_ss, param) \
  glink_mem_log(__FUNCTION__, __LINE__, type, ch_name, xport, remote_ss, param);

#define GLINK_MEM_LOG_SIZE 128

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
typedef enum {
  GLINK_EVENT_INIT,
  GLINK_EVENT_REGISTER_XPORT,
  GLINK_EVENT_CH_OPEN,
  GLINK_EVENT_CH_CLOSE,
  GLINK_EVENT_CH_TX,
  GLINK_EVENT_CH_Q_RX_INTENT,
  GLINK_EVENT_CH_RX_DONE,
  GLINK_EVENT_LINK_UP,
  GLINK_EVENT_RX_CMD_VER,
  GLINK_EVENT_RM_CH_OPEN,
  GLINK_EVENT_CH_OPEN_ACK,
  GLINK_EVENT_CH_CLOSE_ACK,
  GLINK_EVENT_CH_REMOTE_CLOSE,
  GLINK_EVENT_CH_STATE_TRANS,
  GLINK_EVENT_CH_INTENT_PUT,
  GLINK_EVENT_CH_RX_DATA,
  GLINK_EVENT_CH_RX_DATA_FRAG,
  GLINK_EVENT_CH_GET_PKT_CTX,
  GLINK_EVENT_CH_PUT_PKT_CTX,
  GLINK_EVENT_CH_TX_DONE,
  GLINK_EVENT_CH_SIG_SET,
  GLINK_EVENT_CH_SIG_L_GET,
  GLINK_EVENT_CH_SIG_R_GET,
  GLINK_EVENT_CH_NO_MIGRATION,
  GLINK_EVENT_CH_MIGRATION_IN_PROGRESS,
  GLINK_EVENT_XPORT_INTERNAL,
  GLINK_EVENT_TRACER_PKT_FAILURE,
  GLINK_EVENT_TXV_INVALID_BUFFER
}glink_log_event_type;

typedef struct _glink_channel_intents_type {

  /* Link for a channel in Tx queue */
  struct _glink_channel_intents_type* next;

  /* Critical section to protest access to intent queues */
  os_cs_type                    intent_q_cs;

  /* Event that glink_tx waits on and blocks until remote rx_intents
  * are available */
  os_event_type                 rm_intent_avail_evt;

  /* Size of requested intent that this channel wait on */
  size_t                        rm_intent_req_size;

  /* Linked list of remote Rx intents. Data can be transmitted only when
  * remote intents are available */
  smem_list_type                remote_intent_q;

  /* Linked list of remote Rx intents which local GLink core has used to
  * transmit data, and are pending Tx complete */
  smem_list_type                remote_intent_pending_tx_q;

  /* Linked list of local Rx intents. Data can be received only when
  * local intents are available */
  smem_list_type                local_intent_q;

  /* Linked list of remote Rx intents held by the clients */
  smem_list_type                local_intent_client_q;

  /* Read intent being gathered */
  glink_rx_intent_type          *cur_read_intent;

  /* Linked list of Tx packets in the order they where submitted by
   * the channel owner */
  smem_list_type                tx_pkt_q;

} glink_channel_intents_type;

struct glink_channel_ctx {
  /* Link needed for use with list APIs.  Must be at the head of the struct */
  smem_list_link_type                 link;

  /* Channel name */
  char                                name[GLINK_CH_NAME_LEN];

  /* Local channel ID */
  uint32                              lcid;

  /* Remote Channel ID */
  uint32                              rcid;

  /* Local Channel state */
  glink_local_state_type              local_state;

  /* Remote channel state */
  glink_remote_state_type             remote_state;

  /* Critical section to protect channel states */
  os_cs_type                          ch_state_cs;

  /* Channel local control signal state */
  uint32                              local_sigs;

  /* Channel remote control signal state */
  uint32                              remote_sigs;

  /* Critical section to protect tx operations */
  os_cs_type                          tx_cs;

  /* channel intent collection */
  glink_channel_intents_type          *pintents;

  /* Interface pointer with with this channel is registered */
  glink_transport_if_type             *if_ptr;

  /** Private data for client to maintain context. This data is passed back
   * to client in the notification callbacks */
  const void                          *priv;

  /** Data receive notification callback */
  glink_rx_notification_cb            notify_rx;

  /** Tracer Packet data receive notification callback */
  glink_rx_tracer_pkt_notification_cb notify_rx_tracer_pkt;

  /** Vector receive notification callback */
  glink_rxv_notification_cb           notify_rxv;

  /** Data transmit notification callback */
  glink_tx_notification_cb            notify_tx_done;

  /** GLink channel state notification callback */
  glink_state_notification_cb         notify_state;

  /** Intent request from the remote side */
  glink_notify_rx_intent_req_cb       notify_rx_intent_req;

  /** New intent arrival from the remote side */
  glink_notify_rx_intent_cb           notify_rx_intent;

  /** Control signal change notification - Invoked when remote side
   *  alters its control signals */
  glink_notify_rx_sigs_cb             notify_rx_sigs;

  /** rx_intent abort notification. This callback would be invoked for
  *  every rx_intent that is queued with GLink core at the time the
  *  remote side or local side decides to close the port. Optional */
  glink_notify_rx_abort_cb            notify_rx_abort;

  /** tx abort notification. This callback would be invoked if client
  *   had queued a tx buffer with glink and it had not been transmitted i.e.
  *   tx_done callback has not been called for this buffer and remote side
  *   or local side closed the port. Optional */
  glink_notify_tx_abort_cb            notify_tx_abort;

  /* flag to check if channel is marked for deletion
   * This is workaround to prevent channel migration algorithm from finding channel
   * which should be closed but has not been closed yet. This case occurs when glink_close
   * is called for closing channel on initial xport and it is being opened on other xport.
   * This may lead to remote side opening channel on neogitated xport from which local side
   * will get remote open again. In this case channel to be closed will be found for negotiation
   * on initial xport again and channel migration algorithm will be triggered(again)  */
  boolean                             tag_ch_for_close;

  /* save glink open config options */
  uint32                              ch_open_options;
};


typedef struct _glink_mem_log_entry_type {
  const char *func;
  uint32 line;
  glink_log_event_type type;
  const char *msg;
  const char *xport;
  const char *remote_ss;
  uint32 param;
} glink_mem_log_entry_type;

/* Structure to store link notification callbacks */
typedef struct {
  /* Link needed for use with list APIs.  Must be at the head of the struct */
  smem_list_link_type       link;

  const char*               xport;         /* NULL = any transport */
  const char*               remote_ss;     /* NULL = any subsystem */
  glink_link_state_notif_cb link_notifier; /* Notification callback */
  void                      *priv;         /* Notification priv ptr */
} glink_link_notif_data_type;

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/
extern os_cs_type *glink_transport_q_cs[GLINK_NUM_HOSTS];
extern const char *glink_hosts_supported[GLINK_NUM_HOSTS];
extern smem_list_type glink_registered_transports[];

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

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
);

/*===========================================================================
FUNCTION      glink_rx_cmd_version

DESCRIPTION   Receive transport version for remote-initiated version
              negotiation

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            version   Remote Version

            features  Remote Features

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_version
(
  glink_transport_if_type *if_ptr,
  uint32                  version,
  uint32                  features
);

/*===========================================================================
FUNCTION      glink_rx_cmd_version_ack

DESCRIPTION   Receive ACK to previous glink_transport_if::tx_cmd_version
              command

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            version   Remote Version

            features  Remote Features

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_version_ack
(
  glink_transport_if_type *if_ptr,
  uint32                  version,
  uint32                  features
);

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_remote_open

DESCRIPTION   Receive remote channel open request; Calls
              glink_transport_if:: tx_cmd_ch_remote_open_ack as a result

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            rcid      Remote Channel ID

            *name     String name for logical channel

            prio      requested xport priority from remote side

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_ch_remote_open
(
  glink_transport_if_type *if_ptr,
  uint32                  rcid,
  const char              *name,
  glink_xport_priority    prio
);

/*===========================================================================
FUNCTION      glink_rx_cmd_ch_open_ack

DESCRIPTION   This function is invoked by the transport in response to
              glink_transport_if:: tx_cmd_ch_open

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge

            lcid      Local Channel ID

            prio      Negotiated xport priority obtained from remote side
                      after negotiation happened on remote side

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void glink_rx_cmd_ch_open_ack
(
  glink_transport_if_type *if_ptr,
  uint32                  lcid,
  glink_xport_priority    prio
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
                           CHANNEL CONTEXT APIS
===========================================================================*/

/*===========================================================================
FUNCTION      glinki_ch_is_fully_opened

DESCRIPTION   Determine if both the local and remote channel state is fully
              open

ARGUMENTS     *cfg_ptr   - Pointer to channel context

RETURN VALUE  True if fully opened, false otherwise.

SIDE EFFECTS  None
===========================================================================*/
boolean glinki_ch_is_fully_opened
(
  glink_channel_ctx_type *ctx
);


/*===========================================================================
FUNCTION      glinki_ch_push_local_rx_intent

DESCRIPTION   Create and push a local receive intent to internal list

ARGUMENTS     *cfg_ptr   - Pointer to channel context

              *pkt_priv  - Client-provided opaque data

              size       - Size of Receive Intent

RETURN VALUE  Pointer to the new intent

SIDE EFFECTS  None
===========================================================================*/
glink_rx_intent_type* glinki_ch_push_local_rx_intent
(
  glink_channel_ctx_type *ctx,
  const void             *pkt_priv,
  size_t                 size
);

/*===========================================================================
FUNCTION      glinki_ch_get_local_rx_intent

DESCRIPTION   Lookup a local receive intent

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              liid        - Local Receive Intent ID

RETURN VALUE  Pointer to the intent or NULL if not match is found.

SIDE EFFECTS  None
===========================================================================*/
glink_rx_intent_type* glinki_ch_get_local_rx_intent
(
  glink_channel_ctx_type *ctx,
  uint32                 liid
);

/*===========================================================================
FUNCTION      glinki_ch_remove_local_rx_intent

DESCRIPTION   Removes Local Receive Intent ID

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              liid        - Local Receive Intent ID

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glinki_ch_remove_local_rx_intent
(
  glink_channel_ctx_type *ctx,
  uint32                 liid
);

/*===========================================================================
FUNCTION      glinki_ch_set_local_rx_intent_notified

DESCRIPTION   Sets the state of the intent as client-notified

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              *intent_ptr - Pointer to the receive intent

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glinki_ch_set_local_rx_intent_notified
(
  glink_channel_ctx_type *ctx,
  glink_rx_intent_type   *intent_ptr
);

/*===========================================================================
FUNCTION      glinki_ch_get_local_rx_intent_notified

DESCRIPTION   Lookup a Local Receive Intent ID that is in the client-notified
              state

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              *ptr        - Data pointer of receive buffer from client
                            (passed in through glink_rx_done)

RETURN VALUE  Pointer to the intent or NULL if not match is found.

SIDE EFFECTS  None
===========================================================================*/
glink_rx_intent_type* glinki_ch_get_local_rx_intent_notified
(
  glink_channel_ctx_type *ctx,
  void                   *ptr
);

/*===========================================================================
FUNCTION      glinki_ch_remove_local_rx_intent_notified

DESCRIPTION   Removes the Local Receive Intent

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              *intent_ptr - Pointer to the receive intent

RETURN VALUE  New channel context or NULL

SIDE EFFECTS  None
===========================================================================*/
glink_channel_ctx_type* glinki_ch_remove_local_rx_intent_notified
(
  glink_channel_ctx_type *ctx,
  glink_rx_intent_type   *intent_ptr
);

/*===========================================================================
FUNCTION      glinki_ch_push_remote_rx_intent

DESCRIPTION   Adds a new Remote Receive Intent

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              size        - Size of the Remote Receive Intent

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glinki_ch_push_remote_rx_intent
(
  glink_channel_ctx_type *ctx,
  size_t                 size
);

/*===========================================================================
FUNCTION      glinki_ch_pop_remote_rx_intent

DESCRIPTION   Removes a Remote Receive Intent

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              size        - Size of the Remote Receive Intent

              *riid_ptr   - Pointer to the Remote Receive Intent

RETURN VALUE  Standard GLink Err code.

SIDE EFFECTS  None
===========================================================================*/
glink_err_type glinki_ch_pop_remote_rx_intent
(
  glink_channel_ctx_type *ctx,
  size_t                size,
  uint32                *riid_ptr
);

/*===========================================================================
FUNCTION      glinki_ch_get_tx_pending_remote_done

DESCRIPTION   Lookup packet transmit context for a packet that is waiting for
              the remote-done notification.

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              riid        - Remote Receive Intent ID

RETURN VALUE  Pointer to the transmit packet context or NULL if not match is
              found.

SIDE EFFECTS  None
===========================================================================*/
glink_core_tx_pkt_type* glinki_ch_get_tx_pending_remote_done
(
  glink_channel_ctx_type *ctx,
  uint32                 riid
);

/*===========================================================================
FUNCTION      glinki_ch_remove_tx_pending_remote_done

DESCRIPTION   Removes a packet transmit context for a packet that is waiting
              for the remote-done notification.

ARGUMENTS     *cfg_ptr    - Pointer to channel context

              *tx_pkt     - Pointer to the packet context to remove

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glinki_ch_remove_tx_pending_remote_done
(
  glink_channel_ctx_type       *ctx,
  glink_core_tx_pkt_type       *tx_pkt
);

/*===========================================================================
  FUNCTION      glinki_add_ch_to_xport
===========================================================================*/
/**
 * Add remote/local channel context to xport open channel queue
 *
 * @param[in]    if_ptr            Pointer to xport if on which channel is to
 *                                 be opened
 * @param[in]    ch_ctx            channel context
 * @param[out]   allocated_ch_ctx  Pointer to channel context pointer
 * @param[in]    local_open        flag to determine if channel is opened
 *                                 locally or remotely
 * @param[in]    prio              negotiated xport priority
 *                                 (used to send priority via remote_open_ack to
 *                                  remote side)
 *
 * @return       pointer to glink_transport_if_type struct
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
  glink_xport_priority      prio
);

void glink_mem_log
(
  const char *func,
  uint32 line,
  glink_log_event_type type,
  const char *msg,
  const char *xport,
  const char *remote_ss,
  uint32 param
);

/*===========================================================================
FUNCTION      glink_core_setup
===========================================================================*/
/**

  Initializes internal core functions based on the transport capabilities.

  @param[in]  if_ptr   The Pointer to the interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_setup(glink_transport_if_type *if_ptr);

/*===========================================================================
FUNCTION      glink_core_get_default_interface
===========================================================================*/
/**

  Provides default core interface.

  @return     Pointer to the default core interface.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_core_if_type* glink_core_get_default_interface(void);

/*===========================================================================
FUNCTION      glink_core_setup_full_xport
===========================================================================*/
/**

  Initializes internal core functions for the full-featured transport.

  @param[in]  if_ptr   The Pointer to the interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_setup_full_xport(glink_transport_if_type *if_ptr);

/*===========================================================================
FUNCTION      glink_core_setup_intentless_xport
===========================================================================*/
/**

  Initializes internal core functions for the intentless transport.

  @param[in]  if_ptr   The Pointer to the interface instance.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void glink_core_setup_intentless_xport(glink_transport_if_type *if_ptr);

#endif /* GLINK_INTERNAL_H */
