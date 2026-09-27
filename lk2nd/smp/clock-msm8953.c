// SPDX-License-Identifier: BSD-3-Clause

#include <board.h>
#include <boot.h>
#include <clock.h>
#include <platform/iomap.h>
#include <reg.h>

#define MHZ				1000000

static int lk2nd_8953_clock_setup(void *dtb, const char *cmdline,
				  enum boot_type boot_type)
{

	/* Downstream sets this itself */
	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	// select clock for secondary cluster
	clk_get_set_enable("a53ssmux_c0", 800 * MHZ, 0);
	return 0;
}
DEV_TREE_UPDATE(lk2nd_8953_clock_setup);
