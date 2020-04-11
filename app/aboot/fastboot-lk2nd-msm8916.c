#include <reg.h>
#include <stdio.h>
#include <pm8x41_regulator.h>
#include "fastboot.h"

#define EFUSE1	0x0005c004
#define EFUSE	0x0005c00c

static void cmd_oem_dump_speedbin(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	uint32_t efuse1 = readl(EFUSE1);
	uint32_t efuse = readl(EFUSE);
	int bin, bin2, version;

	bin = (efuse >> 2) & 0x7;
	version = (efuse1 >> 18) & 0x3;
	bin2 = (efuse1 >> 23) & 0x3;

	snprintf(response, sizeof(response),
		 "Speed bin: %d (%d), PVS version: %d (qcom,speed%d-bin-v%d)",
		 bin, bin2, version, bin, version);
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
