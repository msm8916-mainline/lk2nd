/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#ifndef _UTP_H_
#define _UTP_H_

#include <ufs_hci.h>
#include <upiu.h>
#include <ucs.h>
#include <dme.h>

/* UPIU general header length without the extra segments. */
#define UPIU_HDR_LEN                                       0x20

#define UTP_MAX_PRD_DATA_BYTE_CNT                          0x40000
#define UTP_MAX_PRD_DATA_BYTE_CNT_BYTE_SHIFT               18
#define UTP_MAX_PRD_TABLE_ENTRIES                          1024

#define UTP_CMD_DESC_BASE_ALIGNMENT_SIZE                   128
#define UTP_REQ_BUILD_CMD_DD_IRQ_FIELD(cmd, dd, irq)       ((cmd << 4) | (dd << 1) | irq)

#define UTP_MUTEX_ACQUIRE_TIMEOUT                          0x100000

#define UTP_GENERIC_CMD_TIMEOUT                            40000
#define UTP_MAX_COMMAND_RETRY                              5000000

struct utp_prdt_entry
{
	uint32_t data_base_addr;
	uint32_t data_upper_addr;
	uint8_t  resv_0[4];
	uint32_t data_byte_cnt;
}__PACKED;

struct utp_trans_req_desc
{
	uint8_t  resv_0[3];
	uint8_t  cmd_type_dd_irq;
	uint8_t  resv_1[4];
	uint8_t  overall_cmd_status;
	uint8_t  resv_2[7];
	uint8_t  cmd_desc_base_addr[4];
	uint8_t  cmd_desc_base_addr_upper[4];
	uint16_t resp_upiu_len;
	uint16_t resp_upiu_offset;
	uint16_t prdt_len;
	uint16_t prdt_offset;
}__PACKED;

enum utp_utrd_irq_type
{
	UTRD_REGULAR_CMD  = 0,
	UTRD_IRQ_CMD      = 1,
};

enum utp_utrd_overall_status_type
{
	UTRD_OCS_SUCCESS                     = 0x0,
	UTRD_OCS_INVALID_COM_TABLE_ATTR      = 0x1,
	UTRD_OCS_INVALID_PRDT_ATTR           = 0x2,
	UTRD_OCS_MISMATCH_DATA_BUFFER_SIZE   = 0x3,
	UTRD_OCS_MISMATCH_RESPONSE_UPIU_SIZE = 0x4,
	UTRD_OCS_COMMUNICATION_FAILURE       = 0x5,
	UTRD_OCS_ABORTED                     = 0x6,
	UTRD_OCS_INVALID_OCS_VALUE           = 0xf,
};

struct utp_utrd_req_build_type
{
	struct upiu_basic_hdr         *req_upiu;
	uint32_t                          req_upiu_len;
	uint32_t                          resp_upiu_len;
	enum upiu_cmd_type            cmd_type;
	enum upiu_dd_type             dd;
	enum utp_utrd_irq_type            irq;
	enum utp_utrd_overall_status_type ocs;
	uint32_t                          prdt_offset;
	uint32_t                          prdt_len;
	uint32_t                          timeout;
};

struct utp_task_mgmt_req_desc
{
	uint8_t  resv_0[3];
	uint8_t  irq;
	uint8_t  resv_1[4];
	uint8_t  overall_cmd_status;
	uint8_t  resv_2[7];
}__PACKED;

enum utp_utmrd_irq_type
{
	NO_IRQ     = 0,
	ENABLE_IRQ = 1,
};

enum utp_utmrd_overall_status_type
{
	UTMRD_OCS_SUCCESS                     = 0x0,
	UTMRD_OCS_TASK_MGMT_FUNC_ATTR         = 0x1,
	UTMRD_OCS_MISMATCH_TASK_MGMT_REQ_SIZE = 0x2,
	UTMRD_OCS_MISMATCH_TASK_MGMT_RES_SIZE = 0x3,
	UTMRD_OCS_PEER_COMM_FAILURE           = 0x4,
	UTMRD_OCS_ABORTED                     = 0x5,
	UTMRD_OCS_FATAL_ERR                   = 0x6,
};

struct utrd_cmd_desc
{
	struct upiu_gen_hdr *req_upiu;
	uint32_t                resp_upiu_len;
	uint32_t                num_prdt;
};

struct utp_bitmap_access_type
{
	uint32_t *bitmap;
	uint32_t door_bell_bit;
	mutex_t *mutx;
};

int utp_enqueue_upiu(struct ufs_dev *dev, struct upiu_req_build_type *upiu_data);
void utp_process_req_completion(struct ufs_req_irq_type *irq);
int utp_poll_utrd_complete(struct ufs_dev *dev);
#endif
