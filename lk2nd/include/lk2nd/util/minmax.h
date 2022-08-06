/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_MINMAX_H
#define LK2ND_UTIL_MINMAX_H

#define max(a, b)	((a) > (b) ? (a) : (b))
#define min(a, b)	((a) < (b) ? (a) : (b))

#define clamp(val, lo, hi) min(max(val, lo), hi)

#endif /* LK2ND_UTIL_MINMAX_H */
