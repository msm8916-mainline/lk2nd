/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Fundation, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __RPM_GLINK_H
#define __RPM_GLINK_H

#include <arch/defines.h>
#include <stdint.h>
#include <sys/types.h>
#include <rpm-ipc.h>
#include <glink.h>

typedef struct
{
	uint32_t version;
	uint32_t cmd;
	uint32_t seqnumber;
	uint32_t namelength;
	char name[32];
} rpm_ssr_req;

typedef rpm_cmd rpm_ack_msg;
glink_err_type rpm_glink_send_data(uint32_t *data, uint32_t len, msg_type type);
uint32_t rpm_glink_recv_data(char *rx_buffer, uint32_t *len);
void rpm_glink_clk_enable(uint32_t *data, uint32_t len);
void rpm_glink_clk_disable(uint32_t *data, uint32_t len);
void rpm_glink_init();
void rpm_glink_uninit();
void rpm_scalar_glink_isr(glink_handle_type port, void *unused_open_data, void *unused_pkt_priv, void *buffer, size_t size, size_t intent_used);
void rpm_vector_glink_isr(glink_handle_type port, void *unused_open_data, void *unused_pkt_priv, void *buffer, size_t size, size_t intent_used, glink_buffer_provider_fn vprovider, glink_buffer_provider_fn pprovider);
void rpm_glink_tx_done_isr(void);

void rpm_glink_notify_state_isr(glink_handle_type handle, void *data, glink_channel_event_type event);
#endif
