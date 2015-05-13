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
 *
 */

#include <mipi_dsi_i2c.h>
#include <blsp_qup.h>
#include <i2c_qup.h>
#include <gsbi.h>
#include <err.h>
#include <debug.h>

#define I2C_CLK_FREQ     100000
#define I2C_SRC_CLK_FREQ 50000000

static struct qup_i2c_dev *i2c_dev;

int mipi_dsi_i2c_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
	if (!buf)
		return ERR_INVALID_ARGS;

	if(!i2c_dev)
		return ERR_NOT_VALID;

	struct i2c_msg rd_buf[] = {
		{addr, I2C_M_WR, 1, &reg},
		{addr, I2C_M_RD, len, buf}
	};

	int err = qup_i2c_xfer(i2c_dev, rd_buf, 2);
	if (err < 0) {
		dprintf(CRITICAL, "Read reg %x failed\n", reg);
		return err;
	}

	return NO_ERROR;
}

int mipi_dsi_i2c_read_byte(uint8_t addr, uint8_t reg, uint8_t *buf)
{
	if (!buf)
		return ERR_INVALID_ARGS;

	return mipi_dsi_i2c_read(addr, reg, buf, 1);
}

int mipi_dsi_i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t val)
{
	if (!i2c_dev)
		return ERR_NOT_VALID;

	unsigned char buf[2] = {reg, val};
	struct i2c_msg msg_buf[] = {
		{addr, I2C_M_WR, 2, buf},
	};

	int err = qup_i2c_xfer(i2c_dev, msg_buf, 1);
	if (err < 0) {
		dprintf(CRITICAL, "Write reg %x failed\n", reg);
		return err;
	}
	return NO_ERROR;
}

int mipi_dsi_i2c_device_init()
{
	i2c_dev = qup_blsp_i2c_init(BLSP_ID_1, QUP_ID_3,
				I2C_CLK_FREQ, I2C_SRC_CLK_FREQ);
	if(!i2c_dev) {
		dprintf(CRITICAL, "mipi_dsi_i2c_device_init() failed\n");
		return ERR_NOT_VALID;
	}
	return NO_ERROR;
}
