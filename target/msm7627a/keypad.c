/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *	may be used to endorse or promote products derived from this
 *	software without specific prior written permission.
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

/* don't turn this on without updating the ffa support */
#define SCAN_FUNCTION_KEYS 0

static unsigned int halibut_row_gpios[] = { 31, 32, 33, 34, 35 };
static unsigned int halibut_col_gpios[] = { 36, 37, 38, 39, 40 };

static unsigned int halibut_row_gpios_qrd[] = { 31, 32 };
static unsigned int halibut_col_gpios_qrd[] = { 36, 37 };

static unsigned int halibut_row_gpios_evb[] = { 31 };
static unsigned int halibut_col_gpios_evb[] = { 36, 37 };

#define KEYMAP_INDEX(row, col) ((row)*ARRAY_SIZE(halibut_col_gpios) + (col))
#define KEYMAP_INDEX_QRD(row, col) ((row)*ARRAY_SIZE(halibut_col_gpios_qrd) + (col))
#define KEYMAP_INDEX_EVB(row, col) ((row)*ARRAY_SIZE(halibut_col_gpios_evb) + (col))

static const unsigned short halibut_keymap[ARRAY_SIZE(halibut_col_gpios) *
					   ARRAY_SIZE(halibut_row_gpios)] = {
	[KEYMAP_INDEX(0, 0)] = KEY_7,
	[KEYMAP_INDEX(0, 1)] = KEY_DOWN,
	[KEYMAP_INDEX(0, 2)] = KEY_UP,
	[KEYMAP_INDEX(0, 3)] = KEY_RIGHT,
	[KEYMAP_INDEX(0, 4)] = KEY_CENTER,

	[KEYMAP_INDEX(1, 0)] = KEY_LEFT,
	[KEYMAP_INDEX(1, 1)] = KEY_SEND,
	[KEYMAP_INDEX(1, 2)] = KEY_1,
	[KEYMAP_INDEX(1, 3)] = KEY_4,
	[KEYMAP_INDEX(1, 4)] = KEY_CLEAR,

	[KEYMAP_INDEX(2, 0)] = KEY_6,
	[KEYMAP_INDEX(2, 1)] = KEY_5,
	[KEYMAP_INDEX(2, 2)] = KEY_8,
	[KEYMAP_INDEX(2, 3)] = KEY_3,
	[KEYMAP_INDEX(2, 4)] = KEY_STAR,

	[KEYMAP_INDEX(3, 0)] = KEY_9,
	[KEYMAP_INDEX(3, 1)] = KEY_SOFT1,
	[KEYMAP_INDEX(3, 2)] = KEY_0,
	[KEYMAP_INDEX(3, 3)] = KEY_2,
	[KEYMAP_INDEX(3, 4)] = KEY_SOFT2,

	[KEYMAP_INDEX(4, 0)] = KEY_MENU,	/*R- */
	[KEYMAP_INDEX(4, 1)] = KEY_HOME,	/*L+ */
	[KEYMAP_INDEX(4, 2)] = KEY_BACK,	/*L- */
	[KEYMAP_INDEX(4, 3)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX(4, 4)] = KEY_VOLUMEDOWN,
};

static const unsigned short halibut_keymap_qrd[ARRAY_SIZE(halibut_col_gpios_qrd)
					       *
					       ARRAY_SIZE
					       (halibut_row_gpios_qrd)] = {
	[KEYMAP_INDEX_QRD(0, 0)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX_QRD(0, 1)] = KEY_VOLUMEDOWN,
};

static const unsigned short halibut_keymap_evb[ARRAY_SIZE(halibut_col_gpios_evb)
					       *
					       ARRAY_SIZE
					       (halibut_row_gpios_evb)] = {
	[KEYMAP_INDEX_EVB(0, 0)] = KEY_VOLUMEUP,
	[KEYMAP_INDEX_EVB(0, 1)] = KEY_VOLUMEDOWN,
};

static struct gpio_keypad_info halibut_keypad_info_surf = {
	.keymap = halibut_keymap,
	.output_gpios = halibut_row_gpios,
	.input_gpios = halibut_col_gpios,
	.noutputs = ARRAY_SIZE(halibut_row_gpios),
	.ninputs = ARRAY_SIZE(halibut_col_gpios),
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
	.flags = GPIOKPF_DRIVE_INACTIVE,
};

static struct gpio_keypad_info halibut_keypad_info_qrd = {
	.keymap = halibut_keymap_qrd,
	.output_gpios = halibut_row_gpios_qrd,
	.input_gpios = halibut_col_gpios_qrd,
	.noutputs = ARRAY_SIZE(halibut_row_gpios_qrd),
	.ninputs = ARRAY_SIZE(halibut_col_gpios_qrd),
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
	.flags = GPIOKPF_DRIVE_INACTIVE,
};

static struct gpio_keypad_info halibut_keypad_info_evb = {
	.keymap = halibut_keymap_evb,
	.output_gpios = halibut_row_gpios_evb,
	.input_gpios = halibut_col_gpios_evb,
	.noutputs = ARRAY_SIZE(halibut_row_gpios_evb),
	.ninputs = ARRAY_SIZE(halibut_col_gpios_evb),
	.settle_time = 5 /* msec */ ,
	.poll_time = 20 /* msec */ ,
	.flags = GPIOKPF_DRIVE_INACTIVE,
};

void keypad_init(void)
{
	if (machine_is_qrd())
		gpio_keypad_init(&halibut_keypad_info_qrd);
	else if (machine_is_evb())
		gpio_keypad_init(&halibut_keypad_info_evb);
	else
		gpio_keypad_init(&halibut_keypad_info_surf);
}
