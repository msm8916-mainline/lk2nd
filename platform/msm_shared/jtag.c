/*
 * Copyright (c) 2008-2014, The Linux Foundation. All rights reserved.
 * Not a contribution
 *
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jtag.h>

#define STATUS_NOMSG 0
#define STATUS_OKAY  1
#define STATUS_FAIL  2
#define STATUS_PRINT 3
#define JTAG_CMD_NUM  256

volatile unsigned _jtag_cmd = 0;
volatile unsigned _jtag_msg = 0;
unsigned char _jtag_cmd_buffer[JTAG_CMD_NUM];
unsigned char _jtag_msg_buffer[128];

volatile unsigned _jtag_arg0 = 0;
volatile unsigned _jtag_arg1 = 0;
volatile unsigned _jtag_arg2 = 0;

void jtag_hook(void);

static void jtag_msg(unsigned status, const char *msg)
{
	unsigned char *out = _jtag_msg_buffer;
	while ((*out++ = *msg++) != 0) ;
	_jtag_msg = status;
	do {
		jtag_hook();
	}
	while (_jtag_msg != 0);
}

void jtag_okay(const char *msg)
{
	if (msg == 0)
		msg = "OKAY";
	jtag_msg(STATUS_OKAY, msg);
}

void jtag_fail(const char *msg)
{
	if (msg == 0)
		msg = "FAIL";
	jtag_msg(STATUS_FAIL, msg);
}

int jtag_cmd_pending()
{
	jtag_hook();
	return (int)_jtag_cmd;
}

void jtag_cmd_loop(void (*do_cmd) (const char *, unsigned, unsigned, unsigned))
{
	unsigned n;
	for (;;) {
		if (jtag_cmd_pending()) {
			do_cmd((const char *)_jtag_cmd_buffer, _jtag_arg0,
			       _jtag_arg1, _jtag_arg2);
			for (n = 0; n < JTAG_CMD_NUM; n++)
				_jtag_cmd_buffer[n] = 0;
			_jtag_arg0 = 0;
			_jtag_arg1 = 0;
			_jtag_arg2 = 0;
			_jtag_cmd = 0;
		}
	}
}

static char jtag_putc_buffer[128];
static unsigned jtag_putc_count = 0;

static void jtag_push_buffer(void)
{
	jtag_putc_buffer[jtag_putc_count] = 0;
	jtag_putc_count = 0;
	jtag_msg(STATUS_PRINT, jtag_putc_buffer);
}

void jtag_dputc(unsigned c)
{
	if ((c < 32) || (c > 127)) {
		if (c == '\n') {
			jtag_push_buffer();
		}
		return;
	}

	jtag_putc_buffer[jtag_putc_count++] = c;
	if (jtag_putc_count == 127) {
		jtag_push_buffer();
	}
}
