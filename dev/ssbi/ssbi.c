/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <debug.h>
#include <reg.h>
#include <sys/types.h>
#include <dev/ssbi.h>
#ifdef TARGET_USES_RSPIN_LOCK
#include <platform/remote_spinlock.h>
#endif

int i2c_ssbi_poll_for_device_ready(void)
{
	unsigned long timeout = SSBI_TIMEOUT_US;

	while (!(readl(MSM_SSBI_BASE + SSBI2_STATUS) & SSBI_STATUS_READY)) {
		if (--timeout == 0) {
		        dprintf(INFO, "In Device ready function:Timeout, status %x\n", readl(MSM_SSBI_BASE + SSBI2_STATUS));
			return 1;
		}
	}

	return 0;
}

int i2c_ssbi_poll_for_read_completed(void)
{
	unsigned long timeout = SSBI_TIMEOUT_US;

	while (!(readl(MSM_SSBI_BASE + SSBI2_STATUS) & SSBI_STATUS_RD_READY)) {
		if (--timeout == 0) {
		        dprintf(INFO, "In read completed function:Timeout, status %x\n", readl(MSM_SSBI_BASE + SSBI2_STATUS));
			return 1;
		}
	}

	return 0;
}

int i2c_ssbi_read_bytes(unsigned char  *buffer, unsigned short length,
                                                unsigned short slave_addr)
{
	int ret = 0;
	unsigned char *buf = buffer;
	unsigned short len = length;
	unsigned short addr = slave_addr;
	unsigned long read_cmd = 0;
	unsigned long mode2 = 0;

	/*
	 * Use remote spin locks since SSBI2 controller is shared with nonHLOS proc
	 */
#ifdef TARGET_USES_RSPIN_LOCK
	remote_spin_lock(rlock);
#endif
	read_cmd = SSBI_CMD_READ(addr);
	mode2 = readl(MSM_SSBI_BASE + SSBI2_MODE2);

	//buf = alloc(len * sizeof(8));
	if (mode2 & SSBI_MODE2_SSBI2_MODE)
		writel(SSBI_MODE2_REG_ADDR_15_8(mode2, addr),
				MSM_SSBI_BASE + SSBI2_MODE2);

	while (len) {
		ret = i2c_ssbi_poll_for_device_ready();
		if (ret) {
		        dprintf (CRITICAL, "Error: device not ready\n");
			goto end;
		}

		writel(read_cmd, MSM_SSBI_BASE + SSBI2_CMD);

		ret = i2c_ssbi_poll_for_read_completed();
		if (ret) {
		        dprintf (CRITICAL, "Error: read not completed\n");
			goto end;
		}

		*buf++ = readl(MSM_SSBI_BASE + SSBI2_RD) & SSBI_RD_REG_DATA_MASK;
		len--;
	}
end:
#ifdef TARGET_USES_RSPIN_LOCK
	remote_spin_unlock(rlock);
#endif
	return ret;
}

int i2c_ssbi_write_bytes(unsigned char  *buffer, unsigned short length,
                                                unsigned short slave_addr)
{
	int ret = 0;
	unsigned long timeout = SSBI_TIMEOUT_US;
	unsigned char *buf = buffer;
	unsigned short len = length;
	unsigned short addr = slave_addr;
	unsigned long mode2 = 0;

	/*
	 * Use remote spin locks since SSBI2 controller is shared with nonHLOS proc
	 */
#ifdef TARGET_USES_RSPIN_LOCK
	remote_spin_lock(rlock);
#endif
	mode2 = readl(MSM_SSBI_BASE + SSBI2_MODE2);

	if (mode2 & SSBI_MODE2_SSBI2_MODE)
		writel(SSBI_MODE2_REG_ADDR_15_8(mode2, addr),
				MSM_SSBI_BASE + SSBI2_MODE2);

	while (len) {
		ret = i2c_ssbi_poll_for_device_ready();
		if (ret) {
		        dprintf (CRITICAL, "Error: device not ready\n");
			goto end;
		}

		writel(SSBI_CMD_WRITE(addr, *buf++), MSM_SSBI_BASE + SSBI2_CMD);

		while (readl(MSM_SSBI_BASE + SSBI2_STATUS) & SSBI_STATUS_MCHN_BUSY) {
		  if (--timeout == 0) {
			dprintf(INFO, "In Device ready function:Timeout, status %x\n", readl(MSM_SSBI_BASE + SSBI2_STATUS));
			ret = 1;
			goto end;
		  }
		}
		len--;
	}
end:
#ifdef TARGET_USES_RSPIN_LOCK
	remote_spin_unlock(rlock);
#endif
	return 0;
}

int pa1_ssbi2_read_bytes(unsigned char  *buffer, unsigned short length,
                                                unsigned short slave_addr)
{
    unsigned val = 0x0;
    unsigned temp = 0x0000;
    unsigned char *buf = buffer;
    unsigned short len = length;
    unsigned short addr = slave_addr;
    unsigned long timeout = SSBI_TIMEOUT_US;

    while(len)
    {
        val |= ((addr << PA1_SSBI2_REG_ADDR_SHIFT) |
		(PA1_SSBI2_CMD_READ << PA1_SSBI2_CMD_RDWRN_SHIFT));
        writel(val, PA1_SSBI2_CMD);
        while(!((temp = readl(PA1_SSBI2_RD_STATUS)) & (1 << PA1_SSBI2_TRANS_DONE_SHIFT))) {
            if (--timeout == 0) {
	        dprintf(INFO, "In Device ready function:Timeout\n");
	        return 1;
	    }
	}
        len--;
        *buf++ = (temp & (PA1_SSBI2_REG_DATA_MASK << PA1_SSBI2_REG_DATA_SHIFT));
    }
    return 0;
}

int pa1_ssbi2_write_bytes(unsigned char  *buffer, unsigned short length,
                                                unsigned short slave_addr)
{
    unsigned val;
    unsigned char *buf = buffer;
    unsigned short len = length;
    unsigned short addr = slave_addr;
    unsigned temp = 0x00;
    unsigned char written_data1 = 0x00;
    unsigned long timeout = SSBI_TIMEOUT_US;
    //unsigned char written_data2 = 0x00;

    while(len)
    {
        temp = 0x00;
        written_data1 = 0x00;
        val = (addr << PA1_SSBI2_REG_ADDR_SHIFT) |
	  (PA1_SSBI2_CMD_WRITE << PA1_SSBI2_CMD_RDWRN_SHIFT) |
 (*buf & 0xFF);
        writel(val, PA1_SSBI2_CMD);
        while(!((temp = readl(PA1_SSBI2_RD_STATUS)) & (1 << PA1_SSBI2_TRANS_DONE_SHIFT))) {
            if (--timeout == 0) {
	        dprintf(INFO, "In Device write function:Timeout\n");
	        return 1;
	    }
	}
        len--;
        buf++;
    }
    return 0;
}

int pa2_ssbi2_read_bytes(unsigned char  *buffer, unsigned short length,
        unsigned short slave_addr)
{
    unsigned val = 0x0;
    unsigned temp = 0x0000;
    unsigned char *buf = buffer;
    unsigned short len = length;
    unsigned short addr = slave_addr;
    unsigned long timeout = SSBI_TIMEOUT_US;

    while(len)
    {
        val |= ((addr << PA2_SSBI2_REG_ADDR_SHIFT) |
                (PA2_SSBI2_CMD_READ << PA2_SSBI2_CMD_RDWRN_SHIFT));
        writel(val, PA2_SSBI2_CMD);
        while(!((temp = readl(PA2_SSBI2_RD_STATUS)) & (1 << PA2_SSBI2_TRANS_DONE_SHIFT))) {
            if (--timeout == 0) {
                dprintf(INFO, "In Device ready function:Timeout\n");
                return 1;
            }
        }
        len--;
        *buf++ = (temp & (PA2_SSBI2_REG_DATA_MASK << PA2_SSBI2_REG_DATA_SHIFT));
    }
    return 0;
}

int pa2_ssbi2_write_bytes(unsigned char  *buffer, unsigned short length,
        unsigned short slave_addr)
{
    unsigned val;
    unsigned char *buf = buffer;
    unsigned short len = length;
    unsigned short addr = slave_addr;
    unsigned temp = 0x00;
    unsigned char written_data1 = 0x00;
    unsigned long timeout = SSBI_TIMEOUT_US;

    while(len)
    {
        temp = 0x00;
        written_data1 = 0x00;
        val = (addr << PA2_SSBI2_REG_ADDR_SHIFT) |
            (PA2_SSBI2_CMD_WRITE << PA2_SSBI2_CMD_RDWRN_SHIFT) |
            (*buf & 0xFF);
        writel(val, PA2_SSBI2_CMD);
        while(!((temp = readl(PA2_SSBI2_RD_STATUS)) & (1 << PA2_SSBI2_TRANS_DONE_SHIFT))) {
            if (--timeout == 0) {
                dprintf(INFO, "In Device write function:Timeout\n");
                return 1;
            }
        }
        len--;
        buf++;
    }
    return 0;
}
