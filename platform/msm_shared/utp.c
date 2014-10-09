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

#include <arch/ops.h>
#include <sys/types.h>
#include <kernel/thread.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <ufs_hw.h>
#include <utp.h>
#include <ufs.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <arch/ops.h>
#include <endian.h>
#include <stdlib.h>
#include <sys/types.h>

void utp_process_req_completion(struct ufs_req_irq_type *irq)
{
	struct ufs_req_node *req;
	struct list_node    *prev;
	uint32_t            val;

	/* Make sure we have more nodes than just the head in the list. */
	if (list_next(irq->list, irq->list) == NULL)
	{
		dprintf(CRITICAL, "UTRD/ UTMRD processed signalled and the wait queue is empty\n");
		ASSERT(0);
	}

	/* Read the door bell register. */
	val = readl(irq->door_bell_reg);

	list_for_every_entry(irq->list, req, struct ufs_req_node, list_node)
	{
		if (!(req->door_bell_bit & val))
		{
			/* Transaction is complete: Either transaction completed in a normal way.
			 * Delete and Signal all requests that have completed.
			 */
			prev = req->list_node.prev;
			/* TODO: move delete to the caller function. */
			list_delete(&(req->list_node));

			if (event_signal(req->event, false))
			{
				dprintf(CRITICAL, "Event signal failed.\n");
				ASSERT(0);

			}
			req = containerof(prev, struct ufs_req_node, list_node);
		}
	}

	return;
}

/* Always called within critical section: utrd_bitmap_mutex/ utmrd_bitmap_mutex. */
static uint32_t utp_get_door_bell_bit(uint32_t reg, uint32_t *reg_bitmap, uint32_t *bit_num)
{
	uint32_t val = 0;
	uint32_t doorbell_bit_val;
	uint32_t found = 0;

	*bit_num = 0;

	val = readl(reg) | *reg_bitmap;
	doorbell_bit_val = 1;

	/* Find an empty slot. */
	do
	{
		(*bit_num)++;

		if (!(doorbell_bit_val & val))
		{
			found = 1;
			*reg_bitmap |= doorbell_bit_val;
			break;
		}
		doorbell_bit_val <<= 1;
	}while (doorbell_bit_val <= (uint32_t) (1 << 31));

	if (!found)
	{
		/* TODO: This case shouldd wait on a semaphore instead od returning error.
		 * Return error until LK adds semaphore support.
		 */
		doorbell_bit_val = 0;
		dprintf(CRITICAL, "Unable to find a free slot for transaction.\n");
	}

	return doorbell_bit_val;
}

static void utp_ring_door_bell(uint32_t reg, uint32_t doorbell_bit)
{
	writel(doorbell_bit, reg);
}

static int utp_utrd_process_timeout_req(struct ufs_dev *dev,
								 struct utp_utrd_req_build_type *utrd_req,
								 struct ufs_req_node *req)
{
	switch (utrd_req->req_upiu->trans_type)
	{
		case UPIU_TYPE_NOP_OUT:
							    writel(~req->door_bell_bit, UFS_UTRLCLR(dev->base));
								return -UFS_RETRY;
		default:
								/* TODO : Add ufs hci sw reset.*/
								ASSERT(0);
								return -UFS_FAILURE;
	}
}

static int utp_remove_from_bitmap(struct utp_bitmap_access_type *req)
{

	if (mutex_acquire(req->mutx))
	{
		return -UFS_FAILURE;
	}

	*(req->bitmap) &= ~req->door_bell_bit;

	if (mutex_release(req->mutx))
	{
		return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}

static void utp_enqueue_utrd_fill_desc(struct utp_trans_req_desc *desc, struct utp_utrd_req_build_type *utrd_req)
{
	/* Fill transfer desc. */
	memset(desc, 0, UPIU_HDR_LEN);
	desc->cmd_type_dd_irq = UTP_REQ_BUILD_CMD_DD_IRQ_FIELD(utrd_req->cmd_type, utrd_req->dd, utrd_req->irq);
	desc->overall_cmd_status = utrd_req->ocs;
	/* Bits 0 - 6 are reserved in cmd_desc_base_addr[0] field. */
	desc->cmd_desc_base_addr[0] = ((uint32_t) utrd_req->req_upiu) & 0xC0;
	desc->cmd_desc_base_addr[1] = ((uint32_t) utrd_req->req_upiu >> 8) & 0xFF;
	desc->cmd_desc_base_addr[2] = ((uint32_t) utrd_req->req_upiu >> 16) & 0xFF;
	desc->cmd_desc_base_addr[3] = ((uint32_t) utrd_req->req_upiu >> 24) & 0xFF;
	desc->resp_upiu_offset = ROUNDUP(utrd_req->req_upiu_len, UPIU_HDR_LEN) / 4;
	desc->resp_upiu_len = utrd_req->resp_upiu_len;

	if (utrd_req->dd != UTRD_NO_DATA_TRANSFER)
	{
		/* Data transfer command.
		 * Fill in PRDT data.
		 */
		desc->prdt_offset = utrd_req->prdt_offset / 4;
		desc->prdt_len    = utrd_req->prdt_len;
	}

	/* Flush UTRD to memory. */
	cache_clean_invalidate_unaligned_start_addr((addr_t)desc, sizeof(struct utp_trans_req_desc));
}

static struct utp_trans_req_desc* utp_get_desc_slot_addr(struct ufs_dev *dev, struct utp_utrd_req_build_type *utrd_req, uint32_t *door_bell_val)
{
	struct utp_trans_req_desc *desc = NULL;
	uint32_t                  door_bell_slot;

	if (mutex_acquire(&(dev->utrd_data.bitmap_mutex)))
	{
		goto utp_get_desc_slot_addr_err;
	}

	*door_bell_val = utp_get_door_bell_bit(UFS_UTRLDBR(dev->base), &dev->utrd_data.bitmap, &door_bell_slot);
	if (!(*door_bell_val))
	{
		goto utp_get_desc_slot_addr_err;
	}

	if (mutex_release(&(dev->utrd_data.bitmap_mutex)))
	{
		goto utp_get_desc_slot_addr_err;
	}

	desc = (struct utp_trans_req_desc *) ((addr_t)dev->utrd_data.list_base_addr + (door_bell_slot - 1) * sizeof(struct utp_trans_req_desc));

utp_get_desc_slot_addr_err:
	return desc;
}

int utp_poll_utrd_complete(struct ufs_dev *dev)
{
	int ret;
	struct ufs_req_irq_type irq;
	uint32_t val, base, retry = 0;
	base = dev->base;
	val = readl(UFS_IS(base));
	irq.irq_handled = 0;
	/* Wait till the desc has been processed. */
	while(((val & UFS_IS_UTRCS) == 0) && ((val & UFS_IS_UTMRCS) == 0))
	{
		val = readl(UFS_IS(base));
		retry++;
		udelay(1);
		if(retry == UTP_MAX_COMMAND_RETRY)
		{
			dprintf(CRITICAL, "UTP command never completed.\n");
			return 1;
		}
#ifdef DEBUG_UFS
		dprintf(INFO, "Waiting for UTRCS/URMRCS Completion...\n");
#endif
	}
	if (readl(UFS_IS(base)) & UFS_IS_UTRCS)
	{
		val = readl(UFS_IS(base)) & UFS_IS_UTRCS;
		writel(UFS_IS_UTRCS, UFS_IS(base));
		irq.irq_handled = UFS_IS_UTRCS;
		irq.list = &(dev->utrd_data.list_head.list_node);
		irq.door_bell_reg = UFS_UTRLDBR(base);
		utp_process_req_completion(&irq);
		ret = INT_NO_RESCHEDULE;
	}
	else if (readl(UFS_IS(base)) & (UFS_IS_UTMRCS))
	{
		val = readl(UFS_IS(base)) & UFS_IS_UTMRCS;
		writel(UFS_IS_UTMRCS, UFS_IS(base));
		irq.irq_handled = UFS_IS_UTMRCS;
		irq.list = &(dev->utmrd_data.list_head.list_node);
		utp_process_req_completion(&irq);
		ret = INT_NO_RESCHEDULE;
	}
	return ret;
}

static int utp_enqueue_utrd(struct ufs_dev *dev, struct utp_utrd_req_build_type *utrd_req)
{
	int                           ret;
	struct utp_trans_req_desc     *desc;
	event_t                       utrd_evt;
	struct ufs_req_node           req;
	uint32_t                      door_bell_bit_val;
	struct utp_bitmap_access_type bitmap_req;

	ret = UFS_SUCCESS;

	event_init(&utrd_evt, false, EVENT_FLAG_AUTOUNSIGNAL);

	desc = utp_get_desc_slot_addr(dev, utrd_req, &door_bell_bit_val);
	if (!desc)
	{
		ret = UFS_FAILURE;
		goto utp_enqueue_utrd_err;
	}

	/* Check register UTRLRSR and make sure it is read ‘1’ before continuing. */
	if (!readl(UFS_UTRLRSR(dev->base)))
	{
		ret = -UFS_FAILURE;
		goto utp_enqueue_utrd_err;
	}

	utp_enqueue_utrd_fill_desc(desc, utrd_req);

	req.door_bell_bit = door_bell_bit_val;
	req.event         = &utrd_evt;

	/* Enqueue the req in the device utrd list. */
	list_add_head(&(dev->utrd_data.list_head.list_node), &(req.list_node));

	dsb();

#ifdef DEBUG_UFS
	// print IS before write
	ufs_dump_is_register(dev);
#endif

	utp_ring_door_bell(UFS_UTRLDBR(dev->base), door_bell_bit_val);

	dsb();

#ifdef DEBUG_UFS
	// print IS after write
	ufs_dump_is_register(dev);
#endif
	ret = utp_poll_utrd_complete(dev);

	if (ret)
	{
		if (ret == ERR_TIMED_OUT)
		{
			/* Transaction not completed even after timeout ms. */
			ret = utp_utrd_process_timeout_req(dev, utrd_req, &req);
		}
		else
			ret = -UFS_FAILURE;
		goto utp_enqueue_utrd_err;
	}
	else
	{
		/* Reset ret before returning. */
		ret = UFS_SUCCESS;

		/* Force read UTRD from memory. */
		dsb();
		cache_clean_invalidate_unaligned_start_addr((addr_t) desc, sizeof(struct ufs_req_node));

		/* Check the response. */
		if (desc->overall_cmd_status != UTRD_OCS_SUCCESS)
		{
			dprintf(CRITICAL, "Command failed. command type = %x\n", utrd_req->cmd_type);
			ret = -UFS_FAILURE;
			goto utp_enqueue_utrd_err;
		}
	}

	/* Signal slot as free. */
	bitmap_req.bitmap        = &dev->utrd_data.bitmap;
	bitmap_req.door_bell_bit = req.door_bell_bit;
	bitmap_req.mutx          = &(dev->utrd_data.bitmap_mutex);

	ret = utp_remove_from_bitmap(&bitmap_req);
	if (ret)
		goto utp_enqueue_utrd_err;

utp_enqueue_utrd_err:
	return ret;
}

static int utp_get_prdt_len(uint32_t data_len, uint32_t *num_prdt)
{
	/* Calculate the prdt entries required. */
	*num_prdt = ROUNDUP(data_len, UTP_MAX_PRD_DATA_BYTE_CNT);
	*num_prdt >>= UTP_MAX_PRD_DATA_BYTE_CNT_BYTE_SHIFT;

	if (*num_prdt > UTP_MAX_PRD_TABLE_ENTRIES)
	{
		dprintf(CRITICAL, "Data length exceeds for a single upiu transfer.\n");
		return -UFS_FAILURE;
	}

	return UFS_SUCCESS;
}

static int utp_fill_req_upiu(struct ufs_dev *dev, struct upiu_req_build_type *upiu_data, struct upiu_gen_hdr *req_upiu)
{
	memset(req_upiu, 0, UPIU_HDR_LEN);

	if (upiu_data->trans_type == UPIU_TYPE_QUERY_REQ)
	{
		/* Fill in query specific fields. */
		if (utp_build_query_req_upiu((struct upiu_trans_mgmt_query_hdr *) req_upiu, upiu_data))
		{
			return -UFS_FAILURE;
		}
	}

	/* If a data transfer cmd, check the alignment and length of the buffer. */
	if (upiu_data->expected_data_len)
	{
		if (upiu_data->data_buffer_addr & 0x3)
		{
			return -UFS_FAILURE;
		}
	}

	req_upiu->basic_hdr.trans_type    = upiu_data->trans_type;
	req_upiu->basic_hdr.flags         = upiu_data->flags;
	req_upiu->basic_hdr.cmd_set_type  = upiu_data->cmd_set_type;
	req_upiu->basic_hdr.data_seg_len  = upiu_data->data_seg_len;
	req_upiu->basic_hdr.lun           = upiu_data->lun;
	req_upiu->basic_hdr.total_ehs_len = upiu_data->ehs_len;
	req_upiu->basic_hdr.task_tag      = atomic_add((int *) &(dev->utrd_data.task_id), 1);
	if (upiu_data->cdb)
		memcpy(&(req_upiu->trans_specific_fields[4]), (void *) upiu_data->cdb, 16);
	/* If command upiu, fill in data length. */
	if (req_upiu->basic_hdr.trans_type == UPIU_TYPE_COMMAND)
		((struct upiu_cmd_hdr *)req_upiu)->data_expected_len = BE32(upiu_data->expected_data_len);

	return UFS_SUCCESS;

}

static void utp_fill_utrd_properties(struct upiu_req_build_type *upiu_data,
									 struct utp_utrd_req_build_type *utrd,
									 struct utrd_cmd_desc *cmd_desc)
{
	utrd->cmd_type      = upiu_data->cmd_type;
	utrd->dd            = upiu_data->dd;
	utrd->irq           = UTRD_IRQ_CMD;
	utrd->prdt_offset   = UPIU_HDR_LEN + cmd_desc->resp_upiu_len;
	utrd->prdt_len      = cmd_desc->num_prdt;
	utrd->req_upiu      = (struct upiu_basic_hdr *) cmd_desc->req_upiu;
	utrd->req_upiu_len  = UPIU_HDR_LEN;
	utrd->resp_upiu_len = cmd_desc->resp_upiu_len;
	utrd->ocs           = 0xF;
	utrd->timeout       = upiu_data->timeout_msecs;
}

static void utp_fill_prdt_entries(struct upiu_req_build_type *upiu_data, struct utp_prdt_entry *prdt_entry)
{
	uint64_t buf;
	uint64_t bytes_remaining;
	uint32_t prd_dbc;

	buf                = upiu_data->data_buffer_addr;
	bytes_remaining    = upiu_data->expected_data_len;

	while (bytes_remaining)
	{
		prdt_entry->data_base_addr   = buf;
		prdt_entry->data_upper_addr  = buf >> 32;
		prd_dbc                      = MIN(UTP_MAX_PRD_DATA_BYTE_CNT, bytes_remaining) - 1;
		prdt_entry->data_byte_cnt    = prd_dbc;
		if (bytes_remaining <= UTP_MAX_PRD_DATA_BYTE_CNT)
			break;
		buf             += UTP_MAX_PRD_DATA_BYTE_CNT;
		bytes_remaining -= UTP_MAX_PRD_DATA_BYTE_CNT;
		prdt_entry++;
	}

}

int utp_enqueue_upiu(struct ufs_dev *dev, struct upiu_req_build_type *upiu_data)
{
	struct upiu_gen_hdr            *req_upiu;
	struct utp_utrd_req_build_type utrd;
	uint32_t                       num_prdt;
	struct utp_prdt_entry          *prdt_entry;
	int                            ret = UFS_SUCCESS;
	uint32_t                       resp_len;
	uint32_t                       cmd_desc_len;
	struct utrd_cmd_desc           cmd_desc;

	/* Round up resp_upiu_len to a DWORD boundary.
	 * Also, make sure it is of min required length.
	 */
	resp_len = ROUNDUP(upiu_data->resp_data_len, 4) + UPIU_HDR_LEN;

	if (utp_get_prdt_len(upiu_data->expected_data_len, &num_prdt))
		return -UFS_FAILURE;

	/* Calculate the length. */
	cmd_desc_len = UPIU_HDR_LEN + resp_len + num_prdt * sizeof(struct utp_prdt_entry);

	/* Allocate memory for UTP Command Descriptor. */
	req_upiu = (struct upiu_gen_hdr*) memalign((size_t ) lcm(CACHE_LINE, UTP_CMD_DESC_BASE_ALIGNMENT_SIZE), ROUNDUP(cmd_desc_len, CACHE_LINE));
	if (!req_upiu)
	{
		dprintf(CRITICAL, "Unable to allocate request upiu\n");
		return -UFS_FAILURE;
	}

	/* Fill req upiu. */
	ret = utp_fill_req_upiu(dev, upiu_data, req_upiu);
	if (ret)
	{
		goto utp_enqueue_upiu_err;
	}

	/* Fill UTRD properties. */
	cmd_desc.num_prdt      = num_prdt;
	cmd_desc.req_upiu      = req_upiu;
	cmd_desc.resp_upiu_len = resp_len;
	utp_fill_utrd_properties(upiu_data, &utrd, &cmd_desc);

	prdt_entry         = (struct utp_prdt_entry *) ((uint32_t) req_upiu + UPIU_HDR_LEN + resp_len);

	/* Fill PRDT entries. */
	if (num_prdt)
	utp_fill_prdt_entries(upiu_data, prdt_entry);

	/* Flush req_upiu */
	dsb();
	arch_clean_invalidate_cache_range((addr_t) req_upiu, cmd_desc_len);

	/* Check the response. */
	ret = utp_enqueue_utrd(dev, &utrd);
	if (ret)
	{
		dprintf(CRITICAL, "Command failed. command = %x\n", req_upiu->basic_hdr.trans_type);
		goto utp_enqueue_upiu_err;
	}

	/* UPIU processed. Invalidate cache to update resp. */
	arch_invalidate_cache_range((addr_t) req_upiu, cmd_desc_len);

	/* Save the response. */
	memcpy(upiu_data->resp_ptr, (void *) ((uint32_t)req_upiu + UPIU_HDR_LEN), upiu_data->resp_len);
	memcpy((void *) upiu_data->resp_data_ptr, (void *) ((uint32_t)req_upiu + 2 * UPIU_HDR_LEN), upiu_data->resp_data_len);

utp_enqueue_upiu_err:
	free(req_upiu);
	return ret;
}
