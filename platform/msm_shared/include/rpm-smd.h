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

#ifndef __RPM_SMD_H
#define __RPM_SMD_H

#include <arch/defines.h>
#include <stdint.h>
#include <sys/types.h>

typedef enum
{
	RPM_REQUEST_TYPE,
	RPM_CMD_TYPE,
	RPM_SUCCESS_REQ_ACK,
	RPM_SUCCESS_CMD_ACK,
	RPM_ERROR_ACK,
}msg_type;

enum
{
	RESOURCETYPE,
	RESOURCEID,
	KVP_KEY,
	KVP_LENGTH,
	KVP_VALUE,
};

typedef struct
{
	uint32_t type;
	uint32_t len;
} rpm_gen_hdr;

typedef struct
{
	uint32_t key;
	uint32_t len;
	uint32_t val;
} kvp_data;

typedef struct
{
	uint32_t id;
	uint32_t set;
	uint32_t resourceType;
	uint32_t resourceId;
	uint32_t dataLength;
}rpm_req_hdr;

typedef struct
{
	rpm_gen_hdr hdr;
	rpm_req_hdr req_hdr;
	kvp_data *data;
} rpm_req;

typedef struct
{
	rpm_gen_hdr hdr;
	kvp_data *data;
} rpm_cmd;

typedef rpm_cmd rpm_ack_msg;
int rpm_send_data(uint32_t *data, uint32_t len, msg_type type);
uint32_t rpm_recv_data(uint32_t *len);
void rpm_clk_enable(uint32_t *data, uint32_t len);
void rpm_clk_disable(uint32_t *data, uint32_t len);
void rpm_smd_init();
void rpm_smd_uninit();
#endif
