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

#include <debug.h>
#include <stdlib.h>
#include <endian.h>
#include <string.h>
#include <utp.h>

static int ucs_do_request_sense(struct ufs_dev *dev);

int ucs_do_scsi_cmd(struct ufs_dev *dev, struct scsi_req_build_type *req)
{
	struct upiu_req_build_type req_upiu;
	struct upiu_basic_hdr      resp_upiu;
	int                        ret;

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
			ret = ucs_do_request_sense(dev);
			if (ret)
				dprintf(CRITICAL, "SCSI request sense failed.\n");
		}

		dprintf(CRITICAL, "ucs_do_scsi_cmd failed status = %x\n", resp_upiu.status);
		return -UFS_FAILURE;
	}

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
	param_list->data_len          = (sizeof(struct unmap_param_list) - 1) << 0x8; /* n-1 */

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
	arch_invalidate_cache_range((addr_t) cdb_param, SCSI_CDB_PARAM_LEN);

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

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) param, SCSI_INQUIRY_LEN);

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

static int ucs_do_request_sense(struct ufs_dev *dev)
{
	STACKBUF_DMA_ALIGN(cdb, sizeof(struct scsi_sense_cdb));
	struct scsi_req_build_type req_upiu;
	struct scsi_sense_cdb      *cdb_param;
	uint8_t                    buf[SCSI_SENSE_BUF_LEN];

	cdb_param = cdb;

	memset(cdb, 0, sizeof(struct scsi_sense_cdb));

	cdb_param->opcode    = SCSI_CMD_SENSE_REQ;
	cdb_param->alloc_len = SCSI_SENSE_BUF_LEN;

	/* Flush cdb to memory. */
	dsb();
	arch_invalidate_cache_range((addr_t) cdb_param, SCSI_CDB_PARAM_LEN);

	memset(&req_upiu, 0 , sizeof(struct scsi_req_build_type));

	req_upiu.cdb			   = (addr_t) cdb_param;
	req_upiu.data_buffer_addr  = (addr_t) buf;
	req_upiu.data_len          = SCSI_SENSE_BUF_LEN;
	req_upiu.flags			   = UPIU_FLAGS_READ;
	req_upiu.lun			   = 0;
	req_upiu.dd			       = UTRD_TARGET_TO_SYSTEM;

	if (ucs_do_scsi_cmd(dev, &req_upiu))
	{
		dprintf(CRITICAL, "ucs_do_request_sense: failed\n");
		return -UFS_FAILURE;
	}

	/* Flush buffer. */
	arch_invalidate_cache_range((addr_t) buf, SCSI_INQUIRY_LEN);

	dump_sense_buffer(buf, SCSI_SENSE_BUF_LEN);

	return UFS_SUCCESS;
}
