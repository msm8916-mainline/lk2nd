#ifndef __STDIO_H
#define __STDIO_H

#include <debug.h>
#include <printf.h>

void putc(char c);
int putchar(int c);
int puts(const char *str);
int getc(char *c); // XXX not really getc

#endif

