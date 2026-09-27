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

#ifndef GLINK_CORE_IF_H
#define GLINK_CORE_IF_H

/**
 * @file glink_core_if.h
 *
 * Public API for the transport abstraction layer to call into GLINK Core
 */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_list.h"
#include "glink.h"
#include "glink_transport_if.h"
#include "glink_os_utils.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
/* Limit of the proportion of total QoS requests
   =GLINK_QOS_RATE_LIMIT_COEFF_N / GLINK_QOS_RATE_LIMIT_COEFF_D
 */
#define GLINK_QOS_RATE_LIMIT_COEFF_N 7
#define GLINK_QOS_RATE_LIMIT_COEFF_D 10

/* Number of QoS tokens given at refill */
#define GLINK_QOS_TOKENS (10) 

/* Number of QoS buckets */
#define GLINK_QOS_BUCKETS (1)

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
typedef struct glink_core_version glink_core_version_type;

/**
 * Transport status
 */
typedef enum {
  GLINK_XPORT_REGISTERED = 0,
  GLINK_XPORT_NEGOTIATING,
  GLINK_XPORT_LINK_UP,

}glink_transport_status_type;

struct glink_rx_intent {
  /* Link needed for use with list APIs.  Must be at the head of the struct */
  smem_list_link_type    link;

  void       *data;      /* Pointer to the data buffer to be transmitted */
  const void *pkt_priv;  /* Per packet private data */
  size_t     size;       /* Size of data buffer */
  size_t     used;       /* Actual valid data in *data field */
  size_t     pkt_sz;     /* Size of the packet */
  uint32     iid;        /* Intent ID */
  void       *iovec;     /* Pointer to the data buffer to be transmitted */
  boolean    tracer_pkt;  /* Specify if this intent is for tracer packet */
  glink_buffer_provider_fn vprovider; /* Buffer provider for virtual space */
  glink_buffer_provider_fn pprovider; /* Buffer provider for physical space */
};

/** Context of Tx activity for a transport */
typedef struct glink_tx_xport_ctx_s {
  os_event_type event;   /* Event to signal Tx thread */
  os_cs_type tx_q_cs;    /* Lock to protect Tx queue */
  smem_list_type tx_q;   /* Tx channel queue */
  uint32 qos_rate_sum;   /* Sum of rates of registered QoS requests */
} glink_tx_xport_ctx_type;

/** G-Link Local channel states */
typedef enum
{
  /** Initial State before entering channel state machine */
  GLINK_LOCAL_CH_INIT,
  
  /** Local G-Link channel is fully closed */
  GLINK_LOCAL_CH_CLOSED,

  /** Local G-Link channel opened channel and waiting for ACK from remote */
  GLINK_LOCAL_CH_OPENING,

  /** Local G-Link channel is fully opened */
  GLINK_LOCAL_CH_OPENED,

  /** Local G-Link channel closed the channel and waiting for ACK from remote */
  GLINK_LOCAL_CH_CLOSING

}glink_local_state_type;

/** G-Link Remote channel states */
typedef enum
{
  /** Initial State before entering channel state machine */
  GLINK_REMOTE_CH_INIT,

  /** Remote G-Link channel is closed */
  GLINK_REMOTE_CH_CLOSED,

  /** Remote G-Link channel is opened */
  GLINK_REMOTE_CH_OPENED,

  /* Glink channel state when SSR is received from remote sub-system */
  GLINK_REMOTE_CH_SSR_RESET,

  /** G-Link channel is pending cleanup. 
      This state is used for deferred channel cleanup in case 
      it sits in Tx scheduler queue */
  GLINK_REMOTE_CH_CLEANUP

}glink_remote_state_type;

/** Indicates that transport is now ready to start negotiation using the
 *  v0 configuration. */
typedef void (*link_up_fn)
(
  glink_transport_if_type *if_ptr    /* Pointer to the interface instance */
);

/** Receive transport version for remote-initiated version negotiation */
typedef void (*rx_cmd_version_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  version, /* Version */
  uint32                  features /* Features */
);

/** Receive ACK to previous glink_transport_if_type::tx_cmd_version command */
typedef void (*rx_cmd_version_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  version, /* Version */
  uint32                  features /* Features */
);

/** Receive remote channel open request; expected response is
 *  glink_transport_if_type:: tx_cmd_ch_remote_open_ack */
typedef void (*rx_cmd_ch_remote_open_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  const char              *name,   /* String name for the logical channel */
  glink_xport_priority    prio     /* priority of xport */
);

/** This function is invoked by the transport in response to
 *  glink_transport_if_type:: tx_cmd_ch_open */
typedef void (*rx_cmd_ch_open_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  lcid,    /* Local channel ID */
  glink_xport_priority    prio
);

/** This function is invoked by the transport in response to
 *  glink_transport_if_type:: tx_cmd_ch_close */
typedef void (*rx_cmd_ch_close_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  lcid     /* Local channel ID */
);

/** Remote channel close request; will result in sending
 *  glink_transport_if_type:: tx_cmd_ch_remote_close_ack */
typedef void (*rx_cmd_ch_remote_close_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid     /* Remote channel ID */
);

/** Transport invokes this call on receiving remote RX intent */
typedef void (*rx_cmd_remote_rx_intent_put_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  riid,    /* Remote intent ID */
  size_t                  size     /* Size of receive intent */
);

/** Get receive packet context (if successful, should be followed by call to
    rx_put_pkt_ctx) */
typedef glink_rx_intent_type* (*rx_get_pkt_ctx_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  liid     /* Local intent ID */
);

/** Receive a packet fragment (must have previously received an rx_cmd_rx_data
    packet). */
typedef void (*rx_put_pkt_ctx_fn)
(
  glink_transport_if_type *if_ptr,     /* Pointer to the interface instance */
  uint32                  rcid,        /* Remote channel ID */
  glink_rx_intent_type    *intent_ptr, /* Fragment ptr */
  boolean                 complete     /* True if pkt is complete */
);

/** Transport invokes this call to inform GLink that remote side is
 *  done with previous transmitted packet. */
typedef void (*rx_cmd_tx_done_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  uint32                  riid,    /* Remote intent ID */
  boolean                 reuse    /* Reuse intent */
);

/** Remote side is requesting an RX intent */
typedef void (*rx_cmd_remote_rx_intent_req_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  size_t                  size     /* Size of the requested intent */
);

/** ACK to RX Intent Request */
typedef void (*rx_cmd_rx_intent_req_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  boolean                 granted  /* True if RX Intent will be queued, false
                                      if request will not be granted. */
);

/** If transport was full and could not continue a transmit operation,
 *  then it will call this function to notify the core that it is ready to
 *  resume transmission. */
typedef void (*tx_resume_fn)
(
  glink_transport_if_type *if_ptr  /* Pointer to the interface instance */
);

/** Received 32-bit signals value from remote side. It is passed on to
 *  the client */
typedef void (*rx_cmd_remote_sigs_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  uint32                  rcid,       /* Remote channel ID */
  uint32                  remote_sigs /* Remote control signals */
);

/** Channel open config verification */
typedef boolean(*verify_open_cfg_fn)
(
  glink_channel_ctx_type   *ch_ctx    /* Pointer to the channel context */
);

/** Channel initialization */
typedef glink_err_type (*channel_init_fn)
(
  glink_channel_ctx_type *open_ch_ctx /* Pointer to the channel context */
);

/** Channel cleanup */
typedef void (*channel_cleanup_fn)
(
  glink_channel_ctx_type *open_ch_ctx /* Pointer to the channel context */
);

/** Channel receive pkt */
typedef void(*channel_receive_pkt_fn)
(
  glink_channel_ctx_type *open_ch_ctx,  /* Pointer to the channel context */
  glink_rx_intent_type *intent_ptr      /* Pointer to the intent context */
);

/** Channel submit pkt */
typedef glink_err_type(*channel_submit_pkt_fn)
(
  glink_channel_ctx_type *open_ch_ctx,  /* Pointer to the channel context */
  glink_core_tx_pkt_type *pctx,         /* Pointer to the packet context */
  boolean req_intent                    /* Request intent flag */
);


/** Transport specific data pointer that transport may choose fill in
 * with some data */
struct glink_core_xport_ctx
{
  /* Transport name */
  const char                    *xport;

  /* Remote subsystem name */
  const char                    *remote_ss;

  /** Keep track of version array index in use */
  const glink_core_version_type *version_array;

  /** Keep track of version array index in use */
  int32                         version_indx;

  /* Keeps track of the current status of the transport */
  glink_transport_status_type   status;

  /* critical section to change/access xport status
   * automically */
  os_cs_type                    status_cs;

  /* Transport's capabilities */
  uint32                        xport_capabilities;

  /* Max lcid */
  uint32                        max_lcid;

  /* Free lcid */
  uint32                        free_lcid;

  /* Keeps track of the open channels for this transport/edge combination */
  smem_list_type                open_list;

  /* Critical section to protect access to open_list and channel state of all 
     channels in that open_list */
  os_cs_type                    channel_q_cs;

  /* Local channel intents queued so far. This also helps determining liid
   * when client queues new rx intents locally */
  uint32                        liid;

  /* Critical section to protect access to liid allocation */
  os_cs_type                    liid_cs;

  /* Context of Tx activity for a transport */
  glink_tx_xport_ctx_type       *tx_ctx;

  /* channel open config verification */
  verify_open_cfg_fn            verify_open_cfg;

  /* channel init function */
  channel_init_fn               channel_init;

  /* channel cleanup function */
  channel_cleanup_fn            channel_cleanup;

  /* channel receive pkt */
  channel_receive_pkt_fn        channel_receive_pkt;

  /* channel submit pkt */
  channel_submit_pkt_fn         channel_submit_pkt;
};

/**
 * Data Structure for Transport abstraction layer to call into GLink Core
 * for logical channel control state update and data Tx/Rx notifications.
 */
struct glink_core_if
{
  /** Indicates that transport is now ready to start negotiation using the
   *  v0 configuration. */
  link_up_fn                     link_up;


  /** Receive transport version for remote-initiated version negotiation */
  rx_cmd_version_fn              rx_cmd_version;


  /** Receive ACK to previous glink_transport_if_type::tx_cmd_version command */
  rx_cmd_version_ack_fn          rx_cmd_version_ack;

  /** Receive remote channel open request; expected response is
   *  glink_transport_if_type:: tx_cmd_ch_remote_open_ack */
  rx_cmd_ch_remote_open_fn       rx_cmd_ch_remote_open;

  /** This function is invoked by the transport in response to
   *  glink_transport_if_type:: tx_cmd_ch_open */
  rx_cmd_ch_open_ack_fn          rx_cmd_ch_open_ack;

  /** This function is invoked by the transport in response to
   *  glink_transport_if_type:: tx_cmd_ch_close */
  rx_cmd_ch_close_ack_fn         rx_cmd_ch_close_ack;

  /** Remote channel close request; will result in sending
   *  glink_transport_if_type:: tx_cmd_ch_remote_close_ack */
  rx_cmd_ch_remote_close_fn      rx_cmd_ch_remote_close;

  /** Transport invokes this call on receiving remote RX intent */
  rx_cmd_remote_rx_intent_put_fn rx_cmd_remote_rx_intent_put;

  /** Get receive packet context (if successful, should be followed by call to
      rx_put_pkt_ctx) */
  rx_get_pkt_ctx_fn              rx_get_pkt_ctx;

  /** Receive a packet fragment (must have previously received an rx_cmd_rx_data
      packet). */
  rx_put_pkt_ctx_fn              rx_put_pkt_ctx;

  /** Transport invokes this call to inform GLink that remote side is
   *   done with previous transmitted packet. */
  rx_cmd_tx_done_fn              rx_cmd_tx_done;

  /** Remote side is requesting an RX intent */
  rx_cmd_remote_rx_intent_req_fn rx_cmd_remote_rx_intent_req;

  /** ACK to RX Intent Request */
  rx_cmd_rx_intent_req_ack_fn    rx_cmd_rx_intent_req_ack;

  /** Received 32-bit signals value from remote side. It is passed on to
   *  the client */
  rx_cmd_remote_sigs_fn          rx_cmd_remote_sigs;

  /** If transport was full and could not continue a transmit operation,
   *  then it will call this function to notify the core that it is ready to
   *  resume transmission. */
  tx_resume_fn                   tx_resume;
};

/** Feature negotiation function.  The version negotiation starts out using
 *  the full feature set from the features element and then calls this
 *  function with a subset passed into the features argument.  This
 *  function should filter passed in features with the supported feature
 *  set.  For simple cases, a bitwise AND can be used, but for more
 *  complicated cases (such as when features are mutually exclusive),
 *  this function enables a more complex negotiation sequence.
 */
typedef uint32 (*negotiate_features_fn)
(
  glink_transport_if_type       *if_ptr,      /* Pointer to the interface
                                                 instance */
  const glink_core_version_type *version_ptr, /* Version structure */
  uint32                        features      /* Subset of features based on
                                                 negotiation */
);

/**
 * Version structure
 */
struct glink_core_version {
  uint32                   version;      /* Version number */
  uint32                   features;     /* Set of features supported in
                                            this version */

  /** Feature negotiation function.  The version negotiation starts out using
   *  the full feature set from the features element and then calls this
   *  function with a subset passed into the features argument.  This
   *  function should filter passed in features with the supported feature
   *  set.  For simple cases, a bitwise AND can be used, but for more
   *  complicated cases (such as when features are mutually exclusive),
   *  this function enables a more complex negotiation sequence.
   */
  negotiate_features_fn    negotiate_features;
};

/**
 * Data Structure for Transport abstraction layer to call into GLink Core
 * while registering with GLink
 */
typedef struct {
  const char                    *name;      /* Name of the transport */
  const char                    *remote_ss; /* Remote host name */
  const glink_core_version_type *version;   /* Array of supported versions */
  size_t                        version_count; /* Number of elements in version[] */
  uint32                        max_cid;    /* Max channel ID supported by
                                               transport */
  uint32                        max_iid;    /* Max Rx intent ID supported by
                                               transport */
}glink_core_transport_cfg_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
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
);

#endif //GLINK_CORE_IF_H

