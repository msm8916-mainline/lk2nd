/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_PSCI_H
#define LK2ND_UTIL_PSCI_H

#include <bits.h>
#include <scm.h>

enum {
	PSCI_F_PSCI_VERSION = 0x84000000,
};

#define PSCI_VERSION_MAJOR(val)	BITS_SHIFT(val, 31, 16)
#define PSCI_VERSION_MINOR(val)	BITS_SHIFT(val, 15, 0)

enum {
	PSCI_RET_SUCCESS = 0,
	PSCI_RET_NOT_SUPPORTED = -1,
};

static inline int32_t psci_version(void)
{
	scmcall_arg arg = { .x0 = PSCI_F_PSCI_VERSION };
	return scm_call2(&arg, NULL);
}

#endif /* LK2ND_UTIL_PSCI_H */
