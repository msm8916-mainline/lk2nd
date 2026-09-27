/* Copyright (c) 2017-2018, 2020, The Linux Foundation. All rights reserved.
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

/*
 * QUP spi driver for Qualcomm MSM platforms
 *
 */

#include <debug.h>
#include <arch/arm.h>
#include <reg.h>
#include <kernel/thread.h>
#include <stdlib.h>
#include <string.h>
#include <gsbi.h>
#include <spi_qup.h>
#include <platform/irqs.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <platform/interrupts.h>

//#define DEBUGLEVEL 3

static unsigned int spi_get_qup_hw_ver(struct qup_spi_dev *dev)
{
	unsigned int data = readl_relaxed(dev->qup_base + QUP_HW_VERSION);

	dprintf(SPEW, "Qup hardware version is 0x%x\n", data);
	return data;
}

static void qup_print_status(struct qup_spi_dev *dev)
{
	unsigned val;
	val = readl(dev->qup_base + QUP_CONFIG);
	dprintf(SPEW, "Qup config is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_STATE);
	dprintf(SPEW, "Qup state is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_IO_MODES);
	dprintf(SPEW, "Qup mode is :0x%x\n", val);
	val = readl(dev->qup_base + SPI_IO_CONTROL);
	dprintf(SPEW, "SPI_IO_CONTROL is :0x%x\n", val);
	val = readl(dev->qup_base + SPI_CONFIG);
	dprintf(SPEW, "SPI_CONFIG is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_WRITE_CNT_CURRENT);
	dprintf(SPEW, "QUP_MX_WRITE_CNT_CURRENT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_WRITE_CNT);
	dprintf(SPEW, "QUP_MX_WRITE_CNT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_OUTPUT_CNT_CURRENT);
	dprintf(SPEW, "QUP_MX_OUTPUT_CNT_CURRENT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_OUTPUT_CNT);
	dprintf(SPEW, "QUP_MX_OUTPUT_CNT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_INPUT_CNT_CURRENT);
	dprintf(SPEW, "QUP_MX_INPUT_CNT_CURRENT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_INPUT_CNT);
	dprintf(SPEW, "QUP_MX_INPUT_CNT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_MX_READ_CNT);
	dprintf(SPEW, "QUP_MX_READ_CNT is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_OPERATIONAL);
	dprintf(SPEW, "QUP_OPERATIONAL is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_OUTPUT_FIFO_WORD_CNT);
	dprintf(SPEW, "QUP_OUTPUT_FIFO_WORD_CNT is :0x%x\n", val);
}

static inline bool qup_state_is_valid(struct qup_spi_dev *dev)
{
	unsigned int st = readl_relaxed(dev->qup_base + QUP_STATE);

	return st & QUP_STATE_VALID;
}

static inline int qup_wait_state_valid(struct qup_spi_dev *dev)
{
	unsigned retries = 0xFFFF;

	while (!qup_state_is_valid(dev) && (--retries != 0));

	if(!retries)
		return -ETIMEDOUT;

	return 0;
}

static int qup_poll_state(struct qup_spi_dev *dev, unsigned state)
{
	int ret = 0;
	unsigned int status;

	ret = qup_wait_state_valid(dev);

	if(ret)
		goto exit;

	status = readl(dev->qup_base + QUP_STATE);
	if ((status & (QUP_STATE_VALID | state)) ==
		(QUP_STATE_VALID | state))
			return 0;

exit:
	dprintf(SPEW, "Polling fail for state:0x%x\n", state);
	qup_print_status(dev);
	return ret;
}

static inline int qup_set_state(struct qup_spi_dev *dev,
				    enum qup_state state)
{
	enum qup_state cur_state;

	if (qup_wait_state_valid(dev)) {
		qup_print_status(dev);
		return -EIO;
	}

	cur_state = readl_relaxed(dev->qup_base + QUP_STATE);

	/* For PAUSE_STATE to RESET_STATE,
	 * two writes of (0b10) are required.
	 */
	if (((cur_state & QUP_STATE_MASK) == QUP_PAUSE_STATE) &&
			(state == QUP_RESET_STATE)) {
		writel(QUP_STATE_CLEAR_BITS, dev->qup_base + QUP_STATE);
		writel(QUP_STATE_CLEAR_BITS, dev->qup_base + QUP_STATE);
	} else {
		writel((cur_state & ~QUP_STATE_MASK) | state,
		       dev->qup_base + QUP_STATE);
	}

	if (qup_poll_state(dev, state)) {
		return -EIO;
	}

	return 0;
}

static inline void qup_register_init(struct qup_spi_dev *dev)
{
	/* Initialize QUP registers */
	qup_set_state(dev, QUP_RESET_STATE);
	writel(0x00000001, dev->qup_base + QUP_SW_RESET);
	qup_wait_state_valid(dev);
	qup_set_state(dev, QUP_RESET_STATE);

	writel(0x00000000, dev->qup_base + QUP_CONFIG);
	writel(0x00000000, dev->qup_base + QUP_IO_MODES);
	writel(0xfff0, dev->qup_base + QUP_OPERATIONAL);
	writel(0x7e, dev->qup_base + QUP_ERROR_FLAGS);
	writel(0x00000000, dev->qup_base + QUP_TEST_CTRL);
	writel(0x00000000, dev->qup_base + QUP_OPERATIONAL_MASK);
	writel(0x0, dev->qup_base + QUP_MX_INPUT_CNT);
	writel(0x1, dev->qup_base + QUP_MX_OUTPUT_CNT);
	writel(0x0, dev->qup_base + QUP_MX_READ_CNT);
}

static inline void spi_register_init(struct qup_spi_dev *dev)
{
	unsigned int io_config;

	/* Set SPI mini core to QUP config */
	writel(QUP_CONFIG_SPI_MODE, dev->qup_base + QUP_CONFIG);

	/* Initialize SPI mini core registers */
	writel(0, dev->qup_base + SPI_CONFIG);

	io_config = SPI_IO_C_NO_TRI_STATE | SPI_IO_C_CS_SELECT_CS0 | SPI_IO_C_CLK_IDLE_HIGH;
	/* Set cs always is low to spi io config */
	if (!dev->force_cs_dis)
		io_config |= SPI_IO_C_FORCE_CS | SPI_IO_C_MX_CS_MODE;
	writel(io_config, dev->qup_base + SPI_IO_CONTROL);
	writel(SPI_ERROR_CLK_OVER_RUN | SPI_ERROR_CLK_UNDER_RUN, dev->qup_base + SPI_ERROR_FLAGS_EN);
	writel(0, dev->qup_base + SPI_DEASSERT_WAIT);

	qup_print_status(dev);
}

static void spi_qup_io_config_block(struct qup_spi_dev *dev, int len)
{
	unsigned int config, iomode, mode;
	unsigned int bits_per_word, max_len;

	qup_register_init(dev);
	spi_register_init(dev);

	/* bytes_per_word valid range is [1-4].
	 * Other value may not working as expected.
	 */
	dev->bytes_per_word = dev->bytes_per_word % 5;

	/* If bytes_per_word not given, use DEFAULT_BYTES_PER_WORD.
	 */
	if(!dev->bytes_per_word)
		dev->bytes_per_word = DEFAULT_BYTES_PER_WORD;

	bits_per_word = dev->bytes_per_word * 8 - 1;
	max_len = len / dev->bytes_per_word > 0 ? len / dev->bytes_per_word : 1;

	if (dev->xfer->tx_buf == NULL || dev->xfer->len == 0)
		writel(0, dev->qup_base + QUP_MX_OUTPUT_CNT);
	else
		writel(max_len, dev->qup_base + QUP_MX_OUTPUT_CNT);

	if (dev->xfer->rx_buf == NULL || dev->xfer->len == 0) {
		writel(0, dev->qup_base + QUP_MX_INPUT_CNT);
		writel(0, dev->qup_base + QUP_MX_READ_CNT);
	} else {
		writel(max_len, dev->qup_base + QUP_MX_INPUT_CNT);
		writel(max_len, dev->qup_base + QUP_MX_READ_CNT);
	}
	writel(0, dev->qup_base + QUP_MX_WRITE_CNT);

	mode = QUP_IO_MODES_BLOCK;
	iomode = readl_relaxed(dev->qup_base + QUP_IO_MODES);
	iomode &= ~(INPUT_MODE_MASK | OUTPUT_MODE_MASK);
	if(dev->unpack_en)
		iomode |= QUP_IO_MODES_UNPACK_EN;
	else
		iomode &= ~QUP_IO_MODES_UNPACK_EN;
	iomode |= (mode << OUTPUT_MODE_SHIFT);
	iomode |= (mode << INPUT_MODE_SHIFT);
	if(dev->bit_shift_en)
		iomode |= QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN;
	else
		iomode &= ~QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN;
	writel(iomode, dev->qup_base + QUP_IO_MODES);

	config = readl_relaxed(dev->qup_base + QUP_CONFIG);
	if (dev->xfer->tx_buf == NULL || dev->xfer->len == 0)
		config |= QUP_CONFIG_NO_OUTPUT;
	if (dev->xfer->rx_buf == NULL || dev->xfer->len == 0)
		config |= QUP_CONFIG_NO_INPUT;
	config |= QUP_CONFIG_SPI_MODE;
	config |= bits_per_word;
	writel(config, dev->qup_base + QUP_CONFIG);

	writel(0, dev->qup_base + QUP_OPERATIONAL_MASK);

	unmask_interrupt(dev->qup_irq);
}

static void spi_qup_fifo_write(struct qup_spi_dev *dev, struct spi_transfer *xfer)
{
	const unsigned char *tx_buf = xfer->tx_buf;
	unsigned int word, state, data;
	unsigned int idx;

	while (dev->tx_bytes < xfer->len) {

		state = readl_relaxed(dev->qup_base + QUP_OPERATIONAL);
		if (state & QUP_OP_OUT_FIFO_FULL)
		{
			dprintf(SPEW, "%s: oper QUP_OP_OUT_FIFO_FULL: 0x%x"
				"dev->tx_bytes:0x%x, xfer->len:0x%x\n",
				__func__, state, dev->tx_bytes, xfer->len);
			break;
		}

		word = 0;
		for (idx = 0; idx < dev->bytes_per_word; idx++, dev->tx_bytes++) {

			if (!tx_buf) {
				dprintf(CRITICAL, "%s: tx_buf null error.\n", __func__);
				dev->tx_bytes += dev->bytes_per_word;
				break;
			}
			data = dev->tx_bytes < xfer->len ? tx_buf[dev->tx_bytes] : 0;
			word |= data << (BITS_PER_BYTE * idx);
		}

		writel(word, dev->qup_base + QUP_OUTPUT_FIFO_BASE);
	}
}

static void spi_qup_fifo_read(struct qup_spi_dev *dev, struct spi_transfer *xfer)
{
	unsigned char *rx_buf = xfer->rx_buf;
	unsigned int word, state;
	unsigned int idx, shift;

	while (dev->rx_bytes < xfer->len) {
		/* Wait for core to receive a block of MISO data before attempting
		* to retrieve it. This prevents looping up and outputting another
		* MOSI block if the input svc flag is not set yet. In that case,
		* MISO data is lost.
		*/
		state = readl_relaxed(dev->qup_base + QUP_OPERATIONAL);
		if (!(state & QUP_OP_IN_SERVICE_FLAG))
			continue;

		/* Input another data block if one is available */
		state = readl_relaxed(dev->qup_base + QUP_OPERATIONAL);
		if (state & QUP_OP_IN_SERVICE_FLAG) {
			state &= ~QUP_OP_IN_SERVICE_FLAG;
			writel(state, dev->qup_base + QUP_OPERATIONAL);

			while (dev->rx_bytes < xfer->len) {
				if (!(state & QUP_OP_IN_FIFO_NOT_EMPTY))
					return;

				word = readl_relaxed(dev->qup_base + QUP_INPUT_FIFO_BASE);
				for (idx = 0; idx < dev->bytes_per_word; idx++, dev->rx_bytes++) {
					/*
					* The data format depends on bytes per SPI word:
					*  4 bytes: 0x12345678
					*  2 bytes: 0x00001234
					*  1 byte : 0x00000012
					*/
					shift = BITS_PER_BYTE;
					shift *= (dev->bytes_per_word - idx - 1);
					rx_buf[dev->rx_bytes] = word >> shift;
				}
			}
		}
	}
}

static int _spi_qup_transfer(struct qup_spi_dev *dev, struct spi_transfer *xfer)
{
	int ret = -EIO;
	int retries = 0xFF;
	unsigned val;
	unsigned int register_cnt;
	unsigned int register_cnt_cur;

	dev->xfer     = xfer;
	dev->tx_bytes = 0;
	dev->rx_bytes = 0;

	if (xfer->tx_buf && xfer->len != 0) {
		register_cnt = dev->qup_base + QUP_MX_OUTPUT_CNT;
		register_cnt_cur = dev->qup_base + QUP_MX_OUTPUT_CNT_CURRENT;
	} else if (xfer->rx_buf && xfer->len != 0) {
		register_cnt = dev->qup_base + QUP_MX_INPUT_CNT;
		register_cnt_cur = dev->qup_base + QUP_MX_INPUT_CNT_CURRENT;
	} else {
		return ret;
	}

	spi_qup_io_config_block(dev, xfer->len);
	ret = qup_set_state(dev, QUP_RUN_STATE);
	if (ret) {
		dprintf(CRITICAL, "%s: cannot set RUN state\n", __func__);
		goto exit;
	}

	while((readl(register_cnt) != readl(register_cnt_cur)) && retries--);
	if (xfer->tx_buf) {
		while(readl(register_cnt_cur)) {
			val = readl(dev->qup_base + QUP_OPERATIONAL);
			val &= ~QUP_OP_OUT_SERVICE_FLAG;
			writel(val, dev->qup_base + QUP_OPERATIONAL);

			ret = qup_set_state(dev, QUP_PAUSE_STATE);
			if (ret) {
				dprintf(CRITICAL, "%s: cannot set PAUSE state\n", __func__);
				goto exit;
			}

			spi_qup_fifo_write(dev, xfer);

			ret = qup_set_state(dev, QUP_RUN_STATE);
			if (ret) {
				dprintf(CRITICAL, "%s: cannot set RUN state\n", __func__);
				goto exit;
			}
		}
	} else if (xfer->rx_buf) {
		spi_qup_fifo_read(dev, xfer);
	}

exit:
	qup_set_state(dev, QUP_RESET_STATE);
	dev->xfer = NULL;
	return ret;
}

static int spi_qup_transfer(struct qup_spi_dev *dev, struct spi_transfer *xfer)
{
	unsigned int cur = 0;
	unsigned int data_size = 0;
	void *mTmpbuff = NULL;
	int ret = -EIO;

	if (!xfer || !dev)
		return ret;

	mTmpbuff = xfer->rx_buf ? (unsigned char *) xfer->rx_buf : (unsigned char *) xfer->tx_buf;
	if (!mTmpbuff || !xfer->len)
		return ret;

	if (xfer->speed_hz != dev->max_speed_hz)
		clock_config_blsp_spi(dev->blsp_id, dev->qup_id, xfer->speed_hz);

	data_size = xfer->len;
	while(data_size > MAX_QUP_MX_TRANSFER_COUNT + cur) {
		xfer->len = MAX_QUP_MX_TRANSFER_COUNT;
		if (xfer->tx_buf)
			xfer->tx_buf = mTmpbuff + cur;
		else if (xfer->rx_buf)
			xfer->rx_buf = mTmpbuff + cur;

		ret = _spi_qup_transfer(dev, xfer);
		if (ret)
			return ret;

		cur += MAX_QUP_MX_TRANSFER_COUNT;
	}

	xfer->len = data_size - cur;
	if (xfer->tx_buf)
		xfer->tx_buf = mTmpbuff + cur;
	else if (xfer->rx_buf)
		xfer->rx_buf = mTmpbuff + cur;
	ret = _spi_qup_transfer(dev, xfer);

	return ret;
}

/**
 * @brief write data_size bytes data from tx_buf via spi
 * @param dev		SPI config structure initialized from qup_blsp_spi_init
 * @param tx_buf	output buffer pointer
 * @param data_size	Should be multiple of max bytes per word
 */
int spi_qup_write(struct qup_spi_dev *dev, const unsigned char *tx_buf, unsigned int data_size)
{
	int ret = -EIO;

	struct spi_transfer s_xfer = {
		.tx_buf = tx_buf,
		.rx_buf = NULL,
		.len = data_size,
		.speed_hz = MAX_SPEED_HZ,
	};

	ret = spi_qup_transfer(dev, &s_xfer);
	if (ret)
		dprintf(CRITICAL, "%s: write error!\n", __func__);

	dprintf(SPEW, "dev->tx_bytes:0x%x, s_xfer->len:0x%x\n",
		dev->tx_bytes, s_xfer.len);
	return ret;
}

/**
 * @brief read data_size bytes data from rx_buf via spi
 * @param dev		SPI config structure initialized from qup_blsp_spi_init
 * @param rx_buf	             input buffer pointer
 * @param data_size	Should be multiple of max bytes per word
 */
int spi_qup_read(struct qup_spi_dev *dev, unsigned char *rx_buf, unsigned int data_size)
{
	int ret = -EIO;

	struct spi_transfer s_xfer = {
		.tx_buf = NULL,
		.rx_buf = rx_buf,
		.len = data_size,
		.speed_hz = MAX_READ_SPEED_HZ,
	};

	ret = spi_qup_transfer(dev, &s_xfer);
	if (ret)
		dprintf(CRITICAL, "%s: read error!\n", __func__);

	dprintf(SPEW, "dev->rx_bytes:0x%x, s_xfer->len:0x%x\n",
		dev->rx_bytes, s_xfer.len);
	return ret;
}

static enum handler_return qup_spi_interrupt(void *arg)
{
	struct qup_spi_dev *dev = (struct qup_spi_dev *)arg;
	unsigned int opflags, qup_err, spi_err;

	if (!dev) {
		dprintf(CRITICAL,
			"dev_addr is NULL, that means spi_qup_init failed...\n");
		return INT_NO_RESCHEDULE;
	}

	qup_err = readl_relaxed(dev->qup_base + QUP_ERROR_FLAGS);
	spi_err = readl_relaxed(dev->qup_base + SPI_ERROR_FLAGS);
	opflags = readl_relaxed(dev->qup_base + QUP_OPERATIONAL);

	/* Writing a 'one' to the error bit to clear it. */
	writel(qup_err, dev->qup_base + QUP_ERROR_FLAGS);
	writel(spi_err, dev->qup_base + SPI_ERROR_FLAGS);
	writel(opflags, dev->qup_base + QUP_OPERATIONAL);

	if (qup_err) {
		if (qup_err & QUP_ERROR_OUTPUT_OVER_RUN)
			dprintf(SPEW, "OUTPUT_OVER_RUN\n");
		if (qup_err & QUP_ERROR_INPUT_UNDER_RUN)
			dprintf(SPEW, "INPUT_UNDER_RUN\n");
		if (qup_err & QUP_ERROR_OUTPUT_UNDER_RUN)
			dprintf(SPEW, "OUTPUT_UNDER_RUN\n");
		if (qup_err & QUP_ERROR_INPUT_OVER_RUN)
			dprintf(SPEW, "INPUT_OVER_RUN\n");
	}

	if (spi_err) {
		if (spi_err & SPI_ERROR_CLK_OVER_RUN)
			dprintf(SPEW, "CLK_OVER_RUN\n");
		if (spi_err & SPI_ERROR_CLK_UNDER_RUN)
			dprintf(SPEW, "CLK_UNDER_RUN\n");
	}

	return INT_NO_RESCHEDULE;
}

static void qup_spi_sec_init(struct qup_spi_dev *dev)
{
	/* Get qup hw version */
	spi_get_qup_hw_ver(dev);

	qup_register_init(dev);
	spi_register_init(dev);

	/* Register the GSBIn QUP IRQ */
	register_int_handler(dev->qup_irq, qup_spi_interrupt, dev);

	/* Then disable it */
	mask_interrupt(dev->qup_irq);
}

struct qup_spi_dev *qup_blsp_spi_init(uint8_t blsp_id, uint8_t qup_id)
{
	struct qup_spi_dev *dev;

	dev = malloc(sizeof(struct qup_spi_dev));
	if (!dev) {
		return NULL;
	}
	dev = memset(dev, 0, sizeof(struct qup_spi_dev));

	/* Platform uses BLSP */
	dev->blsp_id = blsp_id;
	dev->qup_id = qup_id;
	dev->qup_irq = BLSP_QUP_IRQ(blsp_id, qup_id);
	dev->qup_base = BLSP_QUP_BASE(blsp_id, qup_id);

	/* Initialize the GPIO for BLSP spi */
	gpio_config_blsp_spi(blsp_id, qup_id);

	/* Set the highest clk frequency by default for good performance. */
	dev->max_speed_hz = MAX_SPEED_HZ;
	clock_config_blsp_spi(blsp_id, qup_id, dev->max_speed_hz);

	qup_spi_sec_init(dev);

	return dev;
}

int qup_spi_deinit(struct qup_spi_dev *dev)
{
	/* Disable the qup_irq */
	mask_interrupt(dev->qup_irq);
	/* Free the memory used for dev */
	free(dev);
	return 0;
}
