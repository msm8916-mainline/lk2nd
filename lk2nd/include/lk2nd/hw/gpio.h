/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_HW_GPIO_H
#define LK2ND_HW_GPIO_H

#include <bits.h>
#include <stdbool.h>
#include <stdint.h>

#include <dt-bindings/lk2nd/gpio.h>

/**
 * struct gpiol_desc - GPIO pin descriptor.
 * @pin:	The pin number on the controller.
 * @dev:	The device number. See GPIOL_DEVICE_* defines in dt-bindings.
 * @active_low:	Asserted state of this pin is low.
 */
struct gpiol_desc {
	uint32_t pin : 16,
		 dev : 8,
		 active_low : 1;
};

/* Initial state flags. */
#define GPIOL_FLAGS_IN			BIT(16)
#define GPIOL_FLAGS_OUT			BIT(17)
#define GPIOL_FLAGS_ASSERTED		BIT(18)
#define GPIOL_FLAGS_OUT_ASSERTED	(GPIOL_FLAGS_OUT | GPIOL_FLAGS_ASSERTED)
#define GPIOL_FLAGS_OUT_DEASSERTED	GPIOL_FLAGS_OUT

/**
 * gpiol_get() - Get gpio number from the DT definition.
 * @dtb:   pointer to the DT.
 * @node:  Offset of the node containing the foo-gpios property.
 * @name:  Name of the gpio in the DT node (i.e. foo for foo-gpios).
 *         name can be NULL for a simple "gpios" property.
 * @desc:  GPIO pin descriptor to be initialized.
 * @flags: Initial GPIO state to be applied.
 *
 * Returns: 0 on success or an error code.
 */
int gpiol_get(const void *dtb, int node, const char *name,
	      struct gpiol_desc *desc, uint32_t flags);

/**
 * gpiol_direction_input() - Configure the gpio as input.
 * @desc: GPIO descriptor.
 *
 * Returns: 0 on success or an error code.
 */
int gpiol_direction_input(struct gpiol_desc desc);

/**
 * gpiol_direction_output() - Configure the gpio as output.
 * @desc:  GPIO descriptor.
 * @value: GPIO state to be set immediately on mode change.
 *
 * Returns: 0 on success or an error code.
 */
int gpiol_direction_output(struct gpiol_desc desc, bool value);

/**
 * gpiol_is_asserted() - Get the value of the gpio.
 * @desc:  GPIO descriptor.
 *
 * Returns: GPIO state on success or not asserted (false) on failure.
 */
bool gpiol_is_asserted(struct gpiol_desc desc);

/**
 * gpiol_set_asserted() - Set the value of the gpio.
 * @desc:  GPIO descriptor.
 * @value: GPIO state to be set.
 */
void gpiol_set_asserted(struct gpiol_desc desc, bool value);

/**
 * gpiol_set_config() - Set the GPIO pin configuration.
 * @desc:   GPIO descriptor.
 * @config: Flags to be set for the pin configuration.
 *
 * Returns: 0 on success or an error code.
 */
int gpiol_set_config(struct gpiol_desc *desc, uint32_t config);

#endif /* LK2ND_HW_GPIO_H */
