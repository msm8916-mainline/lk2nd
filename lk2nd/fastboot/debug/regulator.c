// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023, Stephan Gerhold <stephan@gerhold.net> */

#include <board.h>
#include <fastboot.h>
#include <printf.h>
#include <stdint.h>

#include <lk2nd/hw/regulator.h>
#include <lk2nd/util/minmax.h>

static const char * const regulator_state[] = {
	"unknown", "disabled", "enabled",
};

static const char * const regulator_modes[] = {
	"", ",   fast", ", normal", ",   idle"
};

static void dump_regulator(struct regulator_dev *rdev)
{
	char response[MAX_RSP_SIZE];

	snprintf(response, sizeof(response),
		 "%4s: %7d mV, %8s%s%s  (%s)",
		 rdev->name,
		 regulator_get_voltage(rdev),
		 regulator_state[clamp(regulator_is_enabled(rdev), -1, 1) + 1],
		 regulator_modes[regulator_get_mode(rdev)],
		 regulator_is_bypassed(rdev) == 1 ? ", bypassed" : "",
		 rdev->driver_type);
	fastboot_info(response);
}

static void dump_regulators(struct regulator_dev *rdev)
{
	for (; rdev; rdev = rdev->next)
		dump_regulator(rdev);
}

static void cmd_oem_debug_spmi_regulators(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	uint32_t target;
	int i;

	for (i = 0; i < MAX_PMIC_DEVICES; ++i) {
		target = board_pmic_target(i);
		if (!target)
			break;

		snprintf(response, sizeof(response), "Detected PMIC %#x", target);
		fastboot_info(response);
		dump_regulators(spmi_regulator_probe(target));
	}
	fastboot_okay("");
}
FASTBOOT_REGISTER("oem debug spmi-regulators", cmd_oem_debug_spmi_regulators);
