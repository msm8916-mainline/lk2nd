// SPDX-License-Identifier: BSD-3-Clause
#include <lk2nd/util/cmdline.h>

bool lk2nd_cmdline_scan(const char *cmdline, const char *arg)
{
	const char *argp;

	while (true) {
		/* Skip spaces */
		for (; *cmdline == ' '; cmdline++);

		/* Compare argument */
		for (argp = arg; *argp && *argp == *cmdline; argp++, cmdline++);
		if (*argp == '\0' && (*cmdline == '\0' || *cmdline == ' '))
			return true;

		/* Skip non-spaces (the rest of the "wrong argument") */
		for (; *cmdline != ' '; cmdline++)
			if (*cmdline == '\0')
				return false;
	}
}
