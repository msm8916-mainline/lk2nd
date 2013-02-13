/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
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

// GPIO configurations

#define	SSBI_REG_ADDR_GPIO_BASE		0x150

#define QT_PMIC_GPIO_KYPD_SNS           0x008
#define QT_PMIC_GPIO_KYPD_DRV           0x003

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

struct pm8058_gpio {
	int		direction;
	int		pull;
	int		vin_sel;	/* 0..7 */
	int		out_strength;
	int		function;
	int		inv_int_pol;	/* invert interrupt polarity */
};

bool pm8058_gpio_get(unsigned int gpio);

typedef int (*read_func)(unsigned char *, unsigned short, unsigned short);
typedef int (*write_func)(unsigned char *, unsigned short, unsigned short);
typedef int (*gpio_get_func)(uint8_t, uint8_t *);

struct qwerty_keypad_info {
	unsigned int   *keymap;
	unsigned int   *gpiomap;
	unsigned int   mapsize;
	unsigned char  *old_keys;
	unsigned char  *rec_keys;
	unsigned int   rows;
	unsigned int   columns;
	unsigned int   num_of_reads;
	read_func      rd_func;
	write_func     wr_func;
	gpio_get_func  key_gpio_get;

	/* time to wait before reading inputs after driving each output */
	time_t         settle_time;
	time_t         poll_time;
	unsigned       flags;
};

void ssbi_keypad_init (struct qwerty_keypad_info *);

#endif /* __DEV_GPIO_KEYPAD_H */
