/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
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

#include <jtag.h>

#define STATUS_NOMSG 0
#define STATUS_OKAY  1
#define STATUS_FAIL  2
#define STATUS_PRINT 3

volatile unsigned _jtag_cmd = 0;
volatile unsigned _jtag_msg = 0;
unsigned char _jtag_cmd_buffer[128];
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
			for (n = 0; n < 256; n++)
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
