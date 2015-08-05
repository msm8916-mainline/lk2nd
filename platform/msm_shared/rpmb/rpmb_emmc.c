/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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

#include <rpmb.h>
#include <mmc_sdhci.h>
#include <debug.h>

static const char *str_err[] =
{
	"Operation Ok",
	"General failure",
	"Authentication error (MAC comparison not matching, MAC calculation failure)",
	"Counter failure (counters not matching in comparison, counter incrementing failure)",
	"Address failure (address out of range, wrong address alignment)",
	"Write failure (data/counter/result write failure)",
	"Read failure (data/counter/result read failure)",
	"Authentication Key not yet programmed",
};

static struct rpmb_frame read_result_reg =
{
	.requestresponse[1] = READ_RESULT_FLAG,
};

int rpmb_write_emmc(struct mmc_device *dev,uint32_t *req_buf, uint32_t blk_cnt, uint32_t rel_wr_count, uint32_t *resp_buf, uint32_t *resp_len)
{
	uint32_t i, num_trans = 0;
	int ret = 0;
	struct mmc_command cmd[3] = {{0},{0},{0}};
	struct rpmb_frame *result = (struct rpmb_frame *)resp_buf;

	dprintf(INFO, "rpmb write command called with blk_cnt and rel_wr_count: 0x%x 0x%x\n", blk_cnt, rel_wr_count);
	if (rel_wr_count == 0x2)
	{
		// if reliable write count reported by secure app is 2 then we can
		// send two half sectors in one transaction. So overall number of
		// transactions would be total block count by 2.
		num_trans = blk_cnt / 2;
	}
	else if(rel_wr_count == 0x1)
	{
		num_trans = blk_cnt; // rel_rw_count = 1 for emmc 5.0 and below
	}
	else if(rel_wr_count == 0x20)
	{
		num_trans = blk_cnt / 32;
	}

	for (i = 0; i < num_trans; i++)
	{
#if DEBUG_RPMB
		for(uint8_t j = 0; j < rel_wr_count; j++)
			dump_rpmb_frame((uint8_t *)req_buf + (j * 512), "request");
#endif

		/* CMD25 program data packet */
		cmd[0].write_flag = true;
		cmd[0].cmd_index = CMD25_WRITE_MULTIPLE_BLOCK;
		cmd[0].argument = 0;
		cmd[0].cmd_type = SDHCI_CMD_TYPE_NORMAL;
		cmd[0].resp_type = SDHCI_CMD_RESP_R1;
		cmd[0].trans_mode = SDHCI_MMC_WRITE;
		cmd[0].data_present = 0x1;
		cmd[0].data.data_ptr = (void *)req_buf;
		if (rel_wr_count == 0x1)
		{
			cmd[0].rel_write = 0;
			cmd[0].data.num_blocks = RPMB_MIN_BLK_CNT;
		}
		else if(rel_wr_count == 0x2 || rel_wr_count == 0x20)
		{
			cmd[0].rel_write = 1;
			cmd[0].data.num_blocks = rel_wr_count;
		}
		/* CMD25 Result Register Read Request Packet */
		cmd[1].write_flag = false;
		cmd[1].cmd_index = CMD25_WRITE_MULTIPLE_BLOCK;
		cmd[1].argument = 0;
		cmd[1].cmd_type = SDHCI_CMD_TYPE_NORMAL;
		cmd[1].resp_type = SDHCI_CMD_RESP_R1;
		cmd[1].trans_mode = SDHCI_MMC_WRITE;
		cmd[1].data_present = 0x1;
		cmd[1].data.data_ptr = (void *)&read_result_reg;
		cmd[1].data.num_blocks = RPMB_MIN_BLK_CNT;

		/* Read actual result with read request */
		cmd[2].write_flag = false;
		cmd[2].cmd_index = CMD18_READ_MULTIPLE_BLOCK;
		cmd[2].argument = 0;
		cmd[2].cmd_type = SDHCI_CMD_TYPE_NORMAL;
		cmd[2].resp_type = SDHCI_CMD_RESP_R1;
		cmd[2].trans_mode = SDHCI_MMC_READ;
		cmd[2].data_present = 0x1;
		cmd[2].data.data_ptr = (void *)resp_buf;
		cmd[2].data.num_blocks = RPMB_MIN_BLK_CNT;

		ret = mmc_sdhci_rpmb_send(dev, cmd);

		if (ret)
		{
			dprintf(CRITICAL, "Failed to Send the RPMB write sequnce of commands\n");
			break;
		}

#if DEBUG_RPMB
		dump_rpmb_frame((uint8_t *)resp_buf, "response");
#endif
		if (result->result[0] == 0x80)
		{
			dprintf(CRITICAL, "Max write counter reached: \n");
			break;
		}

		if (result->result[1])
		{
			dprintf(CRITICAL, "%s\n", str_err[result->result[1]]);
			break;
		}
		req_buf = (uint32_t*) ((uint8_t*)req_buf + (RPMB_BLK_SIZE * rel_wr_count));
	}
	*resp_len = RPMB_MIN_BLK_CNT * RPMB_BLK_SIZE;

	return 0;
}

int rpmb_read_emmc(struct mmc_device *dev, uint32_t *req_buf, uint32_t blk_cnt, uint32_t *resp_buf, uint32_t *resp_len)
{
	struct mmc_command cmd[3] = {{0}, {0}, {0}};
	int ret = 0;
	struct rpmb_frame *result = (struct rpmb_frame *)resp_buf;

#if DEBUG_RPMB
	dump_rpmb_frame((uint8_t *)req_buf, "request");
#endif

	/* CMD25 program data packet */
	cmd[0].write_flag = false;
	cmd[0].cmd_index = CMD25_WRITE_MULTIPLE_BLOCK;
	cmd[0].argument = 0;
	cmd[0].cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd[0].resp_type = SDHCI_CMD_RESP_R1;
	cmd[0].trans_mode = SDHCI_MMC_WRITE;
	cmd[0].data_present = 0x1;
	cmd[0].data.data_ptr = (void *)req_buf;
	cmd[0].data.num_blocks = RPMB_MIN_BLK_CNT;

	/* Read actual result with read request */
	cmd[1].write_flag = false;
	cmd[1].cmd_index = CMD18_READ_MULTIPLE_BLOCK;
	cmd[1].argument = 0;
	cmd[1].cmd_type = SDHCI_CMD_TYPE_NORMAL;
	cmd[1].resp_type = SDHCI_CMD_RESP_R1;
	cmd[1].trans_mode = SDHCI_MMC_READ;
	cmd[1].data_present = 0x1;
	cmd[1].data.data_ptr = (void *)resp_buf;
	cmd[1].data.num_blocks = blk_cnt;
	cmd[1].cmd23_support  = 0x1;

	ret = mmc_sdhci_rpmb_send(dev, cmd);

	if (ret)
	{
		dprintf(CRITICAL, "Failed to Send the RPMB read sequence of commands\n");
		return -1;
	}

	if (result->result[1])
	{
		dprintf(CRITICAL, "%s\n", str_err[result->result[1]]);
	}

#if DEBUG_RPMB
	dump_rpmb_frame((uint8_t *)resp_buf, "response");
#endif
	*resp_len = blk_cnt * RPMB_BLK_SIZE;

	return 0;
}
