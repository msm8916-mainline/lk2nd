// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2026 Barnabas Czeman */
#include <regulator.h>
#include <debug.h>

void regulator_enable(uint32_t __UNUSED enable) {
	dprintf(CRITICAL, "regulator_enable is not supported on this platform\n");
}
