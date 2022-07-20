/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_KEYS_H
#define LK2ND_KEYS_H

#include <stdint.h>
#include <dev/keys.h>

bool lk2nd_keys_pressed(uint32_t keycode);

#endif /* LK2ND_KEYS_H */
