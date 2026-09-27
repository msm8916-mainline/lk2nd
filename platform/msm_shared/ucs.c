/* Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
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
#include <stdlib.h>
#include <endian.h>
#include <string.h>
#include <utp.h>
#include <rpmb.h>

int ucs_do_scsi_cmd(struct ufs_dev *dev, struct scsi_req_build_type *req)
{
	struct upiu_req_build_type req_upiu;
	struct upiu_basic_resp_hdr      resp_upiu;

	memset(&req_upiu, 0 , sizeof(struct upiu_req_build_type));

	req_upiu.cmd_set_type	   = UPIU_SCSI_CMD_SET;
	req_upiu.trans_type	       = UPIU_TYPE_COMMAND;
	req_upiu.data_buffer_addr  = req->data_buffer_addr;
	req_upiu.expected_data_len = req->data_len;
	req_upiu.data_seg_len	   = 0;
	req_upiu.ehs_len		   = 0;
	req_upiu.flags			   = req->flags;
	req_upiu.lun			   = req->lun;
	req_upiu.query_mgmt_func   = 0;
	req_upiu.cdb			   = req->cdb;
	req_upiu.cmd_type		   = UTRD_SCSCI_CMD;
	req_upiu.dd			       = req->dd;
	req_upiu.resp_ptr		   = &resp_upiu;
	req_upiu.resp_len		   = sizeof(resp_upiu);
	req_upiu.timeout_msecs	   = UTP_GENERIC_CMD_TIMEOUT;

	if (utp_enqueue_upiu(dev, &req_upiu))
	{
		dprintf(CRITICAL, "ucs_do_scsi_cmd: enqueue failed\n");
		return -UFS_FAILURE;
	}

	if (resp_upiu.status != SCSI_STATUS_GOOD)
	{
		if (resp_upiu.status == SCSI_STATUS_CHK_COND && (*((uint8_t *)(req->cdb)) != SCSI_CMD_SENSE_REQ))
		{
			dprintf(CRITICAL, "Data segment length: %x\n", BE16(resp_upiu.data_seg_len));
			if (BE16(resp_upiu.data_seg_len))
			{
				dprintf(CRITICAL, "SCSI Request failed and we have sense data\n");
				dprintf(CRITICAL, "Sense Data Length/Response Code: 0x%x/0x%x\n", BE16(resp_upiu.sense_length), BE16(resp_upiu.sense_response_code));
				parse_sense_key(resp_upiu.sense_data[0]);
				dprintf(CRITICAL, "Sense Buffer (HEX): 0x%x 0x%x 0x%x 0x%x\n", BE32(resp_upiu.sense_data[0]), BE32(resp_upiu.sense_data[1]), BE32(resp_upiu.sense_data[2]), BE32(resp_upiu.sense_data[3]));
			}
		}

		dprintf(CRITICAL, "ucs_do_scsi_cmd failed status = %x\n", resp_upiu.status);
		return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}

int parse_sense_key(uint32_t sense_data)
{
	uint32_t key = BE32(sense_data) >> 24;
	dprintf(CRITICAL, "Sense Key: 0x%x\n", key);
	switch(key)
	{
		case 0x0:
			dprintf(INFO, "NO SENSE: No information available to be reported\n");
			break;
		case 0x1:
			dprintf(INFO, "RECOVERED ERROR: Additional sense buffer bytes indicate further details\n");
			break;
		case 0x2:
			dprintf(INFO, "NOT READY: Logical Unit Not Ready and cannot be accessed at this time\n");
			break;
		case 0x3:
			dprintf(INFO, "MEDIUM ERROR: Last command unsuccessful due to non-recoverable error condition\n");
			break;
		case 0x4:
			dprintf(INFO, "HARDWARE ERROR: Target detected a non-recoverable hardware error\n");
			break;
		case 0x5:
			dprintf(INFO, "ILLEGAL REQUEST: Illegal parameter in the command descriptor block in the command sent\n");
			break;
		case 0x6:
			dprintf(INFO, "UNIT ATTENTION: Unit has been reset/unexpectedly power on/removable media has changed\n");
			break;
		case 0x7:
			dprintf(INFO, "DATA PROTECT: Read/Write operation attempted on a block that is protected from this operation\n");
			break;
		case 0x8:
			dprintf(INFO, "BLANK CHECK: Target encountered blank or unformatted media while reading or writing\n");
			break;
		case 0x9:
			dprintf(INFO, "VENDOR SPECIFIC: Vendor specific error or exceptional conditions\n");
			break;
		case 0xB:
			dprintf(INFO, "ABORTED COMMAND: Target aborted the execution of the command\n");
			break;
		case 0xD:
			dprintf(INFO, "VOLUME OVERFLOW: Buffered peripheral device has reached the end of partition\n");
			break;
		case 0xE:
			dprintf(INFO, "MISCOMPARE: Source data did not match the data read from the media\n");
			break;
		default:
			dprintf(INFO, "INVALID sense key\n");
	}
	return key;
}

int ucs_do_scsi_rpmb_read(struct ufs_dev *dev, uint32_t *req_buf, uint32_t blk_cnt,
                                 uint32_t *resp_buf, uint32_t *resp_len)
{
	// validate input parameters
	ASSERT(req_buf);
	ASSERT(resp_buf);
	ASSERT(resp_len);

	STACKBUF_DMA_ALIGN(cdb, sizeof(struct scsi_sec_protocol_cdb));
	struct scsi_req_build_type   req_upiu;
	struct scsi_sec_protocol_cdb *cdb_out_param, *cdb_in_param;
	uint32_t                     blks_remaining;
	uint32_t                     blks_to_transfer;
	uint64_t                     bytes_to_transfer;
	uint64_t                     max_size;
	blks_remaining    = blk_cnt;
	blks_to_transfer  = blks_remaining;
	bytes_to_transfer = blks_to_transfer * RPMB_FRAME_SIZE;

	// check if total bytes to transfer exceed max supported size
	max_size = dev->rpmb_rw_size * RPMB_FRAME_SIZE * blk_cnt;
	if (bytes_to_transfer > max_size)
	{
		dprintf(CRITICAL, "RPMB request transfer size %llu greater than max transfer size %llu\n", bytes_to_transfer, max_size);
		return -UFS_FAILURE;
	}
#ifdef DEBUG_UFS
	dprintf(INFO, "rpmb_read: req_buf: 0x%x blk_count: 0x%x\n", *req_buf, blk_cnt);
	dprintf(INFO, "rpmb_read: bytes_to_transfer: 0x%x blks_to_transfer: 0x%x\n",
                   bytes_to_transfer, blks_to_transfer);
#endif
	// send the request
	cdb_out_param = (struct scsi_sec_protocol_cdb*) cdb;
	memset(cdb_out_param, 0, sizeof(struct scsi_sec_protocol_cdb));

	cdb_out_param->opcode                = SCSI_CMD_SECPROT_OUT;
	cdb_out_param->cdb1                  = SCSI_SEC_PROT;
	cdb_out_param->sec_protocol_specific = BE16(SCSI_SEC_UFS_PROT_ID);
	cdb_out_param->alloc_tlen            = BE32(bytes_to_transfer);

	// Flush CDB to memory
	dsb();
	arch_clean_invalidate_cache_range((addr_t) cdb_out_param, sizeof(struct scsi_sec_protocol_cdb));

	memset(&req_upiu, 0, sizeof(struct scsi_req_build_type));

	req_upiu.cdb              = (addr_t) cdb_out_param;
	req_upiu.data_buffer_addr = (addr_t) req_buf;
	req_upiu.data_len         = bytes_to_transfer;
	req_upiu.flags            = UPIU_FLAGS_WRITE;
	req_upiu.lun              = UFS_WLUN_RPMB;
	req_upiu.dd               = UTRD_TARGET_TO_SYSTEM;

#ifdef DEBUG_UFS
	dprintf(INFO, "Sending RPMB Read request\n");
#endif
	if (ucs_do_scsi_cmd(dev, &req_upiu))
	{
		dprintf(CRITICAL, "%s:%d ucs_do_scsi_rpmb_read: failed\n", __func__, __LINE__);
		return -UFS_FAILURE;
	}
#ifdef DEBUG_UFS
	dprintf(INFO, "Sending RPMB Read request complete\n");
#endif
	// read the response
	cdb_in_param = (struct scsi_sec_protocol_cdb*) cdb;
	memset(cdb_in_param, 0, sizeof(struct scsi_sec_protocol_cdb));

	cdb_in_param->opcode                = SCSI_CMD_SECPROT_IN;
	cdb_in_param->cdb1                  = SCSI_SEC_PROT;
	cdb_in_param->sec_protocol_specific = BE16(SCSI_SEC_UFS_PROT_ID);
	cdb_in_param->alloc_tlen            = BE32(bytes_to_transfer);

	// Flush CDB to memory
	dsb();
	arch_clean_invalidate_cache_range((addr_t) cdb_in_param, sizeof(struct scsi_sec_protocol_cdb));

	memset(&req_upiu, 0, sizeof(struct scsi_req_build_type));

	req_upiu.cdb              = (addr_t) cdb_in_param;
	req_upiu.data_buffer_addr = (addr_t) resp_buf;
	req_upiu.data_len         = bytes_to_transfer;
	req_upiu.flags            = UPIU_FLAGS_READ;
	req_upiu.lun              = UFS_WLUN_RPMB;
	req_upiu.dd               = UTRD_SYSTEM_TO_TARGET;

#ifdef DEBUG_UFS
	dprintf(INFO, "Sending RPMB Read response\n");
#endif
	if (ucs_do_scsi_cmd(dev, &req_upiu))
	{
		dprintf(CRITICAL, "%s:%d ucs_do_scsi_rpmb_read: failed\n", __func__, __LINE__);
		return -UFS_FAILURE;
	}
#ifdef DEBUG_UFS
	dprintf(SPEW, "Sending RPMB Read response complete\n");
#endif
	*resp_len = bytes_to_transfer;
	return UFS_SUCCESS;
}

int ucs_do_scsi_read(struct ufs_dev *dev, struct scsi_rdwr_req *req)
{
	STACKBUF_DMA_ALIGN(cdb, sizeof(struct scsi_rdwr_cdb));
	struct scsi_req_build_type     req_upiu;
	struct scsi_rdwr_cdb           *cdb_param;
	uint32_t                       blks_remaining;
	uint16_t                       blks_to_transfer;
	uint64_t                       bytes_to_transfer;
	uint32_t                       start_blk;
	uint32_t                       buf;

	blks_remaining = req->num_blocks;
	buf            = req->data_buffer_base;
	start_blk      = req->start_lba;

	cdb_param = (struct scsi_rdwr_cdb*) cdb;
	while (blks_remaining)
	{
		if (blks_remaining <= SCSI_MAX_DATA_TRANS_BLK_LEN)
		{
			blks_to_transfer = blks_remaining;
			blks_remaining   = 0;
		}
		else
		{
			blks_to_transfer = SCSI_MAX_DATA_TRANS_BLK_LEN;
			blks_remaining  -= SCSI_MAX_DATA_TRANS_BLK_LEN;
		}

		bytes_to_transfer = blks_to_transfer * UFS_DEFAULT_SECTORE_SIZE;

		memset(cdb_param, 0, sizeof(struct scsi_rdwr_cdb));
		cdb_param->opcode    = SCSI_CMD_READ10;
		cdb_param->cdb1      = SCSI_READ_WRITE_10_CDB1(0, 0, 1, 0);
		cdb_param->lba       = BE32(start_blk);
		cdb_param->trans_len = BE16(blks_to_transfer);


		dsb();
		arch_clean_invalidate_cache_range((addr_t) cdb_param, sizeof(struct scsi_rdwr_cdb));

		memset(&req_upiu, 0 , sizeof(struct scsi_req_build_type));

		req_upiu.cdb               = (addr_t) cdb_param;
		req_upiu.data_buffer_addr  = buf;
		req_upiu.data_len = bytes_to_transfer;
		req_upiu.flags             = UPIU_FLAGS_READ;
		req_upiu.lun               = req->lun;
		req_upiu.dd                = UTRD_TARGET_TO_SYSTEM;

		if (ucs_do_scsi_cmd(dev, &req_upiu))
		{
			dprintf(CRITICAL, "ucs_do_scsi_read: failed\n");
			return -UFS_FAILURE;
		}

		buf       += bytes_to_transfer;
		start_blk += SCSI_MAX_DATA_TRANS_BLK_LEN;
	}

	return UFS_SUCCESS;
}

int ucs_do_scsi_write(struct ufs_dev *dev, struct scsi_rdwr_req *req)
{
	struct scsi_req_build_type     req_upiu;
	STACKBUF_DMA_ALIGN(cdb, sizeof(struct scsi_rdwr_cdb));
	struct scsi_rdwr_cdb           *cdb_param;
	uint32_t                       blks_remaining;
	uint16_t                       blks_to_transfer;
	uint64_t                       bytes_to_transfer;
	uint32_t                       start_blk;
	uint32_t                       buf;

	blks_remaining = req->num_blocks;
	buf = req->data_buffer_base;
	start_blk = req->start_lba;

	cdb_param = (struct scsi_rdwr_cdb*) cdb;
	while (blks_remaining)
	{
		if (blks_remaining <= SCSI_MAX_DATA_TRANS_BLK_LEN)
		{
			blks_to_transfer = blks_remaining;
			blks_remaining   = 0;
		}
		else
		{
			blks_to_transfer = SCSI_MAX_DATA_TRANS_BLK_LEN;
			blks_remaining  -= SCSI_MAX_DATA_TRANS_BLK_LEN;
		}

		bytes_to_transfer = blks_to_transfer * UFS_DEFAULT_SECTORE_SIZE;

		memset(cdb_param, 0, sizeof(struct scsi_rdwr_cdb));
		cdb_param->opcode    = SCSI_CMD_WRITE10;
		cdb_param->cdb1      = SCSI_READ_WRITE_10_CDB1(0, 0, 1, 0);
		cdb_param->lba       = BE32(start_blk);
		cdb_param->trans_len = BE16(blks_to_transfer);

		/* Flush cdb to memory. */
		dsb();
		arch_clean_invalidate_cache_range((addr_t) cdb_param, sizeof(struct scsi_rdwr_cdb));

		memset(&req_upiu, 0 , sizeof(struct scsi_req_build_type));

		req_upiu.cdb               = (addr_t) cdb_param;
		req_upiu.data_buffer_addr  = buf;
		req_upiu.data_len          = bytes_to_transfer;
		req_upiu.flags             = UPIU_FLAGS_WRITE;
		req_upiu.lun               = req->lun;
		req_upiu.dd                = UTRD_SYSTEM_TO_TARGET;

		if (ucs_do_scsi_cmd(dev, &req_upiu))
		{
			dprintf(CRITICAL, "ucs_do_scsi_write: failed\n");
			return -UFS_FAILURE;
		}

		buf       += bytes_to_transfer;
		start_blk += SCSI_MAX_DATA_TRANS_BLK_LEN;
	}

	return UFS_SUCCESS;
}

int ucs_do_scsi_unmap(struct ufs_dev *dev, struct scsi_unmap_req *req)
{
	STACKBUF_DMA_ALIGN(cdb_param, SCSI_CDB_PARAM_LEN);
	STACKBUF_DMA_ALIGN(param, sizeof(struct unmap_param_list));
	struct scsi_req_build_type req_upiu;
	struct unmap_param_list *param_list;
	struct unmap_blk_desc *blk_desc;

	param_list                    = (struct unmap_param_list *)param;

	// data length = size of unmap block descriptor struct (n-1) - size of data length field.
	param_list->data_len          = ((sizeof(struct unmap_param_list) - 1) - 1) << 0x8;

	param_list->blk_desc_data_len = sizeof(struct unmap_blk_desc) << 0x8;


	blk_desc = &(param_list->blk_desc);

	blk_desc->lba      = BE64(req->start_lba);
	blk_desc->num_blks = BE32(req->num_blocks);

	memset((void*)cdb_param, 0, SCSI_CDB_PARAM_LEN);
	cdb_param[0] = SCSI_CMD_UNMAP;
	cdb_param[1] = 0;                    /*ANCHOR = 0 for UFS*/
	cdb_param[6] = 0;                    /*Group No = 0*/
	cdb_param[7] = 0;                    /* Param list length is 1, we erase 1 contiguous blk*/
	cdb_param[8] = sizeof(struct unmap_param_list);
	cdb_param[9] = 0;

        /* Flush cdb to memory. */
	dsb();
	arch_invalidate_cache_range((addr_t) cdb_param, SCSI_CDB_PARAM_LEN);
	arch_invalidate_cache_range((addr_t) param, sizeof(struct unmap_param_list));

	memset((void*)&req_upiu, 0 , sizeof(struct scsi_req_build_type));

	req_upiu.cdb                       = (addr_t) cdb_param;
	req_upiu.data_buffer_addr          = (addr_t) param;
	req_upiu.data_len                  = sizeof(struct unmap_param_list);
	req_upiu.flags                     = UPIU_FLAGS_WRITE;
	req_upiu.lun                       = req->lun;
	req_upiu.dd                        = UTRD_SYSTEM_TO_TARGET;

	if (ucs_do_scsi_cmd(dev, &req_upiu))
	{
		dprintf(CRITICAL, "Failed to send SCSI unmap command \n");
		return -UFS_FAILURE;
	}

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) param, SCSI_INQUIRY_LEN);

	return UFS_SUCCESS;
}

int ucs_scsi_send_inquiry(struct ufs_dev *dev)
{
	STACKBUF_DMA_ALIGN(cdb_param, SCSI_CDB_PARAM_LEN);
	STACKBUF_DMA_ALIGN(param, SCSI_INQUIRY_LEN);
	struct scsi_req_build_type req_upiu;

	memset(cdb_param, 0, SCSI_CDB_PARAM_LEN);
	cdb_param[0] = SCSI_CMD_INQUIRY;
	cdb_param[3] = sizeof(param)>> 8;
	cdb_param[4] = sizeof(param);

	/* Flush cdb to memory. */
	dsb();
	arch_clean_invalidate_cache_range((addr_t) cdb_param, SCSI_CDB_PARAM_LEN);

	memset(&req_upiu, 0 , sizeof(struct scsi_req_build_type));

	req_upiu.cdb			   = (addr_t) cdb_param;
	req_upiu.data_buffer_addr  = (addr_t) param;
	req_upiu.data_len          = SCSI_INQUIRY_LEN;
	req_upiu.flags			   = UPIU_FLAGS_READ;
	req_upiu.lun			   = 0;
	req_upiu.dd			       = UTRD_TARGET_TO_SYSTEM;

	if (ucs_do_scsi_cmd(dev, &req_upiu))
	{
		dprintf(CRITICAL, "Failed to send SCSI inquiry\n");
		return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}

void dump_sense_buffer(uint8_t *buf, int buf_len)
{
	int index=0;

	dprintf(CRITICAL,"----Sense buffer----\n");
	for(index=0; index < buf_len; index++)
		dprintf(CRITICAL,"buf[%d] = %x\n",index, buf[index]);

	dprintf(CRITICAL,"----end of buffer---\n");
}

int ucs_do_request_sense(struct ufs_dev *dev, uint8_t lun)
{
	STACKBUF_DMA_ALIGN(cdb, sizeof(struct scsi_sense_cdb));
	struct scsi_req_build_type req_upiu;
	struct scsi_sense_cdb      *cdb_param;
	STACKBUF_DMA_ALIGN(buf, SCSI_SENSE_BUF_LEN);

	cdb_param = (struct scsi_sense_cdb *) cdb;

	memset(cdb, 0, sizeof(struct scsi_sense_cdb));

	cdb_param->opcode    = SCSI_CMD_SENSE_REQ;
	cdb_param->alloc_len = SCSI_SENSE_BUF_LEN;

	/* Flush cdb to memory. */
	dsb();
	arch_clean_invalidate_cache_range((addr_t) cdb_param, SCSI_CDB_PARAM_LEN);

	memset(&req_upiu, 0 , sizeof(struct scsi_req_build_type));

	req_upiu.cdb			   = (addr_t) cdb_param;
	req_upiu.data_buffer_addr  = (addr_t) buf;
	req_upiu.data_len          = SCSI_SENSE_BUF_LEN;
	req_upiu.flags			   = UPIU_FLAGS_READ;
	req_upiu.lun			   = lun;
	req_upiu.dd			       = UTRD_TARGET_TO_SYSTEM;

	if (ucs_do_scsi_cmd(dev, &req_upiu))
	{
		dprintf(CRITICAL, "ucs_do_request_sense: failed\n");
		return -UFS_FAILURE;
	}

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) buf, SCSI_INQUIRY_LEN);

#if DEBUG_UFS
	dump_sense_buffer(buf, SCSI_SENSE_BUF_LEN);
#endif

	return UFS_SUCCESS;
}
