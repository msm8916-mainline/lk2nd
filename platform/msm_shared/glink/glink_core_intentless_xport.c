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

#include "glink.h"
#include "glink_internal.h"

glink_core_if_type glink_core_intentless_interface =
{
  /** Indicates that transport is now ready to start negotiation using the
   *  v0 configuration. */
  glink_link_up,
  /** Receive transport version for remote-initiated version negotiation */
  glink_rx_cmd_version,
  /** Receive ACK to previous glink_transport_if_type::tx_cmd_version command */
  glink_rx_cmd_version_ack,
  /** Receive remote channel open request; expected response is
   *  glink_transport_if_type:: tx_cmd_ch_remote_open_ack */
  glink_rx_cmd_ch_remote_open,
  /** This function is invoked by the transport in response to
   *  glink_transport_if_type:: tx_cmd_ch_open */
  glink_rx_cmd_ch_open_ack,
  /** This function is invoked by the transport in response to
   *  glink_transport_if_type:: tx_cmd_ch_close */
  glink_rx_cmd_ch_close_ack,
  /** Remote channel close request; will result in sending
   *  glink_transport_if_type:: tx_cmd_ch_remote_close_ack */
  glink_rx_cmd_ch_remote_close,
  /** Transport invokes this call on receiving remote RX intent */
  NULL,
  /** Get receive packet context (if successful, should be followed by call to
      rx_put_pkt_ctx) */
  NULL,
  /** Receive a packet fragment (must have previously received an rx_cmd_rx_data
      packet). */
  glink_rx_put_pkt_ctx,
  /** Transport invokes this call to inform GLink that remote side is
   *   done with previous transmitted packet. */
  NULL,
  /** Remote side is requesting an RX intent */
  NULL,
  /** ACK to RX Intent Request */
  NULL,
  /** Received 32-bit signals value from remote side. It is passed on to
   *  the client */
  glink_rx_cmd_remote_sigs,
  /** If transport was full and could not continue a transmit operation,
   *  then it will call this function to notify the core that it is ready to
   *  resume transmission. */
  NULL
};


/*===========================================================================
LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      glink_core_stub_intentless
===========================================================================*/
/**

  Stub for intentless transport functionality.

  @return     GLINK_STATUS_SUCCESS.

  @sideeffects  None.
*/
/*=========================================================================*/

static glink_err_type glink_core_stub_intentless(void)
{
  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION      glink_verify_open_cfg_intentless
===========================================================================*/
/**

  Verifies open config for an intentless transport

  @param[in]  open_ch_ctx   Channel context.

  @return     TRUE if config is good.

  @sideeffects  None.
*/
/*=========================================================================*/
static boolean glink_verify_open_cfg_intentless
(
glink_channel_ctx_type   *ch_ctx
)
{
  return (ch_ctx->notify_rxv != NULL);
}

/*===========================================================================
  FUNCTION      glink_channel_receive_pkt
===========================================================================*/
/**

  Precesses Rx packet for the channel

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  intent_ptr    Packet descriptor

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
static void glink_channel_receive_pkt_intentless
(
  glink_channel_ctx_type *open_ch_ctx,
  glink_rx_intent_type *intent_ptr
)
{
  open_ch_ctx->notify_rxv(open_ch_ctx, open_ch_ctx->priv,
    NULL, intent_ptr->iovec, intent_ptr->pkt_sz, 0,
    intent_ptr->vprovider, intent_ptr->pprovider);
}

/*===========================================================================
FUNCTION      glink_channel_submit_pkt
===========================================================================*/
/**

  Invokes intentless transport Tx function to transmit a packet

  @param[in]  open_ch_ctx   Channel context.
  @param[in]  pctx_ctx      Packet context.
  @param[in]  req_intent    Request intent flag

  @return     Error code.

  @sideeffects  None.
*/
/*=========================================================================*/
static glink_err_type glink_channel_submit_pkt_intentless
(
  glink_channel_ctx_type *open_ch_ctx,
  glink_core_tx_pkt_type *pctx,
  boolean req_intent
)
{
  glink_transport_if_type *if_ptr = open_ch_ctx->if_ptr;
  glink_err_type status = if_ptr->tx(if_ptr, open_ch_ctx->lcid, pctx);

  GLINK_OS_UNREFERENCED_PARAM( req_intent );
  glink_os_free(pctx);

  return status;
}

/*===========================================================================
EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      glink_core_get_intentless_interface
===========================================================================*/
/**

  Provides core interface for the intentless transport.

  @return     Pointer to the core intentless interface.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_core_if_type* glink_core_get_intentless_interface(void)
{
  return &glink_core_intentless_interface;
}

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
void glink_core_setup_intentless_xport(glink_transport_if_type *if_ptr)
{
  if_ptr->glink_core_if_ptr = glink_core_get_intentless_interface();
  if_ptr->glink_core_priv->verify_open_cfg = glink_verify_open_cfg_intentless;
  if_ptr->glink_core_priv->channel_init = (channel_init_fn)glink_core_stub_intentless;
  if_ptr->glink_core_priv->channel_cleanup = (channel_cleanup_fn)glink_core_stub_intentless;
  if_ptr->glink_core_priv->channel_submit_pkt = glink_channel_submit_pkt_intentless;
  if_ptr->glink_core_priv->channel_receive_pkt = glink_channel_receive_pkt_intentless;
}
