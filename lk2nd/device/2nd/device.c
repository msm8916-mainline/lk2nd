// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <fastboot.h>
#include <string.h>

#include "device.h"

static void lk2nd_device2nd_update_serialno(const char *serialno)
{
	extern char sn_buf[13]; /* aboot.c */

	if (serialno)
		strlcpy(sn_buf, serialno, sizeof(sn_buf));
}

const void *lk2nd_device2nd_init(void)
{
	const void *dtb = lk2nd_device2nd_parse_tags();

	if (lk2nd_dev.cmdline) {
		dprintf(INFO, "Command line from previous bootloader: ");
		dputs(INFO, lk2nd_dev.cmdline);
		dputc(INFO, '\n');

		lk2nd_device2nd_parse_cmdline();
		lk2nd_device2nd_update_serialno(lk2nd_dev.serialno);
	}

	return dtb;
}

static void lk2nd_device2nd_fastboot_register(void)
{
	if (lk2nd_dev.bootloader)
		fastboot_publish("lk2nd:bootloader", lk2nd_dev.bootloader);
	if (lk2nd_dev.device)
		fastboot_publish("lk2nd:device", lk2nd_dev.device);
	if (lk2nd_dev.carrier)
		fastboot_publish("lk2nd:carrier", lk2nd_dev.carrier);
	if (lk2nd_dev.radio)
		fastboot_publish("lk2nd:radio", lk2nd_dev.radio);
}
FASTBOOT_INIT(lk2nd_device2nd_fastboot_register);
