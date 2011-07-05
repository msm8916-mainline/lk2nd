/*
 * Copyright 2008 Jon Loeliger, Freescale Semiconductor, Inc.
 *
 * util_is_printable_string contributed by
 *	Pantelis Antoniou <pantelis.antoniou AT gmail.com>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"

char *xstrdup(const char *s)
{
	int len = strlen(s) + 1;
	char *dup = xmalloc(len);

	memcpy(dup, s, len);

	return dup;
}

char *join_path(const char *path, const char *name)
{
	int lenp = strlen(path);
	int lenn = strlen(name);
	int len;
	int needslash = 1;
	char *str;

	len = lenp + lenn + 2;
	if ((lenp > 0) && (path[lenp-1] == '/')) {
		needslash = 0;
		len--;
	}

	str = xmalloc(len);
	memcpy(str, path, lenp);
	if (needslash) {
		str[lenp] = '/';
		lenp++;
	}
	memcpy(str+lenp, name, lenn+1);
	return str;
}

int util_is_printable_string(const void *data, int len)
{
	const char *s = data;
	const char *ss;

	/* zero length is not */
	if (len == 0)
		return 0;

	/* must terminate with zero */
	if (s[len - 1] != '\0')
		return 0;

	ss = s;
	while (*s && isprint(*s))
		s++;

	/* not zero, or not done yet */
	if (*s != '\0' || (s + 1 - ss) < len)
		return 0;

	return 1;
}
