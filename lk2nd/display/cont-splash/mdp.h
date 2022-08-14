/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DISPLAY_CONT_SPLASH_MDP_H
#define LK2ND_DISPLAY_CONT_SPLASH_MDP_H

#if MDP3
#include <mdp3.h>
#elif MDP4
#include <dev/lcdc.h>
#include <mdp4.h>
#elif MDP5
#include <mdp5.h>
#else
#error Unknown display controller :(
#endif

#include <platform/iomap.h>

#define MDP_X(val)	BITS_SHIFT(val, 15, 0)
#define MDP_Y(val)	BITS_SHIFT(val, 31, 16)

#endif /* LK2ND_DISPLAY_CONT_SPLASH_MDP_H */
