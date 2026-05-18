// SPDX-License-Identifier: BSD-3-Clause

#include <boot.h>
#include <debug.h>
#include <libfdt.h>
#include <scm.h>
#include <string.h>
#include <sys/types.h>

#define RNG_SEED_BYTES 64

/**
 * lk2nd_rng_seed_dt_update() - write a random seed to /chosen/rng-seed
 *
 * scm_random() is used to generate the entropy in a best-effort manner.
 * failures are logged, but do not fail the boot
 */
static int lk2nd_rng_seed_dt_update(void *dtb, const char *cmdline,
				    enum boot_type boot_type)
{
	uintptr_t rngseed[RNG_SEED_BYTES / sizeof(uintptr_t)];
	unsigned int i;
	int offset, ret;

	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	for (i = 0; i < ARRAY_SIZE(rngseed); i++) {
		ret = scm_random(&rngseed[i], sizeof(rngseed[i]));
		if (ret) {
			dprintf(INFO, "rng-seed: scm_call for random failed: %d\n", ret);
			return 0;
		}
	}

	offset = fdt_path_offset(dtb, "/chosen");
	if (offset < 0) {
		dprintf(INFO, "rng-seed: couldn't find /chosen: %d\n", ret);
		return 0;
	}

	ret = fdt_setprop(dtb, offset, "rng-seed", rngseed, sizeof(rngseed));
	if (ret < 0)
		dprintf(INFO, "rng-seed: failed to update /chosen/rng-seed: %d\n", ret);

	return 0;
}
DEV_TREE_UPDATE(lk2nd_rng_seed_dt_update);
