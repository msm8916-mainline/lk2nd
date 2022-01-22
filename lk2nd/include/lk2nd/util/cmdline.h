/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_CMDLINE_H
#define LK2ND_UTIL_CMDLINE_H

#include <compiler.h>
#include <stdbool.h>

/**
 * lk2nd_cmdline_scan() - Scan for a space-separated parameter in command line.
 * @cmdline: The command line (space-separated parameters)
 * @arg: The parameter to search for
 *
 * Return: true if @arg was found in @cmdline, false otherwise
 */
bool lk2nd_cmdline_scan(const char *cmdline, const char *arg) __PURE;

#endif /* LK2ND_UTIL_CMDLINE_H */
