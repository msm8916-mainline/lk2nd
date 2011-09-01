/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include <dev/keys.h>
#include <dev/ssbi.h>
#include <dev/gpio_keypad.h>

#define NUM_OF_ROWS 1
#define NUM_OF_COLS 5

#define BITS_IN_ELEMENT(x) (sizeof(x)[0] * 8)

static unsigned char qwerty_keys_old[NUM_OF_ROWS];
static unsigned char qwerty_keys_new[NUM_OF_ROWS];

#define KEYMAP_INDEX(row, col) (row)* BITS_IN_ELEMENT(qwerty_keys_new) + (col)

unsigned int qwerty_keymap[] = {
    [KEYMAP_INDEX(0, 0)] = KEY_VOLUMEUP,
    [KEYMAP_INDEX(0, 1)] = KEY_VOLUMEDOWN,
};


struct qwerty_keypad_info qwerty_keypad = {
	.keymap         = qwerty_keymap,
	.old_keys       = qwerty_keys_old,
	.rec_keys       = qwerty_keys_new,
	.rows           = NUM_OF_ROWS,
	.columns        = NUM_OF_COLS,
	.num_of_reads   = NUM_OF_ROWS,
	.rd_func        = &pa1_ssbi2_read_bytes,
	.wr_func        = &pa1_ssbi2_write_bytes,
	.settle_time    = 5  /* msec */,
	.poll_time      = 20 /* msec */,
};

void keypad_init(void)
{
	memset(qwerty_keys_old, 0, sizeof(qwerty_keys_old));
	memset(qwerty_keys_new, 0, sizeof(qwerty_keys_new));

	ssbi_keypad_init(&qwerty_keypad);
}
