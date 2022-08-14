// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <lk2nd/init.h>

void lk2nd_init(void)
{
	extern void (*__lk2nd_init_start)(void);
	extern void (*__lk2nd_init_end)(void);
	void (**func)(void);

	dprintf(INFO, "lk2nd_init()\n");
	for (func = &__lk2nd_init_start; func < &__lk2nd_init_end; ++func)
		(*func)();
}
