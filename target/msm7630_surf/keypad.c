/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

#include <dev/keys.h>
#include <dev/ssbi.h>
#include <dev/gpio_keypad.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#define BITS_IN_ELEMENT(x) (sizeof(x)[0] * 8)

static unsigned char qwerty_keys_old[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char qwerty_keys_new[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define KEYMAP_INDEX(row, col) (row)* BITS_IN_ELEMENT(qwerty_keys_new) + (col)

static unsigned int qwerty_keymap[] = {
	[KEYMAP_INDEX(4, 2)] = KEY_BACK,	/* -L on SURF & FFA */
	[KEYMAP_INDEX(3, 4)] = KEY_HOME,	/* +R on SURF & FFA */
	[KEYMAP_INDEX(1, 3)] = KEY_VOLUMEUP,	/* '+' of left side switch on FLUID */
	[KEYMAP_INDEX(1, 4)] = KEY_VOLUMEDOWN,	/* '-' of left side switch on FLUID */
};

static struct qwerty_keypad_info qwerty_keypad = {
	.keymap = qwerty_keymap,
	.old_keys = qwerty_keys_old,
	.rec_keys = qwerty_keys_new,
	.rows = 5,
	.columns = 5,
	.num_of_reads = 6,
	.rd_func = &i2c_ssbi_read_bytes,
	.wr_func = &i2c_ssbi_write_bytes,
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
};

void keypad_init(void)
{
	ssbi_keypad_init(&qwerty_keypad);
}
