/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
 */
#include <debug.h>
#include <rpmb.h>
#include <rpmb_listener.h>
#include <qseecom_lk_api.h>

#define RPMB_LSTNR_VERSION_2        0x2

typedef enum
{
	TZ_CM_CMD_RPMB_INIT = 0x101,    //257
	TZ_CM_CMD_RPMB_READ,        //258
	TZ_CM_CMD_RPMB_WRITE,       //259
	TZ_CM_CMD_RPMB_PARTITION,   //260
} tz_rpmb_cmd_type;

struct tz_device_init_req
{
	uint32_t cmd_id;
	uint32_t version;
}__PACKED;

/* RPMB Init response message */
struct tz_device_init_resp
{
    uint32_t cmd_id;           /* Command ID */
    uint32_t version;          /* Messaging version from RPMB listener */
    uint32_t status;           /* RPMB init status */
    uint32_t num_sectors;      /* Size of RPMB (in sectors) */
    uint32_t rel_wr_count;     /* Reliable write count for the RPMB */
    uint32_t dev_type;         /* Storage device type (like eMMC or UFS) */
    uint32_t reserved1;        /* Reserved 1 */
    uint32_t reserved2;        /* Reserved 2 */
    uint32_t reserved3;        /* Reserved 3 */
    uint32_t reserved4;        /* Reserved 4 */
}__PACKED;

struct tz_rpmb_rw_req
{
	uint32_t cmd_id;
	uint32_t num_sectors;
	uint32_t req_buff_len;
	uint32_t req_buff_offset;
	uint32_t version;
	uint32_t rel_wr_count;
}__PACKED;

struct tz_rpmb_rw_resp
{
	uint32_t cmd_id;
	int32_t  status;
	uint32_t res_buff_len;
	uint32_t res_buff_offset;
	uint32_t version;
}__PACKED;

typedef int (*ListenerCallback)(void*, uint32_t);

static void handle_init_request(void *buf, uint32_t sz)
{
	struct tz_device_init_req *init_req_p = NULL;
	struct tz_device_init_resp *init_resp = (struct tz_device_init_resp*) buf;
	struct rpmb_init_info *rpmb_info = NULL;

	init_req_p = (struct tz_device_init_req *) buf;

	rpmb_info = rpmb_get_init_info();

	if (rpmb_info)
		init_resp->status = 0;

	init_resp->cmd_id = init_req_p->cmd_id;
	init_resp->version = RPMB_LSTNR_VERSION_2;
	init_resp->num_sectors = rpmb_info->size;
	init_resp->rel_wr_count = rpmb_info->rel_wr_count;
	init_resp->dev_type = rpmb_info->dev_type;
}

static void handle_rw_request(void *buf, uint32_t sz)
{
	struct tz_rpmb_rw_req *req_p = (struct tz_rpmb_rw_req *)buf;
	struct tz_rpmb_rw_resp *resp_p = NULL;
	uint32_t *req_buf = buf + req_p->req_buff_offset;
	uint32_t *resp_buf = buf + sizeof(struct tz_rpmb_rw_resp);

	resp_p = (struct tz_rpmb_rw_resp *) buf;

	switch (req_p->cmd_id)
	{
		case TZ_CM_CMD_RPMB_READ:
#if DEBUG_RPMB
			dprintf(INFO, "Read Request received\n");
			dprintf(INFO, "READ: RPMB_REL_RW_COUNT 0x%x\n", req_p->rel_wr_count);
#endif
			resp_p->status = rpmb_read(req_buf, req_p->num_sectors, resp_buf, &resp_p->res_buff_len);
			break;
		case TZ_CM_CMD_RPMB_WRITE:
#if DEBUG_RPMB
			dprintf(INFO, "Write Request received\n");
			dprintf(INFO, "WRITE: RPMB_REL_RW_COUNT 0x%x\n", req_p->rel_wr_count);
#endif
			resp_p->status = rpmb_write(req_buf, req_p->num_sectors, req_p->rel_wr_count, resp_buf, &resp_p->res_buff_len);
			break;
		default:
			dprintf(CRITICAL, "Unsupported request command request: %u\n", req_p->cmd_id);
			ASSERT(0);
	};

	resp_p->res_buff_offset = sizeof(struct tz_rpmb_rw_resp);
	resp_p->cmd_id = req_p->cmd_id;
}

int rpmb_cmd_handler(void *buf, uint32_t sz)
{
	int ret = 0;
	uint32_t cmd_id;

	ASSERT(buf);

	cmd_id = (uint32_t) *((uint32_t *)buf);

	switch(cmd_id)
	{
		case TZ_CM_CMD_RPMB_READ:
		case TZ_CM_CMD_RPMB_WRITE:
			handle_rw_request(buf, sz);
			break;
		case TZ_CM_CMD_RPMB_INIT:
#if DEBUG_RPMB
			dprintf(INFO, "RPMB init received\n");
#endif
			handle_init_request(buf, sz);
			break;
		case TZ_CM_CMD_RPMB_PARTITION:
#if DEBUG_RPMB
			dprintf(INFO, "Partition init received\n");
#endif
			ret = -1;
			break;
		default:
			/* Does qseecom need a response here? */
			dprintf(CRITICAL, "Unsupported Request from qseecom: %d\n", cmd_id);
			ASSERT(0);
	};

	return ret;
}

int rpmb_listener_start()
{
	int ret;
	struct qseecom_listener_services rpmb_listener;

	rpmb_listener.service_name = "RPMB system services";
	rpmb_listener.id           =  RPMB_LSTNR_ID;
	rpmb_listener.sb_size      = 25 * 1024;
	rpmb_listener.service_cmd_handler = rpmb_cmd_handler;

	ret = qseecom_register_listener(&rpmb_listener);

	if (ret < 0)
		dprintf(CRITICAL, "Failed to register rpmb listener\n");

	return ret;
}

int rpmb_listener_stop(int id)
{
	int ret;

	ret = qseecom_deregister_listener(id);

	if (ret < 0)
		dprintf(CRITICAL, "Failed to unregister rpmb listener\n");

	return ret;
}

