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

#include <debug.h>
#include <string.h>
#include <reg.h>
#include <arch/defines.h>
#include <sys/types.h>
#include <stdlib.h>
#include <endian.h>
#include <ufs.h>
#include <dme.h>
#include <uic.h>
#include <utp.h>

int dme_send_linkstartup_req(struct ufs_dev *dev)
{
	struct uic_cmd cmd;

	cmd.uiccmd        = UICCMDR_DME_LINKSTARTUP;
	cmd.num_args      = UICCMD_NO_ARGS;
	cmd.timeout_msecs = DME_LINK_START_TIMEOUT;

	if (uic_send_cmd(dev, &cmd) || cmd.gen_err_code == UICCMD_FAILURE)
		goto dme_send_linkstartup_req_err;

	return UFS_SUCCESS;

dme_send_linkstartup_req_err:
	dprintf(CRITICAL, "DME_LINKSTARTUP command failed.\n");
	return -UFS_FAILURE;
}

int dme_get_req(struct ufs_dev *dev, struct dme_get_req_type *req)
{
	struct uic_cmd cmd;

	cmd.uiccmd	      = UICCMDR_DME_GET;
	cmd.num_args      = UICCMD_ONE_ARGS;
	cmd.uiccmdarg1    = req->attribute << 16 | req->index;
	cmd.timeout_msecs = INFINITE_TIME;

	if (uic_send_cmd(dev, &cmd) || cmd.gen_err_code == UICCMD_FAILURE)
		goto dme_get_req_err;

	/* Return the result. */
	*(req->mibval) = cmd.uiccmdarg3;

	return UFS_SUCCESS;

dme_get_req_err:
	dprintf(CRITICAL, "DME_GET command failed.\n");
	return -UFS_FAILURE;
}

static int dme_get_query_resp(struct ufs_dev *dev,
					          struct upiu_req_build_type *req_upiu,
					          addr_t buffer,
					          size_t buf_len)
{
	struct upiu_trans_mgmt_query_hdr *resp_upiu;

	resp_upiu = (struct upiu_trans_mgmt_query_hdr *) req_upiu->resp_ptr;

	if (resp_upiu->opcode != req_upiu->opcode)
		return -UFS_FAILURE;
	if (resp_upiu->basic_hdr.response != UPIU_QUERY_RESP_SUCCESS)
		return -UFS_FAILURE;

	switch (resp_upiu->opcode)
	{
		case UPIU_QUERY_OP_READ_FLAG:
		case UPIU_QUERY_OP_SET_FLAG:
									  if (buf_len < sizeof(uint32_t))
									  {
										dprintf(CRITICAL, "Insufficient buffer space.\n");
										return -UFS_FAILURE;
									  }

									  *((uint32_t *) buffer) = resp_upiu->flag_value;
									  break;
		case UPIU_QUERY_OP_TOGGLE_FLAG:
		case UPIU_QUERY_OP_CLEAR_FLAG:
		case UPIU_QUERY_OP_READ_DESCRIPTOR:
									 break;
		default: dprintf(CRITICAL, "UPIU query opcode not supported.\n");
				 return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}

static int dme_send_query_upiu(struct ufs_dev *dev, struct utp_query_req_upiu_type *query)
{
	struct upiu_trans_mgmt_query_hdr resp_upiu;
	struct upiu_req_build_type       req_upiu;
	int                              ret;

	memset(&req_upiu, 0, sizeof(req_upiu));

	req_upiu.opcode        = query->opcode;
	req_upiu.selector      = query->selector;
	req_upiu.index         = query->index;
	req_upiu.idn           = query->idn;
	req_upiu.trans_type    = UPIU_TYPE_QUERY_REQ;
	req_upiu.dd            = UTRD_NO_DATA_TRANSFER;
	req_upiu.resp_ptr      = (struct upiu_basic_hdr *) &resp_upiu;
	req_upiu.resp_len      = sizeof(resp_upiu);
	req_upiu.resp_data_ptr = query->buf;
	req_upiu.timeout_msecs = UTP_GENERIC_CMD_TIMEOUT;

	if (query->opcode == UPIU_QUERY_OP_READ_DESCRIPTOR)
	{
		req_upiu.resp_data_len = query->buf_len;
	}

	ret = utp_enqueue_upiu(dev, &req_upiu);
	if (ret)
		goto utp_send_query_upiu_err;

	ret = dme_get_query_resp(dev, &req_upiu, query->buf, query->buf_len);
	if (ret)
		goto utp_send_query_upiu_err;

utp_send_query_upiu_err:
	return ret;
}

int dme_set_fpoweronwpen(struct ufs_dev *dev)
{
	STACKBUF_DMA_ALIGN(result, sizeof(uint32_t));
	uint32_t try_again                        = DME_FPOWERONWPEN_RETRIES;
	struct utp_query_req_upiu_type read_query = {UPIU_QUERY_OP_READ_FLAG,
                                                 UFS_IDX_fPowerOnWPEn,
                                                 0,
                                                 0,
                                                 (addr_t) result,
                                                 sizeof(uint32_t)};
	struct utp_query_req_upiu_type set_query  = {UPIU_QUERY_OP_SET_FLAG,
                                                 UFS_IDX_fPowerOnWPEn,
                                                 0,
                                                 0,
                                                 (addr_t) result,
                                                 sizeof(uint32_t)};


	if (dme_send_query_upiu(dev, &read_query))
		return -UFS_FAILURE;

	arch_invalidate_cache_range((addr_t) result, sizeof(uint32_t));

	if (*result == 1)
		goto utp_set_fpoweronwpen_done;

	do
	{
		try_again--;
		dprintf(CRITICAL, "Power on Write Protect request failed. Retrying again.\n");

		if (dme_send_query_upiu(dev, &set_query))
			return -UFS_FAILURE;
		if (dme_send_query_upiu(dev, &read_query))
			return -UFS_FAILURE;

		if (*result == 1)
			break;
	} while (try_again);

utp_set_fpoweronwpen_done:
	dprintf(INFO,"Power on Write Protect status: %u\n", *result);
	return UFS_SUCCESS;
}

int dme_set_fdeviceinit(struct ufs_dev *dev)
{
	STACKBUF_DMA_ALIGN(result, sizeof(uint32_t));
	uint32_t try_again                        = DME_FDEVICEINIT_RETRIES;
	struct utp_query_req_upiu_type read_query = {UPIU_QUERY_OP_READ_FLAG,
												 UFS_IDX_fDeviceInit,
												 0,
												 0,
												 (addr_t) result,
												 sizeof(uint32_t)};
	struct utp_query_req_upiu_type set_query  = {UPIU_QUERY_OP_SET_FLAG,
												 UFS_IDX_fDeviceInit,
												 0,
												 0,
												 (addr_t) result,
												 sizeof(uint32_t)};


	if (dme_send_query_upiu(dev, &read_query))
		return -UFS_FAILURE;

	arch_invalidate_cache_range((addr_t) result, sizeof(uint32_t));

	if (*result == 1)
		goto utp_set_fdeviceinit_done;

	do
	{
		try_again--;

		if (dme_send_query_upiu(dev, &set_query))
			return -UFS_FAILURE;

		if (dme_send_query_upiu(dev, &read_query))
			return -UFS_FAILURE;

		if (*result == 1)
			break;
	} while (try_again);

utp_set_fdeviceinit_done:
	return UFS_SUCCESS;
}

int dme_read_string_desc(struct ufs_dev *dev, uint8_t index, struct ufs_string_desc *desc)
{
	struct utp_query_req_upiu_type query = {UPIU_QUERY_OP_READ_DESCRIPTOR,
											UFS_DESC_IDN_STRING,
											index,
											0,
											(addr_t) desc,
											sizeof(struct ufs_string_desc)};

	if (dme_send_query_upiu(dev, &query))
		return -UFS_FAILURE;

	if (desc->desc_len != 0)
		return UFS_SUCCESS;

	return -UFS_FAILURE;
}

static uint32_t dme_parse_serial_no(struct ufs_string_desc *desc)
{
	uint32_t serial_no=0;
	uint16_t *ptr;
	int index=0,len=0;

	if(desc->desc_len <= 0)
		return -UFS_FAILURE;

	ptr = desc->serial_num;
	len = (desc->desc_len-2)/2;

	for(index=0; index<len; index++)
	{
		serial_no += *ptr;
		ptr++;
	}

	return serial_no;
}

int dme_read_device_desc(struct ufs_dev *dev)
{
	STACKBUF_DMA_ALIGN(dev_desc, sizeof(struct ufs_dev_desc));
	STACKBUF_DMA_ALIGN(desc, sizeof(struct ufs_string_desc));
	struct ufs_dev_desc            *device_desc = dev_desc;
	struct ufs_string_desc         *str_desc    = desc;
	struct utp_query_req_upiu_type query = {UPIU_QUERY_OP_READ_DESCRIPTOR,
											UFS_DESC_IDN_DEVICE,
											0,
											0,
											(addr_t) dev_desc,
											sizeof(struct ufs_dev_desc)};

	if (dme_send_query_upiu(dev, &query))
		return -UFS_FAILURE;

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) device_desc, sizeof(struct ufs_dev_desc));

	/* Store all relevant data */
	dev->num_lus = device_desc->num_lu;

	/* Get serial number for the device based on the string index. */
	if (dme_read_string_desc(dev, device_desc->serial_num, desc))
		return -UFS_FAILURE;

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) str_desc, sizeof(struct ufs_string_desc));

	dev->serial_num = dme_parse_serial_no(str_desc);
	
	return UFS_SUCCESS;
}

int dme_read_unit_desc(struct ufs_dev *dev, uint8_t index)
{
	STACKBUF_DMA_ALIGN(unit_desc, sizeof(struct ufs_unit_desc));
	struct ufs_unit_desc           *desc = unit_desc;
	struct utp_query_req_upiu_type query = {UPIU_QUERY_OP_READ_DESCRIPTOR,
											UFS_DESC_IDN_UNIT,
											index,
											0,
											(addr_t) unit_desc,
											sizeof(struct ufs_unit_desc)};

	if (dme_send_query_upiu(dev, &query))
		return -UFS_FAILURE;

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) desc, sizeof(struct ufs_unit_desc));

	dev->lun_cfg[index].logical_blk_cnt = BE64(desc->logical_blk_cnt);

	dev->lun_cfg[index].erase_blk_size = BE32(desc->erase_blk_size);

	return UFS_SUCCESS;
}

int dme_read_config_desc(struct ufs_dev *dev)
{
	STACKBUF_DMA_ALIGN(desc, sizeof(struct ufs_config_desc));
	struct ufs_config_desc         *config_desc = desc;
	struct utp_query_req_upiu_type query = {UPIU_QUERY_OP_READ_DESCRIPTOR,
											UFS_DESC_IDN_CONFIGURATION,
											0,
											0,
											(addr_t) config_desc,
											sizeof(struct ufs_config_desc)};

	if (dme_send_query_upiu(dev, &query))
		return -UFS_FAILURE;

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) config_desc, sizeof(struct ufs_config_desc));

	return UFS_SUCCESS;
}

int dme_send_nop_query(struct ufs_dev *dev)
{
	struct upiu_req_build_type     req_upiu;
	struct upiu_basic_hdr          resp_upiu;
	int                            ret;
	unsigned                       try_again;

	ret       = UFS_SUCCESS;
	try_again = DME_NOP_NUM_RETRIES;

	memset(&req_upiu, 0 , sizeof(struct upiu_req_build_type));

	req_upiu.trans_type        = UPIU_TYPE_NOP_OUT;
	req_upiu.flags             = 0;
	req_upiu.query_mgmt_func   = 0;
	req_upiu.cmd_type          = UTRD_DEV_MGMT_FUNC;
	req_upiu.dd                = UTRD_NO_DATA_TRANSFER;
	req_upiu.resp_ptr          = &resp_upiu;
	req_upiu.resp_len          = sizeof(struct upiu_basic_hdr);
	req_upiu.timeout_msecs     = DME_NOP_QUERY_TIMEOUT;

	while (try_again)
	{
		try_again--;

		ret = utp_enqueue_upiu(dev, &req_upiu);

		if (ret == -UFS_RETRY)
		{
			continue;
		}
		else if (ret == -UFS_FAILURE)
		{
			dprintf(CRITICAL, "sending nop out failed.\n");
			goto upiu_send_nop_out_err;
		}

		/* Check response UPIU */
		if (resp_upiu.trans_type != UPIU_TYPE_NOP_IN)
		{
			dprintf(CRITICAL, "Command failed. command = %x. Invalid response.\n", req_upiu.trans_type);
			ret = -UFS_FAILURE;
			goto upiu_send_nop_out_err;
		}
		else
			break;
	}

upiu_send_nop_out_err:
	return ret;
}

int utp_build_query_req_upiu(struct upiu_trans_mgmt_query_hdr *req_upiu,
							 struct upiu_req_build_type *upiu_data)
{
	req_upiu->opcode    = upiu_data->opcode;
	req_upiu->idn       = upiu_data->idn;
	req_upiu->index     = upiu_data->index;
	req_upiu->selector  = upiu_data->selector;
	req_upiu->resp_len  = BE16(upiu_data->resp_data_len);

	switch (upiu_data->opcode)
	{
		case UPIU_QUERY_OP_READ_FLAG:
		case UPIU_QUERY_OP_READ_DESCRIPTOR:
											req_upiu->basic_hdr.query_task_mgmt_func = UPIU_QUERY_FUNC_STD_READ_REQ;
											break;
		case UPIU_QUERY_OP_TOGGLE_FLAG:
		case UPIU_QUERY_OP_CLEAR_FLAG:
		case UPIU_QUERY_OP_SET_FLAG:
									 req_upiu->basic_hdr.query_task_mgmt_func = UPIU_QUERY_FUNC_STD_WRITE_REQ;
									 break;
		default: dprintf(CRITICAL, "UPIU query opcode not supported.\n");
				 return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}
