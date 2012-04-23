/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __STDIO_H
#define __STDIO_H

#include <debug.h>
#include <printf.h>

typedef struct {
	char *fpos; /* Current position of file pointer (absolute address) */
	void *base; /* Pointer to the base of the file */
	unsigned short handle; /* File handle */
	short flags; /* Flags (see FileFlags) */
	short unget; /* 1-byte buffer for ungetc (b15=1 if non-empty) */
	unsigned long alloc; /* Number of currently allocated bytes for the file */
	unsigned short buffincrement; /* Number of bytes allocated at once */
} FILE;

void putc(char c);
int puts(const char *str);
int getc(char *c); // XXX not really getc
size_t fwrite(const void *buf, size_t size, size_t count, FILE *stream);
int sscanf(const char *str, const char *format, ...);

#endif

