// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <fastboot.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

static inline const char *strpresuf(const char *str, const char *pre)
{
	size_t len = strlen(pre);
	return strncmp(pre, str, len) == 0 ? str + len : NULL;
}

static inline void parse_arg(const char *str, const char *pre, const char **out)
{
	const char *val = strpresuf(str, pre);

	if (!val)
		return;
	if (*out)
		dprintf(CRITICAL, "Duplicate parameter '%s' (old value: %s, new value: %s)\n",
			pre, *out, val);
	*out = strdup(val);
}

static bool parse_panel(struct lk2nd_panel *p, char *panel)
{
	char *intf, *ctrl_id, *name, *garbage;

	if (p->name)
		return true;

	/* Should start with 0: or 1: */
	if ((panel[0] != '0' && panel[0] != '1') || panel[1] != ':')
		return false;

	intf = &panel[2];
	ctrl_id = strchr(intf, ':');
	if (!ctrl_id)
		return false;
	*ctrl_id = '\0';

	name = strchr(++ctrl_id, ':');
	if (!name)
		return false;

	garbage = strchr(++name, ':');
	if (garbage)
		*garbage = '\0';

	if (!name[0] || strcmp(name, "none") == 0)
		return true;

	p->name = name;
	p->intf = intf;
	p->initialized = panel[0] == '1';
	return true;
}

void lk2nd_device2nd_parse_cmdline(void)
{
	char *arg, *saveptr;
	char *args = strdup(lk2nd_dev.cmdline);
	const char *panel = NULL;

	for (arg = strtok_r(args, " ", &saveptr); arg;
	     arg = strtok_r(NULL, " ", &saveptr)) {
		const char *suffix;

		if ((suffix = strpresuf(arg, "androidboot."))) {
			parse_arg(suffix, "bootloader=", &lk2nd_dev.bootloader);
			parse_arg(suffix, "serialno=", &lk2nd_dev.serialno);
			parse_arg(suffix, "device=", &lk2nd_dev.device);
			parse_arg(suffix, "carrier=", &lk2nd_dev.carrier);
			parse_arg(suffix, "radio=", &lk2nd_dev.radio);
		} else if ((suffix = strpresuf(arg, "lk2nd."))) {
			parse_arg(suffix, "compatible=", &lk2nd_dev.compatible);
		} else {
			parse_arg(arg, "mdss_mdp.panel=", &panel);
			parse_arg(arg, "mdss_mdp3.panel=", &panel);
		}
	}
	free(args);

	if (panel && !parse_panel(&lk2nd_dev.panel, (char *)panel))
		dprintf(CRITICAL, "Failed to parse panel parameter: %s\n", panel);
}

/* Fastboot */
static void cmd_oem_parsed_cmdline(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.cmdline, strlen(lk2nd_dev.cmdline));
}
static void lk2nd_device_parse_cmdline_register(void)
{
	if (lk2nd_dev.cmdline)
		fastboot_register("oem parsed-cmdline", cmd_oem_parsed_cmdline);
}
FASTBOOT_INIT(lk2nd_device_parse_cmdline_register);
