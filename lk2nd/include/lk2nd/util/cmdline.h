/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_CMDLINE_H
#define LK2ND_UTIL_CMDLINE_H

#include <compiler.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * lk2nd_cmdline_scan_arg() - Get cmdline argument value.
 * @cmdline: The command line (space-separated parameters)
 * @arg:     The parameter to search for
 * @value:   Char array to write value of the argument.
 * @vlen:    Max size of @value.
 *
 * Return: true if @arg was found in @cmdline, false otherwise.
 * If the value was found but has no parameter ('=' sign and
 * trailing value), then @value is unchanged. Otherwise @value
 * is populated with the argument after '='.
 */
bool lk2nd_cmdline_scan_arg(const char *cmdline, const char *arg, char *value, size_t vlen);

/**
 * lk2nd_cmdline_scan() - Scan for a space-separated parameter in command line.
 * @cmdline: The command line (space-separated parameters)
 * @arg: The parameter to search for
 *
 * Return: true if @arg was found in @cmdline, false otherwise
 */
bool lk2nd_cmdline_scan(const char *cmdline, const char *arg) __PURE;

#endif /* LK2ND_UTIL_CMDLINE_H */
