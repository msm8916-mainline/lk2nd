/* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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

void spi_test()
{
	unsigned char *tx_buf1;
	unsigned int data_size = 240*320*2;
	struct qup_spi_dev *spi_dev;
	int i,j,k;

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

	spi_qup_transfer(spi_dev, tx_buf1, data_size);

	free(tx_buf1);
	dprintf(CRITICAL, "-----end %s----\n", __func__);
}
