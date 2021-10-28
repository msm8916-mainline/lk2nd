// SPDX-License-Identifier: GPL-2.0-only
#include <debug.h>
#include <libfdt.h>
#include <lk2nd.h>
#include <psci.h>
#include <scm.h>

#include "cpu-boot.h"

static uint8_t smp_spin_table_code[] = {
	0x5f, 0x20, 0x03, 0xd5,	/* wfe */
	0x7e, 0x00, 0x00, 0x58,	/* ldr	lr, 16 */
	0xde, 0xff, 0xff, 0xb4,	/* cbz	lr, 0 */
	0xc0, 0x03, 0x5f, 0xd6,	/* ret */
	0x00, 0x00, 0x00, 0x00,	/* Release address */
	0x00, 0x00, 0x00, 0x00,	/* (0 by default) */
};
#define SMP_SPIN_TABLE_RELEASE_ADDR	(SMP_SPIN_TABLE_BASE + \
					 sizeof(smp_spin_table_code) - \
					 sizeof(uint64_t))

static int fdt_lookup_phandle(void *fdt, int node, const char *prop_name)
{
	const uint32_t *phandle;
	int len;

	phandle = fdt_getprop(fdt, node, prop_name, &len);
	if (len < 0)
		return len;
	if (len != sizeof(uint32_t))
		return -FDT_ERR_BADVALUE;

	return fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*phandle));
}

static void smp_spin_table_setup_cpu(void *fdt, int cpu_node)
{
	const uint32_t *val;
	int node, len, ret;
	uint32_t cpu;

	val = fdt_getprop(fdt, cpu_node, "reg", &len);
	if (len != sizeof(*val)) {
		dprintf(CRITICAL, "Cannot read reg property of CPU node: %d\n", len);
		return;
	}
	cpu = fdt32_to_cpu(*val);
	dprintf(INFO, "Booting CPU%x\n", cpu);

	ret = fdt_setprop_u64(fdt, cpu_node, "cpu-release-addr", SMP_SPIN_TABLE_RELEASE_ADDR);
	if (ret) {
		dprintf(CRITICAL, "Failed to set cpu-release-addr: %d\n", ret);
		return;
	}

	ret = fdt_setprop_string(fdt, cpu_node, "enable-method", "spin-table");
	if (ret) {
		dprintf(CRITICAL, "Failed to update enable-method: %d\n", ret);
		return;
	}

	node = fdt_lookup_phandle(fdt, cpu_node, "qcom,acc");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find qcom,acc node: %d\n", node);
		return;
	}

	val = fdt_getprop(fdt, node, "reg", &len);
	if (len < sizeof(*val)) {
		dprintf(CRITICAL, "Cannot read reg property of qcom,acc node: %d\n", len);
		return;
	}

	qcom_power_up_arm_cortex(cpu, fdt32_to_cpu(*val));

	node = fdt_lookup_phandle(fdt, cpu_node, "qcom,saw");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find qcom,saw node: %d\n", node);
		return;
	}

	ret = fdt_setprop_string(fdt, node, "status", "okay");
	if (ret)
		dprintf(CRITICAL, "Failed to enable SAW/SPM node: %d\n", ret);
}

static void smp_spin_table_setup_idle_states(void *fdt, int node)
{
	int ret, state_node;

	ret = fdt_nop_property(fdt, node, "entry-method");
	if (ret)
		dprintf(CRITICAL, "Failed to NOP idle-states entry-method: %d\n", ret);

	fdt_for_each_subnode(state_node, fdt, node) {
		const char *name;
		int len;

		name = fdt_getprop(fdt, state_node, "idle-state-name", &len);
		if (len < 0 || name[len]) {
			dprintf(CRITICAL, "Failed to get idle-state-name: %d\n", len);
			continue;
		}

		if (strcmp(name, "standalone-power-collapse") == 0) {
			ret = fdt_setprop_string(fdt, state_node, "compatible", "qcom,idle-state-spc");
			if (ret)
				dprintf(CRITICAL, "Failed to set qcom,idle-state-spc compatible: %d\n", ret);
		}

	}
	if (state_node < 0 && state_node != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "Failed to read idle-states subnodes: %d\n", state_node);
		return;
	}
}

/*
 * If the other CPU cores are booted in aarch64 state before the main CPU
 * switches to aarch64, qhypstub has no way to detect that and will boot them
 * in EL1 instead of EL2 (assuming it was bypassed for the state switch).
 * To avoid that, force execution state to aarch64.
 */
extern void qhypstub_set_state_aarch64(void);

void smp_spin_table_setup(void *fdt)
{
	scmcall_arg arg = {PSCI_0_2_FN_PSCI_VERSION};
	uint32_t psci_version;
	int offset, node, ret;

	if (!is_scm_armv8_support()) {
		dprintf(INFO, "ARM64 not available, cannot use SMP spin table\n");
		return;
	}

	psci_version = scm_call2(&arg, NULL);
	if (psci_version != PSCI_RET_NOT_SUPPORTED) {
		dprintf(INFO, "PSCI v%d.%d detected, no need for SMP spin table\n",
			PSCI_VERSION_MAJOR(psci_version), PSCI_VERSION_MINOR(psci_version));
		return;
	}

	offset = fdt_path_offset(fdt, "/cpus");
	if (offset < 0) {
		dprintf(CRITICAL, "Cannot find /cpus node: %d\n", offset);
		return;
	}

	if (fdt_subnode_offset(fdt, offset, "cpu-map") >= 0) {
		dprintf(CRITICAL, "Multiple CPU clusters are not supported yet\n");
		return;
	}

	memcpy((void*)SMP_SPIN_TABLE_BASE, smp_spin_table_code, sizeof(smp_spin_table_code));

	ret = qcom_set_boot_addr(SMP_SPIN_TABLE_BASE);
	if (ret) {
		dprintf(CRITICAL, "Failed to set CPU boot address: %d\n", ret);
		return;
	}

#if TARGET_MSM8916
	qhypstub_set_state_aarch64();
#endif

	fdt_for_each_subnode(node, fdt, offset) {
		const char *name;
		int len;

		name = fdt_get_name(fdt, node, &len);
		if (len < strlen("cpu@") || name[len])
			continue;
		if (strncmp(name, "cpu@", strlen("cpu@")) == 0)
			smp_spin_table_setup_cpu(fdt, node);
		if (strcmp(name, "idle-states") == 0)
			smp_spin_table_setup_idle_states(fdt, node);
	}
	if (node < 0 && node != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "Failed to read /cpus subnodes: %d\n", node);
		return;
	}

	offset = fdt_path_offset(fdt, "/psci");
	if (offset < 0) {
		dprintf(CRITICAL, "Cannot find /psci node: %d\n", offset);
		return;
	}

	ret = fdt_setprop_string(fdt, offset, "status", "disabled");
	if (ret)
		dprintf(CRITICAL, "Failed to set psci to status = \"disabled\": %d\n", ret);
}
