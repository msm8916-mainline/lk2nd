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

#ifndef  __SPI_QUP__
#define  __SPI_QUP__

#include <stdint.h>
#include <qup.h>

/* SPI_ERROR_FLAGS and SPI_ERROR_FLAGS_EN fields */
#define SPI_ERROR_CLK_OVER_RUN		BIT(1)
#define SPI_ERROR_CLK_UNDER_RUN		BIT(0)

#define SPI_CONFIG                    0x0300
#define SPI_IO_CONTROL                0x0304
#define SPI_IO_MODES                  0x0008
#define SPI_SW_RESET                  0x000C
#define SPI_TIME_OUT_CURRENT          0x0014
#define SPI_MX_OUTPUT_COUNT           0x0100
#define SPI_MX_OUTPUT_CNT_CURRENT     0x0104
#define SPI_MX_INPUT_COUNT            0x0200
#define SPI_MX_INPUT_CNT_CURRENT      0x0204
#define SPI_MX_READ_COUNT             0x0208
#define SPI_MX_READ_CNT_CURRENT       0x020C
#define SPI_OPERATIONAL               0x0018
#define SPI_ERROR_FLAGS               0x001C
#define SPI_ERROR_FLAGS_EN            0x0020
#define SPI_DEASSERT_WAIT             0x0310
#define SPI_OUTPUT_DEBUG              0x0108
#define SPI_INPUT_DEBUG               0x0210
#define SPI_TEST_CTRL                 0x0024
#define SPI_OUTPUT_FIFO               0x0110
#define SPI_INPUT_FIFO                0x0218
#define SPI_STATE                     0x0004

#define SPI_IO_C_NO_TRI_STATE		BIT(0)
#define SPI_IO_C_CLK_ALWAYS_ON		BIT(9)
#define SPI_IO_C_MX_CS_MODE			BIT(8)
#define SPI_IO_C_CS0_ACTIVE_HIGH	BIT(4)
#define SPI_IO_C_CS_SELECT_CS0		00 << 2
#define SPI_IO_C_CLK_IDLE_HIGH		BIT(10)

/* SPI_CONFIG fields */
#define SPI_CONFIG_HS_MODE			BIT(10)
#define SPI_CONFIG_INPUT_FIRST		BIT(9)
#define SPI_CONFIG_LOOPBACK			BIT(8)

#define DEFAULT_BYTES_PER_WORD	0x2

#define BITS_PER_BYTE 	8

#define EIO         5
#define ENOMEM      12
#define EBUSY       16
#define ENODEV      19
#define ENOSYS      38
#define EPROTONOSUPPORT 93
#define ETIMEDOUT   110

struct spi_transfer {
	const unsigned char	*tx_buf;
	int	len;
};

/**
 * qup_spi_dev - spi device config structure
 * @ qup_base - base register address of QUP.
 * @ qup_irq - irq number of QUP.
 * @ tx_bytes - current transfered output data length in bytes
 * @ bytes_per_word - bytes number per word write to FIFO, valid range [1-4]
 * @ xfer - pointer to SPI transfer contents structure.
 */
struct qup_spi_dev {
	unsigned int qup_base;
	int qup_irq;
	int tx_bytes;
	unsigned int bytes_per_word;
	unsigned int bit_shift_en;
	unsigned int unpack_en;
	struct spi_transfer *xfer;
};

/* Function Definitions */
struct qup_spi_dev *qup_blsp_spi_init(uint8_t blsp_id, uint8_t qup_id);
int qup_spi_deinit(struct qup_spi_dev *dev);
int spi_qup_transfer(struct qup_spi_dev *dev, const unsigned char * tx_buf, unsigned int data_size);

#endif				/* __SPI_QUP__ */
