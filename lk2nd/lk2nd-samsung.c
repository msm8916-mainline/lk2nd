// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <libfdt.h>
#include <platform/timer.h>
#include "regmap/regmap-gpio-i2c.h"

/*
 * On some Samsung devices, the bootloader sets the MUIC in a so-called
 * "rustproof" mode, stopping it from detecting attached devices (USB, UART...)
 * automatically without software interaction. This breaks USB and UART in lk2nd.
 *
 * This is necessary on devices that have CONFIG_MUIC_SUPPORT_RUSTPROOF set in
 * their (downstream) kernel config. The intention from Samsung might be to
 * complicate UART access, but overall the option is pretty stupid.
 *
 * The workaround is to reset the MUIC device via an I2C register.
 */

/* TODO: Make this configurable? */
#define MUIC_RESET_REG	0x1B

void lk2nd_samsung_muic_reset(const void *fdt, int offset)
{
	gpio_i2c_info_t gpio_i2c = {
		GPIO_I2C_CLOCKS
	};
	struct regmap_gpio_i2c regmap = {
		.map = REGMAP_GPIO_I2C("muic"),
	};

	/* Check if MUIC reset is necessary */
	offset = fdt_subnode_offset(fdt, offset, "samsung,muic-reset");
	if (offset < 0)
		return;

	if (!gpio_i2c_read_pins(fdt, offset, &gpio_i2c))
		return;
	if (!regmap_gpio_i2c_read_addr(fdt, offset, &regmap))
		return;

	gpio_i2c_add_bus(0, &gpio_i2c);
	if (regmap_write(&regmap.map, MUIC_RESET_REG, 1))
		return;

	/* Wait a bit to let the MUIC detect the cable again */
	mdelay(250);
	dprintf(INFO, "muic-reset: Successful, earlier UART log may be lost!\n");
}
