/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_INIT_H
#define LK2ND_INIT_H

void lk2nd_init(void);

#define LK2ND_INIT(func) static void (*_lk2nd_init_##func)(void) \
	__SECTION(".lk2nd_init") __USED = (func)

#endif /* LK2ND_INIT_H */
