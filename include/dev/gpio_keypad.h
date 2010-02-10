/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

#ifndef __DEV_GPIO_KEYPAD_H
#define __DEV_GPIO_KEYPAD_H

#include <sys/types.h>

/* unset: drive active output low, set: drive active output high */
#define GPIOKPF_ACTIVE_HIGH		(1U << 0)
#define GPIOKPF_DRIVE_INACTIVE		(1U << 1)

struct gpio_keypad_info {
	/* size must be ninputs * noutputs */
	const uint16_t *keymap;
	unsigned *input_gpios;
	unsigned *output_gpios;
	int ninputs;
	int noutputs;
	/* time to wait before reading inputs after driving each output */
	time_t settle_time;
	time_t poll_time;
	unsigned flags;
};

void gpio_keypad_init(struct gpio_keypad_info *kpinfo);

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

// GPIO configurations

#define	SSBI_REG_ADDR_GPIO_BASE		0x150
#define SSBI_OFFSET_ADDR_GPIO_KYPD_SNS  0x000
#define SSBI_OFFSET_ADDR_GPIO_KYPD_DRV  0x008
#define	SSBI_REG_ADDR_GPIO(n)		(SSBI_REG_ADDR_GPIO_BASE + n)

#define	PM_GPIO_DIR_OUT			0x01
#define	PM_GPIO_DIR_IN			0x02
#define	PM_GPIO_DIR_BOTH		(PM_GPIO_DIR_OUT | PM_GPIO_DIR_IN)

#define	PM_GPIO_PULL_UP1		2
#define	PM_GPIO_PULL_UP2		3
#define	PM_GPIO_PULL_DN			4
#define	PM_GPIO_PULL_NO			5

#define	PM_GPIO_STRENGTH_NO		0
#define	PM_GPIO_STRENGTH_HIGH		1
#define	PM_GPIO_STRENGTH_MED		2
#define	PM_GPIO_STRENGTH_LOW		3

#define	PM_GPIO_FUNC_NORMAL		0
#define	PM_GPIO_FUNC_PAIRED		1
#define	PM_GPIO_FUNC_1			2
#define	PM_GPIO_FUNC_2			3

#define	PM8058_GPIO_BANK_MASK		0x70
#define	PM8058_GPIO_BANK_SHIFT		4
#define	PM8058_GPIO_WRITE		0x80

/* Bank 0 */
#define	PM8058_GPIO_VIN_MASK		0x0E
#define	PM8058_GPIO_VIN_SHIFT		1
#define	PM8058_GPIO_MODE_ENABLE		0x01

/* Bank 1 */
#define	PM8058_GPIO_MODE_MASK		0x0C
#define	PM8058_GPIO_MODE_SHIFT		2
#define	PM8058_GPIO_OUT_BUFFER		0x02
#define	PM8058_GPIO_OUT_INVERT		0x01

#define	PM8058_GPIO_MODE_OFF		3
#define	PM8058_GPIO_MODE_OUTPUT		2
#define	PM8058_GPIO_MODE_INPUT		0
#define	PM8058_GPIO_MODE_BOTH		1

/* Bank 2 */
#define	PM8058_GPIO_PULL_MASK		0x0E
#define	PM8058_GPIO_PULL_SHIFT		1

/* Bank 3 */
#define	PM8058_GPIO_OUT_STRENGTH_MASK   0x0C
#define	PM8058_GPIO_OUT_STRENGTH_SHIFT  2

/* Bank 4 */
#define	PM8058_GPIO_FUNC_MASK		0x0E
#define	PM8058_GPIO_FUNC_SHIFT		1

#define NUM_OF_SSBI_READS               6
#define NUM_OF_KYPD_DRV_GPIOS           5
#define NUM_OF_KYPD_SNS_GPIOS           5

struct pm8058_gpio {
	int		direction;
	int		pull;
	int		vin_sel;	/* 0..7 */
	int		out_strength;
	int		function;
	int		inv_int_pol;	/* invert interrupt polarity */
};

struct qwerty_keypad_info {
	/* size must be ninputs * noutputs */
	unsigned int   *keymap;
        unsigned char  *old_keys;
        unsigned char  *rec_keys;
        unsigned int   rows;
        unsigned int   columns;
	/* time to wait before reading inputs after driving each output */
	time_t         settle_time;
	time_t         poll_time;
	unsigned       flags;
};

#define SSBI_CMD_READ(AD) \
	(SSBI_CMD_RDWRN | (((AD) & 0xFF) << SSBI_CMD_REG_ADDR_SHFT))

#define SSBI_CMD_WRITE(AD, DT) \
  ((((AD) & 0xFF) << SSBI_CMD_REG_ADDR_SHFT) | \
   (((DT) & 0xFF) << SSBI_CMD_REG_DATA_SHFT))

#define SSBI_MODE2_REG_ADDR_15_8(MD, AD) \
	(((MD) & 0x0F) | ((((AD) >> 8) << SSBI_MODE2_REG_ADDR_15_8_SHFT) & \
	SSBI_MODE2_REG_ADDR_15_8_MASK))

void ssbi_keypad_init (struct qwerty_keypad_info *);

#endif /* __DEV_GPIO_KEYPAD_H */
