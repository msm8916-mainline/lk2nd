/* Copyright (c) 2015-2016 The Linux Foundation. All rights reserved.
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

#include <rpm-glink.h>
#include <smd.h>
#include <glink.h>
#include <glink_rpm.h>
#include <xport_rpm.h>
#include <stdint.h>
#include <sys/types.h>
#include <arch/defines.h>
#include <debug.h>
#include <stdlib.h>
#include <platform/timer.h>
#include <platform/interrupts.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <pm8x41.h>
#include <kernel/event.h>

#define RPM_REQ_MAGIC 0x00716572
#define RPM_CMD_MAGIC 0x00646d63
#define REQ_MSG_LENGTH 0x14
#define CMD_MSG_LENGTH 0x08
#define ACK_MSG_LENGTH 0x0C

glink_handle_type rpm_glink_port, ssr_glink_port;
static uint32_t msg_id;
static event_t wait_for_rpm_init;
static event_t wait_for_ssr_init;
static event_t wait_for_data;

extern glink_err_type glink_wait_link_down(glink_handle_type handle);

static void rpmdatacpy(uint32_t * dst, uint32_t *src, uint32_t size)
{
	if(size%4)
		ASSERT(0);
	size = size/4;

	while (size >0)
	{
		*dst++ = *src++;
		size--;
	}
}

glink_err_type rpm_glink_send_data(uint32_t *data, uint32_t len, msg_type type)
{
	rpm_req req;
	rpm_cmd cmd;
	glink_err_type send_err = 0;
	uint32_t len_to_rpm = 0;
	void *rpm_data = NULL;
	event_init(&wait_for_data, false, EVENT_FLAG_AUTOUNSIGNAL);

	switch(type)
	{
		case RPM_REQUEST_TYPE:
			req.hdr.type = RPM_REQ_MAGIC;
			req.hdr.len = len + REQ_MSG_LENGTH;//20
			req.req_hdr.id = ++msg_id;
			req.req_hdr.set = 0;
			req.req_hdr.resourceType = data[RESOURCETYPE];
			req.req_hdr.resourceId = data[RESOURCEID];
			req.req_hdr.dataLength = len;

			fill_kvp_object(&req.data, data, len);
			len_to_rpm = req.req_hdr.dataLength + 0x28;

			rpm_data = (void*) malloc(len_to_rpm);
			ASSERT(rpm_data);
			memset(rpm_data, 0, len_to_rpm);
			memcpy(rpm_data, &req.hdr, sizeof(rpm_gen_hdr));
			memcpy(rpm_data + sizeof(rpm_gen_hdr), &req.req_hdr, sizeof(rpm_req_hdr));
			memcpy(rpm_data + sizeof(rpm_gen_hdr)+ sizeof(rpm_req_hdr), req.data, len);

			// Send Data Request to RPM
			send_err = glink_tx(rpm_glink_port, NULL, (const void *)rpm_data, len_to_rpm, 0);
			if (send_err)
			{
				dprintf(CRITICAL, "%s:%d, Glink tx error: 0x%x\n", __func__, __LINE__, send_err);
				free(rpm_data);
				free_kvp_object(&req.data);
				break;
			}
#ifdef DEBUG_GLINK
			dprintf(INFO, "%s:%d, Wait till we receive response from RPM\n", __func__, __LINE__);
#endif
			event_wait(&wait_for_data);
			free(rpm_data);
			free_kvp_object(&req.data);
			break;
		case RPM_CMD_TYPE:
			cmd.hdr.type = RPM_CMD_MAGIC;
			cmd.hdr.len = CMD_MSG_LENGTH;//0x8;
			len_to_rpm  = sizeof(rpm_cmd);

			fill_kvp_object(&cmd.data, data, len);
			send_err = glink_tx(rpm_glink_port, NULL, (const void *)&cmd, len_to_rpm, 0);
			if (send_err)
				dprintf(CRITICAL, "%s:%d, Glink tx error: 0x%x\n", __func__, __LINE__, send_err);
			free_kvp_object(&cmd.data);
			break;
		default:
			dprintf(CRITICAL, "Invalid RPM Request\n");
			break;
	}

	return send_err;
}

uint32_t rpm_glink_recv_data(char *rx_buffer, uint32_t* len)
{
	rpm_ack_msg *resp;
	msg_type type = 0;
	uint32_t ret = 0;
	/* As per the current design rpm response does not exceed 20 bytes */
	if (rx_buffer == NULL)
	{
		dprintf(CRITICAL, "Invalid pointer to data received from RPM\n");
		return 99;
	}
	resp = (rpm_ack_msg *)rx_buffer;

	if(resp->hdr.type == RPM_CMD_MAGIC)
	{
		type = RPM_CMD_TYPE;
	}
	else if(resp->hdr.type == RPM_REQ_MAGIC)
	{
		type = RPM_REQUEST_TYPE;
	}

	if (type == RPM_CMD_TYPE && resp->hdr.len == ACK_MSG_LENGTH)
	{
		dprintf(SPEW, "Received SUCCESS CMD ACK\n");
	}
	else if (type == RPM_REQUEST_TYPE && resp->hdr.len == ACK_MSG_LENGTH)
	{
		dprintf(SPEW, "Received SUCCESS REQ ACK \n");
	}
	else
	{
		ret = 1;
		dprintf(CRITICAL, "Received ERROR ACK \n");
	}

	if(!ret)
	{
		ret = sizeof(rpm_gen_hdr) + sizeof(kvp_data);
	}
#ifdef DEBUG_GLINK
	dprintf(INFO, "%s:%d Return value %u\n", __func__, __LINE__, ret);
#endif
	return ret;
}

void rpm_vector_glink_ssr_isr(glink_handle_type port, void *unused_open_data, void *unused_pkt_priv,
							void *buffer, size_t size, size_t intent_used,
							glink_buffer_provider_fn vprovider, glink_buffer_provider_fn pprovider)
{
	char rx_buffer[12];
	char *return_buffer = NULL;
	uint32_t ret = 0;
	uint32_t offset = 0;
	size_t return_size = 0;

#ifdef DEBUG_GLINK
	dprintf(INFO, "RPM Vector GLINK SSR ISR\n");
#endif
	if (size == 0)
	{
		dprintf(CRITICAL, "Invalid size of RPM response\n");
		ASSERT(0);
	}
	if (size > sizeof(rx_buffer))
	{
		dprintf(CRITICAL, "Need larger RPM rx buffer. Size of Local Buffer: %u\tSize of RX Buffer: %u\n", size, sizeof(rx_buffer));
		ASSERT(0);
	}
	do
	{
		return_buffer = vprovider(buffer, offset, &return_size);
		if(return_buffer)
		{
			rpmdatacpy((uint32_t *)(rx_buffer + offset),(uint32_t *)return_buffer, return_size);
			offset += return_size;
		}
	} while(return_buffer);
	ret = rpm_glink_recv_data(rx_buffer,(uint32_t *)&size);
	if(ret)
	{
		dprintf(CRITICAL, "Return value from recv_data: %x\n", ret);
	}
	// Release the mutex
#ifdef DEBUG_GLINK
	dprintf(INFO, "Received Data from RPM\n");
#endif
	event_signal(&wait_for_data, false);
}

void rpm_vector_glink_isr(glink_handle_type port, void *unused_open_data, void *unused_pkt_priv,
							void *buffer, size_t size, size_t intent_used,
							glink_buffer_provider_fn vprovider, glink_buffer_provider_fn pprovider)
{
	char rx_buffer[64];
	char *return_buffer = NULL;
	uint32_t ret = 0;
	uint32_t offset = 0;
	size_t return_size = 0;
#ifdef DEBUG_GLINK
	dprintf(INFO, "RPM Vector GLINK ISR\n");
#endif
	if (size == 0)
	{
		dprintf(CRITICAL, "Invalid size of RPM response\n");
		ASSERT(0);
	}
	if (size > sizeof(rx_buffer))
	{
		dprintf(CRITICAL, "Need larger RPM rx buffer. Size of Local Buffer: %u\tSize of RX Buffer: %u\n", size, sizeof(rx_buffer));
		ASSERT(0);
	}
	do
	{
		return_buffer = vprovider(buffer, offset, &return_size);
		if(return_buffer)
		{
			rpmdatacpy((uint32_t *)(rx_buffer + offset),(uint32_t *)return_buffer, return_size);
			offset += return_size;
		}
	} while(return_buffer);
	ret = rpm_glink_recv_data(rx_buffer, (uint32_t *)&size);
	if(ret)
	{
		dprintf(CRITICAL, "Return value from recv_data: %x\n", ret);
	}
	// Release the mutex
#ifdef DEBUG_GLINK
	dprintf(INFO, "Received Data from RPM\n");
#endif
	event_signal(&wait_for_data, false);
}

void rpm_glink_notify_state_isr(glink_handle_type handle, void *data, glink_channel_event_type event)
{
	if(event == GLINK_CONNECTED)
	{
		event_signal(&wait_for_rpm_init, false);
		dprintf(INFO, "Glink Connection between APPS and RPM established\n");
		return;
	}
}

void rpm_glink_notify_state_ssr_isr(glink_handle_type handle, void *data, glink_channel_event_type event)
{
	if(event == GLINK_CONNECTED)
	{
		event_signal(&wait_for_ssr_init, false);
		dprintf(INFO, "Glink Connection between APPS and RPM established\n");
		return;
	}
}

void rpm_glink_tx_done_isr(void)
{
	//empty function for tx_done cb. Nothing required here for now since we are always in
	//"single-threaded" operation of sending GLink requests
	return;
}

void rpm_glink_open(glink_link_info_type *link_info, void* priv)
{
	glink_err_type ret;
	glink_open_config_type glink_open_cfg = {0}, glink_ssr_open_cfg = {0};

	// Open channel for tx
	glink_open_cfg.name = "rpm_requests";
	glink_open_cfg.remote_ss = link_info->remote_ss;
	glink_open_cfg.notify_rx = NULL;
	glink_open_cfg.notify_rxv = (glink_rxv_notification_cb)rpm_vector_glink_isr;
	glink_open_cfg.notify_tx_done = (glink_tx_notification_cb)rpm_glink_tx_done_isr;
	glink_open_cfg.notify_state = (glink_state_notification_cb)rpm_glink_notify_state_isr;
	glink_open_cfg.priv = NULL;
	ret = glink_open(&glink_open_cfg, &rpm_glink_port);
	if (ret == GLINK_STATUS_SUCCESS)
		dprintf(INFO, "Opening RPM Glink Port success\n");
	else
	{
		dprintf(CRITICAL, "Opening RPM Glink Port failure %d\n", ret);
		ASSERT(0);
	}

	// Open Channel for tear down
	glink_ssr_open_cfg.name = "glink_ssr";
	glink_ssr_open_cfg.remote_ss = link_info->remote_ss;
	glink_ssr_open_cfg.notify_rx = NULL;
	glink_ssr_open_cfg.notify_rxv = (glink_rxv_notification_cb)rpm_vector_glink_ssr_isr;
	glink_ssr_open_cfg.notify_tx_done = (glink_tx_notification_cb)rpm_glink_tx_done_isr;
	glink_ssr_open_cfg.notify_state = (glink_state_notification_cb)rpm_glink_notify_state_ssr_isr;
	glink_ssr_open_cfg.priv = NULL;

	ret = glink_open(&glink_ssr_open_cfg, &ssr_glink_port);
	if (ret == GLINK_STATUS_SUCCESS)
		dprintf(INFO, "Opening SSR Glink Port success\n");
	else
	{
		dprintf(CRITICAL, "Opening SSR Glink Port failure %d\n", ret);
		ASSERT(0);
	}

}

void rpm_glink_init()
{
	glink_err_type ret;
	glink_link_id_type link_id;
	event_init(&wait_for_rpm_init, false, EVENT_FLAG_AUTOUNSIGNAL);
	event_init(&wait_for_ssr_init, false, EVENT_FLAG_AUTOUNSIGNAL);

	dprintf(INFO, "RPM GLink Init\n");
	// Initialize RPM transport
	ret = xport_rpm_init(NULL);
	if (ret == GLINK_STATUS_SUCCESS)
	{
		unmask_interrupt(GLINK_IPC_IRQ);
		GLINK_LINK_ID_STRUCT_INIT(link_id);
		link_id.remote_ss = "rpm";
		link_id.link_notifier = (glink_link_state_notif_cb)rpm_glink_open;
		glink_register_link_state_cb(&link_id, NULL);
	}
	else
	{
		dprintf(CRITICAL, "RPM Glink Init Failure 0x%x\n", ret);
		ASSERT(0);
	}
	event_wait(&wait_for_rpm_init);
	event_wait(&wait_for_ssr_init);
}

void rpm_glink_uninit()
{
	rpm_ssr_req req;
	glink_err_type ret;
	uint32_t len_to_rpm, loop = 100000;

	// update ssr request
	req.version = 0;
	req.cmd = 0;
	req.seqnumber = 0;
	memset(req.name, 0, sizeof(req.name));
	strlcpy(req.name, "apss", sizeof(req.name));
	req.namelength = strlen(req.name);
	len_to_rpm = sizeof(rpm_ssr_req);
	dprintf(INFO, "RPM GLINK UnInit\n");
	ret = glink_tx(ssr_glink_port, NULL, (const void *)&req, len_to_rpm, 0);

	if (ret)
	{
		dprintf(CRITICAL, "Glink SSR Channel: Tx for link tear down request failure with error code: 0x%x\n", ret);
		ASSERT(0);
	}

#ifdef DEBUG_GLINK
	dprintf(INFO, "%s:%d, Wait till we receive response from RPM\n", __func__, __LINE__);
#endif
	// loop till the FIFO indices are cleared
	while((ret = glink_wait_link_down(ssr_glink_port)) && loop)
	{
		loop--;
		mdelay(1);
		continue;
	}
	if (!loop)
	{
		dprintf(INFO, "%s:%d, Tearing down Glink SSR Channel Timed out\n", __func__, __LINE__);
		ASSERT(0);
	}
}
