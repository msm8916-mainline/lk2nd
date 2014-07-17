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

