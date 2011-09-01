/*
 * Copyright (c) 2008, Google Inc.
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
#ifndef __DEV_SSBI_H
#define __DEV_SSBI_H

//Macros for SSBI Qwerty keypad for 7x30

/* SSBI 2.0 controller registers */
#define MSM_SSBI_BASE                   0xAD900000

#define SSBI_TIMEOUT_US			100

#define SSBI2_CTL			0x0000
#define SSBI2_RESET			0x0004
#define SSBI2_CMD			0x0008
#define SSBI2_RD			0x0010
#define SSBI2_STATUS			0x0014
#define SSBI2_PRIORITIES		0x0018
#define SSBI2_MODE2			0x001C

/* SSBI_CMD fields */
#define SSBI_CMD_SEND_TERM_SYM		(0x01 << 27)
#define SSBI_CMD_WAKEUP_SLAVE		(0x01 << 26)
#define SSBI_CMD_USE_ENABLE		(0x01 << 25)
#define SSBI_CMD_RDWRN			(0x01 << 24)
#define SSBI_CMD_REG_ADDR_SHFT		(0x10)
#define SSBI_CMD_REG_ADDR_MASK		(0xFF << SSBI_CMD_REG_ADDR_SHFT)
#define SSBI_CMD_REG_DATA_SHFT		(0x00)
#define SSBI_CMD_REG_DATA_MASK		(0xFF << SSBI_CMD_REG_DATA_SHFT)

/* SSBI_STATUS fields */
#define SSBI_STATUS_DATA_IN		0x10
#define SSBI_STATUS_RD_CLOBBERED	0x08
#define SSBI_STATUS_RD_READY		0x04
#define SSBI_STATUS_READY		0x02
#define SSBI_STATUS_MCHN_BUSY		0x01

/* SSBI_RD fields */
#define SSBI_RD_USE_ENABLE		0x02000000
#define SSBI_RD_RDWRN			0x01000000
#define SSBI_RD_REG_ADDR_SHFT		0x10
#define SSBI_RD_REG_ADDR_MASK		(0xFF << SSBI_RD_REG_ADDR_SHFT)
#define SSBI_RD_REG_DATA_SHFT		(0x00)
#define SSBI_RD_REG_DATA_MASK		(0xFF << SSBI_RD_REG_DATA_SHFT)

/* SSBI_MODE2 fields */
#define SSBI_MODE2_REG_ADDR_15_8_SHFT	0x04
#define SSBI_MODE2_REG_ADDR_15_8_MASK	(0x7F << SSBI_MODE2_REG_ADDR_15_8_SHFT)
#define SSBI_MODE2_ADDR_WIDTH_SHFT	0x01
#define SSBI_MODE2_ADDR_WIDTH_MASK	(0x07 << SSBI_MODE2_ADDR_WIDTH_SHFT)
#define SSBI_MODE2_SSBI2_MODE		0x00000001

//Keypad controller configurations
#define SSBI_REG_KYPD_CNTL_ADDR         0x148
#define SSBI_REG_KYPD_SCAN_ADDR         0x149
#define SSBI_REG_KYPD_TEST_ADDR         0x14A
#define SSBI_REG_KYPD_REC_DATA_ADDR     0x14B
#define SSBI_REG_KYPD_OLD_DATA_ADDR     0x14C


/* PMIC Arbiter 1: SSBI2 Configuration Micro ARM registers */
#define PA1_SSBI2_CMD                   0x00500000
#define PA1_SSBI2_RD_STATUS             0x00500004

#define PA1_SSBI2_REG_ADDR_SHIFT        8
#define PA1_SSBI2_CMD_RDWRN_SHIFT       24
#define PA1_SSBI2_TRANS_DONE_SHIFT      27

#define PA1_SSBI2_REG_DATA_MASK         0xFF
#define PA1_SSBI2_REG_DATA_SHIFT        0

#define PA1_SSBI2_CMD_READ              1
#define PA1_SSBI2_CMD_WRITE             0

/* PMIC Arbiter 2: SSBI2 Configuration Micro ARM registers */
#define PA2_SSBI2_CMD                   0x00C00000
#define PA2_SSBI2_RD_STATUS             0x00C00004

#define PA2_SSBI2_REG_ADDR_SHIFT        8
#define PA2_SSBI2_CMD_RDWRN_SHIFT       24
#define PA2_SSBI2_TRANS_DONE_SHIFT      27

#define PA2_SSBI2_REG_DATA_MASK         0xFF
#define PA2_SSBI2_REG_DATA_SHIFT        0

#define PA2_SSBI2_CMD_READ              1
#define PA2_SSBI2_CMD_WRITE             0

#define SSBI_CMD_READ(AD) \
	(SSBI_CMD_RDWRN | (((AD) & 0xFF) << SSBI_CMD_REG_ADDR_SHFT))

#define SSBI_CMD_WRITE(AD, DT) \
  ((((AD) & 0xFF) << SSBI_CMD_REG_ADDR_SHFT) | \
   (((DT) & 0xFF) << SSBI_CMD_REG_DATA_SHFT))

#define SSBI_MODE2_REG_ADDR_15_8(MD, AD) \
	(((MD) & 0x0F) | ((((AD) >> 8) << SSBI_MODE2_REG_ADDR_15_8_SHFT) & \
	SSBI_MODE2_REG_ADDR_15_8_MASK))

int i2c_ssbi_read_bytes(unsigned char  *buffer, unsigned short length,
			unsigned short slave_addr);
int i2c_ssbi_write_bytes(unsigned char  *buffer, unsigned short length,
			 unsigned short slave_addr);
int pa1_ssbi2_read_bytes(unsigned char  *buffer, unsigned short length,
			 unsigned short slave_addr);
int pa1_ssbi2_write_bytes(unsigned char  *buffer, unsigned short length,
			  unsigned short slave_addr);

#endif
