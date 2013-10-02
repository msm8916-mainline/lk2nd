/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#ifndef _UPIU_H
#define _UPIU_H

struct upiu_basic_hdr
{
	uint8_t  trans_type;
	uint8_t  flags;
	uint8_t  lun;
	uint8_t  task_tag;
	uint8_t  cmd_set_type;
	uint8_t  query_task_mgmt_func;
	uint8_t  response;
	uint8_t  status;
	uint8_t  total_ehs_len;
	uint8_t  device_info;
	uint16_t data_seg_len;
} __PACKED;

struct upiu_trans_mgmt_query_hdr
{
	struct upiu_basic_hdr basic_hdr;
	uint8_t                   opcode;
	uint8_t                   idn;
	uint8_t                   index;
	uint8_t                   selector;
	uint8_t                   resv_0[2];
	uint16_t                  resp_len;
	uint8_t                   resv_1[3];
	uint8_t                   flag_value;
	uint8_t                   resv_2[4];
}__PACKED;

struct upiu_cmd_hdr
{
	struct upiu_basic_hdr basic_hdr;
	uint32_t			      data_expected_len;	// Requested length
	uint8_t				      param[16];			// Payload, operation specefic field
}__PACKED;

struct upiu_cmd_resp_hdr
{
	struct upiu_basic_hdr basic_hdr;
	uint32_t			      residual_trans_count;
	uint8_t				      resv_0[16];
}__PACKED;

struct upiu_gen_hdr
{
	struct upiu_basic_hdr basic_hdr;
	uint8_t			          trans_specific_fields[20];
}__PACKED;

/* UPIU transaction codes. */
enum upiu_trans_type
{
	UPIU_TYPE_NOP_OUT              = 0x00,
	UPIU_TYPE_COMMAND              = 0x01,
	UPIU_TYPE_TASK_MGMT            = 0x04,
	UPIU_TYPE_QUERY_REQ            = 0x16,
	UPIU_TYPE_NOP_IN               = 0x20,
	UPIU_TYPE_RESPONSE             = 0x21,
	UPIU_TYPE_TASK_MAN_RESP        = 0x24,
	UPIU_TYPE_QUERY_RESP           = 0x36,
	UPIU_TYPE_REJECT               = 0x3f,
};

/* UPIU respones */
enum upiu_response
{
	UPIU_RESPONSE_TARGET_SUCCESS   = 0x00,
	UPIU_RESPONSE_TARGET_FAILURE   = 0x01,
};

enum upiu_cmd_set_type
{
	UPIU_SCSI_CMD_SET           = 0,
	UPIU_UFS_SPECIFIC_CMD_SET   = 1,
};

enum upiu_query_opcode_type
{
	UPIU_QUERY_OP_NOP              = 0x0,
	UPIU_QUERY_OP_READ_DESCRIPTOR  = 0x1,
	UPIU_QUERY_OP_WRITE_DESCRIPTOR = 0x2,
	UPIU_QUERY_OP_READ_ATTRIBUTE   = 0x3,
	UPIU_QUERY_OP_WRITE_ATTRIBUTE  = 0x4,
	UPIU_QUERY_OP_READ_FLAG        = 0x5,
	UPIU_QUERY_OP_SET_FLAG         = 0x6,
	UPIU_QUERY_OP_CLEAR_FLAG       = 0x7,
	UPIU_QUERY_OP_TOGGLE_FLAG      = 0x8,
};

enum upiu_cmd_type
{
	UTRD_SCSCI_CMD        = 0,
	UTRD_NATIVE_UFS_CMD   = 1,
	UTRD_DEV_MGMT_FUNC    = 2,
};

enum upiu_dd_type
{
	UTRD_NO_DATA_TRANSFER  = 0,
	UTRD_SYSTEM_TO_TARGET  = 1,
	UTRD_TARGET_TO_SYSTEM  = 2,
};

struct upiu_req_build_type
{
	enum upiu_trans_type        trans_type;
	uint8_t                         flags;
	uint8_t                         lun;
	enum upiu_cmd_set_type      cmd_set_type;
	uint8_t                         query_mgmt_func;
	uint8_t                         ehs_len;
	uint16_t                        data_seg_len;
	addr_t                          data_buffer_addr;
	uint32_t                        data_buffer_len;
	addr_t                          cdb;
	uint64_t                        expected_data_len;
	enum upiu_query_opcode_type opcode;
	uint8_t                         idn;
	uint8_t                         index;
	uint8_t                         selector;
	struct upiu_basic_hdr       *resp_ptr;
	uint64_t                        resp_len;
	uint16_t                        resp_data_len;
	addr_t                        resp_data_ptr;

	/* UTRD properties. */
	enum upiu_cmd_type          cmd_type;
	enum upiu_dd_type           dd;
	uint64_t                        timeout_msecs;
};


#endif
