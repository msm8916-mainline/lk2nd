#include <stdio.h>
#include <platform/efuse.h>
#include <pm8x41_regulator.h>
#include "fastboot.h"

static void cmd_oem_dump_speedbin(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	int bin, version;

	efuse_read_speed_bin(&bin, &version);

	snprintf(response, sizeof(response),
		 "Speed bin: %d, PVS version: %d (qcom,speed%d-bin-v%d)",
		 bin, version, bin, version);
	fastboot_info(response);
	fastboot_okay("");
}

static void cmd_oem_dump_regulators(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	struct spmi_regulator *vreg;
	for (vreg = target_get_regulators(); vreg->name; ++vreg) {
		snprintf(response, sizeof(response), "%s: enabled: %d, voltage: %d mV",
			 vreg->name, regulator_is_enabled(vreg),
			 regulator_get_voltage(vreg));
		fastboot_info(response);
	}
	fastboot_okay("");
}

void fastboot_lk2nd_register_commands_msm8916(void) {
	fastboot_register("oem dump-speedbin", cmd_oem_dump_speedbin);
	fastboot_register("oem dump-regulators", cmd_oem_dump_regulators);
}
