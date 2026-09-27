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

#ifndef GLINK_TRANSPORT_IF_H
#define GLINK_TRANSPORT_IF_H

/**
 * @file glink_transport_if.h
 *
 * Public API for the GLink Core to transport interface
 */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include <glink.h>
#include <smem_list.h>

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
/* GLink transport capability bit definitions */
/* Whether transport supports signalling */
#define GLINK_CAPABILITY_SIG_SUPPORT 0x00000001

/* Intentless mode of operation */
#define GLINK_CAPABILITY_INTENTLESS  0x00000002

/* Tracer packet support */
#define GLINK_CAPABILITY_TRACER_PKT  0x00000004

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/* forward declaration */
struct glink_transport_if;
typedef struct glink_transport_if   glink_transport_if_type;

struct glink_core_xport_ctx;
typedef struct glink_core_xport_ctx glink_core_xport_ctx_type;

struct glink_core_if;
typedef struct glink_core_if        glink_core_if_type;

struct glink_rx_intent;
typedef struct glink_rx_intent      glink_rx_intent_type;

struct glink_channel_ctx;
typedef struct glink_channel_ctx glink_channel_ctx_type;

/* Priority of transports registered with glink */
typedef enum {
  GLINK_XPORT_SMEM = 100,
  GLINK_XPORT_RPM = 100,
  GLINK_XPORT_SMD = 200,
  GLINK_XPORT_CP_ENGINE = 300,
  GLINK_XPORT_UART = 400,
  GLINK_MIN_PRIORITY = 100000,
  GLINK_INVALID_PRIORITY
} glink_xport_priority;

/** Glink core -> Xport pkt type definition */
typedef struct glink_core_tx_pkt_s {
  struct glink_core_tx_pkt_s *next; /* pointer to the next packet in list */

  void       *data;      /* Pointer to the data buffer to be transmitted */
  const void *pkt_priv;  /* Per packet private data */
  uint32     iid;        /* Remote intent ID being used to transmit data */
  size_t     size;       /* Size of data buffer */
  size_t     size_remaining; /* Size left to transmit */
  void       *iovec;      /* Pointer to the data buffer to be transmitted */
  boolean    tracer_pkt;  /* specify if this intent is for tracer packet */
  glink_buffer_provider_fn vprovider; /* Buffer provider for virtual space */
  glink_buffer_provider_fn pprovider; /* Buffer provider for physical space */
}glink_core_tx_pkt_type;

/** Transmit a version command for local negotiation. The transport would call
 *  glink_transport_if_type::rx_cmd_version_ack callback as a result */
typedef void (*tx_cmd_version_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  version, /* Version */
  uint32                  features /* Features */
);

/** Transmit a version ack for remote nogotiation */
typedef void (*tx_cmd_version_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  version, /* Version */
  uint32                  features /* Features */
);

/** Signals the negotiation is complete and transport can now do version
 *  specific initialization. Return uint32 transport capabilities bit field */
typedef uint32 (*set_version_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  version, /* Version */
  uint32                  features /* Features */
);

/** Sends OPEN command to transport for transport to do whatever it deems
 *  necessary to open a GLink logical channel.
 *  GLink Core expects glink_transport_if_type::rx_cmd_ch_open_ack to be called
 *  as a result of channel opening. */
typedef glink_err_type (*tx_cmd_ch_open_fn)
(
  glink_transport_if_type *if_ptr,/* Pointer to the interface instance */
  uint32                  lcid,   /* Local channel ID */
  const char              *name,  /* String name for the logical channel */
  glink_xport_priority    prio
);

/** Sends CLOSE command to transport for transport to do whatever it deems
 *  necessary to close the specified GLink logical channel.
 *  GLink Core expects glink_transport_if_type::rx_cmd_ch_close_ack to be
 *  called as a result of channel closing. */
typedef glink_err_type (*tx_cmd_ch_close_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                     lcid  /* Local channel ID */
);

/** Sends the remote open ACK command in response to receiving
 *  glink_core_if_type::rx_cmd_ch_remote_open */
typedef void (*tx_cmd_ch_remote_open_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid,    /* Remote channel ID */
  glink_xport_priority    prio
);

/** Sends the remote close ACK command in response to receiving
 *  glink_core_if_type::rx_cmd_ch_remote_close */
typedef void (*tx_cmd_ch_remote_close_ack_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  rcid     /* Remote channel ID */
);

/** Allocates a receive buffer for the local rx intent */
typedef glink_err_type (*allocate_rx_intent_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  size_t                  size,       /* Size of Rx intent */
  glink_rx_intent_type    *intent_ptr /* Intent ptr */
);

/** Deallocates a receive buffer for the local rx intent */
typedef glink_err_type (*deallocate_rx_intent_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  glink_rx_intent_type    *intent_ptr /* Intent ptr */
);

/** Send receive intent ID for a given channel */
typedef glink_err_type (*tx_cmd_local_rx_intent_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  lcid,    /* Local channel ID */
  size_t                  size,    /* Size of Rx intent */
  uint32                  liid     /* Local indent ID */
);

/** Send receive done command for an intent for the specified channel */
typedef void(*tx_cmd_local_rx_done_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  lcid,    /* Local channel ID */
  uint32                  liid,    /* Local indent ID */
  boolean                 reuse    /* Reuse indent */
);

/** Send a data packet to the transport to be transmitted over the specified
 *  channel */
typedef glink_err_type (*tx_fn)
(
  glink_transport_if_type *if_ptr, /* Pointer to the interface instance */
  uint32                  lcid,    /* Local channel ID */
  glink_core_tx_pkt_type  *pctx    /* Packet to be transmitted */
);

/** Send a intent request to the remote side to have it queue more rx intents
 *  to unblock this end's tx operation */
typedef glink_err_type (*tx_cmd_rx_intent_req_fn)
(
  glink_transport_if_type *if_ptr,   /* Pointer to the interface instance */
  uint32                  lcid,      /* Local channel ID */
  size_t                  size       /* Size of the requested intent */
);

/** Send the ACK back for the rx intent request send by the other side,
 *  specifynig whether the request was granted ot not */
typedef glink_err_type (*tx_cmd_remote_rx_intent_req_ack_fn)
(
  glink_transport_if_type *if_ptr,   /* Pointer to the interface instance */
  uint32                  lcid,      /* Local channel ID */
  boolean                 granted    /* Whether of not the intent request was
                                        granted */
);

/** Sets the local channel signals as per the specified 32-bit mask.
 *  Transport may also send the 32-bit value as-is to the remote side. */
typedef glink_err_type (*tx_cmd_set_sigs_fn)
(
  glink_transport_if_type *if_ptr,   /* Pointer to the interface instance */
  uint32                  lcid,      /* Local channel ID */
  uint32                  sigs       /* Whether of not the intent request was
                                        granted */
);

/** Lets the XAL know of SSR on that edge. */
typedef glink_err_type (*ssr_fn)
(
  glink_transport_if_type *if_ptr   /* Pointer to the interface instance */
);

/** Trigger a poll for any rx data / signals on transport. */
typedef glink_err_type (*poll_fn)
(
  glink_transport_if_type *if_ptr   /* Pointer to the interface instance */
);

/** Mask/Unmask rx interrupt associated with transport. */
typedef glink_err_type (*mask_rx_irq_fn)
(
  glink_transport_if_type *if_ptr,  /* Pointer to the interface instance */
  boolean                 mask      /* TRUE-mask, FALSE=unmask */
);

/** Wait for the link to go down. */
typedef boolean (*wait_link_down_fn)
(
  glink_transport_if_type *if_ptr   /* Pointer to the interface instance */
);

/** Provides data rate (tokens/sec) achievable by the transport for given 
    packet size and latency. */
typedef uint32 (*get_req_rate_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  uint32                  latency_us, /* latency in us */
  size_t                  pkt_size    /* Size of the packet */
);

/** Allocates context structure to use by transport servicing given QoS request. */
typedef void* (*alloc_req_ctx_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  uint32                  latency_us, /* latency in us */
  size_t                  pkt_size    /* Size of the packet */
);

/** Frees context structure used by transport servicing given QoS request. */
typedef void (*free_req_ctx_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);

/** Starts QoS mode. */
typedef void (*start_req_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);

/** Stops QoS mode. */
typedef void (*stop_req_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);

/** Provides ramp up time in microseconds. */
typedef uint32 (*get_ramp_time_fn)
(
  glink_transport_if_type *if_ptr,    /* Pointer to the interface instance */
  void                    *qos_ctx    /* Pointer to the transport QoS context */
);


/**
* Data Structure for GLink Core to call into transport QoS API
*
* This structure is used by the GLink core to operate transport
* This set of function pointers to to be filled in by the transport
* abstraction layer.
*/
typedef struct glink_transport_qos_if_s {
  /** Provides maximum data rate (tokens/sec) associated with transport. */
  uint32 max_rate;
  /** Provides data rate achievable by the transport for given request. */
  get_req_rate_fn  get_req_rate;
  /** Allocates context structure to use by transport servicing given QoS request. */
  alloc_req_ctx_fn alloc_req_ctx;
  /** Frees context structure used by transport servicing given QoS request. */
  free_req_ctx_fn  free_req_ctx;
  /** Starts QoS mode. */
  start_req_fn     start_req;
  /** Stops QoS mode. */
  stop_req_fn      stop_req;
  /** Provides ramp up time in microseconds. */
  get_ramp_time_fn get_ramp_time;

} glink_transport_qos_if_type;

/**
 * Data Structure for GLink Core to call into transport API
 *
 * This structure is used by the GLink core to operate transport
 * This set of function pointers to to be filled in by the transport
 * abstraction layer.
 */
struct glink_transport_if {
  /* Link needed for use with list APIs.  Must be at the head of the struct */
  smem_list_link_type                link;

  /** Transmit a version command for local negotiation. The transport would
   * call glink_transport_if_type::rx_cmd_version_ack callback as a result */
  tx_cmd_version_fn                  tx_cmd_version;

  /** Transmit a version ack for remote nogotiation */
  tx_cmd_version_ack_fn              tx_cmd_version_ack;

  /** Signals the negotiation is complete and transport can now do version
   *  specific initialization */
  set_version_fn                     set_version;


  /** Sends OPEN command to transport for transport to do whatever it deems
   *  necessary to open a GLink logical channel.
   *  GLink Core expects glink_transport_if_type::rx_cmd_ch_open_ack to be
   *  called as a result of channel opening. */
  tx_cmd_ch_open_fn                  tx_cmd_ch_open;


  /** Sends CLOSE command to transport for transport to do whatever it deems
   *  necessary to close the specified GLink logical channel.
   *  GLink Core expects glink_transport_if_type::rx_cmd_ch_close_ack to be
   *  called as a result of channel closing. */
  tx_cmd_ch_close_fn                 tx_cmd_ch_close;


  /** Sends the remote open ACK command in response to receiving
   *  glink_core_if_type::rx_cmd_ch_remote_open */
  tx_cmd_ch_remote_open_ack_fn       tx_cmd_ch_remote_open_ack;

  /** Sends the remote close ACK command in response to receiving
   *  glink_core_if_type::rx_cmd_ch_remote_close */
  tx_cmd_ch_remote_close_ack_fn      tx_cmd_ch_remote_close_ack;

  /** Allocates a receive vector buffer for the local rx intent */
  allocate_rx_intent_fn               allocate_rx_intent;

  /** Deallocates a receive vector buffer for the local rx intent */
  deallocate_rx_intent_fn             deallocate_rx_intent;

  /** Send receive intent ID for a given channel */
  tx_cmd_local_rx_intent_fn          tx_cmd_local_rx_intent;


  /** Send receive done command for an intent for the specified channel */
  tx_cmd_local_rx_done_fn            tx_cmd_local_rx_done;


  /** Send a data packet to the transport to be transmitted over the specified
   *  channel */
  tx_fn                              tx;

  /** Send request to the remote to queue more rx intents */
  tx_cmd_rx_intent_req_fn            tx_cmd_rx_intent_req;

  /** Send ACK to the remote side's request to queue more rx intents */
  tx_cmd_remote_rx_intent_req_ack_fn tx_cmd_remote_rx_intent_req_ack;

  /** Sets the local channel signals as per the specified 32-bit mask.
   *  Transport may also send the 32-bit value as-is to the remote side. */
  tx_cmd_set_sigs_fn                 tx_cmd_set_sigs;

  /** Lets the XAL know of SSR on that edge. */
  ssr_fn                             ssr;

  /** Trigger a poll for any rx data / signals on transport. */
  poll_fn                            poll;

  /** Mask/Unmask rx interrupt associated with transport */
  mask_rx_irq_fn                     mask_rx_irq;

  /** Wait for the link to go down. */
  wait_link_down_fn                  wait_link_down;

  /** Transport specific data pointer that transport may choose fill in
   * with some data */
  glink_core_xport_ctx_type          *glink_core_priv;

  /** GLINK core interface pointer associated with this transport interface */
  glink_core_if_type                 *glink_core_if_ptr;

  /* glink transport priority */
  glink_xport_priority                glink_priority;

  /* pointer to glink transport QoS interface */
  glink_transport_qos_if_type        *qos_functbl;
};

#endif //GLINK_TRANSPORT_IF_H
