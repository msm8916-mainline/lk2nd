/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DEVICE_KEYS_H
#define LK2ND_DEVICE_KEYS_H

#include <stdint.h>
#include <dev/keys.h>

/**
 * lk2nd_keys_pressed() - Check if the key is pressed.
 * @keycode: Keycode to check.
 *
 * The method will first check the override keymap and try to find a
 * gpio attached to the keycode. Multiple physical keys can share the
 * same keycode. Every key will be checked to find a pressed one and
 * presence of only one override will cause the default key handlers
 * to be disabled. If there is no override key for the code, the
 * default target-specific key checks will be used for certain keys.
 *
 * Returns: 1 if the key is pressed, 0 if not.
 *
 * If the key is absent, 0 will be returned.
 */
bool lk2nd_keys_pressed(uint32_t keycode);

#endif /* LK2ND_DEVICE_KEYS_H */
