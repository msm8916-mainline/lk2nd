/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
 * QUP driver for Qualcomm MSM platforms
 *
 */

#include <debug.h>
#include <arch/arm.h>
#include <reg.h>
#include <kernel/thread.h>
#include <dev/gpio.h>
#include <stdlib.h>
#include <string.h>

#include <gsbi.h>
#include <i2c_qup.h>
#include <platform/irqs.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <platform/interrupts.h>

static struct qup_i2c_dev *dev_addr = NULL;

/* QUP Registers */
enum {
	QUP_CONFIG = 0x0,
	QUP_STATE = 0x4,
	QUP_IO_MODE = 0x8,
	QUP_SW_RESET = 0xC,
	QUP_OPERATIONAL = 0x18,
	QUP_ERROR_FLAGS = 0x1C,
	QUP_ERROR_FLAGS_EN = 0x20,
	QUP_MX_READ_CNT = 0x208,
	QUP_MX_INPUT_CNT = 0x200,
	QUP_MX_WR_CNT = 0x100,
	QUP_OUT_DEBUG = 0x108,
	QUP_OUT_FIFO_CNT = 0x10C,
	QUP_OUT_FIFO_BASE = 0x110,
	QUP_IN_READ_CUR = 0x20C,
	QUP_IN_DEBUG = 0x210,
	QUP_IN_FIFO_CNT = 0x214,
	QUP_IN_FIFO_BASE = 0x218,
	QUP_I2C_CLK_CTL = 0x400,
	QUP_I2C_STATUS = 0x404,
};

/* QUP States and reset values */
enum {
	QUP_RESET_STATE = 0,
	QUP_RUN_STATE = 1U,
	QUP_STATE_MASK = 3U,
	QUP_PAUSE_STATE = 3U,
	QUP_STATE_VALID = 1U << 2,
	QUP_I2C_MAST_GEN = 1U << 4,
	QUP_OPERATIONAL_RESET = 0xFF0,
	QUP_I2C_STATUS_RESET = 0xFFFFFC,
};

/* QUP OPERATIONAL FLAGS */
enum {
	QUP_OUT_SVC_FLAG = 1U << 8,
	QUP_IN_SVC_FLAG = 1U << 9,
	QUP_MX_INPUT_DONE = 1U << 11,
};

/* I2C mini core related values */
enum {
	I2C_MINI_CORE = 2U << 8,
	I2C_N_VAL = 0xF,

};

/* Packing Unpacking words in FIFOs , and IO modes*/
enum {
	QUP_WR_BLK_MODE = 1U << 10,
	QUP_RD_BLK_MODE = 1U << 12,
	QUP_UNPACK_EN = 1U << 14,
	QUP_PACK_EN = 1U << 15,
};

/* QUP tags */
enum {
	QUP_OUT_NOP = 0,
	QUP_OUT_START = 1U << 8,
	QUP_OUT_DATA = 2U << 8,
	QUP_OUT_STOP = 3U << 8,
	QUP_OUT_REC = 4U << 8,
	QUP_IN_DATA = 5U << 8,
	QUP_IN_STOP = 6U << 8,
	QUP_IN_NACK = 7U << 8,
};

/* Status, Error flags */
enum {
	I2C_STATUS_WR_BUFFER_FULL = 1U << 0,
	I2C_STATUS_BUS_ACTIVE = 1U << 8,
	I2C_STATUS_ERROR_MASK = 0x38000FC,
	QUP_I2C_NACK_FLAG = 1U << 3,
	QUP_IN_NOT_EMPTY = 1U << 5,
	QUP_STATUS_ERROR_FLAGS = 0x7C,
};

#ifdef DEBUG_QUP
static void qup_print_status(struct qup_i2c_dev *dev)
{
	unsigned val;
	val = readl(dev->qup_base + QUP_CONFIG);
	dprintf(INFO, "Qup config is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_STATE);
	dprintf(INFO, "Qup state is :0x%x\n", val);
	val = readl(dev->qup_base + QUP_IO_MODE);
	dprintf(INFO, "Qup mode is :0x%x\n", val);
}
#else
static inline void qup_print_status(struct qup_i2c_dev *dev)
{
}
#endif

static irqreturn_t qup_i2c_interrupt(void)
{
	struct qup_i2c_dev *dev = dev_addr;
	if (!dev) {
		dprintf(CRITICAL,
			"dev_addr is NULL, that means i2c_qup_init failed...\n");
		return IRQ_FAIL;
	}
	unsigned status = readl(dev->qup_base + QUP_I2C_STATUS);
	unsigned status1 = readl(dev->qup_base + QUP_ERROR_FLAGS);
	unsigned op_flgs = readl(dev->qup_base + QUP_OPERATIONAL);
	int err = 0;

	if (!dev->msg)
		return IRQ_HANDLED;

	if (status & I2C_STATUS_ERROR_MASK) {
		dprintf(CRITICAL, "QUP: I2C status flags :0x%x \n", status);
		err = -status;
		/* Clear Error interrupt if it's a level triggered interrupt */
		if (dev->num_irqs == 1) {
			writel(QUP_RESET_STATE, dev->qup_base + QUP_STATE);
		}
		goto intr_done;
	}

	if (status1 & 0x7F) {
		dprintf(CRITICAL, "QUP: QUP status flags :0x%x\n", status1);
		err = -status1;
		/* Clear Error interrupt if it's a level triggered interrupt */
		if (dev->num_irqs == 1)
			writel((status1 & QUP_STATUS_ERROR_FLAGS),
			       dev->qup_base + QUP_ERROR_FLAGS);
		goto intr_done;
	}

	if (op_flgs & QUP_OUT_SVC_FLAG)
		writel(QUP_OUT_SVC_FLAG, dev->qup_base + QUP_OPERATIONAL);
	if (dev->msg->flags == I2C_M_RD) {
		if ((op_flgs & QUP_MX_INPUT_DONE)
		    || (op_flgs & QUP_IN_SVC_FLAG))
			writel(QUP_IN_SVC_FLAG,
			       dev->qup_base + QUP_OPERATIONAL);
		else
			return IRQ_HANDLED;
	}

 intr_done:
	dev->err = err;
	return IRQ_HANDLED;
}

static int qup_i2c_poll_writeready(struct qup_i2c_dev *dev)
{
	unsigned retries = 0;

	while (retries != 2000) {
		unsigned status = readl(dev->qup_base + QUP_I2C_STATUS);

		if (!(status & I2C_STATUS_WR_BUFFER_FULL)) {
			if (!(status & I2C_STATUS_BUS_ACTIVE))
				return 0;
			else	/* 1-bit delay before we check for bus busy */
				udelay(dev->one_bit_t);
		}
		if (retries++ == 1000)
			udelay(100);
	}
	qup_print_status(dev);
	return -ETIMEDOUT;
}

static int qup_i2c_poll_state(struct qup_i2c_dev *dev, unsigned state)
{
	unsigned retries = 0;

	dprintf(INFO, "Polling Status for state:0x%x\n", state);

	while (retries != 2000) {
		unsigned status = readl(dev->qup_base + QUP_STATE);

		if ((status & (QUP_STATE_VALID | state)) ==
		    (QUP_STATE_VALID | state))
			return 0;
		else if (retries++ == 1000)
			udelay(100);
	}
	return -ETIMEDOUT;
}

#ifdef DEBUG
static void
qup_verify_fifo(struct qup_i2c_dev *dev, unsigned val, unsigned addr, int rdwr)
{
	if (rdwr)
		dprintf(INFO, "RD:Wrote 0x%x to out_ff:0x%x\n", val, addr);
	else
		dprintf(INFO, "WR:Wrote 0x%x to out_ff:0x%x\n", val, addr);
}
#else
static inline void
qup_verify_fifo(struct qup_i2c_dev *dev, unsigned val, unsigned addr, int rdwr)
{
}
#endif

static void
qup_issue_read(struct qup_i2c_dev *dev, struct i2c_msg *msg, int *idx,
	       unsigned carry_over)
{
	uint16_t addr = (msg->addr << 1) | 1;
	/* QUP limit 256 bytes per read. By HW design, 0 in the 8-bit field is
	   treated as 256 byte read. */
	uint16_t rd_len = ((dev->cnt == 256) ? 0 : dev->cnt);

	if (*idx % 4) {
		writel(carry_over | ((QUP_OUT_START | addr) << 16),
		       dev->qup_base + QUP_OUT_FIFO_BASE);

		qup_verify_fifo(dev, carry_over |
				((QUP_OUT_START | addr) << 16),
				(unsigned)dev->qup_base + QUP_OUT_FIFO_BASE +
				(*idx - 2), 1);
		writel((QUP_OUT_REC | rd_len),
		       dev->qup_base + QUP_OUT_FIFO_BASE);

		qup_verify_fifo(dev, (QUP_OUT_REC | rd_len),
				(unsigned)dev->qup_base + QUP_OUT_FIFO_BASE +
				(*idx + 2), 1);
	} else {
		writel(((QUP_OUT_REC | rd_len) << 16) |
		       QUP_OUT_START | addr, dev->qup_base + QUP_OUT_FIFO_BASE);

		qup_verify_fifo(dev, QUP_OUT_REC << 16 | rd_len << 16 |
				QUP_OUT_START | addr,
				(unsigned)dev->qup_base + QUP_OUT_FIFO_BASE +
				(*idx), 1);
	}
	*idx += 4;
}

static void
qup_issue_write(struct qup_i2c_dev *dev, struct i2c_msg *msg, int rem,
		int *idx, unsigned *carry_over)
{
	int entries = dev->cnt;
	int empty_sl = dev->wr_sz - ((*idx) >> 1);
	int i = 0;
	unsigned val = 0;
	unsigned last_entry = 0;
	uint16_t addr = msg->addr << 1;

	if (dev->pos == 0) {
		if (*idx % 4) {
			writel(*carry_over | ((QUP_OUT_START | addr) << 16),
			       dev->qup_base + QUP_OUT_FIFO_BASE);

			qup_verify_fifo(dev, *carry_over | QUP_OUT_DATA << 16 |
					addr << 16, (unsigned)dev->qup_base +
					QUP_OUT_FIFO_BASE + (*idx) - 2, 0);
		} else
			val = QUP_OUT_START | addr;
		*idx += 2;
		i++;
		entries++;
	} else {
		/* Avoid setp time issue by adding 1 NOP when number of bytes are more
		   than FIFO/BLOCK size. setup time issue can't appear otherwise since
		   next byte to be written will always be ready */
		val = (QUP_OUT_NOP | 1);
		*idx += 2;
		i++;
		entries++;
	}
	if (entries > empty_sl)
		entries = empty_sl;

	for (; i < (entries - 1); i++) {
		if (*idx % 4) {
			writel(val | ((QUP_OUT_DATA |
				       msg->buf[dev->pos]) << 16),
			       dev->qup_base + QUP_OUT_FIFO_BASE);

			qup_verify_fifo(dev, val | QUP_OUT_DATA << 16 |
					msg->buf[dev->pos] << 16,
					(unsigned)dev->qup_base +
					QUP_OUT_FIFO_BASE + (*idx) - 2, 0);
		} else
			val = QUP_OUT_DATA | msg->buf[dev->pos];
		(*idx) += 2;
		dev->pos++;
	}
	if (dev->pos < (msg->len - 1))
		last_entry = QUP_OUT_DATA;
	else if (rem > 1)	/* not last array entry */
		last_entry = QUP_OUT_DATA;
	else
		last_entry = QUP_OUT_STOP;
	if ((*idx % 4) == 0) {
		/*
		 * If read-start and read-command end up in different fifos, it
		 * may result in extra-byte being read due to extra-read cycle.
		 * Avoid that by inserting NOP as the last entry of fifo only
		 * if write command(s) leave 1 space in fifo.
		 */
		if (rem > 1) {
			struct i2c_msg *next = msg + 1;
			if (next->addr == msg->addr && (next->flags & I2C_M_RD)
			    && *idx == ((dev->wr_sz * 2) - 4)) {
				writel(((last_entry | msg->buf[dev->pos]) |
					((1 | QUP_OUT_NOP) << 16)),
				       dev->qup_base + QUP_OUT_FIFO_BASE);
				*idx += 2;
			} else
				*carry_over = (last_entry | msg->buf[dev->pos]);
		} else {
			writel((last_entry | msg->buf[dev->pos]),
			       dev->qup_base + QUP_OUT_FIFO_BASE);

			qup_verify_fifo(dev, last_entry | msg->buf[dev->pos],
					(unsigned)dev->qup_base +
					QUP_OUT_FIFO_BASE + (*idx), 0);
		}
	} else {
		writel(val | ((last_entry | msg->buf[dev->pos]) << 16),
		       dev->qup_base + QUP_OUT_FIFO_BASE);

		qup_verify_fifo(dev, val | (last_entry << 16) |
				(msg->buf[dev->pos] << 16),
				(unsigned)dev->qup_base + QUP_OUT_FIFO_BASE +
				(*idx) - 2, 0);
	}

	*idx += 2;
	dev->pos++;
	dev->cnt = msg->len - dev->pos;
}

static int qup_update_state(struct qup_i2c_dev *dev, unsigned state)
{
	if (qup_i2c_poll_state(dev, 0) != 0)
		return -EIO;
	writel(state, dev->qup_base + QUP_STATE);
	if (qup_i2c_poll_state(dev, state) != 0)
		return -EIO;
	return 0;
}

static int qup_set_read_mode(struct qup_i2c_dev *dev, int rd_len)
{
	unsigned wr_mode =
	    (dev->wr_sz < dev->out_fifo_sz) ? QUP_WR_BLK_MODE : 0;
	if (rd_len > 256) {
		dprintf(INFO, "HW doesn't support READs > 256 bytes\n");
		return -EPROTONOSUPPORT;
	}
	if (rd_len <= dev->in_fifo_sz) {
		writel(wr_mode | QUP_PACK_EN | QUP_UNPACK_EN,
		       dev->qup_base + QUP_IO_MODE);
		writel(rd_len, dev->qup_base + QUP_MX_READ_CNT);
	} else {
		writel(wr_mode | QUP_RD_BLK_MODE |
		       QUP_PACK_EN | QUP_UNPACK_EN,
		       dev->qup_base + QUP_IO_MODE);
		writel(rd_len, dev->qup_base + QUP_MX_INPUT_CNT);
	}
	return 0;
}

static int qup_set_wr_mode(struct qup_i2c_dev *dev, int rem)
{
	int total_len = 0;
	int ret = 0;
	if (dev->msg->len >= (dev->out_fifo_sz - 1)) {
		total_len =
		    dev->msg->len + 1 + (dev->msg->len / (dev->out_blk_sz - 1));
		writel(QUP_WR_BLK_MODE | QUP_PACK_EN | QUP_UNPACK_EN,
		       dev->qup_base + QUP_IO_MODE);
		dev->wr_sz = dev->out_blk_sz;
	} else
		writel(QUP_PACK_EN | QUP_UNPACK_EN,
		       dev->qup_base + QUP_IO_MODE);

	if (rem > 1) {
		struct i2c_msg *next = dev->msg + 1;
		if (next->addr == dev->msg->addr && next->flags == I2C_M_RD) {
			ret = qup_set_read_mode(dev, next->len);
			/* make sure read start & read command are in 1 blk */
			if ((total_len % dev->out_blk_sz) ==
			    (dev->out_blk_sz - 1))
				total_len += 3;
			else
				total_len += 2;
		}
	}
	/* WRITE COUNT register valid/used only in block mode */
	if (dev->wr_sz == dev->out_blk_sz)
		writel(total_len, dev->qup_base + QUP_MX_WR_CNT);
	return ret;
}

int qup_i2c_xfer(struct qup_i2c_dev *dev, struct i2c_msg msgs[], int num)
{
	int ret;
	int rem = num;
	int err;

	if (dev->suspended) {
		return -EIO;
	}

	/* Set the GSBIn_QUP_APPS_CLK to 24MHz, then below figure out what speed to
	   run I2C_MASTER_CORE at. */
	if (dev->clk_state == 0) {
		if (dev->clk_ctl == 0) {
			clock_config_i2c(dev->gsbi_number, dev->src_clk_freq);
		}
	}
	/* Initialize QUP registers during first transfer */
	if (dev->clk_ctl == 0) {
		int fs_div;
		int hs_div;
		unsigned fifo_reg;
		/* Configure the GSBI Protocol Code for i2c */
		writel((GSBI_PROTOCOL_CODE_I2C <<
			GSBI_CTRL_REG_PROTOCOL_CODE_S),
		       GSBI_CTRL_REG(dev->gsbi_base));

		fs_div = ((dev->src_clk_freq / dev->clk_freq) / 2) - 3;
		hs_div = 3;
		dev->clk_ctl = ((hs_div & 0x7) << 8) | (fs_div & 0xff);
		fifo_reg = readl(dev->qup_base + QUP_IO_MODE);
		if (fifo_reg & 0x3)
			dev->out_blk_sz = (fifo_reg & 0x3) * 16;
		else
			dev->out_blk_sz = 16;
		if (fifo_reg & 0x60)
			dev->in_blk_sz = ((fifo_reg & 0x60) >> 5) * 16;
		else
			dev->in_blk_sz = 16;
		/*
		 * The block/fifo size w.r.t. 'actual data' is 1/2 due to 'tag'
		 * associated with each byte written/received
		 */
		dev->out_blk_sz /= 2;
		dev->in_blk_sz /= 2;
		dev->out_fifo_sz =
		    dev->out_blk_sz * (2 << ((fifo_reg & 0x1C) >> 2));
		dev->in_fifo_sz =
		    dev->in_blk_sz * (2 << ((fifo_reg & 0x380) >> 7));
		dprintf(INFO, "QUP IN:bl:%d, ff:%d, OUT:bl:%d, ff:%d\n",
			dev->in_blk_sz, dev->in_fifo_sz, dev->out_blk_sz,
			dev->out_fifo_sz);
	}

	unmask_interrupt(dev->qup_irq);
	writel(1, dev->qup_base + QUP_SW_RESET);
	ret = qup_i2c_poll_state(dev, QUP_RESET_STATE);
	if (ret) {
		dprintf(INFO, "QUP Busy:Trying to recover\n");
		goto out_err;
	}

	/* Initialize QUP registers */
	writel(0, dev->qup_base + QUP_CONFIG);
	writel(QUP_OPERATIONAL_RESET, dev->qup_base + QUP_OPERATIONAL);
	writel(QUP_STATUS_ERROR_FLAGS, dev->qup_base + QUP_ERROR_FLAGS_EN);

	writel(I2C_MINI_CORE | I2C_N_VAL, dev->qup_base + QUP_CONFIG);

	/* Initialize I2C mini core registers */
	writel(0, dev->qup_base + QUP_I2C_CLK_CTL);
	writel(QUP_I2C_STATUS_RESET, dev->qup_base + QUP_I2C_STATUS);

	dev->cnt = msgs->len;
	dev->pos = 0;
	dev->msg = msgs;
	while (rem) {
		int filled = FALSE;

		dev->wr_sz = dev->out_fifo_sz;
		dev->err = 0;

		if (qup_i2c_poll_state(dev, QUP_I2C_MAST_GEN) != 0) {
			ret = -EIO;
			goto out_err;
		}

		qup_print_status(dev);
		/* HW limits Read upto 256 bytes in 1 read without stop */
		if (dev->msg->flags & I2C_M_RD) {
			ret = qup_set_read_mode(dev, dev->cnt);
			if (ret != 0)
				goto out_err;
		} else {
			ret = qup_set_wr_mode(dev, rem);
			if (ret != 0)
				goto out_err;
			/* Don't fill block till we get interrupt */
			if (dev->wr_sz == dev->out_blk_sz)
				filled = TRUE;
		}

		err = qup_update_state(dev, QUP_RUN_STATE);
		if (err < 0) {
			ret = err;
			goto out_err;
		}

		qup_print_status(dev);
		writel(dev->clk_ctl, dev->qup_base + QUP_I2C_CLK_CTL);

		do {
			int idx = 0;
			unsigned carry_over = 0;

			/* Transition to PAUSE state only possible from RUN */
			err = qup_update_state(dev, QUP_PAUSE_STATE);
			if (err < 0) {
				ret = err;
				goto out_err;
			}

			qup_print_status(dev);
			/* This operation is Write, check the next operation and decide
			   mode */
			while (filled == FALSE) {
				if ((msgs->flags & I2C_M_RD)
				    && (dev->cnt == msgs->len))
					qup_issue_read(dev, msgs, &idx,
						       carry_over);
				else if (!(msgs->flags & I2C_M_RD))
					qup_issue_write(dev, msgs, rem, &idx,
							&carry_over);
				if (idx >= (dev->wr_sz << 1))
					filled = TRUE;
				/* Start new message */
				if (filled == FALSE) {
					if (msgs->flags & I2C_M_RD)
						filled = TRUE;
					else if (rem > 1) {
						/* Only combine operations with same address */
						struct i2c_msg *next = msgs + 1;
						if (next->addr != msgs->addr
						    || next->flags == 0)
							filled = TRUE;
						else {
							rem--;
							msgs++;
							dev->msg = msgs;
							dev->pos = 0;
							dev->cnt = msgs->len;
						}
					} else
						filled = TRUE;
				}
			}
			err = qup_update_state(dev, QUP_RUN_STATE);
			if (err < 0) {
				ret = err;
				goto out_err;
			}
			dprintf(INFO, "idx:%d, rem:%d, num:%d, mode:%d\n",
				idx, rem, num, dev->mode);

			qup_print_status(dev);
			if (dev->err) {
				if (dev->err & QUP_I2C_NACK_FLAG) {
					dprintf(CRITICAL,
						"I2C slave addr:0x%x not connected\n",
						dev->msg->addr);
				} else {
					dprintf(INFO,
						"QUP data xfer error %d\n",
						dev->err);
				}
				ret = dev->err;
				goto out_err;
			}
			if (dev->msg->flags & I2C_M_RD) {
				int i;
				unsigned dval = 0;
				for (i = 0; dev->pos < dev->msg->len;
				     i++, dev->pos++) {
					unsigned rd_status =
					    readl(dev->qup_base +
						  QUP_OPERATIONAL);
					if (i % 2 == 0) {
						if ((rd_status &
						     QUP_IN_NOT_EMPTY) == 0)
							break;
						dval =
						    readl(dev->qup_base +
							  QUP_IN_FIFO_BASE);
						dev->msg->buf[dev->pos] =
						    dval & 0xFF;
					} else
						dev->msg->buf[dev->pos] =
						    ((dval & 0xFF0000) >> 16);
				}
				dev->cnt -= i;
			} else
				filled = FALSE;	/* refill output FIFO */
		}
		while (dev->cnt > 0);
		if (dev->cnt == 0) {
			rem--;
			msgs++;
			if (rem) {
				dev->pos = 0;
				dev->cnt = msgs->len;
				dev->msg = msgs;
			}
		}
		/* Wait for I2C bus to be idle */
		ret = qup_i2c_poll_writeready(dev);
		if (ret) {
			dprintf(INFO, "Error waiting for write ready\n");
			goto out_err;
		}
	}

	ret = num;
 out_err:
	dev->msg = NULL;
	dev->pos = 0;
	dev->err = 0;
	dev->cnt = 0;
	mask_interrupt(dev->qup_irq);
	return ret;
}

struct qup_i2c_dev *qup_i2c_init(uint8_t gsbi_id, unsigned clk_freq,
				 unsigned src_clk_freq)
{
	struct qup_i2c_dev *dev;
	if (dev_addr != NULL) {
		return dev_addr;
	}

	dev = malloc(sizeof(struct qup_i2c_dev));
	if (!dev) {
		return NULL;
	}
	dev = memset(dev, 0, sizeof(struct qup_i2c_dev));

	/* Setup base addresses and irq based on gsbi_id */
	dev->qup_irq = GSBI_QUP_IRQ(gsbi_id);
	dev->qup_base = QUP_BASE(gsbi_id);
	dev->gsbi_base = GSBI_BASE(gsbi_id);
	dev->gsbi_number = gsbi_id;

	/* This must be done for qup_i2c_interrupt to work. */
	dev_addr = dev;

	/* Initialize the GPIO for GSBIn as i2c */
	gpio_config_i2c(dev->gsbi_number);

	/* Configure the GSBI Protocol Code for i2c */
	writel((GSBI_PROTOCOL_CODE_I2C <<
		GSBI_CTRL_REG_PROTOCOL_CODE_S), GSBI_CTRL_REG(dev->gsbi_base));

	/* Set clk_freq and src_clk_freq for i2c. */
	dev->clk_freq = clk_freq;
	dev->src_clk_freq = src_clk_freq;

	dev->num_irqs = 1;

	dev->one_bit_t = USEC_PER_SEC / dev->clk_freq;
	dev->clk_ctl = 0;

	/* Register the GSBIn QUP IRQ */
	register_int_handler(dev->qup_irq, (int_handler) qup_i2c_interrupt, 0);

	/* Then disable it */
	mask_interrupt(dev->qup_irq);

	return dev;
}

int qup_i2c_deinit(struct qup_i2c_dev *dev)
{
	/* Disable the qup_irq */
	mask_interrupt(dev->qup_irq);
	/* Free the memory used for dev */
	free(dev);
	return 0;
}
