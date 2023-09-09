/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_MINMAX_H
#define LK2ND_UTIL_MINMAX_H

#define _max(a, b)	((a) > (b) ? (a) : (b))
#define _min(a, b)	((a) < (b) ? (a) : (b))

#define _clamp(val, lo, hi) _min(_max(val, lo), hi)

#define max(a, b) ({ \
	__typeof__(a) _a = (a); \
	__typeof__(b) _b = (b); \
	_max(_a, _b); \
})

#define min(a, b) ({ \
	__typeof__(a) _a = (a); \
	__typeof__(b) _b = (b); \
	_min(_a, _b); \
})

#define clamp(val, lo, hi) ({ \
	__typeof__(val) _val = (val); \
	__typeof__(lo) _lo = (lo); \
	__typeof__(hi) _hi = (hi); \
	_clamp(_val, _lo, _hi); \
})

#endif /* LK2ND_UTIL_MINMAX_H */
