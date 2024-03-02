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

#ifndef  __QUP__
#define  __QUP__

#include <stdint.h>

#define MAX_READ_SPEED_HZ 9600000
#define MAX_SPEED_HZ 50000000

/* QUP_IO_MODES fields */
#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN  0x00010000
#define QUP_IO_MODES_PACK_EN              0x00008000
#define QUP_IO_MODES_UNPACK_EN            0x00004000
#define QUP_IO_MODES_INPUT_MODE           0x00003000
#define QUP_IO_MODES_OUTPUT_MODE          0x00000C00
#define QUP_IO_MODES_INPUT_FIFO_SIZE      0x00000380
#define QUP_IO_MODES_INPUT_BLOCK_SIZE     0x00000060
#define QUP_IO_MODES_OUTPUT_FIFO_SIZE     0x0000001C
#define QUP_IO_MODES_OUTPUT_BLOCK_SIZE    0x00000003

#define INPUT_BLOCK_SZ_SHIFT		5
#define INPUT_FIFO_SZ_SHIFT			7
#define OUTPUT_BLOCK_SZ_SHIFT		0
#define OUTPUT_FIFO_SZ_SHIFT		2
#define OUTPUT_MODE_SHIFT			10
#define INPUT_MODE_SHIFT			12
#define INPUT_MODE_MASK		(3 << INPUT_MODE_SHIFT)
#define OUTPUT_MODE_MASK	(3 << OUTPUT_MODE_SHIFT)

/* QUP_STATE fields */
#define QUP_STATE_CLEAR_BITS		0x2

/* QUP_ERROR_FLAGS fields */
#define QUP_ERROR_OUTPUT_OVER_RUN	BIT(5)
#define QUP_ERROR_INPUT_UNDER_RUN	BIT(4)
#define QUP_ERROR_OUTPUT_UNDER_RUN	BIT(3)
#define QUP_ERROR_INPUT_OVER_RUN	BIT(2)

/* QUP_OPERATIONAL fields */
#define QUP_OP_MAX_INPUT_DONE_FLAG	BIT(11)
#define QUP_OP_MAX_OUTPUT_DONE_FLAG	BIT(10)
#define QUP_OP_IN_SERVICE_FLAG		BIT(9)
#define QUP_OP_OUT_SERVICE_FLAG		BIT(8)
#define QUP_OP_IN_FIFO_FULL			BIT(7)
#define QUP_OP_OUT_FIFO_FULL		BIT(6)
#define QUP_OP_IN_FIFO_NOT_EMPTY	BIT(5)
#define QUP_OP_OUT_FIFO_NOT_EMPTY	BIT(4)

/* QUP_IO_MODES fields */
#define QUP_IO_MODES_FIFO		0
#define QUP_IO_MODES_BLOCK		1
#define QUP_IO_MODES_DMOV		2
#define QUP_IO_MODES_BAM		3

/* QUP_CONFIG fields */
#define QUP_CONFIG_SPI_MODE			(1 << 8)
#define QUP_CONFIG_CLOCK_AUTO_GATE	BIT(13)
#define QUP_CONFIG_NO_INPUT			BIT(7)
#define QUP_CONFIG_NO_OUTPUT		BIT(6)
#define QUP_CONFIG_N				0x001f

/* QUP_MX_OUTPUT_CNT only supports
 * 0:15 bits as Number of writes of
 * size N to the mini-core per RUN state.
 * And make the count be multiple of max bytes per word.
 */
#define MAX_QUP_MX_OUTPUT_COUNT 0xFFF8
#define MAX_QUP_MX_TRANSFER_COUNT 0xFFF8

/* QUP Registers */
enum {
	QUP_CONFIG = 0x0,
	QUP_STATE = 0x4,
	QUP_IO_MODES = 0x8,
	QUP_SW_RESET = 0xC,
	QUP_OPERATIONAL = 0x18,
	QUP_ERROR_FLAGS = 0x1C,
	QUP_ERROR_FLAGS_EN = 0x20,
	QUP_TEST_CTRL = 0x24,
	QUP_OPERATIONAL_MASK = 0x28,
	QUP_HW_VERSION = 0x30,
	QUP_MX_READ_CNT = 0x208,
	QUP_MX_INPUT_CNT = 0x200,
	QUP_MX_INPUT_CNT_CURRENT = 0x204,
	QUP_MX_OUTPUT_CNT = 0x100,
	QUP_MX_OUTPUT_CNT_CURRENT = 0x104,
	QUP_OUTPUT_DEBUG = 0x108,
	QUP_OUTPUT_FIFO_WORD_CNT = 0x10C,
	QUP_OUTPUT_FIFO_BASE = 0x110,
	QUP_MX_WRITE_CNT = 0x150,
	QUP_MX_WRITE_CNT_CURRENT = 0x154,
	QUP_INPUT_READ_CUR = 0x20C,
	QUP_INPUT_DEBUG = 0x210,
	QUP_INPUT_FIFO_CNT = 0x214,
	QUP_INPUT_FIFO_BASE = 0x218,
	QUP_I2C_CLK_CTL = 0x400,
	QUP_I2C_STATUS = 0x404,
};

/* QUP States and reset values */
enum qup_state{
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

#endif				/* __QUP__ */
