/* Copyright (c) 2017-2018, 2020 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#include <spi_qup.h>
#include <blsp_qup.h>
#include <stdlib.h>
#include <platform/gpio.h>

#define spi_panel_dc_gpio 110

static int spidev_write_cmd(struct qup_spi_dev *dev, char cmd)
{
	int ret = 0;
	unsigned char buf[4] = {0};

	if (!dev) {
		dprintf(CRITICAL, "SPI has not been initialized\n");
		return -ENODEV;
	}

	dev->bytes_per_word = 1;
	dev->bit_shift_en = 1;
	buf[0] = cmd;

	gpio_set(spi_panel_dc_gpio, 0);
	ret = spi_qup_write(dev, buf, 1);
	gpio_set(spi_panel_dc_gpio, 2);
	if (ret)
		dprintf(CRITICAL, "Send SPI command to panel failed\n");

	return ret;
}

static int spidev_read_cmd(struct qup_spi_dev *dev, unsigned char *buf,
	unsigned int bytes_per_word, unsigned int len)
{
	unsigned int i;
	int ret = 0;

	if (!dev) {
		dprintf(CRITICAL, "SPI has not been initialized\n");
		return -ENODEV;
	}

	dev->bytes_per_word = bytes_per_word;
	dev->bit_shift_en = 1;

	gpio_set(spi_panel_dc_gpio, 0);
	ret = spi_qup_read(dev, buf, len);
	gpio_set(spi_panel_dc_gpio, 2);

	for(i = 0; i < bytes_per_word * len; i++)
		dprintf(SPEW, "-----Read: 0x%x-----\n", buf[i]);

	return ret;
}

void spi_test()
{
	unsigned char *tx_buf1;
	unsigned int data_size = 240*320*2;
	struct qup_spi_dev *spi_dev;
	int i,j,k;
	unsigned char rx_buf[8] = {0};

	dprintf(CRITICAL, "-----start %s----\n", __func__);

	tx_buf1 = malloc(data_size);

	k = 0;
	for (i = 0; i < 320; i++) {
		for (j = 0; j < 240; j++) {
			tx_buf1[k] = 0xf8;
			tx_buf1[k+1] = 0x00;
			k = k+2;
		}
	}

	spi_dev = qup_blsp_spi_init(BLSP_ID_1, QUP_ID_4);

	if (!spi_dev) {
		dprintf(CRITICAL, "Failed initializing SPI\n");
		return;
	}

	spi_qup_write(spi_dev, tx_buf1, data_size);

	spidev_write_cmd(spi_dev, 0x09);
	spidev_read_cmd(spi_dev, rx_buf, 1, 4);
	free(tx_buf1);
	dprintf(SPEW, "-----end %s----\n", __func__);

}
