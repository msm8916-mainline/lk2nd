// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <platform/timer.h>

#include <libfdt.h>
#include <lk2nd/hw/gpio_i2c.h>

#include "device.h"

/*
 * On some Samsung devices, the bootloader sets the MUIC in a so-called
 * "rustproof" mode, stopping it from detecting attached devices (USB, UART...)
 * automatically without software interaction. This breaks USB and UART in lk2nd.
 *
 * This is necessary on devices that have CONFIG_MUIC_SUPPORT_RUSTPROOF set in
 * their (downstream) kernel config. The intention from Samsung might be to
 * complicate UART access, but overall the option is pretty stupid.
 *
 * A workaround is to reset the MUIC device via an I2C register.
 */

/* TODO: Make this configurable? */
#define MUIC_RESET_REG	0x1B

static int samsung_muic_reset(const void *dtb, int node)
{
	uint8_t addr, val = 1;
	gpio_i2c_info_t i2c;
	status_t status;

	status = gpiol_i2c_get(dtb, node, &i2c, &addr);
	if (status)
		return status;

	status = gpio_i2c_write_reg_bytes(&i2c, addr, MUIC_RESET_REG, &val, 1);
	if (status) {
		dprintf(CRITICAL, "muic-reset: I2C write error: %d\n", status);
		return status;
	}

	/* Wait a bit to let the MUIC detect the cable again */
	mdelay(250);
	dprintf(INFO, "muic-reset: Successful, earlier UART log might be lost!\n");
	return 0;
}
LK2ND_DEVICE_INIT("samsung,muic-reset", samsung_muic_reset);
