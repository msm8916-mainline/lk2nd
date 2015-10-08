/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation, nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
#include <pm_fg_sram.h>
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <debug.h>
#include <bits.h>
#include <platform/timer.h>
#include <pm_comm.h>

static int fg_check_addr_mask(int sid,uint32_t addr,uint8_t mask,uint8_t set)
{
	uint8_t value;
	int ret = 1;
	int try = FG_MAX_TRY;

	udelay(30);
	while(try)
	{
		ret = pm_comm_read_byte_mask(sid, addr, mask, &value, 0);
		if( !ret && (value == set) )
			goto err;

		udelay(1000);
		try--;
	}
err:
	return ret;
}

int pmi_fg_sram_read(uint32_t addr, uint32_t *data,int sid, uint8_t offset, uint8_t len)
{
	uint8_t value;
	int err = 0;
	uint8_t start_beat_count, end_beat_count;

	if(offset > 3)
	{
		dprintf(INFO,"offset beyond the 4 byte boundary\n");
		goto err;
	}
	/* poll to check if fg memif is available */
	err = fg_check_addr_mask(sid,FG_MEMIF_MEM_INTF_CFG, RIF_MEM_ACCESS_REQ, 0);
	if(err)
	{
		dprintf(INFO,"Failed to get fg sram access\n");
		goto err;
	}

	/* enter into ima mode write 8'hA0 */
	pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_CFG, 0xA0, 0);
	/* ensure single read access 8'h00 */
	pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_CTL, 0x00, 0);

	/* poll ima is ready */
	err = fg_check_addr_mask(sid, FG_MEMIF_IMA_OPERATION_STS, IACS_RDY, 1);
	if(err)
	{
		dprintf(INFO,"IACS is not ready cannot enter ima mode\n");
		goto err;
	}

	/* write lsb address of the requested data */
	value = addr & 0xff;
	pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_ADDR_LSB, value, 0);

	/* write msb address of the requested data */
	value = (addr >> 8) & 0xff;
	pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_ADDR_MSB, value, 0);

	/* poll until transaction is completed */
	/* poll ima is ready */
	err = fg_check_addr_mask(sid, FG_MEMIF_IMA_OPERATION_STS, IACS_RDY, 1);
	if(err)
	{
		dprintf(INFO,"IACS is not ready cannot set address\n");
		goto err;
	}

	/* read start beat count */
	pm_comm_read_byte_mask(sid, FG_MEMIF_FG_BEAT_COUNT, BEAT_COUNT_MASK, &start_beat_count, 0);
	/* read the data */
	pm_comm_read_byte(sid, FG_MEMIF_MEM_INTF_RD_DATA0, &value, 0);
	*data = value;
	pm_comm_read_byte(sid, FG_MEMIF_MEM_INTF_RD_DATA1, &value, 0);
	*data |= value << 8;
	pm_comm_read_byte(sid, FG_MEMIF_MEM_INTF_RD_DATA2, &value, 0);
	*data |= value << 16;
	pm_comm_read_byte(sid, FG_MEMIF_MEM_INTF_RD_DATA3, &value, 0);
	*data = value << 24;

	/* poll to check there was no error */
	err = fg_check_addr_mask(sid, FG_MEMIF_IMA_OPERATION_STS, IACS_RDY, 1);
	if(err)
	{
		dprintf(INFO,"IACS is not ready cannot read\n");
		goto err;
	}

	pm_comm_read_byte(sid, FG_MEMIF_IMA_EXCEPTION_STS, &value, 0);
	err = value;
	if(!err)
	{
		*data = ((*data) >> (offset*8)) & (FG_DATA_MASK >> ((FG_DATA_MAX_LEN - len)*8) );
		pm_comm_read_byte_mask(sid, FG_MEMIF_FG_BEAT_COUNT, BEAT_COUNT_MASK, &end_beat_count, 0);
		if(start_beat_count != end_beat_count)
			err = 1;
	}
	else
	{
		/* error occured */
		/*perform iacs clear sequence 1'b1 */
		pm_comm_read_byte(sid, FG_MEMIF_IMA_CFG, &value, 0);
		value |= IACS_CLR;
		pm_comm_write_byte(sid, FG_MEMIF_IMA_CFG, value, 0);
		/* 8'h04 */
		pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_ADDR_MSB, 0x04, 0);
		/* 8'h00 */
		pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_WR_DATA3, 0x00, 0);
		pm_comm_read_byte(sid, FG_MEMIF_MEM_INTF_RD_DATA3, &value, 0);
		/*perform iacs clear sequence 1'b0 */
		pm_comm_read_byte(sid, FG_MEMIF_IMA_CFG, &value, 0);
		value &= ~(IACS_CLR);
		pm_comm_write_byte(sid, FG_MEMIF_IMA_CFG, value, 0);
	}

err:
	pm_comm_write_byte(sid, FG_MEMIF_MEM_INTF_CFG, 0x00, 0);

	return err;

}
