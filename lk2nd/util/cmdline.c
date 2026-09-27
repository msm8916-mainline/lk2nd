// SPDX-License-Identifier: BSD-3-Clause
#include <lk2nd/util/cmdline.h>

bool lk2nd_cmdline_scan_arg(const char *cmdline, const char *arg, char *value, size_t vlen)
{
	const char *argp;
	size_t i;

	while (true) {
		/* Skip spaces */
		for (; *cmdline == ' '; cmdline++);

		/* Compare argument */
		for (argp = arg; *argp && *argp == *cmdline; argp++, cmdline++);
		if (*argp == '\0') {
			if (*cmdline == '\0' || *cmdline == ' ') {
				if (value && vlen)
					*value = '\0';
				return true;
			} else if (*cmdline == '=') {
				cmdline++;

				if (value && vlen) {
					i = 0;
					while (i++ < (vlen-1) && !(*cmdline == '\0' || *cmdline == ' '))
						*value++ = *cmdline++;
					*value = '\0';
				}
				return true;
			}
		}

		/* Skip non-spaces (the rest of the "wrong argument") */
		for (; *cmdline != ' '; cmdline++)
			if (*cmdline == '\0')
				return false;
	}
}

bool lk2nd_cmdline_scan(const char *cmdline, const char *arg)
{
	return lk2nd_cmdline_scan_arg(cmdline, arg, NULL, 0);
}
