// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <libfdt.h>
#include <dev/gpio_i2c.h>
#include <platform/timer.h>

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

static void i2c_gpio_muic_reset(int sda, int scl, uint8_t i2c_addr)
{
	uint8_t val = 1;
	status_t status;
	const gpio_i2c_info_t i2c_muic = {
		.sda = sda,
		.scl = scl,
		.hcd = 10 /* us */,
		.qcd =  5 /* us */,
	};

	gpio_i2c_add_bus(0, &i2c_muic);
	status = gpio_i2c_write_reg_bytes(0, i2c_addr, MUIC_RESET_REG, &val, 1);
	if (status) {
		dprintf(CRITICAL, "muic-reset: I2C write error: %d\n", status);
		return;
	}

	/* Wait a bit to let the MUIC detect the cable again */
	mdelay(250);
	dprintf(INFO, "muic-reset: Successful, earlier UART log may be lost!\n");
}

void lk2nd_samsung_muic_reset(const void *fdt, int offset)
{
	const uint32_t *i2c_gpios;
	const uint32_t *i2c_addr;
	int len;

	/* Check if MUIC reset is necessary */
	offset = fdt_subnode_offset(fdt, offset, "samsung,muic-reset");
	if (offset < 0)
		return;

	i2c_gpios = fdt_getprop(fdt, offset, "i2c-gpio-pins", &len);
	if (len != 2 * sizeof(*i2c_gpios)) {
		dprintf(CRITICAL, "muic-reset: Invalid 'i2c-gpios' (len %d\n)", len);
		return;
	}
	i2c_addr = fdt_getprop(fdt, offset, "i2c-address", &len);
	if (len != sizeof(*i2c_addr)) {
		dprintf(CRITICAL, "muic-reset: Invalid 'i2c-address' (len %d\n)", len);
		return;
	}

	i2c_gpio_muic_reset(fdt32_to_cpu(i2c_gpios[0]), fdt32_to_cpu(i2c_gpios[1]),
			    fdt32_to_cpu(*i2c_addr));
}
