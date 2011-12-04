/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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

#include <stdlib.h>
#include <reg.h>

#include "adm.h"
#include <platform/adm.h>

/* TODO: This ADM implementation is very specific for use by MMC.
 *       Replace with a generic ADM driver that can also be used
 *       by other peripherals such as usb/uart/nand/display etc.
 */

/* TODO:
 * adm module shouldn't have to include mmc.h.
 * clean this up when generic adm interface is implemented.
 */
#include "mmc.h"

extern void mdelay(unsigned msecs);
extern void dmb(void);

/* limit the max_row_len to fifo size so that
 * the same src and dst row attributes can be used.
 */
#define MAX_ROW_LEN     MMC_BOOT_MCI_FIFO_SIZE
#define MAX_ROW_NUM     0xFFFF

/* Structures for use with ADM:
 * Must be aligned on 8 byte boundary.
 */
static uint32_t adm_cmd_ptr_list[8] __attribute__ ((aligned(8)));
static uint32_t box_mode_entry[8] __attribute__ ((aligned(8)));

adm_result_t adm_transfer_start(uint32_t adm_chn, uint32_t * cmd_ptr_list);

/* CRCI - mmc slot mapping. */
extern uint8_t sdc_crci_map[5];

/* TODO:
 * This interface is very specific to MMC.
 * We need a generic ADM interface that can be easily
 * used by other modules such as usb/uart/nand.
 */
adm_result_t
adm_transfer_mmc_data(unsigned char slot,
		      unsigned char *data_ptr,
		      unsigned int data_len, unsigned char direction)
{
	uint32_t num_rows;
	uint16_t row_len;
	uint16_t row_offset;
	uint32_t row_num;
	uint32_t adm_crci_num;
	adm_result_t result = ADM_RESULT_SUCCESS;

	/* Make sure slot value is in the range 1..4 */
	ASSERT((slot >= 1) && (slot <= 4));

	adm_crci_num = sdc_crci_map[slot];
	row_len = MMC_BOOT_MCI_FIFO_SIZE;
	num_rows = data_len / MMC_BOOT_MCI_FIFO_SIZE;

	/* While there is data to be transferred */
	while (data_len) {
		if (data_len <= MAX_ROW_LEN) {
			row_len = data_len;
			row_offset = 0;
			row_num = 1;
		} else {
			row_len = MAX_ROW_LEN;
			row_offset = MAX_ROW_LEN;
			row_num = data_len / MAX_ROW_LEN;

			/* Limit the number of row to the max value allowed */
			if (row_num > MAX_ROW_NUM) {
				row_num = MAX_ROW_NUM;
			}
		}

		/* Program ADM registers and initiate data transfer */

		/*  Initialize the Box Mode command entry (single entry) */
		box_mode_entry[0] = (ADM_CMD_LIST_LC |
				     (adm_crci_num << 3) | ADM_ADDR_MODE_BOX);

		if (direction == ADM_MMC_READ) {
			box_mode_entry[1] = MMC_BOOT_MCI_FIFO;	/* SRC addr    */
			box_mode_entry[2] = (uint32_t) data_ptr;	/* DST addr    */
			box_mode_entry[3] = ((row_len << 16) |	/* SRC row len */
					     (row_len << 0));	/* DST row len */
			box_mode_entry[4] = ((row_num << 16) |	/* SRC row #   */
					     (row_num << 0));	/* DST row #   */
			box_mode_entry[5] = ((0 << 16) |	/* SRC offset  */
					     (row_offset << 0));	/* DST offset  */
		} else {
			box_mode_entry[1] = (uint32_t) data_ptr;	/* SRC addr    */
			box_mode_entry[2] = MMC_BOOT_MCI_FIFO;	/* DST addr    */
			box_mode_entry[3] = ((row_len << 16) |	/* SRC row len */
					     (row_len << 0));	/* DST row len */
			box_mode_entry[4] = ((row_num << 16) |	/* SRC row #   */
					     (row_num << 0));	/* DST row #   */
			box_mode_entry[5] = ((row_offset << 16) |	/* SRC offset  */
					     (0 << 0));	/* DST offset  */
		}

		/*  Initialize the ADM Command Pointer List (single entry) */
		adm_cmd_ptr_list[0] = (ADM_CMD_PTR_LP |
				       ADM_CMD_PTR_CMD_LIST |
				       (((uint32_t) (&box_mode_entry[0])) >>
					3));

		/* Start ADM transfer, this is a blocking call. */
		result = adm_transfer_start(ADM_CHN, adm_cmd_ptr_list);

		if (result != ADM_RESULT_SUCCESS) {
			break;
		}

		/* Update the data ptr and data len by the amount
		 * we just transferred.
		 */
		data_ptr += (row_len * row_num);
		data_len -= (row_len * row_num);
	}

	return result;
}

/*
 * Start the ADM data transfer and return the result of the transfer.
 * Blocks until transfer is completed.
 */
adm_result_t adm_transfer_start(uint32_t adm_chn, uint32_t * cmd_ptr_list)
{
	uint32_t reg_value;
	uint32_t timeout = 1;
	uint32_t delay_count = 100;

	/* Memory barrier to ensure that all ADM command list structure
	 * writes have completed before starting the ADM transfer.
	 */
	dmb();

	/* Start the ADM transfer by writing the command ptr */
	writel(((uint32_t) cmd_ptr_list) >> 3,
	       ADM_REG_CMD_PTR(adm_chn, ADM_SD));

	/* Poll the status register to check for transfer complete.
	 * Bail out if transfer is not finished within 1 sec.
	 * Note: This time depends on the amount of data being transferred.
	 * Increase the delay_count if this is not sufficient.
	 */
	do {
		reg_value = readl(ADM_REG_STATUS(adm_chn, ADM_SD));
		if ((reg_value & ADM_REG_STATUS__RSLT_VLD___M) != 0) {
			timeout = 0;
			break;
		}

		/* 10ms wait */
		mdelay(10);

	}
	while (delay_count--);

	/* Read out the IRQ register to clear the interrupt.
	 * Even though we are not using interrupts,
	 * kernel is not clearing the interupts during its
	 * ADM initialization, causing it to crash.
	 */
	reg_value = readl(ADM_REG_IRQ(ADM_SD));

	if (timeout) {
		return ADM_RESULT_TIMEOUT;
	} else {
		/* Get the result from the RSLT FIFO */
		reg_value = readl(ADM_REG_RSLT(adm_chn, ADM_SD));

		/* Check for any error */
		if (((reg_value & ADM_REG_RSLT__ERR___M) != 0) ||
		    ((reg_value & ADM_REG_RSLT__TPD___M) == 0) ||
		    ((reg_value & ADM_REG_RSLT__V___M) == 0)) {
			return ADM_RESULT_FAILURE;
		}
	}

	return ADM_RESULT_SUCCESS;
}
