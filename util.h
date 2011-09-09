#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>

/*
 * Copyright 2008 Jon Loeliger, Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *                                                                   USA
 */

static inline void __attribute__((noreturn)) die(char * str, ...)
{
	va_list ap;

	va_start(ap, str);
	fprintf(stderr, "FATAL ERROR: ");
	vfprintf(stderr, str, ap);
	exit(1);
}

static inline void *xmalloc(size_t len)
{
	void *new = malloc(len);

	if (!new)
		die("malloc() failed\n");

	return new;
}

static inline void *xrealloc(void *p, size_t len)
{
	void *new = realloc(p, len);

	if (!new)
		die("realloc() failed (len=%d)\n", len);

	return new;
}

extern char *xstrdup(const char *s);
extern char *join_path(const char *path, const char *name);

/**
 * Check a string of a given length to see if it is all printable and
 * has a valid terminator.
 *
 * @param data	The string to check
 * @param len	The string length including terminator
 * @return 1 if a valid printable string, 0 if not */
int util_is_printable_string(const void *data, int len);

/*
 * Parse an escaped character starting at index i in string s.  The resulting
 * character will be returned and the index i will be updated to point at the
 * character directly after the end of the encoding, this may be the '\0'
 * terminator of the string.
 */
char get_escape_char(const char *s, int *i);

#endif /* _UTIL_H */
