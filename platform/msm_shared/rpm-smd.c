/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#include <rpm-smd.h>
#include <smd.h>
#include <stdint.h>
#include <sys/types.h>
#include <arch/defines.h>
#include <debug.h>
#include <stdlib.h>

#define RPM_REQ_MAGIC 0x00716572
#define RPM_CMD_MAGIC 0x00646d63
#define REQ_MSG_LENGTH 0x14
#define CMD_MSG_LENGTH 0x08
#define ACK_MSG_LENGTH 0x0C

static uint32_t msg_id;
smd_channel_info_t ch;

void rpm_smd_init()
{
	smd_init(&ch, SMD_APPS_RPM);
}

void rpm_smd_uninit()
{
	smd_uninit(&ch);
}

static void fill_kvp_object(kvp_data **kdata, uint32_t *data, uint32_t len)
{
	uint32_t i =0;

	*kdata = (kvp_data *) memalign(CACHE_LINE, ROUNDUP(len, CACHE_LINE));
	ASSERT(*kdata);

	memcpy(*kdata, data+2, len);
}

static void free_kvp_object(kvp_data **kdata)
{
	if(*kdata)
		free(*kdata);
}

int rpm_send_data(uint32_t *data, uint32_t len, msg_type type)
{
	rpm_req req;
	rpm_cmd cmd;
	uint32_t len_to_smd = 0;
	int ret = 0;
	uint32_t ack_msg_len = 0;
	uint32_t rlen = 0;
	void *smd_data = NULL;

	switch(type)
	{
		case RPM_REQUEST_TYPE:
			req.hdr.type = RPM_REQ_MAGIC;
			req.hdr.len = len + REQ_MSG_LENGTH;//20
			req.req_hdr.id = ++msg_id;
			req.req_hdr.set = 0;//assume active set. check sleep set.
			req.req_hdr.resourceType = data[RESOURCETYPE];
			req.req_hdr.resourceId = data[RESOURCEID];
			req.req_hdr.dataLength = len;

			fill_kvp_object(&req.data, data, len);
			len_to_smd = req.req_hdr.dataLength + 0x28;

			smd_data = (void*) malloc(len_to_smd);
			ASSERT(smd_data);

			memcpy(smd_data, &req.hdr, sizeof(rpm_gen_hdr));
			memcpy(smd_data + sizeof(rpm_gen_hdr), &req.req_hdr, sizeof(rpm_req_hdr));
			memcpy(smd_data + sizeof(rpm_gen_hdr)+ sizeof(rpm_req_hdr), req.data, len);

			ret = smd_write(&ch, smd_data, len_to_smd, SMD_APPS_RPM);

			/* Read the response */
			ack_msg_len = rpm_recv_data(&rlen);

			smd_signal_read_complete(&ch, ack_msg_len);

			free(smd_data);
			free_kvp_object(&req.data);
		break;
		case RPM_CMD_TYPE:
			cmd.hdr.type = RPM_CMD_MAGIC;
			cmd.hdr.len = CMD_MSG_LENGTH;//0x8;
			len_to_smd  = sizeof(rpm_cmd);

			fill_kvp_object(&cmd.data, data, len);
			ret = smd_write(&ch, (void *)&cmd, len_to_smd, SMD_APPS_RPM);

			free_kvp_object(&cmd.data);
		break;
		default:
		break;
	}

	return ret;
}

uint32_t rpm_recv_data(uint32_t* len)
{
	rpm_ack_msg *resp;
	msg_type type;
	uint32_t ret = 0;
	/* As per the current design rpm response does not exceed 20 bytes */
	uint32_t response[5];

	smd_read(&ch, len, SMD_APPS_RPM, response);

	resp = (rpm_ack_msg *)response;

	arch_invalidate_cache_range((addr_t)resp, sizeof(rpm_gen_hdr));

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

	return ret;
}

void rpm_clk_enable(uint32_t *data, uint32_t len)
{
	/* Send the request to SMD */
	rpm_send_data(data, len, RPM_REQUEST_TYPE);
}

void rpm_clk_disable(uint32_t *data, uint32_t len)
{
	dprintf(CRITICAL,"Clock disable\n");
}

