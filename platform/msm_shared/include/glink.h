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

#ifndef GLINK_H
#define GLINK_H

/**
 * @file glink.h
 *
 * Public API for the GLink
 */

/** \defgroup glink GLink
 * \ingroup SMD
 *
 * GLink reliable, in-order, datagram-based interprocessor communication
 * over a set of supported transport (Shared Memory, UART, BAM, HSIC)
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/



#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include <stdlib.h>

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
/** GLink status/return codes */
typedef enum {
  GLINK_STATUS_SUCCESS = 0,
  GLINK_STATUS_INVALID_PARAM             = -1,
  GLINK_STATUS_NOT_INIT                  = -2,
  GLINK_STATUS_OUT_OF_RESOURCES          = -3,
  /* Failure due to lack of transport */
  GLINK_STATUS_NO_TRANSPORT              = -4 ,
  /* TX failure when there is no remote intent queued */
  GLINK_STATUS_NO_REMOTE_INTENT_FOUND    = -5,
  /* Failure of a glink_* call when channel is not fully opened yet */
  GLINK_STATUS_CH_NOT_FULLY_OPENED       = -6,
  /* Failure due to closing the same channel twice */
  GLINK_STATUS_CH_ALREADY_CLOSED         = -7,
  /* Returned when intent APIs are used over an intent less xport */
  GLINK_STATUS_API_NOT_SUPPORTED         = -8,
  /* Failure specific to QoS algorithm/implementation */
  GLINK_STATUS_QOS_FAILURE               = -9,
  /* Failure due to tx_cmd* calls */
  GLINK_STATUS_TX_CMD_FAILURE            = -10,
  /* For other Failures not covered above */
  GLINK_STATUS_FAILURE                   = -11,
  /* Failures relating to GLink operation timeout */
  GLINK_STATUS_TIMEOUT                   = -12
}glink_err_type;

/** List of possible subsystems */
/**
  "apss"   Application Processor Subsystem
  "mpss"   Modem subsystem
  "lpass"  Low Power Audio Subsystem
  "dsps"   Sensors Processor
  "wcnss"  Wireless Connectivity Subsystem
  "rpm"    Resource Power Manager processor
*/

/** Max allowed channel name length */
#define GLINK_CH_NAME_LEN 32

/* Bit position of DTR/CTS/CD/RI bits in control sigs 32 bit signal map */
#define SMD_DTR_SIG_SHFT 31
#define SMD_CTS_SIG_SHFT 30
#define SMD_CD_SIG_SHFT  29
#define SMD_RI_SIG_SHFT  28

/** Version number for the glink_link_id_type structure */
#define GLINK_LINK_ID_VER  0x00000001

/** Macro to initialize the link identifier structure with default values.
 * It memsets the header to 0 and initializes the header field */
#define GLINK_LINK_ID_STRUCT_INIT(link_id) \
                          (link_id).xport = 0; \
                          (link_id).remote_ss = 0; \
                          (link_id).link_notifier = 0; \
                          (link_id).handle = 0; \
                          (link_id).version = GLINK_LINK_ID_VER;

/* GLink tx options */
/* Flag for no options */
#define GLINK_TX_NO_OPTIONS      ( 0 )

/* Whether to block and request for remote rx intent in
 * case it is not available for this pkt tx */
#define GLINK_TX_REQ_INTENT      0x00000001

/* If the tx call is being made from single threaded context. GLink tries to
 * flush data into the transport in glink_tx() context, or returns error if
 * it is not able to do so */
#define GLINK_TX_SINGLE_THREADED 0x00000002

/* This option is to turn on tracer packet */
#define GLINK_TX_TRACER_PKT      0x00000004

/* ======================= glink open cfg options ==================*/

/* Specified transport is just the initial transport and migration is possible
 * to higher-priority transports.  Without this flag, the open will fail if
 * the transport does not exist. */
#define GLINK_OPT_INITIAL_XPORT   0x00000001

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/** Enums to identify link state */
typedef enum {
 GLINK_LINK_STATE_UP,  /* Called after specified link is online */
 GLINK_LINK_STATE_DOWN /* Called after SSR (before REMOTE_DISCONNECT) */
} glink_link_state_type;

/** Data structure holding the link state information */
typedef struct          glink_link_info_t {
  const char*           xport;      /* Transport name */
  const char*           remote_ss;  /* Remote subsystem name */
  glink_link_state_type link_state; /* Link state */
}glink_link_info_type;

/* Callback function invoked when link goes up/down */
typedef void (*glink_link_state_notif_cb)
(
  glink_link_info_type *link_info, /* Ptr to link information strcuture */
  void* priv                       /* Client private data */
);

/**
 * Opaque handle returned by glink_register_link_state_cb. Client uses this
 * handle to call glink_deregister_link_state_cb() to deregister their
 * callback
 *
 * Client must not modify or try to interpret this value
 */
typedef struct glink_link_notif_data_type * glink_link_handle_type;

/** Data structure to provide link information for the link to monitor */
typedef struct glink_link_id_t {
  unsigned int              version; /* This fields identifies the verion of
                                        the structure. Initialized by
                                        GLINK_LINK_ID_STRUCT_INIT macro */

  const char*               xport;         /* NULL = any transport */
  const char*               remote_ss;     /* NULL = any subsystem */
  glink_link_state_notif_cb link_notifier; /* Notification callback */
  glink_link_handle_type    handle;  /* Set by glink_register_link_state_cb */
}glink_link_id_type;

/**
 * Opaque handle returned by glink_open. Client uses this handle to call into
 * GLink API for any further activity related to the channel,
 *
 * Client must to modify or try to interpret this value
 */
typedef struct glink_channel_ctx* glink_handle_type;

/** GLink logical link state notifications */
typedef enum {
  /** Channel is fully opened. Both local and remote end have
      opened channel from their respective ends, and data communication
      can now take place */
  GLINK_CONNECTED = 0,

  /** Local side has called glink_close() and remote side has acknowledged
      this end's close. Client may call glink_open() after this point */
  GLINK_LOCAL_DISCONNECTED,

  /** Remote side has called glink_close() to close the channel */
  GLINK_REMOTE_DISCONNECTED
}glink_channel_event_type;

/** Vector buffer provider type*/
typedef void* (*glink_buffer_provider_fn)
(
  void*  iovec,   /* vector buffer */
  size_t offset,  /* offset from the beginning of the vector */
  size_t *size    /* size of the returned contiguous buffer */
);

/** Data receive notification callback type*/
typedef void (*glink_rx_notification_cb)
(
  glink_handle_type handle,     /* handle for the glink channel */
  const void        *priv,      /* priv client data passed in glink_open */
  const void        *pkt_priv,  /* private client data assiciated with the
                                   rx intent that client queued earlier */
  const void        *ptr,       /* pointer to the received buffer */
  size_t            size,       /* size of the packet */
  size_t            intent_used /* size of the intent used for this packet */
);

/** Data receive notification callback type*/
typedef void (*glink_rx_tracer_pkt_notification_cb)
(
  glink_handle_type handle,     /* handle for the glink channel */
  const void        *priv,      /* priv client data passed in glink_open */
  const void        *pkt_priv,  /* private client data assiciated with the
                                   rx intent that client queued earlier */
  const void        *ptr,       /* pointer to the received buffer */
  size_t            size        /* size of the packet */
);

/** Vector receive notification callback type*/
typedef void (*glink_rxv_notification_cb)
(
  glink_handle_type        handle,    /* handle for the glink channel */
  const void               *priv,     /* priv client data passed in glink_open */
  const void               *pkt_priv, /* private client data assiciated with the
                                         rx intent that client queued earlier */
  void                     *iovec,    /* pointer to the received vector */
  size_t                   size,      /* size of the packet */
  size_t                   intent_used, /* size of the intent used for this packet */
  glink_buffer_provider_fn vprovider, /* Buffer provider for virtual space */
  glink_buffer_provider_fn pprovider  /* Buffer provider for physical space */
);

/** Data transmit notification callback type*/
typedef void (*glink_tx_notification_cb)
(
  glink_handle_type handle,    /* handle for the glink channel */
  const void        *priv,     /* priv client data passed in glink_open */
  const void        *pkt_priv, /* private client data assiciated with the
                                  tx pkt that client queued earlier */
  const void        *ptr,      /* pointer to the transmitted buffer */
  size_t            size       /* size of the packet */
);

/** GLink channel state change notification callback type*/
typedef void (*glink_state_notification_cb)
(
  glink_handle_type         handle, /* handle for the glink channel */
  const void                *priv,  /* priv client data passed in glink_open */
  glink_channel_event_type  event   /* Notification event */
);

/** Request for a receive intent of size req_size is queued. The request
 * originates from the remote side GLink client. Return true if intent will
 * be queued or false if intent request will be denied. */
typedef bool (*glink_notify_rx_intent_req_cb)
(
  glink_handle_type         handle,   /* handle for the glink channel */
  const void                *priv,    /* priv client data passed in glink_open */
  size_t                    req_size  /* Requested size */
);

/** New intent arrival notification callback type*/
typedef void (*glink_notify_rx_intent_cb)
(
  glink_handle_type         handle,   /* handle for the glink channel */
  const void                *priv,    /* priv client data passed in glink_open */
  size_t                    size      /* Intent size */
);

/** Control signal change notification - Invoked when remote side
 *  alters its control signals */
typedef void (*glink_notify_rx_sigs_cb)
(
  glink_handle_type  handle,   /* handle for the glink channel */
  const void         *priv,    /* priv client data passed in glink_open  */
  uint32_t             prev,     /* Previous remote state */
  uint32_t             curr      /* Current remote state */
);

/** rx_intent abort notification. This callback would be invoked for
 *  every rx_intent that is queued with GLink core at the time the
 *  remote side or local side decides to close the port */
typedef void(*glink_notify_rx_abort_cb)
(
glink_handle_type  handle,   /* handle for the glink channel */
const void         *priv,    /* priv client data passed in glink_open  */
const void         *pkt_priv /* pkt specific private data */
);

/** tx abort notification. This callback would be invoked if client
 *   had queued a tx buffer with glink and it had not been transmitted i.e.
 *   tx_done callback has not been called for this buffer and remote side
 *   or local side closed the port*/
typedef void(*glink_notify_tx_abort_cb)
(
glink_handle_type  handle,   /* handle for the glink channel */
const void         *priv,    /* priv client data passed in glink_open  */
const void         *pkt_priv /* pkt specific private data */
);

/**
 * Data Structure for GLink logical channel open configuration
 *
 * This structure is used by the clients to open a GLink logical channel
 * when calling glink_open()
 */
typedef struct {
  /** string name for the transport to use (Optional)*/
  const char                          *transport;

  /** string name for the remote subsystem to which the user wants to
      connect */
  const char                          *remote_ss;

  /** string name for the channel */
  const char                          *name;

  /** bitfield for specifying various options */
  unsigned                            options;

  /** Private data for client to maintain context. This data is passed back
      to client in the notification callbacks */
  const void                          *priv;

  /** Data receive notification callback. Optional if notify_rxv is provided */
  glink_rx_notification_cb            notify_rx;

  /** Tracer packet receive notification callback.
   *  Optional if user doesn't need to use this */
  glink_rx_tracer_pkt_notification_cb notify_rx_tracer_pkt;

  /** Vector receive notification callback. Optional if notify_rx is provided */
  glink_rxv_notification_cb           notify_rxv;

  /** Data transmit notification callback */
  glink_tx_notification_cb            notify_tx_done;

  /** GLink channel state notification callback */
  glink_state_notification_cb         notify_state;

  /** Intent request from the remote side. Optional */
  glink_notify_rx_intent_req_cb       notify_rx_intent_req;

  /** New intent arrival from the remote side */
  glink_notify_rx_intent_cb           notify_rx_intent;

  /** Control signal change notification - Invoked when remote side
   *  alters its control signals. Optional */
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

}glink_open_config_type;

/*===========================================================================
                      GLINK PUBLIC API
===========================================================================*/
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
  void* priv
);

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
);

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
);

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
);

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
 * @param[in]   options    Flags specifying how transmission for this buffer
 *                         would be handled. See GLINK_TX_* flag definitions.
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
  uint32_t            options
);

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
 * @param[in]   options    Flags specifying how transmission for this buffer
 *                         would be handled. See GLINK_TX_* flag definitions.
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
  uint32_t                   options
);

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
);

/**
 * Client uses this to signal to GLink layer that it is done with the received
 * data buffer. This API should be called to free up the receive buffer, which,
 * in zero-copy mode is actually remote-side's transmit buffer.
 *
 * @param[in]   handle   GLink handle associated with the logical channel
 *
 * @param[in]   *ptr     Pointer to the received buffer
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
  bool           reuse
);

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
  uint32_t            sig_value
);

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
  uint32_t            *sig_value
);

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
  uint32_t            *sig_value
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_H
